#pragma once

#include "filterInfo.h"
#include "json.hpp"

using namespace nlohmann;

class FilterUtils
{
public:
	static bool Create(FilterInfo &filterInfo, const json json);
	static void AutoFill(vector<FilterInfo> &filterInfos);

private:
	static AVMediaType GetMediaType(const wxString filterId);
};