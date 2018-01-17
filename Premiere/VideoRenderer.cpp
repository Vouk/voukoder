#include "VideoRenderer.h"
#include <tmmintrin.h>
#include <immintrin.h>
#include <chrono>
#include <sstream>
using namespace std::chrono;

static time_point<steady_clock> tp1, tp2, tp3, tp4, tp5;

// reviewed 0.5.3
VideoRenderer::VideoRenderer(csSDK_uint32 videoRenderID, csSDK_uint32 width, csSDK_uint32 height, PrSDKPPixSuite *ppixSuite, PrSDKPPix2Suite *ppix2Suite, PrSDKMemoryManagerSuite *memorySuite, PrSDKExporterUtilitySuite *exporterUtilitySuite, PrSDKImageProcessingSuite *imageProcessingSuite) :
	videoRenderID(videoRenderID),
	width(width),
	height(height),
	ppixSuite(ppixSuite),
	ppix2Suite(ppix2Suite),
	memorySuite(memorySuite),
	exporterUtilitySuite(exporterUtilitySuite),
	imageProcessingSuite(imageProcessingSuite)
{
	// Reserve buffers (max. MAX_FRAME_PLANES planes and 4 components per pixel)
	for (int i = 0; i < MAX_FRAME_PLANES; i++)
	{
		encodingData.plane[i] = (char*)memorySuite->NewPtr(width * height * 4);
	}

	// Reserve conversion buffer (max. MAX_FRAME_PLANES planes and 4 components per pixel)
	conversionBuffer = (char*)memorySuite->NewPtr(width * height * MAX_FRAME_PLANES * 4);
}

// reviewed 0.5.3
VideoRenderer::~VideoRenderer()
{
	memorySuite->PrDisposePtr(conversionBuffer);

	for (int i = 0; i < MAX_FRAME_PLANES; i++)
	{
		memorySuite->PrDisposePtr(encodingData.plane[i]);
	}
}

// reviewed 0.5.3
void VideoRenderer::deinterleave(char* pixels, csSDK_int32 rowBytes, char *bufferY, char *bufferU, char *bufferV)
{
	__m128i mask = _mm_set_epi8(
		12, 8, 4, 0, // Y
		13, 9, 5, 1, // U
		14, 10, 6, 2, // V
		15, 11, 7, 3  // A (not needed)
	);

	M128 dest;

	for (int r = height - 1, p = 0; r >= 0; r--)
	{
		for (int c = 0; c < rowBytes; c += 16)
		{
			dest.i128 = _mm_shuffle_epi8(_mm_loadu_si128((__m128i *)&pixels[r * rowBytes + c]), mask);
			memcpy(bufferY + p, dest.plane.y, 4);
			memcpy(bufferU + p, dest.plane.u, 4);
			memcpy(bufferV + p, dest.plane.v, 4);
			p += 4;
		}
	}
}

// helper function for unrolling
static inline __m128i load_and_scale(const float *src)
{  // and convert to 32-bit integer with truncation towards zero.

   // Scaling factors (note min. values are actually negative) (limited range)
	const float yuvaf[4][2] = {
		{ 0.07306f, 1.09132f }, // Y
		{ 0.57143f, 0.57143f }, // U
		{ 0.57143f, 0.57143f }, // V
		{ 0.00000f, 1.00000f }  // A
	};

	// (Y + yuvaf[n][0]) / (yuvaf[n][0] + yuvaf[n][1]) ->
	// Y * 1.0f/(yuvaf[n][0] + yuvaf[n][1]) + yuvaf[n][0]/(yuvaf[n][0] + yuvaf[n][1])

	// Pixels are in VUYA order in memory, from low to high address
	const __m128 scale_mul = _mm_setr_ps(
		65535.0f / (yuvaf[2][0] + yuvaf[2][1]),  // V
		65535.0f / (yuvaf[1][0] + yuvaf[1][1]),  // U
		65535.0f / (yuvaf[0][0] + yuvaf[0][1]),  // Y
		65535.0f / (yuvaf[3][0] + yuvaf[3][1])   // A
	);

	const __m128 scale_add = _mm_setr_ps(
		65535.0f * yuvaf[2][0] / (yuvaf[2][0] + yuvaf[2][1]),  // V
		65535.0f * yuvaf[1][0] / (yuvaf[1][0] + yuvaf[1][1]),  // U
		65535.0f * yuvaf[0][0] / (yuvaf[0][0] + yuvaf[0][1]),  // Y
		65535.0f * yuvaf[3][0] / (yuvaf[3][0] + yuvaf[3][1])   // A
	);

	// prefer having src aligned for performance, but with AVX it won't help the compiler much to know it's aligned.
	// So just use an unaligned load intrinsic
	__m128 srcv = _mm_loadu_ps(src);
	__m128 scaled = _mm_fmadd_ps(srcv, scale_mul, scale_add);
	__m128i vuya = _mm_cvttps_epi32(scaled);  // truncate toward zero
											  // for round-to-nearest, use cvtps_epi32 instead
	return vuya;
}

// Big thanks to Peter Cordes
void VideoRenderer::deinterleave_avx_fma(char* __restrict pixels, int rowBytes, char *__restrict bufferY, char *__restrict bufferU, char *__restrict bufferV, char *__restrict bufferA)
{

	const float *src = (float*)pixels;
	uint16_t *__restrict Y = (uint16_t*)bufferY;
	uint16_t *__restrict U = (uint16_t*)bufferU;
	uint16_t *__restrict V = (uint16_t*)bufferV;
	uint16_t *__restrict A = (uint16_t*)bufferA;

	// 4 pixels per loop iteration, loading 4x 16 bytes of floats
	// and storing 4x 8 bytes of uint16_t.
	for (unsigned pos = 0; pos < width*height * 4; pos += 4) {
		// pos*4 because each pixel is 4 floats long
		__m128i vuya0 = load_and_scale(src + pos * 4);
		__m128i vuya1 = load_and_scale(src + pos * 4 + 1);
		__m128i vuya2 = load_and_scale(src + pos * 4 + 2);
		__m128i vuya3 = load_and_scale(src + pos * 4 + 3);

		__m128i vuya02 = _mm_packus_epi32(vuya0, vuya2);  // vuya0 | vuya2
		__m128i vuya13 = _mm_packus_epi32(vuya1, vuya3);  // vuya1 | vuya3
		__m128i vvuuyyaa01 = _mm_unpacklo_epi16(vuya02, vuya13);   // V0V1 U0U1 | Y0Y1 A0A1
		__m128i vvuuyyaa23 = _mm_unpackhi_epi16(vuya02, vuya13);   // V2V3 U2U3 | Y2Y3 A2A3
		__m128i vvvvuuuu = _mm_unpacklo_epi32(vvuuyyaa01, vvuuyyaa23); // v0v1v2v3 | u0u1u2u3
		__m128i yyyyaaaa = _mm_unpackhi_epi32(vvuuyyaa01, vvuuyyaa23);

		// we have 2 vectors holding our four 64-bit results (or four 16-bit elements each)
		// We can most efficiently store with VMOVQ and VMOVHPS, even though MOVHPS is "for" FP vectors
		// Further shuffling of another 4 pixels to get 128b vectors wouldn't be a win:
		// MOVHPS is a pure store on Intel CPUs, no shuffle uops.
		// And we have more shuffles than stores already.

		//_mm_storeu_si64(V+pos, vvvvuuuu);  // clang doesn't have this (AVX512?) intrinsic
		_mm_storel_epi64((__m128i*)(V + pos), vvvvuuuu);               // MOVQ
		_mm_storeh_pi((__m64*)(U + pos), _mm_castsi128_ps(vvvvuuuu));  // MOVHPS

		_mm_storel_epi64((__m128i*)(Y + pos), yyyyaaaa);
		_mm_storeh_pi((__m64*)(A + pos), _mm_castsi128_ps(yyyyaaaa));
	}
}

// reviewed 0.5.3
void VideoRenderer::deinterleave(float* pixels, csSDK_int32 rowBytes, char *bufferY, char *bufferU, char *bufferV, char *bufferA)
{
	// Scaling factors (note min. values are actually negative) (limited range)
	const float yuva_factors[4][2] = {
		{ 0.07306f, 1.09132f }, // Y
		{ 0.57143f, 0.57143f }, // U
		{ 0.57143f, 0.57143f }, // V
		{ 0.00000f, 1.00000f }  // A
	};

	// De-Interleave and convert source buffer
	for (int r = height - 1, p = 0; r >= 0; r--)
	{
		for (csSDK_uint32 c = 0; c < width; c++)
		{
			// Get beginning of next block
			const int pos = r * width * 4 + c * 4;

			// VUYA -> YUVA
			((uint16_t*)bufferY)[p] = (uint16_t)((pixels[pos + 2] + yuva_factors[0][0]) / (yuva_factors[0][0] + yuva_factors[0][1]) * 65535.0f);
			((uint16_t*)bufferU)[p] = (uint16_t)((pixels[pos + 1] + yuva_factors[1][0]) / (yuva_factors[1][0] + yuva_factors[1][1]) * 65535.0f);
			((uint16_t*)bufferV)[p] = (uint16_t)((pixels[pos + 0] + yuva_factors[2][0]) / (yuva_factors[2][0] + yuva_factors[2][1]) * 65535.0f);
			((uint16_t*)bufferA)[p] = (uint16_t)((pixels[pos + 3] + yuva_factors[3][0]) / (yuva_factors[3][0] + yuva_factors[3][1]) * 65535.0f);

			p++;
		}
	}
}

// reviewed 0.5.3
prSuiteError VideoRenderer::frameCompleteCallback(const csSDK_uint32 inWhichPass, const csSDK_uint32 inFrameNumber, const csSDK_uint32 inFrameRepeatCount, PPixHand inRenderedFrame, void* inCallbackData)
{
	prSuiteError error = suiteError_NoError;

	tp1 = high_resolution_clock::now();

	// Get pixel format
	PrPixelFormat inFormat;
	ppixSuite->GetPixelFormat(inRenderedFrame, &inFormat);

	// yuv420p
	if (inFormat == PrPixelFormat_YUV_420_MPEG4_FRAME_PICTURE_PLANAR_8u_601 ||
		inFormat == PrPixelFormat_YUV_420_MPEG4_FIELD_PICTURE_PLANAR_8u_601 ||
		inFormat == PrPixelFormat_YUV_420_MPEG4_FRAME_PICTURE_PLANAR_8u_601_FullRange ||
		inFormat == PrPixelFormat_YUV_420_MPEG4_FIELD_PICTURE_PLANAR_8u_601_FullRange ||
		inFormat == PrPixelFormat_YUV_420_MPEG4_FRAME_PICTURE_PLANAR_8u_709 ||
		inFormat == PrPixelFormat_YUV_420_MPEG4_FIELD_PICTURE_PLANAR_8u_709 ||
		inFormat == PrPixelFormat_YUV_420_MPEG4_FRAME_PICTURE_PLANAR_8u_709_FullRange ||
		inFormat == PrPixelFormat_YUV_420_MPEG4_FIELD_PICTURE_PLANAR_8u_709_FullRange)
	{
		EncodingData frameData;
		frameData.pass = inWhichPass + 1;
		frameData.planes = 3;
		frameData.pix_fmt = "yuv420p";

		// Get planar buffers
		ppix2Suite->GetYUV420PlanarBuffers(
			inRenderedFrame,
			PrPPixBufferAccess_ReadOnly,
			&frameData.plane[0],
			(uint32_t*)&frameData.stride[0],
			&frameData.plane[1],
			(uint32_t*)&frameData.stride[1],
			&frameData.plane[2],
			(uint32_t*)&frameData.stride[2]);

		return frameFinished(&frameData, inFormat, inFrameRepeatCount);
	}

	// Get packed rowsize
	csSDK_int32 rowBytes;
	ppixSuite->GetRowBytes(inRenderedFrame, &rowBytes);

	// Get pixels from the renderer
	char *pixels;
	ppixSuite->GetPixels(inRenderedFrame, PrPPixBufferAccess_ReadOnly, &pixels);

	// UYVY
	if (inFormat == PrPixelFormat_UYVY_422_8u_601 ||
		inFormat == PrPixelFormat_UYVY_422_8u_709)
	{
		EncodingData frameData;
		frameData.pass = inWhichPass + 1;
		frameData.planes = 1;
		frameData.pix_fmt = "uyvy422";
		frameData.plane[0] = pixels;
		frameData.stride[0] = rowBytes;

		return frameFinished(&frameData, inFormat, inFrameRepeatCount);
	}

	// YUVA
	if (inFormat == PrPixelFormat_VUYA_4444_8u ||
		inFormat == PrPixelFormat_VUYA_4444_8u_709)
	{
		// Set output format
		encodingData.pass = inWhichPass + 1;
		encodingData.planes = 3;
		encodingData.pix_fmt = "yuv444p";
		encodingData.stride[0] = encodingData.stride[1] = encodingData.stride[2] = rowBytes / 4;

		// Deinterleave vuya
		deinterleave(pixels, rowBytes, encodingData.plane[0], encodingData.plane[1], encodingData.plane[2]);

		return frameFinished(&encodingData, inFormat, inFrameRepeatCount);
	}
	
	// VUYA (> 8bit)
	if (inFormat == PrPixelFormat_VUYA_4444_32f ||
		inFormat == PrPixelFormat_VUYA_4444_32f_709)
	{
		encodingData.pass = inWhichPass + 1;
		encodingData.planes = 4;
		encodingData.pix_fmt = "yuva444p16le";
		encodingData.stride[0] = encodingData.stride[1] = encodingData.stride[2] = rowBytes / encodingData.planes;

		// Deinterlave packed to planar
		deinterleave((float*)pixels, rowBytes, encodingData.plane[0], encodingData.plane[1], encodingData.plane[2], encodingData.plane[3]);

		return frameFinished(&encodingData, inFormat, inFrameRepeatCount);
	}
	
	return suiteError_RenderInvalidPixelFormat;
}

prSuiteError VideoRenderer::frameFinished(EncodingData *frameData, PrPixelFormat inFormat, const csSDK_uint32 inFrameRepeatCount)
{
	tp2 = high_resolution_clock::now();

	// Repeating frames will be rendered only once
	for (csSDK_uint32 i = 0; i < inFrameRepeatCount; i++)
	{
		// Report repeating frames
		exporterUtilitySuite->ReportIntermediateProgressForRepeatedVideoFrame(videoRenderID, 1);

		// Return the frame
		if (!callback(frameData))
		{
			return suiteError_ExporterSuspended;
		}
	}

	tp3 = high_resolution_clock::now();

	stringstream abc;
	abc << "Render: " << ((tp1 - tp4).count() / 1000) << "탎, ";
	abc << "Process: " << ((tp2 - tp1).count() / 1000) << "탎, ";
	abc << "Encoding: " << ((tp3 - tp2).count() / 1000) << "탎, ";
	abc << "Total: " << ((tp3 - tp4).count() / 1000) << "탎\n";
	OutputDebugStringA(abc.str().c_str());
	
	tp4 = high_resolution_clock::now();

	return suiteError_NoError;
}

// reviewed 0.5.2
prSuiteError VideoRenderer::render(PrPixelFormat pixelFormat, PrTime startTime, PrTime endTime, csSDK_uint32 passes, function<bool(EncodingData*)> callback)
{
	this->width = width;
	this->height = height;
	this->callback = callback;

	// Set up render params
	ExportLoopRenderParams renderParams;
	renderParams.inStartTime = startTime;
	renderParams.inEndTime = endTime;
	renderParams.inFinalPixelFormat = pixelFormat;
	renderParams.inRenderParamsSize = sizeof(renderParams);
	renderParams.inRenderParamsVersion = 1;
	renderParams.inReservedProgressPreRender = 0;
	renderParams.inReservedProgressPostRender = 0;

	// Create C conform callback
	Callback<prSuiteError(csSDK_uint32, csSDK_uint32, csSDK_uint32, PPixHand, void*)>::func = bind(&VideoRenderer::frameCompleteCallback, this, placeholders::_1, placeholders::_2, placeholders::_3, placeholders::_4, placeholders::_5);
	PrSDKMultipassExportLoopFrameCompletionFunction c_callback = static_cast<PrSDKMultipassExportLoopFrameCompletionFunction>(Callback<prSuiteError(csSDK_uint32, csSDK_uint32, csSDK_uint32, PPixHand, void*)>::callback);

	// Start encoding loop
	return exporterUtilitySuite->DoMultiPassExportLoop(videoRenderID, &renderParams, passes, c_callback, NULL);
}

PrPixelFormat VideoRenderer::GetTargetRenderFormat(const string pixfmt, ColorSpace colorSpace, ColorRange colorRange, prFieldType fieldType)
{
	PrPixelFormat format = PrPixelFormat_Invalid;

	if (pixfmt == "yuv420p")
	{
		if (fieldType == prFieldsNone)
		{
			if (colorRange == Limited)
			{
				switch (colorSpace)
				{
				case bt601:
					format = PrPixelFormat_YUV_420_MPEG4_FRAME_PICTURE_PLANAR_8u_601;
					break;

				case bt709:
					format = PrPixelFormat_YUV_420_MPEG4_FRAME_PICTURE_PLANAR_8u_709;
					break;
				}
			}
			else if (colorRange == Full)
			{
				switch (colorSpace)
				{
				case bt601:
					format = PrPixelFormat_YUV_420_MPEG4_FRAME_PICTURE_PLANAR_8u_601_FullRange;
					break;

				case bt709:
					format = PrPixelFormat_YUV_420_MPEG4_FRAME_PICTURE_PLANAR_8u_709_FullRange;
					break;
				}
			}
		}
		else
		{
			if (colorRange == Limited)
			{
				switch (colorSpace)
				{
				case bt601:
					format = PrPixelFormat_YUV_420_MPEG4_FIELD_PICTURE_PLANAR_8u_601;
					break;

				case bt709:
					format = PrPixelFormat_YUV_420_MPEG4_FIELD_PICTURE_PLANAR_8u_709;
					break;
				}
			}
			else if (colorRange == Limited)
			{
				switch (colorSpace)
				{
				case bt601:
					format = PrPixelFormat_YUV_420_MPEG4_FIELD_PICTURE_PLANAR_8u_601_FullRange;
					break;

				case bt709:
					format = PrPixelFormat_YUV_420_MPEG4_FIELD_PICTURE_PLANAR_8u_709_FullRange;
					break;
				}
			}
		}
	}
	else if (pixfmt == "yuv422p")
	{
		switch (colorSpace)
		{
		case bt601:
			format = PrPixelFormat_UYVY_422_8u_601;
			break;

		case bt709:
			format = PrPixelFormat_UYVY_422_8u_709;
			break;
		}
	}
	else if (pixfmt == "yuv444p")
	{
		switch (colorSpace)
		{
		case bt601:
			format = PrPixelFormat_VUYA_4444_8u;
			break;

		case bt709:
			format = PrPixelFormat_VUYA_4444_8u_709;
			break;
		}
	}
	else if (pixfmt == "yuv420p10le" ||
		pixfmt == "yuv422p10le" ||
		pixfmt == "yuv444p10le" ||
		pixfmt == "yuv420p12le" ||
		pixfmt == "yuv422p12le" ||
		pixfmt == "yuv444p12le")
	{
		switch (colorSpace)
		{
		case bt601:
			format = PrPixelFormat_VUYA_4444_32f;
			break;

		case bt709:
			format = PrPixelFormat_VUYA_4444_32f_709;
			break;
		}
	}

	return format;
}