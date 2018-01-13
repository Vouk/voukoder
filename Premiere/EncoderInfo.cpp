#include "EncoderInfo.h"

// reviewed 0.3.8
EncoderInfo::EncoderInfo(json encoder)
{
	this->encoder = encoder;

	id = encoder["id"].get<int>();
	name = encoder["name"].get<string>();

	// Hide experimetal endcoders in releases
	if (encoder.find("experimental") != encoder.end())
	{
		experimental = encoder["experimental"].get<bool>();
	}

	// Not used for audio codecs
	if (encoder.find("defaultPixelFormat") != encoder.end())
	{
		defaultPixelFormat = encoder["defaultPixelFormat"].get<string>();
	}

	// Does this codec support multipass encoding?
	if (encoder.find("multipass") != encoder.end())
	{
		multipassParameter = encoder["multipass"]["name"].get<string>();
	}

	// Does this codec support multipass encoding?
	if (encoder.find("threads") != encoder.end())
	{
		threadCount = encoder["threads"].get<int>();
	}

	// Add ui element groups
	for (auto iterator = encoder["groups"].begin(); iterator != encoder["groups"].end(); ++iterator)
	{
		json group = *iterator;

		ElementGroup elementGroup;
		elementGroup.name = group["name"].get<string>();
		elementGroup.label = group["label"].get<string>();
		elementGroup.parent = group["parent"].get<string>();

		groups.push_back(elementGroup);
	}

	// Add parameter groups
	for (auto iterator = encoder["parameterGroups"].begin(); iterator != encoder["parameterGroups"].end(); ++iterator)
	{
		json group = iterator.value();

		ParamGroup paramGroup;
		paramGroup.name = iterator.key();
		paramGroup.delimiter = group["delimiter"].get<string>();
		paramGroup.separator = group["separator"].get<string>();
		paramGroup.noValueReplacement = group["noValueReplacement"].get<string>();
		paramGroup.parameters = group["parameters"].get<vector<string>>();

		paramGroups.push_back(paramGroup);
	}

	// Add params
	for (auto iterator = encoder["params"].begin(); iterator != encoder["params"].end(); ++iterator)
	{
		ParameterInfo paramInfo = createParamInfo(*iterator);
		params.push_back(paramInfo);
	}
}

// reviewed 0.3.8
ParameterInfo EncoderInfo::createParamInfo(json json)
{
	ParameterInfo paramInfo;
	paramInfo.isSubValue = false;
	paramInfo.name = json["name"].get<string>();
	paramInfo.label = json["label"].get<string>();
	paramInfo.type = json["type"].get<string>();
	paramInfo.group = json["group"].get<string>();
	paramInfo.flags = json["flags"].get<vector<string>>();

	if (json.find("useDefaultValue") != json.end())
	{
		paramInfo.useDefaultValue = json["useDefaultValue"].get<bool>();
	}

	// Type specific fields
	if (paramInfo.type == "int")
	{
		paramInfo.default.intValue = json["defaultValue"].get<int>();
		if (json.find("minValue") != json.end())
		{
			paramInfo.min.intValue = json["minValue"].get<int>();
		}
		if (json.find("maxValue") != json.end())
		{
			paramInfo.max.intValue = json["maxValue"].get<int>();
		}
	}
	else if (paramInfo.type == "float")
	{
		paramInfo.default.floatValue = json["defaultValue"].get<float>();
		if (json.find("minValue") != json.end())
		{
			paramInfo.min.floatValue = json["minValue"].get<float>();
		}
		if (json.find("maxValue") != json.end())
		{
			paramInfo.max.floatValue = json["maxValue"].get<float>();
		}
	}
	else if (paramInfo.type == "bool")
	{
		paramInfo.default.intValue = json["defaultValue"].get<int>();
	}

	// Are the values?
	if (json.find("values") != json.end())
	{
		for (auto itValue = json["values"].begin(); itValue != json["values"].end(); ++itValue)
		{
			ParameterValueInfo paramValueInfo = createValueInfo(*itValue);
			paramInfo.values.push_back(paramValueInfo);
		}
	}

	// Parameters
	if (json.find("parameters") != json.end())
	{
		for (auto iterator = json["parameters"].begin(); iterator != json["parameters"].end(); ++iterator)
		{
			string name = iterator.key();
			string value = iterator.value();
			paramInfo.parameters.insert({ name, value });
		}
	}

	return paramInfo;
}

// reviewed 0.3.8
ParameterValueInfo EncoderInfo::createValueInfo(json json)
{
	ParameterValueInfo paramValueInfo;
	paramValueInfo.id = json["id"].get<int>();
	paramValueInfo.name = json["name"].get<string>();

	if (json.find("parameters") != json.end())
	{
		paramValueInfo.parameters = json["parameters"].get<map<string, string>>();
	}

	if (json.find("pixelFormat") != json.end())
	{
		paramValueInfo.pixelFormat = json["pixelFormat"].get<string>();
	}

	// Parameters
	if (json.find("parameters") != json.end())
	{
		for (auto iterator = json["parameters"].begin(); iterator != json["parameters"].end(); ++iterator)
		{
			string name = iterator.key();
			string value = iterator.value();
			paramValueInfo.parameters.insert({ name, value });
		}
	}

	// Do we have suboptions at all?
	if (json.find("subvalues") != json.end())
	{
		for (auto itSubvalues = json["subvalues"].begin(); itSubvalues != json["subvalues"].end(); ++itSubvalues)
		{
			ParameterSubValue paramSubValueInfo = createSubValue(*itSubvalues);
			paramValueInfo.subValues.push_back(paramSubValueInfo);
		}
	}

	return paramValueInfo;
}

// reviewed 0.3.8
ParameterSubValue EncoderInfo::createSubValue(json json)
{
	ParameterSubValue paramSubValueInfo;
	paramSubValueInfo.isSubValue = true;
	paramSubValueInfo.name = json["name"].get<string>();
	paramSubValueInfo.label = json["label"].get<string>();
	paramSubValueInfo.type = json["type"].get<string>();
	paramSubValueInfo.flags = json["flags"].get<vector<string>>();

	if (json.find("pixelFormat") != json.end())
	{
		paramSubValueInfo.pixelFormat = json["pixelFormat"].get<string>();
	}

	if (json.find("useDefaultValue") != json.end())
	{
		paramSubValueInfo.useDefaultValue = json["useDefaultValue"].get<bool>();
	}

	if (json.find("parameters") != json.end())
	{
		paramSubValueInfo.parameters = json["parameters"].get<map<string, string>>();
	}

	if (paramSubValueInfo.type == "int")
	{
		paramSubValueInfo.default.intValue = json["defaultValue"].get<int>();

		if (json.find("minValue") != json.end())
		{
			paramSubValueInfo.min.intValue = json["minValue"].get<int>();
		}

		if (json.find("maxValue") != json.end())
		{
			paramSubValueInfo.max.intValue = json["maxValue"].get<int>();
		}
	}
	else if (paramSubValueInfo.type == "float")
	{
		paramSubValueInfo.default.floatValue = json["defaultValue"].get<float>();

		if (json.find("minValue") != json.end())
		{
			paramSubValueInfo.min.floatValue = json["minValue"].get<float>();
		}

		if (json.find("maxValue") != json.end())
		{
			paramSubValueInfo.max.floatValue = json["maxValue"].get<float>();
		}
	}
	else if (paramSubValueInfo.type == "bool")
	{
		paramSubValueInfo.default.intValue = json["defaultValue"].get<int>();
	}

	// Parameters
	if (json.find("parameters") != json.end())
	{
		for (auto iterator = json["parameters"].begin(); iterator != json["parameters"].end(); ++iterator)
		{
			string name = iterator.key();
			string value = iterator.value();
			paramSubValueInfo.parameters.insert({ name, value });
		}
	}

	return paramSubValueInfo;
}