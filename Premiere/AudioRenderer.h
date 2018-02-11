#pragma once

#include "premiere_cs6\PrSDKExportParamSuite.h"
#include "premiere_cs6\PrSDKSequenceAudioSuite.h"
#include "premiere_cs6\PrSDKMemoryManagerSuite.h"

#define MAX_CHANNELS 6

class AudioRenderer
{
public:
	AudioRenderer(csSDK_uint32 pluginId, PrTime startTime, PrTime endTime, PrAudioChannelType channelType, float audioSampleRate, PrTime ticksPerFrame,
		PrSDKSequenceAudioSuite *sequenceAudioSuite, PrSDKTimeSuite *timeSuite, PrSDKMemoryManagerSuite *memorySuite);
	~AudioRenderer();
	float** getSamples(csSDK_uint32 *size, prBool clip);
	csSDK_int32 getMaxBlip();
	prBool samplesInBuffer();

private:
	csSDK_uint32 pluginId;
	csSDK_uint32 rendererId;
	csSDK_int32 maxBlip;
	PrTime samplesRemaining;
	float *buffer[MAX_CHANNELS];
	PrSDKSequenceAudioSuite *sequenceAudioSuite;
	PrSDKMemoryManagerSuite *memorySuite;
};