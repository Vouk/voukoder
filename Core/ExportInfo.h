#pragma once

#include <wx/wx.h>
#include "lavf.h"
#include "OptionContainer.h"

struct ExportInfo
{
	wxString filename;
	wxString application;
	int passes;

	struct Video
	{
		bool enabled;
		int width;
		int height;
		wxString id;
		OptionContainer options;
		AVRational timebase;
		AVPixelFormat pixelFormat = AVPixelFormat::AV_PIX_FMT_NONE; // TODO
		AVRational sampleAspectRatio;
		AVFieldOrder fieldOrder;
		AVColorRange colorRange = AVColorRange::AVCOL_RANGE_UNSPECIFIED;
		AVColorSpace colorSpace = AVColorSpace::AVCOL_SPC_UNSPECIFIED;
		AVColorPrimaries colorPrimaries = AVColorPrimaries::AVCOL_PRI_UNSPECIFIED;
		AVColorTransferCharacteristic colorTransferCharacteristics = AVColorTransferCharacteristic::AVCOL_TRC_UNSPECIFIED;
		int64_t ticksPerFrame = 0;
		vector<string> filters;
	} video;

	struct Audio
	{
		bool enabled;
		wxString id;
		OptionContainer options;
		AVRational timebase;
		AVSampleFormat sampleFormat = AVSampleFormat::AV_SAMPLE_FMT_NONE; // TODO
		uint64_t channelLayout;
	} audio;

	struct Format
	{
		wxString id;
		bool faststart;
	} format;
};