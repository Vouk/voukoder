#pragma once

#include <string>

using namespace std;

typedef struct EncodingData
{
	int planes;
	const char *pix_fmt;
	char *plane[8] = {};
	unsigned int stride[8];
	int pass;
	struct filters
	{
		bool vflip = false;
		string scale = "";
	} filters;
	bool useBuffers = false;
} EncodingData;
