#include "VideoRenderer.h"
#include <tmmintrin.h>

// reviewed 0.5.2
VideoRenderer::VideoRenderer(csSDK_uint32 videoRenderID, csSDK_uint32 width, csSDK_uint32 height, PrPixelFormat pixelFormat, PrSDKPPixSuite *ppixSuite, PrSDKPPix2Suite *ppix2Suite, PrSDKMemoryManagerSuite *memorySuite, PrSDKExporterUtilitySuite *exporterUtilitySuite, PrSDKImageProcessingSuite *imageProcessingSuite) :
	videoRenderID(videoRenderID),
	width(width),
	height(height),
	pixelFormat(pixelFormat),
	ppixSuite(ppixSuite),
	ppix2Suite(ppix2Suite),
	memorySuite(memorySuite),
	exporterUtilitySuite(exporterUtilitySuite),
	imageProcessingSuite(imageProcessingSuite)
{
	// Get new packed rowsize
	csSDK_int32 destBufferSize;
	imageProcessingSuite->GetSizeForPixelBuffer(pixelFormat, width, height, &destBufferSize);

	// Reserve conversion buffer
	conversionBuffer = (char*)memorySuite->NewPtr(destBufferSize);

	// Reserve max. buffers
	for (int i = 0; i < 4; i++)
	{
		encodingData.plane[i] = (char*)memorySuite->NewPtr(width * height * sizeof(float));
	}
}

// reviewed 0.5.2
VideoRenderer::~VideoRenderer()
{
	// Free buffer
	memorySuite->PrDisposePtr((char*)conversionBuffer);

	// Free buffers
	for (int i = 0; i < 4; i++)
	{
		memorySuite->PrDisposePtr((char*)encodingData.plane[i]);
	}
}

// reviewed 0.5.2
void VideoRenderer::deinterleave(char* pixels, csSDK_int32 rowBytes, char *bufferY, char *bufferU, char *bufferV)
{
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
}

// reviewed 0.5.2
void VideoRenderer::deinterleave(char* pixels, csSDK_int32 rowBytes, char *bufferY, char *bufferU, char *bufferV, char *bufferA)
{
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
		for (csSDK_uint32 c = 0; c < width; c++)
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
}

// reviewed 0.5.2
bool VideoRenderer::isBt709(PrPixelFormat format)
{
	return (format == PrPixelFormat_VUYA_4444_8u_709 ||
		format == PrPixelFormat_VUYX_4444_8u_709 ||
		format == PrPixelFormat_VUYP_4444_8u_709 ||
		format == PrPixelFormat_VUYA_4444_32f_709 ||
		format == PrPixelFormat_VUYX_4444_32f_709 ||
		format == PrPixelFormat_VUYP_4444_32f_709 ||
		format == PrPixelFormat_YUYV_422_8u_709 ||
		format == PrPixelFormat_UYVY_422_8u_709 ||
		format == PrPixelFormat_V210_422_10u_709 ||
		format == PrPixelFormat_YUV_420_MPEG2_FRAME_PICTURE_PLANAR_8u_709 ||
		format == PrPixelFormat_YUV_420_MPEG2_FIELD_PICTURE_PLANAR_8u_709 ||
		format == PrPixelFormat_YUV_420_MPEG2_FRAME_PICTURE_PLANAR_8u_709_FullRange ||
		format == PrPixelFormat_YUV_420_MPEG2_FIELD_PICTURE_PLANAR_8u_709_FullRange ||
		format == PrPixelFormat_YUV_420_MPEG4_FRAME_PICTURE_PLANAR_8u_709 ||
		format == PrPixelFormat_YUV_420_MPEG4_FIELD_PICTURE_PLANAR_8u_709 ||
		format == PrPixelFormat_YUV_420_MPEG4_FRAME_PICTURE_PLANAR_8u_709_FullRange ||
		format == PrPixelFormat_YUV_420_MPEG4_FIELD_PICTURE_PLANAR_8u_709_FullRange);
}

#pragma region StandardVideoRenderer

// reviewed 0.5.2
prSuiteError StandardVideoRenderer::frameCompleteCallback(const csSDK_uint32 inWhichPass, const csSDK_uint32 inFrameNumber, const csSDK_uint32 inFrameRepeatCount, PPixHand inRenderedFrame, void* inCallbackData)
{
	prSuiteError error = suiteError_NoError;

    // Store pass information
	encodingData.pass = inWhichPass + 1;

	// Get pixels from the renderer
	char *pixels;
	error = ppixSuite->GetPixels(inRenderedFrame, PrPPixBufferAccess_ReadOnly, &pixels);

	// Get packed rowsize
	csSDK_int32 rowBytes;
	ppixSuite->GetRowBytes(inRenderedFrame, &rowBytes);

	// Get pixel format
	PrPixelFormat format;
	error = ppixSuite->GetPixelFormat(inRenderedFrame, &format);
	
	// Is really everything VUYA_4444 ?
	if (format == PrPixelFormat_VUYA_4444_8u ||
		format == PrPixelFormat_VUYA_4444_8u_709)
	{
		// Set output format
		encodingData.planes = 3;
		encodingData.pix_fmt = "yuv444p";

		deinterleave(pixels, rowBytes, encodingData.plane[0], encodingData.plane[1], encodingData.plane[2]);
	}
	else if(format == PrPixelFormat_VUYA_4444_32f ||
		format == PrPixelFormat_VUYA_4444_32f_709)
	{
		// Set output format
		encodingData.planes = 4;
		encodingData.pix_fmt = "yuva444p16le";

		deinterleave(pixels, rowBytes, encodingData.plane[0], encodingData.plane[1], encodingData.plane[2], encodingData.plane[3]);
	}
	else
	{
		return suiteError_RenderInvalidPixelFormat;
	}

	// Repeating frames will be rendered only once
	for (csSDK_uint32 i = 0; i < inFrameRepeatCount; i++)
	{
		// Report repeating frames
		exporterUtilitySuite->ReportIntermediateProgressForRepeatedVideoFrame(videoRenderID, 1);

		// Return the frame
		if (!callback(encodingData))
		{
			error = suiteError_ExporterSuspended;
			break;
		}
	}

	return error;
}

// reviewed 0.5.2
prSuiteError StandardVideoRenderer::render(PrTime startTime, PrTime endTime, csSDK_uint32 passes, function<bool(EncodingData)> callback)
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

	// Create C conform callback
	Callback<prSuiteError(csSDK_uint32, csSDK_uint32, csSDK_uint32, PPixHand, void*)>::func = bind(&StandardVideoRenderer::frameCompleteCallback, this, placeholders::_1, placeholders::_2, placeholders::_3, placeholders::_4, placeholders::_5);
	PrSDKMultipassExportLoopFrameCompletionFunction c_callback = static_cast<PrSDKMultipassExportLoopFrameCompletionFunction>(Callback<prSuiteError(csSDK_uint32, csSDK_uint32, csSDK_uint32, PPixHand, void*)>::callback);

	// Start encoding loop
	return exporterUtilitySuite->DoMultiPassExportLoop(videoRenderID, &renderParams, passes, c_callback, NULL);
}

#pragma endregion

#pragma region AccurateVideoRenderer

// reviewed 0.5.2
prSuiteError AccurateVideoRenderer::frameCompleteCallback(const csSDK_uint32 inWhichPass, const csSDK_uint32 inFrameNumber, const csSDK_uint32 inFrameRepeatCount, PPixHand inRenderedFrame, void* inCallbackData)
{
	prSuiteError error = suiteError_NoError;

	// Store pass information
	encodingData.pass = inWhichPass + 1;
	encodingData.filters.scale = "";

	// Get pixels from the renderer
	char *pixels;
	ppixSuite->GetPixels(inRenderedFrame, PrPPixBufferAccess_ReadOnly, &pixels);

	// Get packed rowsize
	csSDK_int32 rowBytes;
	ppixSuite->GetRowBytes(inRenderedFrame, &rowBytes);

	// Get pixel format
	PrPixelFormat format;
	ppixSuite->GetPixelFormat(inRenderedFrame, &format);

	// Read lossless formats
	if (format == PrPixelFormat_UYVY_422_8u_601)
	{
		encodingData.planes = 1;
		encodingData.pix_fmt = "uyvy422";
		encodingData.plane[0] = pixels;

		// Color matrix conversion
		if (isBt709(pixelFormat))
		{
			encodingData.filters.scale = "in_color_matrix=bt601:out_color_matrix=bt709";
		}
	}	
	else if (format == PrPixelFormat_UYVY_422_8u_709)
	{
		encodingData.planes = 1;
		encodingData.pix_fmt = "uyvy422";
		encodingData.plane[0] = pixels;

		// Color matrix conversion
		if (!isBt709(pixelFormat))
		{
			encodingData.filters.scale = "in_color_matrix=bt709:out_color_matrix=bt601";
		}
	}
	else if (format == PrPixelFormat_YUYV_422_8u_601)
	{
		encodingData.planes = 1;
		encodingData.pix_fmt = "yuyv422";
		encodingData.plane[0] = pixels;

		// Color matrix conversion
		if (isBt709(pixelFormat))
		{
			encodingData.filters.scale = "in_color_matrix=bt601:out_color_matrix=bt709";
		}
	}
	else if (format == PrPixelFormat_YUYV_422_8u_709)
	{
		encodingData.planes = 1;
		encodingData.pix_fmt = "yuyv422";
		encodingData.plane[0] = pixels;

		// Color matrix conversion
		if (!isBt709(pixelFormat))
		{
			encodingData.filters.scale = "in_color_matrix=bt709:out_color_matrix=bt601";
		}
	}
	else if (format == PrPixelFormat_VUYA_4444_8u ||
		format == PrPixelFormat_VUYX_4444_8u)
	{
		// Set output format
		encodingData.planes = 3;
		encodingData.pix_fmt = "yuv444p";

		// Color matrix conversion
		if (isBt709(pixelFormat))
		{
			encodingData.filters.scale = "in_color_matrix=bt601:out_color_matrix=bt709";
		}

		deinterleave(pixels, rowBytes, encodingData.plane[0], encodingData.plane[1], encodingData.plane[2]);
	}
	else if (format == PrPixelFormat_VUYX_4444_8u_709 ||
		format == PrPixelFormat_VUYA_4444_8u_709)
	{
		// Set output format
		encodingData.planes = 3;
		encodingData.pix_fmt = "yuv444p";

		// Color matrix conversion
		if (!isBt709(pixelFormat))
		{
			encodingData.filters.scale = "in_color_matrix=bt709:out_color_matrix=bt601";
		}

		deinterleave(pixels, rowBytes, encodingData.plane[0], encodingData.plane[1], encodingData.plane[2]);
	}
	else if (format == PrPixelFormat_VUYA_4444_32f ||
		format == PrPixelFormat_VUYX_4444_32f)
	{
		// Set output format
		encodingData.planes = 4;
		encodingData.pix_fmt = "yuva444p16le";

		// Color matrix conversion
		if (isBt709(pixelFormat))
		{
			encodingData.filters.scale = "in_color_matrix=bt601:out_color_matrix=bt709";
		}

		deinterleave(pixels, rowBytes, encodingData.plane[0], encodingData.plane[1], encodingData.plane[2], encodingData.plane[3]);
	}
	else if (format == PrPixelFormat_VUYA_4444_32f_709 ||
		format == PrPixelFormat_VUYX_4444_32f_709)
	{
		// Set output format
		encodingData.planes = 4;
		encodingData.pix_fmt = "yuva444p16le";

		// Color matrix conversion
		if (!isBt709(pixelFormat))
		{
			encodingData.filters.scale = "in_color_matrix=bt709:out_color_matrix=bt601";
		}

		deinterleave(pixels, rowBytes, encodingData.plane[0], encodingData.plane[1], encodingData.plane[2], encodingData.plane[3]);
	}
	else if (format == PrPixelFormat_YUV_420_MPEG4_FRAME_PICTURE_PLANAR_8u_601 ||
		format == PrPixelFormat_YUV_420_MPEG4_FIELD_PICTURE_PLANAR_8u_601 ||
		format == PrPixelFormat_YUV_420_MPEG4_FRAME_PICTURE_PLANAR_8u_601_FullRange ||
		format == PrPixelFormat_YUV_420_MPEG4_FIELD_PICTURE_PLANAR_8u_601_FullRange)
	{
		encodingData.planes = 3;
		encodingData.pix_fmt = "yuv420p";

		// Get planar buffers
		error = ppix2Suite->GetYUV420PlanarBuffers(
			inRenderedFrame,
			PrPPixBufferAccess_ReadOnly,
			&encodingData.plane[0],
			&encodingData.stride[0],
			&encodingData.plane[1],
			&encodingData.stride[1],
			&encodingData.plane[2],
			&encodingData.stride[2]);

		// Color matrix conversion
		if (isBt709(pixelFormat))
		{
			encodingData.filters.scale = "in_color_matrix=bt601:out_color_matrix=bt709";
		}
	}
	else if (format == PrPixelFormat_YUV_420_MPEG4_FRAME_PICTURE_PLANAR_8u_709 ||
		format == PrPixelFormat_YUV_420_MPEG4_FIELD_PICTURE_PLANAR_8u_709 ||
		format == PrPixelFormat_YUV_420_MPEG4_FRAME_PICTURE_PLANAR_8u_709_FullRange ||
		format == PrPixelFormat_YUV_420_MPEG4_FIELD_PICTURE_PLANAR_8u_709_FullRange)
	{
		encodingData.planes = 3;
		encodingData.pix_fmt = "yuv420p";

		// Get planar buffers
		error = ppix2Suite->GetYUV420PlanarBuffers(
			inRenderedFrame,
			PrPPixBufferAccess_ReadOnly,
			&encodingData.plane[0],
			&encodingData.stride[0],
			&encodingData.plane[1],
			&encodingData.stride[1],
			&encodingData.plane[2],
			&encodingData.stride[2]);

		// Color matrix conversion
		if (!isBt709(pixelFormat))
		{
			encodingData.filters.scale = "in_color_matrix=bt709:out_color_matrix=bt601";
		}
	}
	else // Fallback: This is really slow!
	{
		// Get packed rowsize
		csSDK_int32 rowBytes;
		ppixSuite->GetRowBytes(inRenderedFrame, &rowBytes);

		// Get new packed rowsize
		csSDK_int32 destRowBytes;
		imageProcessingSuite->GetSizeForPixelBuffer(pixelFormat, width, 1, &destRowBytes);

		// Convert frame
		error = imageProcessingSuite->ScaleConvert(format, width, height, rowBytes, prFieldsNone, pixels,
			pixelFormat, width, height, destRowBytes, prFieldsNone, conversionBuffer,
			kPrRenderQuality_High);
		if (error != suiteError_NoError)
		{
			return error;
		}

		// Handle pixel format
		if (pixelFormat == PrPixelFormat_VUYA_4444_8u ||
			pixelFormat == PrPixelFormat_VUYA_4444_8u_709)
		{
			// Set output format
			encodingData.planes = 3;
			encodingData.pix_fmt = "yuv444p";

			deinterleave(conversionBuffer, destRowBytes, encodingData.plane[0], encodingData.plane[1], encodingData.plane[2]);
		}
		else if (pixelFormat == PrPixelFormat_VUYA_4444_32f ||
			pixelFormat == PrPixelFormat_VUYA_4444_32f_709)
		{
			// Set output format
			encodingData.planes = 4;
			encodingData.pix_fmt = "yuva444p16le";

			deinterleave(conversionBuffer, destRowBytes, encodingData.plane[0], encodingData.plane[1], encodingData.plane[2], encodingData.plane[3]);
		}
		else
		{
			return suiteError_RenderInvalidPixelFormat;
		}
	}

	// Repeating frames will be rendered only once
	for (csSDK_uint32 i = 0; i < inFrameRepeatCount; i++)
	{
		// Report repeating frames
		exporterUtilitySuite->ReportIntermediateProgressForRepeatedVideoFrame(videoRenderID, 1);

		// Return the frame
		if (!callback(encodingData))
		{
			error = suiteError_ExporterSuspended;
			break;
		}
	}

	return error;
}

// reviewed 0.5.2
prSuiteError AccurateVideoRenderer::render(PrTime startTime, PrTime endTime, csSDK_uint32 passes, function<bool(EncodingData)> callback)
{
	this->callback = callback;

	// Set up render params
	ExportLoopRenderParams renderParams;
	renderParams.inStartTime = startTime;
	renderParams.inEndTime = endTime;
	renderParams.inFinalPixelFormat = PrPixelFormat_Any;
	renderParams.inRenderParamsSize = sizeof(renderParams);
	renderParams.inRenderParamsVersion = 1;
	renderParams.inReservedProgressPreRender = 0;
	renderParams.inReservedProgressPostRender = 0;
	
	// Create C conform callback
	Callback<prSuiteError(csSDK_uint32, csSDK_uint32, csSDK_uint32, PPixHand, void*)>::func = bind(&AccurateVideoRenderer::frameCompleteCallback, this, placeholders::_1, placeholders::_2, placeholders::_3, placeholders::_4, placeholders::_5);
	PrSDKMultipassExportLoopFrameCompletionFunction c_callback = static_cast<PrSDKMultipassExportLoopFrameCompletionFunction>(Callback<prSuiteError(csSDK_uint32, csSDK_uint32, csSDK_uint32, PPixHand, void*)>::callback);

	// Start encoding loop
	return exporterUtilitySuite->DoMultiPassExportLoop(videoRenderID, &renderParams, passes, c_callback, NULL);
}

#pragma endregion