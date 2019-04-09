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

#pragma comment(linker, "\"/manifestdependency:type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

using namespace nlohmann;

static inline HMODULE GetCurrentModule()
{
	HMODULE hModule = NULL;
	GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, (LPCTSTR)GetCurrentModule, &hModule);

	return hModule;
}

struct Configuration
{
	wxString DefaultVideoEncoder = "libx264";
	wxString DefaultAudioEncoder = "aac";
	wxString DefaultMuxer = "mp4";
	vector<EncoderInfo> encoderInfos;
	vector<MuxerInfo> muxerInfos;
	vector<FilterInfo> filterInfos;
	vector<LanguageInfo> languageInfos;
};

class Voukoder
{
public:
	void Init();
	const Configuration* GetConfiguration();

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

private:
	Configuration configuration;
	bool LoadResources(HMODULE hModule, LPTSTR lpType);
	BOOL EnumNamesFunc(HMODULE hModule, LPCTSTR lpType, LPTSTR lpName, LONG_PTR lParam);
};

