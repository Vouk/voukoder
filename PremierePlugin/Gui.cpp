#include "Gui.h"
#include <wx/wx.h>
#include <LanguageUtils.h>
#include <Log.h>

class actctx_activator
{
protected:
	ULONG_PTR m_cookie; // Cookie for context deactivation

public:
	// Construct the activator and activates the given activation context
	actctx_activator(_In_ HANDLE hActCtx)
	{
		if (!ActivateActCtx(hActCtx, &m_cookie))
			m_cookie = 0;
	}

	// Deactivates activation context and destructs the activator
	virtual ~actctx_activator()
	{
		if (m_cookie)
			DeactivateActCtx(0, m_cookie);
	}
};

HINSTANCE g_instance = NULL;
HANDLE g_act_ctx = NULL;

BOOL WINAPI DllMain(_In_ HINSTANCE hinstDLL, _In_ DWORD fdwReason, _In_ LPVOID lpvReserved)
{
	UNREFERENCED_PARAMETER(lpvReserved);

	if (fdwReason == DLL_PROCESS_ATTACH)
	{
		g_instance = hinstDLL;
		GetCurrentActCtx(&g_act_ctx);
	}
	else if (fdwReason == DLL_PROCESS_DETACH)
	{
		if (g_act_ctx)
			ReleaseActCtx(g_act_ctx);
	}

	return TRUE;
}

Gui::Gui(PrSuites *suites, csSDK_uint32 pluginId) :
	suites(suites),
	pluginId(pluginId)
{
	Voukoder::Config::Get();
}

prMALError Gui::Create()
{
	const PrSDKExportParamSuite *params = suites->exportParamSuite;
	const PrSDKExportInfoSuite *infoSuite = suites->exportInfoSuite;

	// Add parameter groups
	csSDK_int32 groupId;
	params->AddMultiGroup(pluginId, &groupId);
	prCreateParamGroup("ui.premiere.tab.video", ADBEVideoTabGroup, ADBETopParamGroup);
	prCreateParamGroup("ui.premiere.tab.video.basic", ADBEBasicVideoGroup, ADBEVideoTabGroup);
	prCreateParamGroup("ui.premiere.tab.audio", ADBEAudioTabGroup, ADBETopParamGroup);
	prCreateParamGroup("ui.premiere.tab.audio.basic", ADBEBasicAudioGroup, ADBEAudioTabGroup);
	prCreateParamGroup("ui.premiere.tab.voukoder", VKDRVoukoderTabGroup, ADBETopParamGroup);

	// Get sequence values
	PrParam seqWidth, seqHeight, seqPARNum, seqPARDen, seqFrameRate, seqFieldOrder, seqSampleRate, seqChannelType;
	infoSuite->GetExportSourceInfo(pluginId, kExportInfo_VideoWidth, &seqWidth);
	infoSuite->GetExportSourceInfo(pluginId, kExportInfo_VideoHeight, &seqHeight);
	infoSuite->GetExportSourceInfo(pluginId, kExportInfo_PixelAspectNumerator, &seqPARNum);
	infoSuite->GetExportSourceInfo(pluginId, kExportInfo_PixelAspectDenominator, &seqPARDen);
	infoSuite->GetExportSourceInfo(pluginId, kExportInfo_VideoFrameRate, &seqFrameRate);
	infoSuite->GetExportSourceInfo(pluginId, kExportInfo_VideoFieldType, &seqFieldOrder);
	infoSuite->GetExportSourceInfo(pluginId, kExportInfo_AudioSampleRate, &seqSampleRate);
	infoSuite->GetExportSourceInfo(pluginId, kExportInfo_AudioChannelsType, &seqChannelType);

	// Req'd for AME
	if (seqWidth.mInt32 == 0)
		seqWidth.mInt32 = 1920;

	if (seqHeight.mInt32 == 0)
		seqHeight.mInt32 = 1080;

	// Create ADBE UI elements
	prCreateIntParam(ADBEVideoCodec, ADBEBasicVideoGroup, exParamFlag_none, 0, NULL, NULL, kPrFalse, kPrTrue);
	prCreateIntParam(ADBEVideoWidth, ADBEBasicVideoGroup, exParamFlag_none, seqWidth.mInt32, 320, 10240, kPrFalse, kPrFalse);
	prCreateIntParam(ADBEVideoHeight, ADBEBasicVideoGroup, exParamFlag_none, seqHeight.mInt32, 240, 8192, kPrFalse, kPrFalse);
	prCreateRatioParam(ADBEVideoAspect, ADBEBasicVideoGroup, exParamFlag_none, seqPARNum.mInt32, seqPARDen.mInt32);
	prCreateTimeParam(ADBEVideoFPS, ADBEBasicVideoGroup, exParamFlag_none, seqFrameRate.mInt64);
	prCreateIntParam(ADBEVideoFieldType, ADBEBasicVideoGroup, exParamFlag_none, seqFieldOrder.mInt32, NULL, NULL, kPrFalse, kPrFalse);
	prCreateIntParam(VKDRVideoColorRange, ADBEBasicVideoGroup, exParamFlag_none, VKDRColorRanges::LIMITED, NULL, NULL, kPrFalse, kPrFalse);
	prCreateIntParam(VKDRVideoColorSpace, ADBEBasicVideoGroup, exParamFlag_none, VKDRColorSpaces::BT709, NULL, NULL, kPrFalse, kPrFalse);
	prCreateIntParam(ADBEAudioCodec, ADBEBasicAudioGroup, exParamFlag_none, 0, NULL, NULL, kPrFalse, kPrTrue);
	prCreateFloatParam(ADBEAudioRatePerSecond, ADBEBasicAudioGroup, exParamFlag_none, seqSampleRate.mFloat64);
	prCreateIntParam(ADBEAudioNumChannels, ADBEBasicAudioGroup, exParamFlag_none, seqChannelType.mInt32, NULL, NULL, kPrFalse, kPrFalse);

	// Create Voukoder elements
	prCreateIntParam(VKDRSelectedMuxer, VKDRVoukoderTabGroup, exParamFlag_none, 0, NULL, NULL, kPrFalse, kPrFalse);
	prCreateIntParam(VKDRSelectedVideoEncoder, VKDRVoukoderTabGroup, exParamFlag_none, 0, NULL, NULL, kPrFalse, kPrFalse);
	prCreateIntParam(VKDRSelectedAudioEncoder, VKDRVoukoderTabGroup, exParamFlag_none, 0, NULL, NULL, kPrFalse, kPrFalse);
	prCreateButtonParam(VKDRVoukoderConfiguration, VKDRVoukoderTabGroup, exParamFlag_independant);

	return malNoError;
}

prMALError Gui::Update()
{
	// Translate the ui params
	prSetGroupName(ADBEVideoTabGroup, "ui.premiere.tab.video");
	prSetGroupName(ADBEBasicVideoGroup, "ui.premiere.tab.video.basic");
	prSetNameDescription(ADBEVideoWidth, "ui.premiere.tab.video.width");
	prSetNameDescription(ADBEVideoHeight, "ui.premiere.tab.video.height");
	prSetNameDescription(ADBEVideoAspect, "ui.premiere.tab.video.aspect");
	prSetNameDescription(ADBEVideoFPS, "ui.premiere.tab.video.framerate");
	prSetNameDescription(ADBEVideoFieldType, "ui.premiere.tab.video.fieldtype");
	prSetNameDescription(VKDRVideoColorRange, "ui.premiere.tab.video.colorrange");
	prSetNameDescription(VKDRVideoColorSpace, "ui.premiere.tab.video.colorspace");
	prSetGroupName(ADBEAudioTabGroup, "ui.premiere.tab.audio");
	prSetGroupName(ADBEBasicAudioGroup, "ui.premiere.tab.audio.basic");
	prSetNameDescription(ADBEAudioRatePerSecond, "ui.premiere.tab.audio.samplerate");
	prSetNameDescription(ADBEAudioNumChannels, "ui.premiere.tab.audio.channels");
	prSetGroupName(VKDRVoukoderTabGroup, "ui.premiere.tab.voukoder");
	prSetNameDescription(VKDRSelectedMuxer, "ui.premiere.tab.voukoder.selectedMuxer");
	prSetNameDescription(VKDRSelectedVideoEncoder, "ui.premiere.tab.voukoder.selectedVideoEncoder");
	prSetNameDescription(VKDRSelectedAudioEncoder, "ui.premiere.tab.voukoder.selectedAudioEncoder");
	prSetNameDescription(VKDRVoukoderConfiguration, "ui.premiere.tab.voukoder.configuration");

	PrSDKExportParamSuite *paramSuite = suites->exportParamSuite;

	// Video aspect
	paramSuite->ClearConstrainedValues(pluginId, 0, ADBEVideoAspect);
	exOneParamValueRec paramPar;
	for (csSDK_int32 i = 0; i < sizeof(VideoAspects) / sizeof(VideoAspects[0]); i++)
	{
		paramPar.ratioValue.numerator = VideoAspects[i][0];
		paramPar.ratioValue.denominator = VideoAspects[i][1];
	    wstring text = Trans("ui.premiere.tab.video.aspect", "item_" + to_string(i));
		paramSuite->AddConstrainedValuePair(pluginId, 0, ADBEVideoAspect, &paramPar, text.c_str());
	}

	// Get ticks per second
	PrTime ticksPerSecond;
	suites->timeSuite->GetTicksPerSecond(&ticksPerSecond);

	// Video framerate
	paramSuite->ClearConstrainedValues(pluginId, 0, ADBEVideoFPS);
	exOneParamValueRec paramFps;
	for (csSDK_int32 i = 0; i < sizeof(VideoFramerates) / sizeof(VideoFramerates[0]); i++)
	{
		paramFps.timeValue = ticksPerSecond / VideoFramerates[i][0] * VideoFramerates[i][1];
		wstring text = Trans("ui.premiere.tab.video.framerate", "item_" + to_string(i));
		paramSuite->AddConstrainedValuePair(pluginId, 0, ADBEVideoFPS, &paramFps, text.c_str());
	}

	// Video field type
	paramSuite->ClearConstrainedValues(pluginId, 0, ADBEVideoFieldType);
	exOneParamValueRec paramFieldType;
	for (csSDK_int32 i = 0; i < sizeof(VideoFieldTypes) / sizeof(VideoFieldTypes[0]); i++)
	{
		paramFieldType.intValue = VideoFieldTypes[i];
		wstring text = Trans("ui.premiere.tab.video.fieldtype", "item_" + to_string(i));
		paramSuite->AddConstrainedValuePair(pluginId, 0, ADBEVideoFieldType, &paramFieldType, text.c_str());
	}

	// Video color range
	paramSuite->ClearConstrainedValues(pluginId, 0, VKDRVideoColorRange);
	exOneParamValueRec paramColorRangeType;
	for (csSDK_int32 i = 0; i < sizeof(ColorRanges) / sizeof(ColorRanges[0]); i++)
	{
		paramColorRangeType.intValue = ColorRanges[i];
		wstring text = Trans("ui.premiere.tab.video.colorrange", "item_" + to_string(i));
		paramSuite->AddConstrainedValuePair(pluginId, 0, VKDRVideoColorRange, &paramColorRangeType, text.c_str());
	}
	
	// Video color space
	paramSuite->ClearConstrainedValues(pluginId, 0, VKDRVideoColorSpace);
	exOneParamValueRec paramColorSpace;
	for (csSDK_int32 i = 0; i < sizeof(ColorSpaces) / sizeof(ColorSpaces[0]); i++)
	{
		paramColorSpace.intValue = ColorSpaces[i];
		wstring text = Trans("ui.premiere.tab.video.colorspace", "item_" + to_string(i));
		paramSuite->AddConstrainedValuePair(pluginId, 0, VKDRVideoColorSpace, &paramColorSpace, text.c_str());
	}

	// Audio sample rates
	paramSuite->ClearConstrainedValues(pluginId, 0, ADBEAudioRatePerSecond);
	exOneParamValueRec paramAudioRate;
	for (csSDK_int32 i = 0; i < sizeof(AudioSamplingRates) / sizeof(AudioSamplingRates[0]); i++)
	{
		paramAudioRate.floatValue = AudioSamplingRates[i];
		wstring text = Trans("ui.premiere.tab.audio.samplerate", "item_" + to_string(i));
		paramSuite->AddConstrainedValuePair(pluginId, 0, ADBEAudioRatePerSecond, &paramAudioRate, text.c_str());
	}

	// Audio channels
	paramSuite->ClearConstrainedValues(pluginId, 0, ADBEAudioNumChannels);
	exOneParamValueRec paramAudioChannels;
	for (csSDK_int32 i = 0; i < sizeof(AudioChannels) / sizeof(AudioChannels[0]); i++)
	{
		paramAudioChannels.intValue = AudioChannels[i];
		wstring text = Trans("ui.premiere.tab.audio.channels", "item_" + to_string(i));
		paramSuite->AddConstrainedValuePair(pluginId, 0, ADBEAudioNumChannels, &paramAudioChannels, text.c_str());
	}

	CheckSettings();

	return malNoError;
}

void Gui::CheckSettings()
{
	// Clear info
	PrSDKExportParamSuite *paramSuite = suites->exportParamSuite;
	paramSuite->ClearConstrainedValues(pluginId, 0, VKDRSelectedMuxer);
	paramSuite->ClearConstrainedValues(pluginId, 0, VKDRSelectedVideoEncoder);
	paramSuite->ClearConstrainedValues(pluginId, 0, VKDRSelectedAudioEncoder);

	// Define first entry
	exOneParamValueRec value;
	value.intValue = 0;

	wxString videoEncoder, audioEncoder, muxer, none = Trans("ui.premiere.tab.voukoder.none");

	// Try to ready any existing config
	ExportInfo exportInfo;
	if (ReadEncoderOptions(VKDRVoukoderConfiguration, exportInfo))
	{
		muxer = Voukoder::GetResourceName(Voukoder::Config::Get().muxerInfos, exportInfo.format.id, none);
		videoEncoder = Voukoder::GetResourceName(Voukoder::Config::Get().encoderInfos, exportInfo.video.id, none);
		audioEncoder = Voukoder::GetResourceName(Voukoder::Config::Get().encoderInfos, exportInfo.audio.id, none);
	}
	else
	{
		videoEncoder = audioEncoder = muxer = none;
	}

	// Set the values
	paramSuite->AddConstrainedValuePair(pluginId, 0, VKDRSelectedMuxer, &value, muxer);
	paramSuite->AddConstrainedValuePair(pluginId, 0, VKDRSelectedVideoEncoder, &value, videoEncoder);
	paramSuite->AddConstrainedValuePair(pluginId, 0, VKDRSelectedAudioEncoder, &value, audioEncoder);
}

prMALError Gui::GetSelectedFileExtension(prUTF16Char *extension)
{
	wxString muxerId = DefaultMuxer;

	// Overwrite with selected muxer
	ExportInfo exportInfo;
	if (ReadEncoderOptions(VKDRVoukoderConfiguration, exportInfo))
	{
		muxerId = exportInfo.format.id;
	}

	// Find muxer info
	for (auto& muxer : Voukoder::Config::Get().muxerInfos)
	{
		if (muxer.id == muxerId)
		{
			prUTF16CharCopy(extension, muxer.extension);

			return malNoError;
		}
	}

	wxLogWarning("Unsupported format: %s", muxerId.c_str());

	return malUnknownError;
}

prMALError Gui::ButtonAction(exParamButtonRec *paramButtonRecP)
{
	if (strcmp(paramButtonRecP->buttonParamIdentifier, VKDRVoukoderConfiguration) == 0)
	{
		OpenVoukoderConfigDialog(paramButtonRecP);
	}

	return malNoError;
}

void Gui::OpenVoukoderConfigDialog(exParamButtonRec *paramButtonRecP)
{
	// Get JSON config
	ExportInfo exportInfo;
	exportInfo.video.enabled = paramButtonRecP->exportVideo == kPrTrue;
	exportInfo.audio.enabled = paramButtonRecP->exportAudio == kPrTrue;

	GetExportInfo(exportInfo);

	// Restore plugin's activation context.
	actctx_activator actctx(g_act_ctx);

	// Initialize application.
	new wxApp();
	wxEntryStart(g_instance);

	int result;

	// Have an own scope
	{
		HWND hwnd = GetActiveWindow();

		// Create wxWidget-approved parent window.
		wxWindow parent;
		parent.SetHWND((WXHWND)hwnd);
		parent.AdoptAttributesFromHWND();
		wxTopLevelWindows.Append(&parent);
	
		// Create and launch configuration dialog.
		wxVoukoderDialog dialog(&parent, exportInfo);
		dialog.SetConfiguration();
		result = dialog.ShowModal();

		wxTopLevelWindows.DeleteObject(&parent);
		parent.SetHWND((WXHWND)NULL);
	}
		
	// Clean-up and return.
	wxEntryCleanup();
	if (result == (int)wxID_OK)
	{
		StoreEncoderOptions(VKDRVoukoderConfiguration, exportInfo);
		CheckSettings();
	}
}

void Gui::ParamChange(exParamChangedRec *paramRecP)
{
	// En/disable encoder params
	prSetHidden(VKDRSelectedVideoEncoder, 1 - paramRecP->exportVideo);
	prSetHidden(VKDRSelectedAudioEncoder, 1 - paramRecP->exportAudio);

	CheckSettings();
}

bool Gui::ReadEncoderOptions(const char *dataId, ExportInfo &exportInfo)
{
	csSDK_int32 dataSize = 0;

	// Read arb data size
	if (PrSuiteErrorFailed(suites->exportParamSuite->GetArbData(pluginId, 0, dataId, &dataSize, NULL)))
	{
		vkLogError("Unable to retrieve arb data from: %s\n", dataId);
		return false;
	}

	// Is arb data avilable?
	if (dataSize)
	{
		// Load the encoder settings
		ArbData arbData;
		if (PrSuiteErrorSucceeded(suites->exportParamSuite->GetArbData(pluginId, 0, dataId, &dataSize, reinterpret_cast<void*>(&arbData))))
		{
			// Get codec options
			wxString videoOptions = arbData.videoCodecOptions;
			wxString audioOptions = arbData.audioCodecOptions;

			// Convert version=0 parameters
			if (arbData.version == 0)
			{
				videoOptions.Replace(",", PARAM_SEPARATOR);
				audioOptions.Replace(",", PARAM_SEPARATOR);
			}

			// Set main values
			exportInfo.video.id = wxString(arbData.videoCodecId);
			exportInfo.video.options.Deserialize(videoOptions);
			exportInfo.video.filters.Deserialize(arbData.videoFilters);
			exportInfo.audio.id = wxString(arbData.audioCodecId);
			exportInfo.audio.options.Deserialize(audioOptions);
			exportInfo.audio.filters.Deserialize(arbData.audioFilters);
			exportInfo.format.id = wxString(arbData.formatId);
			exportInfo.format.faststart = arbData.faststart;

			// Set pixel format
			if (exportInfo.video.options.find("_pixelFormat") != exportInfo.video.options.end())
			{
				exportInfo.video.pixelFormat = av_get_pix_fmt(exportInfo.video.options.at("_pixelFormat").c_str());
			}

			// Set sample format
			if (exportInfo.audio.options.find("_sampleFormat") != exportInfo.audio.options.end())
			{
				exportInfo.audio.sampleFormat = av_get_sample_fmt(exportInfo.audio.options.at("_sampleFormat").c_str());
			}

			return true;
		}
		else
		{
			vkLogError("Failed loading encoder configuration.");
		}
	}

	return false;
}

bool Gui::StoreEncoderOptions(const char *dataId, ExportInfo exportInfo)
{
	ArbData arbData;
	arbData.version = ARB_VERSION;
	prUTF16CharCopy(arbData.videoCodecId, exportInfo.video.id.ToStdWstring().c_str());
	prUTF16CharCopy(arbData.videoCodecOptions, exportInfo.video.options.Serialize(true).c_str());
	prUTF16CharCopy(arbData.videoFilters, exportInfo.video.filters.Serialize());
	prUTF16CharCopy(arbData.audioCodecId, exportInfo.audio.id.ToStdWstring().c_str());
	prUTF16CharCopy(arbData.audioCodecOptions, exportInfo.audio.options.Serialize(true).c_str());
	prUTF16CharCopy(arbData.audioFilters, exportInfo.audio.filters.Serialize());
	prUTF16CharCopy(arbData.formatId, exportInfo.format.id.ToStdWstring().c_str());
	arbData.faststart = exportInfo.format.faststart;

	// Save the encoder settings
	csSDK_int32 size = static_cast<csSDK_int32>(sizeof(ArbData));

	// Store encoder configuration
	prSuiteError ret = suites->exportParamSuite->SetArbData(pluginId, 0, dataId, size, reinterpret_cast<void*>(&arbData)) == malNoError;
	if (PrSuiteErrorFailed(ret))
	{
		vkLogError("Storing encoder configuration failed!");
		return false;
	}

	return true;
}

prMALError Gui::Validate()
{
	// Try to ready any existing config
	ExportInfo exportInfo;
	if (ReadEncoderOptions(VKDRVoukoderConfiguration, exportInfo))
	{
		return exportReturn_ErrNone;
	}

	// Report missing configuration to the user
	ReportMessage(Trans("ui.premiere.error.missingConfiguration"));

	return exportReturn_ErrLastErrorSet;
}

bool Gui::ClearEncoderOptions(const char *dataId)
{
	//vkLogInfo("Clearing encoder configuration ...");

	// Clear encoder configuration
	prSuiteError ret = suites->exportParamSuite->SetArbData(pluginId, 0, dataId, 0, NULL) == malNoError;
	if (PrSuiteErrorFailed(ret))
	{
		vkLogError("Clearing encoder configuration failed!");
		return false;
	}

	return true;
}

template <class T>
static inline T GCD(T a, T b) { if (a == 0) return b; return GCD(b % a, a); }

void Gui::GetExportInfo(ExportInfo &exportInfo)
{
	// Get current settings
	exParamValues width, height, aspect, fieldType, audioChannelType, ticksPerFrame, colorRange, colorSpace, audioSampleRate;
	suites->exportParamSuite->GetParamValue(pluginId, 0, ADBEVideoWidth, &width);
	suites->exportParamSuite->GetParamValue(pluginId, 0, ADBEVideoHeight, &height);
	suites->exportParamSuite->GetParamValue(pluginId, 0, ADBEVideoFPS, &ticksPerFrame);
	suites->exportParamSuite->GetParamValue(pluginId, 0, ADBEVideoAspect, &aspect);
	suites->exportParamSuite->GetParamValue(pluginId, 0, ADBEVideoFieldType, &fieldType);
	suites->exportParamSuite->GetParamValue(pluginId, 0, VKDRVideoColorRange, &colorRange);
	suites->exportParamSuite->GetParamValue(pluginId, 0, VKDRVideoColorSpace, &colorSpace);
	suites->exportParamSuite->GetParamValue(pluginId, 0, ADBEAudioRatePerSecond, &audioSampleRate);
	suites->exportParamSuite->GetParamValue(pluginId, 0, ADBEAudioNumChannels, &audioChannelType);

	// Find FPS rate
	const PrTime c = GCD(254016000000, ticksPerFrame.value.timeValue);

	// Fill encoder info
	exportInfo.video.width = width.value.intValue;
	exportInfo.video.height = height.value.intValue;
	exportInfo.video.ticksPerFrame = ticksPerFrame.value.timeValue;
	exportInfo.video.timebase.num = static_cast<int>(ticksPerFrame.value.timeValue / c);
	exportInfo.video.timebase.den = static_cast<int>(254016000000 / c);
	exportInfo.video.sampleAspectRatio.num = aspect.value.ratioValue.numerator;
	exportInfo.video.sampleAspectRatio.den = aspect.value.ratioValue.denominator;
	exportInfo.audio.timebase.num = 1;
	exportInfo.audio.timebase.den = static_cast<int>(audioSampleRate.value.floatValue);

	// Read settings
	ReadEncoderOptions(VKDRVoukoderConfiguration, exportInfo);

	// Multipass encoding
	exportInfo.passes = 1;
	if (exportInfo.video.options.find("_2pass") != exportInfo.video.options.end())
	{
		if (exportInfo.video.options.at("_2pass") == "1")
			exportInfo.passes = 2;
	}

	// Pixel format
	if (exportInfo.video.enabled && exportInfo.video.pixelFormat == AV_PIX_FMT_NONE)
	{
		exportInfo.video.pixelFormat = AV_PIX_FMT_YUV420P;
	}

	// Sample format
	if (exportInfo.audio.enabled && exportInfo.audio.sampleFormat == AV_SAMPLE_FMT_NONE)
	{
		AVCodec *codec = avcodec_find_encoder_by_name(exportInfo.audio.id);
		if (codec != NULL) // TODO: Check if sizeof(sample_fmts) > 0?
		{
			exportInfo.audio.sampleFormat = codec->sample_fmts[0];
		}
	}

	// Video field order
	if (fieldType.value.intValue == prFieldsLowerFirst)
	{
		exportInfo.video.fieldOrder = AVFieldOrder::AV_FIELD_BB;
	}
	else if (fieldType.value.intValue == prFieldsUpperFirst)
	{
		exportInfo.video.fieldOrder = AVFieldOrder::AV_FIELD_TT;
	}
	else
	{
		exportInfo.video.fieldOrder = AVFieldOrder::AV_FIELD_PROGRESSIVE;
	}
	
	// Color ranges
	if (colorRange.value.intValue == (csSDK_int32)VKDRColorRanges::LIMITED)
	{
		exportInfo.video.colorRange = AVColorRange::AVCOL_RANGE_MPEG;
	}
	else if (colorRange.value.intValue == (csSDK_int32)VKDRColorRanges::FULL)
	{
		exportInfo.video.colorRange = AVColorRange::AVCOL_RANGE_JPEG;
	}

	// Color spaces
	switch (colorSpace.value.intValue)
	{
	case VKDRColorSpaces::BT601_PAL:
		exportInfo.video.colorSpace = AVColorSpace::AVCOL_SPC_BT470BG;
		exportInfo.video.colorPrimaries = AVColorPrimaries::AVCOL_PRI_BT470BG;
		exportInfo.video.colorTransferCharacteristics = AVColorTransferCharacteristic::AVCOL_TRC_GAMMA28;
		break;

	case VKDRColorSpaces::BT601_NTSC:
		exportInfo.video.colorSpace = AVColorSpace::AVCOL_SPC_SMPTE170M;
		exportInfo.video.colorPrimaries = AVColorPrimaries::AVCOL_PRI_SMPTE170M;
		exportInfo.video.colorTransferCharacteristics = AVColorTransferCharacteristic::AVCOL_TRC_SMPTE170M;
		break;

	case VKDRColorSpaces::BT709:
		exportInfo.video.colorSpace = AVColorSpace::AVCOL_SPC_BT709;
		exportInfo.video.colorPrimaries = AVColorPrimaries::AVCOL_PRI_BT709;
		exportInfo.video.colorTransferCharacteristics = AVColorTransferCharacteristic::AVCOL_TRC_BT709;
		break;

	case VKDRColorSpaces::BT2020_CL:
	case VKDRColorSpaces::BT2020_NCL:
		exportInfo.video.colorPrimaries = AVColorPrimaries::AVCOL_PRI_BT2020;
		exportInfo.video.colorSpace = colorSpace.value.intValue == (csSDK_int32)VKDRColorSpaces::BT2020_CL ? AVCOL_SPC_BT2020_CL : AVCOL_SPC_BT2020_NCL;

		const AVPixFmtDescriptor *pixFmtDescr = av_pix_fmt_desc_get(exportInfo.video.pixelFormat);
		int bits = av_get_bits_per_pixel(pixFmtDescr) / av_pix_fmt_count_planes(exportInfo.video.pixelFormat);
		if (bits == 10) // WRONG!
		{
			exportInfo.video.colorTransferCharacteristics = AVColorTransferCharacteristic::AVCOL_TRC_BT2020_10;
		}
		else if (bits = 12)
		{
			exportInfo.video.colorTransferCharacteristics = AVColorTransferCharacteristic::AVCOL_TRC_BT2020_12;
		}
		break;
	}

	// Audio channel selection
	if (audioChannelType.value.intValue == (csSDK_int32)kPrAudioChannelType_Mono)
	{
		exportInfo.audio.channelLayout = AV_CH_LAYOUT_MONO;
	}
	else if (audioChannelType.value.intValue == (csSDK_int32)kPrAudioChannelType_51)
	{
		exportInfo.audio.channelLayout = AV_CH_LAYOUT_5POINT1_BACK;
	}
	else
	{
		exportInfo.audio.channelLayout = AV_CH_LAYOUT_STEREO;
	}
}

prMALError Gui::ReportMessage(wxString message, csSDK_uint32 type)
{
	// Use the adobe internal messaging system
	suites->exporterUtilitySuite->ReportEvent(pluginId, type, VKDR_APPNAME, message);

	if (type == kEventTypeError)
	{
		vkLogError(message);

		return exportReturn_ErrLastErrorSet;
	}
	else if (type == kEventTypeWarning)
	{
		vkLogWarn(message);

		return exportReturn_ErrLastWarningSet;
	}

	vkLogInfo(message);

	return exportReturn_ErrLastInfoSet;
}