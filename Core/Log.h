#pragma once

#include <wx\wx.h>

#define LOG_LINE_BUFFER_SIZE 65535

class Log
{
private:
	wxArrayString _log;
	Log();

public:
	static Log* instance();
	void AddLine(wxString line);
	wxString GetAsString();
};