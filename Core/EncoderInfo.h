#pragma once

#include <vector>
#include "ResourceInfo.h"
#include "EncoderGroupInfo.h"
#include "EncoderOptionInfo.h"
#include "EncoderOptionPresets.h"
#include "OptionContainer.h"
#include "lavf.h"

using namespace std;

struct EncoderInfo : public ResourceInfo
{
	AVMediaType type;
	OptionContainer defaults;
	vector<EncoderOptionPresetGroup> presets;
	vector<EncoderGroupInfo> groups;
	map<string, vector<string>> paramGroups;
};