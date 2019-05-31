#pragma once

#include "FilterInfo.h"
#include "json.hpp"

using namespace nlohmann;

class OptionResourceUtils
{
public:
	static bool CreateOptionInfo(EncoderOptionInfo &optionInfo, const json json);

private:
	static bool CreateOptionFilterInfos(std::vector<OptionFilterInfo> &filters, json resource, std::string id);
	static bool CreateOptionFilterInfo(OptionFilterInfo &optionFilterInfo, const json property, const std::string ownerId);
};

