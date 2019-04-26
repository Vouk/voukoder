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
	static bool IsAvailable(const string codecId);
	static AVMediaType GetMediaType(const string codecId);
	static void CreateEncoderOptionPresetGroup(EncoderOptionPresetGroup &parent, const json json);
};