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
	std::vector<wxFile*> files;
	wxString filename;
	Log();
	~Log();
	void Init(wxFile* file);
	wxString GetBaseDir();
	wxString CreateFileName();
	void Clean(int days = 7);

public:
	static Log* instance();
	void AddFile(wxString filename);
	void AddSep();
	void AddLine(wxString line);
	void AddLineToFile(wxFile* file, wxString line);
	wxString GetFilename();
};