#pragma once

#include <vector>
#include "EncoderOptionInfo.h"

struct EncoderGroupInfo
{
	std::string id;
	std::wstring name;
	std::string groupClass;
	std::vector<EncoderOptionInfo> options;
};
