#pragma once

#include <wx/wx.h>
#include <map>
#include <string>

#include "json.hpp"
#include "wxBanner.h"

#define VKDR_BANNER_HOST "version.voukoder.org"
#define VKDR_BANNER_URL "/banners.php"

class BannerService
{
public:
	bool LoadConfig();
	wxBanner* CreateBanner(const wxString id, wxWindow* parent);

private:
	std::map<wxString, BannerInfo> bannerInfos;
	bool GetInfo(const wxString id, BannerInfo& bannerInfo);
	wxImage* LoadImage(wxURL url);
};