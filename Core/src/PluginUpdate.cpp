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
#include "PluginUpdate.h"
#include "WinHttpClient/WinHttpClient.h"

bool CheckForUpdate(const Version version, PluginUpdate* pluginUpdate)
{
	// Fallback values
	pluginUpdate->version = version;
	pluginUpdate->isUpdateAvailable = false;

	// Build url
	wxString url = wxString::Format(VKDR_UPDATE_CHECK_URL,
		version.number.major,
		version.number.minor,
		version.number.patch);

	WinHttpClient client(url.ToStdWstring());
	client.SetUserAgent(wxString::Format("voukoder/%d.%d.%d", version.number.major, version.number.minor, version.number.patch).ToStdWstring());
	client.SetTimeouts(2000U, 2000U, 2000U, 2000U);
	if (client.SendHttpRequest())
	{
		wxString content = client.GetResponseContent();

		try
		{
			const json jsonRes = json::parse(content.ToStdString());

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

	return pluginUpdate->isUpdateAvailable;
}