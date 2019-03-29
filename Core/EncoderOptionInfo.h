#pragma once

#include <string>
#include <vector>
#include <map>
#include "OptionValue.h"
#include "OptionFilterInfo.h"

using namespace std;

enum struct EncoderOptionType
{
	Undefined,
	Integer,
	Float,
	Boolean,
	ComboBox,
	String
};

struct EncoderOptionInfo
{
	string id;
	wstring name;
	wstring description;
	string parameter;
	string format;
	vector<OptionFilterInfo> filters;
	bool isForced = false;
	bool isActive = false;
	bool preprendNoIfFalse = false;

	struct ComboItem
	{
		string id;
		wstring name;
		string value;
		vector<OptionFilterInfo> filters;
	};

	struct
	{
		OptionValue value;
		OptionValue minimum;
		OptionValue maximum;
		OptionValue singleStep;
		EncoderOptionType type;
		vector<ComboItem> items;
		int selectedIndex = 0;
		bool enabled = true;
		string regex;
	} control;
};