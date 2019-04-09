#pragma once

#include "FilterInfo.h"
#include "json.hpp"

using namespace std;
using namespace nlohmann;

class OptionResourceUtils
{
public:
	static bool CreateOptionInfo(EncoderOptionInfo &optionInfo, const json json);

private:
	static bool CreateOptionFilterInfos(vector<OptionFilterInfo> &filters, json resource, string id);
	static bool CreateOptionFilterInfo(OptionFilterInfo &optionFilterInfo, const json property, const string ownerId);
};

