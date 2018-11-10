#include "AudioRenderer.h"
#include <math.h>

AudioRenderer::AudioRenderer(csSDK_uint32 pluginId, PrTime startTime, PrTime endTime, PrAudioChannelType channelType, float audioSampleRate, PrTime ticksPerFrame,
	PrSDKSequenceAudioSuite *sequenceAudioSuite, PrSDKTimeSuite *timeSuite, PrSDKMemoryManagerSuite *memorySuite) :
	pluginId(pluginId),
	sequenceAudioSuite(sequenceAudioSuite),
	memorySuite(memorySuite)
{
	PrTime ticksPerSample;
	timeSuite->GetTicksPerAudioSample(audioSampleRate, &ticksPerSample);

	// Ceil up divison
	samplesTotal = 1 + ((endTime - startTime - 1) / ticksPerSample);

#ifdef BUILD_CS6_COMPATIBLE
	sequenceAudioSuite->MakeAudioRenderer(pluginId, startTime, channelType, kPrAudioSampleType_32BitFloat, audioSampleRate, &rendererId);
#else
	PrAudioChannelLabel *layout;
	if (channelType == kPrAudioChannelType_Mono)
	{
		layout = new PrAudioChannelLabel[1]{ kPrAudioChannelLabel_Discrete };
	}
	else if (channelType == kPrAudioChannelType_Stereo)
	{
		layout = new PrAudioChannelLabel[2]{ kPrAudioChannelLabel_FrontLeft, kPrAudioChannelLabel_FrontRight };
	}
	else if (channelType == kPrAudioChannelType_51)
	{
		layout = new PrAudioChannelLabel[6]{ kPrAudioChannelLabel_FrontLeft, kPrAudioChannelLabel_FrontRight, kPrAudioChannelLabel_BackLeft, kPrAudioChannelLabel_BackRight, kPrAudioChannelLabel_FrontCenter, kPrAudioChannelLabel_LowFrequency };
	}
	else
	{
		return;
	}
	sequenceAudioSuite->MakeAudioRenderer(pluginId, startTime, channelType, layout, kPrAudioSampleType_32BitFloat, audioSampleRate, &rendererId);
#endif
	
	sequenceAudioSuite->GetMaxBlip(rendererId, ticksPerFrame, &maxBlip);

	// Reserve audio buffers
	for (int i = 0; i < MAX_CHANNELS; i++)
	{
		buffer[i] = (float*)memorySuite->NewPtr(sizeof(float) * maxBlip);
	}
}

AudioRenderer::~AudioRenderer()
{
	// Free audio buffers
	for (int i = 0; i < MAX_CHANNELS; i++)
	{
		memorySuite->PrDisposePtr((char*)buffer[i]);
	}

	sequenceAudioSuite->ReleaseAudioRenderer(pluginId, rendererId);
}

void AudioRenderer::reset()
{
	sequenceAudioSuite->ResetAudioToBeginning(rendererId);
	samplesRemaining = samplesTotal;
}

float** AudioRenderer::getSamples(csSDK_uint32 *size, prBool clip)
{
	if (*size > (csSDK_uint32)maxBlip)
	{
		*size = (csSDK_uint32)maxBlip;
	}

	if (*size > (csSDK_uint32)samplesRemaining)
	{
		*size = (csSDK_uint32)samplesRemaining;
	}

	sequenceAudioSuite->GetAudio(rendererId, *size, buffer, clip);

	samplesRemaining -= *size;

	return buffer;
}