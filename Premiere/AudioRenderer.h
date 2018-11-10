#pragma once

#ifdef BUILD_CS6_COMPATIBLE
#include "premiere_cs6\PrSDKExportParamSuite.h"
#include "premiere_cs6\PrSDKSequenceAudioSuite.h"
#include "premiere_cs6\PrSDKMemoryManagerSuite.h"
#else
#include "premiere_cc2015\PrSDKExportParamSuite.h"
#include "premiere_cc2015\PrSDKSequenceAudioSuite.h"
#include "premiere_cc2015\PrSDKMemoryManagerSuite.h"
#endif

#define MAX_CHANNELS 6

class AudioRenderer
{
public:
	AudioRenderer(csSDK_uint32 pluginId, PrTime startTime, PrTime endTime, PrAudioChannelType channelType, float audioSampleRate, PrTime ticksPerFrame,
		PrSDKSequenceAudioSuite *sequenceAudioSuite, PrSDKTimeSuite *timeSuite, PrSDKMemoryManagerSuite *memorySuite);
	~AudioRenderer();
	float** getSamples(csSDK_uint32 *size, prBool clip);
	void reset();

private:
	csSDK_uint32 pluginId;
	csSDK_uint32 rendererId;
	csSDK_int32 maxBlip;
	PrTime samplesRemaining;
	PrTime samplesTotal;
	float *buffer[MAX_CHANNELS];
	PrSDKSequenceAudioSuite *sequenceAudioSuite;
	PrSDKMemoryManagerSuite *memorySuite;
};