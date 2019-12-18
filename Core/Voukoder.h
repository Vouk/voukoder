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
