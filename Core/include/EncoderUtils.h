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
#include "EncoderInfo.h"
#include "json.hpp"
#include "lavf.h"

using namespace nlohmann;

class EncoderUtils
{
public:
	static bool Create(EncoderInfo &encoderInfo, const json json, bool validateEncoder = true);
	static void InitOptionsWithDefaults(EncoderInfo encoderInfo, OptionContainer &options);
	static std::string GetParameterGroup(EncoderInfo encoderInfo, std::string parameter);
	static bool IsEncoderAvailable(const wxString codecId);

private:
	static AVMediaType GetMediaType(const wxString codecId);
};
