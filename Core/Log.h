#pragma once

#include <wx/wx.h>

#define vkLogInfo(msg) Log::instance()->AddLine(msg);
#define vkLogWarn(msg) Log::instance()->AddLine(msg);
#define vkLogError(msg) Log::instance()->AddLine(msg);
#define vkLogInfoVA(msg, ...) Log::instance()->AddLine(wxString::Format(msg, __VA_ARGS__));
#define vkLogWarnVA(msg, ...) Log::instance()->AddLine(wxString::Format(msg, __VA_ARGS__));
#define vkLogErrorVA(msg, ...) Log::instance()->AddLine(wxString::Format(msg, __VA_ARGS__));
#define vkLogSep() Log::instance()->AddSep();

class Log
{
private:
	wxString _log;
	Log();

public:
	static Log* instance();
	void AddSep();
	void AddLine(wxString line);
	void Clear();
	wxString GetAsString();
};
