#include <tmmintrin.h>
#include <immintrin.h>
#include "VideoRenderer.h"
#include <InstructionSet.h>
#include <Voukoder.h>
#include <Log.h>

static const __m128i unpackMask8 = _mm_set_epi8(
	12, 8, 4, 0, // Y
	13, 9, 5, 1, // U
	14, 10, 6, 2, // V
	15, 11, 7, 3  // A (not needed)
);

static const __m128 scale_mul = _mm_setr_ps(
	57342.98164f,
	57342.98164f,
	56283.17216f,
	65535.0f
);

static const __m128 scale_add = _mm_setr_ps(
	32767.0f,
	32767.0f,
	4112.04855f,
	0.0f
);

VideoRenderer::VideoRenderer(csSDK_uint32 pluginId, csSDK_uint32 width, csSDK_uint32 height, bool maxDepth, PrSuites *suites, std::function<bool(AVFrame *frame, int pass, int render, int process)> callback):
	suites(suites),
	pluginId(pluginId),
	width(width),
	height(height),
	maxDepth(maxDepth),
	callback(callback)
{}

int VideoRenderer::createFrameFromBuffer(const uint8_t *pixels, const int rowBytes, AVFrame &frame)
{
	// Set pixel format
	frame.format = AV_PIX_FMT_YUV444P;

	// Reserve an aligned buffer
	int ret = av_frame_get_buffer(&frame, av_cpu_max_align());
	if (!ret)
	{
		int q = 0;
		for (int r = height - 1; r >= 0; r--)
		{
			const uint8_t *p = &pixels[r * rowBytes];

			for (int c = 0; c < rowBytes; c += 16)
			{
				const __m128i yuva = _mm_loadu_si128((__m128i*)(p + c));
				const __m128i out = _mm_shuffle_epi8(yuva, unpackMask8);
				memcpy(frame.data[0] + q, out.m128i_u8 + 4, 4);
				memcpy(frame.data[1] + q, out.m128i_u8 + 8, 4);
				memcpy(frame.data[2] + q, out.m128i_u8 + 12, 4);
				q += 4;
			}
		}
	}

	return ret;
}

// Thanks to Peter Cordes
static inline __m128i load_and_scale(const float *src, const bool useFMA)
{
	__m128 srcv = _mm_loadu_ps(src);
	__m128 scaled = useFMA ?
		_mm_fmadd_ps(srcv, scale_mul, scale_add) :
		_mm_add_ps(_mm_mul_ps(srcv, scale_mul), scale_add);

	return _mm_cvttps_epi32(scaled);
}

// Thanks to Peter Cordes
int VideoRenderer::createFrameFromBuffer(const float* pixels, AVFrame &frame)
{
	// Set pixel format
	frame.format = AV_PIX_FMT_YUVA444P16;

	// Reserve an aligned buffer
	int ret = av_frame_get_buffer(&frame, av_cpu_max_align());
	if (!ret)
	{
		const bool useFMA = InstructionSet::FMA();

		int q = 0;
		for (int r = height - 1; r >= 0; r--)
		{
			const float* p = &pixels[r * width * 4];

			for (int c = 0; c < (int)width * 4; c += 16)
			{
				__m128i vuya0 = load_and_scale(p + c, useFMA);
				__m128i vuya1 = load_and_scale(p + c + 4, useFMA);
				__m128i vuya2 = load_and_scale(p + c + 8, useFMA);
				__m128i vuya3 = load_and_scale(p + c + 12, useFMA);

				__m128i vuya02 = _mm_packus_epi32(vuya0, vuya2);
				__m128i vuya13 = _mm_packus_epi32(vuya1, vuya3);
				__m128i vvuuyyaa01 = _mm_unpacklo_epi16(vuya02, vuya13);
				__m128i vvuuyyaa23 = _mm_unpackhi_epi16(vuya02, vuya13);
				__m128i vvvvuuuu = _mm_unpacklo_epi32(vvuuyyaa01, vvuuyyaa23);
				__m128i yyyyaaaa = _mm_unpackhi_epi32(vvuuyyaa01, vvuuyyaa23);

				_mm_storel_epi64((__m128i*)(frame.data[2] + q), vvvvuuuu);
				_mm_storeh_pi((__m64*)(frame.data[1] + q), _mm_castsi128_ps(vvvvuuuu));
				_mm_storel_epi64((__m128i*)(frame.data[0] + q), yyyyaaaa);
				_mm_storeh_pi((__m64*)(frame.data[3] + q), _mm_castsi128_ps(yyyyaaaa));

				q += 8;
			}
		}
	}

	return ret;
}

prSuiteError VideoRenderer::render(PrPixelFormat pixelFormat, PrTime startTime, PrTime endTime, csSDK_uint32 passes)
{
	ExportLoopRenderParams renderParams;
	renderParams.inStartTime = startTime;
	renderParams.inEndTime = endTime;
	renderParams.inFinalPixelFormat = pixelFormat;
	renderParams.inRenderParamsSize = sizeof(renderParams);
	renderParams.inRenderParamsVersion = 1;
	renderParams.inReservedProgressPreRender = 0;
	renderParams.inReservedProgressPostRender = 0;

	// Reset presentation timestamp
	pts = 0;

	// Create C conform callback
	Callback<prSuiteError(csSDK_uint32, csSDK_uint32, csSDK_uint32, PPixHand, void*)>::func = std::bind(&VideoRenderer::frameCompleteCallback, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5);
	PrSDKMultipassExportLoopFrameCompletionFunction c_callback = static_cast<PrSDKMultipassExportLoopFrameCompletionFunction>(Callback<prSuiteError(csSDK_uint32, csSDK_uint32, csSDK_uint32, PPixHand, void*)>::callback);

	tp_frameStart = std::chrono::high_resolution_clock::now();

	return suites->exporterUtilitySuite->DoMultiPassExportLoop(pluginId, &renderParams, passes, c_callback, NULL);
}

inline void setColorSpace709(AVFrame *frame, bool is709)
{
	if (is709)
	{
		frame->colorspace = AVColorSpace::AVCOL_SPC_BT709;
		frame->color_primaries = AVColorPrimaries::AVCOL_PRI_BT709;
		frame->color_trc = AVColorTransferCharacteristic::AVCOL_TRC_BT709;
	}
	else
	{
		frame->colorspace = AVColorSpace::AVCOL_SPC_SMPTE170M;
		frame->color_primaries = AVColorPrimaries::AVCOL_PRI_SMPTE170M;
		frame->color_trc = AVColorTransferCharacteristic::AVCOL_TRC_SMPTE170M;
	}
}

prSuiteError VideoRenderer::frameCompleteCallback(const csSDK_uint32 pass, const csSDK_uint32 frameNumber, const csSDK_uint32 frameRepeatCount, PPixHand renderedFrame, void *callbackData)
{
	tp_frameProcess = std::chrono::high_resolution_clock::now();
	int render = (tp_frameProcess - tp_frameStart) / std::chrono::microseconds(1);

	prSuiteError error = suiteError_NoError;

	// Just in case for multipass encoding
	if (pass > currentPass)
	{
		currentPass = pass;
		pts = 0;
	}
	
	// Create frame to store the data
	AVFrame *frame = av_frame_alloc();
	frame->width = width;
	frame->height = height;
	frame->color_range = AVColorRange::AVCOL_RANGE_MPEG;

	// Get pixel format from the first frame
	if (format == PrPixelFormat_Any)
	{
		// Get pixel format
		suites->ppixSuite->GetPixelFormat(renderedFrame, &format);
	}

	// yuv420p
	if (format == PrPixelFormat_YUV_420_MPEG4_FRAME_PICTURE_PLANAR_8u_601 ||
		format == PrPixelFormat_YUV_420_MPEG4_FIELD_PICTURE_PLANAR_8u_601 ||
		format == PrPixelFormat_YUV_420_MPEG4_FRAME_PICTURE_PLANAR_8u_601_FullRange ||
		format == PrPixelFormat_YUV_420_MPEG4_FIELD_PICTURE_PLANAR_8u_601_FullRange ||
		format == PrPixelFormat_YUV_420_MPEG4_FRAME_PICTURE_PLANAR_8u_709 ||
		format == PrPixelFormat_YUV_420_MPEG4_FIELD_PICTURE_PLANAR_8u_709 ||
		format == PrPixelFormat_YUV_420_MPEG4_FRAME_PICTURE_PLANAR_8u_709_FullRange ||
		format == PrPixelFormat_YUV_420_MPEG4_FIELD_PICTURE_PLANAR_8u_709_FullRange)
	{
		frame->format = AV_PIX_FMT_YUV420P;
		
		// Get plane buffers
		suites->ppix2Suite->GetYUV420PlanarBuffers(renderedFrame, PrPPixBufferAccess_ReadOnly,
			(char**)&frame->data[0], (uint32_t*)&frame->linesize[0],
			(char**)&frame->data[1], (uint32_t*)&frame->linesize[1],
			(char**)&frame->data[2], (uint32_t*)&frame->linesize[2]);

		// Color range
		if (format == PrPixelFormat_YUV_420_MPEG4_FRAME_PICTURE_PLANAR_8u_601_FullRange ||
			format == PrPixelFormat_YUV_420_MPEG4_FIELD_PICTURE_PLANAR_8u_601_FullRange ||
			format == PrPixelFormat_YUV_420_MPEG4_FRAME_PICTURE_PLANAR_8u_709_FullRange ||
			format == PrPixelFormat_YUV_420_MPEG4_FIELD_PICTURE_PLANAR_8u_709_FullRange)
		{
			frame->color_range = AVColorRange::AVCOL_RANGE_JPEG;
		}

		// Color space
		setColorSpace709(frame, format == PrPixelFormat_YUV_420_MPEG4_FRAME_PICTURE_PLANAR_8u_709 ||
			format == PrPixelFormat_YUV_420_MPEG4_FIELD_PICTURE_PLANAR_8u_709 ||
			format == PrPixelFormat_YUV_420_MPEG4_FRAME_PICTURE_PLANAR_8u_709_FullRange ||
			format == PrPixelFormat_YUV_420_MPEG4_FIELD_PICTURE_PLANAR_8u_709_FullRange);
	}
	else
	{
		// Get stride
		csSDK_int32 rowBytes;
		suites->ppixSuite->GetRowBytes(renderedFrame, &rowBytes);

		// Get pixel data
		char *pixels;
		suites->ppixSuite->GetPixels(renderedFrame, PrPPixBufferAccess_ReadOnly, &pixels);

		// Fill buffers
		if (format == PrPixelFormat_UYVY_422_8u_601 || format == PrPixelFormat_UYVY_422_8u_709)
		{
			frame->format = AV_PIX_FMT_UYVY422;
			frame->data[0] = (uint8_t*)pixels;
			frame->linesize[0] = rowBytes;
			
			// Color space
			setColorSpace709(frame, format == PrPixelFormat_UYVY_422_8u_709);
		}
		else if (format == PrPixelFormat_VUYA_4444_8u || format == PrPixelFormat_VUYA_4444_8u_709 ||
			format == PrPixelFormat_VUYX_4444_8u || format == PrPixelFormat_VUYX_4444_8u_709)
		{
			createFrameFromBuffer((uint8_t*)pixels, rowBytes, *frame);

			// Color space
			setColorSpace709(frame, format == PrPixelFormat_VUYA_4444_8u_709 || format == PrPixelFormat_VUYX_4444_8u_709);
		}
		else if (format == PrPixelFormat_VUYA_4444_32f || format == PrPixelFormat_VUYA_4444_32f_709 ||
			format == PrPixelFormat_VUYX_4444_32f || format == PrPixelFormat_VUYX_4444_32f_709)
		{
			createFrameFromBuffer((float*)pixels, *frame);

			// Color space
			setColorSpace709(frame, format == PrPixelFormat_VUYA_4444_32f_709 || format == PrPixelFormat_VUYX_4444_32f_709);
		}
		else if (format == PrPixelFormat_BGRA_4444_8u)
		{
			flipImage(pixels, rowBytes);

			frame->format = AV_PIX_FMT_BGRA;
			frame->data[0] = (uint8_t*)pixels;
			frame->linesize[0] = rowBytes;
			frame->color_range = AVColorRange::AVCOL_RANGE_JPEG;
			frame->colorspace = AVColorSpace::AVCOL_SPC_RGB;
			frame->color_primaries = AVColorPrimaries::AVCOL_PRI_UNSPECIFIED;
			frame->color_trc = AVColorTransferCharacteristic::AVCOL_TRC_LINEAR;
		}
		else
		{
			vkdrReturnError(wxString::Format("Unsupported pixel format returned: %#010x", format));
		}
	}

	return frameFinished(frame, pass + 1, frameRepeatCount, render);
}

prSuiteError VideoRenderer::frameFinished(AVFrame *frame, int pass, const csSDK_uint32 inFrameRepeatCount, int render)
{
	int process = (std::chrono::high_resolution_clock::now() - tp_frameProcess) / std::chrono::microseconds(1);

	suites->exporterUtilitySuite->ReportIntermediateProgressForRepeatedVideoFrame(pluginId, 1);

	for (unsigned int r = 0; r < inFrameRepeatCount; r++)
	{
		frame->pts = pts + r;

		if (!callback(frame, pass, render, process))
		{
			return exportReturn_ErrLastErrorSet;
		}
	}

	pts += inFrameRepeatCount;

	av_frame_free(&frame);

	tp_frameStart = std::chrono::high_resolution_clock::now();

	return suiteError_NoError;
}

void VideoRenderer::flipImage(char* pixels, const csSDK_int32 rowBytes)
{
	char *linebuffer = (char*)av_malloc(rowBytes);
	for (unsigned int i = 0; i < height / 2; i++)
	{
		const int p = i * rowBytes;
		const int p2 = (height - 1 - i) * rowBytes;
		memcpy(linebuffer, pixels + p2, rowBytes);
		memcpy(pixels + p2, pixels + p, rowBytes);
		memcpy(pixels + p, linebuffer, rowBytes);
	}
	av_free(linebuffer);
}

PrPixelFormat VideoRenderer::GetTargetRenderFormat(ExportInfo encoderInfo)
{
	// Rendering with max depth
	if (maxDepth)
	{
		vkLogInfo("Rendering with maximum depth requested. Forcing YUVX_4444_32f.");

		return encoderInfo.video.colorSpace == AVColorSpace::AVCOL_SPC_BT709 ? PrPixelFormat_VUYX_4444_32f_709 : PrPixelFormat_VUYX_4444_32f;
	}

	vkLogInfo("Requesting pixel format: %s", av_get_pix_fmt_name(encoderInfo.video.pixelFormat));

	if (encoderInfo.video.pixelFormat == AV_PIX_FMT_YUV420P ||
		encoderInfo.video.pixelFormat == AV_PIX_FMT_NV12 ||
		encoderInfo.video.pixelFormat == AV_PIX_FMT_P010LE) // Not really nice
	{
		if (encoderInfo.video.fieldOrder == AVFieldOrder::AV_FIELD_PROGRESSIVE)
		{
			if (encoderInfo.video.colorSpace == AVColorSpace::AVCOL_SPC_BT709)
			{
				return PrPixelFormat_YUV_420_MPEG4_FRAME_PICTURE_PLANAR_8u_709;
			}

			return PrPixelFormat_YUV_420_MPEG4_FRAME_PICTURE_PLANAR_8u_601;
		}
		else
		{
			if (encoderInfo.video.colorSpace == AVColorSpace::AVCOL_SPC_BT709)
			{
				return PrPixelFormat_YUV_420_MPEG4_FIELD_PICTURE_PLANAR_8u_709;
			}

			return PrPixelFormat_YUV_420_MPEG4_FIELD_PICTURE_PLANAR_8u_601;
		}
	}
	else if (encoderInfo.video.pixelFormat == AV_PIX_FMT_YUV422P)
	{
		if (encoderInfo.video.colorSpace == AVColorSpace::AVCOL_SPC_BT709)
		{
			return PrPixelFormat_UYVY_422_8u_709;
		}

		return PrPixelFormat_UYVY_422_8u_601;
	}
	else if (encoderInfo.video.pixelFormat == AV_PIX_FMT_YUV444P)
	{
		if (encoderInfo.video.colorSpace == AVColorSpace::AVCOL_SPC_BT709)
		{
			return PrPixelFormat_VUYA_4444_8u_709;
		}

		return PrPixelFormat_VUYA_4444_8u;
	}
	else if (encoderInfo.video.pixelFormat == AV_PIX_FMT_BGRA)
	{
		return PrPixelFormat_BGRA_4444_8u;
	}

	return encoderInfo.video.colorSpace == AVColorSpace::AVCOL_SPC_BT709 ? PrPixelFormat_VUYX_4444_32f_709 : PrPixelFormat_VUYX_4444_32f;
}

PrTime VideoRenderer::GetPts()
{
	return pts;
}