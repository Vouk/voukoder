#include "CVoukoder.h"
#include "../Core/wxVoukoderDialog.h"

static inline void AvCallback(void*, int level, const char* szFmt, va_list varg)
{
	char logbuf[2000];
	vsnprintf(logbuf, sizeof(logbuf), szFmt, varg);
	logbuf[sizeof(logbuf) - 1] = '\0';

	OutputDebugStringA(logbuf);
}

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
	av_log_set_level(AV_LOG_DEBUG);
	av_log_set_callback(AvCallback);

	// Add window title to log
	wchar_t text[256];
	HWND hwnd = FindMainWindow(GetCurrentProcessId());
	if (GetWindowText(hwnd, text, sizeof(text)))
	{
		vkLogInfo(wxString::Format("Plugin running in %s", text));
		vkLogSep();
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

STDMETHODIMP CVoukoder::SetConfig(VKENCODERCONFIG config)
{
	// Standard fields
	exportInfo.video.id = config.video.encoder;
	exportInfo.video.options.Deserialize(config.video.options);
	exportInfo.video.filters.Deserialize(config.video.filters);
	exportInfo.video.sideData.Deserialize(config.video.sidedata);
	exportInfo.audio.id = config.audio.encoder;
	exportInfo.audio.options.Deserialize(config.audio.options);
	exportInfo.audio.filters.Deserialize(config.audio.filters);
	exportInfo.audio.sideData.Deserialize(config.audio.sidedata);
	exportInfo.format.id = config.format.container;
	exportInfo.format.faststart = config.format.faststart;
	exportInfo.video.fieldOrder = AVFieldOrder::AV_FIELD_PROGRESSIVE;
	exportInfo.video.colorRange = AVColorRange::AVCOL_RANGE_UNSPECIFIED;
	exportInfo.video.colorSpace = AVColorSpace::AVCOL_SPC_UNSPECIFIED;
	exportInfo.video.colorPrimaries = AVColorPrimaries::AVCOL_PRI_UNSPECIFIED;
	exportInfo.video.colorTransferCharacteristics = AVColorTransferCharacteristic::AVCOL_TRC_UNSPECIFIED;

	// Deal with color spaces
	for(const auto & options: exportInfo.video.filters)
	{
		if (options->id == "filter.setparams")
		{
			// Field mode
			if (options->find("field_mode") != options->end())
			{
				auto field_mode = options->at("field_mode");
				if (field_mode == "prog")
					exportInfo.video.fieldOrder = AVFieldOrder::AV_FIELD_PROGRESSIVE;
				else if (field_mode == "bff")
					exportInfo.video.fieldOrder = AVFieldOrder::AV_FIELD_BB;
				else if (field_mode == "tff")
					exportInfo.video.fieldOrder = AVFieldOrder::AV_FIELD_TT;
				else
					vkLogInfoVA("Unknown field mode supplied: %s", field_mode.c_str());
			}

			// Color range
			if (options->find("range") != options->end())
			{
				auto range = options->at("range");
				exportInfo.video.colorRange = (AVColorRange)av_color_range_from_name(range.c_str());
				if (exportInfo.video.colorRange == AVColorRange::AVCOL_RANGE_UNSPECIFIED)
					vkLogInfoVA("Unknown color range supplied: %s", range.c_str());
			}

			// Color space
			if (options->find("colorspace") != options->end())
			{
				auto colorspace = options->at("colorspace");
				exportInfo.video.colorSpace = (AVColorSpace)av_color_space_from_name(colorspace.c_str());
				if (exportInfo.video.colorSpace == AVColorSpace::AVCOL_SPC_UNSPECIFIED)
					vkLogInfoVA("Unknown color space supplied: %s", colorspace.c_str());
			}

			// Color primaries
			if (options->find("color_primaries") != options->end())
			{
				auto color_primaries = options->at("color_primaries");
				exportInfo.video.colorPrimaries = (AVColorPrimaries)av_color_primaries_from_name(color_primaries.c_str());
				if (exportInfo.video.colorPrimaries == AVColorPrimaries::AVCOL_PRI_UNSPECIFIED)
					vkLogInfoVA("Unknown color primaries supplied: %s", color_primaries.c_str());
			}

			// Color transfer
			if (options->find("color_trc") != options->end())
			{
				auto color_trc = options->at("color_trc");
				exportInfo.video.colorTransferCharacteristics = (AVColorTransferCharacteristic)av_color_transfer_from_name(color_trc.c_str());
				if (exportInfo.video.colorTransferCharacteristics == AVColorTransferCharacteristic::AVCOL_TRC_UNSPECIFIED)
					vkLogInfoVA("Unknown color transfer supplied: %s", color_trc.c_str());
			}
		}
		else if (options->id == "filter.colorspace" &&
			options->find("range") != options->end() &&
			options->find("space") != options->end() &&
			options->find("primaries") != options->end() &&
			options->find("trc") != options->end())
		{
			// Color range
			auto range = options->at("range");
			exportInfo.video.colorRange = (AVColorRange)av_color_range_from_name(range.c_str());
			if (exportInfo.video.colorRange == AVColorRange::AVCOL_RANGE_UNSPECIFIED)
				vkLogInfoVA("Unknown color range supplied: %s", range.c_str());

			// Color space
			auto space = options->at("space");
			if (space == "bt709")
				exportInfo.video.colorSpace = AVColorSpace::AVCOL_SPC_BT709;
			else if (space == "fcc")
				exportInfo.video.colorSpace = AVColorSpace::AVCOL_SPC_FCC;
			else if (space == "bt470bg")
				exportInfo.video.colorSpace = AVColorSpace::AVCOL_SPC_BT470BG;
			else if (space == "smpte170m")
				exportInfo.video.colorSpace = AVColorSpace::AVCOL_SPC_SMPTE170M;
			else if (space == "smpte240m")
				exportInfo.video.colorSpace = AVColorSpace::AVCOL_SPC_SMPTE240M;
			else if (space == "ycgco")
				exportInfo.video.colorSpace = AVColorSpace::AVCOL_SPC_YCGCO;
			else if (space == "gbr")
				exportInfo.video.colorSpace = AVColorSpace::AVCOL_SPC_RGB;
			else if (space == "bt2020nc")
				exportInfo.video.colorSpace = AVColorSpace::AVCOL_SPC_BT2020_NCL;
			else if (space == "bt2020ncl")
				exportInfo.video.colorSpace = AVColorSpace::AVCOL_SPC_BT2020_NCL;
			else
				vkLogInfoVA("Unknown color space supplied: %s", space.c_str());

			// Color primaries
			auto primaries = options->at("primaries");
			if (primaries == "bt709")
				exportInfo.video.colorPrimaries = AVColorPrimaries::AVCOL_PRI_BT709;
			else if (primaries == "bt470m")
				exportInfo.video.colorPrimaries = AVColorPrimaries::AVCOL_PRI_BT470M;
			else if (primaries == "bt470bg")
				exportInfo.video.colorPrimaries = AVColorPrimaries::AVCOL_PRI_BT470BG;
			else if (primaries == "smpte170m")
				exportInfo.video.colorPrimaries = AVColorPrimaries::AVCOL_PRI_SMPTE170M;
			else if (primaries == "smpte240m")
				exportInfo.video.colorPrimaries = AVColorPrimaries::AVCOL_PRI_SMPTE240M;
			else if (primaries == "smpte428")
				exportInfo.video.colorPrimaries = AVColorPrimaries::AVCOL_PRI_SMPTE428;
			else if (primaries == "film")
				exportInfo.video.colorPrimaries = AVColorPrimaries::AVCOL_PRI_FILM;
			else if (primaries == "smpte431")
				exportInfo.video.colorPrimaries = AVColorPrimaries::AVCOL_PRI_SMPTE431;
			else if (primaries == "smpte432")
				exportInfo.video.colorPrimaries = AVColorPrimaries::AVCOL_PRI_SMPTE432;
			else if (primaries == "bt2020")
				exportInfo.video.colorPrimaries = AVColorPrimaries::AVCOL_PRI_BT2020;
			else if (primaries == "jedec-p22")
				exportInfo.video.colorPrimaries = AVColorPrimaries::AVCOL_PRI_JEDEC_P22;
			else
				vkLogInfoVA("Unknown color primaries supplied: %s", primaries.c_str());

			// Color transfer
			auto trc = options->at("trc");
			if (trc == "bt709")
				exportInfo.video.colorTransferCharacteristics = AVColorTransferCharacteristic::AVCOL_TRC_BT709;
			else if (trc == "bt470m")
				exportInfo.video.colorTransferCharacteristics = AVColorTransferCharacteristic::AVCOL_TRC_GAMMA22;
			else if (trc == "bt470bg")
				exportInfo.video.colorTransferCharacteristics = AVColorTransferCharacteristic::AVCOL_TRC_GAMMA28;
			else if (trc == "gamma22")
				exportInfo.video.colorTransferCharacteristics = AVColorTransferCharacteristic::AVCOL_TRC_GAMMA22;
			else if (trc == "gamma28")
				exportInfo.video.colorTransferCharacteristics = AVColorTransferCharacteristic::AVCOL_TRC_GAMMA28;
			else if (trc == "smpte170m")
				exportInfo.video.colorTransferCharacteristics = AVColorTransferCharacteristic::AVCOL_TRC_SMPTE170M;
			else if (trc == "smpte240m")
				exportInfo.video.colorTransferCharacteristics = AVColorTransferCharacteristic::AVCOL_TRC_SMPTE240M;
			else if (trc == "srgb")
				exportInfo.video.colorTransferCharacteristics = AVColorTransferCharacteristic::AVCOL_TRC_IEC61966_2_1;
			else if (trc == "iec61966-2-1")
				exportInfo.video.colorTransferCharacteristics = AVColorTransferCharacteristic::AVCOL_TRC_IEC61966_2_1;
			else if (trc == "iec61966-2-4")
				exportInfo.video.colorTransferCharacteristics = AVColorTransferCharacteristic::AVCOL_TRC_IEC61966_2_4;
			else if (trc == "xvycc")
				exportInfo.video.colorTransferCharacteristics = AVColorTransferCharacteristic::AVCOL_TRC_IEC61966_2_4;
			else if (trc == "bt2020-10")
				exportInfo.video.colorTransferCharacteristics = AVColorTransferCharacteristic::AVCOL_TRC_BT2020_10;
			else if (trc == "bt2020-12")
				exportInfo.video.colorTransferCharacteristics = AVColorTransferCharacteristic::AVCOL_TRC_BT2020_12;
			else
				vkLogInfoVA("Unknown color transfer supplied: %s", trc.c_str());
		}
	}

	return S_OK;
}

STDMETHODIMP CVoukoder::GetConfig(VKENCODERCONFIG* config)
{
	strcpy_s(config->video.encoder, exportInfo.video.id.mb_str());
	strcpy_s(config->video.options, exportInfo.video.options.Serialize(true).mb_str());
	strcpy_s(config->video.filters, exportInfo.video.filters.Serialize().mb_str());
	strcpy_s(config->video.sidedata, exportInfo.video.sideData.Serialize(true).mb_str());
	strcpy_s(config->audio.encoder, exportInfo.audio.id.mb_str());
	strcpy_s(config->audio.options, exportInfo.audio.options.Serialize(true).mb_str());
	strcpy_s(config->audio.filters, exportInfo.audio.filters.Serialize().mb_str());
	strcpy_s(config->audio.sidedata, exportInfo.audio.sideData.Serialize(true).mb_str());
	strcpy_s(config->format.container, exportInfo.format.id.mb_str());
	config->format.faststart = exportInfo.format.faststart;
	config->version = 1;

	return S_OK;
}

STDMETHODIMP CVoukoder::GetFileExtension(BSTR* extension)
{
	for (auto info : Voukoder::Config::Get().muxerInfos)
	{
		if (info.id == exportInfo.format.id)
		{
			*extension = SysAllocString(info.extension);

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

	// Replace file extension if necessary (req'd for vegas)
	wxString filename(info.filename);
	for (auto info : Voukoder::Config::Get().muxerInfos)
	{
		if (info.id == exportInfo.format.id)
		{
			filename = filename.BeforeLast('.') + "." + info.extension;
			break;
		}
	}

	// Set non-stored export settings
	exportInfo.filename = filename;
	exportInfo.application = wxString::Format("%s (%s)", VKDR_APPNAME, std::wstring(info.application));
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

	// Multipass encoding
	if (exportInfo.video.enabled && exportInfo.video.options.find("_2pass") != exportInfo.video.options.end())
	{
		if (exportInfo.video.options.at("_2pass") == "1")
			exportInfo.passes = 2;
	}
	else
		exportInfo.passes = 1;

	// Dump
	vkLogInfoVA("Filename:        %s", exportInfo.filename);
	vkLogInfoVA("Application:     %s", exportInfo.application);
	vkLogInfoVA("Passes:          %d", exportInfo.passes);

	// Video
	if (exportInfo.video.enabled)
	{
		vkLogInfo("- Video -------------------------------------");
		vkLogInfoVA("Frame size:      %dx%d", exportInfo.video.width, exportInfo.video.height);
		vkLogInfoVA("Pixel aspect:    %d:%d", exportInfo.video.sampleAspectRatio.num, exportInfo.video.sampleAspectRatio.den);
		vkLogInfoVA("Timebase:        %d/%d (%.2f fps)", exportInfo.video.timebase.num, exportInfo.video.timebase.den, ((float)exportInfo.video.timebase.den / (float)exportInfo.video.timebase.num));

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

		vkLogInfoVA("Encoder:         %s", exportInfo.video.id);
		vkLogInfoVA("Options:         %s", exportInfo.video.options.Serialize(true, "", ' '));
		vkLogInfoVA("Side data:       %s", exportInfo.video.sideData.Serialize(true, "", ' '));
		vkLogInfoVA("Filters:         %s", exportInfo.video.filters.Serialize());
		vkLogInfoVA("Color range:     %s", av_color_range_name(exportInfo.video.colorRange));
		vkLogInfoVA("Color space:     %s", av_color_space_name(exportInfo.video.colorSpace));
		vkLogInfoVA("Color primaries: %s", av_color_primaries_name(exportInfo.video.colorPrimaries));
		vkLogInfoVA("Color transfer:  %s", av_color_transfer_name(exportInfo.video.colorTransferCharacteristics));
	}

	// Audio
	if (exportInfo.audio.enabled)
	{
		vkLogInfo("- Audio -------------------------------------");
		vkLogInfoVA("Timebase:        %d/%d", exportInfo.audio.timebase.num, exportInfo.audio.timebase.den);
		vkLogInfoVA("Channels:        %d", av_get_channel_layout_nb_channels(exportInfo.audio.channelLayout));
		vkLogInfoVA("Encoder:         %s", exportInfo.audio.id);
		vkLogInfoVA("Options:         %s", exportInfo.audio.options.Serialize(true, "", ' '));
		vkLogInfoVA("Side data:       %s", exportInfo.audio.sideData.Serialize(true, "", ' '));
		vkLogInfoVA("Filters:         %s", exportInfo.audio.filters.Serialize());
	}

	vkLogSep();

	// Create encoder instance
	encoder = new EncoderEngine(exportInfo);
	if (encoder->open() < 0)
	{
		vkLogInfo("Opening encoder failed! Aborting ...")
		return E_FAIL;
	}

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

	HRESULT hr = encoder->writeAudioFrame(frame) == 0 ? S_OK : E_FAIL;

	av_frame_free(&frame);

	return hr;
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
			return E_FAIL;
		}
	}

	// Fill frame
	AVFrame* f = av_frame_alloc();
	f->width = frame.width;
	f->height = frame.height;
	f->format = av_get_pix_fmt(frame.format);
	f->color_range = (AVColorRange)av_color_range_from_name(frame.colorRange);
	f->colorspace = (AVColorSpace)av_color_space_from_name(frame.colorSpace);
	f->color_primaries = (AVColorPrimaries)av_color_primaries_from_name(frame.colorPrimaries);
	f->color_trc = (AVColorTransferCharacteristic)av_color_transfer_from_name(frame.colorTrc);

	// Fill each plane
	for (int i = 0; i < frame.planes; i++)
	{
		f->data[i] = frame.buffer[i];
		f->linesize[i] = frame.rowsize[i];
	}

	HRESULT hr = encoder->writeVideoFrame(f) == 0 ? S_OK : E_FAIL;

	av_frame_free(&f);

	return hr;
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