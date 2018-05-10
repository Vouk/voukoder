#pragma once

#include <string>
#include <vector>
#include <map>
#include "ParamInfo.h"

using namespace std;

namespace LibVKDR
{
	enum EncoderType {
		Audio,
		Video
	};

	typedef struct EncoderInfo : IParamContainer
	{
		string text;
		string defaultPixelFormat;
		string multipassParameter;
		EncoderType encoderType;
		vector<EncoderParameterGroup> paramGroups;
		struct interlaced {
			map<string, string> bottomFrameFirst;
			map<string, string> topFrameFirst;
			map<string, string> progressive;
		} interlaced;
	} EncoderInfo;
}