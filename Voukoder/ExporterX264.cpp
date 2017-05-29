#include "Encoder.h"
#include "ExporterX264.h"
#include "ExporterX264Common.h"
#include "ExporterX264Settings.h"

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
	}

	return exportReturn_Unsupported;
}

prMALError exStartup(exportStdParms *stdParmsP, exExporterInfoRec *infoRecP)
{
	infoRecP->fileType = 'X264';
	copyConvertStringLiteralIntoUTF16(APPNAME, infoRecP->fileTypeName);
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

		if (instRec->memorySuite)
		{
			instRec->memorySuite->PrDisposePtr(reinterpret_cast<PrMemoryPtr>(instRec));
			result = spBasic->ReleaseSuite(kPrSDKMemoryManagerSuite, kPrSDKMemoryManagerSuiteVersion);
		}
	}

	return malNoError;
}

prMALError exFileExtension(exportStdParms *stdParmsP, exQueryExportFileExtensionRec *exportFileExtensionRecP)
{
	prMALError result = malNoError;
	csSDK_uint32 exID = exportFileExtensionRecP->exporterPluginID;
	InstanceRec *instRec = reinterpret_cast<InstanceRec *>(exportFileExtensionRecP->privateData);

	/* Get the users multiplexer choice */
	exParamValues multiplexer;
	instRec->exportParamSuite->GetParamValue(exID, 0, FFMultiplexer, &multiplexer);

	Config config;
	switch (multiplexer.value.intValue)
	{
	case FFExportFormat::MP4:
		copyConvertStringLiteralIntoUTF16(L"mp4", exportFileExtensionRecP->outFileExtension);
		break;
	case FFExportFormat::Matroska:
		copyConvertStringLiteralIntoUTF16(L"mkv", exportFileExtensionRecP->outFileExtension);
		break;
	case FFExportFormat::MOV:
		copyConvertStringLiteralIntoUTF16(L"mov", exportFileExtensionRecP->outFileExtension);
		break;
	}

	return malNoError;
}

prMALError exExport(exportStdParms *stdParmsP, exDoExportRec *exportInfoP)
{
	prMALError result = malNoError;
	csSDK_uint32 exID = exportInfoP->exporterPluginID;
	InstanceRec *instRec = reinterpret_cast<InstanceRec *>(exportInfoP->privateData);
	std::vector<std::string> options;
	int ret;

	/* Export video params */
	exParamValues videoCodec, videoWidth, videoHeight, pixelAspectRatio, ticksPerFrame, fieldType, audioCodec, channelType, audioSampleRate, audioBitrate;
	instRec->exportParamSuite->GetParamValue(exID, 0, ADBEVideoCodec, &videoCodec);
	instRec->exportParamSuite->GetParamValue(exID, 0, ADBEVideoWidth, &videoWidth);
	instRec->exportParamSuite->GetParamValue(exID, 0, ADBEVideoHeight, &videoHeight);
	instRec->exportParamSuite->GetParamValue(exID, 0, ADBEVideoAspect, &pixelAspectRatio);
	instRec->exportParamSuite->GetParamValue(exID, 0, ADBEVideoFPS, &ticksPerFrame);
	instRec->exportParamSuite->GetParamValue(exID, 0, ADBEVideoFieldType, &fieldType);
	instRec->exportParamSuite->GetParamValue(exID, 0, ADBEAudioCodec, &audioCodec);
	instRec->exportParamSuite->GetParamValue(exID, 0, ADBEAudioNumChannels, &channelType);
	instRec->exportParamSuite->GetParamValue(exID, 0, ADBEAudioRatePerSecond, &audioSampleRate);
	instRec->exportParamSuite->GetParamValue(exID, 0, ADBEAudioBitrate, &audioBitrate);

#pragma region Private x264 codec settings

	/* Codec Options */
	exParamValues preset, profile, level, strategy;
	instRec->exportParamSuite->GetParamValue(exID, 0, X264_Preset, &preset);
	instRec->exportParamSuite->GetParamValue(exID, 0, X264_Profile, &profile);
	instRec->exportParamSuite->GetParamValue(exID, 0, X264_Level, &level);
	instRec->exportParamSuite->GetParamValue(exID, 0, X264_Strategy, &strategy);

	/* Preset */
	switch (preset.value.intValue)
	{
	case (csSDK_int32)X264::Preset::UltraFast:
		options.push_back("preset=ultrafast");
		break;

	case (csSDK_int32)X264::Preset::SuperFast:
		options.push_back("preset=superfast");
		break;

	case (csSDK_int32)X264::Preset::VeryFast:
		options.push_back("preset=veryfast");
		break;

	case (csSDK_int32)X264::Preset::Faster:
		options.push_back("preset=faster");
		break;

	case (csSDK_int32)X264::Preset::Fast:
		options.push_back("preset=fast");
		break;

	case (csSDK_int32)X264::Preset::Slow:
		options.push_back("preset=slow");
		break;

	case (csSDK_int32)X264::Preset::Slower:
		options.push_back("preset=slower");
		break;

	case (csSDK_int32)X264::Preset::VerySlow:
		options.push_back("preset=veryslow");
		break;

	case (csSDK_int32)X264::Preset::Placebo:
		options.push_back("preset=placebo");
		break;
	}

	/* Profile */
	switch (profile.value.intValue)
	{
	case (csSDK_int32)X264::Profile::Baseline:
		options.push_back("profile=baseline");
		break;

	case (csSDK_int32)X264::Profile::Main:
		options.push_back("profile=main");
		break;

	case (csSDK_int32)X264::Profile::High422:
		options.push_back("profile=high422");
		break;

	case (csSDK_int32)X264::Profile::High444:
		options.push_back("profile=high444");
		break;
	}

	/* Level */
	switch (level.value.intValue)
	{
	case (csSDK_int32)X264::Level::Level1:
		options.push_back("level=1");
		break;

	case (csSDK_int32)X264::Level::Level1_1:
		options.push_back("level=1.1");
		break;

	case (csSDK_int32)X264::Level::Level1_2:
		options.push_back("level=1.2");
		break;

	case (csSDK_int32)X264::Level::Level1_3:
		options.push_back("level=1.3");
		break;

	case (csSDK_int32)X264::Level::Level2:
		options.push_back("level=2");
		break;

	case (csSDK_int32)X264::Level::Level2_1:
		options.push_back("level=2.1");
		break;

	case (csSDK_int32)X264::Level::Level2_2:
		options.push_back("level=2.2");
		break;

	case (csSDK_int32)X264::Level::Level3:
		options.push_back("level=3");
		break;

	case (csSDK_int32)X264::Level::Level3_1:
		options.push_back("level=3.1");
		break;

	case (csSDK_int32)X264::Level::Level3_2:
		options.push_back("level=3.2");
		break;

	case (csSDK_int32)X264::Level::Level4:
		options.push_back("level=4");
		break;

	case (csSDK_int32)X264::Level::Level4_1:
		options.push_back("level=4.1");
		break;

	case (csSDK_int32)X264::Level::Level4_2:
		options.push_back("level=4.2");
		break;

	case (csSDK_int32)X264::Level::Level5:
		options.push_back("level=5");
		break;

	case (csSDK_int32)X264::Level::Level5_1:
		options.push_back("level=5.1");
		break;
	}

	/* Strategy */
	exParamValues strategyValue;
	char value[40];
	switch (strategy.value.intValue)
	{
	case (csSDK_int32)X264::Strategy::ABR:
		instRec->exportParamSuite->GetParamValue(exID, 0, ADBEVideoBitrateEncoding, &strategyValue);
		sprintf_s(value, "bitrate=%.0f", strategyValue.value.floatValue * 1000);
		options.push_back(value);
		break;

	case (csSDK_int32)X264::Strategy::CRF:
		instRec->exportParamSuite->GetParamValue(exID, 0, X264_Strategy_CRF, &strategyValue);
		sprintf_s(value, "crf=%d", strategyValue.value.intValue);
		options.push_back(value);
		break;

	case (csSDK_int32)X264::Strategy::QP:
		instRec->exportParamSuite->GetParamValue(exID, 0, X264_Strategy_QP, &strategyValue);
		sprintf_s(value, "qp=%d", strategyValue.value.intValue);
		options.push_back(value);
		break;
	}

	/* PAR */
	if (pixelAspectRatio.value.ratioValue.numerator != 1 && pixelAspectRatio.value.ratioValue.denominator != 1)
	{
		char par[100];
		sprintf_s(par, "sar=%d:%d", pixelAspectRatio.value.ratioValue.numerator, pixelAspectRatio.value.ratioValue.denominator);
		options.push_back(par);
	}

	/* Field type */
	switch (fieldType.value.intValue)
	{
	case prFieldsUpperFirst:
		options.push_back("tff");
		break;

	case prFieldsLowerFirst:
		options.push_back("bff");
		break;
	}

	std::stringstream ss;
	for (size_t i = 0; i < options.size(); ++i)
	{
		if (i != 0)
			ss << ':';
		ss << options[i];
	}

#pragma endregion

	/* Define the render params */
	SequenceRender_ParamsRec renderParms;
	PrPixelFormat pixelFormats[] = { PrPixelFormat_ARGB_4444_8u };
	renderParms.inRequestedPixelFormatArray = pixelFormats;
	renderParms.inRequestedPixelFormatArrayCount = 1;
	renderParms.inWidth = videoWidth.value.intValue;
	renderParms.inHeight = videoHeight.value.intValue;
	renderParms.inPixelAspectRatioNumerator = pixelAspectRatio.value.ratioValue.numerator;
	renderParms.inPixelAspectRatioDenominator = pixelAspectRatio.value.ratioValue.denominator;
	renderParms.inRenderQuality = kPrRenderQuality_High;
	renderParms.inFieldType = fieldType.value.intValue;
	renderParms.inDeinterlace = kPrFalse;
	renderParms.inDeinterlaceQuality = kPrRenderQuality_High;

	PrTime ticksPerSecond = 0;
	instRec->timeSuite->GetTicksPerSecond(&ticksPerSecond);

	/* Timebase */
	Config config;
	csSDK_int32 num = 1, den = 25;
	csSDK_int64 tpf;
	for (RatioParam item : config.framerates)
	{
		tpf = ticksPerSecond / item.num * item.den;
		if (tpf == ticksPerFrame.value.timeValue)
		{
			num = item.den;
			den = item.num;
			break;
		}
	}

	/* Pixel format depends on the x264 profile */
	AVPixelFormat pixelFormat = AV_PIX_FMT_YUV420P;
	if (profile.value.intValue == (csSDK_int32)X264::Profile::High422)
	{
		pixelFormat = AV_PIX_FMT_YUV422P;
	}
	else if (profile.value.intValue == (csSDK_int32)X264::Profile::High444)
	{
		pixelFormat = AV_PIX_FMT_YUV444P;
	}

	/* Make video renderer */
	csSDK_uint32 videoRenderID;
	instRec->sequenceRenderSuite->MakeVideoRenderer(exID, &videoRenderID, ticksPerFrame.value.timeValue);

	PrTime ticksPerSample = 0;
	instRec->timeSuite->GetTicksPerAudioSample(audioSampleRate.value.floatValue, &ticksPerSample);

	/* Make the audio renderer */
	csSDK_uint32 audioRenderID;
	instRec->sequenceAudioSuite->MakeAudioRenderer(exID, exportInfoP->startTime, (PrAudioChannelType)channelType.value.intValue, kPrAudioSampleType_32BitFloat, audioSampleRate.value.floatValue, &audioRenderID);

	/* Translate the channel layout to AVLib */
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

	float progress = 0.0;
	PrTime exportDur = exportInfoP->endTime - exportInfoP->startTime;
	PrTime *exportDuration = &exportDur;

	/* Get the file name */
	prUTF16Char prFilename[kPrMaxPath];
	csSDK_int32 prFilenameLength = kPrMaxPath;
	size_t i;
	instRec->exportFileSuite->GetPlatformPath(exportInfoP->fileObject, &prFilenameLength, prFilename);
	char *filename = (char*)malloc(kPrMaxPath);
	wcstombs_s(&i, filename, (size_t)kPrMaxPath, prFilename, (size_t)kPrMaxPath);

	/* Get the frame size we need to send to the encoder */
	csSDK_int32 maxBlip = 0;
	instRec->sequenceAudioSuite->GetMaxBlip(audioRenderID, ticksPerFrame.value.timeValue, &maxBlip);

	/* Create an encoder instance */
	Encoder *encoder = new Encoder(filename);
	encoder->setVideoCodec(static_cast<AVCodecID>(videoCodec.value.intValue), videoWidth.value.intValue, videoHeight.value.intValue, pixelFormat, { pixelAspectRatio.value.ratioValue.numerator, pixelAspectRatio.value.ratioValue.denominator }, { num, den }, static_cast<AVFieldOrder>(fieldType.value.intValue));
	encoder->setAudioCodec(static_cast<AVCodecID>(audioCodec.value.intValue), channelLayout, audioBitrate.value.intValue * 1000, static_cast<int>(audioSampleRate.value.floatValue), maxBlip);

	/* Open the encoder streams */
	encoder->open(ss.str().c_str(), NULL);

	/* Export range samples */
	PrAudioSample totalAudioSamples = exportDur / ticksPerSample;
	PrAudioSample totalSamplesRemaining = totalAudioSamples;

	csSDK_int32 samplesRequestedL;
	PrTime ticksPerAudioFrame = ticksPerSample * maxBlip;
	PrTime nextVideoFrame = 0, nextAudioFrame = 0;

	/* Export loop */
	PrTime videoTime = exportInfoP->startTime;
	while (videoTime < exportInfoP->endTime)
	{
		/* Add a video frame */
		if (videoTime >= nextVideoFrame)
		{
			SequenceRender_GetFrameReturnRec renderResult;
			result = instRec->sequenceRenderSuite->RenderVideoFrame(videoRenderID, exportInfoP->startTime + nextVideoFrame, &renderParms, kRenderCacheType_None, &renderResult);

			char *pixels;
			instRec->ppixSuite->GetPixels(renderResult.outFrame, PrPPixBufferAccess_ReadOnly, &pixels);

			/* Let ffmpeg encode the frame */
			ret = encoder->writeVideoFrame(pixels);

			instRec->ppixSuite->Dispose(renderResult.outFrame);
			nextVideoFrame += ticksPerFrame.value.timeValue;
		}

		/* Add an audio frame */
		if (videoTime >= nextAudioFrame)
		{
			/* Get the actual frame size, at the end it will be smaller */
			samplesRequestedL = maxBlip;
			if (totalSamplesRemaining < samplesRequestedL)
			{
				samplesRequestedL = (csSDK_int32)totalSamplesRemaining;
			}

			/* Allocate output buffer */
			float *audioBuffer[6] = { NULL, NULL, NULL, NULL, NULL, NULL };
			for (csSDK_int32 bufferIndexL = 0; bufferIndexL < 6; bufferIndexL++)
			{
				audioBuffer[bufferIndexL] = (float *)instRec->memorySuite->NewPtr(samplesRequestedL * 4);
			}

			/* Get the sample data */
			result = instRec->sequenceAudioSuite->GetAudio(audioRenderID, samplesRequestedL, audioBuffer, kPrFalse);

			/* Let ffmpeg encode the frame */
			ret = encoder->writeAudioFrame((const uint8_t**)audioBuffer, samplesRequestedL);

			/* Free output buffer */
			for (csSDK_int32 bufferIndexL = 0; bufferIndexL < 6; bufferIndexL++)
			{
				instRec->memorySuite->PrDisposePtr((char *)audioBuffer[bufferIndexL]);
			}

			totalSamplesRemaining -= samplesRequestedL;
			nextAudioFrame += ticksPerAudioFrame;
		}

		/* Update the encoding progress */
		progress = static_cast<float>(videoTime - exportInfoP->startTime) / static_cast<float>(*exportDuration);
		result = instRec->exportProgressSuite->UpdateProgressPercent(exID, progress);

		if (result == suiteError_ExporterSuspended)
		{
			instRec->exportProgressSuite->WaitForResume(exID);
		}
		else if (result == exportReturn_Abort)
		{
			*exportDuration = videoTime + ticksPerFrame.value.timeValue - exportInfoP->startTime < *exportDuration ? videoTime + ticksPerFrame.value.timeValue - exportInfoP->startTime : *exportDuration;
			break;
		}

		/* Increase the progress by the smallest unit */
		if (ticksPerFrame.value.timeValue > ticksPerAudioFrame)
		{
			videoTime += ticksPerAudioFrame;
		}
		else
		{
			videoTime += ticksPerFrame.value.timeValue;
		}
	}

	/* Flush the encoder */
	encoder->writeVideoFrame(NULL);
	encoder->writeAudioFrame(NULL, 0);
	encoder->close();

	/* Release renderers */
	instRec->sequenceRenderSuite->ReleaseVideoRenderer(exID, videoRenderID);
	instRec->sequenceAudioSuite->ReleaseAudioRenderer(exID, audioRenderID);

	encoder->~Encoder();

	return result;
}