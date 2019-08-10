#include "LanguageUtils.h"
#include "Voukoder.h"
#include "Log.h"
#include <wx/msw/registry.h>

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

#ifdef _WIN32

	// If users UI language is supported use this as fallback
	if (IsLanguageAvailable(languageInfos, GetUserDefaultUILanguage()))
	{
		langId = GetUserDefaultUILanguage();
	}

	// Does the user have stored a selection in the registry?
	wxRegKey key(wxRegKey::HKCU, VKDR_REG_ROOT);
	if (key.Exists() && key.HasValue(VKDR_REG_LANGUAGE))
	{
		long val;
		key.QueryValue(VKDR_REG_LANGUAGE, &val);

		if (IsLanguageAvailable(languageInfos, val))
		{
			langId = val;
		}

	}

#endif

	return langId;
}

void LanguageUtils::InitTranslation(std::vector<LanguageInfo> languageInfos)
{
	// Fallback to english in worst case
	LANGID langId = GetLanguageId(languageInfos);
	for (auto& languageInfo : languageInfos)
	{
		if (languageInfo.langId == langId)
		{
			__translations.insert(languageInfo.translations.begin(), languageInfo.translations.end());
		}
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
#ifdef _WIN32

	wxRegKey key(wxRegKey::HKCU, VKDR_REG_ROOT);
	if (!key.Exists())
	{
		key.Create();
	}

	key.SetValue(VKDR_REG_LANGUAGE, (long)langId);

#endif
}

const wxString LanguageUtils::Translate(wxString key, const wxString sub)
{
	if (sub.length() > 0)
		key += "._" + sub;

	wxString val = __translations.find(key) != __translations.end() ? __translations.at(key) : key;
	val.Replace("*", "");

	return val;
}