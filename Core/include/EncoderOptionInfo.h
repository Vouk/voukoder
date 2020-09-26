/**
 * Voukoder
 * Copyright (C) 2017-2020 Daniel Stankewitz, All Rights Reserved
 * https://www.voukoder.org
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 * http://www.gnu.org/copyleft/gpl.html
 */
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
	bool prependNoIfFalse = false;

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
