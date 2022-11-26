/**
 * Voukoder
 * Copyright (C) 2017-2022 Daniel Stankewitz, All Rights Reserved
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

#include <wx/wx.h>

#include "json.hpp"

using namespace nlohmann;

#define VKDR_UPDATE_CHECK_URL "http://version.voukoder.org/check.php?host=premiere&ver=%d.%d.%d"

union Version
{
	struct number
	{
		uint8_t build;
		uint8_t patch;
		uint8_t minor;
		uint8_t major;
	} number;
	uint32_t code = 0;
};

struct PluginUpdate
{
	Version version;
	wxString url;
	wxString headline;
	wxString message;
	bool isUpdateAvailable = false;
};

bool CheckForUpdate(const Version version, PluginUpdate* pluginUpdate);