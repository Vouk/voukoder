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

extern "C" {
#include "libavformat\avformat.h"
#include "libavcodec\avcodec.h"
#include "libswscale\swscale.h"
#include "libavfilter\avfilter.h"
#include "libavfilter\buffersrc.h"
#include "libavfilter\buffersink.h"
#include "libavutil\avutil.h"
#include "libavutil\audio_fifo.h"
#include "libavutil\opt.h"
#include "libavutil\imgutils.h"
#include "libavutil\fifo.h"
#include "libavutil\log.h"
}

#pragma comment(lib, "avformat.lib")
#pragma comment(lib, "avcodec.lib")
#pragma comment(lib, "avfilter.lib")
#pragma comment(lib, "avutil.lib")

#define APPNAME L"Voukoder (x264 plugin)"
#define PLUGIN_AUDIO_SAMPLE_FORMAT AV_SAMPLE_FMT_FLTP
//#define PLUGIN_AUDIO_SAMPLE_RATE 48000
#define PLUGIN_VIDEO_PIX_FORMAT AV_PIX_FMT_ARGB

struct ContainerFormat
{

};

struct Framerate
{
	int64_t ticksPerSecond;
	AVRational timebase;
};

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
} InstanceRec;

enum FFExportFormat
{
	MP4,
	Matroska,
	MOV
};

struct ExportFormats
{
	FFExportFormat exportFormat;
	std::string name;
	std::vector<AVCodecID> codecIDs;
};

struct RatioParam
{
	csSDK_int32 num;
	csSDK_int32 den;
	std::string name;
};

struct SimpleParam
{
	csSDK_int32 value;
	std::string name;
};

struct DoubleParam
{
	double value;
	std::string name;
};

typedef struct Config
{
	std::vector<ExportFormats> const formats
	{
		{ FFExportFormat::MP4, "mp4", { AV_CODEC_ID_H264, AV_CODEC_ID_AAC } },
		{ FFExportFormat::Matroska, "matroska",{ AV_CODEC_ID_H264, AV_CODEC_ID_AAC } },
		{ FFExportFormat::MOV, "mov",{ AV_CODEC_ID_H264, AV_CODEC_ID_AAC } }
	};
	std::vector<RatioParam> const pixelAspectRatios
	{
		{ 1, 1, "Square pixels (1.0)" },
		{ 10, 11, "D1/DV NTSC (0.9091)" },
		{ 40, 33, "D1/DV NTSC Widescreen 16:9 (1.2121)" },
		{ 768, 702, "D1/DV PAL (1.0940)" },
		{ 1024, 702, "D1/DV PAL Widescreen 16:9 (1.4587)" },
		{ 2, 1, "Anamorphic 2:1 (2.0)" },
		{ 4, 3, "HD Anamorphic 1080 (1.3333)" },
		{ 3, 2, "DVCPRO HD (1.5)" }
	};
	std::vector<RatioParam> const framerates
	{
		{ 10, 1, "10" },
		{ 15, 1, "15" },
		{ 24000, 1001, "23.976" },
		{ 24, 1, "24" },
		{ 25, 1, "25 (PAL)" },
		{ 30000, 1001, "29.97 (NTSC)" },
		{ 30, 1, "30" },
		{ 50, 1, "50" },
		{ 60000, 1001, "59.94" },
		{ 60, 1, "60" }
	};
	std::vector<SimpleParam> const fieldOrders
	{
		{ prFieldsUpperFirst, "Upper First" },
		{ prFieldsLowerFirst, "Lower First" },
		{ prFieldsNone, "Progressive" }
	};
} Config;

#pragma region Codec: X264

typedef struct X264
{
	enum class Preset { UltraFast, SuperFast, VeryFast, Faster, Fast, Medium, Slow, Slower, VerySlow, Placebo };
	enum class Profile { Baseline, Main, High, High422, High444 };
	enum class Level { Auto, Level1, Level1_1, Level1_2, Level1_3, Level2, Level2_1, Level2_2, Level3, Level3_1, Level3_2, Level4, Level4_1, Level4_2, Level5, Level5_1 };
	enum class Strategy { ABR, CRF, QP };

	std::vector<SimpleParam> const presets
	{
		{ (csSDK_int32)X264::Preset::UltraFast, "Ultra fast" },
		{ (csSDK_int32)X264::Preset::SuperFast, "Super fast" },
		{ (csSDK_int32)X264::Preset::VeryFast, "Very fast" },
		{ (csSDK_int32)X264::Preset::Faster, "Faster" },
		{ (csSDK_int32)X264::Preset::Fast, "Fast" },
		{ (csSDK_int32)X264::Preset::Medium, "Medium" },
		{ (csSDK_int32)X264::Preset::Slow, "Slow" },
		{ (csSDK_int32)X264::Preset::Slower, "Slower" },
		{ (csSDK_int32)X264::Preset::VerySlow, "Very slow" },
		{ (csSDK_int32)X264::Preset::Placebo, "Placebo" }
	};
	std::vector<SimpleParam> const profiles
	{
		{ (csSDK_int32)X264::Profile::Baseline, "Baseline" },
		{ (csSDK_int32)X264::Profile::Main, "Main" },
		{ (csSDK_int32)X264::Profile::High, "High" }
	};
	std::vector<SimpleParam> const levels
	{
		{ (csSDK_int32)X264::Level::Auto, "(Automatic)" },
		{ (csSDK_int32)X264::Level::Level1, "1" },
		{ (csSDK_int32)X264::Level::Level1_1, "1.1" },
		{ (csSDK_int32)X264::Level::Level1_2, "1.2" },
		{ (csSDK_int32)X264::Level::Level1_3, "1.3" },
		{ (csSDK_int32)X264::Level::Level2, "2" },
		{ (csSDK_int32)X264::Level::Level2_1, "2.1" },
		{ (csSDK_int32)X264::Level::Level2_2, "2.2" },
		{ (csSDK_int32)X264::Level::Level3, "3" },
		{ (csSDK_int32)X264::Level::Level3_1, "3.1" },
		{ (csSDK_int32)X264::Level::Level3_2, "3.2" },
		{ (csSDK_int32)X264::Level::Level4, "4" },
		{ (csSDK_int32)X264::Level::Level4_1, "4.1" },
		{ (csSDK_int32)X264::Level::Level4_2, "4.2" },
		{ (csSDK_int32)X264::Level::Level5, "5" },
		{ (csSDK_int32)X264::Level::Level5_1, "5.1" }
	};
	std::vector<SimpleParam> const strategies
	{
		{ (csSDK_int32)X264::Strategy::ABR, "Average Bitrate (ARB)" },
		{ (csSDK_int32)X264::Strategy::CRF, "Constant Rate Factor (CRF)" },
		{ (csSDK_int32)X264::Strategy::QP, "Constant Quantizer (QP)" }
	};
} X264;

#pragma endregion

#pragma region Codec: AAC

typedef struct AAC
{
	enum class SampleRate { SR44100 = 44100, SR48000 = 48000, SR96000 = 96000 };
	enum class Channels { Mono = kPrAudioChannelType_Mono, Stereo = kPrAudioChannelType_Stereo, FivePointOne = kPrAudioChannelType_51 };
	enum class Bitrate { BR64 = 64, BR96 = 96, BR128 = 128, BR192 = 192, BR256 = 256, BR320 = 320, BR384 = 384 };

	std::vector<SimpleParam> const samplerates
	{
		{ (csSDK_int32)AAC::SampleRate::SR44100, "44100 Hz" },
		{ (csSDK_int32)AAC::SampleRate::SR48000, "48000 Hz" },
		{ (csSDK_int32)AAC::SampleRate::SR96000, "96000 Hz" }
	};
	std::vector<SimpleParam> const channels
	{
		{ (csSDK_int32)AAC::Channels::Mono, "Mono" },
		{ (csSDK_int32)AAC::Channels::Stereo, "Stereo" },
		{ (csSDK_int32)AAC::Channels::FivePointOne, "5.1" }
	};
	std::vector<SimpleParam> const bitrates
	{
		{ (csSDK_int32)AAC::Bitrate::BR64, "64" },
		{ (csSDK_int32)AAC::Bitrate::BR96, "96" },
		{ (csSDK_int32)AAC::Bitrate::BR128, "128" },
		{ (csSDK_int32)AAC::Bitrate::BR192, "192" },
		{ (csSDK_int32)AAC::Bitrate::BR256, "256" },
		{ (csSDK_int32)AAC::Bitrate::BR320, "320" },
		{ (csSDK_int32)AAC::Bitrate::BR384, "384" }
	};
} AAC;

#pragma endregion


void copyConvertStringLiteralIntoUTF16(const wchar_t* inputString, prUTF16Char* destination);
