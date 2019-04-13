#pragma once

#include <vector>
#include "ResourceInfo.h"
#include "EncoderGroupInfo.h"
#include "EncoderOptionInfo.h"
#include "OptionContainer.h"
#include "lavf.h"

using namespace std;

struct EncoderInfo : public ResourceInfo
{
	AVMediaType type;
	OptionContainer defaults;
	vector<EncoderGroupInfo> groups;
	map<string, vector<string>> paramGroups;
};