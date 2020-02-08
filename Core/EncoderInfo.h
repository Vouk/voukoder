#pragma once

#include <vector>
#include "ResourceInfo.h"
#include "EncoderGroupInfo.h"
#include "EncoderOptionInfo.h"
#include "OptionContainer.h"
#include "PresetInfo.h"
#include "lavf.h"

struct EncoderInfo : public ResourceInfo
{
	AVMediaType type = AVMEDIA_TYPE_UNKNOWN;
	OptionContainer defaults;
	std::vector<PresetInfo> presets;
	std::vector<EncoderGroupInfo> groups;
	std::map<std::string, std::vector<std::string>> paramGroups;

	bool operator< (const EncoderInfo& other) const
	{
		return (id < other.id);
	}
};
