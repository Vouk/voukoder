#pragma once

#include <string>
#include "lavf.h"

using namespace std;

namespace LibVKDR
{
	struct ExportSettings
	{
		string filename;
		string muxerName;
		string videoCodecName;
		string audioCodecName;
		int width;
		int height;
		int passes;
		AVFieldOrder fieldOrder;
		string pixelFormat;
		int videoTimebaseNum;
		int videoTimebaseDen;
		int audioChannelLayout;
		int audioTimebaseNum;
		int audioTimebaseDen;
		string videoOptions;
		string audioOptions;
		string tvStandard;
		AVColorRange colorRange;
		AVColorSpace colorSpace;
		AVColorPrimaries colorPrimaries;
		AVColorTransferCharacteristic colorTRC;
	};
}