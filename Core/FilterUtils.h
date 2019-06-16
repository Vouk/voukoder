#pragma once

#include "FilterInfo.h"
#include "json.hpp"

using namespace nlohmann;

class FilterUtils
{
public:
	static bool Create(FilterInfo &filterInfo, const json json);

private:
	static AVMediaType GetMediaType(const wxString filterId);
};
