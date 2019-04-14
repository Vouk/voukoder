#pragma once

#include <wx/wx.h>

class wxLogStringBuffer : public wxLog
{
public:
	wxLogStringBuffer()
	{
		OutputDebugStringA("hallo");
	}

	~wxLogStringBuffer()
	{
		OutputDebugStringA("qqqq");
	}

	virtual void Flush()
	{
	}

	wxString GetBuffer()
	{
		wxString out;

		for (int i = 0; i < buffer.GetCount(); i++)
			out << buffer.Item(i) << "\n";

		return out;
	}

protected:
	virtual void DoLogRecord(wxLogLevel level, const wxString& msg, const wxLogRecordInfo& info)
	{
		buffer.Add(msg);
	}

private:
	wxArrayString buffer;
};