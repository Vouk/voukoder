#pragma once

#include <wx/wx.h>
#include "json.hpp"
#include "LanguageInfo.h"

#define Trans LanguageUtils::Translate

using namespace nlohmann;

class LanguageUtils
{
public:
	static bool Create(LanguageInfo &languageInfo, const json json);
	static LANGID GetLanguageId(std::vector<LanguageInfo> languageInfos);
	static void InitTranslation(std::vector<LanguageInfo> languageInfos);
	static void StoreLanguageId(LANGID langId);
	static const wxString Translate(wxString key, const wxString sub = "");

private:
	static bool IsLanguageAvailable(std::vector<LanguageInfo> languageInfos, LANGID langId);
};
