#include "wxAboutPanel.h"
#include "Images.h"

wxBEGIN_EVENT_TABLE(wxAboutPanel, wxPanel)
EVT_PAINT(wxAboutPanel::paintEvent)
wxEND_EVENT_TABLE()

void wxAboutPanel::paintEvent(wxPaintEvent& evt)
{
	wxPaintDC dc(this);
	render(dc);
}

void wxAboutPanel::paintNow()
{
	wxClientDC dc(this);
	render(dc);
}

void wxAboutPanel::render(wxDC& dc)
{
	wxCoord a = (GetSize().GetWidth() - image.GetWidth()) / 2;
	dc.DrawBitmap(image, a, 0, false);
}