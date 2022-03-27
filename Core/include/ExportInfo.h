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

#include <wx\wx.h>
#include "lavf.h"
#include "OptionContainer.h"
#include "FilterConfig.h"
#include "json.hpp"

enum VKEncVideoFlags {
	VK_FLAG_NONE = 0x0000,
	VK_FLAG_DEINTERLACE_BOBBING = 0x0001
};

struct ExportInfo
{
	wxString filename = wxEmptyString;
	wxString application = wxEmptyString;
	int passes = 0;

	struct Video
	{
		bool enabled = true;
		int width = 0;
		int height = 0;
		wxString id = wxEmptyString;
		OptionContainer options;
		OptionContainer sideData;
		FilterConfig filters;
		AVRational timebase = { 0, 0 };
		AVRational sampleAspectRatio = { 0, 0 };
		AVFieldOrder fieldOrder = AVFieldOrder::AV_FIELD_UNKNOWN;
		AVColorRange colorRange = AVColorRange::AVCOL_RANGE_UNSPECIFIED;
		AVColorSpace colorSpace = AVColorSpace::AVCOL_SPC_UNSPECIFIED;
		AVColorPrimaries colorPrimaries = AVColorPrimaries::AVCOL_PRI_UNSPECIFIED;
		AVColorTransferCharacteristic colorTransferCharacteristics = AVColorTransferCharacteristic::AVCOL_TRC_UNSPECIFIED;
		int64_t ticksPerFrame = 0;
		int flags = VKEncVideoFlags::VK_FLAG_NONE;
	} video;

	struct Audio
	{
		bool enabled = true;
		wxString id = wxEmptyString;
		OptionContainer options;
		OptionContainer sideData;
		FilterConfig filters;
		AVRational timebase = { 0, 0 };
		uint64_t channelLayout = 0;
	} audio;

	struct Format
	{
		wxString id = wxEmptyString;
		bool faststart = false;
	} format;

	wxString exportPreset()
	{
		nlohmann::json video;
		video["id"] = this->video.id;
		video["options"] = this->video.options.Serialize(true);
		video["sidedata"] = this->video.sideData.Serialize(true);
		video["filters"] = this->video.filters.Serialize();

		nlohmann::json audio;
		audio["id"] = this->audio.id;
		audio["options"] = this->audio.options.Serialize(true);
		audio["sidedata"] = this->audio.sideData.Serialize(true);
		audio["filters"] = this->audio.filters.Serialize();

		nlohmann::json format;
		format["id"] = this->format.id;
		format["faststart"] = this->format.faststart;

		nlohmann::json preset;
		preset["name"] = "";
		preset["video"] = video;
		preset["audio"] = audio;
		preset["format"] = format;

		return preset.dump();
	}

	static EncoderInfo FromPreset(wxString input)
	{
		json preset;

		EncoderInfo encoderInfo = {};

		//try
		//{
		//	preset = nlohmann::json::parse(input);
		//}
		//catch (json::parse_error p)
		//{
		//	return encoderInfo;
		//}






		return encoderInfo;
	}
};