#pragma once

#include <vector>
#include <map>
#include <string>
#include "PrSDKStructs.h"
#include "PrSDKImport.h"
#include "PrSDKExport.h"
#include "PrSDKExportFileSuite.h"
#include "PrSDKExportInfoSuite.h"
#include "PrSDKExportParamSuite.h"
#include "PrSDKExportProgressSuite.h"
#include "PrSDKErrorSuite.h"
#include "PrSDKMALErrors.h"
#include "PrSDKMarkerSuite.h"
#include "PrSDKSequenceRenderSuite.h"
#include "PrSDKSequenceAudioSuite.h"
#include "PrSDKClipRenderSuite.h"
#include "PrSDKPPixCreatorSuite.h"
#include "PrSDKPPixCacheSuite.h"
#include "PrSDKMemoryManagerSuite.h"
#include "PrSDKWindowSuite.h"
#include "Settings.h"

extern "C" {
#include "libavformat\avformat.h"
#include "libavcodec\avcodec.h"
#include "libavfilter\avfilter.h"
#include "libavutil\avutil.h"
#include "libavutil\channel_layout.h"
#include "libswscale\swscale.h"
#include "libavfilter\buffersrc.h"
#include "libavfilter\buffersink.h"
#include "libavutil\audio_fifo.h"
#include "libavutil\opt.h"
#include "libavutil\imgutils.h"
#include "libavutil\fifo.h"
#include "libavutil\log.h"
}

#pragma comment(lib, "libavformat.lib")
#pragma comment(lib, "libavcodec.lib")
#pragma comment(lib, "libavfilter.lib")
#pragma comment(lib, "libavutil.lib")
#pragma comment(lib, "libswscale.lib")
#pragma comment(lib, "libavresample.lib")
#pragma comment(lib, "libx264.lib")

#define PLUGIN_APPNAME L"Voukoder"
#define PLUGIN_ERR_COMBINATION_NOT_SUPPORTED L"This combination of muxer, encoders and parameters is not supported.\n\nNOTE: MP4 supports AAC audio only."
#define PLUGIN_AUDIO_SAMPLE_FORMAT AV_SAMPLE_FMT_FLTP
//#define PLUGIN_AUDIO_SAMPLE_RATE 48000
#define PLUGIN_VIDEO_PIX_FORMAT AV_PIX_FMT_ARGB

#define FFMultiplexerTabGroup			"MultiplexerTab"
#define FFMultiplexerBasicGroup			"MultiplexerBasicGroup"
#define FFMultiplexer					"Multiplexer"

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
	PrSDKSequenceRenderSuite *sequenceRenderSuite;
	PrSDKExportFileSuite *exportFileSuite;
	PrSDKPPixSuite *ppixSuite;
	PrSDKExportProgressSuite *exportProgressSuite;
	PrSDKWindowSuite *windowSuite;
	Settings *settings;
} InstanceRec;

void copyConvertStringLiteralIntoUTF16(const wchar_t* inputString, prUTF16Char* destination);
void populateEncoders(InstanceRec *instRec, csSDK_uint32 pluginId, csSDK_int32 groupIndex, const char *elementIdentifier, json encoders);
void createEncoderOptionElements(PrSDKExportParamSuite *exportParamSuite, csSDK_uint32 pluginId, csSDK_int32 groupIndex, const char *groupIdentifier, json encoders, int selectedId);
exNewParamInfo createGUIElement(json option, prBool hidden);
void populateEncoderOptionValues(InstanceRec *instRec, csSDK_uint32 pluginId, csSDK_int32 groupIndex, json encoders);
void configureEncoderOption(InstanceRec *instRec, csSDK_uint32 pluginId, csSDK_int32 groupIndex, json option);