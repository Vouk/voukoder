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

STDMETHODIMP_(void) CVoukoder::SetConfig(Voukoder::CONFIG config)
{
	// Video
	exportInfo.video.id = config.video.encoder;
	exportInfo.video.enabled = !exportInfo.video.id.IsEmpty();
	exportInfo.video.options.Deserialize(config.video.options);
	exportInfo.video.filters.Deserialize(config.video.filters);
	exportInfo.video.sideData.Deserialize(config.video.sidedata);

	if (wcslen(config.video.format) == 0)
	{
		if (exportInfo.video.options.find("_pixelFormat") != exportInfo.video.options.end())
			exportInfo.video.pixelFormat = av_get_pix_fmt(exportInfo.video.options.at("_pixelFormat").c_str());
	}
	else
		exportInfo.video.pixelFormat = av_get_pix_fmt(wxString(config.video.format));

	// Audio
	exportInfo.audio.id = config.audio.encoder;
	exportInfo.audio.enabled = !exportInfo.audio.id.IsEmpty();
	exportInfo.audio.options.Deserialize(config.audio.options);
	exportInfo.audio.filters.Deserialize(config.audio.filters);
	exportInfo.audio.sideData.Deserialize(config.audio.sidedata);

	if (wcslen(config.audio.format) == 0)
	{
		if (exportInfo.audio.options.find("_sampleFormat") != exportInfo.audio.options.end())
			exportInfo.audio.sampleFormat = av_get_sample_fmt(exportInfo.audio.options.at("_sampleFormat").c_str());
	}
	else
		exportInfo.audio.sampleFormat = av_get_sample_fmt(wxString(config.audio.format));

	// Format
	exportInfo.format.id = config.format.container;
	exportInfo.format.faststart = config.format.faststart;

	// Multipass encoding
	if (exportInfo.video.options.find("_2pass") != exportInfo.video.options.end())
	{
		if (exportInfo.video.options.at("_2pass") == "1")
			exportInfo.passes = 2;
	}
	else
		exportInfo.passes = 1;
}

STDMETHODIMP_(void) CVoukoder::GetConfig(Voukoder::CONFIG* config)
{
	config->version = VOUKODER_CONFIG_VERSION;

	// Video
	wcscpy_s(config->video.encoder, exportInfo.video.id);
	wcscpy_s(config->video.options, exportInfo.video.options.Serialize(true));
	wcscpy_s(config->video.filters, exportInfo.video.filters.Serialize());
	wcscpy_s(config->video.sidedata, exportInfo.video.sideData.Serialize(true));
	const char* vformat = av_get_pix_fmt_name(exportInfo.video.pixelFormat);
	mbstowcs(config->video.format, vformat, strlen(vformat));

	// Audio
	wcscpy_s(config->audio.encoder, exportInfo.audio.id);
	wcscpy_s(config->audio.options, exportInfo.audio.options.Serialize(true));
	wcscpy_s(config->audio.filters, exportInfo.audio.filters.Serialize());
	wcscpy_s(config->audio.sidedata, exportInfo.audio.sideData.Serialize(true));
	const char* aformat = av_get_sample_fmt_name(exportInfo.audio.sampleFormat);
	mbstowcs(config->audio.format, aformat, strlen(aformat));
	
	// Format
	wcscpy_s(config->format.container, exportInfo.format.id);
	config->format.faststart = exportInfo.format.faststart;
}

STDMETHODIMP_(void) CVoukoder::GetAudioChunkSize(int* chunkSize)
{
	*chunkSize = encoder->getAudioFrameSize();
}

STDMETHODIMP_(bool) CVoukoder::GetFileExtension(std::wstring& extension)
{
	for (auto info : Voukoder::Config::Get().muxerInfos)
	{
		if (info.id == exportInfo.format.id)
		{
			extension = info.extension;
			return true;
		}
	}

	return false;
}

STDMETHODIMP_(int) CVoukoder::GetMaxPasses()
{
	return exportInfo.passes;
}

STDMETHODIMP_(bool) CVoukoder::Open(Voukoder::INFO info)
{
	// Replace file extension if necessary
	std::wstring fname(info.filename);
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
	exportInfo.application = info.application;
	exportInfo.video.width = info.video.width;
	exportInfo.video.height = info.video.height;
	exportInfo.video.timebase = { (int)info.video.timebase.num, (int)info.video.timebase.den };
	exportInfo.video.sampleAspectRatio = { (int)info.video.aspectratio.num, (int)info.video.aspectratio.den };

	// Video field order
	switch (info.video.fieldorder)
	{
	case Voukoder::FieldOrder::Top:
		exportInfo.video.fieldOrder = AV_FIELD_TT;
		break;
	case Voukoder::FieldOrder::Bottom:
		exportInfo.video.fieldOrder = AV_FIELD_BB;
		break;
	default:
		exportInfo.video.fieldOrder = AV_FIELD_PROGRESSIVE;
	}

	exportInfo.video.colorRange = info.video.colorRange == Voukoder::ColorRange::Full ? AVColorRange::AVCOL_RANGE_JPEG : AVColorRange::AVCOL_RANGE_MPEG;

	// Video color space
	switch (info.video.colorSpace)
	{
	case Voukoder::ColorSpace::bt709:
		exportInfo.video.colorSpace = AVColorSpace::AVCOL_SPC_BT709;
		exportInfo.video.colorPrimaries = AVColorPrimaries::AVCOL_PRI_BT709;
		exportInfo.video.colorTransferCharacteristics = AVColorTransferCharacteristic::AVCOL_TRC_BT709;
		break;
	case Voukoder::ColorSpace::bt601_PAL:
		exportInfo.video.colorSpace = AVColorSpace::AVCOL_SPC_BT470BG;
		exportInfo.video.colorPrimaries = AVColorPrimaries::AVCOL_PRI_BT470BG;
		exportInfo.video.colorTransferCharacteristics = AVColorTransferCharacteristic::AVCOL_TRC_GAMMA28;
	case Voukoder::ColorSpace::bt601_NTSC:
		exportInfo.video.colorSpace = AVColorSpace::AVCOL_SPC_SMPTE170M;
		exportInfo.video.colorPrimaries = AVColorPrimaries::AVCOL_PRI_SMPTE170M;
		exportInfo.video.colorTransferCharacteristics = AVColorTransferCharacteristic::AVCOL_TRC_SMPTE170M;
	case Voukoder::ColorSpace::bt2020_CL:
		exportInfo.video.colorSpace = AVColorSpace::AVCOL_SPC_BT2020_CL;
		exportInfo.video.colorPrimaries = AVColorPrimaries::AVCOL_PRI_BT2020;
		exportInfo.video.colorTransferCharacteristics = AVColorTransferCharacteristic::AVCOL_TRC_SMPTE2084;
	case Voukoder::ColorSpace::bt2020_NCL:
		exportInfo.video.colorSpace = AVColorSpace::AVCOL_SPC_BT2020_NCL;
		exportInfo.video.colorPrimaries = AVColorPrimaries::AVCOL_PRI_BT2020;
		exportInfo.video.colorTransferCharacteristics = AVColorTransferCharacteristic::AVCOL_TRC_SMPTE2084;
		break;
	}

	exportInfo.audio.timebase = { 1, info.audio.samplerate };

	// Audio channel layout
	switch (info.audio.channellayout)
	{
	case Voukoder::ChannelLayout::Mono:
		exportInfo.audio.channelLayout = AV_CH_LAYOUT_MONO;
		break;
	case Voukoder::ChannelLayout::Stereo:
		exportInfo.audio.channelLayout = AV_CH_LAYOUT_STEREO;
		break;
	case Voukoder::ChannelLayout::FivePointOne:
		if (exportInfo.audio.id == "dca")
			exportInfo.audio.channelLayout = AV_CH_LAYOUT_5POINT1; //Use 5.1(side) when dts audio is selected
		else
			exportInfo.audio.channelLayout = AV_CH_LAYOUT_5POINT1_BACK;
		break;
	}

	// Create encoder instance
	encoder = new EncoderEngine(exportInfo);
	if (encoder->open() < 0)
		return false;

	return true;
}

STDMETHODIMP_(void) CVoukoder::Close(bool finalize)
{
	if (encoder)
	{
		if (finalize)
			encoder->finalize();

		encoder->close();

		delete encoder;
	}
}

STDMETHODIMP_(void) CVoukoder::Log(std::wstring text, ...)
{
	wchar_t log[1024];

	va_list args;
	va_start(args, log);
	vswprintf_s(log, text.c_str(), args);
	va_end(args);

	vkLogInfo(log);
}

STDMETHODIMP_(bool) CVoukoder::IsAudioActive()
{
	return encoder->hasAudio();
}

STDMETHODIMP_(bool) CVoukoder::IsAudioWaiting()
{
	return encoder->hasAudio() &&
		(av_compare_ts(vPts, exportInfo.video.timebase, aPts, exportInfo.audio.timebase) > 0);
}

STDMETHODIMP_(bool) CVoukoder::IsVideoActive()
{
	return encoder->hasVideo();
}

STDMETHODIMP_(bool) CVoukoder::SendAudioSampleChunk(uint8_t** buffer, int samples, int blockSize, int planes, int sampleRate, Voukoder::ChannelLayout layout, const char* format)
{
	assert(planes <= AV_NUM_DATA_POINTERS);

	AVFrame* frame = av_frame_alloc();
	frame->format = av_get_sample_fmt(format);
	frame->sample_rate = sampleRate;
	frame->nb_samples = samples;

	switch (layout)
	{
	case Voukoder::ChannelLayout::Mono:
		frame->channel_layout = AV_CH_LAYOUT_MONO;
		break;
	case Voukoder::ChannelLayout::Stereo:
		frame->channel_layout = AV_CH_LAYOUT_STEREO;
		break;
	case Voukoder::ChannelLayout::FivePointOne:
		if (exportInfo.audio.id == "dca")
			frame->channel_layout = AV_CH_LAYOUT_5POINT1; //Use 5.1(side) when dts audio is selected
		else
			frame->channel_layout = AV_CH_LAYOUT_5POINT1_BACK;
		break;
	}

	frame->channels = av_get_channel_layout_nb_channels(frame->channel_layout);

	// Fill each plane
	for (int p = 0; p < planes; p++)
	{
		frame->data[p] = buffer[p];
		frame->linesize[p] = samples * blockSize;
	}

	frame->pts = aPts;

	aPts += frame->nb_samples;

	bool ret = encoder->writeAudioFrame(frame) == 0;

	av_frame_free(&frame);

	return ret;
}

STDMETHODIMP_(bool) CVoukoder::SendVideoFrame(int64_t idx, uint8_t** buffer, int* rowsize, int planes, int width, int height, int pass, const char* format)
{
	assert(planes <= AV_NUM_DATA_POINTERS);

	// Multipass
	if (pass > encoder->pass)
	{
		// Close current encoding pass
		encoder->finalize();
		encoder->close();

		// Set new pass
		encoder->pass = pass;

		// Start new pass
		if (encoder->open() < 0)
		{
			//gui->ReportMessage(wxString::Format("Unable to start pass #%d", encoder.pass));
			return false;
		}
	}

	// Fill frame
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

	bool ret = encoder->writeVideoFrame(frame) == 0;

	av_frame_free(&frame);

	return ret;
}

STDMETHODIMP_(bool) CVoukoder::ShowVoukoderDialog(HANDLE act_ctx, HINSTANCE instance)
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

	return result == (int)wxID_OK;
}