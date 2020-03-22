/**
 * Voukoder
 * Copyright (C) 2017-2020 Daniel Stankewitz, All Rights Reserved
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
#pragma once

#include <wx/wx.h>
#include <wx/url.h>
#include <wx/sstream.h>
#include <wx/protocol/http.h>
#include "json.hpp"

using namespace nlohmann;

#define VKDR_UPDATE_CHECK_HOST "version.voukoder.org"
#define VKDR_UPDATE_CHECK_URL "/check.php?host=premiere&ver=%d.%d.%d"

union Version
{
	struct number
	{
		uint8_t build;
		uint8_t patch;
		uint8_t minor;
		uint8_t major;
	} number;
	uint32_t code = 0;
};

struct PluginUpdate
{
	Version version;
	wxString url;
	wxString headline;
	wxString message;
	bool isUpdateAvailable = false;
};

static bool CheckForUpdate(const Version version, PluginUpdate* pluginUpdate)
{
	// Fallback values
	pluginUpdate->version = version;
	pluginUpdate->isUpdateAvailable = false;

	// Build url
	wxString url = wxString::Format(VKDR_UPDATE_CHECK_URL,
		version.number.major,
		version.number.minor,
		version.number.patch);

	wxHTTP get;
	get.SetHeader(_T("Content-type"), _T("text/html; charset=utf-8"));
	get.SetHeader(_T("User-Agent"), wxString::Format(_T("voukoder/%d.%d.%d"), version.number.major, version.number.minor, version.number.patch));
	get.SetTimeout(2);

	if (get.Connect(VKDR_UPDATE_CHECK_HOST))
	{
		wxApp::IsMainLoopRunning();

		wxInputStream* httpStream = get.GetInputStream(url);

		if (get.GetError() == wxPROTO_NOERR)
		{
			wxString res;
			wxStringOutputStream out_stream(&res);
			httpStream->Read(out_stream);

			try
			{
				const json jsonRes = json::parse(res.ToStdString());

				pluginUpdate->version.number.major = jsonRes["version"]["major"].get<uint8_t>();
				pluginUpdate->version.number.minor = jsonRes["version"]["minor"].get<uint8_t>();
				pluginUpdate->version.number.patch = jsonRes["version"]["patch"].get<uint8_t>();
				pluginUpdate->version.number.build = 0;
				pluginUpdate->isUpdateAvailable = pluginUpdate->version.code > version.code;
                pluginUpdate->url = jsonRes["url"].get<std::string>();
                pluginUpdate->headline = jsonRes["headline"].get<std::string>();
                pluginUpdate->message = jsonRes["message"].get<std::string>();
			}
			catch (json::parse_error p)
			{
			}
		}

		wxDELETE(httpStream);
		get.Close();
	}

	return pluginUpdate->isUpdateAvailable;
}
