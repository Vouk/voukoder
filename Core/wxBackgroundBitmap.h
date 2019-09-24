#pragma once

#include <wx/wx.h>

class wxBackgroundBitmap : public wxEvtHandler {
	typedef wxEvtHandler Inherited;

public:
	wxBackgroundBitmap(const wxBitmap& B) : Bitmap(B), wxEvtHandler() { }
	virtual bool        ProcessEvent(wxEvent& Event);

protected:
	wxBitmap            Bitmap;
};