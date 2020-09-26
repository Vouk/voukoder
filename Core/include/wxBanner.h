#pragma once

#include <wx/wx.h>

#include "BannerInfo.h"

class wxBanner : public wxStaticBitmap
{
public:
	wxBanner(wxWindow* parent, wxWindowID id, const wxGDIImage& label, BannerInfo info);
	~wxBanner();

private:
	BannerInfo info;
	void OnClick(wxMouseEvent& event);
	void OnEnter(wxMouseEvent& event);
	void OnLeave(wxMouseEvent& event);
};
