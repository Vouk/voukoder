#include <map>
#include <string>
#include <vector>
#include "ExporterX264Settings.h"

prMALError exGenerateDefaultParams(exportStdParms *stdParms, exGenerateDefaultParamRec *generateDefaultParamRec)
{
	prMALError result = malNoError;

	csSDK_int32 exID = generateDefaultParamRec->exporterPluginID, groupIndex;
	InstanceRec *instRec = reinterpret_cast<InstanceRec *>(generateDefaultParamRec->privateData);
	PrSDKExportParamSuite *exportParamSuite = instRec->exportParamSuite;
	PrParam seqWidth, seqHeight, seqPARNum, seqPARDen, seqFrameRate, seqFieldOrder, seqChannelType, seqSampleRate;

	/* Import as many settings as possible from the sequence */
	PrSDKExportInfoSuite *exportInfoSuite = instRec->exportInfoSuite;
	if (exportInfoSuite)
	{
		exportInfoSuite->GetExportSourceInfo(exID, kExportInfo_VideoWidth, &seqWidth);
		exportInfoSuite->GetExportSourceInfo(exID, kExportInfo_VideoHeight, &seqHeight);
		if (seqWidth.mInt32 == 0)
		{
			seqWidth.mInt32 = 1920;
		}
		if (seqHeight.mInt32 == 0)
		{
			seqHeight.mInt32 = 1080;
		}
		exportInfoSuite->GetExportSourceInfo(exID, kExportInfo_PixelAspectNumerator, &seqPARNum);
		exportInfoSuite->GetExportSourceInfo(exID, kExportInfo_PixelAspectDenominator, &seqPARDen);
		exportInfoSuite->GetExportSourceInfo(exID, kExportInfo_VideoFrameRate, &seqFrameRate);
		exportInfoSuite->GetExportSourceInfo(exID, kExportInfo_VideoFieldType, &seqFieldOrder);
		exportInfoSuite->GetExportSourceInfo(exID, kExportInfo_AudioSampleRate, &seqSampleRate);
		exportInfoSuite->GetExportSourceInfo(exID, kExportInfo_AudioChannelsType, &seqChannelType);
	}

	if (exportParamSuite)
	{
		exportParamSuite->AddMultiGroup(exID, &groupIndex);
		PrSDKTimeSuite *timeSuite = instRec->timeSuite;
		
		/* Video tab group */
		exportParamSuite->AddParamGroup(exID, groupIndex, ADBETopParamGroup, ADBEVideoTabGroup, L"Video", kPrFalse, kPrFalse, kPrFalse);

#pragma region Group: Basic Video Settings

		/* Basic settings group*/
		exportParamSuite->AddParamGroup(exID, groupIndex, ADBEVideoTabGroup, ADBEBasicVideoGroup, L"Basic Video Settings", kPrFalse, kPrFalse, kPrFalse);

		/* Video Codec */
		exNewParamInfo videoCodecParam;
		exParamValues videoCodecValues;
		safeStrCpy(videoCodecParam.identifier, kPrMaxName, ADBEVideoCodec);
		videoCodecParam.paramType = exParamType_int;
		videoCodecParam.flags = exParamFlag_none;
		videoCodecValues.value.intValue = AV_CODEC_ID_H264;
		videoCodecValues.disabled = kPrFalse;
		videoCodecValues.hidden = kPrFalse;
		videoCodecParam.paramValues = videoCodecValues;
		exportParamSuite->AddParam(exID, groupIndex, ADBEBasicVideoGroup, &videoCodecParam);
		
		/* Width */
		exNewParamInfo widthParam;
		exParamValues widthValues;
		safeStrCpy(widthParam.identifier, kPrMaxName, ADBEVideoWidth);
		widthParam.paramType = exParamType_int;
		widthParam.flags = exParamFlag_none;
		widthValues.rangeMin.intValue = 16;
		widthValues.rangeMax.intValue = 4096;
		widthValues.value.intValue = seqWidth.mInt32;
		widthValues.disabled = kPrFalse;
		widthValues.hidden = kPrFalse;
		widthParam.paramValues = widthValues;
		exportParamSuite->AddParam(exID, groupIndex, ADBEBasicVideoGroup, &widthParam);

		/* Height */
		exNewParamInfo heightParam;
		exParamValues heightValues;
		safeStrCpy(heightParam.identifier, kPrMaxName, ADBEVideoHeight);
		heightParam.paramType = exParamType_int;
		heightParam.flags = exParamFlag_none;
		heightValues.rangeMin.intValue = 16;
		heightValues.rangeMax.intValue = 4096;
		heightValues.value.intValue = seqHeight.mInt32;
		heightValues.disabled = kPrFalse;
		heightValues.hidden = kPrFalse;
		heightParam.paramValues = heightValues;
		exportParamSuite->AddParam(exID, groupIndex, ADBEBasicVideoGroup, &heightParam);

		/* Pixel aspect ratio */
		exNewParamInfo PARParam;
		exParamValues PARValues;
		safeStrCpy(PARParam.identifier, kPrMaxName, ADBEVideoAspect);
		PARParam.paramType = exParamType_ratio;
		PARParam.flags = exParamFlag_none;
		PARValues.rangeMin.ratioValue.numerator = 10;
		PARValues.rangeMin.ratioValue.denominator = 11;
		PARValues.rangeMax.ratioValue.numerator = 2;
		PARValues.rangeMax.ratioValue.denominator = 1;
		PARValues.value.ratioValue.numerator = seqPARNum.mInt32;
		PARValues.value.ratioValue.denominator = seqPARDen.mInt32;
		PARValues.disabled = kPrFalse;
		PARValues.hidden = kPrFalse;
		PARParam.paramValues = PARValues;
		exportParamSuite->AddParam(exID, groupIndex, ADBEBasicVideoGroup, &PARParam);

		/* Frame rate */
		exNewParamInfo frameRateParam;
		exParamValues frameRateValues;
		safeStrCpy(frameRateParam.identifier, kPrMaxName, ADBEVideoFPS);
		frameRateParam.paramType = exParamType_ticksFrameRate;
		frameRateParam.flags = exParamFlag_none;
		frameRateValues.rangeMin.timeValue = 1;
		timeSuite->GetTicksPerSecond(&frameRateValues.rangeMax.timeValue);
		frameRateValues.value.timeValue = seqFrameRate.mInt64;
		frameRateValues.disabled = kPrFalse;
		frameRateValues.hidden = kPrFalse;
		frameRateParam.paramValues = frameRateValues;
		exportParamSuite->AddParam(exID, groupIndex, ADBEBasicVideoGroup, &frameRateParam);

		/* Field order */
		exNewParamInfo fieldOrderParam;
		exParamValues fieldOrderValues;
		safeStrCpy(fieldOrderParam.identifier, kPrMaxName, ADBEVideoFieldType);
		fieldOrderParam.paramType = exParamType_int;
		fieldOrderParam.flags = exParamFlag_none;
		fieldOrderValues.value.intValue = seqFieldOrder.mInt32;
		fieldOrderValues.disabled = kPrFalse;
		fieldOrderValues.hidden = kPrFalse;
		fieldOrderParam.paramValues = fieldOrderValues;
		exportParamSuite->AddParam(exID, groupIndex, ADBEBasicVideoGroup, &fieldOrderParam);

#pragma endregion

#pragma region Group: Basic Video Encoder Settings

		exportParamSuite->AddParamGroup(exID, groupIndex, ADBEVideoTabGroup, ADBEVideoCodecGroup, L"Basic Encoder Settings", kPrFalse, kPrFalse, kPrFalse);

		/* Preset */
		exNewParamInfo presetParam;
		exParamValues presetValues;
		safeStrCpy(presetParam.identifier, kPrMaxName, X264_Preset);
		presetParam.paramType = exParamType_int;
		presetParam.flags = exParamFlag_none;
		presetValues.value.intValue = static_cast<csSDK_int32>(X264::Preset::Medium);
		presetValues.disabled = kPrFalse;
		presetValues.hidden = kPrFalse;
		presetParam.paramValues = presetValues;
		exportParamSuite->AddParam(exID, groupIndex, ADBEVideoCodecGroup, &presetParam);

		/* Profile */
		exNewParamInfo profileParam;
		exParamValues profileValues;
		safeStrCpy(profileParam.identifier, kPrMaxName, X264_Profile);
		profileParam.paramType = exParamType_int;
		profileParam.flags = exParamFlag_none;
		profileValues.value.intValue = (csSDK_int32)(X264::Profile::High);
		profileValues.disabled = kPrFalse;
		profileValues.hidden = kPrFalse;
		profileParam.paramValues = profileValues;
		exportParamSuite->AddParam(exID, groupIndex, ADBEVideoCodecGroup, &profileParam);

		/* Level */
		exNewParamInfo levelParam;
		exParamValues levelValues;
		safeStrCpy(levelParam.identifier, kPrMaxName, X264_Level);
		levelParam.paramType = exParamType_int;
		levelParam.flags = exParamFlag_none;
		levelValues.value.intValue = (csSDK_int32)(X264::Level::Auto);
		levelValues.disabled = kPrFalse;
		levelValues.hidden = kPrFalse;
		levelParam.paramValues = levelValues;
		exportParamSuite->AddParam(exID, groupIndex, ADBEVideoCodecGroup, &levelParam);

		/* Strategy */
		exNewParamInfo strategyParam;
		exParamValues strategyValues;
		safeStrCpy(strategyParam.identifier, kPrMaxName, X264_Strategy);
		strategyParam.paramType = exParamType_int;
		strategyParam.flags = exParamFlag_none;
		strategyValues.value.intValue = (csSDK_int32)(X264::Strategy::CRF);
		strategyValues.disabled = kPrFalse;
		strategyValues.hidden = kPrFalse;
		strategyParam.paramValues = strategyValues;
		exportParamSuite->AddParam(exID, groupIndex, ADBEVideoCodecGroup, &strategyParam);

		/* Encoding bitrate (ABR mode only) */
		exNewParamInfo videoBitrateParam;
		exParamValues videoBitrateValues;
		safeStrCpy(videoBitrateParam.identifier, kPrMaxName, ADBEVideoBitrateEncoding);
		videoBitrateParam.paramType = exParamType_float;
		videoBitrateParam.flags = exParamFlag_slider;
		videoBitrateValues.rangeMin.floatValue = 1.0;
		videoBitrateValues.rangeMax.floatValue = 50.0;
		videoBitrateValues.value.floatValue = 15.0;
		videoBitrateValues.disabled = kPrFalse;
		videoBitrateValues.hidden = kPrTrue;
		videoBitrateParam.paramValues = videoBitrateValues;
		exportParamSuite->AddParam(exID, groupIndex, ADBEVideoCodecGroup, &videoBitrateParam);

		/* Constant Rate Factor (CRF) */
		exNewParamInfo crfParam;
		exParamValues crfValues;
		safeStrCpy(crfParam.identifier, kPrMaxName, X264_Strategy_CRF);
		crfParam.paramType = exParamType_int;
		crfParam.flags = exParamFlag_slider;
		crfValues.structVersion = 1;
		crfValues.rangeMin.intValue = 0;
		crfValues.rangeMax.intValue = 51;
		crfValues.value.intValue = 23;
		crfValues.disabled = kPrFalse;
		crfValues.hidden = kPrFalse;
		crfParam.paramValues = crfValues;
		exportParamSuite->AddParam(exID, groupIndex, ADBEVideoCodecGroup, &crfParam);

		/* Constant Quantizer (CQ) */
		exNewParamInfo cqParam;
		exParamValues cqValues;
		safeStrCpy(cqParam.identifier, kPrMaxName, X264_Strategy_QP);
		cqParam.paramType = exParamType_int;
		cqParam.flags = exParamFlag_slider;
		cqValues.structVersion = 1;
		cqValues.rangeMin.intValue = 0;
		cqValues.rangeMax.intValue = 69;
		cqValues.value.intValue = 23;
		cqValues.disabled = kPrFalse;
		cqValues.hidden = kPrTrue;
		cqParam.paramValues = cqValues;
		exportParamSuite->AddParam(exID, groupIndex, ADBEVideoCodecGroup, &cqParam);

#pragma endregion

		/* Audio tab group */
		exportParamSuite->AddParamGroup(exID, groupIndex, ADBETopParamGroup, ADBEAudioTabGroup, L"Audio", kPrFalse, kPrFalse, kPrFalse);

#pragma region Group: Basic Audio Settings

		/* Basic audio settings group*/
		exportParamSuite->AddParamGroup(exID, groupIndex, ADBEAudioTabGroup, ADBEBasicAudioGroup, L"Basic Audio Settings", kPrFalse, kPrFalse, kPrFalse);

		/* Audio Codec */
		exNewParamInfo audioCodecParam;
		exParamValues audioCodecValues;
		safeStrCpy(audioCodecParam.identifier, kPrMaxName, ADBEAudioCodec);
		audioCodecParam.paramType = exParamType_int;
		audioCodecParam.flags = exParamFlag_none;
		audioCodecValues.value.intValue = AV_CODEC_ID_AAC;
		audioCodecValues.disabled = kPrFalse;
		audioCodecValues.hidden = kPrFalse;
		audioCodecParam.paramValues = audioCodecValues;
		exportParamSuite->AddParam(exID, groupIndex, ADBEBasicAudioGroup, &audioCodecParam);

#pragma endregion

#pragma region Group: Basic Audio Encoder Settings

		/* Basic settings group*/
		exportParamSuite->AddParamGroup(exID, groupIndex, ADBEAudioTabGroup, ADBEAudioCodecGroup, L"Basic Encoder Settings", kPrFalse, kPrFalse, kPrFalse);

		/* Sample rate */
		exNewParamInfo sampleRateParam;
		exParamValues sampleRateValues;
		safeStrCpy(sampleRateParam.identifier, 256, ADBEAudioRatePerSecond);
		sampleRateParam.paramType = exParamType_float;
		sampleRateParam.flags = exParamFlag_none;
		sampleRateValues.value.floatValue = seqSampleRate.mFloat64;
		sampleRateValues.disabled = kPrFalse;
		sampleRateValues.hidden = kPrFalse;
		sampleRateParam.paramValues = sampleRateValues;
		exportParamSuite->AddParam(exID, groupIndex, ADBEAudioCodecGroup, &sampleRateParam);

		/* Channel type */
		exNewParamInfo channelTypeParam;
		exParamValues channelTypeValues;
		safeStrCpy(channelTypeParam.identifier, 256, ADBEAudioNumChannels);
		channelTypeParam.paramType = exParamType_int;
		channelTypeParam.flags = exParamFlag_none;
		channelTypeValues.value.intValue = seqChannelType.mInt32;
		channelTypeValues.disabled = kPrFalse;
		channelTypeValues.hidden = kPrFalse;
		channelTypeParam.paramValues = channelTypeValues;
		exportParamSuite->AddParam(exID, groupIndex, ADBEAudioCodecGroup, &channelTypeParam);
		
		/* Bitrate */
		exNewParamInfo audioBitrateParam;
		exParamValues audioBitrateValues;
		safeStrCpy(audioBitrateParam.identifier, 256, ADBEAudioBitrate);
		audioBitrateParam.paramType = exParamType_int;
		audioBitrateParam.flags = exParamFlag_none;
		audioBitrateValues.value.intValue = 128;
		audioBitrateValues.disabled = kPrFalse;
		audioBitrateValues.hidden = kPrFalse;
		audioBitrateParam.paramValues = audioBitrateValues;
		exportParamSuite->AddParam(exID, groupIndex, ADBEAudioCodecGroup, &audioBitrateParam);

#pragma endregion
	
		/* Multiplexer tab group */
		exportParamSuite->AddParamGroup(exID, groupIndex, ADBETopParamGroup, FFMultiplexerTabGroup, L"Multiplexer", kPrFalse, kPrFalse, kPrFalse);

#pragma region Group: Multiplexer Settings

		/* Basic settings group*/
		exportParamSuite->AddParamGroup(exID, groupIndex, FFMultiplexerTabGroup, FFMultiplexerBasicGroup, L"Basic Multiplexer Settings", kPrFalse, kPrFalse, kPrFalse);

		/* Multiplexer */
		exNewParamInfo multiplexerParam;
		exParamValues multiplexerValues;
		safeStrCpy(multiplexerParam.identifier, kPrMaxName, FFMultiplexer);
		multiplexerParam.paramType = exParamType_int;
		multiplexerParam.flags = exParamFlag_multiLine;
		multiplexerValues.value.intValue = FFExportFormat::MP4;
		multiplexerValues.disabled = kPrFalse;
		multiplexerValues.hidden = kPrFalse;
		multiplexerParam.paramValues = multiplexerValues;
		exportParamSuite->AddParam(exID, groupIndex, FFMultiplexerBasicGroup, &multiplexerParam);

#pragma endregion

	}

	return result;
}

prMALError exPostProcessParams(exportStdParms *stdParmsP, exPostProcessParamsRec *postProcessParamsRecP)
{
	prMALError result = malNoError;

	Config config;
	csSDK_int32 exID = postProcessParamsRecP->exporterPluginID, groupIndex = 0;
	InstanceRec *instRec = reinterpret_cast<InstanceRec *>(postProcessParamsRecP->privateData);
	prUTF16Char tempString[kPrMaxName];
	exOneParamValueRec tempParamValue;
	
	PrTime ticksPerSecond = 0;
	instRec->timeSuite->GetTicksPerSecond(&ticksPerSecond);

	/* Populate codecs */
	exParamValues multiplexerValue;
	instRec->exportParamSuite->GetParamValue(exID, groupIndex, FFMultiplexer, &multiplexerValue);
	
	instRec->exportParamSuite->SetParamName(exID, groupIndex, ADBEVideoCodec, L"Video Codec");
	instRec->exportParamSuite->SetParamName(exID, groupIndex, ADBEAudioCodec, L"Audio Codec");
	instRec->exportParamSuite->ClearConstrainedValues(exID, groupIndex, ADBEVideoCodec);
	instRec->exportParamSuite->ClearConstrainedValues(exID, groupIndex, ADBEAudioCodec);

	/* Fill content */
	for (ExportFormats item : config.formats)
	{
		if (item.exportFormat == static_cast<FFExportFormat>(multiplexerValue.value.intValue))
		{
			const AVCodecDescriptor *descriptor;
			for (AVCodecID codecId : item.codecIDs)
			{
				descriptor = avcodec_descriptor_get(codecId);
				if (descriptor != NULL)
				{
					tempParamValue.intValue = codecId;
					swprintf(tempString, kPrMaxName, L"%hs", descriptor->long_name);
					if (descriptor->type == AVMEDIA_TYPE_VIDEO)
					{
						instRec->exportParamSuite->AddConstrainedValuePair(exID, groupIndex, ADBEVideoCodec, &tempParamValue, tempString);
					}
					else if (descriptor->type == AVMEDIA_TYPE_AUDIO)
					{
						instRec->exportParamSuite->AddConstrainedValuePair(exID, groupIndex, ADBEAudioCodec, &tempParamValue, tempString);
					}
				}
			}
			break;
		}
	}

#pragma region Group: Basic Video Settings

	/* Video width */
	instRec->exportParamSuite->SetParamName(exID, groupIndex, ADBEVideoWidth, L"Width");

	/* Video height */
	instRec->exportParamSuite->SetParamName(exID, groupIndex, ADBEVideoHeight, L"Height");

	/* Pixel aspect ratio */
	instRec->exportParamSuite->SetParamName(exID, groupIndex, ADBEVideoAspect, L"Pixel Aspect Ratio");
	instRec->exportParamSuite->ClearConstrainedValues(exID, 0, ADBEVideoAspect);
	exOneParamValueRec tempPAR;
	for (RatioParam item : config.pixelAspectRatios)
	{
		tempPAR.ratioValue.numerator = item.num;
		tempPAR.ratioValue.denominator = item.den;
		swprintf(tempString, kPrMaxName, L"%hs", item.name.c_str());
		instRec->exportParamSuite->AddConstrainedValuePair(exID, groupIndex, ADBEVideoAspect, &tempPAR, tempString);
	}

	/* Framerate */
	instRec->exportParamSuite->SetParamName(exID, groupIndex, ADBEVideoFPS, L"Frame Rate");
	instRec->exportParamSuite->ClearConstrainedValues(exID, groupIndex, ADBEVideoFPS);
	exOneParamValueRec tempFPS;
	for (RatioParam item : config.framerates)
	{
		tempFPS.timeValue = ticksPerSecond / item.num * item.den;
		swprintf(tempString, kPrMaxName, L"%hs", item.name.c_str());
		instRec->exportParamSuite->AddConstrainedValuePair(exID, groupIndex, ADBEVideoFPS, &tempFPS, tempString);
	}

	/* Field order */
	instRec->exportParamSuite->SetParamName(exID, groupIndex, ADBEVideoFieldType, L"Field Order");
	instRec->exportParamSuite->ClearConstrainedValues(exID, groupIndex, ADBEVideoFieldType);
	exOneParamValueRec tempFieldOrder;
	for (SimpleParam item : config.fieldOrders)
	{
		tempFieldOrder.intValue = item.value;
		swprintf(tempString, kPrMaxName, L"%hs", item.name.c_str());
		instRec->exportParamSuite->AddConstrainedValuePair(exID, groupIndex, ADBEVideoFieldType, &tempFieldOrder, tempString);
	}

#pragma endregion

#pragma region Group: Basic Video Encoder Settings

	X264 x264;

	/* Preset */
	instRec->exportParamSuite->SetParamName(exID, groupIndex, X264_Preset, L"Preset");
	instRec->exportParamSuite->ClearConstrainedValues(exID, groupIndex, X264_Preset);
	exOneParamValueRec tempPreset;
	for (SimpleParam item : x264.presets)
	{
		tempPreset.intValue = item.value;
		swprintf(tempString, kPrMaxName, L"%hs", item.name.c_str());
		instRec->exportParamSuite->AddConstrainedValuePair(exID, groupIndex, X264_Preset, &tempPreset, tempString);
	}

	/* Profile */
	instRec->exportParamSuite->SetParamName(exID, groupIndex, X264_Profile, L"Profile");
	instRec->exportParamSuite->ClearConstrainedValues(exID, groupIndex, X264_Profile);
	exOneParamValueRec tempProfile;
	for (SimpleParam item : x264.profiles)
	{
		tempProfile.intValue = item.value;
		swprintf(tempString, kPrMaxName, L"%hs", item.name.c_str());
		instRec->exportParamSuite->AddConstrainedValuePair(exID, groupIndex, X264_Profile, &tempProfile, tempString);
	}

	/* Level */
	instRec->exportParamSuite->SetParamName(exID, groupIndex, X264_Level, L"Level");
	instRec->exportParamSuite->ClearConstrainedValues(exID, groupIndex, X264_Level);
	exOneParamValueRec tempLevel;
	for (SimpleParam item : x264.levels)
	{
		tempLevel.intValue = item.value;
		swprintf(tempString, kPrMaxName, L"%hs", item.name.c_str());
		instRec->exportParamSuite->AddConstrainedValuePair(exID, groupIndex, X264_Level, &tempLevel, tempString);
	}

	/* Strategy */
	instRec->exportParamSuite->SetParamName(exID, groupIndex, X264_Strategy, L"Strategy");
	instRec->exportParamSuite->ClearConstrainedValues(exID, groupIndex, X264_Strategy);
	exOneParamValueRec tempStrategy;
	for (SimpleParam item : x264.strategies)
	{
		tempStrategy.intValue = item.value;
		swprintf(tempString, kPrMaxName, L"%hs", item.name.c_str());
		instRec->exportParamSuite->AddConstrainedValuePair(exID, groupIndex, X264_Strategy, &tempStrategy, tempString);
	}
	instRec->exportParamSuite->SetParamName(exID, groupIndex, ADBEVideoBitrateEncoding, L"Average Bitrate (Mbps)");
	instRec->exportParamSuite->SetParamName(exID, groupIndex, X264_Strategy_CRF, L"Constant Rate Factor");
	instRec->exportParamSuite->SetParamName(exID, groupIndex, X264_Strategy_QP, L"Constant Quantizer");

#pragma endregion

#pragma region Group: Basic Audio Encoder Settings

	AAC aac;

	/* Sample rate */
	instRec->exportParamSuite->SetParamName(exID, groupIndex, ADBEAudioRatePerSecond, L"Sample Rate");
	instRec->exportParamSuite->ClearConstrainedValues(exID, groupIndex, ADBEAudioRatePerSecond);
	exOneParamValueRec tempSampleRate;
	for (SimpleParam item : aac.samplerates)
	{
		tempSampleRate.floatValue = (double)item.value;
		swprintf(tempString, kPrMaxName, L"%hs", item.name.c_str());
		instRec->exportParamSuite->AddConstrainedValuePair(exID, groupIndex, ADBEAudioRatePerSecond, &tempSampleRate, tempString);
	}

	/* Channels */
	instRec->exportParamSuite->SetParamName(exID, groupIndex, ADBEAudioNumChannels, L"Channels");
	instRec->exportParamSuite->ClearConstrainedValues(exID, groupIndex, ADBEAudioNumChannels);
	exOneParamValueRec tempChannels;
	for (SimpleParam item : aac.channels)
	{
		tempChannels.intValue = item.value;
		swprintf(tempString, kPrMaxName, L"%hs", item.name.c_str());
		instRec->exportParamSuite->AddConstrainedValuePair(exID, groupIndex, ADBEAudioNumChannels, &tempChannels, tempString);
	}

	/* Bitrate */
	instRec->exportParamSuite->SetParamName(exID, groupIndex, ADBEAudioBitrate, L"Bitrate (Kbps)");
	instRec->exportParamSuite->ClearConstrainedValues(exID, groupIndex, ADBEAudioBitrate);
	exOneParamValueRec tempAudioBitrates;
	for (SimpleParam item : aac.bitrates)
	{
		tempAudioBitrates.intValue = item.value;
		swprintf(tempString, kPrMaxName, L"%hs", item.name.c_str());
		instRec->exportParamSuite->AddConstrainedValuePair(exID, groupIndex, ADBEAudioBitrate, &tempAudioBitrates, tempString);
	}

#pragma endregion

	instRec->exportParamSuite->SetParamName(exID, groupIndex, FFMultiplexerTabGroup, L"Multiplexer");

#pragma region Group: Multiplexer
	
	/* Container format */
	instRec->exportParamSuite->SetParamName(exID, groupIndex, FFMultiplexer, L"Container Format");
	instRec->exportParamSuite->ClearConstrainedValues(exID, groupIndex, FFMultiplexer);
	AVOutputFormat *format;
	for (ExportFormats exportFormat : config.formats)
	{
		format = av_guess_format(exportFormat.name.c_str(), NULL, NULL);
		if (format != NULL)
		{
			tempParamValue.intValue = exportFormat.exportFormat;
			swprintf(tempString, kPrMaxName, L"%hs", format->long_name);
			instRec->exportParamSuite->AddConstrainedValuePair(exID, groupIndex, FFMultiplexer, &tempParamValue, tempString);
		}
	}

#pragma endregion

	return result;
}

prMALError exValidateParamChanged(exportStdParms *stdParmsP, exParamChangedRec *validateParamChangedRecP)
{
	prMALError result = malNoError;
	csSDK_uint32 exID = validateParamChangedRecP->exporterPluginID, groupIndex = validateParamChangedRecP->multiGroupIndex;
	InstanceRec *instRec = reinterpret_cast<InstanceRec *>(validateParamChangedRecP->privateData);
	exParamValues changedValue, tempValue;

	if (instRec->exportParamSuite)
	{
		/* Get the changed value */
		instRec->exportParamSuite->GetParamValue(exID, groupIndex, validateParamChangedRecP->changedParamIdentifier, &changedValue);

		/* What has changed? */
		if (strcmp(validateParamChangedRecP->changedParamIdentifier, X264_Strategy) == 0)
		{
			prBool disabled = kPrFalse;

			switch (changedValue.value.intValue)
			{
			case (csSDK_int32)X264::Strategy::ABR:
				instRec->exportParamSuite->GetParamValue(exID, validateParamChangedRecP->multiGroupIndex, ADBEVideoBitrateEncoding, &tempValue);
				tempValue.hidden = kPrFalse;
				instRec->exportParamSuite->ChangeParam(exID, validateParamChangedRecP->multiGroupIndex, ADBEVideoBitrateEncoding, &tempValue);
				instRec->exportParamSuite->GetParamValue(exID, validateParamChangedRecP->multiGroupIndex, X264_Strategy_CRF, &tempValue);
				tempValue.hidden = kPrTrue;
				instRec->exportParamSuite->ChangeParam(exID, validateParamChangedRecP->multiGroupIndex, X264_Strategy_CRF, &tempValue);
				instRec->exportParamSuite->GetParamValue(exID, validateParamChangedRecP->multiGroupIndex, X264_Strategy_QP, &tempValue);
				tempValue.hidden = kPrTrue;
				instRec->exportParamSuite->ChangeParam(exID, validateParamChangedRecP->multiGroupIndex, X264_Strategy_QP, &tempValue);
				break;

			case (csSDK_int32)X264::Strategy::CRF:
				instRec->exportParamSuite->GetParamValue(exID, validateParamChangedRecP->multiGroupIndex, ADBEVideoBitrateEncoding, &tempValue);
				tempValue.hidden = kPrTrue;
				instRec->exportParamSuite->ChangeParam(exID, validateParamChangedRecP->multiGroupIndex, ADBEVideoBitrateEncoding, &tempValue);
				instRec->exportParamSuite->GetParamValue(exID, validateParamChangedRecP->multiGroupIndex, X264_Strategy_QP, &tempValue);
				tempValue.hidden = kPrTrue;
				instRec->exportParamSuite->ChangeParam(exID, validateParamChangedRecP->multiGroupIndex, X264_Strategy_QP, &tempValue);
				instRec->exportParamSuite->GetParamValue(exID, validateParamChangedRecP->multiGroupIndex, X264_Strategy_CRF, &tempValue);
				tempValue.hidden = kPrFalse;
				instRec->exportParamSuite->ChangeParam(exID, validateParamChangedRecP->multiGroupIndex, X264_Strategy_CRF, &tempValue);

				/* Profile selection is disabled if CRF=0 */
				disabled = tempValue.value.intValue == 0 ? kPrTrue : kPrFalse;
				break;

			case (csSDK_int32)X264::Strategy::QP:
				instRec->exportParamSuite->GetParamValue(exID, validateParamChangedRecP->multiGroupIndex, ADBEVideoBitrateEncoding, &tempValue);
				tempValue.hidden = kPrTrue;
				instRec->exportParamSuite->ChangeParam(exID, validateParamChangedRecP->multiGroupIndex, ADBEVideoBitrateEncoding, &tempValue);
				instRec->exportParamSuite->GetParamValue(exID, validateParamChangedRecP->multiGroupIndex, X264_Strategy_CRF, &tempValue);
				tempValue.hidden = kPrTrue;
				instRec->exportParamSuite->ChangeParam(exID, validateParamChangedRecP->multiGroupIndex, X264_Strategy_CRF, &tempValue);
				instRec->exportParamSuite->GetParamValue(exID, validateParamChangedRecP->multiGroupIndex, X264_Strategy_QP, &tempValue);
				tempValue.hidden = kPrFalse;
				instRec->exportParamSuite->ChangeParam(exID, validateParamChangedRecP->multiGroupIndex, X264_Strategy_QP, &tempValue);

				/* Profile selection is disabled if QP=0 */
				disabled = tempValue.value.intValue == 0 ? kPrTrue : kPrFalse;
				break;
			}

			/* Profile selection is disabled if QP=0 */
			instRec->exportParamSuite->GetParamValue(exID, validateParamChangedRecP->multiGroupIndex, X264_Profile, &tempValue);
			tempValue.disabled = disabled;
			if (disabled)
			{
				tempValue.value.intValue = (csSDK_int32)(X264::Profile::High);
			}

			instRec->exportParamSuite->ChangeParam(exID, validateParamChangedRecP->multiGroupIndex, X264_Profile, &tempValue);
		}
		else if (strcmp(validateParamChangedRecP->changedParamIdentifier, X264_Strategy_CRF) == 0 || strcmp(validateParamChangedRecP->changedParamIdentifier, X264_Strategy_QP) == 0)
		{
			instRec->exportParamSuite->GetParamValue(exID, validateParamChangedRecP->multiGroupIndex, X264_Profile, &tempValue);
			tempValue.disabled = changedValue.value.intValue == 0 ? kPrTrue : kPrFalse;
			if (changedValue.value.intValue == 0)
			{
				tempValue.value.intValue = (csSDK_int32)(X264::Profile::High);
			}
			instRec->exportParamSuite->ChangeParam(exID, validateParamChangedRecP->multiGroupIndex, X264_Profile, &tempValue);
		}
	}
	else
	{
		result = exportReturn_ErrMemory;
	}

	return result;
}