#include <tmmintrin.h>
#include <Windows.h>
#include "Utils.h"
#include "InstructionSet.h"

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

void Utils::ConvertVUYA4444_8uToYUV444(char *pixels, int width, int height, char *bufferY, char *bufferU, char *bufferV)
{
	const int rowBytes = width * 4;

	milliseconds ms = _getTimestamp();

	// Using SSSE3 is 10 times faster ....
	if (InstructionSet::SSSE3())
	{
		// Shuffle mask
		__m128i mask = _mm_set_epi8(
			12, 8, 4, 0,
			13, 9, 5, 1,
			14, 10, 6, 2,
			15, 11, 7, 3
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
	else
	{
		// Do classic loops and byte after byte (slow ...)
		for (int r = height - 1, p = 0; r >= 0; r--)
		{
			for (int c = 0; c < width; c++)
			{
				int pp = r * rowBytes + c * 4;

				bufferY[p] = pixels[pp + 2];
				bufferU[p] = pixels[pp + 1];
				bufferV[p] = pixels[pp + 0];

				p++;
			}
		}

		// Add to performance log
		_printPerformance("pixel format conversion (yuva4444_8u to yuv444p)", ms);
	}
}

void Utils::ConvertVUYA4444_32fToYUVA444p16(char *pixels, int width, int height, char *bufferY, char *bufferU, char *bufferV, char *bufferA)
{
	// Scaling factors (note min. values are actually negative)
	const float yuva_factors[4][4] = {
		{ 0.07306f, 1.09132f, 0.00000f, 1.00000f }, // Y
		{ 0.57143f, 0.57143f, 0.50000f, 0.50000f }, // U
		{ 0.57143f, 0.57143f, 0.50000f, 0.50000f }, // V
		{ 0.00000f, 1.00000f, 0.00000f, 1.00000f }  // A
	};

	const int rowBytes = width * 4;

	milliseconds ms = _getTimestamp();

	// Using SSSE3 is 10 times faster ....
	/*
	if (InstructionSet::SSSE3())
	{
		// Missing
	}
	else
	*/
	{
		//TODO: Colors are not right

		// De-Interleave and convert source buffer
		for (int r = height - 1, p = 0; r >= 0; r--)
		{
			for (int c = 0; c < width; c++)
			{
				// Get beginning of next block
				const int pos = r * width * 4 + c * 4;

				((uint16_t*)bufferY)[p] = (uint16_t)((((float*)pixels)[pos + 0] + yuva_factors[0][0]) / (yuva_factors[0][0] + yuva_factors[0][1]) * 65535.0f);
				((uint16_t*)bufferU)[p] = (uint16_t)((((float*)pixels)[pos + 1] + yuva_factors[1][0]) / (yuva_factors[1][0] + yuva_factors[1][1]) * 65535.0f);
				((uint16_t*)bufferV)[p] = (uint16_t)((((float*)pixels)[pos + 2] + yuva_factors[2][0]) / (yuva_factors[2][0] + yuva_factors[2][1]) * 65535.0f);
				((uint16_t*)bufferA)[p] = (uint16_t)((((float*)pixels)[pos + 3] + yuva_factors[3][0]) / (yuva_factors[3][0] + yuva_factors[3][1]) * 65535.0f);

				p++;
			}
		}		

		// Add to performance log
		_printPerformance("pixel format conversion (yuva4444_32f to yuv444p16le)", ms);
	}
}