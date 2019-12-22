#pragma once

#include <wx/wx.h>
#include <wx/msw/registry.h>

#define VKDR_REG_ROOT "Software\\Voukoder"
#define VKDR_REG_SHOW_ADVANCED_OPTIONS "ShowAdvancedOptions"
#define VKDR_REG_LANGUAGE "Language"
#define VKDR_REG_SEP_LOG_FILES "SeparateLogFiles"

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