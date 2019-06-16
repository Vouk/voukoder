#include "PremierePlugin.h"
#include <sstream>
#include <functional>
#include "AudioRenderer.h"
#include "VideoRenderer.h"
#include <EncoderEngine.h>
#include <LanguageUtils.h>
#include <PluginUpdate.h>
#include <premiere_cs6/PrSDKAppInfoSuite.h>
#include <Log.h>

#ifdef _DEBUG
static inline void AvCallback(void*, int level, const char* szFmt, va_list varg)
{
	char logbuf[2000];
	vsnprintf(logbuf, sizeof(logbuf), szFmt, varg);
	logbuf[sizeof(logbuf) - 1] = '\0';

	OutputDebugStringA(logbuf);
}
#endif

DllExport PREMPLUGENTRY xSDKExport(csSDK_int32 selector, exportStdParms* stdParmsP, void* param1, void* param2)
{
	switch (selector)
	{
	case exSelStartup:
	{
#ifdef _DEBUG
		av_log_set_level(AV_LOG_TRACE);
		av_log_set_callback(AvCallback);
#endif

		int fourCC = 0;
		VersionInfo version = { 0, 0, 0 };

		PrSDKAppInfoSuite* appInfoSuite = NULL;
		stdParmsP->getSPBasicSuite()->AcquireSuite(kPrSDKAppInfoSuite, kPrSDKAppInfoSuiteVersion, (const void**)& appInfoSuite);

		if (appInfoSuite)
		{
			appInfoSuite->GetAppInfo(PrSDKAppInfoSuite::kAppInfo_AppFourCC, (void*)& fourCC);
			appInfoSuite->GetAppInfo(PrSDKAppInfoSuite::kAppInfo_Version, (void*)& version);

			stdParmsP->getSPBasicSuite()->ReleaseSuite(kPrSDKAppInfoSuite, kPrSDKAppInfoSuiteVersion);

			wxString hostApp;

			// Set host application name
			if (fourCC == kAppPremierePro)
			{
				hostApp << "Premiere Pro";
			}
			else if (fourCC == kAppMediaEncoder)
			{
				hostApp << "Media Encoder";
			}
			else
			{
				hostApp << "Adobe (Unsupported)";
			}

			// Add version information
			vkLogInfoVA("Plugin running in " + hostApp + " %d.%d.%d",
				version.major,
				version.minor,
				version.build);
			vkLogInfoVA("---------------------------------------------", NULL);

			if (fourCC == kAppAfterEffects)
				return exportReturn_IterateExporterDone;
		}

		exExporterInfoRec* infoRecP = reinterpret_cast<exExporterInfoRec*>(param1);
		infoRecP->fileType = 'VK02';
		::prUTF16CharCopy(infoRecP->fileTypeName, VKDR_APPNAME);
		infoRecP->classID = 'VK02';
		infoRecP->exportReqIndex = 0;
		infoRecP->wantsNoProgressBar = kPrFalse;
		infoRecP->hideInUI = kPrFalse;
		infoRecP->doesNotSupportAudioOnly = kPrFalse;
		infoRecP->canExportVideo = kPrTrue;
		infoRecP->canExportAudio = kPrTrue;
		infoRecP->interfaceVersion = EXPORTMOD_VERSION;
		infoRecP->isCacheable = kPrTrue;

		if (stdParmsP->interfaceVer >= 6 &&
			((fourCC == kAppPremierePro && version.major >= 9) ||
			(fourCC == kAppMediaEncoder && version.major >= 9)))
		{
#if EXPORTMOD_VERSION >= 6
			infoRecP->canConformToMatchParams = kPrTrue;
#else
			csSDK_uint32* info = &infoRecP->isCacheable;
			info[1] = kPrTrue;
#endif
		}

		return malNoError;
	}

	case exSelShutdown:
	{
		return malNoError;
	}

	case exSelBeginInstance:
	{
		exExporterInstanceRec* instanceRecP = reinterpret_cast<exExporterInstanceRec*>(param1);

		CPremierePluginApp* connector = new CPremierePluginApp(instanceRecP->exporterPluginID);
		prMALError error = connector->beginInstance(stdParmsP->getSPBasicSuite(), instanceRecP);
		if (error == malNoError)
		{
			instanceRecP->privateData = reinterpret_cast<void*>(connector);
		}

		return error;
	}

	case exSelEndInstance:
	{
		const exExporterInstanceRec* instanceRecP = reinterpret_cast<exExporterInstanceRec*>(param1);

		CPremierePluginApp* connector = reinterpret_cast<CPremierePluginApp*>(instanceRecP->privateData);
		prMALError error = connector->endInstance();
		if (error == malNoError)
		{
			delete(connector);
		}

		return error;
	}

	case exSelGenerateDefaultParams:
	{
		exGenerateDefaultParamRec* instanceRecP = reinterpret_cast<exGenerateDefaultParamRec*>(param1);
		return (reinterpret_cast<CPremierePluginApp*>(instanceRecP->privateData))->generateDefaultParams(instanceRecP);
	}

	case exSelPostProcessParams:
	{
		exPostProcessParamsRec* instanceRecP = reinterpret_cast<exPostProcessParamsRec*>(param1);
		return (reinterpret_cast<CPremierePluginApp*>(instanceRecP->privateData))->postProcessParams(instanceRecP);
	}

	case exSelValidateParamChanged:
	{
		exParamChangedRec* instanceRecP = reinterpret_cast<exParamChangedRec*>(param1);
		return (reinterpret_cast<CPremierePluginApp*>(instanceRecP->privateData))->validateParamChanged(instanceRecP);
	}
	case exSelGetParamSummary:
	{
		exParamSummaryRec* instanceRecP = reinterpret_cast<exParamSummaryRec*>(param1);
		return (reinterpret_cast<CPremierePluginApp*>(instanceRecP->privateData))->getParamSummary(instanceRecP);
	}
	case exSelExport:
	{
		exDoExportRec* instanceRecP = reinterpret_cast<exDoExportRec*>(param1);
		return (reinterpret_cast<CPremierePluginApp*>(instanceRecP->privateData))->StartExport(instanceRecP);
	}
	case exSelQueryExportFileExtension:
	{
		exQueryExportFileExtensionRec* instanceRecP = reinterpret_cast<exQueryExportFileExtensionRec*>(param1);
		return (reinterpret_cast<CPremierePluginApp*>(instanceRecP->privateData))->queryExportFileExtension(instanceRecP);
	}
	case exSelQueryOutputSettings:
	{
		exQueryOutputSettingsRec* instanceRecP = reinterpret_cast<exQueryOutputSettingsRec*>(param1);
		return (reinterpret_cast<CPremierePluginApp*>(instanceRecP->privateData))->queryOutputSettings(instanceRecP);
	}
	case exSelValidateOutputSettings:
	{
		exValidateOutputSettingsRec* instanceRecP = reinterpret_cast<exValidateOutputSettingsRec*>(param1);
		return (reinterpret_cast<CPremierePluginApp*>(instanceRecP->privateData))->validateOutputSettings(instanceRecP);
	}
	case exSelParamButton:
	{
		exParamButtonRec* instanceRecP = reinterpret_cast<exParamButtonRec*>(param1);
		return (reinterpret_cast<CPremierePluginApp*>(instanceRecP->privateData))->buttonAction(instanceRecP);
	}
	}

	return exportReturn_Unsupported;
}

CPremierePluginApp::CPremierePluginApp(csSDK_uint32 pluginId) :
	pluginId(pluginId)
{}

prMALError CPremierePluginApp::beginInstance(SPBasicSuite * spBasicSuite, exExporterInstanceRec * instanceRecP)
{
	suites.basicSuite = spBasicSuite;

	prMALError ret;
	ret = spBasicSuite->AcquireSuite(kPrSDKMemoryManagerSuite, kPrSDKMemoryManagerSuiteVersion, const_cast<const void**>(reinterpret_cast<void**>(&suites.memorySuite)));
	ret = spBasicSuite->AcquireSuite(kPrSDKTimeSuite, kPrSDKTimeSuiteVersion, const_cast<const void**>(reinterpret_cast<void**>(&suites.timeSuite)));
	ret = spBasicSuite->AcquireSuite(kPrSDKExportParamSuite, kPrSDKExportParamSuiteVersion, const_cast<const void**>(reinterpret_cast<void**>(&suites.exportParamSuite)));
	ret = spBasicSuite->AcquireSuite(kPrSDKExportInfoSuite, kPrSDKExportInfoSuiteVersion, const_cast<const void**>(reinterpret_cast<void**>(&suites.exportInfoSuite)));
	ret = spBasicSuite->AcquireSuite(kPrSDKSequenceAudioSuite, kPrSDKSequenceAudioSuiteVersion, const_cast<const void**>(reinterpret_cast<void**>(&suites.sequenceAudioSuite)));
	ret = spBasicSuite->AcquireSuite(kPrSDKExportFileSuite, kPrSDKExportFileSuiteVersion, const_cast<const void**>(reinterpret_cast<void**>(&suites.exportFileSuite)));
	ret = spBasicSuite->AcquireSuite(kPrSDKPPixSuite, kPrSDKPPixSuiteVersion, const_cast<const void**>(reinterpret_cast<void**>(&suites.ppixSuite)));
	ret = spBasicSuite->AcquireSuite(kPrSDKPPix2Suite, kPrSDKPPix2SuiteVersion, const_cast<const void**>(reinterpret_cast<void**>(&suites.ppix2Suite)));
	ret = spBasicSuite->AcquireSuite(kPrSDKExportProgressSuite, kPrSDKExportProgressSuiteVersion, const_cast<const void**>(reinterpret_cast<void**>(&suites.exportProgressSuite)));
	ret = spBasicSuite->AcquireSuite(kPrSDKWindowSuite, kPrSDKWindowSuiteVersion, const_cast<const void**>(reinterpret_cast<void**>(&suites.windowSuite)));
	ret = spBasicSuite->AcquireSuite(kPrSDKExporterUtilitySuite, kPrSDKExporterUtilitySuiteVersion, const_cast<const void**>(reinterpret_cast<void**>(&suites.exporterUtilitySuite)));
	ret = spBasicSuite->AcquireSuite(kPrSDKSequenceInfoSuite, kPrSDKSequenceInfoSuiteVersion, const_cast<const void**>(reinterpret_cast<void**>(&suites.sequenceInfoSuite)));
	ret = spBasicSuite->AcquireSuite(kPrSDKImageProcessingSuite, kPrSDKImageProcessingSuiteVersion, const_cast<const void**>(reinterpret_cast<void**>(&suites.imageProcessingSuite)));

	gui = new Gui(&suites, pluginId);

	return ret;
}

prMALError CPremierePluginApp::endInstance()
{
	if (gui)
		delete(gui);

	prMALError ret;
	ret = suites.basicSuite->ReleaseSuite(kPrSDKTimeSuite, kPrSDKTimeSuiteVersion);
	ret = suites.basicSuite->ReleaseSuite(kPrSDKExportParamSuite, kPrSDKExportParamSuiteVersion);
	ret = suites.basicSuite->ReleaseSuite(kPrSDKExportInfoSuite, kPrSDKExportInfoSuiteVersion);
	ret = suites.basicSuite->ReleaseSuite(kPrSDKSequenceAudioSuite, kPrSDKSequenceAudioSuiteVersion);
	ret = suites.basicSuite->ReleaseSuite(kPrSDKExportFileSuite, kPrSDKExportFileSuiteVersion);
	ret = suites.basicSuite->ReleaseSuite(kPrSDKPPixSuite, kPrSDKPPixSuiteVersion);
	ret = suites.basicSuite->ReleaseSuite(kPrSDKPPix2Suite, kPrSDKPPix2SuiteVersion);
	ret = suites.basicSuite->ReleaseSuite(kPrSDKExportProgressSuite, kPrSDKExportProgressSuiteVersion);
	ret = suites.basicSuite->ReleaseSuite(kPrSDKWindowSuite, kPrSDKWindowSuiteVersion);
	ret = suites.basicSuite->ReleaseSuite(kPrSDKExporterUtilitySuite, kPrSDKWindowSuiteVersion);
	ret = suites.basicSuite->ReleaseSuite(kPrSDKWindowSuite, kPrSDKExporterUtilitySuiteVersion);
	ret = suites.basicSuite->ReleaseSuite(kPrSDKSequenceInfoSuite, kPrSDKSequenceInfoSuiteVersion);
	ret = suites.basicSuite->ReleaseSuite(kPrSDKImageProcessingSuite, kPrSDKImageProcessingSuiteVersion);

	return ret;
}

prMALError CPremierePluginApp::generateDefaultParams(exGenerateDefaultParamRec * instanceRecP)
{
	return gui->Create();
}

prMALError CPremierePluginApp::postProcessParams(exPostProcessParamsRec * instanceRecP)
{
	return gui->Update();
}

prMALError CPremierePluginApp::queryOutputSettings(exQueryOutputSettingsRec * outputSettingsRecP)
{
	csSDK_uint32 bitrate = 0;

	ExportInfo exportInfo;
	bool hasData = gui->ReadEncoderOptions(VKDRVoukoderConfiguration, exportInfo);

	if (outputSettingsRecP->inExportVideo)
	{
		exParamValues width;
		suites.exportParamSuite->GetParamValue(pluginId, outputSettingsRecP->inMultiGroupIndex, ADBEVideoWidth, &width);
		outputSettingsRecP->outVideoWidth = width.value.intValue;

		exParamValues height;
		suites.exportParamSuite->GetParamValue(pluginId, outputSettingsRecP->inMultiGroupIndex, ADBEVideoHeight, &height);
		outputSettingsRecP->outVideoHeight = height.value.intValue;

		exParamValues frameRate;
		suites.exportParamSuite->GetParamValue(pluginId, outputSettingsRecP->inMultiGroupIndex, ADBEVideoFPS, &frameRate);
		outputSettingsRecP->outVideoFrameRate = frameRate.value.timeValue;

		exParamValues pixelAspectRatio;
		suites.exportParamSuite->GetParamValue(pluginId, outputSettingsRecP->inMultiGroupIndex, ADBEVideoAspect, &pixelAspectRatio);
		outputSettingsRecP->outVideoAspectNum = pixelAspectRatio.value.ratioValue.numerator;
		outputSettingsRecP->outVideoAspectDen = pixelAspectRatio.value.ratioValue.denominator;

		exParamValues fieldType;
		suites.exportParamSuite->GetParamValue(pluginId, outputSettingsRecP->inMultiGroupIndex, ADBEVideoFieldType, &fieldType);
		outputSettingsRecP->outVideoFieldType = fieldType.value.intValue;

		// Calculate bitrate
		if (hasData && exportInfo.video.options.find("b") != exportInfo.video.options.end())
		{
			bitrate += atoi(exportInfo.video.options["b"].c_str());
		}
	}

	if (outputSettingsRecP->inExportAudio)
	{
		exParamValues sampleRate;
		suites.exportParamSuite->GetParamValue(pluginId, outputSettingsRecP->inMultiGroupIndex, ADBEAudioRatePerSecond, &sampleRate);
		outputSettingsRecP->outAudioSampleRate = sampleRate.value.floatValue;
		outputSettingsRecP->outAudioSampleType = kPrAudioSampleType_32BitFloat;

		exParamValues channelNum;
		suites.exportParamSuite->GetParamValue(pluginId, outputSettingsRecP->inMultiGroupIndex, ADBEAudioNumChannels, &channelNum);
		outputSettingsRecP->outAudioChannelType = (PrAudioChannelType)channelNum.value.intValue;

		// Calculate bitrate
		if (hasData)
		{
			if (exportInfo.audio.id == "pcm_s16le")
			{
				bitrate += channelNum.value.intValue = 16 * channelNum.value.intValue * sampleRate.value.floatValue;
			}
			else if (exportInfo.audio.id == "pcm_s24le")
			{
				bitrate += channelNum.value.intValue = 24 * channelNum.value.intValue * sampleRate.value.floatValue;
			}
			else if (exportInfo.audio.id == "pcm_s32le")
			{
				bitrate += channelNum.value.intValue = 32 * channelNum.value.intValue * sampleRate.value.floatValue;
			}
			else if (exportInfo.audio.options.find("b") != exportInfo.audio.options.end())
			{
				bitrate += atoi(exportInfo.audio.options["b"].c_str());
			}
		}
	}

	outputSettingsRecP->outUseMaximumRenderPrecision = kPrTrue;
	outputSettingsRecP->outBitratePerSecond = hasData ? bitrate / 1000 : 0;

	return malNoError;
}

prMALError CPremierePluginApp::queryExportFileExtension(exQueryExportFileExtensionRec * exportFileExtensionRecP)
{
	return gui->GetSelectedFileExtension(exportFileExtensionRecP->outFileExtension);
}

prMALError CPremierePluginApp::validateParamChanged(exParamChangedRec * paramRecP)
{
	gui->ParamChange(paramRecP);
	return malNoError;
}

prMALError CPremierePluginApp::getParamSummary(exParamSummaryRec * summaryRecP)
{
	ExportInfo info;
	gui->GetExportInfo(info);

	// Collect video info
	exParamValues width, height, frameRate, pixelAspectRatio;
	suites.exportParamSuite->GetParamValue(pluginId, 0, ADBEVideoWidth, &width);
	suites.exportParamSuite->GetParamValue(pluginId, 0, ADBEVideoHeight, &height);
	suites.exportParamSuite->GetParamValue(pluginId, 0, ADBEVideoFPS, &frameRate);
	suites.exportParamSuite->GetParamValue(pluginId, 0, ADBEVideoAspect, &pixelAspectRatio);

	PrTime ticksPerSecond;
	suites.timeSuite->GetTicksPerSecond(&ticksPerSecond);

	const float par = static_cast<float>(pixelAspectRatio.value.ratioValue.numerator) / static_cast<float>(pixelAspectRatio.value.ratioValue.denominator);
	const float fps = static_cast<float>(ticksPerSecond) / static_cast<float>(frameRate.value.timeValue);

	std::wstringstream videoSummary;
	videoSummary << info.video.id << ", ";
	videoSummary << width.value.intValue << "x" << height.value.intValue;
	videoSummary << std::fixed;
	videoSummary.precision(2);
	videoSummary << " (" << par << "), ";
	videoSummary << fps << " fps";

	prUTF16CharCopy(summaryRecP->videoSummary, videoSummary.str().c_str());

	// Collect audio info
	exParamValues sampleRate, channelType;
	suites.exportParamSuite->GetParamValue(pluginId, 0, ADBEAudioRatePerSecond, &sampleRate);
	suites.exportParamSuite->GetParamValue(pluginId, 0, ADBEAudioNumChannels, &channelType);

	std::wstringstream audioSummary;
	audioSummary << info.audio.id << ", ";
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

	// We don't deliver bitrate data
	prUTF16CharCopy(summaryRecP->bitrateSummary, L"");

	return malNoError;
}

prMALError CPremierePluginApp::validateOutputSettings(exValidateOutputSettingsRec * outputSettingsRecP)
{
	return gui->Validate();
}

const wxString CPremierePluginApp::GetFilename(csSDK_uint32 fileObject)
{
	prUTF16Char prFilename[kPrMaxPath];
	csSDK_int32 prFilenameLength = kPrMaxPath;
	suites.exportFileSuite->GetPlatformPath(fileObject, &prFilenameLength, prFilename);

	return prFilename;
}

prMALError CPremierePluginApp::StartExport(exDoExportRec * exportRecP)
{
	// This should normally not happen (blocked by the UI)
	if (exportRecP->exportVideo == kPrFalse && exportRecP->exportAudio == kPrFalse)
	{
		return gui->ReportMessage("Neither video or audio was selected to export.");
	}

	// Create encoderinfo
	ExportInfo exportInfo;
	exportInfo.video.enabled = exportRecP->exportVideo == kPrTrue;
	exportInfo.audio.enabled = exportRecP->exportAudio == kPrTrue;
	exportInfo.filename = GetFilename(exportRecP->fileObject);
	exportInfo.application = VKDR_APPNAME;

	// Get additional export info
	gui->GetExportInfo(exportInfo);

	// Log video settings
	vkLogSep();
	vkLogInfo("Export started");
	vkLogSep();
	vkLogInfoVA("Frame size:      %dx%d", exportInfo.video.width, exportInfo.video.height);
	vkLogInfoVA("Pixel aspect:    %d:%d", exportInfo.video.sampleAspectRatio.num, exportInfo.video.sampleAspectRatio.den);
	vkLogInfoVA("Frame rate:      %.2f", (float)exportInfo.video.timebase.den / (float)exportInfo.video.timebase.num);

	// Log field order
	switch (exportInfo.video.fieldOrder)
	{
	case AV_FIELD_PROGRESSIVE:
		vkLogInfo("Interlaced:      No");
		break;
	case AV_FIELD_BB:
	case AV_FIELD_BT:
		vkLogInfo("Interlaced:      Bottom first");
		break;
	case AV_FIELD_TT:
	case AV_FIELD_TB:
		vkLogInfo("Interlaced:      Top first");
		break;
	}

	// Log color settings
	vkLogInfoVA("Color range:     %s", av_color_range_name(exportInfo.video.colorRange));
	vkLogInfoVA("Color space:     %s", av_color_space_name(exportInfo.video.colorSpace));
	vkLogInfoVA("Color primaries: %s", av_color_primaries_name(exportInfo.video.colorPrimaries));
	vkLogInfoVA("Color TRC:       %s", av_color_transfer_name(exportInfo.video.colorTransferCharacteristics));

	// Log audio settings
	vkLogInfoVA("Sample rate:     %d", exportInfo.audio.timebase.den / exportInfo.audio.timebase.num);
	vkLogInfoVA("Audio channels:  %d", av_get_channel_layout_nb_channels(exportInfo.audio.channelLayout));
	vkLogSep();

	// Create encoder instance
	EncoderEngine encoder(exportInfo);
	if (encoder.open() < 0)
	{
		wxString msg = Trans("ui.premiere.error.encoderError");

		if (exportInfo.video.id == "h264_nvenc" || exportInfo.video.id == "hevc_nvenc")
		{
			msg += "\n\n" + Trans("ui.premiere.error.encoderError.nvenc.sessions");
		}

		if (exportInfo.video.options.find("level") != exportInfo.video.options.end())
		{
			msg += "\n\n" + Trans("ui.premiere.error.encoderError.level");
		}

		return gui->ReportMessage(msg);
	}

	AVFrame* aFrame = av_frame_alloc();
	int res = 0;

	prMALError prError = malNoError;

	// Create audio renderer
	AudioRenderer audioRenderer(pluginId, exportRecP->startTime, exportRecP->endTime, encoder.getAudioFrameSize(), exportInfo, &suites);

	if (exportInfo.video.enabled)
	{
		bool renderWithMaxDepth = false;
		auto tp_exportStart = std::chrono::high_resolution_clock::now();
		int frames;

		// Create video renderer
		VideoRenderer videoRenderer(pluginId, exportInfo.video.width, exportInfo.video.height, renderWithMaxDepth, &suites, [&](AVFrame * vFrame, int pass, int usRender, int usProcess)
			{
				frames = vFrame->pts;

				// Multipass
				if (pass > encoder.pass)
				{
					// Close current encoding pass
					encoder.finalize();
					encoder.close();

					// Set new pass
					encoder.pass = pass;

					// Start new pass
					if (encoder.open() < 0)
					{
						gui->ReportMessage(wxString::Format("Unable to start pass #%d", encoder.pass));

						return false;
					}

					// Reset audio renderer
					if (exportInfo.audio.enabled)
					{
						audioRenderer.Reset();
					}
				}

				auto tp_encStart = std::chrono::high_resolution_clock::now();

				// Write video frame first
				if ((res = encoder.writeVideoFrame(vFrame)) < 0)
				{
					gui->ReportMessage(wxString::Format("Failed writing video frame #%lld. (Error code: %d)", vFrame->pts, res));
					return false;
				}

				int usEncode = (std::chrono::high_resolution_clock::now() - tp_encStart) / std::chrono::microseconds(1);

				wxString audioLog;

				if (exportInfo.audio.enabled)
				{
					// Does the muxer want more audio frames?
					while ((av_compare_ts(vFrame->pts, exportInfo.video.timebase, audioRenderer.GetPts(), exportInfo.audio.timebase) > 0))
					{
						auto tp_render = std::chrono::high_resolution_clock::now();

						// Abort loop if no samles are left
						if (audioRenderer.GetNextFrame(*aFrame) == 0)
						{
							vkLogInfo("Aborting audio renderer loop: No audio samples left!.");
							break;
						}

						auto tp_encode = std::chrono::high_resolution_clock::now();

						// Write the audio frames
						if ((res = encoder.writeAudioFrame(aFrame)) < 0)
						{
							gui->ReportMessage(wxString::Format("Failed writing audio frame #%lld. (Error code: %d)", aFrame->pts, res));
							return false;
						}

						// Audio performance
						audioLog = wxString::Format(", aRender: %d 탎, aEncoding: %d 탎",
							(int)((tp_encode - tp_render) / std::chrono::microseconds(1)),
							(int)((std::chrono::high_resolution_clock::now() - tp_encode) / std::chrono::microseconds(1)));
					}
				}

				// Write performance log
				vkLogInfoVA("Frame #%lld: vRender: %d 탎, vProcess: %d 탎, vEncoding: %d 탎" + audioLog + ", Latency: %d 탎",
					vFrame->pts,
					usRender,
					usProcess,
					usEncode,
					(int)((std::chrono::high_resolution_clock::now() - tp_framePrev) / std::chrono::microseconds(1)));

				// Measure video latency w/o audio
				tp_framePrev = std::chrono::high_resolution_clock::now();

				return true;
			});

		tp_framePrev = std::chrono::high_resolution_clock::now();

		// Exporting loop with callback per rendered video frame
		prError = videoRenderer.render(videoRenderer.GetTargetRenderFormat(exportInfo), exportRecP->startTime, exportRecP->endTime, exportInfo.passes);

		int msEncode = (std::chrono::high_resolution_clock::now() - tp_exportStart) / std::chrono::milliseconds(1);

		// Log performance
		vkLogInfoVA("Exported %d frames in %d seconds. (avg. %d fps)",
			frames,
			msEncode / 1000,
			frames * 1000 / msEncode);
	}
	else if (exportInfo.audio.enabled)
	{
		// Export all audio only
		while (audioRenderer.GetNextFrame(*aFrame) > 0)
		{
			if ((res = encoder.writeAudioFrame(aFrame)) < 0)
			{
				prError = gui->ReportMessage(wxString::Format("Failed writing audio frame #%lld. (Error code: %d)", aFrame->pts, res));
				break;
			}
		}
	}

	// Finalize
	if (PrSuiteErrorSucceeded(prError))
	{
		encoder.finalize();
	}
	else
	{
		vkLogErrorVA("Encoding loop finished (Code: %d)", prError);
	}

	encoder.close();

	vkLogSep();
	vkLogInfo("Export finished");
	vkLogSep();

	av_frame_free(&aFrame);

	return prError;
}

prMALError CPremierePluginApp::buttonAction(exParamButtonRec * paramButtonRecP)
{
	return gui->ButtonAction(paramButtonRecP);
}
