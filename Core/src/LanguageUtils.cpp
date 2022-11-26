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
#include "LanguageUtils.h"
#include "RegistryUtils.h"
#include "Log.h"

static std::map<wxString, wxString> __translations;

bool LanguageUtils::Create(LanguageInfo &languageInfo, const json resource)
{
	languageInfo.id = resource["id"].get<std::string>();
	languageInfo.name = wxString::FromUTF8(resource["name"].get<std::string>());
	languageInfo.langId = resource["langId"].get<LANGID>();

	vkLogInfoVA("Loading: translations/%s.json", languageInfo.id.c_str());

	for (auto &item : resource["translations"].items())
	{
		wxString value = wxString::FromUTF8(item.value().get<std::string>());
		languageInfo.translations.insert(make_pair(item.key(), value));
	}

	return true;
}

LANGID LanguageUtils::GetLanguageId(std::vector<LanguageInfo> languageInfos)
{
	// Fallback to english in worst case
	LANGID langId = 1033;

	// If users UI language is supported use this as fallback
	if (IsLanguageAvailable(languageInfos, GetUserDefaultUILanguage()))
		langId = GetUserDefaultUILanguage();

	// Does the user have stored a selection in the registry?
	long val = RegistryUtils::GetValue(VKDR_REG_LANGUAGE, -1L);
	if (IsLanguageAvailable(languageInfos, val))
		langId = val;

	return langId;
}

void LanguageUtils::InitTranslation(std::vector<LanguageInfo> languageInfos)
{
	// Fallback to english in worst case
	LANGID langId = GetLanguageId(languageInfos);
	for (auto& languageInfo : languageInfos)
	{
		if (languageInfo.langId == langId)
			__translations.insert(languageInfo.translations.begin(), languageInfo.translations.end());
	}
}

bool LanguageUtils::IsLanguageAvailable(std::vector<LanguageInfo> languageInfos, LANGID langId)
{
	for (auto& languageInfo : languageInfos)
	{
		if (languageInfo.langId == langId)
			return true;
	}

	return false;
}

void LanguageUtils::StoreLanguageId(LANGID langId)
{
	RegistryUtils::SetValue(VKDR_REG_LANGUAGE, (long)langId);
}

const wxString LanguageUtils::Translate(wxString key, const wxString sub)
{
	if (sub.length() > 0)
		key += "._" + sub;

	wxString val = __translations.find(key) != __translations.end() ? __translations.at(key) : key;

	if (val.StartsWith("*"))
		val = val.AfterFirst('*');

	return val;
}