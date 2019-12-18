#pragma once

#include <wx/wx.h>
#include <map>
#include "json.hpp"

using nlohmann::json;

struct PresetInfo
{
	std::string name;
	std::string description;
	std::map<std::string, std::string> options;
};

void from_json(const nlohmann::json& j, PresetInfo& p);