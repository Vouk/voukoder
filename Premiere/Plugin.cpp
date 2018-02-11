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
	}

	return exportReturn_Unsupported;
}

Plugin::Plugin(csSDK_uint32 pluginId):
	pluginId(pluginId)
{
	config = new Config();
	gui = new GUI(pluginId, config);
}

Plugin::~Plugin()
{
	delete(gui);
	delete(config);
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
	return gui->init(suites->exportParamSuite, suites->exportInfoSuite, suites->timeSuite, VKDR_PARAM_VERSION);
}

prMALError Plugin::postProcessParams(exPostProcessParamsRec *instanceRecP)
{
	return gui->update(suites->exportParamSuite, suites->timeSuite);
}

prMALError Plugin::queryOutputSettings(exQueryOutputSettingsRec *outputSettingsRecP)
{
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
	}

	if (outputSettingsRecP->inExportAudio)
	{
		exParamValues sampleRate;
		suites->exportParamSuite->GetParamValue(pluginId, groupIndex, ADBEAudioRatePerSecond, &sampleRate);
		outputSettingsRecP->outAudioSampleRate = sampleRate.value.floatValue;
		outputSettingsRecP->outAudioSampleType = kPrAudioSampleType_32BitFloat;

		exParamValues channelType;
		suites->exportParamSuite->GetParamValue(pluginId, groupIndex, ADBEAudioNumChannels, &channelType);
		outputSettingsRecP->outAudioChannelType = (PrAudioChannelType)channelType.value.intValue;
	}

	// Do not display filesize estimation
	outputSettingsRecP->outBitratePerSecond = 0;

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
	gui->getCurrentEncoderSettings(suites->exportParamSuite, EncoderType::Video, &videoEncoderSettings);
	prUTF16CharCopy(summaryRecP->videoSummary, wstring(videoEncoderSettings.text.begin(), videoEncoderSettings.text.end()).c_str());

	EncoderSettings audioEncoderSettings;
	gui->getCurrentEncoderSettings(suites->exportParamSuite, EncoderType::Audio, &audioEncoderSettings);
	prUTF16CharCopy(summaryRecP->audioSummary, wstring(audioEncoderSettings.text.begin(), audioEncoderSettings.text.end()).c_str());

	prUTF16CharCopy(summaryRecP->bitrateSummary, L"");

	return malNoError;
}

prMALError Plugin::validateOutputSettings(exValidateOutputSettingsRec *outputSettingsRecP)
{
	prMALError result = malNoError;

	ExportSettings exportSettings;
	gui->getExportSettings(suites->exportParamSuite, &exportSettings);
	
	Encoder encoder(exportSettings);
	if (encoder.testSettings() < 0)
	{
		stringstream buffer;
		buffer << "FFMpeg rejected the current configuration.\n\n";

		vector<string> logs = logger.getLastEntries(5);
		for (string msg : logs)
		{
			buffer << msg << "\n";
		}

		buffer << "\nMuxer: " << exportSettings.muxerName << "\n";
		buffer << "Video encoder: " << exportSettings.videoCodecName << "\n";
		buffer << "Audio encoder: " << exportSettings.audioCodecName << "\n";
		buffer << "\n(Press CTRL + C to copy this information to the clipboard.)";

		gui->showDialog(
			suites->windowSuite,
			buffer.str(),
			"Voukoder Export Error");

		result = malUnknownError;
	}

	return result;
}

prMALError Plugin::doExport(exDoExportRec *exportRecP)
{
	prUTF16Char prFilename[kPrMaxPath];
	csSDK_int32 prFilenameLength = kPrMaxPath;
	suites->exportFileSuite->GetPlatformPath(exportRecP->fileObject, &prFilenameLength, prFilename);

	// -------------------- TODO: UTF16 support
	size_t i;
	char *filename = (char*)malloc(kPrMaxPath);
	wcstombs_s(&i, filename, (size_t)kPrMaxPath, prFilename, (size_t)kPrMaxPath);
	// ----------------------------------------

	ExportSettings exportSettings;
	exportSettings.filename = filename;
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
		(float)exportSettings.audioTimebaseDen,
		ticksPerFrame.value.timeValue,
		suites->sequenceAudioSuite,
		suites->timeSuite,
		suites->memorySuite);

	PrPixelFormat pixelFormat = VideoRenderer::GetTargetRenderFormat(exportSettings);
	
	if (pixelFormat == PrPixelFormat_Invalid)
		return suiteError_RenderInvalidPixelFormat;

	Encoder encoder(exportSettings);
	
	int ret = videoRenderer.render(
		pixelFormat, 
		exportRecP->startTime, 
		exportRecP->endTime, 
		exportSettings.passes,
		[&](EncoderData *encoderData)
	{
		// Prepare the encoder for this iteration
		if (encoder.prepare(encoderData) < 0)
		{
			return false;
		}

		if (encoder.writeVideoFrame(encoderData) == 0)
		{
			// Let the encoder choose how many audio frames we need
			while (encoder.getNextFrameType() == FrameType::AudioFrame && audioRenderer.samplesInBuffer())
			{
				csSDK_uint32 size = 0;
				float **samples = audioRenderer.getSamples(&size, kPrFalse);
				if (size > 0)
				{
					if (encoder.writeAudioFrame(samples, size) < 0)
					{
						return false;
					}
				}
			}

			return true;
		}

		return false;
	});

	encoder.close(ret == suiteError_NoError);

	return malNoError;
}
