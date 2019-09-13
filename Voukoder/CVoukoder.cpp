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
	aPts(0), vPts(0)
{
#ifdef _DEBUG
	av_log_set_level(AV_LOG_TRACE);
	av_log_set_callback(AvCallback);
#endif
}

CVoukoder::~CVoukoder()
{}

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

STDMETHODIMP CVoukoder::GetAudioChunkSize(int* chunkSize)
{
	*chunkSize = encoder->getAudioFrameSize();
	return S_OK;
}


STDMETHODIMP CVoukoder::Open(const wchar_t* filename, const wchar_t* application, const int passes, const int width, const int height, const rational timebase, const rational aspectratio, const fieldorder fieldorder, const int samplerate, const char* channellayout)
{
	// Replace file extension if necessary
	std::wstring fname(filename);
	std::wstring::size_type i = fname.rfind('.', fname.length());
	for (auto info : Voukoder::Config::Get().muxerInfos)
	{
		if (info.id == exportInfo.format.id)
		{
			if (i != std::wstring::npos)
			{
				fname.replace(i + 1, info.extension.length(), info.extension);
				break;
			}
		}
	}

	// Set non-stored export settings
	exportInfo.filename = fname;
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
		return S_FALSE;

	return S_OK;
}

STDMETHODIMP CVoukoder::Close(bool finalize)
{
	if (encoder)
	{
		if (finalize)
			encoder->finalize();

		encoder->close();

		delete encoder;
	}

	return S_OK;
}

STDMETHODIMP CVoukoder::Log(const wchar_t* text, LogLevel level)
{
	switch (level)
	{
	case LogLevel::Error:
		vkLogError(text);
		break;
	case LogLevel::Warn:
		vkLogWarn(text);
		break;
	default:
		vkLogInfo(text);
	}

	return S_OK;
}

STDMETHODIMP CVoukoder::IsAudioWaiting()
{
	return encoder->hasAudio() &&
		(av_compare_ts(vPts, exportInfo.video.timebase, aPts, exportInfo.audio.timebase) > 0) ? S_OK : S_FALSE;
}

STDMETHODIMP CVoukoder::SendAudioSamples(uint8_t** buffer, int samples, int blockSize, int planes, int sampleRate, const char* layout, const char* format)
{
	assert(planes <= AV_NUM_DATA_POINTERS);

	AVFrame* frame = av_frame_alloc();
	frame->format = av_get_sample_fmt(format);
	frame->channel_layout = av_get_channel_layout(layout);
	frame->channels = av_get_channel_layout_nb_channels(frame->channel_layout);
	frame->sample_rate = sampleRate;
	frame->nb_samples = samples;

	// Fill each plane
	for (int p = 0; p < planes; p++)
	{
		frame->data[p] = buffer[p];
		frame->linesize[p] = samples * blockSize;
	}

	frame->pts = aPts;

	aPts += frame->nb_samples;

	HRESULT hr = encoder->writeAudioFrame(frame) == 0 ? S_OK : S_FALSE;

	av_frame_free(&frame);

	return hr;
}

STDMETHODIMP CVoukoder::SendVideoFrame(int64_t idx, uint8_t** buffer, int* rowsize, int planes, int width, int height, const char* format)
{
	assert(planes <= AV_NUM_DATA_POINTERS);

	AVFrame* frame = av_frame_alloc();
	frame->width = width;
	frame->height = height;
	frame->format = av_get_pix_fmt(format);
	frame->color_range = AVColorRange::AVCOL_RANGE_MPEG;
	frame->colorspace = AVColorSpace::AVCOL_SPC_BT709;
	frame->color_primaries = AVColorPrimaries::AVCOL_PRI_BT709;
	frame->color_trc = AVColorTransferCharacteristic::AVCOL_TRC_BT709;

	// Fill each plane
	for (int i = 0; i < planes; i++)
	{
		frame->data[i] = buffer[i];
		frame->linesize[i] = rowsize[i];
	}

	frame->pts = vPts = idx;

	HRESULT hr = encoder->writeVideoFrame(frame) == 0 ? S_OK : S_FALSE;

	av_frame_free(&frame);

	return hr;
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