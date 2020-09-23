#include "wxBanner.h"

wxBanner::wxBanner(wxWindow* parent, wxWindowID id, const wxGDIImage& label, BannerInfo info):
	wxStaticBitmap(parent, id, label, wxDefaultPosition, wxDefaultSize, 0L),
	info(info)
{
	Bind(wxEVT_LEFT_UP, &wxBanner::OnClick, this);
	Bind(wxEVT_ENTER_WINDOW, &wxBanner::OnEnter, this);
	Bind(wxEVT_LEAVE_WINDOW, &wxBanner::OnLeave, this);
}

wxBanner::~wxBanner()
{}

void wxBanner::OnClick(wxMouseEvent& event)
{
	wxLaunchDefaultBrowser(info.actionUrl, wxBROWSER_NEW_WINDOW);
}

void wxBanner::OnEnter(wxMouseEvent& event)
{
	SetCursor(wxCURSOR_HAND);
}

void wxBanner::OnLeave(wxMouseEvent& event)
{
	SetCursor(wxCURSOR_DEFAULT);
}