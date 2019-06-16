#pragma once

#include <premiere_cs6/PrSDKMemoryManagerSuite.h>
#include <premiere_cs6/PrSDKExportParamSuite.h>
#include <premiere_cs6/PrSDKExportInfoSuite.h>
#include <premiere_cs6/PrSDKSequenceAudioSuite.h>
#include <premiere_cs6/PrSDKExportFileSuite.h>
#include <premiere_cs6/PrSDKPPix2Suite.h>
#include <premiere_cs6/PrSDKExportProgressSuite.h>
#include <premiere_cs6/PrSDKWindowSuite.h>
#include <premiere_cs6/PrSDKExporterUtilitySuite.h>
#include <premiere_cs6/PrSDKSequenceInfoSuite.h>
#include <premiere_cs6/PrSDKImageProcessingSuite.h>

#define kEventTypeInformational 1
#define kEventTypeWarning 2
#define kEventTypeError 3

typedef struct PrSuites
{
	SPBasicSuite *basicSuite;
	PrSDKMemoryManagerSuite *memorySuite;
	PrSDKTimeSuite *timeSuite;
	PrSDKExportParamSuite *exportParamSuite;
	PrSDKExportInfoSuite *exportInfoSuite;
	PrSDKSequenceAudioSuite *sequenceAudioSuite;
	PrSDKExportFileSuite *exportFileSuite;
	PrSDKPPixSuite *ppixSuite;
	PrSDKPPix2Suite *ppix2Suite;
	PrSDKExportProgressSuite *exportProgressSuite;
	PrSDKWindowSuite *windowSuite;
	PrSDKExporterUtilitySuite *exporterUtilitySuite;
	PrSDKSequenceInfoSuite *sequenceInfoSuite;
	PrSDKImageProcessingSuite *imageProcessingSuite;
} PrSuites;
