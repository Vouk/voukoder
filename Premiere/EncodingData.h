#pragma once

#include <string>

using namespace std;

typedef struct EncodingData
{
	int planes;
	const char *pix_fmt;
	char *plane[8] = {};
	int32_t stride[8] = { -1, -1, -1, -1, -1, -1, -1, -1 };
	int pass;
	struct filters
	{
		bool vflip = false;
		string scale = "";
	} filters;
	bool useBuffers = false;
} EncodingData;
