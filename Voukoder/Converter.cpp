#include <tmmintrin.h>
#include <Windows.h>
#include "Converter.h"

milliseconds _getTimestamp()
{
#if defined(_DEBUG) 
	return duration_cast<milliseconds>(system_clock::now().time_since_epoch());
#endif
}

void _printPerformance(const char *text, milliseconds start)
{
#if defined(_DEBUG) 
	milliseconds end = duration_cast< milliseconds >(system_clock::now().time_since_epoch());

	char buffer[1024];
	sprintf_s(buffer, "%s: %I64ums\n", text, (end - start).count());

	_log(buffer);
#endif
}

void _log(const char *text)
{
	OutputDebugStringA(text);
}

Converter::Converter(int width, int height):
	width(width),
	height(height)
{}

Converter::~Converter()
{
	if (v210decoder != NULL)
	{
		v210decoder->close();
		v210decoder = NULL;
	}
}

void Converter::convertVUYA4444_8uToYUV444(char *pixels, char *bufferY, char *bufferU, char *bufferV)
{
	const int rowBytes = width * 4;

	milliseconds ms = _getTimestamp();

	// Shuffle mask
	__m128i mask = _mm_set_epi8(
		12, 8,  4, 0, // Y
		13, 9,  5, 1, // U
		14, 10, 6, 2, // V
		15, 11, 7, 3  // A
	);

	M128 dest;

	// De-Interleave source buffer
	for (int r = height - 1, p = 0; r >= 0; r--)
	{
		for (int c = 0; c <rowBytes; c += 16)
		{
			dest.i128 = _mm_shuffle_epi8(_mm_loadu_si128((__m128i *)&pixels[r * rowBytes + c]), mask);
			memcpy(bufferY + p, dest.plane.y, 4);
			memcpy(bufferU + p, dest.plane.u, 4);
			memcpy(bufferV + p, dest.plane.v, 4);
			p += 4;
		}
	}

	// Add to performance log
	_printPerformance("pixel format conversion (yuva4444_8u to yuv444p using SSSE3)", ms);
}

void Converter::convertVUYA4444_32fToYUVA444p16(char *pixels, char *bufferY, char *bufferU, char *bufferV, char *bufferA)
{
	// Scaling factors (note min. values are actually negative) (limited range)
	const float yuva_factors[4][2] = {
		{ 0.07306f, 1.09132f }, // Y
		{ 0.57143f, 0.57143f }, // U
		{ 0.57143f, 0.57143f }, // V
		{ 0.00000f, 1.00000f }  // A
	};

	const int rowBytes = width * 4;

	milliseconds ms = _getTimestamp();

	float *frameBuffer = (float*)pixels;

	// De-Interleave and convert source buffer
	for (int r = height - 1, p = 0; r >= 0; r--)
	{
		for (int c = 0; c < width; c++)
		{
			// Get beginning of next block
			const int pos = r * width * 4 + c * 4;

			// VUYA -> YUVA
			((uint16_t*)bufferY)[p] = (uint16_t)((frameBuffer[pos + 2] + yuva_factors[0][0]) / (yuva_factors[0][0] + yuva_factors[0][1]) * 65535.0f);
			((uint16_t*)bufferU)[p] = (uint16_t)((frameBuffer[pos + 1] + yuva_factors[1][0]) / (yuva_factors[1][0] + yuva_factors[1][1]) * 65535.0f);
			((uint16_t*)bufferV)[p] = (uint16_t)((frameBuffer[pos + 0] + yuva_factors[2][0]) / (yuva_factors[2][0] + yuva_factors[2][1]) * 65535.0f);
			((uint16_t*)bufferA)[p] = (uint16_t)((frameBuffer[pos + 3] + yuva_factors[3][0]) / (yuva_factors[3][0] + yuva_factors[3][1]) * 65535.0f);

			p++;
		}
	}		

	// Add to performance log
	_printPerformance("pixel format conversion (yuva4444_32f to yuv444p16le)", ms);
}

void Converter::decodeV210ToYUV422p10(char *pixels, int rowBytes, char *bufferY, char *bufferU, char *bufferV)
{
	milliseconds ms = _getTimestamp();

	// Is a decoder instance open?
	if (v210decoder == NULL)
	{
		v210decoder = new Decoder();
		v210decoder->open(AV_CODEC_ID_V210, width, height);
	}

	// Send encoded data to decoder
	AVFrame *frame = v210decoder->decodeData((uint8_t*)pixels, rowBytes * height);
	if (frame != NULL)
	{
		bufferY = (char*)frame->data[0];
		bufferU = (char*)frame->data[1];
		bufferV = (char*)frame->data[2];
	}

	// Add to performance log
	_printPerformance("pixel format conversion (v210 to yuv422p10le)", ms);
}