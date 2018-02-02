#include "Encoder.h"
#include "Common.h"
#include <Windows.h>

// reviewed 0.5.3
Encoder::Encoder(const char *short_name, const char *filename)
{
	this->filename = filename;

	formatContext = avformat_alloc_context();
	formatContext->oformat = av_guess_format(short_name, this->filename, NULL);

	// Set metadata
	av_dict_set(&formatContext->metadata, "encoding_tool", PLUGIN_ENCODER_TOOL, 0);
	
	videoContext = new EncoderContext(formatContext);
	audioContext = new EncoderContext(formatContext);
}

// reviewed 0.5.3
Encoder::~Encoder()
{	
	delete(videoContext);
	delete(audioContext);

	avformat_free_context(formatContext);
}

// reviewed 0.5.3
int Encoder::open()
{
	int ret;

	// Open video stream
	if ((ret = videoContext->openCodec()) != S_OK)
	{
		close(false);

		return ret;
	}

	// Open audio stream
	if ((ret = audioContext->openCodec()) != S_OK)
	{
		close(false);

		return ret;
	}

	// Encoder to file or to memory?
	if (this->filename != NULL)
	{
		// Open the target file
		if ((ret = avio_open(&formatContext->pb, this->filename, AVIO_FLAG_WRITE)) != S_OK)
		{
			close(false);

			return ret;
		}
	}
	else
	{
		// Open a memory stream just to test muxer settings
		if ((ret = avio_open_dyn_buf(&formatContext->pb)) != S_OK)
		{
			close(false);

			return ret;
		}
	}

	AVDictionary *options = NULL;
	//av_dict_set(&options, "movflags", "faststart", 0);

	// Check muxer/codec combination and write header
	ret = avformat_write_header(formatContext, &options);

	if (ret != S_OK)
		close(false);

	return ret;
}

// reviewed 0.3.8
void Encoder::close(bool writeTrailer)
{
	// Write trailer
	if (writeTrailer)
	{
		// Flush the encoders
		writeVideoFrame(NULL);
		writeAudioFrame(NULL, 0);

		av_write_trailer(formatContext);
	}

	videoContext->closeCodec();
	audioContext->closeCodec();

	// Close the file
	if (this->filename != NULL)
	{
		avio_close(formatContext->pb);
	}
	else
	{
		uint8_t *buffer = NULL;
		avio_close_dyn_buf(formatContext->pb, &buffer);
		av_free(buffer);
	}
}

// reviewed 0.3.8
int Encoder::writeVideoFrame(EncodingData *encodingData)
{
	// Do we just want to flush the encoder?
	if (encodingData == NULL)
	{
		// Send the frame to the encoder
		encodeAndWriteFrame(videoContext, NULL, NULL);

		// Destroy frame filters
		for (auto items : videoContext->frameFilters)
		{
			delete(items.second);
		}

		// Clear filter map
		videoContext->frameFilters.clear();

		return S_OK;
	}

	FrameFilter *frameFilter = NULL;

	// Do we need a frame filter for pixel format conversion?
	if (av_get_pix_fmt(encodingData->pix_fmt) != videoContext->codecContext->pix_fmt)
	{
		// Set up frame filter
		if (videoContext->frameFilters.find(encodingData->pix_fmt) == videoContext->frameFilters.end())
		{
			// Configure source format
			FrameFilterOptions options;
			options.media_type = videoContext->codec->type;
			options.width = videoContext->codecContext->width;
			options.height = videoContext->codecContext->height;
			options.pix_fmt = av_get_pix_fmt(encodingData->pix_fmt);
			options.time_base = videoContext->codecContext->time_base;
			options.sar.den = videoContext->codecContext->sample_aspect_ratio.den;
			options.sar.num = videoContext->codecContext->sample_aspect_ratio.num;

			// Set target format
			char filterConfig[256];
			sprintf_s(filterConfig, "format=pix_fmts=%s", videoContext->encoderConfig->getPixelFormat().c_str());

			frameFilter = new FrameFilter();
			frameFilter->configure(options, filterConfig);

			videoContext->frameFilters.insert(pair<string, FrameFilter*>(encodingData->pix_fmt, frameFilter));
		}
		else
		{
			frameFilter = videoContext->frameFilters.at(encodingData->pix_fmt);
		}
	}

	// Create a new frame
	AVFrame *frame = av_frame_alloc();
	frame->width = videoContext->codecContext->width;
	frame->height = videoContext->codecContext->height;
	frame->format = av_get_pix_fmt(encodingData->pix_fmt);
	
	int ret;

	// Reserve buffer space
	if ((ret = av_frame_get_buffer(frame, 0)) < 0)
	{
		return ret;
	}

	// Fill the source frame
	for (int i = 0; i < encodingData->planes; i++)
	{
		frame->data[i] = (uint8_t*)encodingData->plane[i];
		frame->linesize[i] = encodingData->stride[i];
	}

	// Presentation timestamp
	frame->pts = videoContext->next_pts++;
	
	// Send the frame to the encoder
	if ((ret = encodeAndWriteFrame(videoContext, frame, frameFilter)) < 0)
	{
		av_frame_free(&frame);
		return ret;
	}

	av_frame_free(&frame);

	return S_OK;
}

// reviewed 0.5.6
int Encoder::writeAudioFrame(float **data, int32_t sampleCount)
{
	// Just want to finish encoding?
	if (data == NULL)
	{
		// Flush the encoder
		encodeAndWriteFrame(audioContext, NULL, NULL);

		// Destroy frame filters
		for (auto items : audioContext->frameFilters)
		{
			delete(items.second);
		}

		// Clear filter map
		audioContext->frameFilters.clear();

		return S_OK;
	}
	
	FrameFilter *frameFilter;

	// Set up frame filter
	if (audioContext->frameFilters.size() == 0)
	{
		// Configure source format
		FrameFilterOptions options;
		options.media_type = audioContext->codec->type;
		options.channel_layout = audioContext->codecContext->channel_layout;
		options.sample_fmt = AV_SAMPLE_FMT_FLTP;
		options.time_base = { 1, audioContext->codecContext->sample_rate };

		// Map audio layout
		char *audioLayout;
		switch (audioContext->codecContext->channel_layout)
		{
		case AV_CH_LAYOUT_MONO:
			audioLayout = "mono";
			break;
		case AV_CH_LAYOUT_STEREO:
			audioLayout = "stereo";
			break;
		case AV_CH_LAYOUT_5POINT1_BACK:
			audioLayout = "5.1";
			break;
		default:
			audioLayout = "stereo";
		}

		// Find the right sample size (for PCM we take what we get from the host)
		int frame_size = audioContext->codecContext->frame_size;
		if (frame_size == 0)
		{
			frame_size = sampleCount;
		}

		// Set target format
		char filterConfig[256];
		sprintf_s(filterConfig, 
			"aformat=channel_layouts=%s:sample_fmts=%s:sample_rates=%d,asetnsamples=n=%d:p=0", 
			audioLayout,
			av_get_sample_fmt_name(audioContext->codecContext->sample_fmt),
			audioContext->codecContext->sample_rate,
			frame_size);
		
		frameFilter = new FrameFilter();
		frameFilter->configure(options, filterConfig);

		audioContext->frameFilters.insert(pair<string, FrameFilter*>("default", frameFilter));
	}
	else
	{
		// Take first frame filter found
		frameFilter = audioContext->frameFilters.begin()->second;
	}

	// Create the source frame
	AVFrame *frame;
	frame = av_frame_alloc();
	frame->nb_samples = sampleCount;
	frame->channel_layout = audioContext->codecContext->channel_layout;
	frame->format = AV_SAMPLE_FMT_FLTP;
	frame->sample_rate = audioContext->codecContext->sample_rate;
	frame->pts = audioContext->next_pts++;

	int ret;

	// Allocate the buffer for the frame
	if ((ret = av_frame_get_buffer(frame, 0)) == 0)
	{
		// Assign the audio plane pointer
		for (int c = 0; c < audioContext->codecContext->channels; c++)
		{
			frame->data[c] = (uint8_t*)data[c];
		}

		// Send the frame to the encoder
		ret = encodeAndWriteFrame(audioContext, frame, frameFilter);
	}

	av_frame_free(&frame);

	return ret;
}

// reviewed 0.3.8
int Encoder::encodeAndWriteFrame(EncoderContext *context, AVFrame *frame, FrameFilter *frameFilter)
{
	int ret = S_OK;

	// In case we want to flush the encoder
	if (frame == NULL)
	{
		// Send NULL frame
		if ((ret = avcodec_send_frame(context->codecContext, frame)) != S_OK)
		{
			return ret;
		}

		// Flush receiver
		goto receive_packets;
	}

	if (frameFilter != NULL)
	{
		// Send the uncompressed frame to frame filter
		if ((ret = frameFilter->sendFrame(frame)) != S_OK)
		{
			return ret;
		}

		AVFrame *tmp_frame;
		tmp_frame = av_frame_alloc();

		// Receive frames from the rame filter
		while (frameFilter->receiveFrame(tmp_frame) >= 0)
		{
			// Send the frame to the encoder
			if ((ret = avcodec_send_frame(context->codecContext, tmp_frame)) != S_OK)
			{
				av_frame_free(&tmp_frame);
				return ret;
			}

			av_frame_unref(tmp_frame);
		}

		av_frame_free(&tmp_frame);
	}
	else
	{
		// Send the frame directly to the encoder
		if ((ret = avcodec_send_frame(context->codecContext, frame)) != S_OK)
		{
			return ret;
		}
	}

receive_packets:	

	AVPacket *packet = av_packet_alloc();

	// If we receive a packet from the encoder write it to the stream
	while (avcodec_receive_packet(context->codecContext, packet) >= 0)
	{
		av_packet_rescale_ts(packet, context->codecContext->time_base, context->stream->time_base);
		packet->stream_index = context->stream->index;
		
		if ((ret = av_interleaved_write_frame(formatContext, packet)) < 0)
			break;

		av_packet_unref(packet);
	}

	return ret;
}

// reviewed 0.3.8
FrameType Encoder::getNextFrameType()
{
	if (av_compare_ts(videoContext->next_pts, videoContext->codecContext->time_base, audioContext->next_pts, audioContext->codecContext->time_base) <= 0)
	{
		return FrameType::VideoFrame;
	}

	return FrameType::AudioFrame;
}