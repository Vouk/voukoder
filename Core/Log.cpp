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
	wxDateTime dt = wxDateTime::Now();

	// TOO SLOW!!! Need to find better solution
	_log.Add("[" + dt.FormatISOCombined() + "] " + line);
	if (_log.size() > LOG_LINE_BUFFER_SIZE)
	{
		_log.RemoveAt(0);
	}
}

wxString Log::GetAsString()
{
	wxString log;
	for (int i = 0; i < _log.GetCount(); i++)
		log.Append(_log.Item(i)).Append("\n");

	return log;
}