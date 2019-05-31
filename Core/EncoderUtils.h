#pragma once

#include <string>
#include "EncoderInfo.h"
#include "json.hpp"
#include "lavf.h"

using namespace nlohmann;

class EncoderUtils
{
public:
	static bool Create(EncoderInfo &encoderInfo, const json json);
	static void InitOptionsWithDefaults(EncoderInfo encoderInfo, OptionContainer &options);
	static std::string GetParameterGroup(EncoderInfo encoderInfo, std::string parameter);

private:
	static bool IsAvailable(const std::string codecId);
	static AVMediaType GetMediaType(const std::string codecId);
	static void CreateEncoderOptionPresetGroup(EncoderOptionPresetGroup &parent, const json json);
};
