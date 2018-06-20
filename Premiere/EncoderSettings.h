#pragma once

#include <string>
#include <regex>
#include <map>
#include "..\LibVKDR\LibVKDR.h"

using namespace std;
using namespace LibVKDR;

struct EncoderSettings
{
	string name;
	string text;
	string pixelFormat;
	csSDK_int32 passes;
	map<string, string> params;

	void addParams(IParamInfo paramInfo, map<string, string> parameters, exParamValues paramValue, vector<EncoderParameterGroup> paramGroups)
	{
		char buffer[256];

		for (pair<string, string> parameter : parameters)
		{
			if (paramInfo.type == "float")
			{
				if ((fabs(paramInfo.default.floatValue - paramValue.value.floatValue) < 1e-2) && !paramInfo.useDefaultValue)
				{
					return;
				}

				sprintf_s(buffer, parameter.second.c_str(), paramValue.value.floatValue);
			}
			else if (paramInfo.type == "int")
			{
				if (paramInfo.default.intValue == paramValue.value.intValue && !paramInfo.useDefaultValue)
				{
					return;
				}

				sprintf_s(buffer, parameter.second.c_str(), paramValue.value.intValue);
			}
			else if (paramInfo.type == "bool")
			{
				if ((paramInfo.default.intValue == paramValue.value.intValue && !paramInfo.useDefaultValue) || paramValue.value.intValue == 0)
				{
					return;
				}

				sprintf_s(buffer, "%d", paramValue.value.intValue);
			}
			else if (paramInfo.type == "string")
			{
				const wstring defaultValue(paramInfo.defaultStringValue.begin(), paramInfo.defaultStringValue.end());
				wstring currentValue(paramValue.paramString);

				if (defaultValue == currentValue && !paramInfo.useDefaultValue)
				{
					return;
				}

				// Make this more nice
				currentValue = std::regex_replace(currentValue, std::wregex(L":"), L"\\\\:");
				currentValue = std::regex_replace(currentValue, std::wregex(L"="), L"\\\\=");

				size_t l = 0;
				wcstombs_s(&l, buffer, sizeof(buffer), currentValue.c_str(), currentValue.length());
			}

			string value(buffer);

			bool isInGroup = false;

			for (EncoderParameterGroup group : paramGroups)
			{
				if (find(group.parameters.begin(), group.parameters.end(), parameter.first) != group.parameters.end())
				{
					if (value.length() == 0 && !group.noValueReplacement.empty() && paramInfo.type != "string")
					{
						value = group.noValueReplacement;
					}

					stringstream sbuffer;

					map<string, string>::iterator it = params.find(group.name);
					if (it != params.end())
					{
						string current = it->second;
						current.erase(remove(current.begin(), current.end(), '\0'), current.end());
						sbuffer << current;
						sbuffer << group.separator;
						params.erase(it);
					}

					sbuffer << parameter.first;
					sbuffer << group.delimiter;
					sbuffer << value;

					params.insert(make_pair(group.name, sbuffer.str()));

					isInGroup = true;
					break;
				}
			}

			if (!isInGroup)
				params.insert(make_pair(parameter.first, value));
		}
	};

	string toString()
	{
		stringstream stream;
		for (pair<string, string> kv : params)
		{
			string key = kv.first;
			key.erase(remove(key.begin(), key.end(), '\0'), key.end());

			string value = kv.second;
			value.erase(remove(value.begin(), value.end(), '\0'), value.end());

			if (stream.tellp() > 0)
			{
				stream << ",";
			}

			stream << key;
			stream << "=";
			stream << value;
		}

		return stream.str();
	};
};