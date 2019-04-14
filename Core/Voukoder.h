#pragma once

#include <vector>
#include <windows.h>
#include <wx/wx.h>
#include "resource.h"
#include "EncoderInfo.h"
#include "MuxerInfo.h"
#include "FilterInfo.h"
#include "LanguageInfo.h"
#include "json.hpp"

#define VKDR_APPNAME L"Voukoder R2"
#define VKDR_VERSION_MAJOR 2
#define VKDR_VERSION_MINOR 0
#define VKDR_VERSION_PATCH 0
#define VKDR_VERSION_SUFFIX ""
#define VKDR_REG_ROOT "Software\\Voukoder"
#define VKDR_REG_LANGUAGE "Language"

#define DefaultVideoEncoder "libx264"
#define DefaultAudioEncoder "aac"
#define DefaultMuxer "mp4"

#pragma comment(linker, "\"/manifestdependency:type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

static wxString LOG_BUFFER;

static inline HMODULE GetCurrentModule()
{
	HMODULE hModule = NULL;
	GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, (LPCTSTR)GetCurrentModule, &hModule);

	return hModule;
};

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
		vector<EncoderInfo> encoderInfos;
		vector<MuxerInfo> muxerInfos;
		vector<FilterInfo> filterInfos;
		vector<LanguageInfo> languageInfos;

	private:
		Config();
		bool LoadResources(HMODULE hModule, LPTSTR lpType);
		BOOL EnumNamesFunc(HMODULE hModule, LPCTSTR lpType, LPTSTR lpName, LONG_PTR lParam);

	public:
		Config(Config const&) = delete;
		void operator=(Config const&) = delete;
	};

	template <class T> static wxString GetResourceName(vector<T> items, wxString id, wxString def = "")
	{
		for (auto& item : items)
		{
			if (item.id == id)
			{
				return item.name;
			}
		}

		return def;
	}
}