#pragma once

#include <vector>
#include "EncoderOptionInfo.h"

using namespace std;

struct EncoderGroupInfo
{
	string id;
	wstring name;
	string groupClass;
	vector<EncoderOptionInfo> options;
};