#pragma once

#include <string>

#include "json.hpp"

struct BannerInfo
{
	std::string imageUrl;
	std::string actionUrl;
	bool enabled;
};

void from_json(const nlohmann::json& j, BannerInfo& p);
