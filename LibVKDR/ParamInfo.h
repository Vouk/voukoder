#pragma once

#include <string>
#include <vector>
#include <map>

using namespace std;

namespace LibVKDR
{
	struct IParamInfo
	{
		string name;
		string label;
		string description;
		string type;
		vector<string> flags;
		bool isSubValue;
		string defaultStringValue = "";
		bool useDefaultValue = false;

		union
		{
			int intValue;
			float floatValue;
		} default;

		union
		{
			int intValue;
			float floatValue;
		} min;
		bool minSet = false;

		union
		{
			int intValue;
			float floatValue;
		} max;
		bool maxSet = false;
	};

	struct ParamSubValueInfo : IParamInfo
	{
		string pixelFormat;
		map<string, string> parameters;
	};

	struct ParamValueInfo
	{
		int id;
		string name;
		string pixelFormat;
		vector<ParamSubValueInfo> subValues;
		map<string, string> parameters;
	};

	struct ParamInfo : IParamInfo
	{
		string group;
		vector<ParamValueInfo> values;
		map<string, string> parameters;
	};

	struct ParamGroupInfo
	{
		string name;
		string label;
		string parent;
	};

	struct EncoderParameterGroup
	{
		string name;
		string delimiter;
		string separator;
		string noValueReplacement;
		vector<string> parameters;
	};

	struct IParamContainer {
		int id;
		string name;
		vector<ParamGroupInfo> groups;
		vector<ParamInfo> params;
	};
}