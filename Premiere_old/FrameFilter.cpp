#include "FrameFilter.h"

FrameFilter::FrameFilter()
{
	filterGraph = avfilter_graph_alloc();
}

FrameFilter::~FrameFilter()
{
	avfilter_graph_free(&filterGraph);
}

int FrameFilter::configure(FrameFilterOptions options, const char *filters)
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
		av_get_channel_layout_string(ch_layout, sizeof(ch_layout), NULL, options.channel_layout);
		av_opt_set(in_ctx, "channel_layout", ch_layout, AV_OPT_SEARCH_CHILDREN);
		av_opt_set(in_ctx, "sample_fmt", av_get_sample_fmt_name(options.sample_fmt), AV_OPT_SEARCH_CHILDREN);
		av_opt_set_int(in_ctx, "channels", av_get_channel_layout_nb_channels(options.channel_layout), AV_OPT_SEARCH_CHILDREN);
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
	err = avfilter_graph_parse(filterGraph, filters, inputs, outputs, NULL);
	err = avfilter_graph_config(filterGraph, NULL);

	// Free inputs & outputs
	//avfilter_inout_free(&inputs);
	//avfilter_inout_free(&outputs);

	return err;
}

int FrameFilter::sendFrame(AVFrame *frame)
{
	return av_buffersrc_add_frame(in_ctx, frame);// , AV_BUFFERSRC_FLAG_KEEP_REF);
}

int FrameFilter::receiveFrame(AVFrame *frame)
{
	return av_buffersink_get_frame(out_ctx, frame);
}
