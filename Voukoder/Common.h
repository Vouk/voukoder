#pragma once

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
#include "Settings.h"
#include "EncoderConfig.h"

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
#include "libavutil\mathematics.h"
#include "x264.h"
#include "x265.h"
}

#pragma comment(lib, "libavformat.lib")
#pragma comment(lib, "libavcodec.lib")
#pragma comment(lib, "libavfilter.lib")
#pragma comment(lib, "libavutil.lib")
#pragma comment(lib, "libswscale.lib")
#pragma comment(lib, "libx264.lib")
#pragma comment(lib, "x265.lib")

#if defined(LIB_LIBAV) 
	#pragma comment(lib, "libavresample.lib")
#endif  

#if defined(LIB_FFMPEG) 
	#pragma comment(lib, "libpostproc.lib")
	#pragma comment(lib, "libswresample.lib")
#endif  

#define PLUGIN_APPNAME							L"Voukoder 0.3.8"
#define PLUGIN_ERR_COMBINATION_NOT_SUPPORTED	L"This combination of muxer, codecs and parameters is not supported."
#define PLUGIN_AUDIO_SAMPLE_FORMAT				AV_SAMPLE_FMT_FLTP
//#define PLUGIN_AUDIO_SAMPLE_RATE				48000
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

typedef struct EncodingData
{
	char *pix_fmt;
	char *plane[4] = {};
	csSDK_uint32 stride[4];
} EncodingData;

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
	PrSDKPPix2Suite *ppix2Suite;
	PrSDKExportProgressSuite *exportProgressSuite;
	PrSDKWindowSuite *windowSuite;
	HMODULE hInstance;
	Settings *settings;
	EncoderConfig *videoConfig;
	EncoderConfig *audioConfig;
	csSDK_uint32 videoRenderID;
	csSDK_uint32 audioRenderID;
	csSDK_int32 maxBlip;
	//csSDK_uint32 maxPasses;
	csSDK_uint32 currentPass;
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
