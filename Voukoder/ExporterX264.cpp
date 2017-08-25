#include "Encoder.h"
#include "ExporterX264.h"
#include "ExporterX264Common.h"
#include "ExporterX264Settings.h"
#include <windows.h>
#include "resource.h"

void initFeatures(json *features)
{
	MEMORY_BASIC_INFORMATION mbi;
	static int dummy;
	VirtualQuery(&dummy, &mbi, sizeof(mbi));
	HMODULE hModule = reinterpret_cast<HMODULE>(mbi.AllocationBase);
	HRSRC hRes = FindResourceEx(hModule, MAKEINTRESOURCE(ID_TEXT), MAKEINTRESOURCE(IDR_ID_TEXT1), MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL));
	if (NULL != hRes)
	{
		HGLOBAL hData = LoadResource(hModule, hRes);
		if (NULL != hData)
		{
			const DWORD dataSize = SizeofResource(hModule, hRes);
			char *resource = new char[dataSize + 1];
			memcpy(resource, LockResource(hData), dataSize);
			resource[dataSize] = 0;

			*features = json::parse(resource);
		}
	}
}

DllExport PREMPLUGENTRY xSDKExport(csSDK_int32 selector, exportStdParms *stdParmsP, void *param1, void	*param2)
{
	switch (selector)
	{
	case exSelStartup:					return exStartup(stdParmsP, reinterpret_cast<exExporterInfoRec*>(param1));
	case exSelBeginInstance:			return exBeginInstance(stdParmsP, reinterpret_cast<exExporterInstanceRec*>(param1));
	case exSelEndInstance:				return exEndInstance(stdParmsP, reinterpret_cast<exExporterInstanceRec*>(param1));
	case exSelGenerateDefaultParams:	return exGenerateDefaultParams(stdParmsP, reinterpret_cast<exGenerateDefaultParamRec*>(param1));
	case exSelPostProcessParams:		return exPostProcessParams(stdParmsP, reinterpret_cast<exPostProcessParamsRec*>(param1));
	case exSelValidateParamChanged:		return exValidateParamChanged(stdParmsP, reinterpret_cast<exParamChangedRec*>(param1));
	case exSelExport:					return exExport(stdParmsP, reinterpret_cast<exDoExportRec*>(param1));
	case exSelQueryExportFileExtension: return exFileExtension(stdParmsP, reinterpret_cast<exQueryExportFileExtensionRec*>(param1));
	case exSelQueryOutputSettings:		return exQueryOutputSettings(stdParmsP, reinterpret_cast<exQueryOutputSettingsRec*>(param1));
	}

	return exportReturn_Unsupported;
}

prMALError exStartup(exportStdParms *stdParmsP, exExporterInfoRec *infoRecP)
{
	av_register_all();
	avfilter_register_all();

	infoRecP->fileType = 'X264';
	copyConvertStringLiteralIntoUTF16(PLUGIN_APPNAME, infoRecP->fileTypeName);
	copyConvertStringLiteralIntoUTF16(L"mp4", infoRecP->fileTypeDefaultExtension);
	infoRecP->classID = 'DTEK';
	infoRecP->exportReqIndex = 0;
	infoRecP->wantsNoProgressBar = kPrFalse;
	infoRecP->hideInUI = kPrFalse;
	infoRecP->doesNotSupportAudioOnly = kPrTrue;
	infoRecP->canExportVideo = kPrTrue;
	infoRecP->canExportAudio = kPrTrue;
	infoRecP->interfaceVersion = EXPORTMOD_VERSION;

	return malNoError;
}

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
			spError = spBasic->AcquireSuite(kPrSDKExportProgressSuite, kPrSDKExportProgressSuiteVersion, const_cast<const void**>(reinterpret_cast<void**>(&(instRec->exportProgressSuite))));
			spError = spBasic->AcquireSuite(kPrSDKWindowSuite, kPrSDKWindowSuiteVersion, const_cast<const void**>(reinterpret_cast<void**>(&(instRec->windowSuite))));

			// Get settings
			instRec->settings = new Settings();
		}

		instanceRecP->privateData = reinterpret_cast<void*>(instRec);
	}
	else
	{
		result = exportReturn_ErrMemory;
	}

	return result;
}

prMALError exEndInstance(exportStdParms *stdParmsP, exExporterInstanceRec *instanceRecP)
{
	prMALError result = malNoError;

	/* Release all aquired suites */
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

		if (instRec->exportProgressSuite)
		{
			result = spBasic->ReleaseSuite(kPrSDKExportProgressSuite, kPrSDKExportProgressSuiteVersion);
		}
		
		if (instRec->windowSuite)
		{
			result = spBasic->ReleaseSuite(kPrSDKWindowSuite, kPrSDKWindowSuiteVersion);
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

prMALError exFileExtension(exportStdParms *stdParmsP, exQueryExportFileExtensionRec *exportFileExtensionRecP)
{
	prMALError result = malNoError;
	csSDK_uint32 exID = exportFileExtensionRecP->exporterPluginID;
	InstanceRec *instRec = reinterpret_cast<InstanceRec *>(exportFileExtensionRecP->privateData);
	Settings *settings = instRec->settings;

	// Get the users multiplexer choice
	exParamValues multiplexer;
	instRec->exportParamSuite->GetParamValue(exID, 0, FFMultiplexer, &multiplexer);

	// Get selected muxer
	const json muxers = settings->getMuxers();
	const json muxer = Settings::filterArrayById(muxers, multiplexer.value.intValue);

	// Set the right extension
	if (!muxer.empty())
	{
		const std::string extension = muxer["extension"].get<std::string>();
		const std::wstring widestr = std::wstring(extension.begin(), extension.end());
		copyConvertStringLiteralIntoUTF16(widestr.c_str(), exportFileExtensionRecP->outFileExtension);

		return malNoError;
	}

	return malUnknownError;
}

prMALError exQueryOutputSettings(exportStdParms *stdParmsP, exQueryOutputSettingsRec *outputSettingsP)
{
	prMALError result = malNoError;
	csSDK_uint32 exID = outputSettingsP->exporterPluginID;
	exParamValues width, height, frameRate, pixelAspectRatio, fieldType, codec, sampleRate, channelType;
	InstanceRec *instRec = reinterpret_cast<InstanceRec *>(outputSettingsP->privateData);
	Settings *settings = instRec->settings;
	PrSDKExportParamSuite *paramSuite = instRec->exportParamSuite;
	csSDK_int32 mgroupIndex = 0;
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

	// Calculate bitrate
	PrTime ticksPerSecond = 0;
	csSDK_uint32 videoBitrate = 0, audioBitrate = 0;
	if (outputSettingsP->inExportVideo)
	{
		instRec->timeSuite->GetTicksPerSecond(&ticksPerSecond);
		fps = static_cast<float>(ticksPerSecond) / frameRate.value.timeValue;
		paramSuite->GetParamValue(exID, mgroupIndex, ADBEVideoCodec, &codec);
		videoBitrate = static_cast<csSDK_uint32>(width.value.intValue * height.value.intValue * 4 * fps); //TODO
	}
	if (outputSettingsP->inExportAudio)
	{
		audioBitrate = static_cast<csSDK_uint32>(sampleRate.value.floatValue * 4 * 2); //TODO
	}
	outputSettingsP->outBitratePerSecond = videoBitrate + audioBitrate;

	// New in CS5 - return outBitratePerSecond in kbps
	outputSettingsP->outBitratePerSecond = outputSettingsP->outBitratePerSecond * 8 / 1000;

	//TODO: Add proper bitrate calculation if possible

	return result;
}

prMALError exExport(exportStdParms *stdParmsP, exDoExportRec *exportInfoP)
{
	prMALError result = malNoError;
	csSDK_uint32 exID = exportInfoP->exporterPluginID;
	
	InstanceRec *instRec = reinterpret_cast<InstanceRec *>(exportInfoP->privateData);
	Settings *settings = instRec->settings;

	int ret;

	// Export video params
	exParamValues videoCodec, videoWidth, videoHeight, ticksPerFrame, audioCodec, channelType, audioSampleRate, audioBitrate;
	instRec->exportParamSuite->GetParamValue(exID, 0, ADBEVideoCodec, &videoCodec);
	instRec->exportParamSuite->GetParamValue(exID, 0, ADBEVideoWidth, &videoWidth);
	instRec->exportParamSuite->GetParamValue(exID, 0, ADBEVideoHeight, &videoHeight);
	instRec->exportParamSuite->GetParamValue(exID, 0, ADBEVideoFPS, &ticksPerFrame);
	instRec->exportParamSuite->GetParamValue(exID, 0, ADBEAudioCodec, &audioCodec);
	instRec->exportParamSuite->GetParamValue(exID, 0, ADBEAudioNumChannels, &channelType);
	instRec->exportParamSuite->GetParamValue(exID, 0, ADBEAudioRatePerSecond, &audioSampleRate);
	instRec->exportParamSuite->GetParamValue(exID, 0, ADBEAudioBitrate, &audioBitrate);

#pragma region Get video encoder settings

	const int videoCodecId = videoCodec.value.intValue;

	// Get the selected codec
	json videoCodecs = settings->getVideoEncoders();
	json vCodec = settings->filterArrayById(videoCodecs, videoCodecId);

	// Go through all codec options
	std::vector<std::string> videoEncoderOptions;
	createEncoderConfiguration(instRec, exID, 0, vCodec["options"], &videoEncoderOptions);

	// Convert vector to string
	std::stringstream videoEncoderStream;
	for (size_t i = 0; i < videoEncoderOptions.size(); ++i)
	{
		if (i != 0)
			videoEncoderStream << ':';
		videoEncoderStream << videoEncoderOptions[i];
	}

	const std::string videoEncoderConfig = videoEncoderStream.str();

#pragma endregion

#pragma region Get audio encoder settings

	const int audioCodecId = audioCodec.value.intValue;

	// Get the selected codec
	json audioCodecs = settings->getAudioEncoders();
	json aCodec = settings->filterArrayById(audioCodecs, audioCodecId);

	// Go through all codec options
	std::vector<std::string> audioEncoderOptions;
	createEncoderConfiguration(instRec, exID, 0, aCodec["options"], &audioEncoderOptions);

	// Convert vector to string
	std::stringstream audioEncoderStream;
	for (size_t i = 0; i < audioEncoderOptions.size(); ++i)
	{
		if (i != 0)
			audioEncoderStream << ':';
		audioEncoderStream << audioEncoderOptions[i];
	}

	const std::string audioEncoderConfig = audioEncoderStream.str();

#pragma endregion

	// Use a lossless pixel format for converstion to libav
	PrPixelFormat pixelFormats[] = { PrPixelFormat_ARGB_4444_8u };

	// Define the render params
	SequenceRender_ParamsRec renderParms;
	renderParms.inRequestedPixelFormatArray = pixelFormats;
	renderParms.inRequestedPixelFormatArrayCount = 1;
	renderParms.inWidth = videoWidth.value.intValue;
	renderParms.inHeight = videoHeight.value.intValue;
	renderParms.inPixelAspectRatioNumerator = 1;
	renderParms.inPixelAspectRatioDenominator = 1;
	renderParms.inRenderQuality = kPrRenderQuality_High;
	renderParms.inFieldType = prFieldsNone;
	renderParms.inDeinterlace = kPrFalse;
	renderParms.inDeinterlaceQuality = kPrRenderQuality_High;
	renderParms.inCompositeOnBlack = kPrTrue;

#pragma region Get the selected framerate

	// Get the ticks per second
	PrTime ticksPerSecond = 0;
	instRec->timeSuite->GetTicksPerSecond(&ticksPerSecond);

	json configuration = settings->getConfiguration();

	// Set default framerate
	csSDK_int32 num = 1, den = 25;
	
	// Iterate all framerate options
	for (auto iterator = configuration["videoFramerates"].begin(); iterator != configuration["videoFramerates"].end(); ++iterator)
	{
		const json framerate = *iterator;

		const int num2 = framerate["num"].get<int>();
		const int den2 = framerate["den"].get<int>();

		csSDK_int64 tpf = ticksPerSecond / num2 * den2;

		if (tpf == ticksPerFrame.value.timeValue)
		{
			num = num2;
			den = den2;
			break;
		}
	}

#pragma endregion

	// Make video renderer
	csSDK_uint32 videoRenderID;
	instRec->sequenceRenderSuite->MakeVideoRenderer(exID, &videoRenderID, ticksPerFrame.value.timeValue);

	PrTime ticksPerSample;
	instRec->timeSuite->GetTicksPerAudioSample(audioSampleRate.value.floatValue, &ticksPerSample);

	// Make the audio renderer
	csSDK_uint32 audioRenderID;
	instRec->sequenceAudioSuite->MakeAudioRenderer(exID, exportInfoP->startTime, (PrAudioChannelType)channelType.value.intValue, kPrAudioSampleType_32BitFloat, audioSampleRate.value.floatValue, &audioRenderID);

	// Translate the channel layout to AVLib
	csSDK_int64 channelLayout;
	switch (channelType.value.intValue)
	{
	case kPrAudioChannelType_Mono:
		channelLayout = AV_CH_LAYOUT_MONO;
		break;
	case kPrAudioChannelType_51:
		channelLayout = AV_CH_LAYOUT_5POINT1;
		break;
	default:
		channelLayout = AV_CH_LAYOUT_STEREO;
		break;
	}
	
	// Get the file name 
	prUTF16Char prFilename[kPrMaxPath];
	csSDK_int32 prFilenameLength = kPrMaxPath;
	size_t i;
	instRec->exportFileSuite->GetPlatformPath(exportInfoP->fileObject, &prFilenameLength, prFilename);
	char *filename = (char*)malloc(kPrMaxPath);
	wcstombs_s(&i, filename, (size_t)kPrMaxPath, prFilename, (size_t)kPrMaxPath);

	// Get the audio frame size we need to send to the encoder
	csSDK_int32 maxBlip = 0;
	instRec->sequenceAudioSuite->GetMaxBlip(audioRenderID, ticksPerFrame.value.timeValue, &maxBlip);

	// Get the selected encoders
	const std::string videoEncoder = vCodec["name"].get<std::string>();
	const std::string audioEncoder = aCodec["name"].get<std::string>();

	// Create and configure an encoder instance
	Encoder *encoder = new Encoder(filename);
	encoder->setVideoCodec(videoEncoder, videoEncoderConfig, videoWidth.value.intValue, videoHeight.value.intValue, { den, num }); // framerate -> timebase
	encoder->setAudioCodec(audioEncoder, audioEncoderConfig, channelLayout, (int)audioSampleRate.value.floatValue, maxBlip);
	
	// Open the encoder
	if (encoder->open() != S_OK)
	{
		// Show message box
		HWND mainWnd = instRec->windowSuite->GetMainWindow();
		MessageBox(GetLastActivePopup(mainWnd), PLUGIN_ERR_COMBINATION_NOT_SUPPORTED, PLUGIN_APPNAME, MB_OK);

		// Show no premiere error message
		result = malNoError;

		goto error;
	}

	// Export range samples
	PrTime exportDur = exportInfoP->endTime - exportInfoP->startTime;
	PrAudioSample totalSamplesRemaining = exportDur / ticksPerSample;
	PrTime nextFramePos = 0, nextVideoFrame = 0, nextAudioFrame = 0;
	csSDK_int32 samplesRequestedL;

	// Export loop
	while (nextFramePos < exportDur && totalSamplesRemaining > 0)
	{
		// Add a video frame
		if (nextFramePos >= nextVideoFrame)
		{
			// Render the uncompressed frame
			SequenceRender_GetFrameReturnRec renderResult;
			result = instRec->sequenceRenderSuite->RenderVideoFrame(videoRenderID, exportInfoP->startTime + nextVideoFrame, &renderParms, kRenderCacheType_None, &renderResult);

			char *pixels;
			instRec->ppixSuite->GetPixels(renderResult.outFrame, PrPPixBufferAccess_ReadOnly, &pixels);

			// Send raw data to the encoder
			ret = encoder->writeVideoFrame(pixels);

			// Dispose the rendered frame
			result = instRec->ppixSuite->Dispose(renderResult.outFrame);

			nextVideoFrame += ticksPerFrame.value.timeValue;
		}

		// Add an audio frame
		if (nextFramePos >= nextAudioFrame)
		{
			// How many samples should we request
			samplesRequestedL = maxBlip;
			if (totalSamplesRemaining < samplesRequestedL)
			{
				samplesRequestedL = (csSDK_int32)totalSamplesRemaining;
			}

			// Allocate output buffer
			float *audioBuffer[6] = { NULL, NULL, NULL, NULL, NULL, NULL };
			for (csSDK_int32 bufferIndexL = 0; bufferIndexL < 6; bufferIndexL++)
			{
				audioBuffer[bufferIndexL] = (float *)instRec->memorySuite->NewPtr(samplesRequestedL * 4); //TODO: get proper sample size
			}

			// Get the sample data
			result = instRec->sequenceAudioSuite->GetAudio(audioRenderID, samplesRequestedL, audioBuffer, kPrFalse); //TODO: What is inClipAudio?

			// Send data to the encoder
			ret = encoder->writeAudioFrame((const uint8_t**)audioBuffer, samplesRequestedL);

			// Free output buffer
			for (csSDK_int32 bufferIndexL = 0; bufferIndexL < 6; bufferIndexL++)
			{
				instRec->memorySuite->PrDisposePtr((char *)audioBuffer[bufferIndexL]);
			}

			totalSamplesRemaining -= samplesRequestedL;

			nextAudioFrame += ticksPerSample * samplesRequestedL;
		}

		// Increase the progress by the smallest unit
		nextFramePos = FFMIN(nextVideoFrame, nextAudioFrame);

		// Update the encoding progress
		const float progress = static_cast<float>(nextFramePos) / static_cast<float>(exportDur);
		result = instRec->exportProgressSuite->UpdateProgressPercent(exID, progress);

		// Error handling
		if (result == suiteError_ExporterSuspended)
		{
			instRec->exportProgressSuite->WaitForResume(exID);
		}
		else if (result == exportReturn_Abort)
		{
			exportDur = nextFramePos + ticksPerFrame.value.timeValue < exportDur ? nextFramePos + ticksPerFrame.value.timeValue : exportDur;
			break;
		}
	}

	// Flush the encoders
	encoder->writeVideoFrame(NULL);
	encoder->writeAudioFrame(NULL, 0);
	encoder->close();

error:

	// Release renderers
	instRec->sequenceRenderSuite->ReleaseVideoRenderer(exID, videoRenderID);
	instRec->sequenceAudioSuite->ReleaseAudioRenderer(exID, audioRenderID);

	encoder->~Encoder();

	return result;
}
	
void createEncoderConfiguration(InstanceRec *instRec, csSDK_uint32 pluginId, csSDK_int32 groupIndex, json options, std::vector<std::string> *encoderConfiguration)
{
	std::map<std::string, std::string> optionMap;

	exParamValues paramValues;
	char buffer[50];
	int length;

	// Iterate all encoder options
	for (auto iterator = options.begin(); iterator != options.end(); ++iterator)
	{
		const json option = *iterator;

		const std::string name = option["name"].get<std::string>();
		const std::string flags = option["flags"].get<std::string>();

		// What element type is it?
		if (flags == "slider")
		{
			const std::string type = option["type"].get<std::string>();
			const std::string param = option["param"].get<std::string>();  

			// Get the suboptions selected value
			instRec->exportParamSuite->GetParamValue(pluginId, groupIndex, name.c_str(), &paramValues);

			// Format according to datatype
			if (type == "float")
			{
				const double value = paramValues.value.floatValue;
				length = sprintf_s(buffer, param.c_str(), value);
			}
			else
			{
				const int value = paramValues.value.intValue;
				length = sprintf_s(buffer, param.c_str(), value);
			}

			// Get the parameter
			const std::string config = std::string(buffer, length + 1);

			// Add to options if not empty
			if (!config.empty())
			{
				optionMap.insert(std::pair<std::string, std::string>(name, config));
			}
		}
		else
		{
			// Get the selection
			instRec->exportParamSuite->GetParamValue(pluginId, groupIndex, name.c_str(), &paramValues);
			const int selectionId = paramValues.value.intValue;

			// Get the selected value
			const json value = Settings::filterArrayById(option["values"], selectionId);

			// Do we have suboptions?
			if (value.find("suboptions") != value.end())
			{
				// Iterate all suboptions
				for (auto iterator2 = value["suboptions"].begin(); iterator2 != value["suboptions"].end(); ++iterator2)
				{
					const json suboption = *iterator2;

					const std::string subname = suboption["name"].get<std::string>();
					const std::string subtype = suboption["type"].get<std::string>();
					const std::string subparam = suboption["param"].get<std::string>();

					// Get the suboptions selected value
					instRec->exportParamSuite->GetParamValue(pluginId, groupIndex, subname.c_str(), &paramValues);
				
					bool isZero = false;

					// Format according to datatype
					if (subtype == "float")
					{
						const double value = paramValues.value.floatValue;
						isZero = value == 0.0;
						length = sprintf_s(buffer, subparam.c_str(), value);
					}
					else
					{
						const int value = paramValues.value.intValue;
						isZero = value == 0;
						length = sprintf_s(buffer, subparam.c_str(), value);
					}

					// TODO
					if (isZero && suboption.find("disableFieldOnZero") != suboption.end())
					{
						const std::string name = suboption["name"].get<std::string>();
						const std::string field = suboption["disableFieldOnZero"].get<std::string>();

						optionMap.erase(field);
						//TODO
					}

					// Get the parameter
					const std::string param = std::string(buffer, length + 1);

					// Add to options if not empty
					if (!param.empty())
					{
						optionMap.insert(std::pair<std::string, std::string>(name, param));
					}
				}
			}
			else
			{
				// Get the parameter
				const std::string param = value["param"].get<std::string>();

				// Add to options if not empty
				if (!param.empty())
				{
					optionMap.insert(std::pair<std::string, std::string>(name, param));
				}
			}
		}
	}

	// Copy remaining values to vector
	for (std::map<std::string, std::string>::iterator it = optionMap.begin(); it != optionMap.end(); ++it)
	{
		encoderConfiguration->push_back(it->second);
	}
}

