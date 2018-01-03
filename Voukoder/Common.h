#pragma once

#include <tmmintrin.h>
#include <vector>
#include <map>
#include <string>
#include "premiere_cs6\PrSDKStructs.h"
#include "premiere_cs6\PrSDKImport.h"
#include "premiere_cs6\PrSDKExport.h"
#include "premiere_cs6\PrSDKExportFileSuite.h"
#include "premiere_cs6\PrSDKExportInfoSuite.h"
#include "premiere_cs6\PrSDKExportParamSuite.h"
#include "premiere_cs6\PrSDKExportProgressSuite.h"
#include "premiere_cs6\PrSDKErrorSuite.h"
#include "premiere_cs6\PrSDKMALErrors.h"
#include "premiere_cs6\PrSDKMarkerSuite.h"
#include "premiere_cs6\PrSDKSequenceRenderSuite.h"
#include "premiere_cs6\PrSDKSequenceAudioSuite.h"
#include "premiere_cs6\PrSDKClipRenderSuite.h"
#include "premiere_cs6\PrSDKPPixCreatorSuite.h"
#include "premiere_cs6\PrSDKPPixCacheSuite.h"
#include "premiere_cs6\PrSDKPPix2Suite.h"
#include "premiere_cs6\PrSDKMemoryManagerSuite.h"
#include "premiere_cs6\PrSDKWindowSuite.h"
#include "premiere_cs6\PrSDKExporterUtilitySuite.h"
#include "premiere_cs6\PrSDKImageProcessingSuite.h"
#include "Settings.h"
#include "EncoderConfig.h"
#include "lavf.h"

#define PLUGIN_APPNAME							L"Voukoder 0.5.2"
#define MAX_AUDIO_CHANNELS						6
#define VKDRTVStandard							"VKDRTVStandard"
#define VKDRColorSpace							"VKDRColorSpace"
#define VKDRColorRange							"VKDRColorRange"
#define VKDRAdvVideoCodecTabGroup				"VKDRAdvVideoCodecTabGroup"
#define FFMultiplexerTabGroup					"MultiplexerTab"
#define FFMultiplexerBasicGroup					"MultiplexerBasicGroup"
#define FFMultiplexer							"Multiplexer"

using json = nlohmann::json;

#define string2wchar_t(str) std::wstring(str.begin(), str.end()).c_str()

typedef struct InstanceRec
{
	SPBasicSuite *spBasic;
	PrSDKMemoryManagerSuite *memorySuite;
	PrSDKTimeSuite *timeSuite;
	PrSDKExportParamSuite *exportParamSuite;
	PrSDKExportInfoSuite *exportInfoSuite;
	PrSDKSequenceAudioSuite *sequenceAudioSuite;
	PrSDKExportFileSuite *exportFileSuite;
	PrSDKPPixSuite *ppixSuite;
	PrSDKExportProgressSuite *exportProgressSuite;
	PrSDKWindowSuite *windowSuite;
	PrSDKExporterUtilitySuite *exporterUtilitySuite;
	PrSDKImageProcessingSuite *imageProcessingSuite;
	HMODULE hInstance;
	Settings *settings;
} InstanceRec;

typedef csSDK_int32 vkdrTvStandard;
#define vkdrPAL			0
#define vkdrNTSC		1

typedef csSDK_int32 vkdrColorSpace;
#define vkdrBT601		0		// SD
#define vkdrBT709		1		// HD
#define vkdrBT2020		2		// UHD

typedef csSDK_int32 vkdrColorRange;
#define vkdrFullColorRange		kPrTrue
#define vkdrLimitedColorRange	kPrFalse

PrTime gcd(PrTime a, PrTime b);
void PopulateEncoders(InstanceRec *instRec, csSDK_uint32 pluginId, csSDK_int32 groupIndex, const char *elementIdentifier, vector<EncoderInfo> encoderInfos);
void CreateEncoderParamElements(PrSDKExportParamSuite *exportParamSuite, csSDK_uint32 pluginId, csSDK_int32 groupIndex, vector<EncoderInfo> encoderInfos, int selectedId);
exNewParamInfo CreateParamElement(ParamInfo *info, bool hidden);
void PopulateParamValues(InstanceRec *instRec, csSDK_uint32 pluginId, csSDK_int32 groupIndex, vector<EncoderInfo> encoderInfos);
void ConfigureEncoderParam(InstanceRec *instRec, csSDK_uint32 pluginId, csSDK_int32 groupIndex, ParamInfo *info);
void ShowMessageBox(InstanceRec *instRec, LPCWSTR lpText, LPCWSTR lpCaption, UINT uType);
void ShowMessageBox(InstanceRec *instRec, LPCSTR lpText, LPCSTR lpCaption, UINT uType);