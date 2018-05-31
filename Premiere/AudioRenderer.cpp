#include "AudioRenderer.h"

AudioRenderer::AudioRenderer(csSDK_uint32 pluginId, PrTime startTime, PrTime endTime, PrAudioChannelType channelType, float audioSampleRate, PrTime ticksPerFrame,
	PrSDKSequenceAudioSuite *sequenceAudioSuite, PrSDKTimeSuite *timeSuite, PrSDKMemoryManagerSuite *memorySuite) :
	pluginId(pluginId),
	sequenceAudioSuite(sequenceAudioSuite),
	memorySuite(memorySuite)
{
	PrTime ticksPerSample;
	timeSuite->GetTicksPerAudioSample(audioSampleRate, &ticksPerSample);
	samplesTotal = (endTime - startTime) / ticksPerSample;

	sequenceAudioSuite->MakeAudioRenderer(
		pluginId,
		startTime, 
		channelType,
		kPrAudioSampleType_32BitFloat, 
		audioSampleRate,
		&rendererId);

	reset();

	sequenceAudioSuite->GetMaxBlip(rendererId, ticksPerFrame, &maxBlip);

	for (int i = 0; i < MAX_CHANNELS; i++)
	{
		buffer[i] = (float*)memorySuite->NewPtr(sizeof(float) * maxBlip);
	}
}

AudioRenderer::~AudioRenderer()
{
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
		*size = (csSDK_uint32)maxBlip;

	if (*size > (csSDK_uint32)samplesRemaining)
		*size = (csSDK_uint32)samplesRemaining;

	sequenceAudioSuite->GetAudio(rendererId, *size, buffer, clip);

	samplesRemaining -= *size;

	return buffer;
}

prBool AudioRenderer::samplesInBuffer()
{
	return samplesRemaining > 0;
}