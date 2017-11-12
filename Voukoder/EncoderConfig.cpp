#include "EncoderConfig.h"
#include <sstream>
#include <boost/format.hpp>
#include <boost/locale.hpp>
#include <boost/algorithm/string/replace.hpp>

EncoderConfig::EncoderConfig(PrSDKExportParamSuite *exportParamSuite, csSDK_uint32 exporterPluginID)
{
	this->exportParamSuite = exportParamSuite;
	this->exporterPluginID = exporterPluginID;
}

EncoderConfig::~EncoderConfig()
{
}

void EncoderConfig::initFromSettings(json encoder)
{
	this->rawConfig = encoder;

	// Start from scratch
	this->config.clear();

	// Iterate all encoder parameters
	for (auto itParam = encoder["params"].begin(); itParam != encoder["params"].end(); ++itParam)
	{
		const json param = *itParam;

		// Is the current value equal to the default value?
		bool isDefaultValue = true;

		// Use the param even if it is set to the default value?
		bool useDefaultValue = param.find("useDefaultValue") != param.end() && param["useDefaultValue"].get<bool>();

		const std::string name = param["name"].get<std::string>();
		const std::string type = param["type"].get<std::string>();

		// Skip some element types
		if (type == "button")
		{
			continue;
		}

		bool isSlider = false;
		bool isCodecParams = false;

		// Flags
		if (param["flags"].is_array())
		{
			for (auto itFlag = param["flags"].begin(); itFlag != param["flags"].end(); ++itFlag)
			{
				const std::string flag = (*itFlag).get<std::string>();

				if (flag == "slider")
				{
					isSlider = true;
					break;
				}
				else if (flag == "codec_params")
				{
					isCodecParams = true;
					break;
				}
			}
		}

		if (type == "string")
		{
			// Get the selection
			exParamValues paramValues;
			exportParamSuite->GetParamValue(this->exporterPluginID, this->multiGroupIndex, name.c_str(), &paramValues);
			std::string value = boost::locale::conv::utf_to_utf<char>(paramValues.paramString);

			// Do some special conversion if there are codec params
			if (isCodecParams)
			{
				// Support params separated by newline
				boost::replace_all(value, "\r\n", ":");
			}

			// Add to options if not empty
			if (!value.empty())
			{
				this->addParameters(param["parameters"], value.c_str());
			}

			continue;
		}

		// Is this a dropdown element?
		if (param.find("values") != param.end() && param["values"].is_array())
		{
			// Get the selection
			exParamValues paramValue;
			exportParamSuite->GetParamValue(this->exporterPluginID, this->multiGroupIndex, name.c_str(), &paramValue);

			// Get the selected value
			json selectedValue = Settings::filterArrayById(param["values"], paramValue.value.intValue);
			if (selectedValue == NULL)
			{
				selectedValue = Settings::filterArrayById(param["values"], param["defaultValue"].get<int>());
			}

			// Process only non-default values
			if (param["defaultValue"].get<int>() != selectedValue["id"].get<int>() || useDefaultValue)
			{
				// Do we have suboptions?
				if (selectedValue.find("subvalues") != selectedValue.end())
				{
					// Iterate all suboptions
					for (auto itSubvalue = selectedValue["subvalues"].begin(); itSubvalue != selectedValue["subvalues"].end(); ++itSubvalue)
					{
						const json subvalue = *itSubvalue;

						const std::string subname = subvalue["name"].get<std::string>();
						const std::string subtype = subvalue["type"].get<std::string>();

						// Get the selected value
						exParamValues paramValue;
						exportParamSuite->GetParamValue(this->exporterPluginID, this->multiGroupIndex, subname.c_str(), &paramValue);
						
						// Format param according to datatype
						if (subtype == "float")
						{
							const float defaultValue = subvalue["defaultValue"].get<float>();
							const bool isDefaultValue = std::fabs(defaultValue - paramValue.value.floatValue) < 1e-2;

							if (!isDefaultValue || useDefaultValue)
							{
								this->addParameters(subvalue["parameters"], paramValue.value.floatValue);
							}
						}
						else if (subtype == "int")
						{
							const int defaultValue = subvalue["defaultValue"].get<int>();
							const bool isDefaultValue = defaultValue == paramValue.value.intValue;

							if (!isDefaultValue || useDefaultValue)
							{
								this->addParameters(subvalue["parameters"], paramValue.value.intValue);
							}
						}
						else if (subtype == "bool")
						{
							const int defaultValue = subvalue["defaultValue"].get<int>();
							const bool isDefaultValue = defaultValue == paramValue.value.intValue;

							if ((!isDefaultValue || useDefaultValue) && paramValue.value.intValue == 1)
							{
								this->addParameters(subvalue["parameters"]);
							}
						}
						else
						{
							continue;
						}
					}
				}
				else // No subvalues
				{
					this->addParameters(selectedValue["parameters"]);
				}
			}
		}
		else // Not a dropdown element
		{
			// Get the selected value
			exParamValues paramValue;
			exportParamSuite->GetParamValue(this->exporterPluginID, this->multiGroupIndex, name.c_str(), &paramValue);

			// Format param according to datatype
			if (type == "float")
			{
				const float defaultValue = param["defaultValue"].get<float>();
				const bool isDefaultValue = std::fabs(defaultValue - paramValue.value.floatValue) < 1e-2;

				if (!isDefaultValue || useDefaultValue)
				{
					this->addParameters(param["parameters"], paramValue.value.floatValue);
				}
			}
			else if (type == "int")
			{
				const int defaultValue = param["defaultValue"].get<int>();
				const bool isDefaultValue = defaultValue == paramValue.value.intValue;

				if (!isDefaultValue || useDefaultValue)
				{
					this->addParameters(param["parameters"], paramValue.value.intValue);
				}
			}
			else if (type == "bool")
			{
				const int defaultValue = param["defaultValue"].get<int>();
				const bool isDefaultValue = defaultValue == paramValue.value.intValue;

				if ((!isDefaultValue || useDefaultValue) && paramValue.value.intValue == 1)
				{
					this->addParameters(param["parameters"]);
				}
			}
			else
			{
				continue;
			}
		}
	}

	exParamValues fieldType;
	exportParamSuite->GetParamValue(this->exporterPluginID, 0, ADBEVideoFieldType, &fieldType);

	// Add field order for interlaced output
	if (fieldType.value.intValue == prFieldsUpperFirst)
	{
		this->addParameters(encoder["interlaced"]["tffParameters"]);
	}
	else if (fieldType.value.intValue == prFieldsLowerFirst)
	{
		this->addParameters(encoder["interlaced"]["bffParameters"]);
	}
}

void EncoderConfig::setParam(std::string key, std::string value)
{
	this->config.insert(std::pair<std::string, std::string>(key, value));
}

void EncoderConfig::parseCommandLine(std::string input)
{
	int g = input.length();
}

void EncoderConfig::getConfig(AVDictionary **options)
{
	return this->getConfig(options, 0, 0);
}

void EncoderConfig::getConfig(AVDictionary **options, int maxPasses, int pass)
{
	// Add all config parameters
	AddParametersToDictionary(options, this->config);

	// Multipass encoding
	if (maxPasses > 0)
	{
		json parameters;

		if (pass == 1)
		{
			parameters = this->rawConfig["multipass"]["firstParameters"];
		}
		else if (pass >= maxPasses)
		{
			parameters = this->rawConfig["multipass"]["lastParameters"];
		}
		else
		{
			parameters = this->rawConfig["multipass"]["nthParameters"];
		}

		// Add multipass parameters
		AddParametersToDictionary(options, parameters);
	}
}

std::string EncoderConfig::getConfigAsParamString(std::string dashes)
{
	std::stringstream configStream;

	int i = 0;
	for (auto itParameter = this->config.begin(); itParameter != this->config.end(); ++itParameter)
	{
		// Separate parameters by space
		if (i++ > 0)
		{
			configStream << ' ';
		}

		// Append the name
		configStream << dashes;
		configStream << itParameter->first;

		// Remove all null terminators from the string
		std::string value = itParameter->second;
		value.erase(std::remove(value.begin(), value.end(), '\0'), value.end());

		// If the string is not empty append a space and the value
		if (!value.empty())
		{
			configStream << ' ';
			configStream << value;
		}
	}

	return configStream.str();
}

int EncoderConfig::getMaxPasses()
{
	int passes = 1;

	if (this->rawConfig.find("multipass") != this->rawConfig.end())
	{
		json multipass = this->rawConfig["multipass"];
		if (multipass.find("name") != multipass.end())
		{
			const std::string name = multipass["name"].get<std::string>();

			exParamValues maxPasses;
			this->exportParamSuite->GetParamValue(this->exporterPluginID, this->multiGroupIndex, name.c_str(), &maxPasses);

			passes = maxPasses.value.intValue;
		}
	}

	return passes;
}

void EncoderConfig::addParameters(json parameters)
{
	this->addParameters(parameters, NULL);
}

template<class T>
void EncoderConfig::addParameters(json parameters, T value)
{
	char buffer[64];
	std::pair<std::string, std::string> pair;

	// Add all parameters to the list
	for (auto itParameter = parameters.begin(); itParameter != parameters.end(); ++itParameter)
	{
		const std::string pName = itParameter.key();
		const std::string pPattern = itParameter.value();
		
		std::string pValue;
		const int length = sprintf_s(buffer, pPattern.c_str(), value);

		// 
		if (length > 0)
		{
			pValue = std::string(buffer, length + 1);
		}

		ParamGroup *group = this->getGroupFromParameter(pName);
		if (group != NULL)
		{
			// Handle no-value params
			if (pValue.empty() && !group->noValueReplacement.empty())
			{
				pValue = group->noValueReplacement;
			}

			std::stringstream sbuffer;

			// Find existing values
			std::map<std::string, std::string>::iterator it = this->config.find(group->name);
			if (it != this->config.end())
			{
				std::string current = it->second;
				current.erase(std::remove(current.begin(), current.end(), '\0'), current.end());;
				sbuffer << current;
				sbuffer << group->separator;
				this->config.erase(it);
			}

			sbuffer << pName;
			sbuffer << group->delimiter;
			sbuffer << pValue;

			pair = std::pair<std::string, std::string>(group->name, sbuffer.str());
		}
		else
		{
			pair = std::pair<std::string, std::string>(pName, pValue);
		}

		// Add key/value pair
		this->config.insert(pair);
	}
}

ParamGroup* EncoderConfig::getGroupFromParameter(std::string name)
{
	const auto groups = this->rawConfig["parameterGroups"];

	for (auto itGroup = groups.begin(); itGroup != groups.end(); ++itGroup)
	{
		std::vector<std::string> parameters = itGroup.value()["parameters"];

		if (std::find(parameters.begin(), parameters.end(), name) != parameters.end())
		{
			ParamGroup *pGroup = new ParamGroup;
			pGroup->name = itGroup.key();
			pGroup->delimiter = itGroup.value()["delimiter"].get<std::string>();
			pGroup->separator = itGroup.value()["separator"].get<std::string>();
			pGroup->noValueReplacement = itGroup.value()["noValueReplacement"].get<std::string>();
			pGroup->parameters = parameters;

			return pGroup;
		}
	}

	return NULL;
}

void EncoderConfig::AddParametersToDictionary(AVDictionary **options, std::map<std::string, std::string> parameters)
{
	for (auto itParameter = parameters.begin(); itParameter != parameters.end(); ++itParameter)
	{
		const std::string name = itParameter->first;
		const std::string value = itParameter->second;

		// Req'd because AVDictionary does not support no-value entries
		if (value.length() == 0)
		{
			av_dict_set(options, name.c_str(), "1", 0);
		}
		else
		{
			av_dict_set(options, name.c_str(), value.c_str(), 0);
		}
	}
}