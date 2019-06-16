#pragma once

#include <wx/wx.h>
#include <map>

#ifndef _WIN32
typedef unsigned short LANGID;
#endif

struct LanguageInfo
{
	wxString id;
	wxString name;
	LANGID langId;
    std::map<wxString, wxString> translations;
};
