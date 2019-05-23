#include "AudioRenderer.h"
#include <Log.h>

AudioRenderer::AudioRenderer(const csSDK_uint32 pluginId, const PrTime startTime, const PrTime endTime, csSDK_uint32 chunkSize, ExportInfo encoderInfo, PrSuites *suites):
	pluginId(pluginId),
	chunkSize(chunkSize),
	channelLayout(encoderInfo.audio.channelLayout),
	sampleRate(encoderInfo.audio.timebase.den),
	numChannels(av_get_channel_layout_nb_channels(encoderInfo.audio.channelLayout)),
	suites(suites)
{
	// Calculate total sample count
	PrTime ticksPerSample;
	suites->timeSuite->GetTicksPerAudioSample(encoderInfo.audio.timebase.den, &ticksPerSample);
	samplesTotal = 1 + ((endTime - startTime - 1) / ticksPerSample);

	// Create high precision audio renderer
	suites->sequenceAudioSuite->MakeAudioRenderer(pluginId, startTime, GetChannelType(), kPrAudioSampleType_32BitFloat, encoderInfo.audio.timebase.den, &rendererId);
	
	// Get max. chunk size
	suites->sequenceAudioSuite->GetMaxBlip(rendererId, encoderInfo.video.ticksPerFrame, &maxChunkSize);

	// Reserve audio buffers for each channel
	for (int i = 0; i < numChannels; i++)
		buffer[i] = (float*)av_malloc(chunkSize * sizeof(float));
}

AudioRenderer::~AudioRenderer()
{	
	// Free audio buffers
	for (int i = 0; i < numChannels; i++)
		av_free(buffer[i]);

	// Release renderer
	suites->sequenceAudioSuite->ReleaseAudioRenderer(pluginId, rendererId);
}

void AudioRenderer::Reset()
{
	suites->sequenceAudioSuite->ResetAudioToBeginning(rendererId);
	pts = 0;
}

int AudioRenderer::GetNextFrame(AVFrame &frame)
{
	// Create frame buffer
	if (frame.data[0] == NULL)
	{
		// Allocate frame
		frame.format = AV_SAMPLE_FMT_FLTP;
		frame.channel_layout = channelLayout;
		frame.sample_rate = sampleRate;
		frame.nb_samples = chunkSize;

		// Reserve buffer
		if (av_frame_get_buffer(&frame, av_cpu_max_align()) < 0)
		{
			vkLogError("Can not reserve audio buffer!");
			return malUnknownError;
		}
	}

	// Check chunk size
	frame.nb_samples = FFMIN(chunkSize, samplesTotal - pts);
	frame.pts = pts;

	pts += frame.nb_samples;

	csSDK_int32 pos = 0;

	// Fill sample buffer to requested size
	while (pos < frame.nb_samples)
	{
		// Set chunk size
		csSDK_uint32 chunk = FFMIN(frame.nb_samples - pos, maxChunkSize);

		// Get samples from premiere
		if (pos == 0)
		{	
			// Fill buffer initially
			suites->sequenceAudioSuite->GetAudio(rendererId, chunk, (float**)frame.data, kPrFalse);
		}
		else
		{
			// Get next sample data chunk
			suites->sequenceAudioSuite->GetAudio(rendererId, chunk, buffer, kPrFalse);

			// Fill buffer with first batch of audio samples
			for (csSDK_int32 c = 0; c < numChannels; c++)
				memcpy(((float**)frame.data)[c] + pos, buffer[c], chunk * sizeof(float));
		}

		pos += chunk;
	}

	return frame.nb_samples;
}

PrTime AudioRenderer::GetPts()
{
	return pts;
}

PrAudioChannelType AudioRenderer::GetChannelType()
{
	if (channelLayout == AV_CH_LAYOUT_MONO)
	{
		return kPrAudioChannelType_Mono;
	}
	else if (channelLayout == AV_CH_LAYOUT_5POINT1_BACK)
	{
		return kPrAudioChannelType_51;
	}

	return kPrAudioChannelType_Stereo;
}