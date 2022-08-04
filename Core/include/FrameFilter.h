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

#include "lavf.h"

struct FrameFilterOptions
{
	int width = 0;
	int height = 0;
	AVMediaType media_type = AVMEDIA_TYPE_UNKNOWN;
	AVRational time_base = { 0, 0 };
	AVChannelLayout ch_layout;
	AVSampleFormat sample_fmt = AV_SAMPLE_FMT_NONE;
	AVPixelFormat pix_fmt = AV_PIX_FMT_NONE;
	AVRational sar = { 1, 1 };
};

class FrameFilter
{
public:
	FrameFilter();
	~FrameFilter();
	int configure(AVCodecContext* context, FrameFilterOptions options, const char *filters);
	int sendFrame(AVFrame *frame);
	int receiveFrame(AVFrame *frame);
private:
	AVFilterGraph *filterGraph = NULL;
	AVFilterContext *in_ctx = NULL;
	AVFilterContext *out_ctx = NULL;
};