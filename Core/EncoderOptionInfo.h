#pragma once

#include <string>
#include <vector>
#include <map>
#include "OptionValue.h"
#include "OptionFilterInfo.h"

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
	std::string id;
	std::wstring name;
	std::wstring description;
	std::string parameter;
	std::map<std::string, std::string> extraOptions;
	int multiplicationFactor = 1;
	std::vector<OptionFilterInfo> filters;
	bool isForced = false;
	bool isActive = false;
	bool preprendNoIfFalse = false;

	struct ComboItem
	{
		std::string id;
		std::wstring name;
		std::string value;
		std::map<std::string, std::string> extraOptions;
		std::vector<OptionFilterInfo> filters;
	};

	struct
	{
		OptionValue value;
		OptionValue minimum;
		OptionValue maximum;
		OptionValue singleStep;
		EncoderOptionType type;
		std::vector<ComboItem> items;
		int selectedIndex = 0;
		bool enabled = true;
		std::string regex;
	} control;
};
