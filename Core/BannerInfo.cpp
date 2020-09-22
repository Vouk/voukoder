#include "BannerInfo.h"

void from_json(const nlohmann::json& j, BannerInfo& p)
{
	j.at("imageUrl").get_to(p.imageUrl);
	j.at("actionUrl").get_to(p.actionUrl);
	j.at("enabled").get_to(p.enabled);
}