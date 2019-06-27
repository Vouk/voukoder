#pragma once

#include <vector>
#include "ResourceInfo.h"
#include "EncoderGroupInfo.h"
#include "OptionContainer.h"
#include "lavf.h"

struct FilterInfo : public ResourceInfo
{
	AVMediaType type = AVMEDIA_TYPE_UNKNOWN;
	OptionContainer defaults;
	std::vector<EncoderGroupInfo> groups;
};
