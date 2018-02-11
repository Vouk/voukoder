#pragma once

#include <string>

#define MAX_FRAME_PLANES 4

using namespace std;

namespace LibVKDR
{
	typedef struct EncoderData
	{
		int planes;
		const char *pix_fmt;
		char *plane[MAX_FRAME_PLANES] = {};
		int32_t stride[MAX_FRAME_PLANES] = { -1, -1, -1, -1 };
		int pass;
	} EncoderData;
}