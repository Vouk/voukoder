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
#include "lavf.h"
#include "OptionContainer.h"
#include "FilterConfig.h"
#include "json.hpp"

using namespace nlohmann;

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
};