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

#include <vector>

#include <wx/wx.h>
#include "resource.h"
#include "EncoderInfo.h"
#include "MuxerInfo.h"
#include "LanguageInfo.h"
#include "json.hpp"
#include "Version.h"
#include "Log.h"
#include "PluginUpdate.h"

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#define VKDR_APPNAME L"Voukoder"

#define DefaultVideoEncoder "libx264"
#define DefaultAudioEncoder "aac"
#define DefaultMuxer "mp4"

#pragma comment(linker, "\"/manifestdependency:type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

static wxString LOG_BUFFER;

#ifdef _WIN32
static inline HMODULE GetCurrentModule()
{
	HMODULE hModule = NULL;
	GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, (LPCTSTR)GetCurrentModule, &hModule);

	return hModule;
};
#endif

namespace Voukoder
{
	class Config
	{
	public:
		static Config& Get()
		{
			static Config instance;
			return instance;
		}
		EncoderInfo videoSideData;
		EncoderInfo audioSideData;
		std::vector<EncoderInfo> videoEncoderInfos;
		std::vector<EncoderInfo> audioEncoderInfos;
		std::vector<MuxerInfo> muxerInfos;
		std::vector<EncoderInfo> videoFilterInfos;
		std::vector<EncoderInfo> audioFilterInfos;
		std::vector<LanguageInfo> languageInfos;
		PluginUpdate update;

	private:
		Config();
#ifdef _WIN32
		bool LoadResources(HMODULE hModule, LPTSTR lpType);
		BOOL EnumNamesFunc(HMODULE hModule, LPCTSTR lpType, LPTSTR lpName, LONG_PTR lParam);
#endif

	public:
		Config(Config const&) = delete;
		void operator=(Config const&) = delete;
	};

	static wxString GetAppVersion()
	{
		return VKDR_VERSION_PUBLIC;
	}

	static wxString GetApplicationName()
	{
		return wxString::Format("%s %s", VKDR_APPNAME, GetAppVersion());
	}
	   
	template <class T> static wxString GetResourceName(std::vector<T> items, wxString id, wxString def = "")
	{
		for (auto& item : items)
		{
			if (item.id == id)
				return item.name;
		}

		return def;
	}
}
