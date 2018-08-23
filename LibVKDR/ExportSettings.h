#pragma once

#include <string>
#include <vector>
#include "lavf.h"

using namespace std;

namespace LibVKDR
{
	struct ExportSettings
	{
		string filename;
		string muxerName;
		string application;
		string videoCodecName;
		string audioCodecName;
		int width;
		int height;
		int resizeWidth = 0;
		int resizeHeight = 0;
		int passes;
		bool pipe = false;
		wstring pipeCommand;
		AVFieldOrder fieldOrder;
		string pixelFormat;
		AVRational videoTimebase;
		AVRational videoSar;
		int audioChannelLayout;
		AVRational audioTimebase;
		string videoOptions;
		string audioOptions;
		string tvStandard;
		AVColorRange colorRange;
		AVColorSpace colorSpace;
		AVColorPrimaries colorPrimaries;
		AVColorTransferCharacteristic colorTRC;
		bool exportAudio;
		bool flagFaststart = true;
		vector<string> videoFilters;
	};
}