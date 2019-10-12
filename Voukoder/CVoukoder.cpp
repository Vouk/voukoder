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

struct handle_data {
	unsigned long process_id;
	HWND window_handle;
};

BOOL IsMainWindow(HWND handle)
{
	return GetWindow(handle, GW_OWNER) == (HWND)0 && IsWindowVisible(handle);
}

BOOL CALLBACK EnumWindowsCallback(HWND handle, LPARAM lParam)
{
	handle_data& data = *(handle_data*)lParam;
	unsigned long process_id = 0;
	GetWindowThreadProcessId(handle, &process_id);
	if (data.process_id != process_id || !IsMainWindow(handle))
		return TRUE;
	data.window_handle = handle;
	return FALSE;
}

HWND FindMainWindow(unsigned long process_id)
{
	handle_data data;
	data.process_id = process_id;
	data.window_handle = 0;
	EnumWindows(EnumWindowsCallback, (LPARAM)&data);
	return data.window_handle;
}

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

CVoukoder::CVoukoder()
{
#ifdef _DEBUG
	av_log_set_level(AV_LOG_TRACE);
	av_log_set_callback(AvCallback);
#endif

	// Add window title to log
	wchar_t text[256];
	HWND hwnd = FindMainWindow(GetCurrentProcessId());
	if (GetWindowText(hwnd, text, sizeof(text)))
	{
		vkLogInfo(wxString::Format("Plugin running in %s", text));
		vkLogInfo("---------------------------------------------");
	}
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

STDMETHODIMP CVoukoder::SetConfig(VKENCODERCONFIG& config)
{
	// Video
	exportInfo.video.id = config.video.encoder;
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

	// Refresh config (not so nice, but helps with format and stuff ..)
	GetConfig(&config);

	return S_OK;
}

STDMETHODIMP CVoukoder::GetConfig(VKENCODERCONFIG* config)
{
	config->version = VOUKODER_CONFIG_VERSION;

	// Video
	wcscpy_s(config->video.encoder, exportInfo.video.id);
	wcscpy_s(config->video.options, exportInfo.video.options.Serialize(true));
	wcscpy_s(config->video.filters, exportInfo.video.filters.Serialize());
	wcscpy_s(config->video.sidedata, exportInfo.video.sideData.Serialize(true));
	const char* vformat = av_get_pix_fmt_name(exportInfo.video.pixelFormat);
	if (vformat)
		mbstowcs(config->video.format, vformat, strlen(vformat) + 1);

	// Audio
	wcscpy_s(config->audio.encoder, exportInfo.audio.id);
	wcscpy_s(config->audio.options, exportInfo.audio.options.Serialize(true));
	wcscpy_s(config->audio.filters, exportInfo.audio.filters.Serialize());
	wcscpy_s(config->audio.sidedata, exportInfo.audio.sideData.Serialize(true));
	const char* aformat = av_get_sample_fmt_name(exportInfo.audio.sampleFormat);
	if (aformat)
		mbstowcs(config->audio.format, aformat, strlen(aformat) + 1);
	
	// Format
	wcscpy_s(config->format.container, exportInfo.format.id);
	config->format.faststart = exportInfo.format.faststart;

	return S_OK;
}

STDMETHODIMP CVoukoder::GetAudioChunkSize(UINT* chunkSize)
{
	*chunkSize = encoder->getAudioFrameSize();

	return S_OK;
}

STDMETHODIMP CVoukoder::GetFileExtension(BSTR* extension)
{
	for (auto info : Voukoder::Config::Get().muxerInfos)
	{
		if (info.id == exportInfo.format.id)
		{
			_bstr_t _bstr(info.extension.ToStdWstring().c_str());
			*extension = _bstr.copy();

			return S_OK;
		}
	}

	return E_FAIL;
}

STDMETHODIMP CVoukoder::GetMaxPasses(UINT* passes)
{
	*passes = exportInfo.passes;

	return S_OK;
}

STDMETHODIMP CVoukoder::Open(VKENCODERINFO info)
{
	vkLogSep();
	vkLogInfo("Export started");
	vkLogSep();

	// Replace file extension if necessary
	std::wstring fname(info.filename, SysStringLen(info.filename));
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
	exportInfo.application = wxString::Format("%s (%s)", VKDR_APPNAME, std::wstring(info.application, SysStringLen(info.application)));
	exportInfo.video.enabled = info.video.enabled;
	exportInfo.video.width = info.video.width;
	exportInfo.video.height = info.video.height;
	exportInfo.video.timebase = { info.video.timebase.num, info.video.timebase.den };
	exportInfo.video.sampleAspectRatio = { info.video.aspectratio.num, info.video.aspectratio.den };

	// Video field order
	switch (info.video.fieldorder)
	{
	case FieldOrder::Top:
		exportInfo.video.fieldOrder = AV_FIELD_TT;
		break;
	case FieldOrder::Bottom:
		exportInfo.video.fieldOrder = AV_FIELD_BB;
		break;
	default:
		exportInfo.video.fieldOrder = AV_FIELD_PROGRESSIVE;
	}

	exportInfo.video.colorRange = info.video.colorRange == ColorRange::Full ? AVColorRange::AVCOL_RANGE_JPEG : AVColorRange::AVCOL_RANGE_MPEG;

	// Video color space
	switch (info.video.colorSpace)
	{
	case ColorSpace::bt709:
		exportInfo.video.colorSpace = AVColorSpace::AVCOL_SPC_BT709;
		exportInfo.video.colorPrimaries = AVColorPrimaries::AVCOL_PRI_BT709;
		exportInfo.video.colorTransferCharacteristics = AVColorTransferCharacteristic::AVCOL_TRC_BT709;
		break;
	case ColorSpace::bt601_PAL:
		exportInfo.video.colorSpace = AVColorSpace::AVCOL_SPC_BT470BG;
		exportInfo.video.colorPrimaries = AVColorPrimaries::AVCOL_PRI_BT470BG;
		exportInfo.video.colorTransferCharacteristics = AVColorTransferCharacteristic::AVCOL_TRC_GAMMA28;
	case ColorSpace::bt601_NTSC:
		exportInfo.video.colorSpace = AVColorSpace::AVCOL_SPC_SMPTE170M;
		exportInfo.video.colorPrimaries = AVColorPrimaries::AVCOL_PRI_SMPTE170M;
		exportInfo.video.colorTransferCharacteristics = AVColorTransferCharacteristic::AVCOL_TRC_SMPTE170M;
	case ColorSpace::bt2020_CL:
		exportInfo.video.colorSpace = AVColorSpace::AVCOL_SPC_BT2020_CL;
		exportInfo.video.colorPrimaries = AVColorPrimaries::AVCOL_PRI_BT2020;
		exportInfo.video.colorTransferCharacteristics = AVColorTransferCharacteristic::AVCOL_TRC_SMPTE2084;
	case ColorSpace::bt2020_NCL:
		exportInfo.video.colorSpace = AVColorSpace::AVCOL_SPC_BT2020_NCL;
		exportInfo.video.colorPrimaries = AVColorPrimaries::AVCOL_PRI_BT2020;
		exportInfo.video.colorTransferCharacteristics = AVColorTransferCharacteristic::AVCOL_TRC_SMPTE2084;
		break;
	}

	exportInfo.audio.enabled = info.audio.enabled;
	exportInfo.audio.timebase = { 1, info.audio.samplerate };

	// Audio channel layout
	switch (info.audio.channellayout)
	{
	case ChannelLayout::Mono:
		exportInfo.audio.channelLayout = AV_CH_LAYOUT_MONO;
		break;
	case ChannelLayout::Stereo:
		exportInfo.audio.channelLayout = AV_CH_LAYOUT_STEREO;
		break;
	case ChannelLayout::FivePointOne:
		if (exportInfo.audio.id == "dca")
			exportInfo.audio.channelLayout = AV_CH_LAYOUT_5POINT1; //Use 5.1(side) when dts audio is selected
		else
			exportInfo.audio.channelLayout = AV_CH_LAYOUT_5POINT1_BACK;
		break;
	}

	// Create encoder instance
	encoder = new EncoderEngine(exportInfo);
	if (encoder->open() < 0)
	{
		vkLogInfo("Opening encoder failed! Aborting ...")
		return E_FAIL;
	}

	vkLogSep();
	
	BOOL isActive = false;

	// Log video
	IsVideoActive(&isActive);
	if (isActive)
	{
		vkLogInfoVA("Frame size:      %dx%d", info.video.width, info.video.height);
		vkLogInfoVA("Pixel aspect:    %d:%d", info.video.aspectratio.num, info.video.aspectratio.den);
		vkLogInfoVA("Frame rate:      %.2f", ((float)info.video.timebase.den / (float)info.video.timebase.num));

		// Log field order
		switch (info.video.fieldorder)
		{
		case FieldOrder::Progressive:
			vkLogInfo("Interlaced:      No");
			break;
		case FieldOrder::Bottom:
			vkLogInfo("Interlaced:      Bottom first");
			break;
		case FieldOrder::Top:
			vkLogInfo("Interlaced:      Top first");
			break;
		}

		// Color range
		switch (info.video.colorRange)
		{
		case ColorRange::Full:
			vkLogInfo("Color range:     Full");
			break;
		case ColorRange::Limited:
			vkLogInfo("Color range:     Limited");
			break;
		}

		// Color space
		switch (info.video.colorSpace)
		{
		case ColorSpace::bt601_NTSC:
			vkLogInfo("Color space:     bt.601 (NTSC)");
			break;
		case ColorSpace::bt601_PAL:
			vkLogInfo("Color space:     bt.601 (PAL)");
			break;
		case ColorSpace::bt709:
			vkLogInfo("Color space:     bt.709");
			break;
		case ColorSpace::bt2020_CL:
			vkLogInfo("Color space:     bt.2020 (CL)");
			break;
		case ColorSpace::bt2020_NCL:
			vkLogInfo("Color space:     bt.2020 (NCL)");
			break;
		}

		UINT passes = 0;
		GetMaxPasses(&passes);
		vkLogInfoVA("Passes:          %d", passes);
	}

	// Log audio
	IsAudioActive(&isActive);
	if (isActive)
	{
		vkLogInfoVA("Sample rate:     %d Hz", info.audio.samplerate);
		vkLogInfoVA("Audio channels:  %d", info.audio.numberChannels);
	}

	vkLogSep();

	return S_OK;
}

STDMETHODIMP CVoukoder::Close(BOOL finalize)
{
	if (encoder)
	{
		if (finalize)
			encoder->finalize();

		encoder->close();

		delete encoder;
	}

	vkLogSep();
	vkLogInfo("Export finished");
	vkLogSep();

	return S_OK;
}

STDMETHODIMP CVoukoder::Log(BSTR text)
{
	vkLogInfo(std::wstring(text, SysStringLen(text)));

	return S_OK;
}

STDMETHODIMP CVoukoder::IsAudioActive(BOOL* isActive)
{
	*isActive = encoder->hasAudio();

	return S_OK;
}

STDMETHODIMP CVoukoder::IsAudioWaiting(BOOL* isWaiting)
{
	*isWaiting = encoder->hasAudio() && encoder->getNextFrameType() == AVMEDIA_TYPE_AUDIO;

	return S_OK;
}

STDMETHODIMP CVoukoder::IsVideoActive(BOOL* isActive)
{
	*isActive = encoder->hasVideo();
	
	return S_OK;
}

STDMETHODIMP CVoukoder::SendAudioSampleChunk(VKAUDIOCHUNK chunk)
{
	assert(chunk.planes <= AV_NUM_DATA_POINTERS);

	AVFrame* frame = av_frame_alloc();
	frame->format = av_get_sample_fmt(chunk.format);
	frame->sample_rate = chunk.sampleRate;
	frame->nb_samples = chunk.samples;

	switch (chunk.layout)
	{
	case ChannelLayout::Mono:
		frame->channel_layout = AV_CH_LAYOUT_MONO;
		break;
	case ChannelLayout::Stereo:
		frame->channel_layout = AV_CH_LAYOUT_STEREO;
		break;
	case ChannelLayout::FivePointOne:
		if (exportInfo.audio.id == "dca")
			frame->channel_layout = AV_CH_LAYOUT_5POINT1; //Use 5.1(side) when dts audio is selected
		else
			frame->channel_layout = AV_CH_LAYOUT_5POINT1_BACK;
		break;
	}

	frame->channels = av_get_channel_layout_nb_channels(frame->channel_layout);

	// Fill each plane
	for (int p = 0; p < chunk.planes; p++)
	{
		frame->data[p] = chunk.buffer[p];
		frame->linesize[p] = chunk.samples * chunk.blockSize;
	}

	bool ret = encoder->writeAudioFrame(frame) == 0;

	av_frame_free(&frame);

	return ret ? S_OK : E_FAIL;
}

STDMETHODIMP CVoukoder::SendVideoFrame(VKVIDEOFRAME frame)
{
	assert(frame.planes <= AV_NUM_DATA_POINTERS);

	// Multipass
	if (frame.pass > encoder->pass)
	{
		// Close current encoding pass
		encoder->finalize();
		encoder->close();

		// Set new pass
		encoder->pass = frame.pass;

		// Start new pass
		if (encoder->open() < 0)
		{
			vkLogErrorVA("Unable to start pass #%d", encoder->pass);
			return false;
		}
	}

	// Fill frame
	AVFrame* f = av_frame_alloc();
	f->width = frame.width;
	f->height = frame.height;
	f->format = av_get_pix_fmt(frame.format);
	f->color_range = AVColorRange::AVCOL_RANGE_MPEG;
	f->colorspace = AVColorSpace::AVCOL_SPC_BT709;
	f->color_primaries = AVColorPrimaries::AVCOL_PRI_BT709;
	f->color_trc = AVColorTransferCharacteristic::AVCOL_TRC_BT709;

	// Fill each plane
	for (int i = 0; i < frame.planes; i++)
	{
		f->data[i] = frame.buffer[i];
		f->linesize[i] = frame.rowsize[i];
	}

	bool ret = encoder->writeVideoFrame(f) == 0;

	av_frame_free(&f);

	return ret ? S_OK : E_FAIL;
}

STDMETHODIMP CVoukoder::ShowVoukoderDialog(BOOL video, BOOL audio, BOOL* isOkay, HANDLE act_ctx, HINSTANCE instance)
{
	int result;

	wxBeginBusyCursor();

	exportInfo.video.enabled = video;
	exportInfo.audio.enabled = audio;

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
		wxEndBusyCursor();
		result = dialog.ShowModal();

		wxTopLevelWindows.DeleteObject(&parent);
		parent.SetHWND((WXHWND)NULL);
	}

	// Clean-up and return.
	wxEntryCleanup();
	
	*isOkay = result == (int)wxID_OK;

	return S_OK;
}