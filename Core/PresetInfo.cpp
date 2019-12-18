#include "PresetInfo.h"

void from_json(const json& j, PresetInfo& p)
{
	j.at("name").get_to(p.name);
	j.at("description").get_to(p.description);
	j.at("options").get_to(p.options);
}