#pragma once

#include <windows.h>
#include <map>
#include "resource.h"
#include "json.hpp"
#include "Voukoder.h"

using namespace nlohmann;

static std::map<wxString, wxString> __translations;

static const wxString Trans(wxString key, const wxString sub = "")
{
	if (__translations.size() == 0)
	{
		HMODULE hMod = GetCurrentModule();
		const HRSRC hRes = FindResourceEx(hMod, MAKEINTRESOURCE(ID_TRANSLATION), MAKEINTRESOURCE(IDR_TRANS_EN), MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL));
		if (NULL != hRes)
		{
			HGLOBAL hData = LoadResource(hMod, hRes);
			if (NULL != hData)
			{
				const DWORD dataSize = SizeofResource(hMod, hRes);
				char *resource = new char[dataSize + 1];
				memcpy(resource, LockResource(hData), dataSize);
				resource[dataSize] = 0;
				FreeResource(hData);

				try
				{
					json jsonResource = json::parse(resource);

					for (json::iterator it = jsonResource.begin(); it != jsonResource.end(); ++it)
					{
						wxString val = it.value().get<string>();
						__translations.insert(make_pair(it.key(), val));
					}
				}
				catch (json::parse_error p)
				{
					OutputDebugStringA(p.what());
				}
			}
		}
	}

	if (sub.length() > 0)
	{
		key += "._" + sub;
	}

	return __translations.find(key) != __translations.end() ? __translations.at(key) : key;
}