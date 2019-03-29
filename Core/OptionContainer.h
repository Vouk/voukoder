#pragma once

#include <map>
#include <wx/wx.h>

using namespace std;

class OptionContainer : public map<string, string>
{
public:
	void fromString(const wxString input, const char parameterSeparator = ',', const char valueSeparator = '=');
	const wxString toString(const bool includePrivate = false, wxString prefix = "", char parameterSeparator = ',', char valueSeparator = '=');
};