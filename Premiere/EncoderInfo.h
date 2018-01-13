#pragma once

#include <string>
#include "json.hpp"
#include "EncoderUtils.h"

using namespace std;
using json = nlohmann::json;

// reviewed 0.3.8
class EncoderInfo
{
public:
	int id;
	string name;
	bool experimental = false;
	string defaultPixelFormat;
	vector<ElementGroup> groups;
	string multipassParameter;
	int threadCount = 0;
	vector<ParamGroup> paramGroups;
	vector<ParameterInfo> params;
	EncoderInfo(json encoderSettings);

private:
	json encoder;
	ParameterInfo createParamInfo(json json);
	ParameterValueInfo createValueInfo(json json);
	ParameterSubValue createSubValue(json json);
};