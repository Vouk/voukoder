#pragma once

#include <vector>
#include "ResourceInfo.h"
#include "EncoderGroupInfo.h"
#include "lavf.h"

struct FilterInfo : public ResourceInfo
{
	AVMediaType type;
	std::vector<EncoderGroupInfo> groups;
};