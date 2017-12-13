#include "Common.h"

// reviewed 0.3.8
PrTime gcd(PrTime a, PrTime b)
{
	if (a == 0) return b;
	return gcd(b % a, a);
}

// reviewed 0.3.8
void ShowMessageBox(InstanceRec *instRec, LPCWSTR lpText, LPCWSTR lpCaption, UINT uType)
{
	HWND mainWnd = instRec->windowSuite->GetMainWindow();

	MessageBox(GetLastActivePopup(mainWnd), lpText, lpCaption, uType);
}

// reviewed 0.4.1
void ShowMessageBox(InstanceRec *instRec, LPCSTR lpText, LPCSTR lpCaption, UINT uType)
{
	HWND mainWnd = instRec->windowSuite->GetMainWindow();

	MessageBoxA(GetLastActivePopup(mainWnd), lpText, lpCaption, uType);
}

// reviewed 0.3.8
void PopulateEncoders(InstanceRec *instRec, csSDK_uint32 pluginId, csSDK_int32 groupIndex, const char *elementIdentifier, vector<EncoderInfo> encoderInfos)
{
	// Clear existing values
	instRec->exportParamSuite->ClearConstrainedValues(pluginId, groupIndex, elementIdentifier);

	// Populate encoders
	for (EncoderInfo encoderInfo: encoderInfos)
	{
		const AVCodec *codec = avcodec_find_encoder_by_name(encoderInfo.name.c_str());
		if (codec != NULL)
		{
			exOneParamValueRec oneParamValueRec;
			oneParamValueRec.intValue = encoderInfo.id;

			prUTF16Char tempString[kPrMaxName];
			swprintf(tempString, kPrMaxName, L"%hs", codec->long_name);

			instRec->exportParamSuite->AddConstrainedValuePair(pluginId, groupIndex, elementIdentifier, &oneParamValueRec, tempString);
		}
	}
}

// reviewed 0.3.8
void CreateEncoderParamElements(PrSDKExportParamSuite *exportParamSuite, csSDK_uint32 pluginId, csSDK_int32 groupIndex, vector<EncoderInfo> encoderInfos, int selectedId)
{
	exNewParamInfo paramInfo;
		
	for (EncoderInfo encoderInfo: encoderInfos)
	{
		// Setup groups
		for (ElementGroup group: encoderInfo.groups)
		{
			exportParamSuite->AddParamGroup(pluginId, groupIndex, group.parent.c_str(), group.name.c_str(), string2wchar_t(group.label), kPrFalse, kPrFalse, kPrFalse);
		}

		// Should these params be hidden?
		const bool isSelected = encoderInfo.id == selectedId;

		// Iterate encoder params
		for (ParameterInfo parameterInfo: encoderInfo.params)
		{
			paramInfo = CreateParamElement(&parameterInfo, !isSelected);

			exportParamSuite->AddParam(pluginId, groupIndex, parameterInfo.group.c_str(), &paramInfo);

			// Add possible existing subvalues
			for (ParameterValueInfo parameterValueInfo: parameterInfo.values)
			{
				bool isDefault = parameterInfo.default.intValue == parameterValueInfo.id;

				// Are there subvalues?
				for(ParameterSubValue parameterSubValue: parameterValueInfo.subValues)
				{
					paramInfo = CreateParamElement(&parameterSubValue, !isDefault || !isSelected);

					exportParamSuite->AddParam(pluginId, groupIndex, parameterInfo.group.c_str(), &paramInfo);
				}
			}
		}
	}
}

// reviewed 0.3.8
exNewParamInfo CreateParamElement(ParamInfo *info, bool hidden)
{
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
	::lstrcpyA(paramInfo.identifier, info->name.c_str());
	
	// Data types
	if (info->type == "float")
	{
		paramInfo.paramType = exParamType_float;
		paramValues.value.floatValue = info->default.floatValue;
		
		if (!isnan(info->min.floatValue))
		{
			paramValues.rangeMin.floatValue = info->min.floatValue;
		}
		
		if (!isnan(info->max.floatValue))
		{
			paramValues.rangeMax.floatValue = info->max.floatValue;
		}
	}
	else if (info->type == "int")
	{
		paramInfo.paramType = exParamType_int;
		paramValues.value.intValue = info->default.intValue;

		if (!isnan(info->min.floatValue)) // This is a union so float is okay
		{
			paramValues.rangeMin.intValue = info->min.intValue;
		}

		if (!isnan(info->max.floatValue))
		{
			paramValues.rangeMax.intValue = info->max.intValue;
		}
	}
	else if (info->type == "bool")
	{
		paramInfo.paramType = exParamType_bool;
		paramValues.value.intValue = info->default.intValue;
	}
	else if (info->type == "button")
	{
		paramInfo.paramType = exParamType_button;
	}

	// Flags
	for (string flag: info->flags)
	{
		if (flag == "slider")
		{
			paramInfo.flags = exParamFlag_slider;
		}
		else if (flag == "multiline")
		{
			paramInfo.flags = exParamFlag_multiLine;
		}
	}

	paramInfo.paramValues = paramValues;

	return paramInfo;
}

// reviewed 0.3.8
void PopulateParamValues(InstanceRec *instRec, csSDK_uint32 pluginId, csSDK_int32 groupIndex, vector<EncoderInfo> encoderInfos)
{
	// Iterate all encoders
	for (EncoderInfo encoderInfo: encoderInfos)
	{
		// Groups
		for (ElementGroup elementGroup: encoderInfo.groups)
		{
			instRec->exportParamSuite->SetParamName(pluginId, groupIndex, elementGroup.name.c_str(), string2wchar_t(elementGroup.label));
		}

		// Iterate all encoder options
		for (ParameterInfo parameterInfo: encoderInfo.params)
		{
			ConfigureEncoderParam(instRec, pluginId, groupIndex, &parameterInfo);
		}
	}
}

// reviewed 0.3.8
void ConfigureEncoderParam(InstanceRec *instRec, csSDK_uint32 pluginId, csSDK_int32 groupIndex, ParamInfo *info)
{
	prUTF16Char tempString[kPrMaxName];

	// Assign a param label
	instRec->exportParamSuite->SetParamName(pluginId, groupIndex, info->name.c_str(), string2wchar_t(info->label));

	// Get the elements values
	exParamValues paramValues;
	instRec->exportParamSuite->GetParamValue(pluginId, groupIndex, info->name.c_str(), &paramValues);

	// Type specific values
	if (!isnan(info->min.floatValue))
	{
		if (info->type == "float")
		{
			paramValues.rangeMin.floatValue = info->min.floatValue;
		}
		else if (info->type == "int")
		{
			paramValues.rangeMin.intValue = info->min.intValue;
		}
	}
	
	if (!isnan(info->max.floatValue))
	{
		if (info->type == "float")
		{
			paramValues.rangeMax.floatValue = info->max.floatValue;
		}
		else if (info->type == "int")
		{
			paramValues.rangeMax.intValue = info->max.intValue;
		}
	}

	// Write changed values
	instRec->exportParamSuite->ChangeParam(pluginId, groupIndex, info->name.c_str(), &paramValues);

	// Handle possible values
	if (!info->isSubValue)
	{
		ParameterInfo* pparameterInfo = static_cast<ParameterInfo*>(info);
		ParameterInfo parameterInfo = *pparameterInfo;

		// It is a simple dropdown?
		if (parameterInfo.values.size() > 0)
		{
			// Clear values
			instRec->exportParamSuite->ClearConstrainedValues(pluginId, groupIndex, parameterInfo.name.c_str());

			// Populate new values
			exOneParamValueRec oneParamValueRec;
			for (ParameterValueInfo parameterValueInfo : parameterInfo.values)
			{
				// Add value
				oneParamValueRec.intValue = parameterValueInfo.id;
				swprintf(tempString, kPrMaxName, L"%hs", parameterValueInfo.name.c_str());
				instRec->exportParamSuite->AddConstrainedValuePair(pluginId, groupIndex, parameterInfo.name.c_str(), &oneParamValueRec, tempString);

				for (ParameterSubValue parameterSubValue : parameterValueInfo.subValues)
				{
					ConfigureEncoderParam(instRec, pluginId, groupIndex, &parameterSubValue);
				}
			}
		}
	}
}
