#include "CVoukoder.h"
#include "../Core/wxVoukoderDialog.h"

#ifdef _DEBUG
static inline void AvCallback(void*, int level, const char* szFmt, va_list varg)
{
	char logbuf[2000];
	vsnprintf(logbuf, sizeof(logbuf), szFmt, varg);
	logbuf[sizeof(logbuf) - 1] = '\0';

	OutputDebugStringA(logbuf);
}
#endif

class actctx_activator
{
protected:
	ULONG_PTR m_cookie;

public:
	actctx_activator(_In_ HANDLE hActCtx)
	{
		if (!ActivateActCtx(hActCtx, &m_cookie))
			m_cookie = 0;
	}

	virtual ~actctx_activator()
	{
		if (m_cookie)
			DeactivateActCtx(0, m_cookie);
	}
};

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

STDMETHODIMP CVoukoder::SetConfig(VOUKODER_CONFIG config)
{
	// Video
	exportInfo.video.id = config.video.encoder;
	exportInfo.video.enabled = !exportInfo.video.id.IsEmpty();
	exportInfo.video.options.Deserialize(config.video.options);
	exportInfo.video.filters.Deserialize(config.video.filters);
	exportInfo.video.sideData.Deserialize(config.video.sidedata);
	exportInfo.video.pixelFormat = av_get_pix_fmt(config.video.format);

	// Audio
	exportInfo.audio.id = config.audio.encoder;
	exportInfo.audio.enabled = !exportInfo.audio.id.IsEmpty();
	exportInfo.audio.options.Deserialize(config.audio.options);
	exportInfo.audio.filters.Deserialize(config.audio.filters);
	exportInfo.audio.sideData.Deserialize(config.audio.sidedata);
	exportInfo.audio.sampleFormat = av_get_sample_fmt(config.audio.format);

	// Format
	exportInfo.format.id = config.format.container;
	exportInfo.format.faststart = config.format.faststart;

	return S_OK;
}

STDMETHODIMP CVoukoder::GetConfig(VOUKODER_CONFIG* config)
{
	config->version = VOUKODER_CONFIG_VERSION;

	// Video
	strcpy_s(config->video.encoder, exportInfo.video.id);
	strcpy_s(config->video.options, exportInfo.video.options.Serialize(true));
	strcpy_s(config->video.filters, exportInfo.video.filters.Serialize());
	strcpy_s(config->video.sidedata, exportInfo.video.sideData.Serialize(true));
	strcpy_s(config->video.format, av_get_pix_fmt_name(exportInfo.video.pixelFormat));

	// Audio
	strcpy_s(config->audio.encoder, exportInfo.audio.id);
	strcpy_s(config->audio.options, exportInfo.audio.options.Serialize(true));
	strcpy_s(config->audio.filters, exportInfo.audio.filters.Serialize());
	strcpy_s(config->audio.sidedata, exportInfo.audio.sideData.Serialize(true));
	strcpy_s(config->audio.format, av_get_sample_fmt_name(exportInfo.audio.sampleFormat));
	
	// Format
	strcpy_s(config->format.container, exportInfo.format.id);
	config->format.faststart = exportInfo.format.faststart;

	return S_OK;
}

STDMETHODIMP CVoukoder::Open(const wchar_t* filename, const wchar_t* application, const int passes, const int width, const int height, const rational timebase, const rational aspectratio, const fieldorder fieldorder, const int samplerate, const char* channellayout)
{
	// Set non-stored export settings
	exportInfo.filename = filename;
	exportInfo.application = application;
	exportInfo.passes = passes;
	exportInfo.video.width = width;
	exportInfo.video.height = height;
	exportInfo.video.timebase = { (int)timebase.num, (int)timebase.den };
	exportInfo.video.sampleAspectRatio = { (int)aspectratio.num, (int)aspectratio.den };

	switch (fieldorder)
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

	exportInfo.video.colorRange = AVColorRange::AVCOL_RANGE_MPEG; // TODO
	exportInfo.video.colorSpace = AVColorSpace::AVCOL_SPC_BT709; // TODO
	exportInfo.video.colorPrimaries = AVColorPrimaries::AVCOL_PRI_BT709; // TODO
	exportInfo.video.colorTransferCharacteristics = AVColorTransferCharacteristic::AVCOL_TRC_BT709; // TODO
	exportInfo.audio.timebase = { 1, samplerate };
	exportInfo.audio.channelLayout = av_get_channel_layout(channellayout);

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

STDMETHODIMP CVoukoder::ShowVoukoderDialog(HANDLE act_ctx, HINSTANCE instance)
{
	int result;

	// Restore plugin's activation context.
	if (act_ctx)
		actctx_activator actctx(act_ctx);

	// Initialize application.
	new wxApp();
	if (instance)
		wxEntryStart(instance);

	// Have an own scope
	{
		// Create wxWidget-approved parent window.
		wxWindow parent;
		parent.SetHWND((WXHWND)GetActiveWindow());
		parent.AdoptAttributesFromHWND();
		wxTopLevelWindows.Append(&parent);

		wxSetlocale(LC_ALL, "C");

		// Create and launch configuration dialog.
		wxVoukoderDialog dialog(&parent, exportInfo);
		result = dialog.ShowModal();

		wxTopLevelWindows.DeleteObject(&parent);
		parent.SetHWND((WXHWND)NULL);
	}

	// Clean-up and return.
	wxEntryCleanup();

	return result == (int)wxID_OK ? S_OK : E_ABORT;
}