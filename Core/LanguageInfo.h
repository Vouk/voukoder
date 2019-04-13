#pragma once

#include <wx/wx.h>
#include <map>

using namespace std;

struct LanguageInfo
{
	wxString id;
	wxString name;
	LANGID langId;
	map<wxString, wxString> translations;
};