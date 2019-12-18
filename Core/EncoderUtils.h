#pragma once

#include <string>
#include "EncoderInfo.h"
#include "json.hpp"
#include "lavf.h"

using namespace nlohmann;

class EncoderUtils
{
public:
	static bool Create(EncoderInfo &encoderInfo, const json json, bool validateEncoder = true);
	static void InitOptionsWithDefaults(EncoderInfo encoderInfo, OptionContainer &options);
	static std::string GetParameterGroup(EncoderInfo encoderInfo, std::string parameter);
	static bool IsEncoderAvailable(const wxString codecId);

private:
	static AVMediaType GetMediaType(const wxString codecId);
};
