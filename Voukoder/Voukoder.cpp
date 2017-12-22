#include <windows.h>
#include "Encoder.h"
#include "Decoder.h"
#include "Voukoder.h"
#include "Common.h"
#include "Converter.h"
#include "InstructionSet.h"

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
			spError = spBasic->AcquireSuite(kPrSDKSequenceRenderSuite, kPrSDKSequenceRenderSuiteVersion, const_cast<const void**>(reinterpret_cast<void**>(&(instRec->sequenceRenderSuite))));
			spError = spBasic->AcquireSuite(kPrSDKExportFileSuite, kPrSDKExportFileSuiteVersion, const_cast<const void**>(reinterpret_cast<void**>(&(instRec->exportFileSuite))));
			spError = spBasic->AcquireSuite(kPrSDKPPixSuite, kPrSDKPPixSuiteVersion, const_cast<const void**>(reinterpret_cast<void**>(&(instRec->ppixSuite))));
			spError = spBasic->AcquireSuite(kPrSDKPPix2Suite, kPrSDKPPix2SuiteVersion, const_cast<const void**>(reinterpret_cast<void**>(&(instRec->ppix2Suite))));
			spError = spBasic->AcquireSuite(kPrSDKExportProgressSuite, kPrSDKExportProgressSuiteVersion, const_cast<const void**>(reinterpret_cast<void**>(&(instRec->exportProgressSuite))));
			spError = spBasic->AcquireSuite(kPrSDKWindowSuite, kPrSDKWindowSuiteVersion, const_cast<const void**>(reinterpret_cast<void**>(&(instRec->windowSuite))));
			spError = spBasic->AcquireSuite(kPrSDKExporterUtilitySuite, kPrSDKExporterUtilitySuiteVersion, const_cast<const void**>(reinterpret_cast<void**>(&(instRec->exporterUtilitySuite))));

			// Get the DLL module handle
			MEMORY_BASIC_INFORMATION mbi;
			static int dummy;
			VirtualQuery(&dummy, &mbi, sizeof(mbi));
			instRec->hInstance = reinterpret_cast<HMODULE>(mbi.AllocationBase);

			instRec->settings = new Settings(instRec->hInstance);
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

		if (instRec->sequenceRenderSuite)
		{
			result = spBasic->ReleaseSuite(kPrSDKSequenceRenderSuite, kPrSDKSequenceRenderSuiteVersion);
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

		if (instRec->memorySuite)
		{
			instRec->memorySuite->PrDisposePtr(reinterpret_cast<PrMemoryPtr>(instRec));
			result = spBasic->ReleaseSuite(kPrSDKMemoryManagerSuite, kPrSDKMemoryManagerSuiteVersion);
		}
		
		instRec->settings->~Settings();
		instRec->settings = NULL;
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

	// Get selected muxer
	vector<MuxerInfo> muxerInfos = settings->getMuxers();
	MuxerInfo muxerInfo = FilterTypeVectorById(muxerInfos, multiplexer.value.intValue);

	// Is the muxer known?
	if (muxerInfo.id > -1)
	{
		const std::wstring widestr = std::wstring(muxerInfo.extension.begin(), muxerInfo.extension.end());
		prUTF16CharCopy(exportFileExtensionRecP->outFileExtension, widestr.c_str());

		return malNoError;
	}

	return malUnknownError;
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
// TODO: Cleanup loop
prMALError exExport(exportStdParms *stdParmsP, exDoExportRec *exportInfoP)
{
	prMALError result = malNoError;

	csSDK_uint32 exID = exportInfoP->exporterPluginID;
	InstanceRec *instRec = reinterpret_cast<InstanceRec *>(exportInfoP->privateData);
	Settings *settings = instRec->settings;

#pragma region Get filename

	// Get the file name 
	prUTF16Char prFilename[kPrMaxPath];
	csSDK_int32 prFilenameLength = kPrMaxPath;
	size_t i;
	instRec->exportFileSuite->GetPlatformPath(exportInfoP->fileObject, &prFilenameLength, prFilename);
	char *filename = (char*)malloc(kPrMaxPath);
	wcstombs_s(&i, filename, (size_t)kPrMaxPath, prFilename, (size_t)kPrMaxPath);

#pragma endregion

#pragma region Create video encoder config

	// Get selected video encoder
	exParamValues videoCodec;
	instRec->exportParamSuite->GetParamValue(exID, 0, ADBEVideoCodec, &videoCodec);

	// Get the selected encoder
	const vector<EncoderInfo> videoEncoderInfos = settings->getEncoders(EncoderType::VIDEO);
	EncoderInfo videoEncoderInfo = FilterTypeVectorById(videoEncoderInfos, videoCodec.value.intValue);

	// Create config
	EncoderConfig *videoEncoderConfig = new EncoderConfig(instRec->exportParamSuite, exID);
	videoEncoderConfig->initFromSettings(&videoEncoderInfo);

#pragma endregion

#pragma region Create audio encoder config

	// Get selected audio encoder
	exParamValues audioCodec;
	instRec->exportParamSuite->GetParamValue(exID, 0, ADBEAudioCodec, &audioCodec);

	// Get the selected encoder
	const vector<EncoderInfo> encoderInfos = settings->getEncoders(EncoderType::AUDIO);
	EncoderInfo audioEncoderInfo = FilterTypeVectorById(encoderInfos, audioCodec.value.intValue);

	// Create config
	EncoderConfig *audioEncoderConfig = new EncoderConfig(instRec->exportParamSuite, exID);
	audioEncoderConfig->initFromSettings(&audioEncoderInfo);

#pragma endregion

	const csSDK_int32 maxPasses = videoEncoderConfig->getMaxPasses();

	// Do encoding loops
	for (csSDK_int32 pass = 1; pass <= maxPasses; pass++)
	{
		// Create encoder instance
		Encoder *encoder = new Encoder(NULL, filename);

		result = SetupEncoderInstance(instRec, exID, encoder, videoEncoderConfig, audioEncoderConfig);

		// Multipass encoding
		if (maxPasses > 1)
		{
			if (pass == 1)
			{
				encoder->videoContext->setCodecFlags(AV_CODEC_FLAG_PASS1);
			}
			else
			{
				encoder->videoContext->setCodecFlags(AV_CODEC_FLAG_PASS2);
			}
		}
		
		// Open the encoder
		if (encoder->open() == S_OK)
		{
			// Render and write all video and audio frames
			result = RenderAndWriteAllFrames(exportInfoP, encoder, pass, maxPasses);

			encoder->close(true);
		}

		encoder->~Encoder();
		encoder = NULL;
	}


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
	vector<EncoderInfo> encoderInfos = settings->getEncoders(EncoderType::VIDEO);
	CreateEncoderParamElements(exportParamSuite, exID, groupIndex, encoderInfos, settings->defaultVideoEncoder);

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
	encoderInfos = settings->getEncoders(EncoderType::AUDIO);
	CreateEncoderParamElements(exportParamSuite, exID, groupIndex, encoderInfos, audioCodecValues.value.intValue);

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
	json configuration = settings->getConfiguration();

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
	vector<EncoderInfo> videoEncoderInfos = settings->getEncoders(EncoderType::VIDEO);
	PopulateEncoders(instRec, exID, groupIndex, ADBEVideoCodec, videoEncoderInfos);

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

	PopulateParamValues(instRec, exID, groupIndex, videoEncoderInfos);

#pragma endregion

#pragma region Group: Audio settings

	// Label elements
	instRec->exportParamSuite->SetParamName(exID, groupIndex, ADBEBasicAudioGroup, L"Audio settings");
	instRec->exportParamSuite->SetParamName(exID, groupIndex, ADBEAudioCodec, L"Audio Encoder");
	instRec->exportParamSuite->SetParamName(exID, groupIndex, ADBEAudioRatePerSecond, L"Sample Rate");
	instRec->exportParamSuite->SetParamName(exID, groupIndex, ADBEAudioNumChannels, L"Channels");

	// Populate audio encoder dropdown
	vector<EncoderInfo> audioEncoderInfos = settings->getEncoders(EncoderType::AUDIO);
	PopulateEncoders(instRec, exID, groupIndex, ADBEAudioCodec, audioEncoderInfos);

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
	PopulateParamValues(instRec, exID, groupIndex, audioEncoderInfos);

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
	vector<MuxerInfo> muxerInfos = settings->getMuxers();
	for (MuxerInfo const muxerInfo : muxerInfos)
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
			encoderInfos = settings->getEncoders(EncoderType::VIDEO);
		}
		else if (strcmp(changedParamName, ADBEAudioCodec) == 0)
		{
			encoderInfos = settings->getEncoders(EncoderType::AUDIO);
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
		vector<EncoderInfo> encoderInfos = settings->getEncoders(EncoderType::VIDEO);
		vector<EncoderInfo> audioEncoderInfos = settings->getEncoders(EncoderType::AUDIO);
		encoderInfos.insert(encoderInfos.end(), audioEncoderInfos.begin(), audioEncoderInfos.end());

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

	return result;
}

// reviewed 0.3.8
prMALError exGetParamSummary(exportStdParms *stdParmsP, exParamSummaryRec *summaryRecP)
{
	InstanceRec *instRec = reinterpret_cast<InstanceRec *>(summaryRecP->privateData);
	Settings *settings = instRec->settings;
	
#pragma region Video Summary

	exParamValues videoCodec;
	instRec->exportParamSuite->GetParamValue(summaryRecP->exporterPluginID, 0, ADBEVideoCodec, &videoCodec);

	// Init video encoder configuration
	vector<EncoderInfo> encoderInfos = settings->getEncoders(EncoderType::VIDEO);
	EncoderInfo encoderInfo = FilterTypeVectorById(encoderInfos, videoCodec.value.intValue);
	if (encoderInfo.id > -1)
	{
		// Create config
		EncoderConfig *videoEncoderConfig = new EncoderConfig(instRec->exportParamSuite, summaryRecP->exporterPluginID);
		videoEncoderConfig->initFromSettings(&encoderInfo);

		//Create summary string
		string videoSummary = encoderInfo.name + " (" + videoEncoderConfig->getConfigAsParamString("-") + ")";
		prUTF16CharCopy(summaryRecP->videoSummary, string2wchar_t(videoSummary));
	}

#pragma endregion

#pragma region Audio Summary

	exParamValues audioCodec;
	instRec->exportParamSuite->GetParamValue(summaryRecP->exporterPluginID, 0, ADBEAudioCodec, &audioCodec);

	// Init audio encoder configuration
	encoderInfos = settings->getEncoders(EncoderType::AUDIO);
	encoderInfo = FilterTypeVectorById(encoderInfos, audioCodec.value.intValue);
	if (encoderInfo.id > -1)
	{
		// Create config
		EncoderConfig *audioEncoderConfig = new EncoderConfig(instRec->exportParamSuite, summaryRecP->exporterPluginID);
		audioEncoderConfig->initFromSettings(&encoderInfo);

		//Create summary string
		string audioSummary = encoderInfo.name + " (" + audioEncoderConfig->getConfigAsParamString("-") + ")";
		prUTF16CharCopy(summaryRecP->audioSummary, string2wchar_t(audioSummary));
	}

#pragma endregion

	// No bitrate summary yet
	prUTF16CharCopy(summaryRecP->bitrateSummary, L"");

	return malNoError;
}

// reviewed 0.4.1
prMALError exValidateOutputSettings(exportStdParms *stdParmsP, exValidateOutputSettingsRec *validateOutputSettingsRec)
{
	prMALError result = malNoError;

	csSDK_uint32 exID = validateOutputSettingsRec->exporterPluginID;
	InstanceRec *instRec = reinterpret_cast<InstanceRec *>(validateOutputSettingsRec->privateData);
	Settings *settings = instRec->settings;

#pragma region Create video encoder info

	// Get selected video encoder
	exParamValues videoCodec;
	instRec->exportParamSuite->GetParamValue(exID, 0, ADBEVideoCodec, &videoCodec);

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

	// Get selected audio encoder
	exParamValues audioCodec;
	instRec->exportParamSuite->GetParamValue(exID, 0, ADBEAudioCodec, &audioCodec);

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

	// Get the users multiplexer choice
	exParamValues multiplexer;
	instRec->exportParamSuite->GetParamValue(exID, 0, FFMultiplexer, &multiplexer);

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

	encoder->~Encoder();
	encoder = NULL;

	return result;
}

// reviewed 0.3.8
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
	int num = 254016000000 / c;
	int den = ticksPerFrame.value.timeValue / c;

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
		audioContextInfo.channelLayout = AV_CH_LAYOUT_5POINT1;
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
	videoContextInfo.timebase = { den, num };
	
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

// reviewed 0.3.8
prMALError RenderAndWriteAllFrames(exDoExportRec *exportInfoP, Encoder *encoder, csSDK_int32 pass, csSDK_int32 maxPasses)
{
	prMALError result = malNoError;
	csSDK_uint32 exID = exportInfoP->exporterPluginID;
	InstanceRec *instRec = reinterpret_cast<InstanceRec *>(exportInfoP->privateData);

	// Get render parameter values
	exParamValues videoWidth, videoHeight, pixelAspectRatio, fieldType, colorSpace, colorRange, ticksPerFrame, channelType, audioSampleRate;
	instRec->exportParamSuite->GetParamValue(exID, 0, ADBEVideoWidth, &videoWidth);
	instRec->exportParamSuite->GetParamValue(exID, 0, ADBEVideoHeight, &videoHeight);
	instRec->exportParamSuite->GetParamValue(exID, 0, ADBEVideoAspect, &pixelAspectRatio);
	instRec->exportParamSuite->GetParamValue(exID, 0, ADBEVideoFieldType, &fieldType);
	instRec->exportParamSuite->GetParamValue(exID, 0, ADBEVideoFPS, &ticksPerFrame);
	instRec->exportParamSuite->GetParamValue(exID, 0, VKDRColorSpace, &colorSpace);
	instRec->exportParamSuite->GetParamValue(exID, 0, VKDRColorRange, &colorRange);
	instRec->exportParamSuite->GetParamValue(exID, 0, ADBEAudioNumChannels, &channelType);
	instRec->exportParamSuite->GetParamValue(exID, 0, ADBEAudioRatePerSecond, &audioSampleRate);

	// Make audio renderer
	PrTime ticksPerSample;
	instRec->timeSuite->GetTicksPerAudioSample(audioSampleRate.value.floatValue, &ticksPerSample);
	instRec->sequenceAudioSuite->MakeAudioRenderer(exID, exportInfoP->startTime, (PrAudioChannelType)channelType.value.intValue, kPrAudioSampleType_32BitFloat, audioSampleRate.value.floatValue, &instRec->audioRenderID);
	instRec->sequenceAudioSuite->ResetAudioToBeginning(instRec->audioRenderID);

	// Get the audio frame size we need to send to the encoder
	instRec->sequenceAudioSuite->GetMaxBlip(instRec->audioRenderID, ticksPerFrame.value.timeValue, &instRec->maxBlip);

	PrTime exportDuration = exportInfoP->endTime - exportInfoP->startTime;
	PrTime audioSamplesLeft = exportDuration / ticksPerSample;

	// Define the render params
	SequenceRender_ParamsRec renderParms;

	// Find the right pixel format
	const PrPixelFormat pixelFormats[] = {
		PrPixelFormat_YUV_420_MPEG4_FRAME_PICTURE_PLANAR_8u_709,
		PrPixelFormat_YUV_420_MPEG4_FRAME_PICTURE_PLANAR_8u_709_FullRange,
		PrPixelFormat_YUV_420_MPEG4_FIELD_PICTURE_PLANAR_8u_709,
		PrPixelFormat_YUV_420_MPEG4_FIELD_PICTURE_PLANAR_8u_709_FullRange,
		PrPixelFormat_YUV_420_MPEG4_FRAME_PICTURE_PLANAR_8u_601,
		PrPixelFormat_YUV_420_MPEG4_FRAME_PICTURE_PLANAR_8u_601_FullRange,
		PrPixelFormat_YUV_420_MPEG4_FIELD_PICTURE_PLANAR_8u_601,
		PrPixelFormat_YUV_420_MPEG4_FIELD_PICTURE_PLANAR_8u_601_FullRange,
		PrPixelFormat_UYVY_422_8u_709,
		PrPixelFormat_UYVY_422_8u_601,
		PrPixelFormat_YUYV_422_8u_709,
		PrPixelFormat_YUYV_422_8u_601,
//		PrPixelFormat_V210_422_10u_709,
//		PrPixelFormat_V210_422_10u_601,
		PrPixelFormat_VUYA_4444_8u_709,
		PrPixelFormat_VUYA_4444_8u,
		PrPixelFormat_VUYA_4444_32f_709,
		PrPixelFormat_VUYA_4444_32f,
		PrPixelFormat_BGRA_4444_8u
	};
	renderParms.inRequestedPixelFormatArray = pixelFormats;
	renderParms.inRequestedPixelFormatArrayCount = sizeof(pixelFormats) / sizeof(pixelFormats[0]);
	renderParms.inWidth = videoWidth.value.intValue;
	renderParms.inHeight = videoHeight.value.intValue;
	renderParms.inPixelAspectRatioNumerator = pixelAspectRatio.value.ratioValue.numerator;
	renderParms.inPixelAspectRatioDenominator = pixelAspectRatio.value.ratioValue.denominator;
	renderParms.inRenderQuality = kPrRenderQuality_Max;
	renderParms.inFieldType = fieldType.value.intValue;
	renderParms.inDeinterlace = kPrFalse;
	renderParms.inDeinterlaceQuality = kPrRenderQuality_High;
	renderParms.inCompositeOnBlack = kPrFalse;

	// Make video renderer
	instRec->sequenceRenderSuite->MakeVideoRenderer(exID, &instRec->videoRenderID, ticksPerFrame.value.timeValue);

	// Cache the rendered frames when using multipass
	const PrRenderCacheType cacheType = maxPasses > 1 ? kRenderCacheType_RenderedStillFrames : kRenderCacheType_None;

	// Set colorspace and color range
	stringstream rgbScale;
	rgbScale << "out_color_matrix=";
	if (colorSpace.value.intValue == vkdrBT709)
	{
		rgbScale << "bt709";
	}
	else
	{
		rgbScale << "bt601";
	}
	rgbScale << ":out_range=";
	if (colorRange.value.intValue == vkdrFullColorRange)
	{
		rgbScale << "jpeg";
	}
	else
	{
		rgbScale << "mpeg";
	}

	EncodingData encodingData;
	
	Converter *converter = new Converter(videoWidth.value.intValue, videoHeight.value.intValue);

	// Prepare plane buffers
	char *planeBuffer[8];
	for (int i = 0; i < 8; i++)
	{
		planeBuffer[i] = (char *)instRec->memorySuite->NewPtr(videoHeight.value.intValue * videoWidth.value.intValue * 8); // max. 4 components / 16 bit
	}
	
	// Allocate audio output buffer
	float *audioBuffer[MAX_AUDIO_CHANNELS];

	for (int i = 0; i < MAX_AUDIO_CHANNELS; i++)
	{
		audioBuffer[i] = (float *)instRec->memorySuite->NewPtr(sizeof(float) * instRec->maxBlip);
	}

	// Export loop
	PrTime videoTime = exportInfoP->startTime;
	while (videoTime <= (exportInfoP->endTime - ticksPerFrame.value.timeValue))
	{
		FrameType frameType = encoder->getNextFrameType();
		if (FrameType::VideoFrame == frameType || audioSamplesLeft <= 0)
		{
			// Render the uncompressed frame
			SequenceRender_GetFrameReturnRec renderResult;
			result = instRec->sequenceRenderSuite->RenderVideoFrame(instRec->videoRenderID, videoTime, &renderParms, cacheType, &renderResult);
			if (result != malNoError)
			{
				ShowMessageBox(instRec, L"Error: Required pixel format has not been requested in render params.", PLUGIN_APPNAME, MB_OK);

				break;
			}

			// Get pixel format
			PrPixelFormat format;
			result = instRec->ppixSuite->GetPixelFormat(renderResult.outFrame, &format);

			// Skip invalid frames
			if (format == PrPixelFormat_Invalid)
			{
				OutputDebugStringA("Warning: Received an invalid pixel format. Skipping this frame ...");
			}
			// Planar YUV 4:2:0 8bit formats
			else if (
				format == PrPixelFormat_YUV_420_MPEG4_FRAME_PICTURE_PLANAR_8u_709 ||
				format == PrPixelFormat_YUV_420_MPEG4_FIELD_PICTURE_PLANAR_8u_709 ||
				format == PrPixelFormat_YUV_420_MPEG4_FRAME_PICTURE_PLANAR_8u_709_FullRange ||
				format == PrPixelFormat_YUV_420_MPEG4_FIELD_PICTURE_PLANAR_8u_709_FullRange ||
				format == PrPixelFormat_YUV_420_MPEG4_FRAME_PICTURE_PLANAR_8u_601 ||
				format == PrPixelFormat_YUV_420_MPEG4_FIELD_PICTURE_PLANAR_8u_601 ||
				format == PrPixelFormat_YUV_420_MPEG4_FRAME_PICTURE_PLANAR_8u_601_FullRange ||
				format == PrPixelFormat_YUV_420_MPEG4_FIELD_PICTURE_PLANAR_8u_601_FullRange)
			{
				encodingData.planes = 3;
				encodingData.pix_fmt = "yuv420p";

				// Get planar buffers
				result = instRec->ppix2Suite->GetYUV420PlanarBuffers(
					renderResult.outFrame,
					PrPPixBufferAccess_ReadOnly,
					&encodingData.plane[0],
					&encodingData.stride[0],
					&encodingData.plane[1],
					&encodingData.stride[1],
					&encodingData.plane[2],
					&encodingData.stride[2]);
			}
			else // Other (packet) formats
			{
				// Get packed rowsize
				csSDK_int32 rowBytes;
				result = instRec->ppixSuite->GetRowBytes(renderResult.outFrame, &rowBytes);

				// Get pixels from the renderer
				char *pixels;
				result = instRec->ppixSuite->GetPixels(renderResult.outFrame, PrPPixBufferAccess_ReadOnly, &pixels);

				// Handle packed formats
				if (format == PrPixelFormat_UYVY_422_8u_709 ||
					format == PrPixelFormat_UYVY_422_8u_601)
				{
					encodingData.planes = 1;
					encodingData.pix_fmt = "uyvy422";
					encodingData.plane[0] = pixels;
					encodingData.filters.vflip = false;
					encodingData.filters.scale = "";
				}
				else if (format == PrPixelFormat_YUYV_422_8u_601 ||
					format == PrPixelFormat_YUYV_422_8u_709)
				{
					encodingData.planes = 1;
					encodingData.pix_fmt = "yuyv422";
					encodingData.plane[0] = pixels;
					encodingData.filters.vflip = false;
					encodingData.filters.scale = "";
				}
				else if (format == PrPixelFormat_V210_422_10u_709 ||
					format == PrPixelFormat_V210_422_10u_601)
				{
					// Decode v210 to yuv444p
					converter->decodeV210ToYUV422p10(pixels, rowBytes, planeBuffer[0], planeBuffer[1], planeBuffer[2]);

					encodingData.planes = 3;
					encodingData.pix_fmt = av_get_pix_fmt_name(AV_PIX_FMT_YUV422P10LE);
					encodingData.plane[0] = planeBuffer[0];
					encodingData.plane[1] = planeBuffer[1];
					encodingData.plane[2] = planeBuffer[2];
					encodingData.filters.vflip = false;
					encodingData.filters.scale = "";
				}
				else if (format == PrPixelFormat_VUYA_4444_8u ||
					format == PrPixelFormat_VUYA_4444_8u_709)
				{
					// Convert ayuv to yuv444p
					converter->convertVUYA4444_8uToYUV444(pixels, planeBuffer[0], planeBuffer[1], planeBuffer[2]);

					// Fill encoding data
					encodingData.planes = 3;
					encodingData.pix_fmt = "yuv444p";
					encodingData.plane[0] = planeBuffer[0];
					encodingData.plane[1] = planeBuffer[1];
					encodingData.plane[2] = planeBuffer[2];
					encodingData.filters.vflip = false;
					encodingData.filters.scale = "";
				}
				else if (format == PrPixelFormat_VUYA_4444_32f ||
					format == PrPixelFormat_VUYA_4444_32f_709)
				{
					// Convert float to int16
					converter->convertVUYA4444_32fToYUVA444p16(pixels, planeBuffer[0], planeBuffer[1], planeBuffer[2], planeBuffer[3]);

					// Fill encoding data
					encodingData.planes = 4;
					encodingData.pix_fmt = "yuva444p16le";
					encodingData.plane[0] = planeBuffer[0];
					encodingData.plane[1] = planeBuffer[1];
					encodingData.plane[2] = planeBuffer[2];
					encodingData.plane[3] = planeBuffer[3];
					encodingData.filters.vflip = false;
					encodingData.filters.scale = "";
				}
				else if (format == PrPixelFormat_BGRA_4444_8u) // Default BGRA format
				{
					encodingData.planes = 1;
					encodingData.pix_fmt = "bgra";
					encodingData.plane[0] = pixels;
					encodingData.stride[0] = rowBytes;
					encodingData.filters.vflip = true;
					encodingData.filters.scale = rgbScale.str();
				}
				else if (format == PrPixelFormat_BGRA_4444_16u) 
				{
					encodingData.planes = 1;
					encodingData.pix_fmt = "bgra64le";
					encodingData.plane[0] = pixels;
					encodingData.filters.vflip = true;
					encodingData.filters.scale = rgbScale.str();
				}
				else
				{
					char buf[256];
					sprintf_s(buf, "Error: Pixel Format #%d is not implemented!", format);

					ShowMessageBox(instRec, buf, "Not Implemented Exception", MB_OK);

					result = malUnknownError;
					break;
				}
			}

			// Send to encoder
			if (encoder->writeVideoFrame(&encodingData) != S_OK)
			{
				result = malUnknownError;
				break;
			}

			// Dispose the rendered frame
			result = instRec->ppixSuite->Dispose(renderResult.outFrame);

			videoTime += ticksPerFrame.value.timeValue;
		}
		else if (FrameType::AudioFrame == frameType)
		{
			int chunk = audioSamplesLeft;

			// Set chunk size
			if (chunk > instRec->maxBlip)
			{
				chunk = instRec->maxBlip;
			}

			// Get the sample data
			result = instRec->sequenceAudioSuite->GetAudio(instRec->audioRenderID, chunk, audioBuffer, kPrFalse);

			// Send raw data to the encoder
			if (encoder->writeAudioFrame((const uint8_t**)audioBuffer, chunk) != S_OK)
			{
				result = malUnknownError;
				break;
			}

			audioSamplesLeft -= chunk;
		}

		// Update progress & handle export cancellation
		float offset = (1.0f / static_cast<float>(maxPasses)) * (static_cast<float>(pass) - 1);
		float progress = offset + static_cast<float>(videoTime - exportInfoP->startTime) / static_cast<float>(exportDuration) / static_cast<float>(maxPasses);
		result = instRec->exportProgressSuite->UpdateProgressPercent(exID, progress);
		if (result == suiteError_ExporterSuspended)
		{
			instRec->exportProgressSuite->WaitForResume(exID);
		}
		else if (result == exportReturn_Abort)
		{
			exportDuration = videoTime + ticksPerFrame.value.timeValue - exportInfoP->startTime < exportDuration ? videoTime + ticksPerFrame.value.timeValue - exportInfoP->startTime : exportDuration;
			break;
		}
	}

	// Free audio output buffer
	for (int i = 0; i < MAX_AUDIO_CHANNELS; i++)
	{
		instRec->memorySuite->PrDisposePtr((char *)audioBuffer[i]);
	}

	// Free plane buffers
	for (int i = 0; i < 8; i++)
	{
		instRec->memorySuite->PrDisposePtr((char *)planeBuffer[i]);
	}

	converter->~Converter();
	converter = NULL;

	// Release renderers
	instRec->sequenceRenderSuite->ReleaseVideoRenderer(exID, instRec->videoRenderID);
	instRec->sequenceAudioSuite->ReleaseAudioRenderer(exID, instRec->audioRenderID);
	
	return result;
}

Voukoder::Voukoder()
{
	// Build this as a class sometime
}