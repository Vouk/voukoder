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
#include "FrameFilter.h"
#include "Log.h"

FrameFilter::FrameFilter()
{
	filterGraph = avfilter_graph_alloc();
}

FrameFilter::~FrameFilter()
{
	avfilter_graph_free(&filterGraph);
}

int FrameFilter::configure(AVCodecContext* context, FrameFilterOptions options, const char *filters)
{
	int err;
	const AVFilter *in, *out;

	// Create the right input & output filters
	switch (options.media_type)
	{
	case AVMEDIA_TYPE_AUDIO:
		in = avfilter_get_by_name("abuffer");
		out = avfilter_get_by_name("abuffersink");
		break;

	case AVMEDIA_TYPE_VIDEO:
		in = avfilter_get_by_name("buffer");
		out = avfilter_get_by_name("buffersink");
		break;
	default:
		return -1;
	}

	// Allocate input & output contexts
	in_ctx = avfilter_graph_alloc_filter(filterGraph, in, "in");
	out_ctx = avfilter_graph_alloc_filter(filterGraph, out, "out");

	// Set the source options
	switch (options.media_type)
	{
	case AVMEDIA_TYPE_AUDIO:
		char ch_layout[64];
		av_channel_layout_describe(&options.channel_layout, ch_layout, sizeof(ch_layout));
		av_opt_set(in_ctx, "channel_layout", ch_layout, AV_OPT_SEARCH_CHILDREN);
		av_opt_set(in_ctx, "sample_fmt", av_get_sample_fmt_name(options.sample_fmt), AV_OPT_SEARCH_CHILDREN);
		av_opt_set_int(in_ctx, "channels", options.channel_layout.nb_channels, AV_OPT_SEARCH_CHILDREN);
		av_opt_set(in_ctx, "sample_fmt", av_get_sample_fmt_name(options.sample_fmt), AV_OPT_SEARCH_CHILDREN);
		av_opt_set_q(in_ctx, "time_base", options.time_base, AV_OPT_SEARCH_CHILDREN);
		av_opt_set_int(in_ctx, "sample_rate", options.time_base.den, AV_OPT_SEARCH_CHILDREN);
		break;

	case AVMEDIA_TYPE_VIDEO:
		av_opt_set_int(in_ctx, "width", options.width, AV_OPT_SEARCH_CHILDREN);
		av_opt_set_int(in_ctx, "height", options.height, AV_OPT_SEARCH_CHILDREN);
		av_opt_set(in_ctx, "pix_fmt", av_get_pix_fmt_name(options.pix_fmt), AV_OPT_SEARCH_CHILDREN);
		av_opt_set_q(in_ctx, "time_base", options.time_base, AV_OPT_SEARCH_CHILDREN);
		av_opt_set_q(in_ctx, "sar", options.sar, AV_OPT_SEARCH_CHILDREN);
		break;
	default:
		break;
	}

	// Initialize filters
	err = avfilter_init_str(in_ctx, NULL);
	err = avfilter_init_str(out_ctx, NULL);

	// Allocate input & output endpoints
	AVFilterInOut *outputs = avfilter_inout_alloc();
	AVFilterInOut *inputs = avfilter_inout_alloc();

	// Configure input & output endpoints
	outputs->name = av_strdup("in");
	outputs->filter_ctx = in_ctx;
	outputs->pad_idx = 0;
	outputs->next = NULL;
	inputs->name = av_strdup("out");
	inputs->filter_ctx = out_ctx;
	inputs->pad_idx = 0;
	inputs->next = NULL;

	// Parse filter chain & configure graph
	if ((err = avfilter_graph_parse(filterGraph, filters, inputs, outputs, NULL)) < 0)
	{
		vkLogErrorVA("Unable to parse filter graph: %s", filters);
		return err;
	}

	if ((err = avfilter_graph_config(filterGraph, NULL)) < 0)
	{
		vkLogErrorVA("Unable to configure filter graph. (Code %d)", err);
		return err;
	}

	// Get the updated options
	//switch (options.media_type)
	//{
	//case AVMEDIA_TYPE_AUDIO:
	//	context->channel_layout = av_buffersink_get_channel_layout(out_ctx);
	//	context->sample_fmt = (AVSampleFormat)av_buffersink_get_format(out_ctx);
	//	context->time_base = av_buffersink_get_time_base(out_ctx);
	//	break;

	//case AVMEDIA_TYPE_VIDEO:
	//	context->width = av_buffersink_get_w(out_ctx);
	//	context->height = av_buffersink_get_h(out_ctx);
	//	context->pix_fmt = (AVPixelFormat)av_buffersink_get_format(out_ctx);
	//	context->time_base = av_buffersink_get_time_base(out_ctx);
	//	context->sample_aspect_ratio = av_buffersink_get_sample_aspect_ratio(out_ctx);
	//	break;
	//default:
	//	break;
	//}

	// Free inputs & outputs
	//avfilter_inout_free(&inputs);
	//avfilter_inout_free(&outputs);

	return err;
}

int FrameFilter::sendFrame(AVFrame *frame)
{
	return  av_buffersrc_write_frame(in_ctx, frame);
}

int FrameFilter::receiveFrame(AVFrame *frame)
{
	return av_buffersink_get_frame(out_ctx, frame);
}
