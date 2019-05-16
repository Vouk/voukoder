#pragma once

#include <vector>
#include "ResourceInfo.h"
#include "EncoderGroupInfo.h"
#include "OptionContainer.h"
#include "lavf.h"

struct FilterInfo : public ResourceInfo
{
	AVMediaType type;
	OptionContainer defaults;
	std::vector<EncoderGroupInfo> groups;
};