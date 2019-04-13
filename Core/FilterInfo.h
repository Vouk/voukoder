#pragma once

#include <vector>
#include "ResourceInfo.h"
#include "EncoderGroupInfo.h"

using namespace std;

struct FilterInfo : public ResourceInfo
{
	vector<EncoderGroupInfo> groups;
};