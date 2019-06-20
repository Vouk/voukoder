#pragma once

#include <wx/wx.h>

class wxAboutPanel : public wxPanel
{
public:
	wxAboutPanel(wxWindow* parent, wxBitmap image) : wxPanel(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL), image(image) {}
	void paintEvent(wxPaintEvent& evt);
	void paintNow();
	void render(wxDC& dc);

private:
	wxBitmap image;

	DECLARE_EVENT_TABLE()
};