#pragma once

#include <string>
#include <map>
#include "OptionValue.h"

struct OptionFilterInfo
{
	typedef std::map<std::string, OptionValue> Arguments;
	typedef std::map<std::string, std::vector<Arguments>> Params;

	std::string name;
	std::string ownerId;
	Params params;
};
