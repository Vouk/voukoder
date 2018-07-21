#include "Plugin.h"
#include "VideoRenderer.h"
#include "AudioRenderer.h"

DllExport PREMPLUGENTRY xSDKExport(csSDK_int32 selector, exportStdParms *stdParmsP, void *param1, void	*param2)
{
	switch (selector)
	{
	case exSelStartup:
	{
		exExporterInfoRec *infoRecP = reinterpret_cast<exExporterInfoRec*>(param1);
		infoRecP->fileType = 'VOUK';
		prUTF16CharCopy(infoRecP->fileTypeName, VKDR_APPNAME);
		infoRecP->classID = 'VOUK';
		infoRecP->exportReqIndex = 0;
		infoRecP->wantsNoProgressBar = kPrFalse;
		infoRecP->hideInUI = kPrFalse;
		infoRecP->doesNotSupportAudioOnly = kPrTrue;
		infoRecP->canExportVideo = kPrTrue;
		infoRecP->canExportAudio = kPrTrue;
		infoRecP->interfaceVersion = EXPORTMOD_VERSION;
		infoRecP->isCacheable = kPrFalse;

		Logger::vkdrInit();

		return malNoError;
	}

	case exSelBeginInstance:
	{
		exExporterInstanceRec *instanceRecP = reinterpret_cast<exExporterInstanceRec*>(param1);

		Plugin *plugin = new Plugin(instanceRecP->exporterPluginID);

		prMALError error = plugin->beginInstance(stdParmsP->getSPBasicSuite(), instanceRecP);
		if (error == malNoError)
		{
			instanceRecP->privateData = reinterpret_cast<void*>(plugin);
		}

		return error;
	}

	case exSelEndInstance:
	{
		const exExporterInstanceRec *instanceRecP = reinterpret_cast<exExporterInstanceRec*>(param1);
		Plugin *plugin = reinterpret_cast<Plugin*>(instanceRecP->privateData);
		
		prMALError error = plugin->endInstance();
		if (error == malNoError)
		{
			delete (plugin);
		}

		return error;
	}

	case exSelGenerateDefaultParams:
	{
		exGenerateDefaultParamRec *instanceRecP = reinterpret_cast<exGenerateDefaultParamRec*>(param1);
		return (reinterpret_cast<Plugin*>(instanceRecP->privateData))->generateDefaultParams(instanceRecP);
	}

	case exSelPostProcessParams:
	{
		exPostProcessParamsRec *instanceRecP = reinterpret_cast<exPostProcessParamsRec*>(param1);
		return (reinterpret_cast<Plugin*>(instanceRecP->privateData))->postProcessParams(instanceRecP);
	}

	case exSelValidateParamChanged:
	{
		exParamChangedRec *instanceRecP = reinterpret_cast<exParamChangedRec*>(param1);
		return (reinterpret_cast<Plugin*>(instanceRecP->privateData))->validateParamChanged(instanceRecP);
	}
	case exSelGetParamSummary:
	{
		exParamSummaryRec *instanceRecP = reinterpret_cast<exParamSummaryRec*>(param1);
		return (reinterpret_cast<Plugin*>(instanceRecP->privateData))->getParamSummary(instanceRecP);
	}
	case exSelExport:
	{
		exDoExportRec *instanceRecP = reinterpret_cast<exDoExportRec*>(param1);
		return (reinterpret_cast<Plugin*>(instanceRecP->privateData))->doExport(instanceRecP);
	}
	case exSelQueryExportFileExtension:
	{
		exQueryExportFileExtensionRec *instanceRecP = reinterpret_cast<exQueryExportFileExtensionRec*>(param1);
		return (reinterpret_cast<Plugin*>(instanceRecP->privateData))->queryExportFileExtension(instanceRecP);
	}
	case exSelQueryOutputSettings:
	{
		exQueryOutputSettingsRec *instanceRecP = reinterpret_cast<exQueryOutputSettingsRec*>(param1);
		return (reinterpret_cast<Plugin*>(instanceRecP->privateData))->queryOutputSettings(instanceRecP);
	}
	case exSelValidateOutputSettings:
	{
		exValidateOutputSettingsRec *instanceRecP = reinterpret_cast<exValidateOutputSettingsRec*>(param1);
		return (reinterpret_cast<Plugin*>(instanceRecP->privateData))->validateOutputSettings(instanceRecP);
	}
	case exSelParamButton:
		exParamButtonRec *instanceRecP = reinterpret_cast<exParamButtonRec*>(param1);
		return (reinterpret_cast<Plugin*>(instanceRecP->privateData))->buttonAction(instanceRecP);
	}

	return exportReturn_Unsupported;
}

Plugin::Plugin(csSDK_uint32 pluginId):
	pluginId(pluginId)
{
	Version version;
	version.number.major = VKDR_VERSION_MAJOR;
	version.number.minor = VKDR_VERSION_MINOR;
	version.number.patch = VKDR_VERSION_PATCH;
	
	pluginUpdate = new PluginUpdate;
	Config::CheckForUpdate(version, pluginUpdate);

	config = new Config();
	gui = new GUI(pluginId, config, pluginUpdate, VKDR_PARAM_VERSION);
	logger = new Logger(pluginId);
}

Plugin::~Plugin()
{
	delete(gui);
	delete(pluginUpdate);
	delete(config);
	delete(logger);
}

prMALError Plugin::beginInstance(SPBasicSuite *spBasicSuite, exExporterInstanceRec *instanceRecP)
{
	this->spBasicSuite = spBasicSuite;
	SPErr spError;

	PrSDKMemoryManagerSuite *memorySuite;
	spError = spBasicSuite->AcquireSuite(kPrSDKMemoryManagerSuite, kPrSDKMemoryManagerSuiteVersion, const_cast<const void**>(reinterpret_cast<void**>(&memorySuite)));
	suites = reinterpret_cast<PrSuites*>(memorySuite->NewPtrClear(sizeof(PrSuites)));
	suites->memorySuite = memorySuite;
	
	spError = spBasicSuite->AcquireSuite(kPrSDKTimeSuite, kPrSDKTimeSuiteVersion, const_cast<const void**>(reinterpret_cast<void**>(&suites->timeSuite)));
	spError = spBasicSuite->AcquireSuite(kPrSDKExportParamSuite, kPrSDKExportParamSuiteVersion, const_cast<const void**>(reinterpret_cast<void**>(&suites->exportParamSuite)));
	spError = spBasicSuite->AcquireSuite(kPrSDKExportInfoSuite, kPrSDKExportInfoSuiteVersion, const_cast<const void**>(reinterpret_cast<void**>(&suites->exportInfoSuite)));
	spError = spBasicSuite->AcquireSuite(kPrSDKSequenceAudioSuite, kPrSDKSequenceAudioSuiteVersion, const_cast<const void**>(reinterpret_cast<void**>(&suites->sequenceAudioSuite)));
	spError = spBasicSuite->AcquireSuite(kPrSDKExportFileSuite, kPrSDKExportFileSuiteVersion, const_cast<const void**>(reinterpret_cast<void**>(&suites->exportFileSuite)));
	spError = spBasicSuite->AcquireSuite(kPrSDKPPixSuite, kPrSDKPPixSuiteVersion, const_cast<const void**>(reinterpret_cast<void**>(&suites->ppixSuite)));
	spError = spBasicSuite->AcquireSuite(kPrSDKPPix2Suite, kPrSDKPPix2SuiteVersion, const_cast<const void**>(reinterpret_cast<void**>(&suites->ppix2Suite)));
	spError = spBasicSuite->AcquireSuite(kPrSDKExportProgressSuite, kPrSDKExportProgressSuiteVersion, const_cast<const void**>(reinterpret_cast<void**>(&suites->exportProgressSuite)));
	spError = spBasicSuite->AcquireSuite(kPrSDKWindowSuite, kPrSDKWindowSuiteVersion, const_cast<const void**>(reinterpret_cast<void**>(&suites->windowSuite)));
	spError = spBasicSuite->AcquireSuite(kPrSDKExporterUtilitySuite, kPrSDKExporterUtilitySuiteVersion, const_cast<const void**>(reinterpret_cast<void**>(&suites->exporterUtilitySuite)));
	
	return spError;
}

prMALError Plugin::endInstance()
{
	prMALError result = malNoError;
	
	result = spBasicSuite->ReleaseSuite(kPrSDKTimeSuite, kPrSDKTimeSuiteVersion);
	result = spBasicSuite->ReleaseSuite(kPrSDKExportParamSuite, kPrSDKExportParamSuiteVersion);
	result = spBasicSuite->ReleaseSuite(kPrSDKExportInfoSuite, kPrSDKExportInfoSuiteVersion);
	result = spBasicSuite->ReleaseSuite(kPrSDKSequenceAudioSuite, kPrSDKSequenceAudioSuiteVersion);
	result = spBasicSuite->ReleaseSuite(kPrSDKExportFileSuite, kPrSDKExportFileSuiteVersion);
	result = spBasicSuite->ReleaseSuite(kPrSDKPPixSuite, kPrSDKPPixSuiteVersion);
	result = spBasicSuite->ReleaseSuite(kPrSDKPPix2Suite, kPrSDKPPix2SuiteVersion);
	result = spBasicSuite->ReleaseSuite(kPrSDKExportProgressSuite, kPrSDKExportProgressSuiteVersion);
	result = spBasicSuite->ReleaseSuite(kPrSDKWindowSuite, kPrSDKWindowSuiteVersion);
	result = spBasicSuite->ReleaseSuite(kPrSDKMemoryManagerSuite, kPrSDKMemoryManagerSuiteVersion);

	return result;
}

prMALError Plugin::generateDefaultParams(exGenerateDefaultParamRec *instanceRecP)
{
	return gui->createParameters(suites->exportParamSuite, suites->exportInfoSuite, suites->timeSuite);
}

prMALError Plugin::postProcessParams(exPostProcessParamsRec *instanceRecP)
{
	return gui->updateParameters(suites->exportParamSuite, suites->timeSuite);
}

prMALError Plugin::queryOutputSettings(exQueryOutputSettingsRec *outputSettingsRecP)
{
	csSDK_uint32 bitrate = 0;

	ExportSettings exportSettings;
	gui->getExportSettings(suites->exportParamSuite, &exportSettings);

	if (outputSettingsRecP->inExportVideo)
	{
		exParamValues width;
		suites->exportParamSuite->GetParamValue(pluginId, groupIndex, ADBEVideoWidth, &width);
		outputSettingsRecP->outVideoWidth = width.value.intValue;

		exParamValues height;
		suites->exportParamSuite->GetParamValue(pluginId, groupIndex, ADBEVideoHeight, &height);
		outputSettingsRecP->outVideoHeight = height.value.intValue;

		exParamValues frameRate;
		suites->exportParamSuite->GetParamValue(pluginId, groupIndex, ADBEVideoFPS, &frameRate);
		outputSettingsRecP->outVideoFrameRate = frameRate.value.timeValue;

		exParamValues pixelAspectRatio;
		suites->exportParamSuite->GetParamValue(pluginId, groupIndex, ADBEVideoAspect, &pixelAspectRatio);
		outputSettingsRecP->outVideoAspectNum = pixelAspectRatio.value.ratioValue.numerator;
		outputSettingsRecP->outVideoAspectDen = pixelAspectRatio.value.ratioValue.denominator;

		exParamValues fieldType;
		suites->exportParamSuite->GetParamValue(pluginId, groupIndex, ADBEVideoFieldType, &fieldType);
		outputSettingsRecP->outVideoFieldType = fieldType.value.intValue;

		// Calculate video bitrate
		AVDictionary *dictionary = NULL;
		if (av_dict_parse_string(&dictionary, exportSettings.videoOptions.c_str(), "=", ",", 0) == 0)
		{
			AVDictionaryEntry *entry = av_dict_get(dictionary, "b", NULL, NULL);
			if (entry != NULL)
			{
				bitrate += atoi(entry->value);
			}
		}
	}

	if (outputSettingsRecP->inExportAudio)
	{
		exParamValues sampleRate;
		suites->exportParamSuite->GetParamValue(pluginId, groupIndex, ADBEAudioRatePerSecond, &sampleRate);
		outputSettingsRecP->outAudioSampleRate = sampleRate.value.floatValue;
		outputSettingsRecP->outAudioSampleType = kPrAudioSampleType_32BitFloat;

		exParamValues channelNum;
		suites->exportParamSuite->GetParamValue(pluginId, groupIndex, ADBEAudioNumChannels, &channelNum);
		outputSettingsRecP->outAudioChannelType = (PrAudioChannelType)channelNum.value.intValue;

		// Calculate audio bitrate only if we have a video bitrate already
		if (bitrate > 0)
		{
			if (exportSettings.audioCodecName == "pcm_s16le")
			{
				bitrate += channelNum.value.intValue = 16 * channelNum.value.intValue * sampleRate.value.floatValue;
			}
			else if (exportSettings.audioCodecName == "pcm_s24le")
			{
				bitrate += channelNum.value.intValue = 24 * channelNum.value.intValue * sampleRate.value.floatValue;
			}
			else
			{
				AVDictionary *dictionary = NULL;
				if (av_dict_parse_string(&dictionary, exportSettings.audioOptions.c_str(), "=", ",", 0) == 0)
				{
					AVDictionaryEntry *entry = av_dict_get(dictionary, "b", NULL, NULL);
					if (entry != NULL)
					{
						bitrate += atoi(entry->value);
					}
				}
			}
		}
	}

	outputSettingsRecP->outBitratePerSecond = bitrate / 1000;

	return malNoError;
}

prMALError Plugin::queryExportFileExtension(exQueryExportFileExtensionRec *exportFileExtensionRecP)
{
	exParamValues multiplexer;
	suites->exportParamSuite->GetParamValue(pluginId, groupIndex, FFMultiplexer, &multiplexer);

	for (const MultiplexerInfo multiplexerInfo : config->Multiplexers)
	{
		if (multiplexerInfo.id == multiplexer.value.intValue)
		{
			prUTF16CharCopy(
				exportFileExtensionRecP->outFileExtension,
				wstring(multiplexerInfo.extension.begin(), multiplexerInfo.extension.end()).c_str());

			return malNoError;
		}
	}

	return malUnknownError;
}

prMALError Plugin::validateParamChanged(exParamChangedRec *paramRecP)
{
	return gui->onParamChange(suites->exportParamSuite, paramRecP);
}

prMALError Plugin::getParamSummary(exParamSummaryRec *summaryRecP)
{
	EncoderSettings videoEncoderSettings;
	gui->getCurrentEncoderSettings(suites->exportParamSuite, prFieldsInvalid, EncoderType::Video, &videoEncoderSettings);

	exParamValues width, height, frameRate, pixelAspectRatio;
	suites->exportParamSuite->GetParamValue(pluginId, groupIndex, ADBEVideoWidth, &width);
	suites->exportParamSuite->GetParamValue(pluginId, groupIndex, ADBEVideoHeight, &height);
	suites->exportParamSuite->GetParamValue(pluginId, groupIndex, ADBEVideoFPS, &frameRate);
	suites->exportParamSuite->GetParamValue(pluginId, groupIndex, ADBEVideoAspect, &pixelAspectRatio);

	PrTime ticksPerSecond;
	suites->timeSuite->GetTicksPerSecond(&ticksPerSecond);

	const float par = static_cast<float>(pixelAspectRatio.value.ratioValue.numerator) / static_cast<float>(pixelAspectRatio.value.ratioValue.denominator);
	const float fps = static_cast<float>(ticksPerSecond) / static_cast<float>(frameRate.value.timeValue);

	wstringstream videoSummary;
	videoSummary << wstring(videoEncoderSettings.text.begin(), videoEncoderSettings.text.end()) << ", ";
	videoSummary << width.value.intValue << "x" << height.value.intValue;
	videoSummary << std::fixed;
	videoSummary.precision(2);
	videoSummary << " (" << par << "), ";
	videoSummary << fps << " fps";

	prUTF16CharCopy(summaryRecP->videoSummary, videoSummary.str().c_str());

	EncoderSettings audioEncoderSettings;
	gui->getCurrentEncoderSettings(suites->exportParamSuite, prFieldsInvalid, EncoderType::Audio, &audioEncoderSettings);

	exParamValues sampleRate, channelType;
	suites->exportParamSuite->GetParamValue(pluginId, groupIndex, ADBEAudioRatePerSecond, &sampleRate);
	suites->exportParamSuite->GetParamValue(pluginId, groupIndex, ADBEAudioNumChannels, &channelType);

	wstringstream audioSummary;
	audioSummary << wstring(audioEncoderSettings.text.begin(), audioEncoderSettings.text.end()) << ", ";
	audioSummary.precision(0);
	audioSummary << sampleRate.value.floatValue << " Hz, ";

	switch (channelType.value.intValue)
	{
	case kPrAudioChannelType_Mono:
		audioSummary << "Mono";
		break;
	case kPrAudioChannelType_Stereo:
		audioSummary << "Stereo";
		break;
	case kPrAudioChannelType_51:
		audioSummary << "5.1 Surround";
		break;
	default:
		audioSummary << "Unknown channel type";
	}
	prUTF16CharCopy(summaryRecP->audioSummary, audioSummary.str().c_str());

	prUTF16CharCopy(summaryRecP->bitrateSummary, L"");

	return malNoError;
}

prMALError Plugin::validateOutputSettings(exValidateOutputSettingsRec *outputSettingsRecP)
{
	prMALError result = malNoError;

	ExportSettings exportSettings;
	exportSettings.application = "Settings validator";
	gui->getExportSettings(suites->exportParamSuite, &exportSettings);
	
	Encoder encoder(exportSettings);
	if (encoder.testSettings() < 0)
	{
		char charPath[MAX_PATH];
		if (GetTempPathA(MAX_PATH, charPath))
		{
			stringstream buffer;
			buffer << "FFMpeg rejected the current configuration.\n\n";
			buffer << "Please take a look in your voukoder logfile:\n\n";
			buffer << charPath << "voukoder.log\n\n";
			buffer << "Muxer: " << exportSettings.muxerName << "\n";
			buffer << "Video encoder: " << exportSettings.videoCodecName << "\n";
			buffer << "Audio encoder: " << exportSettings.audioCodecName << "\n";
			buffer << "\n(Press CTRL + C to copy this information to the clipboard.)";

			gui->showDialog(
				suites->windowSuite,
				buffer.str(),
				"Voukoder Export Error");
		}

		result = malUnknownError;
	}

	return result;
}

prMALError Plugin::doExport(exDoExportRec *exportRecP)
{
	prUTF16Char prFilename[kPrMaxPath];
	csSDK_int32 prFilenameLength = kPrMaxPath;
	suites->exportFileSuite->GetPlatformPath(exportRecP->fileObject, &prFilenameLength, prFilename);

	wstring wstr(prFilename);
	int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
	std::string filename(size_needed, 0);
	WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &filename[0], size_needed, NULL, NULL);

	const wstring app(VKDR_APPNAME);

	ExportSettings exportSettings;
	exportSettings.filename = filename;
	exportSettings.application = string(app.begin(), app.end()) + " (Premiere)";
	exportSettings.exportAudio = exportRecP->exportAudio == kPrTrue;
	gui->getExportSettings(suites->exportParamSuite, &exportSettings);

	VideoRenderer videoRenderer(
		pluginId,
		exportSettings.width, 
		exportSettings.height,
		suites->ppixSuite, 
		suites->ppix2Suite, 
		suites->memorySuite, 
		suites->exporterUtilitySuite);

	exParamValues audioChannelType, ticksPerFrame;
	suites->exportParamSuite->GetParamValue(pluginId, groupIndex, ADBEAudioNumChannels, &audioChannelType);
	suites->exportParamSuite->GetParamValue(pluginId, groupIndex, ADBEVideoFPS, &ticksPerFrame);

	AudioRenderer audioRenderer(
		pluginId,
		exportRecP->startTime,
		exportRecP->endTime,
		(PrAudioChannelType)audioChannelType.value.intValue,
		(float)exportSettings.audioTimebase.den,
		ticksPerFrame.value.timeValue,
		suites->sequenceAudioSuite,
		suites->timeSuite,
		suites->memorySuite);

	PrPixelFormat pixelFormat = VideoRenderer::GetTargetRenderFormat(exportSettings);
	
	if (pixelFormat == PrPixelFormat_Invalid)
		return suiteError_RenderInvalidPixelFormat;

	Encoder encoder(exportSettings);

	// Exporting loop with callback per rendered video frame
	int ret = videoRenderer.render(pixelFormat, exportRecP->startTime, exportRecP->endTime, exportSettings.passes, [&](EncoderData *encoderData)
	{
		int res = 0;

		// Starting a new pass
		if (encoder.pass == 0 || (exportSettings.passes > 1 && encoderData->pass > encoder.pass))
		{
			// Close encoder for current pass
			if (encoderData->pass > 1)
			{
				encoder.close(true);
			}

			encoder.pass = encoderData->pass;

			// Open encoder for new pass
			if ((res = encoder.open()) < 0)
			{
				LOG(WARNING) << "Unable to open the encoder for pass #" << encoder.pass << ". (Error code: " << res << ")";

				return false;
			}

			if (exportSettings.exportAudio)
				audioRenderer.reset();
		}

		// Write video frame first
		if ((res = encoder.writeVideoFrame(encoderData)) < 0)
		{
			LOG(WARNING) << "Failed writing video frame #" << encoderData->frame << ". (Error code: " << res << ")";

			return false;
		}

		// Write audio frames if enabled
		if (exportSettings.exportAudio)
		{
			csSDK_uint32 size = encoder.getAudioFrameSize();

			// Does the muxer want more audio frames?
			while (encoder.getNextFrameType() == FrameType::AudioFrame)
			{
				float **samples = audioRenderer.getSamples(&size, kPrFalse);

				// Abort loop if no samles are left
				if (size <= 0)
				{
					LOG(INFO) << "Aborting audio renderer loop: No audio samlples left!";

					break;
				}

				// Write the audio frames
				if ((res = encoder.writeAudioFrame(samples, size)) < 0)
				{
					LOG(WARNING) << "Failed writing audio frames for video frame #" << encoderData->frame << ". (Error code>: " << res << ")";

					return false;
				}
			}
		}

		return true;
	});

	encoder.close(ret == suiteError_NoError || ret == 0x00000001);

	return malNoError;
}

prMALError Plugin::buttonAction(exParamButtonRec *paramButtonRecP)
{
	if (strcmp(paramButtonRecP->buttonParamIdentifier, VKDRUpdateButton) == 0)
	{
		ShellExecuteA(0, 0, pluginUpdate->url.c_str(), 0, 0, SW_SHOW);
	}

	return malNoError;
}