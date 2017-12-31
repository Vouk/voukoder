#include "VideoRenderer.h"
#include <tmmintrin.h>

VideoRenderer::VideoRenderer(csSDK_uint32 videoRenderID, csSDK_uint32 width, csSDK_uint32 height, PrPixelFormat pixelFormat, PrSDKPPixSuite *ppixSuite, PrSDKMemoryManagerSuite *memorySuite, PrSDKExporterUtilitySuite *exporterUtilitySuite) :
	videoRenderID(videoRenderID),
	width(width),
	height(height),
	pixelFormat(pixelFormat),
	ppixSuite(ppixSuite),
	memorySuite(memorySuite),
	exporterUtilitySuite(exporterUtilitySuite)
{
	// Reserve max. buffers
	for (int i = 0; i < 4; i++)
	{
		encodingData.plane[i] = (char*)memorySuite->NewPtr(width * height * sizeof(float));
	}
}

VideoRenderer::~VideoRenderer()
{
	// Free buffers
	for (int i = 0; i < 4; i++)
	{
		memorySuite->PrDisposePtr((char*)encodingData.plane[i]);
	}
}

prSuiteError VideoRenderer::deinterleave(PPixHand renderedFrame, char *bufferY, char *bufferU, char *bufferV)
{
	// Get packed rowsize
	csSDK_int32 rowBytes;
	ppixSuite->GetRowBytes(renderedFrame, &rowBytes);

	// Get pixels from the renderer
	char *pixels;
	ppixSuite->GetPixels(renderedFrame, PrPPixBufferAccess_ReadOnly, &pixels);

	// Shuffle mask
	__m128i mask = _mm_set_epi8(
		12, 8, 4, 0, // Y
		13, 9, 5, 1, // U
		14, 10, 6, 2, // V
		15, 11, 7, 3  // A (not needed)
	);

	M128 dest;

	// De-Interleave source buffer
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

	return suiteError_NoError;
}

prSuiteError VideoRenderer::deinterleave(PPixHand renderedFrame, char *bufferY, char *bufferU, char *bufferV, char *bufferA)
{
	// Get packed rowsize
	csSDK_int32 rowBytes;
	ppixSuite->GetRowBytes(renderedFrame, &rowBytes);

	// Get pixels from the renderer
	char *pixels;
	ppixSuite->GetPixels(renderedFrame, PrPPixBufferAccess_ReadOnly, &pixels);

	// Scaling factors (note min. values are actually negative) (limited range)
	const float yuva_factors[4][2] = {
		{ 0.07306f, 1.09132f }, // Y
		{ 0.57143f, 0.57143f }, // U
		{ 0.57143f, 0.57143f }, // V
		{ 0.00000f, 1.00000f }  // A
	};

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

	return suiteError_NoError;
}

prSuiteError FrameCompletionFunction(const csSDK_uint32 inWhichPass, const csSDK_uint32 inFrameNumber, const csSDK_uint32 inFrameRepeatCount, PPixHand inRenderedFrame, void* inCallbackData)
{
	prSuiteError error = suiteError_NoError;

	VideoRenderer *renderer = reinterpret_cast<VideoRenderer*>(inCallbackData);

	// Get pixel format
	PrPixelFormat format;
	error = renderer->ppixSuite->GetPixelFormat(inRenderedFrame, &format);

	// Store pass information
	renderer->encodingData.pass = inWhichPass + 1;

	// Is really everything VUYA_4444 ?
	if (format == PrPixelFormat_VUYA_4444_8u ||
		format == PrPixelFormat_VUYA_4444_8u_709)
	{
		// Set output format
		renderer->encodingData.planes = 3;
		renderer->encodingData.pix_fmt = "yuv444p";

		renderer->deinterleave(inRenderedFrame, renderer->encodingData.plane[0], renderer->encodingData.plane[1], renderer->encodingData.plane[2]);
	}
	else if(format == PrPixelFormat_VUYA_4444_32f ||
		format == PrPixelFormat_VUYA_4444_32f_709)
	{
		// Set output format
		renderer->encodingData.planes = 4;
		renderer->encodingData.pix_fmt = "yuva444p16le";

		renderer->deinterleave(inRenderedFrame, renderer->encodingData.plane[0], renderer->encodingData.plane[1], renderer->encodingData.plane[2], renderer->encodingData.plane[3]);
	}
	else
	{
		return suiteError_RenderInvalidPixelFormat;
	}

	// Repeating frames will be rendered only once
	for (csSDK_uint32 i = 0; i < inFrameRepeatCount; i++)
	{
		// Return the frame
		if (!renderer->callback(renderer->encodingData))
		{
			error = suiteError_ExporterSuspended;
			break;
		}
	}

	return error;
}

prSuiteError VideoRenderer::render(PrTime startTime, PrTime endTime, csSDK_uint32 passes, function<bool(EncodingData)> callback)
{
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

	// Start encoding loop
	return exporterUtilitySuite->DoMultiPassExportLoop(videoRenderID, &renderParams, passes, FrameCompletionFunction, (void *)this);
}