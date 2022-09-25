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
#include "MuxerUtils.h"
#include "wx/wx.h"
#include "Log.h"

bool MuxerUtils::Create(MuxerInfo &muxerInfo, const json resource)
{
	wxString muxerId = resource["id"].get<std::string>();

	// Is this encoder supported?
	if (!IsAvailable(muxerId.BeforeFirst('#')))
		return false;

	vkLogInfoVA("Loading: muxers/%s.json", muxerId.c_str());

	// Parse encoder info
	muxerInfo.id = muxerId;
	muxerInfo.extension = resource["extension"].get<std::string>();
	muxerInfo.name = resource["name"].get<std::string>() + " (." + muxerInfo.extension + ")";
	muxerInfo.videoCodecIds = resource["encoders"]["video"].get<std::vector<std::string>>();
	muxerInfo.audioCodecIds = resource["encoders"]["audio"].get<std::vector<std::string>>();
	muxerInfo.capabilities.faststart = resource["capabilities"]["faststart"].get<bool>();

	return true;
}

bool MuxerUtils::IsAvailable(const wxString id)
{
	return av_guess_format(id.c_str(), NULL, NULL) != NULL;
}
