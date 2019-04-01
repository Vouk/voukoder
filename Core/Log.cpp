#include "Log.h"

Log* Log::instance()
{
	static Log instance;

	return &instance;
}

Log::Log()
{}

void Log::AddLine(wxString line)
{
	_log.Append("[" + wxDateTime::Now().FormatISOCombined() + "] " + line + "\n");

	if (_log.size() > LOG_BUFFER_SIZE)
	{
		_log.erase(0, _log.Find('\n') + 1);
	}
}

void Log::Clear()
{
	_log.Clear();
}

wxString Log::GetAsString()
{
	return _log;
}