#include <wx/wx.h>
#include <wx/url.h>
#include <wx/sstream.h>
#include <wx/protocol/http.h>
#include <wx/mstream.h>

#include "BannerService.h"

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
	wxHTTP get;
	get.SetHeader("Content-type", "application/json; charset=utf-8");
	get.SetTimeout(2);

	if (get.Connect(VKDR_BANNER_HOST))
	{
		wxApp::IsMainLoopRunning();

		wxInputStream* httpStream = get.GetInputStream(VKDR_BANNER_URL);

		if (get.GetError() == wxPROTO_NOERR)
		{
			wxString res;
			wxStringOutputStream out_stream(&res);
			httpStream->Read(out_stream);
			
			wxDELETE(httpStream);
			get.Close();

			nlohmann::json data;

			try
			{
				data = nlohmann::json::parse(res.ToStdString());
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

wxImage* BannerService::LoadImage(wxURL url)
{
	wxImage* pImage = nullptr;

	wxHTTP get;
	get.SetTimeout(5);

	if (get.Connect(url.GetServer()))
	{
		wxApp::IsMainLoopRunning();

		wxInputStream* httpStream = get.GetInputStream(url.GetPath());

		if (get.GetError() == wxPROTO_NOERR)
		{
			unsigned char buffer[8192];
			size_t count = -1;
			wxMemoryBuffer mem_buf;

			while (!httpStream->Eof() && count != 0)
			{
				httpStream->Read(buffer, sizeof(buffer));
				count = httpStream->LastRead();
				if (count > 0)
					mem_buf.AppendData(buffer, count);
			}

			wxMemoryInputStream stream(mem_buf.GetData(), mem_buf.GetDataLen());

			pImage = new wxImage();
			pImage->LoadFile(stream, wxBITMAP_TYPE_ANY);
		}
	}

	return pImage;
}
