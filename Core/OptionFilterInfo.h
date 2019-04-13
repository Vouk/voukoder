#pragma once

#include <string>
#include <map>
#include "OptionValue.h"

using namespace std;

struct OptionFilterInfo
{
	typedef map<string, OptionValue> Arguments;
	typedef map<string, vector<Arguments>> Params;

	string name;
	string ownerId;
	Params params;
};
