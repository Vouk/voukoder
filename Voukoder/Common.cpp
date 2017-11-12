#include "Common.h"
#include "json.hpp"

PrTime gcd(PrTime a, PrTime b)
{
	if (a == 0) return b;
	return gcd(b % a, a);
}

void ShowMessageBox(InstanceRec *instRec, LPCWSTR lpText, LPCWSTR lpCaption, UINT uType)
{
	HWND mainWnd = instRec->windowSuite->GetMainWindow();

	MessageBox(GetLastActivePopup(mainWnd), lpText, lpCaption, uType);
}

void PopulateEncoders(InstanceRec *instRec, csSDK_uint32 pluginId, csSDK_int32 groupIndex, const char *elementIdentifier, json encoders)
{
	exOneParamValueRec oneParamValueRec;
	prUTF16Char tempString[kPrMaxName];

	// Clear existing values
	instRec->exportParamSuite->ClearConstrainedValues(pluginId, groupIndex, elementIdentifier);

	// Populate encoders
	for (auto itEncoder = encoders.begin(); itEncoder != encoders.end(); ++itEncoder)
	{
		json encoder = *itEncoder;

		std::string name = encoder["name"].get<std::string>();

		AVCodec *codec = avcodec_find_encoder_by_name(name.c_str());
		if (codec != NULL)
		{
			oneParamValueRec.intValue = encoder["id"].get<int>();
			swprintf(tempString, kPrMaxName, L"%hs", codec->long_name);
			instRec->exportParamSuite->AddConstrainedValuePair(pluginId, groupIndex, elementIdentifier, &oneParamValueRec, tempString);
		}
	}
}

void CreateEncoderParamElements(PrSDKExportParamSuite *exportParamSuite, csSDK_uint32 pluginId, csSDK_int32 groupIndex, json encoders, int selectedId)
{
	bool isSelected;
	exNewParamInfo paramInfo;

	// Iterate encoders
	for (auto itEncoder = encoders.begin(); itEncoder != encoders.end(); ++itEncoder)
	{
		json encoder = *itEncoder;

		// Should these params be hidden?
		isSelected = encoder["id"].get<int>() == selectedId;

		// Setup groups
		if (encoder["groups"].is_array())
		{
			for (auto itGroup = encoder["groups"].begin(); itGroup != encoder["groups"].end(); ++itGroup)
			{
				json group = *itGroup;
				const std::string name = group["name"].get<std::string>();
				const std::string label = group["label"].get<std::string>();
				const std::string parent = group["parent"].get<std::string>();

				// Set group name
				exportParamSuite->AddParamGroup(pluginId, groupIndex, parent.c_str(), name.c_str(), string2wchar_t(label), kPrFalse, kPrFalse, kPrFalse);
			}
		}

		// Iterate encoder params
		for (auto itParam = encoder["params"].begin(); itParam != encoder["params"].end(); ++itParam)
		{
			json param = *itParam;

			std::string group = param["group"].get<std::string>();

			paramInfo = CreateParamElement(param, !isSelected);

			exportParamSuite->AddParam(pluginId, groupIndex, group.c_str(), &paramInfo);

			// Add possible existing subvalues
			for (auto itValue = param["values"].begin(); itValue != param["values"].end(); ++itValue)
			{
				json value = *itValue;

				bool isDefault = param["defaultValue"].get<int>() == value["id"].get<int>();

				// Are there subvalues?
				if (value.find("subvalues") != value.end())
				{
					// Iterate all subvalues
					for (auto itSubvalue = value["subvalues"].begin(); itSubvalue != value["subvalues"].end(); ++itSubvalue)
					{
						paramInfo = CreateParamElement(*itSubvalue, !isDefault || !isSelected);

						exportParamSuite->AddParam(pluginId, groupIndex, group.c_str(), &paramInfo);
					}
				}
			}
		}
	}
}

exNewParamInfo CreateParamElement(json param, bool hidden)
{
	// Get param type
	const std::string type = param["type"].get<std::string>();
	const std::string name = param["name"].get<std::string>();

	// Fill param values
	exParamValues paramValues;
	paramValues.structVersion = 1;
	paramValues.disabled = kPrFalse;
	paramValues.hidden = hidden;
	paramValues.optionalParamEnabled = kPrFalse;

	// Fill paramInfo
	exNewParamInfo paramInfo;
	paramInfo.structVersion = 1;
	paramInfo.flags = exParamFlag_none;
	paramInfo.paramType = exParamType_undefined;	
	::lstrcpyA(paramInfo.identifier, name.c_str());
	
	// Data types
	if (type == "float")
	{
		paramInfo.paramType = exParamType_float;
		paramValues.value.floatValue = param["defaultValue"].get<float>();
		
		if (param["minValue"].is_number())
		{
			paramValues.rangeMin.floatValue = param["minValue"].get<float>();
		}
		
		if (param["maxValue"].is_number())
		{
			paramValues.rangeMax.floatValue = param["maxValue"].get<float>();
		}
	}
	else if (type == "int")
	{
		paramInfo.paramType = exParamType_int;
		paramValues.value.intValue = param["defaultValue"].get<int>();

		if (param["minValue"].is_number())
		{
			paramValues.rangeMin.intValue = param["minValue"].get<int>();
		}

		if (param["maxValue"].is_number())
		{
			paramValues.rangeMax.intValue = param["maxValue"].get<int>();
		}
	}
	else if (type == "bool")
	{
		paramInfo.paramType = exParamType_bool;
		paramValues.value.intValue = param["defaultValue"].get<int>();
	}
	else if (type == "string")
	{
		paramInfo.paramType = exParamType_string;
		const std::string defaultValue = param["defaultValue"].get<std::string>();
		::lstrcpyW(paramValues.paramString, string2wchar_t(defaultValue));
	}
	else if (type == "button")
	{
		paramInfo.paramType = exParamType_button;
	}

	// Flags
	if (param["flags"].is_array())
	{
		for (auto itFlag = param["flags"].begin(); itFlag != param["flags"].end(); ++itFlag)
		{
			const std::string flag = (*itFlag).get<std::string>();

			if (flag == "slider")
			{
				paramInfo.flags = exParamFlag_slider;
			}
			else if (flag == "multiline")
			{
				paramInfo.flags = exParamFlag_multiLine;
			}
		}
	}

	paramInfo.paramValues = paramValues;

	return paramInfo;
}

void PopulateParamValues(InstanceRec *instRec, csSDK_uint32 pluginId, csSDK_int32 groupIndex, json encoders)
{
	// Iterate all encoders
	for (auto itEncoder = encoders.begin(); itEncoder != encoders.end(); ++itEncoder)
	{
		json encoder = *itEncoder;

		// Configure groups
		if (encoder["groups"].is_array())
		{
			for (auto itGroup = encoder["groups"].begin(); itGroup != encoder["groups"].end(); ++itGroup)
			{
				json group = *itGroup;
				const std::string name = group["name"].get<std::string>();
				const std::string label = group["label"].get<std::string>();
				const std::string parent = group["parent"].get<std::string>();

				// Add group
				instRec->exportParamSuite->SetParamName(pluginId, groupIndex, name.c_str(), string2wchar_t(label));
			}
		}

		// Iterate all encoder options
		for (auto itParam = encoder["params"].begin(); itParam != encoder["params"].end(); ++itParam)
		{
			ConfigureEncoderParam(instRec, pluginId, groupIndex, *itParam);
		}
	}
}

void ConfigureEncoderParam(InstanceRec *instRec, csSDK_uint32 pluginId, csSDK_int32 groupIndex, json param)
{
	prUTF16Char tempString[kPrMaxName];
	exParamValues paramValues;

	const std::string name = param["name"].get<std::string>();
	const std::string label = param["label"].get<std::string>();
	const std::string type = param["type"].get<std::string>();

	// Assign a param label
	instRec->exportParamSuite->SetParamName(pluginId, groupIndex, name.c_str(), string2wchar_t(label));

	// Flags
	instRec->exportParamSuite->GetParamValue(pluginId, groupIndex, name.c_str(), &paramValues);

	if (type == "float")
	{
		if (param["minValue"].is_number())
		{
			paramValues.rangeMin.floatValue = param["minValue"].get<float>();
		}
		if (param["maxValue"].is_number())
		{
			paramValues.rangeMax.floatValue = param["maxValue"].get<float>();
		}
	}
	else if (type == "int")
	{
		if (param["minValue"].is_number())
		{
			paramValues.rangeMin.intValue = param["minValue"].get<int>();
		}
		if (param["maxValue"].is_number())
		{
			paramValues.rangeMax.intValue = param["maxValue"].get<int>();
		}
	}

	instRec->exportParamSuite->ChangeParam(pluginId, groupIndex, name.c_str(), &paramValues);
	
	// It is a simple dropdown?
	if (param.find("values") != param.end() && param["values"].is_array())
	{
		// Clear values
		instRec->exportParamSuite->ClearConstrainedValues(pluginId, groupIndex, name.c_str());

		// Populate new values
		exOneParamValueRec oneParamValueRec;
		for (auto itValue = param["values"].begin(); itValue != param["values"].end(); ++itValue)
		{
			json value = *itValue;

			// Add value
			oneParamValueRec.intValue = value["id"].get<int>();
			swprintf(tempString, kPrMaxName, L"%hs", value["name"].get<std::string>().c_str());
			instRec->exportParamSuite->AddConstrainedValuePair(pluginId, groupIndex, name.c_str(), &oneParamValueRec, tempString);

			// Are there subvalues
			if (value.find("subvalues") != value.end())
			{
				for (auto itSubvalue = value["subvalues"].begin(); itSubvalue != value["subvalues"].end(); ++itSubvalue)
				{
					ConfigureEncoderParam(instRec, pluginId, groupIndex, *itSubvalue);
				}
			}
		}
	}	
}