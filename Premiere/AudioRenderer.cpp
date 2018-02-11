#include "AudioRenderer.h"

AudioRenderer::AudioRenderer(csSDK_uint32 pluginId, PrTime startTime, PrTime endTime, PrAudioChannelType channelType, float audioSampleRate, PrTime ticksPerFrame,
	PrSDKSequenceAudioSuite *sequenceAudioSuite, PrSDKTimeSuite *timeSuite, PrSDKMemoryManagerSuite *memorySuite) :
	pluginId(pluginId),
	sequenceAudioSuite(sequenceAudioSuite),
	memorySuite(memorySuite)
{
	PrTime ticksPerSample;
	timeSuite->GetTicksPerAudioSample(audioSampleRate, &ticksPerSample);

	sequenceAudioSuite->MakeAudioRenderer(
		pluginId,
		startTime, 
		channelType,
		kPrAudioSampleType_32BitFloat, 
		audioSampleRate,
		&rendererId);

	sequenceAudioSuite->ResetAudioToBeginning(rendererId);
	sequenceAudioSuite->GetMaxBlip(rendererId, ticksPerFrame, &maxBlip);

	for (int i = 0; i < MAX_CHANNELS; i++)
	{
		buffer[i] = (float*)memorySuite->NewPtr(sizeof(float) * maxBlip);
	}

	samplesRemaining = (endTime - startTime) / ticksPerSample;
}

AudioRenderer::~AudioRenderer()
{
	for (int i = 0; i < MAX_CHANNELS; i++)
	{
		memorySuite->PrDisposePtr((char*)buffer[i]);
	}

	sequenceAudioSuite->ReleaseAudioRenderer(pluginId, rendererId);
}

csSDK_int32 AudioRenderer::getMaxBlip()
{
	return maxBlip;
}

float** AudioRenderer::getSamples(csSDK_uint32 *size, prBool clip)
{
	if (samplesRemaining > maxBlip)
	{
		*size = maxBlip;
	}
	else
	{
		*size = (csSDK_uint32)samplesRemaining;
	}

	sequenceAudioSuite->GetAudio(rendererId, *size, buffer, clip);

	samplesRemaining -= *size;

	return buffer;
}

prBool AudioRenderer::samplesInBuffer()
{
	return samplesRemaining > 0;
}