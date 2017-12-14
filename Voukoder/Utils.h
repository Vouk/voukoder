#pragma once

#include <chrono>

using namespace std::chrono;

typedef char fourchars[4];

union M128 {
	struct plane { fourchars a, y, u, v; } plane;
	__m128i i128;
};

void _log(const char *text);
milliseconds _getTimestamp();
void _printPerformance(const char *text, milliseconds start);

class Utils
{
public:
	static void ConvertVUYA4444_8uToYUV444(char *pixels, int width, int height, char *bufferY, char *bufferU, char *bufferV);
	static void ConvertVUYA4444_32fToYUVA444p16(char *pixels, int width, int height, char *bufferY, char *bufferU, char *bufferV, char *bufferA);
};