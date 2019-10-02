#pragma once

#include <wx/wx.h>
#include <wx/filename.h>

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
	wxFile file;
	wxString filename;
	Log();
	~Log();
	wxString GetBaseDir();
	wxString CreateFileName();
	void Clean(int days = 7);

public:
	static Log* instance();
	void AddSep();
	void AddLine(wxString line);
	wxString GetFilename();
};