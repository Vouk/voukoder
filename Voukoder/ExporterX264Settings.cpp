#include <map>
#include <string>
#include <vector>
#include "ExporterX264Settings.h"

prMALError exGenerateDefaultParams(exportStdParms *stdParms, exGenerateDefaultParamRec *generateDefaultParamRec)
{
	prMALError result = malNoError;

	csSDK_int32 exID = generateDefaultParamRec->exporterPluginID, groupIndex;
	InstanceRec *instRec = reinterpret_cast<InstanceRec *>(generateDefaultParamRec->privateData);
	Settings *settings = instRec->settings;
	PrSDKExportParamSuite *exportParamSuite = instRec->exportParamSuite;
	PrParam seqWidth, seqHeight, seqFrameRate, seqChannelType, seqSampleRate;

	// Import as many settings as possible from the sequence
	PrSDKExportInfoSuite *exportInfoSuite = instRec->exportInfoSuite;
	if (exportInfoSuite)
	{
		// Get the values from the sequence
		exportInfoSuite->GetExportSourceInfo(exID, kExportInfo_VideoWidth, &seqWidth);
		exportInfoSuite->GetExportSourceInfo(exID, kExportInfo_VideoHeight, &seqHeight);
		exportInfoSuite->GetExportSourceInfo(exID, kExportInfo_VideoFrameRate, &seqFrameRate);
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
	}

	if (exportParamSuite)
	{
		exportParamSuite->AddMultiGroup(exID, &groupIndex);
		PrSDKTimeSuite *timeSuite = instRec->timeSuite;

#pragma region Group: Basic Video Settings

		// Video tab group
		exportParamSuite->AddParamGroup(exID, groupIndex, ADBETopParamGroup, ADBEVideoTabGroup, L"Video", kPrFalse, kPrFalse, kPrFalse);

		// Basic settings group
		exportParamSuite->AddParamGroup(exID, groupIndex, ADBEVideoTabGroup, ADBEBasicVideoGroup, L"Basic Video Settings", kPrFalse, kPrFalse, kPrFalse);

		// Video Encoder
		exParamValues videoCodecValues;
		videoCodecValues.structVersion = 1;
		videoCodecValues.value.intValue = settings->getDefaultVideoCodecId();
		videoCodecValues.disabled = kPrFalse;
		videoCodecValues.hidden = kPrFalse;
		videoCodecValues.optionalParamEnabled = kPrFalse;
		exNewParamInfo videoCodecParam;
		videoCodecParam.structVersion = 1;
		videoCodecParam.flags = exParamFlag_none;
		videoCodecParam.paramType = exParamType_int;
		videoCodecParam.paramValues = videoCodecValues;
		::lstrcpyA(videoCodecParam.identifier, ADBEVideoCodec);
		exportParamSuite->AddParam(exID, groupIndex, ADBEBasicVideoGroup, &videoCodecParam);

		// Video width
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

		// Video height
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

#pragma endregion

#pragma region Group: Video Encoder Options

		// Basic settings group
		exportParamSuite->AddParamGroup(exID, groupIndex, ADBEVideoTabGroup, ADBEVideoCodecGroup, L"Basic Encoder Settings", kPrFalse, kPrFalse, kPrFalse);

		// Populate video encoder options
		json videoCodecs = settings->getVideoEncoders();
		createEncoderOptionElements(exportParamSuite, exID, groupIndex, ADBEVideoCodecGroup, videoCodecs, videoCodecValues.value.intValue);

#pragma endregion

#pragma region Group: Basic Audio Settings

		// Audio tab group
		exportParamSuite->AddParamGroup(exID, groupIndex, ADBETopParamGroup, ADBEAudioTabGroup, L"Audio", kPrFalse, kPrFalse, kPrFalse);

		// Basic audio settings group
		exportParamSuite->AddParamGroup(exID, groupIndex, ADBEAudioTabGroup, ADBEBasicAudioGroup, L"Basic Audio Settings", kPrFalse, kPrFalse, kPrFalse);

		// Audio Codec
		exParamValues audioCodecValues;
		audioCodecValues.structVersion = 1;
		audioCodecValues.value.intValue = settings->getDefaultAudioCodecId();
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
		json audioCodecs = settings->getAudioEncoders();
		createEncoderOptionElements(exportParamSuite, exID, groupIndex, ADBEAudioCodecGroup, audioCodecs, audioCodecValues.value.intValue);

#pragma endregion
	
#pragma region Group: Multiplexer

		// Multiplexer tab group
		exportParamSuite->AddParamGroup(exID, groupIndex, ADBETopParamGroup, FFMultiplexerTabGroup, L"Multiplexer", kPrFalse, kPrFalse, kPrFalse);

		// Basic settings group
		exportParamSuite->AddParamGroup(exID, groupIndex, FFMultiplexerTabGroup, FFMultiplexerBasicGroup, L"Multiplexer Settings", kPrFalse, kPrFalse, kPrFalse);

		// Multiplexer
		exParamValues multiplexerValues;
		multiplexerValues.structVersion = 1;
		multiplexerValues.value.intValue = settings->getDefaultMuxerId();
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
	prUTF16Char tempString[kPrMaxName];
	exOneParamValueRec tempParamValue;

	csSDK_int32 exID = postProcessParamsRecP->exporterPluginID, groupIndex = 0;
	InstanceRec *instRec = reinterpret_cast<InstanceRec *>(postProcessParamsRecP->privateData);
	Settings *settings = instRec->settings;

	// Get various settings
	json configuration = settings->getConfiguration();
	json videoEncoders = settings->getVideoEncoders();
	json audioEncoders = settings->getAudioEncoders();
	json muxers = settings->getMuxers();

#pragma region Group: Video settings

	// Label elements
	instRec->exportParamSuite->SetParamName(exID, groupIndex, ADBEBasicVideoGroup, L"Video settings");
	instRec->exportParamSuite->SetParamName(exID, groupIndex, ADBEVideoCodec, L"Video Codec");
	instRec->exportParamSuite->SetParamName(exID, groupIndex, ADBEVideoWidth, L"Width");
	instRec->exportParamSuite->SetParamName(exID, groupIndex, ADBEVideoHeight, L"Height");
	instRec->exportParamSuite->SetParamName(exID, groupIndex, ADBEVideoFPS, L"Frame Rate");

	// Populate video encoder dropdown
	populateEncoders(instRec, exID, groupIndex, ADBEVideoCodec, videoEncoders);

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
		swprintf(tempString, kPrMaxName, L"%hs", name.c_str());
		instRec->exportParamSuite->AddConstrainedValuePair(exID, groupIndex, ADBEVideoFPS, &tempFPS, tempString);
	}

#pragma endregion

#pragma region Group: Video encoder settings

	instRec->exportParamSuite->SetParamName(exID, 0, ADBEVideoCodecGroup, L"Codec settings");

	populateEncoderOptionValues(instRec, exID, groupIndex, videoEncoders);

#pragma endregion

#pragma region Group: Audio settings

	// Label elements
	instRec->exportParamSuite->SetParamName(exID, groupIndex, ADBEBasicAudioGroup, L"Audio settings");
	instRec->exportParamSuite->SetParamName(exID, groupIndex, ADBEAudioCodec, L"Audio Codec");
	instRec->exportParamSuite->SetParamName(exID, groupIndex, ADBEAudioRatePerSecond, L"Sample Rate");
	instRec->exportParamSuite->SetParamName(exID, groupIndex, ADBEAudioNumChannels, L"Channels");

	// Populate audio encoder dropdown
	populateEncoders(instRec, exID, groupIndex, ADBEAudioCodec, audioEncoders);

	// Clear the samplerate dropdown
	instRec->exportParamSuite->ClearConstrainedValues(exID, groupIndex, ADBEAudioRatePerSecond);

	// Populate the samplerate dropdown
	exOneParamValueRec oneParamValueRec;
	for (auto iterator = configuration["audioSampleRates"].begin(); iterator != configuration["audioSampleRates"].end(); ++iterator)
	{
		json sampleRate = *iterator;

		oneParamValueRec.floatValue = sampleRate["id"].get<double>();
	
		std::string name = sampleRate["name"].get<std::string>();
		swprintf(tempString, kPrMaxName, L"%hs", name.c_str());
		instRec->exportParamSuite->AddConstrainedValuePair(exID, groupIndex, ADBEAudioRatePerSecond, &oneParamValueRec, tempString);
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
		swprintf(tempString, kPrMaxName, L"%hs", name.c_str());
		instRec->exportParamSuite->AddConstrainedValuePair(exID, groupIndex, ADBEAudioNumChannels, &oneParamValueRec2, tempString);
	}

#pragma endregion

#pragma region Group: Audio encoder settings

	// Label elements
	instRec->exportParamSuite->SetParamName(exID, 0, ADBEAudioCodecGroup, L"Codec settings");

	// Populate audio encoder dropdown
	populateEncoderOptionValues(instRec, exID, groupIndex, audioEncoders);

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
	for (auto iterator = muxers.begin(); iterator != muxers.end(); ++iterator)
	{
		json item = *iterator;
		std::string name = item["name"].get<std::string>();

		format = av_guess_format(name.c_str(), NULL, NULL);
		if (format != NULL)
		{
			tempParamValue.intValue = item["id"].get<int>();
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
	Settings *settings = instRec->settings;
	exParamValues tempValue;

	if (instRec->exportParamSuite)
	{
		// Get the changed value
		exParamValues changedValue;
		instRec->exportParamSuite->GetParamValue(exID, groupIndex, validateParamChangedRecP->changedParamIdentifier, &changedValue);

		// What has changed?
		const char* optionName = validateParamChangedRecP->changedParamIdentifier;
		
		// Is it a encoder selection element?
		if (strcmp(optionName, ADBEVideoCodec) == 0 || strcmp(optionName, ADBEAudioCodec) == 0)
		{
			json encoders;

			// Get the encoders
			if (strcmp(optionName, ADBEVideoCodec) == 0)
			{
				encoders = settings->getVideoEncoders();
			}
			else if (strcmp(optionName, ADBEAudioCodec) == 0)
			{
				encoders = settings->getAudioEncoders();
			}
			
			// Iterate all encoders
			for (auto iterator = encoders.begin(); iterator != encoders.end(); ++iterator)
			{
				const json encoder = *iterator;

				// Should these options be hidden?
				const bool isSelected = encoder["id"].get<int>() == changedValue.value.intValue;

				for (auto iterator2 = encoder["options"].begin(); iterator2 != encoder["options"].end(); ++iterator2)
				{
					const json option = *iterator2;

					const std::string name = option["name"].get<std::string>();
					const std::string flags = option["flags"].get<std::string>();

					// Change the visibility of an element
					instRec->exportParamSuite->GetParamValue(exID, validateParamChangedRecP->multiGroupIndex, name.c_str(), &tempValue);
					tempValue.hidden = isSelected ? kPrFalse : kPrTrue;
					instRec->exportParamSuite->ChangeParam(exID, validateParamChangedRecP->multiGroupIndex, name.c_str(), &tempValue);

					if (flags == "none" && option.find("values") != option.end())
					{
						int selectedId = tempValue.value.intValue;

						// Handle the elements suboptions
						for (auto iterator = option["values"].begin(); iterator != option["values"].end(); ++iterator)
						{
							const json value = *iterator;

							// Do we have suboptions at all?
							if (value.find("suboptions") != value.end())
							{
								// Should this suboption be visible
								const bool isValueSelected = isSelected && value["id"].get<int>() == selectedId;

								// Iterate these suboptions
								for (auto iterator2 = value["suboptions"].begin(); iterator2 != value["suboptions"].end(); ++iterator2)
								{
									const json suboption = *iterator2;

									const std::string name = suboption["name"].get<std::string>();

									// Change the elements visibility
									instRec->exportParamSuite->GetParamValue(exID, validateParamChangedRecP->multiGroupIndex, name.c_str(), &tempValue);
									tempValue.hidden = isValueSelected ? kPrFalse : kPrTrue;
									instRec->exportParamSuite->ChangeParam(exID, validateParamChangedRecP->multiGroupIndex, name.c_str(), &tempValue);
								}
							}
						}
					}
				}
			}
		}
		else // Dynamic created elements
		{
			// Try to find the selection in one option of the codecs
			json option = Settings::find(settings->getVideoEncoders(), "name", optionName);
			if (option.empty())
			{
				option = Settings::find(settings->getAudioEncoders(), "name", optionName);
				if (option.empty())
				{
					return result;
				}
			}

			// What type of element do we have?
			const std::string flags = option["flags"].get<std::string>();

			// Process the different cases
			if (flags == "slider")
			{
				if (option.find("disableFieldOnZero") != option.end())
				{
					json fields = option["disableFieldOnZero"];

					// Iterate all mentioned fields
					for (json::iterator iterator = fields.begin(); iterator != fields.end(); ++iterator)
					{
						const std::string field = (*iterator).get<std::string>();

						// Change the elements visibility
						instRec->exportParamSuite->GetParamValue(exID, validateParamChangedRecP->multiGroupIndex, field.c_str(), &tempValue);
						tempValue.disabled = changedValue.value.intValue == 0 ? kPrTrue : kPrFalse;
						instRec->exportParamSuite->ChangeParam(exID, validateParamChangedRecP->multiGroupIndex, field.c_str(), &tempValue);
					}
				}
			}
			else
			{
				// Iterate these options
				for (auto iterator = option["values"].begin(); iterator != option["values"].end(); ++iterator)
				{
					const json value = *iterator;

					if (value.find("suboptions") != value.end())
					{
						// Should this suboption be visible
						const bool isSelected = value["id"].get<int>() == changedValue.value.intValue;

						// Iterate these suboptions
						for (auto iterator2 = value["suboptions"].begin(); iterator2 != value["suboptions"].end(); ++iterator2)
						{
							const json suboption = *iterator2;

							const std::string name = suboption["name"].get<std::string>();

							// Change the elements visibility
							instRec->exportParamSuite->GetParamValue(exID, validateParamChangedRecP->multiGroupIndex, name.c_str(), &tempValue);
							tempValue.hidden = isSelected ? kPrFalse : kPrTrue;
							instRec->exportParamSuite->ChangeParam(exID, validateParamChangedRecP->multiGroupIndex, name.c_str(), &tempValue);

							// Disable one field on zero value?
							if (isSelected && suboption.find("disableFieldOnZero") != suboption.end())
							{
								const std::string name = suboption["name"].get<std::string>();

								json fields = option["disableFieldOnZero"];

								// Get current suboption value
								exParamValues suboptionValue;
								instRec->exportParamSuite->GetParamValue(exID, groupIndex, name.c_str(), &suboptionValue);

								// Iterate all mentioned fields
								for (json::iterator iterator3 = fields.begin(); iterator3 != fields.end(); ++iterator)
								{
									const std::string field = (*iterator3).get<std::string>();

									// Change the elements visibility
									instRec->exportParamSuite->GetParamValue(exID, validateParamChangedRecP->multiGroupIndex, field.c_str(), &tempValue);
									tempValue.disabled = suboptionValue.value.intValue == 0 ? kPrTrue : kPrFalse;
									instRec->exportParamSuite->ChangeParam(exID, validateParamChangedRecP->multiGroupIndex, field.c_str(), &tempValue);
								}
							}
						}
					}
				}
			}
		}

		// Here
	}
	else
	{
		result = exportReturn_ErrMemory;
	}

	return result;
}