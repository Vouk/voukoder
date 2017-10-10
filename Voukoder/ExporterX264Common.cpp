#include "ExporterX264Common.h"
#include "json.hpp"

PrTime gcd(PrTime a, PrTime b)
{
	if (a == 0) return b;
	return gcd(b % a, a);
}

void populateEncoders(InstanceRec *instRec, csSDK_uint32 pluginId, csSDK_int32 groupIndex, const char *elementIdentifier, json encoders)
{
	exOneParamValueRec oneParamValueRec;
	prUTF16Char tempString[kPrMaxName];

	// Clear existing values
	instRec->exportParamSuite->ClearConstrainedValues(pluginId, groupIndex, elementIdentifier);

	// Populate encoders
	for (auto iterator = encoders.begin(); iterator != encoders.end(); ++iterator)
	{
		json encoder = *iterator;

		std::string name = encoder["name"].get<std::string>();

		AVCodec *codec = avcodec_find_encoder_by_name(name.c_str());
		oneParamValueRec.intValue = encoder["id"].get<int>();
		swprintf(tempString, kPrMaxName, L"%hs", codec->long_name);
		instRec->exportParamSuite->AddConstrainedValuePair(pluginId, groupIndex, elementIdentifier, &oneParamValueRec, tempString);
	}
}

void createEncoderOptionElements(PrSDKExportParamSuite *exportParamSuite, csSDK_uint32 pluginId, csSDK_int32 groupIndex, const char *groupIdentifier, json encoders, int selectedId)
{
	bool isSelected;
	exNewParamInfo paramInfo;

	// Iterate encoders
	for (auto iterator = encoders.begin(); iterator != encoders.end(); ++iterator)
	{
		json encoder = *iterator;

		// Should these options be hidden?
		isSelected = encoder["id"].get<int>() == selectedId;

		// Iterate encoder options
		for (auto iterator2 = encoder["options"].begin(); iterator2 != encoder["options"].end(); ++iterator2)
		{
			json option = *iterator2;

			paramInfo = createGUIElement(option, isSelected ? kPrFalse : kPrTrue);
			exportParamSuite->AddParam(pluginId, groupIndex, groupIdentifier, &paramInfo);

			// Add possible existing suboptions
			for (auto iterator3 = option["values"].begin(); iterator3 != option["values"].end(); ++iterator3)
			{
				json value = *iterator3;

				bool isDefault = option["defaultValue"].get<int>() == value["id"].get<int>();

				// Are there suboptions?
				if (value.find("suboptions") != value.end())
				{
					// Iterate all suboptions
					for (auto iterator4 = value["suboptions"].begin(); iterator4 != value["suboptions"].end(); ++iterator4)
					{
						json suboption = *iterator4;

						paramInfo = createGUIElement(suboption, isDefault ? kPrFalse : kPrTrue);
						exportParamSuite->AddParam(pluginId, groupIndex, groupIdentifier, &paramInfo);
					}
				}
			}
		}
	}
}

exNewParamInfo createGUIElement(json option, prBool hidden)
{
	const std::string type = option["type"].get<std::string>();
	const std::string name = option["name"].get<std::string>();
	const std::string flags = option["flags"].get<std::string>();

	// Fill paramInfo
	exNewParamInfo paramInfo;
	paramInfo.structVersion = 1;
	if (type == "float")
	{
		paramInfo.paramType = exParamType_float;
	}
	else if (type == "int")
	{
		paramInfo.paramType = exParamType_int;
	}

	// Fill param values
	exParamValues paramValues;
	paramValues.structVersion = 1;
	paramValues.disabled = kPrFalse;
	paramValues.hidden = hidden;
	paramValues.optionalParamEnabled = kPrFalse;
	if (type == "float")
	{
		paramValues.value.floatValue = option["defaultValue"].get<float>();
	}
	else if (type == "int")
	{
		paramValues.value.intValue = option["defaultValue"].get<int>();
	}

	// Set up element depending on flags
	if (flags == "slider")
	{
		paramInfo.flags = exParamFlag_slider;

		if (type == "float")
		{
			paramValues.rangeMin.floatValue = option["minValue"].get<float>();
			paramValues.rangeMax.floatValue = option["maxValue"].get<float>();
		}
		else if (type == "int")
		{
			paramValues.rangeMin.intValue = option["minValue"].get<int>();
			paramValues.rangeMax.intValue = option["maxValue"].get<int>();
		}
	}
	else
	{
		paramInfo.flags = exParamFlag_none;
	}

	paramInfo.paramValues = paramValues;

	::lstrcpyA(paramInfo.identifier, name.c_str());

	return paramInfo;
}

void populateEncoderOptionValues(InstanceRec *instRec, csSDK_uint32 pluginId, csSDK_int32 groupIndex, json encoders)
{
	// Iterate all encoders
	for (auto iterator = encoders.begin(); iterator != encoders.end(); ++iterator)
	{
		json encoder = *iterator;

		// Iterate all encoder options
		for (auto iterator2 = encoder["options"].begin(); iterator2 != encoder["options"].end(); ++iterator2)
		{
			json option = *iterator2;

			configureEncoderOption(instRec, pluginId, groupIndex, option);
		}
	}
}

void configureEncoderOption(InstanceRec *instRec, csSDK_uint32 pluginId, csSDK_int32 groupIndex, json option)
{
	prUTF16Char tempString[kPrMaxName];
	exParamValues paramValues;

	const std::string name = option["name"].get<std::string>();
	const std::string label = option["label"].get<std::string>();
	const std::string flags = option["flags"].get<std::string>();

	// Prepare GUI element
	instRec->exportParamSuite->SetParamName(pluginId, groupIndex, name.c_str(), string2wchar_t(label));

	if (flags == "slider")
	{
		instRec->exportParamSuite->GetParamValue(pluginId, groupIndex, name.c_str(), &paramValues);

		const std::string type = option["type"].get<std::string>();

		if (type == "float")
		{
			paramValues.rangeMin.floatValue = option["minValue"].get<float>();
			paramValues.rangeMax.floatValue = option["maxValue"].get<float>();
		}
		else if (type == "int")
		{
			paramValues.rangeMin.intValue = option["minValue"].get<int>();
			paramValues.rangeMax.intValue = option["maxValue"].get<int>();
		}

		instRec->exportParamSuite->ChangeParam(pluginId, groupIndex, name.c_str(), &paramValues);
	}
	else if (flags == "none" && option.find("values") != option.end())
	{
		// Clear values
		instRec->exportParamSuite->ClearConstrainedValues(pluginId, groupIndex, name.c_str());

		exOneParamValueRec oneParamValueRec;
		for (auto iterator3 = option["values"].begin(); iterator3 != option["values"].end(); ++iterator3)
		{
			json value = *iterator3;

			oneParamValueRec.intValue = value["id"].get<int>();

			swprintf(tempString, kPrMaxName, L"%hs", value["name"].get<std::string>().c_str());
			instRec->exportParamSuite->AddConstrainedValuePair(pluginId, groupIndex, name.c_str(), &oneParamValueRec, tempString);

			if (value.find("suboptions") != value.end())
			{
				// Iterate all suboptions
				for (auto iterator4 = value["suboptions"].begin(); iterator4 != value["suboptions"].end(); ++iterator4)
				{
					json suboption = *iterator4;

					configureEncoderOption(instRec, pluginId, groupIndex, suboption);
				}
			}
		}
	}
}

void ShowMessageBox(InstanceRec *instRec, LPCWSTR lpText, LPCWSTR lpCaption, UINT uType)
{
	HWND mainWnd = instRec->windowSuite->GetMainWindow();

	MessageBox(GetLastActivePopup(mainWnd), lpText, lpCaption, uType);
}