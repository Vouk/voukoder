/**
 * Voukoder
 * Copyright (C) 2017-2022 Daniel Stankewitz, All Rights Reserved
 * https://www.voukoder.org
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 * http://www.gnu.org/copyleft/gpl.html
 */
#include <wx/wx.h>
#include <wx/mstream.h>

#include "BannerService.h"
#include "WinHttpClient/WinHttpClient.h"

wxBanner* BannerService::CreateBanner(const wxString id, wxWindow* parent)
{
	BannerInfo info;
	if (GetInfo(id, info))
	{
		wxImage* img = LoadImageW(wxString(info.imageUrl));
		if (img)
			return new wxBanner(parent, wxID_ANY, wxBitmap(*img), info);
	}

	return nullptr;
}

bool BannerService::LoadConfig()
{
	wxString url(VKDR_BANNER_URL);

	WinHttpClient client(url.ToStdWstring());
	client.SetTimeouts(2000U, 2000U, 2000U, 2000U);
	if (client.SendHttpRequest())
	{
		wxString content = client.GetResponseContent();

		nlohmann::json data;

		try
		{
			data = nlohmann::json::parse(content.ToStdString());
		}
		catch (nlohmann::json::parse_error p)
		{
			return false;
		}

		bannerInfos.clear();

		for (auto& item : data.items())
		{
			nlohmann::json entry = item.value();
			BannerInfo info = entry.get<BannerInfo>();

			if (info.enabled)
				bannerInfos.insert(make_pair(item.key(), info));
		}
	}

	return true;
}

bool BannerService::GetInfo(const wxString id, BannerInfo& info)
{
	if (bannerInfos.find(id) == bannerInfos.end())
		return false;

	info = bannerInfos.at(id);
	return true;
}

wxImage* BannerService::LoadImage(const wxString url)
{
	wxImage* pImage = nullptr;

	WinHttpClient client(url.ToStdWstring());
	client.SetTimeouts(0U, 2000U, 2000U, 2000U);
	if (client.SendHttpRequest())
	{
		wxMemoryInputStream stream(client.GetRawResponseContent(), client.GetRawResponseContentLength());

		pImage = new wxImage();
		pImage->LoadFile(stream, wxBITMAP_TYPE_ANY);

		if (!pImage->IsOk())
			wxDELETE(pImage);
	}

	return pImage;
}
