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

enum struct OptionValueType {
	Empty,
	Integer,
	Float,
	Boolean,
	String
};

struct OptionValue
{
	int intValue = 0;
	float floatValue = 0.0f;
	std::string stringValue;
	bool boolValue = false;
	OptionValueType type = OptionValueType::Empty;

	OptionValue() {};
	OptionValue(std::string value) : stringValue(value), type(OptionValueType::String) {};
	OptionValue(int value) : intValue(value), type(OptionValueType::Integer) {};
	OptionValue(float value) : floatValue(value), type(OptionValueType::Float) {};
	OptionValue(bool value) : boolValue(value), type(OptionValueType::Boolean) {};

	const std::string toString()
	{
		switch (type)
		{
		case OptionValueType::Integer:
			return std::to_string(intValue);
		case OptionValueType::Float:
			return std::to_string(floatValue);
		case OptionValueType::Boolean:
			return boolValue ? "1" : "0";
		default:
			return stringValue;
		}
	}
};
