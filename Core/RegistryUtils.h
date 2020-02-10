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

#include <wx/wx.h>
#include <wx/msw/registry.h>

#define VKDR_REG_ROOT "Software\\Voukoder"
#define VKDR_REG_SHOW_ADVANCED_OPTIONS "ShowAdvancedOptions"
#define VKDR_REG_LANGUAGE "Language"
#define VKDR_REG_SEP_LOG_FILES "SeparateLogFiles"
#define VKDR_REG_LOW_LEVEL_LOGGING "LowLevelLogging"

class RegistryUtils
{
public:
	static long GetValue(wxString regkey, long def)
	{
#ifdef _WIN32
		// Does the user have stored a selection in the registry?
		wxRegKey key(wxRegKey::HKCU, VKDR_REG_ROOT);
		if (key.Exists() && key.HasValue(regkey))
		{
			long val;
			key.QueryValue(regkey, &val);

			return val;
		}
#endif
		return def;
	}

	static bool GetValue(wxString regkey, bool def)
	{
		return GetValue(regkey, (long)def);
	}

	static void SetValue(wxString regkey, long value)
	{
#ifdef _WIN32
		wxRegKey key(wxRegKey::HKCU, VKDR_REG_ROOT);
		if (!key.Exists())
			key.Create();

		key.SetValue(regkey, value);
#endif
	}

	static void SetValue(wxString regkey, bool value)
	{
		SetValue(regkey, (long)value);
	}
};