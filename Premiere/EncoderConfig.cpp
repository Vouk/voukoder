#include "EncoderConfig.h"
#include "EncoderUtils.h"
#include <sstream>

// reviewed 0.3.8
EncoderConfig::EncoderConfig(PrSDKExportParamSuite *exportParamSuite, csSDK_uint32 exporterPluginID)
{
	this->exportParamSuite = exportParamSuite;
	this->exporterPluginID = exporterPluginID;
}

// reviewed 0.3.8
void EncoderConfig::initFromSettings(EncoderInfo *encoderInfo)
{
	this->encoderInfo = encoderInfo;
	
	// Set default pixel format
	pixelFormat = encoderInfo->defaultPixelFormat;

	multipassParameter.clear();

	// Start from scratch
	config.clear();

	// Iterate all encoder parameters
	for (ParameterInfo parameterInfo: encoderInfo->params)
	{
		// Skip some element types
		if (parameterInfo.type == "button")
		{
			continue;
		}

		// Is this a dropdown element?
		if (parameterInfo.values.size() > 0)
		{
			// Get the selection
			exParamValues paramValue;
			exportParamSuite->GetParamValue(this->exporterPluginID, this->multiGroupIndex, parameterInfo.name.c_str(), &paramValue);

			// Get the selected value (fallback to default value (fallback to 0))
			const int selectedParamIdx = FindVectorIndexById(&parameterInfo.values, paramValue.value.intValue, FindVectorIndexById(&parameterInfo.values, parameterInfo.default.intValue, 0));
			ParameterValueInfo parameterValueInfo = parameterInfo.values.at(selectedParamIdx);

			// Do we have suboptions?
			if (parameterValueInfo.subValues.size() > 0)
			{
				// Iterate all suboptions
				for (ParameterSubValue parameterSubValue: parameterValueInfo.subValues)
				{
					if (parameterSubValue.pixelFormat.size() > 0)
					{
						pixelFormat = parameterSubValue.pixelFormat;
					}

					// Set multipass parameter
					if (parameterSubValue.name == encoderInfo->multipassParameter)
					{
						multipassParameter = parameterSubValue.name;
					}

					// Get the selected value
					exParamValues paramValue;
					exportParamSuite->GetParamValue(this->exporterPluginID, this->multiGroupIndex, parameterSubValue.name.c_str(), &paramValue);
						
					// Format param according to datatype
					if (parameterSubValue.type == "float")
					{
						const bool isDefaultValue = std::fabs(parameterSubValue.default.floatValue - paramValue.value.floatValue) < 1e-2;

						if (!isDefaultValue || parameterSubValue.useDefaultValue)
						{
							this->addParameters(parameterSubValue.parameters, paramValue.value.floatValue);
						}
					}
					else if (parameterSubValue.type == "int")
					{
						const bool isDefaultValue = parameterSubValue.default.intValue == paramValue.value.intValue;

						if (!isDefaultValue || parameterSubValue.useDefaultValue)
						{
							this->addParameters(parameterSubValue.parameters, paramValue.value.intValue);
						}
					}
					else if (parameterSubValue.type == "bool")
					{
						const bool isDefaultValue = parameterSubValue.default.intValue == paramValue.value.intValue;

						if ((!isDefaultValue || parameterSubValue.useDefaultValue) && paramValue.value.intValue == 1)
						{
							this->addParameters(parameterSubValue.parameters);
						}
					}
					else
					{
						continue;
					}
				}
			}
			else if (parameterInfo.default.intValue != parameterValueInfo.id || parameterInfo.useDefaultValue)
			{
				this->addParameters(parameterValueInfo.parameters);

				// Does the element has a pixel format set?
				if (parameterValueInfo.pixelFormat.size() > 0)
				{
					pixelFormat = parameterValueInfo.pixelFormat;
				}

				// Set multipass parameter
				if (parameterValueInfo.name == encoderInfo->multipassParameter)
				{
					multipassParameter = parameterValueInfo.name;
				}

			}

		}
		else // Not a dropdown element
		{
			// Get the selected value
			exParamValues paramValue;
			exportParamSuite->GetParamValue(this->exporterPluginID, this->multiGroupIndex, parameterInfo.name.c_str(), &paramValue);

			// Set multipass parameter
			if (parameterInfo.name == encoderInfo->multipassParameter)
			{
				multipassParameter = parameterInfo.name;
			}

			// Format param according to datatype
			if (parameterInfo.type == "float")
			{
				const bool isDefaultValue = std::fabs(parameterInfo.default.floatValue - paramValue.value.floatValue) < 1e-2;

				if (!isDefaultValue || parameterInfo.useDefaultValue)
				{
					this->addParameters(parameterInfo.parameters, paramValue.value.floatValue);
				}
			}
			else if (parameterInfo.type == "int")
			{
				const bool isDefaultValue = parameterInfo.default.intValue == paramValue.value.intValue;

				if (!isDefaultValue || parameterInfo.useDefaultValue)
				{
					this->addParameters(parameterInfo.parameters, paramValue.value.intValue);
				}
			}
			else if (parameterInfo.type == "bool")
			{
				const bool isDefaultValue = parameterInfo.default.intValue == paramValue.value.intValue;

				if ((!isDefaultValue || parameterInfo.useDefaultValue) && paramValue.value.intValue == 1)
				{
					this->addParameters(parameterInfo.parameters);
				}
			}
			else
			{
				continue;
			}
		}
	}
}

// reviewed 0.3.8
int EncoderConfig::getMaxPasses()
{
	int passes = 1;

	if (multipassParameter.size() > 0)
	{
		// Get max passes setting
		exParamValues maxPasses;
		this->exportParamSuite->GetParamValue(this->exporterPluginID, this->multiGroupIndex, multipassParameter.c_str(), &maxPasses);

		passes = maxPasses.value.intValue;
	}

	return passes;
}

// reviewed 0.3.8
void EncoderConfig::parseCommandLine(std::string input)
{
	//int g = input.length();
}

// reviewed 0.3.8
void EncoderConfig::getConfig(AVDictionary **options)
{
	return getConfig(options, 1, 1);
}

// reviewed 0.3.8
void EncoderConfig::getConfig(AVDictionary **options, int maxPasses, int pass)
{
	// Add all config parameters
	AddParametersToDictionary(options, config);
}

// reviewed 0.3.8
string EncoderConfig::getConfigAsParamString(string dashes)
{
	stringstream configStream;

	int i = 0;
	for (pair<string, string> parameter: config)
	{
		// Separate parameters by space
		if (i++ > 0)
		{
			configStream << ' ';
		}

		// Append the name
		configStream << dashes;
		configStream << parameter.first;

		// Remove all null terminators from the string
		string value = parameter.second;
		value.erase(remove(value.begin(), value.end(), '\0'), value.end());

		// If the string is not empty append a space and the value
		if (!value.empty())
		{
			configStream << ' ';
			configStream << value;
		}
	}

	return configStream.str();
}

// reviewed 0.3.8
string EncoderConfig::getPixelFormat()
{
	return pixelFormat;
}

// reviewed 0.3.8
void EncoderConfig::addParameters(map<string, string> parameters)
{
	addParameters(parameters, NULL);
}

// reviewed 0.3.8
template<class T>
void EncoderConfig::addParameters(map<string, string> parameters, T value)
{
	char buffer[64];

	// Add all parameters to the list
	for (pair<string, string> parameter: parameters)
	{
		// Replace pattern with value
		const int length = sprintf_s(buffer, parameter.second.c_str(), value);
		if (length > 0)
		{
			parameter.second = string(buffer, length + 1);
		}

		// Does this param belong to a group?
		ParamGroup group;
		if (getGroupFromParameter(parameter.first, group))
		{
			// Handle no-value params
			if (parameter.second.empty() && !group.noValueReplacement.empty())
			{
				parameter.second = group.noValueReplacement;
			}

			stringstream sbuffer;

			// Find existing values
			map<string, string>::iterator it = config.find(group.name);
			if (it != this->config.end())
			{
				string current = it->second;
				current.erase(remove(current.begin(), current.end(), '\0'), current.end());;
				sbuffer << current;
				sbuffer << group.separator;
				this->config.erase(it);
			}

			sbuffer << parameter.first;
			sbuffer << group.delimiter;
			sbuffer << parameter.second;

			config.insert(pair<string, string>(group.name, sbuffer.str()));
		}
		else
		{
			config.insert(parameter);
		}
	}
}

// reviewed 0.3.8
bool EncoderConfig::getGroupFromParameter(string name, ParamGroup &retGroup)
{
	for (ParamGroup group: encoderInfo->paramGroups)
	{
		if (find(group.parameters.begin(), group.parameters.end(), name) != group.parameters.end())
		{
			retGroup = group;
			return true;
		}
	}

	return false;
}

// reviewed 0.3.8
void EncoderConfig::AddParametersToDictionary(AVDictionary **options, map<string, string> parameters)
{
	for (pair<string, string> parameter: parameters)
	{
		if (parameter.second.length() == 0)
		{
			av_dict_set(options, parameter.first.c_str(), "1", 0);
		}
		else
		{
			av_dict_set(options, parameter.first.c_str(), parameter.second.c_str(), 0);
		}
	}
}