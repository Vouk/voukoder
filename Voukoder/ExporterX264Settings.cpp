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
		exParamValues videoCodecValues;
		videoCodecValues.structVersion = 1;
		videoCodecValues.value.intValue = AV_CODEC_ID_H264;
		videoCodecValues.disabled = kPrFalse;
		videoCodecValues.hidden = kPrFalse;
		videoCodecValues.optionalParamEnabled = kPrFalse;
		exNewParamInfo videoCodecParam;
		videoCodecParam.structVersion = 1;
		videoCodecParam.flags = exParamFlag_none;
		videoCodecParam.paramType = exParamType_int;
		videoCodecParam.paramValues = videoCodecValues;
		::lstrcpyA(videoCodecParam.identifier, ADBEVideoCodec);

		/* Width */
		exParamValues widthValues;
		widthValues.structVersion = 1;
		widthValues.value.intValue = seqWidth.mInt32;
		widthValues.rangeMin.intValue = 16;
		widthValues.rangeMax.intValue = 4096;
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

		/* Height */
		exParamValues heightValues;
		heightValues.structVersion = 1;
		heightValues.value.intValue = seqHeight.mInt32;
		heightValues.rangeMin.intValue = 16;
		heightValues.rangeMax.intValue = 4096;
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

		/* Pixel aspect ratio */
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

		/* Frame rate */
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

		/* Field order */
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

#pragma endregion

#pragma region Group: Basic Video Encoder Settings

		exportParamSuite->AddParamGroup(exID, groupIndex, ADBEVideoTabGroup, ADBEVideoCodecGroup, L"Basic Encoder Settings", kPrFalse, kPrFalse, kPrFalse);

		/* Preset */
		exParamValues presetValues;
		presetValues.structVersion = 1;
		presetValues.value.intValue = static_cast<csSDK_int32>(X264::Preset::Medium);
		presetValues.disabled = kPrFalse;
		presetValues.hidden = kPrFalse;
		presetValues.optionalParamEnabled = kPrFalse;
		exNewParamInfo presetParam;
		presetParam.structVersion = 1;
		presetParam.flags = exParamFlag_none;
		presetParam.paramType = exParamType_int;
		presetParam.paramValues = presetValues;
		::lstrcpyA(presetParam.identifier, X264_Preset);
		exportParamSuite->AddParam(exID, groupIndex, ADBEVideoCodecGroup, &presetParam);

		/* Profile */
		exParamValues profileValues;
		profileValues.structVersion = 1;
		profileValues.value.intValue = (csSDK_int32)(X264::Profile::High);
		profileValues.disabled = kPrFalse;
		profileValues.hidden = kPrFalse;
		profileValues.optionalParamEnabled = kPrFalse;
		exNewParamInfo profileParam;
		profileParam.structVersion = 1;
		profileParam.flags = exParamFlag_none;
		profileParam.paramType = exParamType_int;
		profileParam.paramValues = profileValues;
		::lstrcpyA(profileParam.identifier, X264_Profile);
		exportParamSuite->AddParam(exID, groupIndex, ADBEVideoCodecGroup, &profileParam);

		/* Level */
		exParamValues levelValues;
		levelValues.structVersion = 1;
		levelValues.value.intValue = (csSDK_int32)(X264::Level::Auto);
		levelValues.disabled = kPrFalse;
		levelValues.hidden = kPrFalse;
		levelValues.optionalParamEnabled = kPrFalse;
		exNewParamInfo levelParam;
		levelParam.structVersion = 1;
		levelParam.flags = exParamFlag_none;
		levelParam.paramType = exParamType_int;
		levelParam.paramValues = levelValues;
		::lstrcpyA(levelParam.identifier, X264_Level);
		exportParamSuite->AddParam(exID, groupIndex, ADBEVideoCodecGroup, &levelParam);

		/* Strategy */
		exParamValues strategyValues;
		strategyValues.structVersion = 1;
		strategyValues.value.intValue = (csSDK_int32)(X264::Strategy::CRF);
		strategyValues.disabled = kPrFalse;
		strategyValues.hidden = kPrFalse;
		strategyValues.optionalParamEnabled = kPrFalse;
		exNewParamInfo strategyParam;
		strategyParam.structVersion = 1;
		strategyParam.flags = exParamFlag_none;
		strategyParam.paramType = exParamType_int;
		strategyParam.paramValues = strategyValues;
		::lstrcpyA(strategyParam.identifier, X264_Strategy);
		exportParamSuite->AddParam(exID, groupIndex, ADBEVideoCodecGroup, &strategyParam);

		/* Encoding bitrate (ABR mode only) */
		exParamValues videoBitrateValues;
		videoBitrateValues.structVersion = 1;
		videoBitrateValues.value.floatValue = 15.0;
		videoBitrateValues.rangeMin.floatValue = 1.0;
		videoBitrateValues.rangeMax.floatValue = 50.0;
		videoBitrateValues.disabled = kPrFalse;
		videoBitrateValues.hidden = kPrTrue;
		videoBitrateValues.optionalParamEnabled = kPrFalse;
		exNewParamInfo videoBitrateParam;
		videoBitrateParam.structVersion = 1;
		videoBitrateParam.flags = exParamFlag_slider;
		videoBitrateParam.paramType = exParamType_float;
		videoBitrateParam.paramValues = videoBitrateValues;
		::lstrcpyA(videoBitrateParam.identifier, ADBEVideoBitrateEncoding);
		exportParamSuite->AddParam(exID, groupIndex, ADBEVideoCodecGroup, &videoBitrateParam);

		/* Constant Rate Factor (CRF) */
		exParamValues crfValues;
		crfValues.structVersion = 1;
		crfValues.value.intValue = 23;
		crfValues.rangeMin.intValue = 0;
		crfValues.rangeMax.intValue = 51;
		crfValues.disabled = kPrFalse;
		crfValues.hidden = kPrFalse;
		crfValues.optionalParamEnabled = kPrFalse;
		exNewParamInfo crfParam;
		crfParam.structVersion = 1;
		crfParam.flags = exParamFlag_slider;
		crfParam.paramType = exParamType_int;
		crfParam.paramValues = crfValues;
		::lstrcpyA(crfParam.identifier, X264_Strategy_CRF);
		exportParamSuite->AddParam(exID, groupIndex, ADBEVideoCodecGroup, &crfParam);

		/* Constant Quantizer (CQ) */
		exParamValues cqValues;
		cqValues.structVersion = 1;
		cqValues.value.intValue = 23;
		cqValues.rangeMin.intValue = 0;
		cqValues.rangeMax.intValue = 69;
		cqValues.disabled = kPrFalse;
		cqValues.hidden = kPrTrue;
		cqValues.optionalParamEnabled = kPrFalse;
		exNewParamInfo cqParam;
		cqParam.structVersion = 1;
		cqParam.flags = exParamFlag_slider;
		cqParam.paramType = exParamType_int;
		cqParam.paramValues = cqValues;
		::lstrcpyA(cqParam.identifier, X264_Strategy_QP);
		exportParamSuite->AddParam(exID, groupIndex, ADBEVideoCodecGroup, &cqParam);

#pragma endregion

		/* Audio tab group */
		exportParamSuite->AddParamGroup(exID, groupIndex, ADBETopParamGroup, ADBEAudioTabGroup, L"Audio", kPrFalse, kPrFalse, kPrFalse);

#pragma region Group: Basic Audio Settings

		/* Basic audio settings group*/
		exportParamSuite->AddParamGroup(exID, groupIndex, ADBEAudioTabGroup, ADBEBasicAudioGroup, L"Basic Audio Settings", kPrFalse, kPrFalse, kPrFalse);

		/* Audio Codec */
		exParamValues audioCodecValues;
		audioCodecValues.structVersion = 1;
		audioCodecValues.value.intValue = AV_CODEC_ID_AAC;
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

#pragma endregion

#pragma region Group: Basic Audio Encoder Settings

		/* Basic settings group*/
		exportParamSuite->AddParamGroup(exID, groupIndex, ADBEAudioTabGroup, ADBEAudioCodecGroup, L"Basic Encoder Settings", kPrFalse, kPrFalse, kPrFalse);

		/* Sample rate */
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
		exportParamSuite->AddParam(exID, groupIndex, ADBEAudioCodecGroup, &sampleRateParam);

		/* Channel type */
		exParamValues channelTypeValues;
		channelTypeValues.structVersion = 1;
		channelTypeValues.value.intValue = seqChannelType.mInt32;
		channelTypeValues.disabled = kPrFalse;
		channelTypeValues.hidden = kPrFalse;
		channelTypeValues.optionalParamEnabled = kPrFalse;
		exNewParamInfo channelTypeParam;
		channelTypeParam.structVersion = 1;
		channelTypeParam.flags = exParamFlag_none;
		channelTypeParam.paramType = exParamType_int;
		channelTypeParam.paramValues = channelTypeValues;
		::lstrcpyA(channelTypeParam.identifier, ADBEAudioNumChannels);
		exportParamSuite->AddParam(exID, groupIndex, ADBEAudioCodecGroup, &channelTypeParam);
		
		/* Bitrate */
		exParamValues audioBitrateValues;
		audioBitrateValues.structVersion = 1;
		audioBitrateValues.value.intValue = 128;
		audioBitrateValues.disabled = kPrFalse;
		audioBitrateValues.hidden = kPrFalse;
		audioBitrateValues.optionalParamEnabled = kPrFalse;
		exNewParamInfo audioBitrateParam;
		audioBitrateParam.structVersion = 1;
		audioBitrateParam.flags = exParamFlag_none;
		audioBitrateParam.paramType = exParamType_int;
		audioBitrateParam.paramValues = audioBitrateValues;
		::lstrcpyA(audioBitrateParam.identifier, ADBEAudioBitrate);
		exportParamSuite->AddParam(exID, groupIndex, ADBEAudioCodecGroup, &audioBitrateParam);

#pragma endregion
	
		/* Multiplexer tab group */
		exportParamSuite->AddParamGroup(exID, groupIndex, ADBETopParamGroup, FFMultiplexerTabGroup, L"Multiplexer", kPrFalse, kPrFalse, kPrFalse);

#pragma region Group: Multiplexer Settings

		/* Basic settings group*/
		exportParamSuite->AddParamGroup(exID, groupIndex, FFMultiplexerTabGroup, FFMultiplexerBasicGroup, L"Basic Multiplexer Settings", kPrFalse, kPrFalse, kPrFalse);

		/* Multiplexer */
		exParamValues multiplexerValues;
		multiplexerValues.structVersion = 1;
		multiplexerValues.value.intValue = FFExportFormat::MP4;
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

	}

	return result;
}

prMALError exPostProcessParams(exportStdParms *stdParmsP, exPostProcessParamsRec *postProcessParamsRecP)
{
	prMALError result = malNoError;

	csSDK_int32 exID = postProcessParamsRecP->exporterPluginID, groupIndex = 0;
	InstanceRec *instRec = reinterpret_cast<InstanceRec *>(postProcessParamsRecP->privateData);
	
	prUTF16Char tempString[kPrMaxName];
	exOneParamValueRec tempParamValue;

#pragma region Group: Video settings

	Config config;
	PrTime ticksPerSecond = 0;
	instRec->timeSuite->GetTicksPerSecond(&ticksPerSecond);

	/* Group: Encoder settings */
	instRec->exportParamSuite->SetParamName(exID, 0, ADBEBasicVideoGroup, L"Video settings");

	/* Video codec */
	instRec->exportParamSuite->SetParamName(exID, groupIndex, ADBEVideoCodec, L"Video Codec");
	instRec->exportParamSuite->ClearConstrainedValues(exID, groupIndex, ADBEVideoCodec);
	exOneParamValueRec tempVideoCodec;
	tempVideoCodec.intValue = AV_CODEC_ID_H264;
	const AVCodecDescriptor *descriptor = avcodec_descriptor_get((AVCodecID)tempVideoCodec.intValue);
	swprintf(tempString, kPrMaxName, L"%hs", descriptor->long_name);
	instRec->exportParamSuite->AddConstrainedValuePair(exID, groupIndex, ADBEVideoCodec, &tempVideoCodec, tempString);

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

#pragma region Group: Video encoder settings

	X264 x264;

	/* Group: Encoder settings */
	instRec->exportParamSuite->SetParamName(exID, 0, ADBEVideoCodecGroup, L"Codec settings");

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
	
	/* Strategy: ABR */
	instRec->exportParamSuite->SetParamName(exID, groupIndex, ADBEVideoBitrateEncoding, L"Average Bitrate (Mbps)");
	exParamValues abrValues;
	instRec->exportParamSuite->GetParamValue(exID, groupIndex, ADBEVideoBitrateEncoding, &abrValues);
	abrValues.rangeMin.floatValue = 1.0;
	abrValues.rangeMax.floatValue = 50.0;
	instRec->exportParamSuite->ChangeParam(exID, groupIndex, ADBEVideoBitrateEncoding, &abrValues);

	/* Strategy: CRF */
	instRec->exportParamSuite->SetParamName(exID, groupIndex, X264_Strategy_CRF, L"Constant Rate Factor");
	exParamValues crfValues;
	instRec->exportParamSuite->GetParamValue(exID, groupIndex, X264_Strategy_CRF, &crfValues);
	crfValues.rangeMin.intValue = 0;
	crfValues.rangeMax.intValue = 51;
	instRec->exportParamSuite->ChangeParam(exID, groupIndex, X264_Strategy_CRF, &crfValues);

	/* Strategy: QP */
	instRec->exportParamSuite->SetParamName(exID, groupIndex, X264_Strategy_QP, L"Constant Quantizer");
	exParamValues qpValues;
	instRec->exportParamSuite->GetParamValue(exID, groupIndex, X264_Strategy_QP, &qpValues);
	qpValues.rangeMin.intValue = 0;
	qpValues.rangeMax.intValue = 69;
	instRec->exportParamSuite->ChangeParam(exID, groupIndex, X264_Strategy_QP, &qpValues);

#pragma endregion

#pragma region Group: Audio settings

	AAC aac;

	/* Group: Encoder settings */
	instRec->exportParamSuite->SetParamName(exID, 0, ADBEBasicAudioGroup, L"Audio settings");

	/* Video codec */
	instRec->exportParamSuite->SetParamName(exID, groupIndex, ADBEAudioCodec, L"Audio Codec");
	instRec->exportParamSuite->ClearConstrainedValues(exID, groupIndex, ADBEAudioCodec);
	exOneParamValueRec tempAudioCodec;
	tempAudioCodec.intValue = AV_CODEC_ID_AAC;
	const AVCodecDescriptor *descriptor2 = avcodec_descriptor_get((AVCodecID)tempAudioCodec.intValue);
	swprintf(tempString, kPrMaxName, L"%hs", descriptor2->long_name);
	instRec->exportParamSuite->AddConstrainedValuePair(exID, groupIndex, ADBEAudioCodec, &tempAudioCodec, tempString);

#pragma endregion

#pragma region Group: Audio encoder settings

	/* Group: Audio encoder settings */
	instRec->exportParamSuite->SetParamName(exID, 0, ADBEAudioCodecGroup, L"Codec settings");

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

#pragma region Group: Multiplexer

	instRec->exportParamSuite->SetParamName(exID, groupIndex, FFMultiplexerTabGroup, L"Multiplexer");
	instRec->exportParamSuite->SetParamName(exID, groupIndex, FFMultiplexerBasicGroup, L"Container");

	/* Container format */
	instRec->exportParamSuite->SetParamName(exID, groupIndex, FFMultiplexer, L"Format");
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