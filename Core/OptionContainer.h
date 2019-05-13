#pragma once

#include <map>
#include <wx/wx.h>

#define PARAM_SEPARATOR "|"
#define VALUE_SEPARATOR "="

using namespace std;

class OptionContainer : public map<string, string>
{
public:
	OptionContainer() {};
	OptionContainer(OptionContainer* options);
	void Deserialize(const wxString input, const char parameterSeparator = PARAM_SEPARATOR[0], const char valueSeparator = VALUE_SEPARATOR[0]);
	const wxString Serialize(const bool includePrivate = false, wxString prefix = "", char parameterSeparator = PARAM_SEPARATOR[0], char valueSeparator = VALUE_SEPARATOR[0]);
};