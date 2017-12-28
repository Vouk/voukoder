#include "VideoRenderer.h"
#include <tmmintrin.h>

VideoRenderer::VideoRenderer(csSDK_uint32 videoRenderID, csSDK_uint32 width, csSDK_uint32 height, PrPixelFormat pixelFormat, PrSDKPPixSuite *ppixSuite, PrSDKMemoryManagerSuite *memorySuite, PrSDKExporterUtilitySuite *exporterUtilitySuite) :
	videoRenderID(videoRenderID),
	pixelFormat(pixelFormat),
	ppixSuite(ppixSuite),
	memorySuite(memorySuite),
	exporterUtilitySuite(exporterUtilitySuite)
{
	// Set output format
	encodingData.planes = 3;
	encodingData.pix_fmt = "yuv444p";

	// Reserve buffers
	for (int i = 0; i < encodingData.planes; i++)
	{
		encodingData.plane[i] = (char*)memorySuite->NewPtr(width * height);
	}
}

VideoRenderer::~VideoRenderer()
{
	// Free buffers
	for (int i = 0; i < encodingData.planes; i++)
	{
		memorySuite->PrDisposePtr((char *)encodingData.plane[i]);
	}
}

prSuiteError VideoRenderer::deinterleave(PPixHand renderedFrame, char *bufferY, char *bufferU, char *bufferV)
{
	// Get frame size
	prRect bounds;
	ppixSuite->GetBounds(renderedFrame, &bounds);

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
		15, 11, 7, 3  // A
	);

	M128 dest;

	// De-Interleave source buffer
	for (int r = bounds.bottom - 1, p = 0; r >= bounds.top; r--)
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

prSuiteError FrameCompletionFunction(const csSDK_uint32 inWhichPass, const csSDK_uint32 inFrameNumber, const csSDK_uint32 inFrameRepeatCount, PPixHand inRenderedFrame, void* inCallbackData)
{
	prSuiteError error = suiteError_NoError;

	VideoRenderer *renderer = reinterpret_cast<VideoRenderer*>(inCallbackData);

	// Get pixel format
	PrPixelFormat format;
	error = renderer->ppixSuite->GetPixelFormat(inRenderedFrame, &format);

	// Is really everything VUYA_4444 ?
	if (format == PrPixelFormat_VUYA_4444_8u ||
		format == PrPixelFormat_VUYA_4444_8u_709)
	{
		// Deinterleave the frame
		renderer->deinterleave(inRenderedFrame, renderer->encodingData.plane[0], renderer->encodingData.plane[1], renderer->encodingData.plane[2]);

		// Return the frame
		renderer->callback(renderer->encodingData);
	}
	
	return error;
}

prSuiteError VideoRenderer::renderFrames(PrTime startTime, PrTime endTime, function<void(EncodingData)> callback)
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
	return exporterUtilitySuite->DoMultiPassExportLoop(videoRenderID, &renderParams, 1, FrameCompletionFunction, (void *)this);
}