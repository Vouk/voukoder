#include "CVoukoder.h"

#ifdef _DEBUG
static inline void AvCallback(void*, int level, const char* szFmt, va_list varg)
{
	char logbuf[2000];
	vsnprintf(logbuf, sizeof(logbuf), szFmt, varg);
	logbuf[sizeof(logbuf) - 1] = '\0';

	OutputDebugStringA(logbuf);
}
#endif

CVoukoder::CVoukoder():
	aPts(0),
	vPts(0)
{
#ifdef _DEBUG
	av_log_set_level(AV_LOG_TRACE);
	av_log_set_callback(AvCallback);
#endif

	// Create a 64k audio buffer
	audioBuffer = new uint8_t *[2]; // Stereo only
	for (int p = 0; p < 2; p++)
		audioBuffer[p] = new uint8_t[65536];
	audioBufferPos = 0;
}

CVoukoder::~CVoukoder()
{
	if (aFrame)
		av_frame_free(&aFrame);

	if (vFrame)
		av_frame_free(&vFrame);
}

STDMETHODIMP CVoukoder::QueryInterface(REFIID riid, LPVOID *ppv)
{
	*ppv = NULL;
	if (IsEqualIID(riid, IID_IUnknown) || IsEqualIID(riid, __uuidof(IVoukoder)))
	{
		*ppv = (IVoukoder *)this;
		_AddRef();
		return S_OK;
	}
	return E_NOINTERFACE;
}

STDMETHODIMP CVoukoder::Open(VOUKODERINFO info)
{
	// General info
	exportInfo.filename = info.filename;
	exportInfo.application = info.application;
	exportInfo.passes = info.passes;

	// Video info
	exportInfo.video.enabled = (info.video.width > 0 && info.video.height > 0);
	exportInfo.video.width = info.video.width;
	exportInfo.video.height = info.video.height;
	exportInfo.video.timebase = { info.video.timebase.num, info.video.timebase.den };
	exportInfo.video.sampleAspectRatio = { info.video.aspectratio.num, info.video.aspectratio.den };

	switch (info.video.fieldorder)
	{
	case fieldorder::top:
		exportInfo.video.fieldOrder = AV_FIELD_TT;
		break;
	case fieldorder::bottom:
		exportInfo.video.fieldOrder = AV_FIELD_BB;
		break;
	default:
		exportInfo.video.fieldOrder = AV_FIELD_PROGRESSIVE;
	}

	exportInfo.video.pixelFormat = av_get_pix_fmt(info.video.pixelformat);
	exportInfo.video.id = info.video.encoder;

	// Audio info
	exportInfo.audio.enabled = info.audio.samplerate > 0;
	exportInfo.audio.timebase = { 1, info.audio.samplerate };
	exportInfo.audio.sampleFormat = av_get_sample_fmt(info.audio.sampleformat);
	exportInfo.audio.channelLayout = av_get_channel_layout(info.audio.channellayout);
	exportInfo.audio.id = info.audio.encoder;

	// Format info
	exportInfo.format.id = info.format.container;
	exportInfo.format.faststart = info.format.faststart;

	// TODO
	exportInfo.video.options.Deserialize("");
	exportInfo.video.filters.Deserialize("");
	exportInfo.video.sideData.Deserialize("");
	exportInfo.video.colorRange = AVCOL_RANGE_JPEG;
	exportInfo.video.colorSpace = AVCOL_SPC_BT709;
	exportInfo.video.colorPrimaries = AVCOL_PRI_BT709;
	exportInfo.video.colorTransferCharacteristics = AVCOL_TRC_BT709;
	exportInfo.audio.options.Deserialize("");
	exportInfo.audio.filters.Deserialize("");
	exportInfo.audio.sideData.Deserialize("");
	
	// Create encoder instance
	encoder = new EncoderEngine(exportInfo);
	if (encoder->open() < 0)
	{
		return -1;
	}

	return S_OK;
}

STDMETHODIMP CVoukoder::Close(bool finalize)
{
	if (finalize)
		encoder->finalize();

	encoder->close();

	return S_OK;
}

STDMETHODIMP CVoukoder::IsAudioWaiting()
{
	return encoder->hasAudio() &&
		(av_compare_ts(vPts, exportInfo.video.timebase, aPts, exportInfo.audio.timebase) > 0) ? S_OK : S_FALSE;
}

STDMETHODIMP CVoukoder::SendAudioSamples(uint8_t** buffer, int samples, int blockSize, int planes, int sampleRate, const char* layout, const char* format)
{
	int chunkSize = encoder->getAudioFrameSize();

	if (aFrame == NULL)
	{
		aFrame = av_frame_alloc();
		aFrame->format = av_get_sample_fmt(format);
		aFrame->channel_layout = av_get_channel_layout(layout);
		aFrame->sample_rate = sampleRate;
		aFrame->nb_samples = chunkSize;

		// Reserve buffer
		if (av_frame_get_buffer(aFrame, av_cpu_max_align()) < 0)
		{
			vkLogError("Can not reserve audio buffer!");
			return S_FALSE;
		}
	}

	// Add samples to buffer
	for (int p = 0; p < planes; p++)
		memcpy(audioBuffer[p] + audioBufferPos, buffer[p], samples * blockSize);

	audioBufferPos += samples * blockSize;

	while (audioBufferPos >= encoder->getAudioFrameSize())
	{
		// Fill frame buffer
		for (int p = 0; p < planes; p++)
			memcpy(aFrame->data[p], audioBuffer[p], aFrame->nb_samples * blockSize);
		
		audioBufferPos -= aFrame->nb_samples * blockSize;

		aFrame->pts = aPts;

		aPts += aFrame->nb_samples;

		int hr = encoder->writeAudioFrame(aFrame) == 0;
	}

	return S_OK;
}

STDMETHODIMP CVoukoder::SendVideoFrame(int64_t idx, uint8_t** buffer, int* rowsize, int planes, int width, int height, const char* format)
{
	if (vFrame == NULL)
	{
		vFrame = av_frame_alloc();
		vFrame->width = width;
		vFrame->height = height;
		vFrame->format = av_get_pix_fmt(format);
		vFrame->color_range = AVColorRange::AVCOL_RANGE_MPEG;
		vFrame->colorspace = AVColorSpace::AVCOL_SPC_BT709;
		vFrame->color_primaries = AVColorPrimaries::AVCOL_PRI_BT709;
		vFrame->color_trc = AVColorTransferCharacteristic::AVCOL_TRC_BT709;
	}

	// Fill each plane
	for (int i = 0; i < FFMIN(planes, AV_NUM_DATA_POINTERS); i++)
	{
		vFrame->data[i] = buffer[i];
		vFrame->linesize[i] = rowsize[i];
	}

	vFrame->pts = vPts = idx;

	return encoder->writeVideoFrame(vFrame);
}