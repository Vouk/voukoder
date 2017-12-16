#pragma once

#include <chrono>
#include "Decoder.h"

using namespace std::chrono;

typedef char fourchars[4];

union M128 {
	struct plane { fourchars a, y, u, v; } plane;
	__m128i i128;
};

void _log(const char *text);
milliseconds _getTimestamp();
void _printPerformance(const char *text, milliseconds start);

class Converter
{
public:
	Converter(int width, int height);
	~Converter();
	void convertVUYA4444_8uToYUV444(char *pixels, char *bufferY, char *bufferU, char *bufferV);
	void convertVUYA4444_32fToYUVA444p16(char *pixels, char *bufferY, char *bufferU, char *bufferV, char *bufferA);
	void decodeV210ToYUV422p10(char *pixels, int rowBytes, char *bufferY, char *bufferU, char *bufferV);

private:
	int width;
	int height;
	Decoder *v210decoder;

};