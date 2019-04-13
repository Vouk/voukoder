#pragma once

#include <lavf.h>
#include <ExportInfo.h>
#include "Suites.h"

#define MAX_CHANNELS 6

class AudioRenderer
{
public:
	AudioRenderer(const csSDK_uint32 pluginId, const PrTime startTime, const PrTime endTime, csSDK_uint32 chunkSize, ExportInfo exportInfo, PrSuites *suites);
	~AudioRenderer();
	int GetNextFrame(AVFrame &frame);
	void Reset();
	PrTime GetPts();

private:
	csSDK_uint32 pluginId;
	csSDK_uint32 rendererId;
	csSDK_int32 numChannels;
	csSDK_uint32 chunkSize;
	csSDK_int32 maxChunkSize;
	csSDK_int32 sampleRate;
	csSDK_int64 samplesTotal;
	csSDK_int64 pts = 0;
	float *buffer[MAX_CHANNELS];
	uint64_t channelLayout;
	PrSuites *suites;
	PrAudioChannelType GetChannelType();
};