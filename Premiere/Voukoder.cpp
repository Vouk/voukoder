#include <windows.h>
#include "Encoder.h"
#include "Voukoder.h"
#include "Common.h"
#include "InstructionSet.h"
#include "VideoRenderer.h"
#include <functional>

// reviewed 0.3.8
static void avlog_cb(void *, int level, const char * szFmt, va_list varg)
{
	char logbuf[2000];
	vsnprintf(logbuf, sizeof(logbuf), szFmt, varg);
	logbuf[sizeof(logbuf) - 1] = '\0';

	OutputDebugStringA(logbuf);
}

// reviewed 0.3.8
DllExport PREMPLUGENTRY xSDKExport(csSDK_int32 selector, exportStdParms *stdParmsP, void *param1, void	*param2)
{
	switch (selector)
	{
		case exSelStartup:					return exStartup(stdParmsP, reinterpret_cast<exExporterInfoRec*>(param1));
		case exSelShutdown:					return exShutdown();
		case exSelBeginInstance:			return exBeginInstance(stdParmsP, reinterpret_cast<exExporterInstanceRec*>(param1));
		case exSelGenerateDefaultParams:	return exGenerateDefaultParams(stdParmsP, reinterpret_cast<exGenerateDefaultParamRec*>(param1));
		case exSelPostProcessParams:		return exPostProcessParams(stdParmsP, reinterpret_cast<exPostProcessParamsRec*>(param1));
		case exSelValidateParamChanged:		return exValidateParamChanged(stdParmsP, reinterpret_cast<exParamChangedRec*>(param1));
		case exSelGetParamSummary:          return exGetParamSummary(stdParmsP, reinterpret_cast<exParamSummaryRec*>(param1));
		case exSelEndInstance:				return exEndInstance(stdParmsP, reinterpret_cast<exExporterInstanceRec*>(param1));
		case exSelExport:					return exExport(stdParmsP, reinterpret_cast<exDoExportRec*>(param1));
		case exSelQueryExportFileExtension: return exFileExtension(stdParmsP, reinterpret_cast<exQueryExportFileExtensionRec*>(param1));
		//case exSelQueryOutputFileList
		case exSelQueryOutputSettings:		return exQueryOutputSettings(stdParmsP, reinterpret_cast<exQueryOutputSettingsRec*>(param1));
		case exSelValidateOutputSettings:   return exValidateOutputSettings(stdParmsP, reinterpret_cast<exValidateOutputSettingsRec*>(param1));
	}

	return exportReturn_Unsupported;
}

// reviewed 0.3.8
prMALError exStartup(exportStdParms *stdParmsP, exExporterInfoRec *infoRecP)
{
	// Check for required processor
	if (!InstructionSet::SSSE3())
	{
		// Show an error message to the user
		MessageBoxA(NULL, "This plugin requires a processor supporting SSSE3!\n\n(https://de.wikipedia.org/wiki/Supplemental_Streaming_SIMD_Extensions_3)", "Attention!", MB_OK);

		return malUnknownError;
	}

	av_log_set_level(AV_LOG_DEBUG);
	av_log_set_callback(avlog_cb);

	av_register_all();
	avfilter_register_all();
	
	infoRecP->fileType = 'X264';
	prUTF16CharCopy(infoRecP->fileTypeName, PLUGIN_APPNAME);
	prUTF16CharCopy(infoRecP->fileTypeDefaultExtension, L"mkv");
	infoRecP->classID = 'VOUK';
	infoRecP->exportReqIndex = 0;
	infoRecP->wantsNoProgressBar = kPrFalse;
	infoRecP->hideInUI = kPrFalse;
	infoRecP->doesNotSupportAudioOnly = kPrTrue;
	infoRecP->canExportVideo = kPrTrue;
	infoRecP->canExportAudio = kPrTrue;
	infoRecP->interfaceVersion = EXPORTMOD_VERSION;
	infoRecP->isCacheable = kPrFalse;

	return malNoError;
}

prMALError exShutdown()
{
	// Maybe needed later

	return malNoError;
}

// reviewed 0.3.8
prMALError exBeginInstance(exportStdParms *stdParmsP, exExporterInstanceRec *instanceRecP)
{
	prMALError result = malNoError;

	/* Get the basic suite */
	SPBasicSuite *spBasic = stdParmsP->getSPBasicSuite();
	if (spBasic != NULL)
	{
		/* Get the memory manager */
		PrSDKMemoryManagerSuite *memorySuite;
		SPErr spError = spBasic->AcquireSuite(kPrSDKMemoryManagerSuite, kPrSDKMemoryManagerSuiteVersion, const_cast<const void**>(reinterpret_cast<void**>(&memorySuite)));

		/* Create the export settings */
		InstanceRec *instRec = reinterpret_cast<InstanceRec *>(memorySuite->NewPtrClear(sizeof(InstanceRec)));
		if (instRec)
		{
			instRec->spBasic = spBasic;
			instRec->memorySuite = memorySuite;

			spError = spBasic->AcquireSuite(kPrSDKTimeSuite, kPrSDKTimeSuiteVersion, const_cast<const void**>(reinterpret_cast<void**>(&(instRec->timeSuite))));
			spError = spBasic->AcquireSuite(kPrSDKExportParamSuite, kPrSDKExportParamSuiteVersion, const_cast<const void**>(reinterpret_cast<void**>(&(instRec->exportParamSuite))));
			spError = spBasic->AcquireSuite(kPrSDKExportInfoSuite, kPrSDKExportInfoSuiteVersion, const_cast<const void**>(reinterpret_cast<void**>(&(instRec->exportInfoSuite))));
			spError = spBasic->AcquireSuite(kPrSDKSequenceAudioSuite, kPrSDKSequenceAudioSuiteVersion, const_cast<const void**>(reinterpret_cast<void**>(&(instRec->sequenceAudioSuite))));
			spError = spBasic->AcquireSuite(kPrSDKExportFileSuite, kPrSDKExportFileSuiteVersion, const_cast<const void**>(reinterpret_cast<void**>(&(instRec->exportFileSuite))));
			spError = spBasic->AcquireSuite(kPrSDKPPixSuite, kPrSDKPPixSuiteVersion, const_cast<const void**>(reinterpret_cast<void**>(&(instRec->ppixSuite))));
			spError = spBasic->AcquireSuite(kPrSDKPPix2Suite, kPrSDKPPix2SuiteVersion, const_cast<const void**>(reinterpret_cast<void**>(&(instRec->ppix2Suite))));
			spError = spBasic->AcquireSuite(kPrSDKExportProgressSuite, kPrSDKExportProgressSuiteVersion, const_cast<const void**>(reinterpret_cast<void**>(&(instRec->exportProgressSuite))));
			spError = spBasic->AcquireSuite(kPrSDKWindowSuite, kPrSDKWindowSuiteVersion, const_cast<const void**>(reinterpret_cast<void**>(&(instRec->windowSuite))));
			spError = spBasic->AcquireSuite(kPrSDKExporterUtilitySuite, kPrSDKExporterUtilitySuiteVersion, const_cast<const void**>(reinterpret_cast<void**>(&(instRec->exporterUtilitySuite))));
			spError = spBasic->AcquireSuite(kPrSDKImageProcessingSuite, kPrSDKImageProcessingSuiteVersion, const_cast<const void**>(reinterpret_cast<void**>(&(instRec->imageProcessingSuite))));

			// Get the DLL module handle
			MEMORY_BASIC_INFORMATION mbi;
			static int dummy;
			VirtualQuery(&dummy, &mbi, sizeof(mbi));
			instRec->hInstance = reinterpret_cast<HMODULE>(mbi.AllocationBase);

			instRec->settings = new Settings();
			instRec->settings->initFromResources(instRec->hInstance);
		}

		instanceRecP->privateData = reinterpret_cast<void*>(instRec);
	}
	else
	{
		result = exportReturn_ErrMemory;
	}

	return result;
}

// reviewed 0.3.8
prMALError exEndInstance(exportStdParms *stdParmsP, exExporterInstanceRec *instanceRecP)
{
	prMALError result = malNoError;
	InstanceRec *instRec = reinterpret_cast<InstanceRec *>(instanceRecP->privateData);
	SPBasicSuite *spBasic = instRec->spBasic;

	if (spBasic != NULL)
	{
		if (instRec->timeSuite)
		{
			result = spBasic->ReleaseSuite(kPrSDKTimeSuite, kPrSDKTimeSuiteVersion);
		}

		if (instRec->exportParamSuite)
		{
			result = spBasic->ReleaseSuite(kPrSDKExportParamSuite, kPrSDKExportParamSuiteVersion);
		}

		if (instRec->exportInfoSuite)
		{
			result = spBasic->ReleaseSuite(kPrSDKExportInfoSuite, kPrSDKExportInfoSuiteVersion);
		}

		if (instRec->sequenceAudioSuite)
		{
			result = spBasic->ReleaseSuite(kPrSDKSequenceAudioSuite, kPrSDKSequenceAudioSuiteVersion);
		}

		if (instRec->exportFileSuite)
		{
			result = spBasic->ReleaseSuite(kPrSDKExportFileSuite, kPrSDKExportFileSuiteVersion);
		}

		if (instRec->ppixSuite)
		{
			result = spBasic->ReleaseSuite(kPrSDKPPixSuite, kPrSDKPPixSuiteVersion);
		}

		if (instRec->ppix2Suite)
		{
			result = spBasic->ReleaseSuite(kPrSDKPPix2Suite, kPrSDKPPix2SuiteVersion);
		}

		if (instRec->exportProgressSuite)
		{
			result = spBasic->ReleaseSuite(kPrSDKExportProgressSuite, kPrSDKExportProgressSuiteVersion);
		}
		
		if (instRec->windowSuite)
		{
			result = spBasic->ReleaseSuite(kPrSDKWindowSuite, kPrSDKWindowSuiteVersion);
		}

		if (instRec->exporterUtilitySuite)
		{
			result = spBasic->ReleaseSuite(kPrSDKExporterUtilitySuite, kPrSDKExporterUtilitySuiteVersion);
		}

		if (instRec->imageProcessingSuite)
		{
			result = spBasic->ReleaseSuite(kPrSDKImageProcessingSuite, kPrSDKImageProcessingSuiteVersion);
		}

		if (instRec->memorySuite)
		{
			instRec->memorySuite->PrDisposePtr(reinterpret_cast<PrMemoryPtr>(instRec));
			result = spBasic->ReleaseSuite(kPrSDKMemoryManagerSuite, kPrSDKMemoryManagerSuiteVersion);
		}
		
		delete(instRec->settings);
	}

	return malNoError;
}

// reviewed 0.3.8
prMALError exFileExtension(exportStdParms *stdParmsP, exQueryExportFileExtensionRec *exportFileExtensionRecP)
{
	csSDK_uint32 exID = exportFileExtensionRecP->exporterPluginID;
	InstanceRec *instRec = reinterpret_cast<InstanceRec *>(exportFileExtensionRecP->privateData);
	Settings *settings = instRec->settings;

	// Get the users multiplexer choice
	exParamValues multiplexer;
	instRec->exportParamSuite->GetParamValue(exID, 0, FFMultiplexer, &multiplexer);
	
	// Find the muxer
	vector<MuxerInfo> muxerInfos = settings->muxerInfos;
	const int muxerIdx = FindVectorIndexById(&settings->muxerInfos, multiplexer.value.intValue, Settings::defaultMuxer);
	const MuxerInfo muxerInfo = muxerInfos.at(muxerIdx);
	const std::wstring widestr = std::wstring(muxerInfo.extension.begin(), muxerInfo.extension.end());
	prUTF16CharCopy(exportFileExtensionRecP->outFileExtension, widestr.c_str());

	return malNoError;
}

// reviewed 0.3.8
// TODO: Add some bitrate calc logic
prMALError exQueryOutputSettings(exportStdParms *stdParmsP, exQueryOutputSettingsRec *outputSettingsP)
{
	prMALError result = malNoError;

	csSDK_uint32 exID = outputSettingsP->exporterPluginID;
	csSDK_int32 mgroupIndex = 0;
	exParamValues width, height, frameRate, pixelAspectRatio, fieldType, sampleRate, channelType;

	InstanceRec *instRec = reinterpret_cast<InstanceRec *>(outputSettingsP->privateData);
	Settings *settings = instRec->settings;
	PrSDKExportParamSuite *paramSuite = instRec->exportParamSuite;

	float fps = 0.0f;

	if (outputSettingsP->inExportVideo)
	{
		paramSuite->GetParamValue(exID, mgroupIndex, ADBEVideoWidth, &width);
		outputSettingsP->outVideoWidth = width.value.intValue;
		paramSuite->GetParamValue(exID, mgroupIndex, ADBEVideoHeight, &height);
		outputSettingsP->outVideoHeight = height.value.intValue;
		paramSuite->GetParamValue(exID, mgroupIndex, ADBEVideoFPS, &frameRate);
		outputSettingsP->outVideoFrameRate = frameRate.value.timeValue;
		paramSuite->GetParamValue(exID, mgroupIndex, ADBEVideoAspect, &pixelAspectRatio);
		outputSettingsP->outVideoAspectNum = pixelAspectRatio.value.ratioValue.numerator;
		outputSettingsP->outVideoAspectDen = pixelAspectRatio.value.ratioValue.denominator;
		paramSuite->GetParamValue(exID, mgroupIndex, ADBEVideoFieldType, &fieldType);
		outputSettingsP->outVideoFieldType = fieldType.value.intValue;
	}
	if (outputSettingsP->inExportAudio)
	{
		paramSuite->GetParamValue(exID, mgroupIndex, ADBEAudioRatePerSecond, &sampleRate);
		outputSettingsP->outAudioSampleRate = sampleRate.value.floatValue;
		outputSettingsP->outAudioSampleType = kPrAudioSampleType_32BitFloat;
		paramSuite->GetParamValue(exID, mgroupIndex, ADBEAudioNumChannels, &channelType);
		outputSettingsP->outAudioChannelType = (PrAudioChannelType)channelType.value.intValue;
	}

	// Bitrate calculation is not possible for not-constant-bitrate encoders
	outputSettingsP->outBitratePerSecond = 0;

	return result;
}

// reviewed 0.3.8
// TODO: Use element creation function here
prMALError exGenerateDefaultParams(exportStdParms *stdParms, exGenerateDefaultParamRec *generateDefaultParamRec)
{
	prMALError result = malNoError;
	PrParam seqWidth, seqHeight, seqPARNum, seqPARDen, seqFrameRate, seqFieldOrder, seqChannelType, seqSampleRate;

	InstanceRec *instRec = reinterpret_cast<InstanceRec *>(generateDefaultParamRec->privateData);
	PrSDKExportParamSuite *exportParamSuite = instRec->exportParamSuite;
	PrSDKExportInfoSuite *exportInfoSuite = instRec->exportInfoSuite;
	PrSDKTimeSuite *timeSuite = instRec->timeSuite;
	Settings *settings = instRec->settings;

	csSDK_int32 exID = generateDefaultParamRec->exporterPluginID;
	csSDK_int32 groupIndex = 0;

	// Get the values from the sequence
	exportInfoSuite->GetExportSourceInfo(exID, kExportInfo_VideoWidth, &seqWidth);
	exportInfoSuite->GetExportSourceInfo(exID, kExportInfo_VideoHeight, &seqHeight);
	exportInfoSuite->GetExportSourceInfo(exID, kExportInfo_PixelAspectNumerator, &seqPARNum);
	exportInfoSuite->GetExportSourceInfo(exID, kExportInfo_PixelAspectDenominator, &seqPARDen);
	exportInfoSuite->GetExportSourceInfo(exID, kExportInfo_VideoFrameRate, &seqFrameRate);
	exportInfoSuite->GetExportSourceInfo(exID, kExportInfo_VideoFieldType, &seqFieldOrder);
	exportInfoSuite->GetExportSourceInfo(exID, kExportInfo_AudioSampleRate, &seqSampleRate);
	exportInfoSuite->GetExportSourceInfo(exID, kExportInfo_AudioChannelsType, &seqChannelType);

	// Width
	if (seqWidth.mInt32 == 0)
	{
		seqWidth.mInt32 = 1920;
	}

	// Height
	if (seqHeight.mInt32 == 0)
	{
		seqHeight.mInt32 = 1080;
	}

	exportParamSuite->AddMultiGroup(exID, &groupIndex);

#pragma region Group: Basic Video Settings

	// Video tab group
	exportParamSuite->AddParamGroup(exID, groupIndex, ADBETopParamGroup, ADBEVideoTabGroup, L"Video", kPrFalse, kPrFalse, kPrFalse);

	// Basic settings group
	exportParamSuite->AddParamGroup(exID, groupIndex, ADBEVideoTabGroup, ADBEBasicVideoGroup, L"Basic Video Settings", kPrFalse, kPrFalse, kPrFalse);

	// Video Encoder
	ParamInfo paramInfo;
	paramInfo.default.intValue = settings->defaultVideoEncoder;
	paramInfo.type = "int";
	exNewParamInfo videoCodecParam = CreateParamElement(&paramInfo, false);
	::lstrcpyA(videoCodecParam.identifier, ADBEVideoCodec);
	exportParamSuite->AddParam(exID, groupIndex, ADBEBasicVideoGroup, &videoCodecParam);

	// Video width
	exParamValues widthValues;
	widthValues.structVersion = 1;
	widthValues.value.intValue = seqWidth.mInt32;
	widthValues.rangeMin.intValue = 16;
	widthValues.rangeMax.intValue = 5120;
	widthValues.disabled = kPrFalse;
	widthValues.hidden = kPrFalse;
	widthValues.optionalParamEnabled = kPrFalse;
	exNewParamInfo widthParam;
	widthParam.structVersion = 1;
	widthParam.flags = exParamFlag_none;
	widthParam.paramType = exParamType_int;
	widthParam.paramValues = widthValues;
	::lstrcpyA(widthParam.identifier, ADBEVideoWidth);
	exportParamSuite->AddParam(exID, groupIndex, ADBEBasicVideoGroup, &widthParam);

	// Video height
	exParamValues heightValues;
	heightValues.structVersion = 1;
	heightValues.value.intValue = seqHeight.mInt32;
	heightValues.rangeMin.intValue = 16;
	heightValues.rangeMax.intValue = 5120;
	heightValues.disabled = kPrFalse;
	heightValues.hidden = kPrFalse;
	heightValues.optionalParamEnabled = kPrFalse;
	exNewParamInfo heightParam;
	heightParam.structVersion = 1;
	heightParam.flags = exParamFlag_none;
	heightParam.paramType = exParamType_int;
	heightParam.paramValues = heightValues;
	::lstrcpyA(heightParam.identifier, ADBEVideoHeight);
	exportParamSuite->AddParam(exID, groupIndex, ADBEBasicVideoGroup, &heightParam);

	// Pixel aspect ratio
	exParamValues PARValues;
	PARValues.structVersion = 1;
	PARValues.value.ratioValue.numerator = seqPARNum.mInt32;
	PARValues.value.ratioValue.denominator = seqPARDen.mInt32;
	PARValues.rangeMin.ratioValue.numerator = 10;
	PARValues.rangeMin.ratioValue.denominator = 11;
	PARValues.rangeMax.ratioValue.numerator = 2;
	PARValues.rangeMax.ratioValue.denominator = 1;
	PARValues.disabled = kPrFalse;
	PARValues.hidden = kPrFalse;
	PARValues.optionalParamEnabled = kPrFalse;
	exNewParamInfo PARParam;
	PARParam.structVersion = 1;
	PARParam.flags = exParamFlag_none;
	PARParam.paramType = exParamType_ratio;
	PARParam.paramValues = PARValues;
	::lstrcpyA(PARParam.identifier, ADBEVideoAspect);
	exportParamSuite->AddParam(exID, groupIndex, ADBEBasicVideoGroup, &PARParam);

	// Video frame rate
	exParamValues frameRateValues;
	frameRateValues.structVersion = 1;
	frameRateValues.value.timeValue = seqFrameRate.mInt64;
	frameRateValues.rangeMin.timeValue = 1;
	timeSuite->GetTicksPerSecond(&frameRateValues.rangeMax.timeValue);
	frameRateValues.disabled = kPrFalse;
	frameRateValues.hidden = kPrFalse;
	frameRateValues.optionalParamEnabled = kPrFalse;
	exNewParamInfo frameRateParam;
	frameRateParam.structVersion = 1;
	frameRateParam.flags = exParamFlag_none;
	frameRateParam.paramType = exParamType_ticksFrameRate;
	frameRateParam.paramValues = frameRateValues;
	::lstrcpyA(frameRateParam.identifier, ADBEVideoFPS);
	exportParamSuite->AddParam(exID, groupIndex, ADBEBasicVideoGroup, &frameRateParam);

	// Field order
	exParamValues fieldOrderValues;
	fieldOrderValues.structVersion = 1;
	fieldOrderValues.value.intValue = seqFieldOrder.mInt32;
	fieldOrderValues.disabled = kPrFalse;
	fieldOrderValues.hidden = kPrFalse;
	fieldOrderValues.optionalParamEnabled = kPrFalse;
	exNewParamInfo fieldOrderParam;
	fieldOrderParam.structVersion = 1;
	fieldOrderParam.flags = exParamFlag_none;
	fieldOrderParam.paramType = exParamType_int;
	fieldOrderParam.paramValues = fieldOrderValues;
	::lstrcpyA(fieldOrderParam.identifier, ADBEVideoFieldType);
	exportParamSuite->AddParam(exID, groupIndex, ADBEBasicVideoGroup, &fieldOrderParam);

	// TV standard
	exParamValues tvStandardValues;
	tvStandardValues.structVersion = 1;
	tvStandardValues.value.intValue = seqFieldOrder.mInt32;
	tvStandardValues.disabled = kPrFalse;
	tvStandardValues.hidden = kPrFalse;
	tvStandardValues.optionalParamEnabled = kPrFalse;
	exNewParamInfo tvStandardParam;
	tvStandardParam.structVersion = 1;
	tvStandardParam.flags = exParamFlag_none;
	tvStandardParam.paramType = exParamType_int;
	tvStandardParam.paramValues = tvStandardValues;
	::lstrcpyA(tvStandardParam.identifier, VKDRTVStandard);
	exportParamSuite->AddParam(exID, groupIndex, ADBEBasicVideoGroup, &tvStandardParam);

	// Color space
	exParamValues colorSpaceValues;
	colorSpaceValues.structVersion = 1;
	colorSpaceValues.value.intValue = vkdrBT709;
	colorSpaceValues.disabled = kPrFalse;
	colorSpaceValues.hidden = kPrFalse;
	colorSpaceValues.optionalParamEnabled = kPrFalse;
	exNewParamInfo colorSpaceParam;
	colorSpaceParam.structVersion = 1;
	colorSpaceParam.flags = exParamFlag_none;
	colorSpaceParam.paramType = exParamType_int;
	colorSpaceParam.paramValues = colorSpaceValues;
	::lstrcpyA(colorSpaceParam.identifier, VKDRColorSpace);
	exportParamSuite->AddParam(exID, groupIndex, ADBEBasicVideoGroup, &colorSpaceParam);

	// Color range
	exParamValues colorRangeValues;
	colorRangeValues.structVersion = 1;
	colorRangeValues.value.intValue = vkdrLimitedColorRange;
	colorRangeValues.disabled = kPrFalse;
	colorRangeValues.hidden = kPrFalse;
	colorRangeValues.optionalParamEnabled = kPrFalse;
	exNewParamInfo colorRangeParam;
	colorRangeParam.structVersion = 1;
	colorRangeParam.flags = exParamFlag_none;
	colorRangeParam.paramType = exParamType_bool;
	colorRangeParam.paramValues = colorRangeValues;
	::lstrcpyA(colorRangeParam.identifier, VKDRColorRange);
	exportParamSuite->AddParam(exID, groupIndex, ADBEBasicVideoGroup, &colorRangeParam);

#pragma endregion

	// Advanced video codec tab
	exportParamSuite->AddParamGroup(exID, groupIndex, ADBETopParamGroup, VKDRAdvVideoCodecTabGroup, L"Advanced", kPrFalse, kPrFalse, kPrFalse);

#pragma region Group: Video Encoder Options

	// Basic settings group
	exportParamSuite->AddParamGroup(exID, groupIndex, ADBEVideoTabGroup, ADBEVideoCodecGroup, L"Basic Encoder Settings", kPrFalse, kPrFalse, kPrFalse);

	// Populate video encoder options
	CreateEncoderParamElements(exportParamSuite, exID, groupIndex, settings->videoEncoderInfos, settings->defaultVideoEncoder);

#pragma endregion

#pragma region Group: Basic Audio Settings

	// Audio tab group
	exportParamSuite->AddParamGroup(exID, groupIndex, ADBETopParamGroup, ADBEAudioTabGroup, L"Audio", kPrFalse, kPrFalse, kPrFalse);

	// Basic audio settings group
	exportParamSuite->AddParamGroup(exID, groupIndex, ADBEAudioTabGroup, ADBEBasicAudioGroup, L"Basic Audio Settings", kPrFalse, kPrFalse, kPrFalse);

	// Audio Codec
	exParamValues audioCodecValues;
	audioCodecValues.structVersion = 1;
	audioCodecValues.value.intValue = settings->defaultAudioEncoder;
	audioCodecValues.disabled = kPrFalse;
	audioCodecValues.hidden = kPrFalse;
	audioCodecValues.optionalParamEnabled = kPrFalse;
	exNewParamInfo audioCodecParam;
	audioCodecParam.structVersion = 1;
	audioCodecParam.flags = exParamFlag_none;
	audioCodecParam.paramType = exParamType_int;
	audioCodecParam.paramValues = audioCodecValues;
	::lstrcpyA(audioCodecParam.identifier, ADBEAudioCodec);
	exportParamSuite->AddParam(exID, groupIndex, ADBEBasicAudioGroup, &audioCodecParam);

	// Sample rate
	exParamValues sampleRateValues;
	sampleRateValues.structVersion = 1;
	sampleRateValues.value.floatValue = seqSampleRate.mFloat64;
	sampleRateValues.disabled = kPrFalse;
	sampleRateValues.hidden = kPrFalse;
	sampleRateValues.optionalParamEnabled = kPrFalse;
	exNewParamInfo sampleRateParam;
	sampleRateParam.structVersion = 1;
	sampleRateParam.flags = exParamFlag_none;
	sampleRateParam.paramType = exParamType_float;
	sampleRateParam.paramValues = sampleRateValues;
	::lstrcpyA(sampleRateParam.identifier, ADBEAudioRatePerSecond);
	exportParamSuite->AddParam(exID, groupIndex, ADBEBasicAudioGroup, &sampleRateParam);

	// Channels
	exParamValues channelsValues;
	channelsValues.structVersion = 1;
	channelsValues.value.intValue = seqChannelType.mInt32;
	channelsValues.disabled = kPrFalse;
	channelsValues.hidden = kPrFalse;
	channelsValues.optionalParamEnabled = kPrFalse;
	exNewParamInfo channelsParam;
	channelsParam.structVersion = 1;
	channelsParam.flags = exParamFlag_none;
	channelsParam.paramType = exParamType_int;
	channelsParam.paramValues = channelsValues;
	::lstrcpyA(channelsParam.identifier, ADBEAudioNumChannels);
	exportParamSuite->AddParam(exID, groupIndex, ADBEBasicAudioGroup, &channelsParam);

#pragma endregion

#pragma region Group: Audio Encoder Options

	// Basic settings group
	exportParamSuite->AddParamGroup(exID, groupIndex, ADBEAudioTabGroup, ADBEAudioCodecGroup, L"Encoder Options", kPrFalse, kPrFalse, kPrFalse);

	// Populate audio encoder options
	CreateEncoderParamElements(exportParamSuite, exID, groupIndex, settings->audioEncoderInfos, audioCodecValues.value.intValue);

#pragma endregion

#pragma region Group: Multiplexer

	// Multiplexer tab group
	exportParamSuite->AddParamGroup(exID, groupIndex, ADBETopParamGroup, FFMultiplexerTabGroup, L"Multiplexer", kPrFalse, kPrFalse, kPrFalse);

	// Multiplexer settings group
	exportParamSuite->AddParamGroup(exID, groupIndex, FFMultiplexerTabGroup, FFMultiplexerBasicGroup, L"Multiplexer Settings", kPrFalse, kPrFalse, kPrFalse);

	// Multiplexer
	exParamValues multiplexerValues;
	multiplexerValues.structVersion = 1;
	multiplexerValues.value.intValue = settings->defaultMuxer;
	multiplexerValues.disabled = kPrFalse;
	multiplexerValues.hidden = kPrFalse;
	multiplexerValues.optionalParamEnabled = kPrFalse;
	exNewParamInfo multiplexerParam;
	multiplexerParam.structVersion = 1;
	multiplexerParam.flags = exParamFlag_multiLine;
	multiplexerParam.paramType = exParamType_int;
	multiplexerParam.paramValues = multiplexerValues;
	::lstrcpyA(multiplexerParam.identifier, FFMultiplexer);
	exportParamSuite->AddParam(exID, groupIndex, FFMultiplexerBasicGroup, &multiplexerParam);

#pragma endregion

	exportParamSuite->SetParamsVersion(exID, 1);

	return result;
}

// reviewed 0.3.8
// TODO: Really needs cleanup (put consts in main config)
prMALError exPostProcessParams(exportStdParms *stdParmsP, exPostProcessParamsRec *postProcessParamsRecP)
{
	prMALError result = malNoError;
	prUTF16Char paramString[kPrMaxName];
	exOneParamValueRec tempParamValue;

	const csSDK_int32		PARs[][2] = {
		{ 1, 1 },			// Square pixels (1.0)
		{ 10, 11 },		// D1/DV NTSC (0.9091)
		{ 40, 33 },		// D1/DV NTSC Widescreen 16:9 (1.2121)
		{ 768, 702 },		// D1/DV PAL (1.0940)
		{ 1024, 702 },	// D1/DV PAL Widescreen 16:9 (1.4587)
		{ 2, 1 },			// Anamorphic 2:1 (2.0)
		{ 4, 3 },			// HD Anamorphic 1080 (1.3333)
		{ 3, 2 }			// DVCPRO HD (1.5)
	};
	const wchar_t* const	PARStrings[] = {
		L"Square pixels (1.0)",
		L"D1/DV NTSC (0.9091)",
		L"D1/DV NTSC Widescreen 16:9 (1.2121)",
		L"D1/DV PAL (1.0940)",
		L"D1/DV PAL Widescreen 16:9 (1.4587)",
		L"Anamorphic 2:1 (2.0)",
		L"HD Anamorphic 1080 (1.3333)",
		L"DVCPRO HD (1.5)"
	};

	const csSDK_int32		fieldOrders[] = {
		prFieldsNone,
		prFieldsUpperFirst,
		prFieldsLowerFirst
	};
	const wchar_t* const	fieldOrderStrings[] = {
		L"None (Progressive)",
		L"Upper First",
		L"Lower First"
	};

	csSDK_int32 exID = postProcessParamsRecP->exporterPluginID;
	csSDK_int32 groupIndex = 0;

	InstanceRec *instRec = reinterpret_cast<InstanceRec *>(postProcessParamsRecP->privateData);
	Settings *settings = instRec->settings;

	// Get various settings
	json configuration = settings->mainConfig;

	instRec->exportParamSuite->SetParamName(exID, groupIndex, VKDRAdvVideoCodecTabGroup, L"Advanced");

#pragma region Group: Video settings

	// Label elements
	instRec->exportParamSuite->SetParamName(exID, groupIndex, ADBEBasicVideoGroup, L"Video settings");
	instRec->exportParamSuite->SetParamName(exID, groupIndex, ADBEVideoCodec, L"Video Encoder");
	instRec->exportParamSuite->SetParamName(exID, groupIndex, ADBEVideoWidth, L"Width");
	instRec->exportParamSuite->SetParamName(exID, groupIndex, ADBEVideoHeight, L"Height");
	instRec->exportParamSuite->SetParamName(exID, groupIndex, ADBEVideoAspect, L"Pixel Aspect Ratio");
	instRec->exportParamSuite->SetParamName(exID, groupIndex, ADBEVideoFPS, L"Frame Rate");
	instRec->exportParamSuite->SetParamName(exID, groupIndex, VKDRTVStandard, L"TV Standard");
	instRec->exportParamSuite->SetParamName(exID, groupIndex, ADBEVideoFieldType, L"Field Order");
	instRec->exportParamSuite->SetParamName(exID, groupIndex, VKDRColorSpace, L"Color Space");
	instRec->exportParamSuite->SetParamName(exID, groupIndex, VKDRColorRange, L"Use full color range");

	// Populate video encoder dropdown
	PopulateEncoders(instRec, exID, groupIndex, ADBEVideoCodec, settings->videoEncoderInfos);

	// Pixel aspect ratio
	instRec->exportParamSuite->ClearConstrainedValues(exID, 0, ADBEVideoAspect);
	exOneParamValueRec tempPAR;
	for (csSDK_int32 i = 0; i < sizeof(PARs) / sizeof(PARs[0]); i++)
	{
		tempPAR.ratioValue.numerator = PARs[i][0];
		tempPAR.ratioValue.denominator = PARs[i][1];
		swprintf(paramString, kPrMaxName, L"%s", PARStrings[i]);
		instRec->exportParamSuite->AddConstrainedValuePair(exID, 0, ADBEVideoAspect, &tempPAR, paramString);
	}

	// Get ticks per second
	PrTime ticksPerSecond = 0;
	instRec->timeSuite->GetTicksPerSecond(&ticksPerSecond);

	// Clear the framerate dropdown
	instRec->exportParamSuite->ClearConstrainedValues(exID, groupIndex, ADBEVideoFPS);

	// Populate framerate dropdown
	exOneParamValueRec tempFPS;
	for (auto iterator = configuration["videoFramerates"].begin(); iterator != configuration["videoFramerates"].end(); ++iterator)
	{
		json framerate = *iterator;

		int num = framerate["num"].get<int>();
		int den = framerate["den"].get<int>();

		tempFPS.timeValue = ticksPerSecond / num * den;

		std::string name = framerate["name"].get<std::string>();
		swprintf(paramString, kPrMaxName, L"%hs", name.c_str());
		instRec->exportParamSuite->AddConstrainedValuePair(exID, groupIndex, ADBEVideoFPS, &tempFPS, paramString);
	}

	// TV Standard
	instRec->exportParamSuite->ClearConstrainedValues(exID, groupIndex, VKDRTVStandard);
	exOneParamValueRec tempTvStandard;
	tempTvStandard.intValue = vkdrPAL;
	swprintf(paramString, kPrMaxName, L"%s", L"PAL");
	instRec->exportParamSuite->AddConstrainedValuePair(exID, groupIndex, VKDRTVStandard, &tempTvStandard, paramString);
	tempTvStandard.intValue = vkdrNTSC;
	swprintf(paramString, kPrMaxName, L"%s", L"NTSC");
	instRec->exportParamSuite->AddConstrainedValuePair(exID, groupIndex, VKDRTVStandard, &tempTvStandard, paramString);

	// Field orders
	instRec->exportParamSuite->ClearConstrainedValues(exID, groupIndex, ADBEVideoFieldType);
	exOneParamValueRec tempOrder;
	for (csSDK_int32 i = 0; i < sizeof(fieldOrders) / sizeof(fieldOrders[0]); i++)
	{
		tempOrder.intValue = fieldOrders[i];
		swprintf(paramString, kPrMaxName, L"%s", fieldOrderStrings[i]);
		instRec->exportParamSuite->AddConstrainedValuePair(exID, groupIndex, ADBEVideoFieldType, &tempOrder, paramString);
	}

	// Color Space
	instRec->exportParamSuite->ClearConstrainedValues(exID, groupIndex, VKDRColorSpace);
	exOneParamValueRec tempcolorSpace;
	tempcolorSpace.intValue = vkdrBT601;
	swprintf(paramString, kPrMaxName, L"%s", L"BT.601 (SD)");
	instRec->exportParamSuite->AddConstrainedValuePair(exID, groupIndex, VKDRColorSpace, &tempcolorSpace, paramString);
	tempcolorSpace.intValue = vkdrBT709;
	swprintf(paramString, kPrMaxName, L"%s", L"BT.709 (HD)");
	instRec->exportParamSuite->AddConstrainedValuePair(exID, groupIndex, VKDRColorSpace, &tempcolorSpace, paramString);

#pragma endregion

#pragma region Group: Video encoder settings

	instRec->exportParamSuite->SetParamName(exID, 0, ADBEVideoCodecGroup, L"Codec settings");

	PopulateParamValues(instRec, exID, groupIndex, settings->videoEncoderInfos);

#pragma endregion

#pragma region Group: Audio settings

	// Label elements
	instRec->exportParamSuite->SetParamName(exID, groupIndex, ADBEBasicAudioGroup, L"Audio settings");
	instRec->exportParamSuite->SetParamName(exID, groupIndex, ADBEAudioCodec, L"Audio Encoder");
	instRec->exportParamSuite->SetParamName(exID, groupIndex, ADBEAudioRatePerSecond, L"Sample Rate");
	instRec->exportParamSuite->SetParamName(exID, groupIndex, ADBEAudioNumChannels, L"Channels");

	// Populate audio encoder dropdown
	PopulateEncoders(instRec, exID, groupIndex, ADBEAudioCodec, settings->audioEncoderInfos);

	// Clear the samplerate dropdown
	instRec->exportParamSuite->ClearConstrainedValues(exID, groupIndex, ADBEAudioRatePerSecond);

	// Populate the samplerate dropdown
	exOneParamValueRec oneParamValueRec;
	for (auto iterator = configuration["audioSampleRates"].begin(); iterator != configuration["audioSampleRates"].end(); ++iterator)
	{
		json sampleRate = *iterator;

		oneParamValueRec.floatValue = sampleRate["id"].get<double>();

		std::string name = sampleRate["name"].get<std::string>();
		swprintf(paramString, kPrMaxName, L"%hs", name.c_str());
		instRec->exportParamSuite->AddConstrainedValuePair(exID, groupIndex, ADBEAudioRatePerSecond, &oneParamValueRec, paramString);
	}

	// Clear the channels dropdown
	instRec->exportParamSuite->ClearConstrainedValues(exID, groupIndex, ADBEAudioNumChannels);

	// Populate the channels dropdown
	exOneParamValueRec oneParamValueRec2;
	for (auto iterator = configuration["audioChannels"].begin(); iterator != configuration["audioChannels"].end(); ++iterator)
	{
		json channels = *iterator;

		oneParamValueRec2.intValue = channels["id"].get<int>();

		std::string name = channels["name"].get<std::string>();
		swprintf(paramString, kPrMaxName, L"%hs", name.c_str());
		instRec->exportParamSuite->AddConstrainedValuePair(exID, groupIndex, ADBEAudioNumChannels, &oneParamValueRec2, paramString);
	}

#pragma endregion

#pragma region Group: Audio encoder settings

	// Label elements
	instRec->exportParamSuite->SetParamName(exID, 0, ADBEAudioCodecGroup, L"Codec settings");

	// Populate audio encoder dropdown
	PopulateParamValues(instRec, exID, groupIndex, settings->audioEncoderInfos);

#pragma endregion

#pragma region Group: Multiplexer

	// Label elements
	instRec->exportParamSuite->SetParamName(exID, groupIndex, FFMultiplexerTabGroup, L"Multiplexer");
	instRec->exportParamSuite->SetParamName(exID, groupIndex, FFMultiplexerBasicGroup, L"Container");
	instRec->exportParamSuite->SetParamName(exID, groupIndex, FFMultiplexer, L"Format");

	// Clear the multiplexer dropdown
	instRec->exportParamSuite->ClearConstrainedValues(exID, groupIndex, FFMultiplexer);

	// Populate the multiplexer dropdown
	AVOutputFormat *format;
	for (MuxerInfo const muxerInfo : settings->muxerInfos)
	{
		format = av_guess_format(muxerInfo.name.c_str(), NULL, NULL);
		if (format != NULL)
		{
			tempParamValue.intValue = muxerInfo.id;
			swprintf(paramString, kPrMaxName, L"%hs", format->long_name);
			instRec->exportParamSuite->AddConstrainedValuePair(exID, groupIndex, FFMultiplexer, &tempParamValue, paramString);
		}
	}

#pragma endregion

	return result;
}

// reviewed 0.3.8
prMALError exValidateParamChanged(exportStdParms *stdParmsP, exParamChangedRec *validateParamChangedRecP)
{
	prMALError result = malNoError;

	/*

	exParamValues tempValue;

	InstanceRec *instRec = reinterpret_cast<InstanceRec *>(validateParamChangedRecP->privateData);
	Settings *settings = instRec->settings;

	csSDK_uint32 exID = validateParamChangedRecP->exporterPluginID;
	csSDK_uint32 groupIndex = validateParamChangedRecP->multiGroupIndex;

	// Get the changed value
	exParamValues changedValue;
	instRec->exportParamSuite->GetParamValue(exID, groupIndex, validateParamChangedRecP->changedParamIdentifier, &changedValue);

	// What has changed?
	const char* changedParamName = validateParamChangedRecP->changedParamIdentifier;

	// Is it a encoder selection element?
	if (strcmp(changedParamName, ADBEVideoCodec) == 0 || 
		strcmp(changedParamName, ADBEAudioCodec) == 0)
	{
		// Get the conderInfos
		vector<EncoderInfo> encoderInfos;
		if (strcmp(changedParamName, ADBEVideoCodec) == 0)
		{
			encoderInfos = settings->videoEncoderInfos;
		}
		else if (strcmp(changedParamName, ADBEAudioCodec) == 0)
		{
			encoderInfos = settings->audioEncoderInfos;
		}

		// Iterate the encoders
		for (EncoderInfo encoderInfo: encoderInfos)
		{
			// Should these options be hidden?
			const bool visible = encoderInfo.id == changedValue.value.intValue;

			for (ParameterInfo parameterInfo: encoderInfo.params)
			{
				// Change the visibility of an element
				instRec->exportParamSuite->GetParamValue(exID, validateParamChangedRecP->multiGroupIndex, parameterInfo.name.c_str(), &tempValue);
				tempValue.hidden = visible ? kPrFalse : kPrTrue;
				instRec->exportParamSuite->ChangeParam(exID, validateParamChangedRecP->multiGroupIndex, parameterInfo.name.c_str(), &tempValue);

				// Is this a dropdown element?
				if (parameterInfo.values.size() > 0)
				{
					const int selectedId = tempValue.value.intValue;

					// Handle the elements suboptions
					for (ParameterValueInfo parameterValueInfo: parameterInfo.values)
					{
						// Should this suboption be visible
						const bool isValueSelected = visible && parameterValueInfo.id == selectedId;

						// Iterate these suboptions
						for (ParameterSubValue parameterSubValue: parameterValueInfo.subValues)
						{
							// Change the elements visibility
							instRec->exportParamSuite->GetParamValue(exID, validateParamChangedRecP->multiGroupIndex, parameterSubValue.name.c_str(), &tempValue);
							tempValue.hidden = isValueSelected ? kPrFalse : kPrTrue;
							instRec->exportParamSuite->ChangeParam(exID, validateParamChangedRecP->multiGroupIndex, parameterSubValue.name.c_str(), &tempValue);
						}
					}
				}
			}
		}
	}
	else if (strlen(changedParamName) > 0)
	{
		vector<ParameterValueInfo> parameterValueInfos;

		// Merge video and audio encoders
		vector<EncoderInfo> encoderInfos;
		encoderInfos.insert(encoderInfos.end(), settings->videoEncoderInfos.begin(), settings->videoEncoderInfos.end());
		encoderInfos.insert(encoderInfos.end(), settings->audioEncoderInfos.begin(), settings->audioEncoderInfos.end());

		// Find the changed parameter
		for (EncoderInfo encoderInfo : encoderInfos)
		{
			for (ParameterInfo parameterInfo : encoderInfo.params)
			{
				if (parameterInfo.name == changedParamName)
				{
					parameterValueInfos = parameterInfo.values;
					goto processValues;
				}

				for (ParameterValueInfo parameterValueInfo : parameterInfo.values)
				{
					for (ParameterSubValue parameterSubValue : parameterValueInfo.subValues)
					{
						if (parameterSubValue.name == changedParamName)
						{
							return result;
						}
					}
				}
			}
		}

processValues:

		// Iterate these values
		for (ParameterValueInfo parameterValueInfo : parameterValueInfos)
		{
			if (parameterValueInfo.subValues.size() > 0)
			{
				// Should this suboption be visible
				const bool isSelected = parameterValueInfo.id == changedValue.value.intValue;

				// Iterate these suboptions
				for (ParameterSubValue parameterSubValue : parameterValueInfo.subValues)
				{
					// Change the elements visibility
					instRec->exportParamSuite->GetParamValue(exID, validateParamChangedRecP->multiGroupIndex, parameterSubValue.name.c_str(), &tempValue);
					tempValue.hidden = isSelected ? kPrFalse : kPrTrue;
					instRec->exportParamSuite->ChangeParam(exID, validateParamChangedRecP->multiGroupIndex, parameterSubValue.name.c_str(), &tempValue);
				}
			}
		}
	}
	*/

	return result;
}

// reviewed 0.5.3
prMALError exGetParamSummary(exportStdParms *stdParmsP, exParamSummaryRec *summaryRecP)
{
	InstanceRec *instRec = reinterpret_cast<InstanceRec *>(summaryRecP->privateData);
	Settings *settings = instRec->settings;
	
#pragma region Video Summary

	exParamValues videoCodec;
	instRec->exportParamSuite->GetParamValue(summaryRecP->exporterPluginID, 0, ADBEVideoCodec, &videoCodec);

	// Find the encoder (and fallback to first one)
	vector<EncoderInfo> videoEncoderInfos = settings->videoEncoderInfos;
	int videoEncoderIdx = FindVectorIndexById(&videoEncoderInfos, videoCodec.value.intValue, 0);
	EncoderInfo videoEncoderInfo = videoEncoderInfos.at(videoEncoderIdx);

	// Create config
	EncoderConfig videoEncoderConfig = EncoderConfig(instRec->exportParamSuite, summaryRecP->exporterPluginID);
	videoEncoderConfig.initFromSettings(&videoEncoderInfo);

	//Create summary string
	string videoSummary = videoEncoderInfo.name + " (" + videoEncoderConfig.getConfigAsParamString("-") + ")";
	prUTF16CharCopy(summaryRecP->videoSummary, string2wchar_t(videoSummary));

#pragma endregion

#pragma region Audio Summary

	exParamValues audioCodec;
	instRec->exportParamSuite->GetParamValue(summaryRecP->exporterPluginID, 0, ADBEAudioCodec, &audioCodec);

	// Find the encoder (and fallback to first one)
	vector<EncoderInfo> audioEncoderInfos = settings->audioEncoderInfos;
	int audioEncoderIdx = FindVectorIndexById(&audioEncoderInfos, audioCodec.value.intValue, 0);
	EncoderInfo audioEncoderInfo = audioEncoderInfos.at(audioEncoderIdx);

	// Create config
	EncoderConfig *audioEncoderConfig = new EncoderConfig(instRec->exportParamSuite, summaryRecP->exporterPluginID);
	audioEncoderConfig->initFromSettings(&audioEncoderInfo);

	//Create summary string
	string audioSummary = audioEncoderInfo.name + " (" + audioEncoderConfig->getConfigAsParamString("-") + ")";
	prUTF16CharCopy(summaryRecP->audioSummary, string2wchar_t(audioSummary));

#pragma endregion

	// No bitrate summary yet
	prUTF16CharCopy(summaryRecP->bitrateSummary, L"");

	return malNoError;
}

// reviewed 0.5.0
prMALError exValidateOutputSettings(exportStdParms *stdParmsP, exValidateOutputSettingsRec *validateOutputSettingsRec)
{
	prMALError result = malNoError;
	/*
	csSDK_uint32 exID = validateOutputSettingsRec->exporterPluginID;
	InstanceRec *instRec = reinterpret_cast<InstanceRec *>(validateOutputSettingsRec->privateData);
	Settings *settings = instRec->settings;

	// Get selected video encoder
	exParamValues videoCodec, audioCodec, multiplexer;
	instRec->exportParamSuite->GetParamValue(exID, 0, ADBEVideoCodec, &videoCodec);
	instRec->exportParamSuite->GetParamValue(exID, 0, ADBEAudioCodec, &audioCodec);
	instRec->exportParamSuite->GetParamValue(exID, 0, FFMultiplexer, &multiplexer);

#pragma region Create video encoder info

	// Get the selected encoder
	const vector<EncoderInfo> videoEncoderInfos = settings->getEncoders(EncoderType::VIDEO);
	EncoderInfo videoEncoderInfo = FilterTypeVectorById(videoEncoderInfos, videoCodec.value.intValue);
	if (videoEncoderInfo.id == -1)
	{
		return malUnknownError;
	}

	// Create config
	EncoderConfig *videoEncoderConfig = new EncoderConfig(instRec->exportParamSuite, exID);
	videoEncoderConfig->initFromSettings(&videoEncoderInfo);

#pragma endregion

#pragma region Create audio encoder info

	// Get the selected encoder
	const vector<EncoderInfo> encoderInfos = settings->getEncoders(EncoderType::AUDIO);
	EncoderInfo audioEncoderInfo = FilterTypeVectorById(encoderInfos, audioCodec.value.intValue);
	if (audioEncoderInfo.id == -1)
	{
		return malUnknownError;
	}

	// Create config
	EncoderConfig *audioEncoderConfig = new EncoderConfig(instRec->exportParamSuite, exID);
	audioEncoderConfig->initFromSettings(&audioEncoderInfo);

#pragma endregion

#pragma region Create muxer info

	// Get selected muxer
	vector<MuxerInfo> muxerInfos = settings->getMuxers();
	MuxerInfo muxerInfo = FilterTypeVectorById(muxerInfos, multiplexer.value.intValue);
	if (muxerInfo.id == -1)
	{
		return malUnknownError;
	}

#pragma endregion

	// Create encoder instance
	Encoder *encoder = new Encoder(muxerInfo.name.c_str(), NULL);
	result = SetupEncoderInstance(instRec, exID, encoder, videoEncoderConfig, audioEncoderConfig);

	// Open the encoder
	if (encoder->open() == S_OK)
	{
		// Test successful
		encoder->close(false);
	}
	else
	{
		string errmsg = "";

		// Add an error text fo most common errors
		if (muxerInfo.name == "mp4" &&
			audioEncoderInfo.name == "flac")
		{
			errmsg = "Error: The FLAC codec can not be used in a mp4 container.\n\n";
		}

		// Build the error message string
		char buffer[4096];
		sprintf_s(buffer, "This configuration is not supported by FFMpeg:\n\n%s- Multiplexer: %s\n- Video encoder: %s (%s)\n- Audio encoder: %s (%s)\n\n(Press CTRL+C to copy this information to the clipboard.)",
			errmsg.c_str(),
			muxerInfo.name.c_str(),
			videoEncoderInfo.name.c_str(),
			videoEncoderConfig->getConfigAsParamString("-").c_str(),
			audioEncoderInfo.name.c_str(),
			audioEncoderConfig->getConfigAsParamString("-").c_str());

		// Show an error message to the user
		ShowMessageBox(instRec, buffer, "Error", MB_OK);

		result = exportReturn_ErrLastErrorSet;
	}

	delete(encoder);
	delete(videoEncoderConfig);
	delete(audioEncoderConfig);

	*/

	return result;
}

// reviewed 0.5.2
prMALError SetupEncoderInstance(InstanceRec *instRec, csSDK_uint32 exID, Encoder *encoder, EncoderConfig *videoConfig, EncoderConfig *audioConfig)
{
	prMALError result = malNoError;

	Settings *settings = instRec->settings;

	// Get export video params
	exParamValues videoWidth, videoHeight, tvStandard, vkdrColorSpace, vkdrColorRange, ticksPerFrame, channelType, audioSampleRate;
	instRec->exportParamSuite->GetParamValue(exID, 0, ADBEVideoWidth, &videoWidth);
	instRec->exportParamSuite->GetParamValue(exID, 0, ADBEVideoHeight, &videoHeight);
	instRec->exportParamSuite->GetParamValue(exID, 0, VKDRTVStandard, &tvStandard);
	instRec->exportParamSuite->GetParamValue(exID, 0, ADBEVideoFPS, &ticksPerFrame);
	instRec->exportParamSuite->GetParamValue(exID, 0, VKDRColorSpace, &vkdrColorSpace);
	instRec->exportParamSuite->GetParamValue(exID, 0, VKDRColorRange, &vkdrColorRange);
	instRec->exportParamSuite->GetParamValue(exID, 0, ADBEAudioNumChannels, &channelType);
	instRec->exportParamSuite->GetParamValue(exID, 0, ADBEAudioRatePerSecond, &audioSampleRate);

	// Find the correct fps ratio
	PrTime c = gcd(254016000000, ticksPerFrame.value.timeValue);
	PrTime num = 254016000000 / c;
	PrTime den = ticksPerFrame.value.timeValue / c;

	// Create audio context information
	EncoderContextInfo audioContextInfo;
	audioContextInfo.name = audioConfig->encoderInfo->name;
	audioContextInfo.timebase.den = (int)audioSampleRate.value.floatValue;
	audioContextInfo.timebase.num = 1;

	// Translate the channel layout to AVLib
	switch (channelType.value.intValue)
	{
	case kPrAudioChannelType_Mono:
		audioContextInfo.channelLayout = AV_CH_LAYOUT_MONO;
		break;

	case kPrAudioChannelType_51:
		audioContextInfo.channelLayout = AV_CH_LAYOUT_5POINT1_BACK;
		break;

	default:
		audioContextInfo.channelLayout = AV_CH_LAYOUT_STEREO;
		break;
	}
	
	// Set and configure audio codec
	encoder->audioContext->setCodec(audioContextInfo, audioConfig);
	
	// Create video context information
	EncoderContextInfo videoContextInfo;
	videoContextInfo.name = videoConfig->encoderInfo->name;
	videoContextInfo.width = videoWidth.value.intValue;
	videoContextInfo.height = videoHeight.value.intValue;
	videoContextInfo.timebase = {(int)den, (int)num };
	
	// Get the right color range
	videoContextInfo.colorRange = vkdrColorRange.value.intValue == vkdrFullColorRange ? AVColorRange::AVCOL_RANGE_JPEG : AVColorRange::AVCOL_RANGE_MPEG;

	// Color conversion values
	if (vkdrColorSpace.value.intValue == vkdrBT601)
	{
		if (tvStandard.value.intValue == vkdrPAL)
		{
			videoContextInfo.colorSpace = AVColorSpace::AVCOL_SPC_BT470BG;
			videoContextInfo.colorPrimaries = AVColorPrimaries::AVCOL_PRI_BT470BG; 
			videoContextInfo.colorTRC = AVColorTransferCharacteristic::AVCOL_TRC_GAMMA28;
		}
		else if (tvStandard.value.intValue == vkdrNTSC)
		{
			videoContextInfo.colorSpace = AVColorSpace::AVCOL_SPC_SMPTE170M;
			videoContextInfo.colorPrimaries = AVColorPrimaries::AVCOL_PRI_SMPTE170M;
			videoContextInfo.colorTRC = AVColorTransferCharacteristic::AVCOL_TRC_SMPTE170M;
		}
	}
	else if (vkdrColorSpace.value.intValue == vkdrBT709)
	{
		videoContextInfo.colorSpace = AVColorSpace::AVCOL_SPC_BT709;
		videoContextInfo.colorPrimaries = AVColorPrimaries::AVCOL_PRI_BT709;
		videoContextInfo.colorTRC = AVColorTransferCharacteristic::AVCOL_TRC_BT709;
	}
	
	// Set and configure video codec
	encoder->videoContext->setCodec(videoContextInfo, videoConfig);
	
	return result;
}

// reviewed 0.5.2
prMALError exExport(exportStdParms *stdParmsP, exDoExportRec *exportInfoP)
{
	prMALError result = malNoError;

	csSDK_uint32 exID = exportInfoP->exporterPluginID;
	InstanceRec *instRec = reinterpret_cast<InstanceRec *>(exportInfoP->privateData);
	Settings *settings = instRec->settings;

#pragma region Get filename

	// Get export filename
	prUTF16Char prFilename[kPrMaxPath];
	csSDK_int32 prFilenameLength = kPrMaxPath;
	instRec->exportFileSuite->GetPlatformPath(exportInfoP->fileObject, &prFilenameLength, prFilename);
	
	size_t i;
	char *filename = (char*)malloc(kPrMaxPath);
	wcstombs_s(&i, filename, (size_t)kPrMaxPath, prFilename, (size_t)kPrMaxPath);

#pragma endregion

#pragma region Create video encoder config

	// Get selected video encoder
	exParamValues videoCodec;
	instRec->exportParamSuite->GetParamValue(exID, 0, ADBEVideoCodec, &videoCodec);

	// Get the selected encoder
	vector<EncoderInfo> videoEncoderInfos = settings->videoEncoderInfos;
	const int videoEncoderIdx = FindVectorIndexById(&videoEncoderInfos, videoCodec.value.intValue, 0);
	EncoderInfo videoEncoderInfo = videoEncoderInfos.at(videoEncoderIdx);

	// Create config
	EncoderConfig videoEncoderConfig = EncoderConfig(instRec->exportParamSuite, exID);
	videoEncoderConfig.initFromSettings(&videoEncoderInfo);

#pragma endregion

#pragma region Create audio encoder config

	// Get selected audio encoder
	exParamValues audioCodec;
	instRec->exportParamSuite->GetParamValue(exID, 0, ADBEAudioCodec, &audioCodec);

	// Get the selected encoder
	vector<EncoderInfo> encoderInfos = settings->audioEncoderInfos;
	const int audioEncoderIdx = FindVectorIndexById(&encoderInfos, audioCodec.value.intValue, 0);
	EncoderInfo audioEncoderInfo = encoderInfos.at(audioEncoderIdx);

	// Create config
	EncoderConfig audioEncoderConfig = EncoderConfig(instRec->exportParamSuite, exID);
	audioEncoderConfig.initFromSettings(&audioEncoderInfo);

#pragma endregion

	// Get render parameter values
	exParamValues videoWidth, videoHeight, ticksPerFrame, colorSpace, channelType, audioSampleRate;
	instRec->exportParamSuite->GetParamValue(exID, 0, ADBEVideoWidth, &videoWidth);
	instRec->exportParamSuite->GetParamValue(exID, 0, ADBEVideoHeight, &videoHeight);
	instRec->exportParamSuite->GetParamValue(exID, 0, ADBEVideoFPS, &ticksPerFrame);
	instRec->exportParamSuite->GetParamValue(exID, 0, VKDRColorSpace, &colorSpace);
	instRec->exportParamSuite->GetParamValue(exID, 0, ADBEAudioNumChannels, &channelType);
	instRec->exportParamSuite->GetParamValue(exID, 0, ADBEAudioRatePerSecond, &audioSampleRate);

	PrTime ticksPerSample;
	instRec->timeSuite->GetTicksPerAudioSample((float)audioSampleRate.value.floatValue, &ticksPerSample);

	// Make audio renderer
	csSDK_uint32 audioRendererID;
	instRec->sequenceAudioSuite->MakeAudioRenderer(exID, exportInfoP->startTime, (PrAudioChannelType)channelType.value.intValue, 
		kPrAudioSampleType_32BitFloat, (float)audioSampleRate.value.floatValue, &audioRendererID);
	instRec->sequenceAudioSuite->ResetAudioToBeginning(audioRendererID);

	// Get the audio frame size we need to send to the encoder
	csSDK_int32 maxBlip;
	instRec->sequenceAudioSuite->GetMaxBlip(audioRendererID, ticksPerFrame.value.timeValue, &maxBlip);

	// Allocate audio output buffer
	float *audioBuffer[MAX_AUDIO_CHANNELS];
	for (int i = 0; i < MAX_AUDIO_CHANNELS; i++)
	{
		audioBuffer[i] = (float *)instRec->memorySuite->NewPtr(sizeof(float) * maxBlip);
	}

	PrTime audioSamplesLeft = (exportInfoP->endTime - exportInfoP->startTime) / ticksPerSample;
	csSDK_int32 chunk;

	// Create renderer instance
	VideoRenderer *videoRenderer = new VideoRenderer(exID, instRec->ppixSuite,
		instRec->ppix2Suite, instRec->memorySuite, instRec->exporterUtilitySuite, instRec->imageProcessingSuite);

	int currentPass = 0;
	int maxPasses = videoEncoderConfig.getMaxPasses();

	// Create encoder instance
	Encoder encoder = Encoder(NULL, filename);

	result = SetupEncoderInstance(instRec, exID, &encoder, &videoEncoderConfig, &audioEncoderConfig);

	// Start the rendering loop
	result = videoRenderer->render(videoWidth.value.intValue, videoHeight.value.intValue, 
		colorSpace.value.intValue == vkdrBT601 ? ColorSpace::bt601 : ColorSpace::bt709, 
		exportInfoP->startTime, exportInfoP->endTime, maxPasses, [&](EncodingData *encodingData)
	{
		// Handle multiple passes
		if (currentPass == 0 || (maxPasses > 1 && encodingData->pass > currentPass))
		{
			// Close current encoder instance
			if (currentPass > 0)
			{
				encoder.close(true);
			}

			// Start next pass
			currentPass = encodingData->pass;

			// Multipass encoding
			if (maxPasses > 1)
			{
				if (currentPass == 1)
				{
					encoder.videoContext->setCodecFlags(AV_CODEC_FLAG_PASS1);
				}
				else
				{
					encoder.videoContext->setCodecFlags(AV_CODEC_FLAG_PASS2);
				}
			}

			// Open the encoder
			if (encoder.open() != S_OK)
			{
				return false;
			}
		}

		// Encode and write the rendered video frame
		if (encoder.writeVideoFrame(encodingData) != S_OK)
		{
			return false;
		}

		// Write all audio samples for that video frame
		while (encoder.getNextFrameType() == FrameType::AudioFrame && audioSamplesLeft > 0)
		{
			// Set chunk size
			if (audioSamplesLeft > maxBlip)
			{
				chunk = maxBlip;
			}
			else
			{
				chunk = (csSDK_int32)audioSamplesLeft;
			}

			// Get the sample data
			result = instRec->sequenceAudioSuite->GetAudio(audioRendererID, chunk, audioBuffer, kPrFalse);

			// Send raw data to the encoder
			if (encoder.writeAudioFrame((const uint8_t**)audioBuffer, chunk) != S_OK)
			{
				return false;
			}

			audioSamplesLeft -= chunk;

			// Free audio buffers when not needed anymore
			if (audioSamplesLeft <= 0)
			{
				for (int i = 0; i < MAX_AUDIO_CHANNELS; i++)
				{
					instRec->memorySuite->PrDisposePtr((char *)audioBuffer[i]);
				}
			}
		}

		return true;
	});

	// TODO: call this once both audio and video is flushed and finished!

	// Close encoder and free memory
	encoder.close(result == suiteError_NoError);

	return result;
}