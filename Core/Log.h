#pragma once

#include <wx\wx.h>

#define vkLogInfo(msg, ...) Log::instance()->AddLine(wxString::Format(msg, __VA_ARGS__));
#define vkLogWarn(msg, ...) Log::instance()->AddLine(wxString::Format(msg, __VA_ARGS__));
#define vkLogError(msg, ...) Log::instance()->AddLine(wxString::Format(msg, __VA_ARGS__));

class Log
{
private:
	wxString _log;
	Log();

public:
	static Log* instance();
	void AddLine(wxString line);
	void Clear();
	wxString GetAsString();
};