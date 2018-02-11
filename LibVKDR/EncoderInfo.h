#pragma once

#include <string>
#include <vector>
#include <map>

using namespace std;

namespace LibVKDR
{
	enum EncoderType {
		Audio,
		Video
	};

	struct IParamInfo
	{
		string name;
		string label;
		string type;
		vector<string> flags;
		bool isSubValue;

		union
		{
			int intValue;
			float floatValue = NAN;
		} default;

		union
		{
			int intValue;
			float floatValue = NAN;
		} min;

		union
		{
			int intValue;
			float floatValue = NAN;
		} max;
	};

	struct ParamSubValueInfo : IParamInfo
	{
		string pixelFormat;
		bool useDefaultValue = false;
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
		bool useDefaultValue;
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

	typedef struct EncoderInfo
	{
		int id;
		string name;
		string text;
		string defaultPixelFormat;
		string multipassParameter;
		EncoderType encoderType;
		vector<ParamGroupInfo> groups;
		vector<EncoderParameterGroup> paramGroups;
		vector<ParamInfo> params;
	} EncoderInfo;
}