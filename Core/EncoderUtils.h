#pragma once

#include <string>
#include "EncoderInfo.h"
#include "json.hpp"
#include "lavf.h"

using namespace std;
using namespace nlohmann;

class EncoderUtils
{
public:
	static bool Create(EncoderInfo &encoderInfo, const json json);
	static void InitOptionsWithDefaults(EncoderInfo encoderInfo, OptionContainer &options);
	static string GetParameterGroup(EncoderInfo encoderInfo, string parameter);

private:
	static bool CreateOptionInfo(EncoderOptionInfo &optionInfo, const json json);
	static bool CreateOptionFilterInfos(vector<OptionFilterInfo> &filters, json resource, string id);
	static bool CreateOptionFilterInfo(OptionFilterInfo &optionFilterInfo, const json property, const string ownerId);
	static bool IsAvailable(const string codecId);
	static AVMediaType GetMediaType(const string codecId);
};