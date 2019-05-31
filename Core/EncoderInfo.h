#pragma once

#include <vector>
#include "ResourceInfo.h"
#include "EncoderGroupInfo.h"
#include "EncoderOptionInfo.h"
#include "EncoderOptionPresets.h"
#include "OptionContainer.h"
#include "lavf.h"

struct EncoderInfo : public ResourceInfo
{
	AVMediaType type;
	OptionContainer defaults;
	std::vector<EncoderOptionPresetGroup> presets;
	std::vector<EncoderGroupInfo> groups;
	std::map<std::string, std::vector<std::string>> paramGroups;
};
