#include "AfterEffectsPlugin.h"
#include "AEGP_SuiteHandler.cpp"
#include "MissingSuiteError.cpp"
#include <wx/wx.h>
#include <wxVoukoderDialog.h>
#include <EncoderEngine.h>
#include <Log.h>

#ifdef _WIN32
#include <Windows.h>
#endif

#ifdef _DEBUG
static inline void AvCallback(void*, int level, const char* szFmt, va_list varg)
{
	char logbuf[2000];
	vsnprintf(logbuf, sizeof(logbuf), szFmt, varg);
	logbuf[sizeof(logbuf) - 1] = '\0';

	OutputDebugStringA(logbuf);
}
#endif

static EncoderEngine* encoderEngine;
static std::chrono::time_point<std::chrono::steady_clock> tp_exportStart;
static std::chrono::time_point<std::chrono::steady_clock> tp_framePrev;
static int frame_count;

class actctx_activator
{
protected:
	ULONG_PTR m_cookie;

public:
	actctx_activator(_In_ HANDLE hActCtx)
	{
		if (!ActivateActCtx(hActCtx, &m_cookie))
			m_cookie = 0;
	}

	virtual ~actctx_activator()
	{
		if (m_cookie)
			DeactivateActCtx(0, m_cookie);
	}
};

HINSTANCE g_instance = NULL;
HANDLE g_act_ctx = NULL;

BOOL WINAPI DllMain(_In_ HINSTANCE hinstDLL, _In_ DWORD fdwReason, _In_ LPVOID lpvReserved)
{
	UNREFERENCED_PARAMETER(lpvReserved);

	if (fdwReason == DLL_PROCESS_ATTACH)
	{
		g_instance = hinstDLL;
		GetCurrentActCtx(&g_act_ctx);
	}
	else if (fdwReason == DLL_PROCESS_DETACH)
	{
		if (g_act_ctx)
			ReleaseActCtx(g_act_ctx);
	}

	return TRUE;
}

AEGP_PluginID S_mem_id = 0;

static A_Err DeathHook(AEGP_GlobalRefcon unused1, AEGP_DeathRefcon unused2)
{
	return A_Err_NONE;
}

static A_Err AEIO_ConstructModuleInfo(AEIO_ModuleInfo *info)
{
	A_Err err = A_Err_NONE;

	if (info)
	{
		auto muxerInfos = Voukoder::Config::Get().muxerInfos;

		info->sig = 'VKDR';
		info->max_width = 8192;
		info->max_height = 4320;
		info->num_filetypes = 1;
		info->num_extensions = muxerInfos.size();
		info->num_clips = 0;
		info->num_aux_extensionsS = 0;

		info->flags = AEIO_MFlag_OUTPUT |
			AEIO_MFlag_FILE |
			AEIO_MFlag_VIDEO |
			//AEIO_MFlag_AUDIO |
			AEIO_MFlag_NO_TIME;

		info->create_kind.type = 'VKDR';
		info->create_kind.creator = 'VOUK';

		// Set plugin name
		strcpy_s(info->name, (wxString)VKDR_APPNAME);

		//
		info->read_kinds[0].mac.type = 'VKDR';
		info->read_kinds[0].mac.creator = AEIO_ANY_CREATOR;

		// Register all extensions
		for (int i = 0; i < muxerInfos.size(); i++)
		{
			// Add extension
			info->read_kinds[i + 1].ext.pad = '.';
			for (int j = 0; j < 3; j++)
				info->read_kinds[i + 1].ext.extension[j] = muxerInfos[i].extension.at(j);

			// Set default extension
			if (muxerInfos[i].id == DefaultMuxer)
				info->create_ext = info->read_kinds[i + 1].ext;
		}
	}
	else
	{
		err = A_Err_STRUCT;
	}

	return err;
}

static A_Err My_InitOutputSpec(AEIO_BasicData *basic_dataP, AEIO_OutSpecH outH, A_Boolean *user_interacted)
{
	A_Err err = A_Err_NONE;

	AEGP_SuiteHandler suites(basic_dataP->pica_basicP);

	// Make new options handle
	ArbData* newArb = NULL;
	AEGP_MemSize newSize;
	AEIO_Handle newOptionsH = NULL;
	suites.MemorySuite1()->AEGP_NewMemHandle(S_mem_id, "ARB Data", sizeof(ArbData), AEGP_MemFlag_CLEAR, &newOptionsH);
	err = suites.MemorySuite1()->AEGP_GetMemHandleSize(newOptionsH, &newSize);
	err = suites.MemorySuite1()->AEGP_LockMemHandle(newOptionsH, (void**)& newArb);

	// Existing options handle
	AEIO_Handle optionsH = NULL;
	suites.IOOutSuite4()->AEGP_GetOutSpecOptionsHandle(outH, (void**)& optionsH);

	// Do we have saved data?
	if (optionsH)
	{
		AEGP_MemSize size;
		ERR(suites.MemorySuite1()->AEGP_GetMemHandleSize(optionsH, &size));
		
		// Did the data structure increase in size?
		if (newSize > size)
		{
			suites.MemorySuite1()->AEGP_UnlockMemHandle(optionsH);
			suites.MemorySuite1()->AEGP_ResizeMemHandle("Old Handle Resize", size, optionsH);
		}

		// Get existing data
		ArbData* arb = NULL;
		suites.MemorySuite1()->AEGP_LockMemHandle(optionsH, (void**)& arb);

		// Copy existing data to new data
		memcpy((char*)newArb, (char*)arb, newSize);

		suites.MemorySuite1()->AEGP_UnlockMemHandle(optionsH);
	}
	else
	{
		// Set default values as initial values
		memset(newArb, 0, sizeof(ArbData));
		strcpy_s(newArb->audioCodecId, DefaultAudioEncoder);
		strcpy_s(newArb->videoCodecId, DefaultVideoEncoder);
		strcpy_s(newArb->formatId, DefaultMuxer);
	}

	suites.MemorySuite1()->AEGP_UnlockMemHandle(newOptionsH);

	// Set the options handle
	AEIO_Handle oldOptionsH = NULL;
	suites.IOOutSuite4()->AEGP_SetOutSpecOptionsHandle(outH, (void*)newOptionsH, (void**)&oldOptionsH);
	if (oldOptionsH)
	{
		suites.MemorySuite1()->AEGP_FreeMemHandle(oldOptionsH);
	}

	return err;
}


// TODO: Check if it is necessary
static A_Err My_GetFlatOutputOptions(AEIO_BasicData	*basic_dataP, AEIO_OutSpecH outH, AEIO_Handle *new_optionsPH)
{
	A_Err err = A_Err_NONE;

	AEIO_Handle old_optionsH = NULL;
	ArbData	*new_optionsP = NULL,
		*old_optionsP = NULL;
	AEGP_SuiteHandler			suites(basic_dataP->pica_basicP);

	ERR(suites.IOOutSuite4()->AEGP_GetOutSpecOptionsHandle(outH, reinterpret_cast<void**>(&old_optionsH)));

	if (!err && old_optionsH) {
		ERR(suites.MemorySuite1()->AEGP_NewMemHandle(S_mem_id,
			"flat optionsH",
			sizeof(ArbData),
			AEGP_MemFlag_CLEAR,
			new_optionsPH));
		if (!err && *new_optionsPH) {
			ERR(suites.MemorySuite1()->AEGP_LockMemHandle(*new_optionsPH, reinterpret_cast<void**>(&new_optionsP)));
			ERR(suites.MemorySuite1()->AEGP_LockMemHandle(old_optionsH, reinterpret_cast<void**>(&old_optionsP)));

			if (!err && new_optionsP && old_optionsP) {
				// Convert the old unflat structure into a separate flat structure for output
				// In this case, we just do a simple copy and set the isFlat flag
				memcpy(new_optionsP, old_optionsP, sizeof(ArbData));
				//new_optionsP->isFlat = TRUE;

				ERR(suites.MemorySuite1()->AEGP_UnlockMemHandle(*new_optionsPH));
				ERR(suites.MemorySuite1()->AEGP_UnlockMemHandle(old_optionsH));
			}
		}
	}
	return err;
}

static A_Err My_DisposeOutputOptions(AEIO_BasicData *basic_dataP, void *optionsPV)
{
	A_Err err = A_Err_NONE;

	AEGP_SuiteHandler suites(basic_dataP->pica_basicP);

	// Dispose options if we have any
	AEIO_Handle optionsH = reinterpret_cast<AEIO_Handle>(optionsPV);
	if (optionsH)
	{
		ERR(suites.MemorySuite1()->AEGP_FreeMemHandle(optionsH));
	}

	return err;
}

static int ShowVoukoderDialog(ExportInfo &exportInfo)
{
	// Restore plugin's activation context.
	actctx_activator actctx(g_act_ctx);

	// Initialize application.
	new wxApp();
	wxEntryStart(g_instance);

	int result;

	// Have an own scope
	{
		HWND hwnd = GetActiveWindow();

		// Create wxWidget-approved parent window.
		wxWindow parent;
		parent.SetHWND((WXHWND)hwnd);
		parent.AdoptAttributesFromHWND();
		wxTopLevelWindows.Append(&parent);

		// Create and launch configuration dialog.
		wxVoukoderDialog dialog(&parent, exportInfo);
		result = dialog.ShowModal();

		wxTopLevelWindows.DeleteObject(&parent);
		parent.SetHWND((WXHWND)NULL);
	}

	// Clean-up and return.
	wxEntryCleanup();

	return result;
}

static A_Err My_UserOptionsDialog(AEIO_BasicData *basic_dataP, AEIO_OutSpecH outH, const PF_EffectWorld *sample0, A_Boolean *user_interacted0)
{
	A_Err err = A_Err_NONE;

	AEGP_SuiteHandler suites(basic_dataP->pica_basicP);

	// Get ARB data
	AEIO_Handle optionsH = NULL, old_optionsH = 0;
	ArbData *arbData = NULL;
	ERR(suites.IOOutSuite4()->AEGP_GetOutSpecOptionsHandle(outH, reinterpret_cast<void**>(&optionsH)));
	ERR(suites.MemorySuite1()->AEGP_LockMemHandle(optionsH, reinterpret_cast<void**>(&arbData)));
	ERR(suites.MemorySuite1()->AEGP_UnlockMemHandle(optionsH));

	// Has video?
	A_long widthL = 0, heightL = 0;
	ERR(suites.IOOutSuite4()->AEGP_GetOutSpecDimensions(outH, &widthL, &heightL));

	// Has audio?
	A_FpLong soundRateF = 0.0;
	ERR(suites.IOOutSuite4()->AEGP_GetOutSpecSoundRate(outH, &soundRateF));

	// Fill export data
	ExportInfo exportInfo;
	exportInfo.video.enabled = (widthL > 0 && heightL > 0);
	exportInfo.video.id = arbData->videoCodecId;
	exportInfo.video.options.Deserialize(arbData->videoCodecOptions);
	exportInfo.audio.enabled = soundRateF > 0;
	exportInfo.audio.id = arbData->audioCodecId;
	exportInfo.audio.options.Deserialize(arbData->audioCodecOptions);
	exportInfo.format.id = arbData->formatId;
	exportInfo.format.faststart = arbData->faststart;
	exportInfo.video.filters.Deserialize(arbData->videoFilters);
	exportInfo.audio.filters.Deserialize(arbData->audioFilters);
	exportInfo.video.sideData.Deserialize(arbData->videoSideData);
	exportInfo.audio.sideData.Deserialize(arbData->audioSideData);


	// Show voukoder dialog
	int result = ShowVoukoderDialog(exportInfo);

	// On OK click
	if (result == (int)wxID_OK)
	{
		*user_interacted0 = true;

		// Fill ArbData
		arbData->version = ARB_VERSION;
		strcpy_s(arbData->videoCodecId, exportInfo.video.id);
		strcpy_s(arbData->videoCodecOptions, exportInfo.video.options.Serialize().c_str());
		strcpy_s(arbData->audioCodecId, exportInfo.audio.id);
		strcpy_s(arbData->audioCodecOptions, exportInfo.audio.options.Serialize().c_str());
		strcpy_s(arbData->formatId, exportInfo.format.id);
		arbData->faststart = exportInfo.format.faststart;
		strcpy_s(arbData->videoSideData, exportInfo.video.sideData.Serialize().c_str());
		strcpy_s(arbData->audioSideData, exportInfo.audio.sideData.Serialize().c_str());
		strcpy_s(arbData->videoFilters, exportInfo.video.filters.Serialize().c_str());
		strcpy_s(arbData->audioFilters, exportInfo.audio.filters.Serialize().c_str());

		// Store configuration
		ERR(suites.IOOutSuite4()->AEGP_SetOutSpecOptionsHandle(outH, optionsH, reinterpret_cast<void**>(&old_optionsH)));
	}

	return err;
}

static A_Err My_GetOutputInfo(AEIO_BasicData *basic_dataP, AEIO_OutSpecH outH, AEIO_Verbiage *verbiageP)
{
	A_Err err = A_Err_NONE;
	
	AEGP_SuiteHandler suites(basic_dataP->pica_basicP);

	// Set infos
	suites.ANSICallbacksSuite1()->strcpy(verbiageP->name, GetFileName(basic_dataP, outH));
	suites.ANSICallbacksSuite1()->strcpy(verbiageP->type, (wxString)VKDR_APPNAME);
	verbiageP->sub_type[0] = '\0';

	return err;
}

static A_Err
My_OutputInfoChanged(
	AEIO_BasicData		*basic_dataP,
	AEIO_OutSpecH		outH)
{
	/*	This function is called when either the user
		or the plug-in has changed the output options info.
		You may want to update your plug-in's internal
		representation of the output at this time.
		We've exercised the likely getters below.
	*/

	A_Err err = A_Err_NONE;

	AEIO_AlphaLabel	alpha;
	AEFX_CLR_STRUCT(alpha);

	FIEL_Label		fields;
	AEFX_CLR_STRUCT(fields);

	A_short			depthS = 0;
	A_Time			durationT = { 0,1 };

	A_Fixed			native_fps = 0;
	A_Ratio			hsf = { 1,1 };
	A_Boolean		is_missingB = TRUE;

	AEGP_SuiteHandler	suites(basic_dataP->pica_basicP);

	ERR(suites.IOOutSuite4()->AEGP_GetOutSpecIsMissing(outH, &is_missingB));

	if (!is_missingB)
	{
		// Find out all the details of the output spec; update
		// your options data as necessary.
		ERR(suites.IOOutSuite4()->AEGP_GetOutSpecAlphaLabel(outH, &alpha));
		ERR(suites.IOOutSuite4()->AEGP_GetOutSpecDepth(outH, &depthS));
		ERR(suites.IOOutSuite4()->AEGP_GetOutSpecInterlaceLabel(outH, &fields));
		ERR(suites.IOOutSuite4()->AEGP_GetOutSpecDuration(outH, &durationT));
		ERR(suites.IOOutSuite4()->AEGP_GetOutSpecFPS(outH, &native_fps));
		ERR(suites.IOOutSuite4()->AEGP_GetOutSpecHSF(outH, &hsf));
	}
	return err;
}

static A_Err My_SetOutputFile(AEIO_BasicData *basic_dataP, AEIO_OutSpecH outH, const A_UTF16Char *file_pathZ)
{
	return AEIO_Err_USE_DFLT_CALLBACK;
}

static wxString GetFileName(AEIO_BasicData* basic_dataP, AEIO_OutSpecH outH)
{
	AEGP_SuiteHandler suites(basic_dataP->pica_basicP);

	AEGP_MemHandle file_pathH = NULL;
	A_Boolean file_reservedB = FALSE;
	A_UTF16Char* file_pathZ = NULL;
	suites.IOOutSuite4()->AEGP_GetOutSpecFilePath(outH, &file_pathH, &file_reservedB);
	suites.MemorySuite1()->AEGP_LockMemHandle(file_pathH, reinterpret_cast<void**>(&file_pathZ));
	suites.MemorySuite1()->AEGP_UnlockMemHandle(file_pathH);
	suites.MemorySuite1()->AEGP_FreeMemHandle(file_pathH);
	
	return (wchar_t*)file_pathZ;
}

static A_Err My_StartAdding(AEIO_BasicData *basic_dataP, AEIO_OutSpecH outH, A_long flags)
{
	A_Err err = A_Err_NONE;
	AEGP_SuiteHandler suites(basic_dataP->pica_basicP);
	
	// Get stored voukoder config
	AEIO_Handle optionsH = NULL;
	ArbData* arbData = NULL;
	ERR(suites.IOOutSuite4()->AEGP_GetOutSpecOptionsHandle(outH, reinterpret_cast<void**>(&optionsH)));
	ERR(suites.MemorySuite1()->AEGP_LockMemHandle(optionsH, reinterpret_cast<void**>(&arbData)));
	ERR(suites.MemorySuite1()->AEGP_UnlockMemHandle(optionsH));
	
	// Create export information
	ExportInfo exportInfo;
	exportInfo.application = VKDR_APPNAME;
	exportInfo.filename = GetFileName(basic_dataP, outH);
	exportInfo.format.id = arbData->formatId;
	exportInfo.format.faststart = arbData->faststart;

	// Handle two pass encoding
	exportInfo.passes = 1;
	if (exportInfo.video.options.find("_2pass") != exportInfo.video.options.end())
	{
		if (exportInfo.video.options.at("_2pass") == "1")
			exportInfo.passes = 2;
	}

	// ### Video settings ###
	exportInfo.video.id = arbData->videoCodecId;
	exportInfo.video.options.Deserialize(arbData->videoCodecOptions);

	// Video dimensions
	A_long widthL = 0, heightL = 0;
	suites.IOOutSuite4()->AEGP_GetOutSpecDimensions(outH, &widthL, &heightL);
	exportInfo.video.width = widthL;
	exportInfo.video.height = heightL;

	// Is video enabled?
	exportInfo.video.enabled = widthL > 0 && heightL > 0;

	// Video timebase
	A_Fixed fps = 0;
	ERR(suites.IOOutSuite4()->AEGP_GetOutSpecFPS(outH, &fps));
	exportInfo.video.timebase = { 65536, (int)fps };
	
	// Video interlace mode
	FIEL_Label fields;
	AEFX_CLR_STRUCT(fields);
	ERR(suites.IOOutSuite4()->AEGP_GetOutSpecInterlaceLabel(outH, &fields));
	if (fields.type == FIEL_Type_FRAME_RENDERED)
	{
		exportInfo.video.fieldOrder = AV_FIELD_PROGRESSIVE;
	}
	else
	{
		exportInfo.video.fieldOrder = fields.order == FIEL_Order_LOWER_FIRST ? AV_FIELD_BB : AV_FIELD_TT;
	}

	// Set pixel format
	if (exportInfo.video.options.find("_pixelFormat") != exportInfo.video.options.end())
	{
		exportInfo.video.pixelFormat = av_get_pix_fmt(exportInfo.video.options.at("_pixelFormat").c_str());
	}
	else
	{
		// Find default pixel format
		AVCodec* codec = avcodec_find_encoder_by_name(exportInfo.video.id);
		if (codec != NULL)
		{
			exportInfo.video.pixelFormat = codec->pix_fmts[0];
		}
	}

	// Get configuration
	exportInfo.video.sampleAspectRatio = { 1, 1 };
	exportInfo.video.colorRange = AVCOL_RANGE_MPEG;
	exportInfo.video.colorSpace = AVCOL_SPC_BT709;
	exportInfo.video.colorPrimaries = AVCOL_PRI_BT709;
	exportInfo.video.colorTransferCharacteristics = AVCOL_TRC_BT709;

	// ### Audio settings ###
	exportInfo.audio.id = arbData->audioCodecId;
	exportInfo.audio.options.Deserialize(arbData->audioCodecOptions);

	// Get audio timebase
	A_FpLong soundRateF = 0.0;
	suites.IOOutSuite4()->AEGP_GetOutSpecSoundRate(outH, &soundRateF);
	exportInfo.audio.timebase = { 1, (int)soundRateF };

	// Is audio enabled?
	exportInfo.audio.enabled = exportInfo.audio.timebase.den > 0;
	
	// Audio channels
	AEIO_SndChannels channels; 
	suites.IOOutSuite4()->AEGP_GetOutSpecSoundChannels(outH, &channels);
	if (channels == AEIO_SndChannels_MONO)
	{
		exportInfo.audio.channelLayout = AV_CH_LAYOUT_MONO;
	}
	else if (channels == AEIO_SndChannels_STEREO || channels == 0)
	{
		exportInfo.audio.channelLayout = AV_CH_LAYOUT_STEREO;
	}
	else
	{
		vkLogErrorVA("Channel layout with %d channels is not supported. Audio track disabled.", (int)channels);

		exportInfo.audio.enabled = false;
	}

	// Set sample format
	if (exportInfo.audio.options.find("_sampleFormat") != exportInfo.audio.options.end())
	{
		exportInfo.audio.sampleFormat = av_get_sample_fmt(exportInfo.audio.options.at("_sampleFormat").c_str());
	}
	else
	{
		// Find default sample format
		AVCodec* codec = avcodec_find_encoder_by_name(exportInfo.audio.id);
		if (codec != NULL)
		{
			exportInfo.audio.sampleFormat = codec->sample_fmts[0];
		}
	}
	
	// Create a new encoder engine instance
	encoderEngine = new EncoderEngine(exportInfo);
	if (encoderEngine->open() < 0)
	{
		vkLogError("Unable to open the encoder engine with the supplied export info.");
		err = A_Err_GENERIC;
	}

	// Reset performance log
	tp_framePrev = tp_exportStart = std::chrono::high_resolution_clock::now();
	frame_count = 0;

	return err;
}

static A_Err My_AddFrame(AEIO_BasicData *basic_dataP, AEIO_OutSpecH outH, A_long frame_index, A_long frames, const PF_EffectWorld *wP, const A_LPoint *origin0, A_Boolean was_compressedB, AEIO_InterruptFuncs *inter0)
{
	A_Err err = A_Err_NONE;

	int usRender = (std::chrono::high_resolution_clock::now() - tp_framePrev) / std::chrono::microseconds(1);

	// Create a frame with argb data
	AVFrame* frame = av_frame_alloc();
	frame->width = wP->width;
	frame->height = wP->height;
	frame->data[0] = reinterpret_cast<uint8_t*>(wP->data);
	frame->pts = frame_index;

	// Fill in 8 or 16 bpc data
	if (PF_WORLD_IS_DEEP(wP))
	{
		frame->format = AV_PIX_FMT_BGRA64LE;
		frame->linesize[0] = frame->width * 8;
	}
	else
	{
		frame->format = AV_PIX_FMT_ARGB;
		frame->linesize[0] = frame->width * 4;
	}

	auto tp_encStart = std::chrono::high_resolution_clock::now();

	// Encode & write video frame
	if (encoderEngine->writeVideoFrame(frame) < 0)
	{
		vkLogErrorVA("Unable to write video frame #%d.", frame->pts);
		err = A_Err_GENERIC;
	}

	int usEncode = (std::chrono::high_resolution_clock::now() - tp_encStart) / std::chrono::microseconds(1);

	// Free the frame from memory again
	av_frame_free(&frame);

	// Write performance log
	vkLogInfoVA("Frame #%d: vRender: %d 탎, vProcess: %d 탎, vEncoding: %d 탎, Latency: %d 탎",
		frame_index,
		usRender,
		0,
		usEncode,
		(int)((std::chrono::high_resolution_clock::now() - tp_framePrev) / std::chrono::microseconds(1)));

	// Measure video latency w/o audio
	tp_framePrev = std::chrono::high_resolution_clock::now();
	frame_count++;
	
	return err;
}

static A_Err My_EndAdding(AEIO_BasicData *basic_dataP, AEIO_OutSpecH outH, A_long flags)
{
	int msEncode = (std::chrono::high_resolution_clock::now() - tp_exportStart) / std::chrono::milliseconds(1);

	// Log performance
	vkLogInfoVA("Exported %d frames in %d seconds. (avg. %d fps)",
		frame_count,
		msEncode / 1000,
		frame_count * 1000 / msEncode);

	if (encoderEngine)
	{
		// Shutdown encoders
		encoderEngine->finalize();
		encoderEngine->close();

		delete(encoderEngine);
	}

	return A_Err_NONE;
}

static A_Err My_WriteLabels(AEIO_BasicData *basic_dataP, AEIO_OutSpecH outH, AEIO_LabelFlags *written)
{
	return AEIO_Err_USE_DFLT_CALLBACK;
}

static A_Err My_GetSizes(AEIO_BasicData *basic_dataP, AEIO_OutSpecH outH, A_u_longlong *free_space, A_u_longlong *file_size)
{
	return AEIO_Err_USE_DFLT_CALLBACK;
}

static A_Err My_Flush(AEIO_BasicData *basic_dataP, AEIO_OutSpecH outH)
{
	return A_Err_NONE;
}

static A_Err My_AddSoundChunk(AEIO_BasicData *basic_dataP, AEIO_OutSpecH outH, const A_Time *start, A_u_long num_samplesLu, const void *dataPV)
{
	A_Err err = A_Err_NONE;

	AEGP_SuiteHandler suites(basic_dataP->pica_basicP);

	// Create a frame with argb data
	AVFrame* frame = av_frame_alloc();
	frame->nb_samples = num_samplesLu;
	frame->data[0] = (uint8_t*)dataPV;
	frame->pts = 0;

	//
	A_FpLong soundRateF = 0.0;
	ERR(suites.IOOutSuite4()->AEGP_GetOutSpecSoundRate(outH, &soundRateF));
	frame->sample_rate = (int)soundRateF;

	//
	AEIO_SndChannels channels;
	suites.IOOutSuite4()->AEGP_GetOutSpecSoundChannels(outH, &channels);
	frame->channels = channels;
	frame->channel_layout = av_get_default_channel_layout(channels);

	//
	AEIO_SndSampleSize sampleSize = 0;
	suites.IOOutSuite4()->AEGP_GetOutSpecSoundSampleSize(outH, &sampleSize);

	// Get data encoding
	AEIO_SndEncoding encoding;
	suites.IOOutSuite4()->AEGP_GetOutSpecSoundEncoding(outH, &encoding);

	if (encoding == AEIO_E_UNSIGNED_PCM)
	{
		frame->format = AV_SAMPLE_FMT_U8P;
	}
	else if (encoding == AEIO_E_SIGNED_PCM)
	{
		frame->format = AV_SAMPLE_FMT_S16;
	}
	else if (encoding == AEIO_E_SIGNED_FLOAT)
	{
		frame->format = AV_SAMPLE_FMT_FLTP;
	}
	   
	// Encode & write video frame
	if (encoderEngine->writeAudioFrame(frame) < 0)
	{
		vkLogErrorVA("Unable to write audio frame #%lld.", frame->pts);
		err = A_Err_GENERIC;
	}

	// Free the frame from memory again
	av_frame_free(&frame);

	return err;
};

static A_Err My_GetDepths(AEIO_BasicData *basic_dataP, AEIO_OutSpecH outH, AEIO_SupportedDepthFlags *which)
{
	*which = AEIO_SupportedDepthFlags_DEPTH_24 | AEIO_SupportedDepthFlags_DEPTH_32;// | AEIO_SupportedDepthFlags_DEPTH_64;

	return A_Err_NONE;
}

static A_Err My_GetOutputSuffix(AEIO_BasicData *basic_dataP, AEIO_OutSpecH outH, A_char *suffix)
{
	A_Err err = A_Err_NONE;

	AEGP_SuiteHandler suites(basic_dataP->pica_basicP);

	// Load config from storage
	AEIO_Handle optionsH = NULL;
	ERR(suites.IOOutSuite4()->AEGP_GetOutSpecOptionsHandle(outH, reinterpret_cast<void**>(&optionsH)));
	if (!optionsH)
	{
		return AEIO_Err_USE_DFLT_CALLBACK;
	}

	// Get current settings
	ArbData* arbData = NULL;
	ERR(suites.MemorySuite1()->AEGP_LockMemHandle(optionsH, reinterpret_cast<void**>(&arbData)));
	wxString formatId = arbData->formatId;
	ERR(suites.MemorySuite1()->AEGP_UnlockMemHandle(optionsH));

	// Find suffix
	for (auto& muxer : Voukoder::Config::Get().muxerInfos)
	{
		if (muxer.id == formatId)
		{
			suites.ANSICallbacksSuite1()->strcpy(suffix, "." + muxer.extension);
			break;
		}
	}

	return A_Err_NONE;
}

static A_Err My_CloseSourceFiles(AEIO_BasicData	*basic_dataP, AEIO_InSpecH seqH)
{
	return A_Err_NONE;
}

static A_Err My_SetUserData(AEIO_BasicData *basic_dataP, AEIO_OutSpecH outH, A_u_long typeLu, A_u_long indexLu, const AEIO_Handle dataH)
{
	return A_Err_NONE;
}

static A_Err AEIO_ConstructFunctionBlock(AEIO_FunctionBlock4 *funcs)
{
	if (funcs)
	{
		funcs->AEIO_AddFrame = My_AddFrame;
		funcs->AEIO_AddSoundChunk = My_AddSoundChunk;
		funcs->AEIO_DisposeOutputOptions = My_DisposeOutputOptions;
		funcs->AEIO_EndAdding = My_EndAdding;
		funcs->AEIO_Flush = My_Flush;
		funcs->AEIO_GetDepths = My_GetDepths;
		funcs->AEIO_GetOutputInfo = My_GetOutputInfo;
		funcs->AEIO_GetOutputSuffix = My_GetOutputSuffix;
		funcs->AEIO_GetSizes = My_GetSizes;
		funcs->AEIO_SetOutputFile = My_SetOutputFile;
		funcs->AEIO_SetUserData = My_SetUserData;
		funcs->AEIO_StartAdding = My_StartAdding;
		funcs->AEIO_UserOptionsDialog = My_UserOptionsDialog;
		funcs->AEIO_WriteLabels = My_WriteLabels;
		funcs->AEIO_InitOutputSpec = My_InitOutputSpec;
		funcs->AEIO_GetFlatOutputOptions = My_GetFlatOutputOptions;
		funcs->AEIO_OutputInfoChanged = My_OutputInfoChanged;

		return A_Err_NONE;
	}
	else
		return A_Err_STRUCT;
}

A_Err GPMain_IO(struct SPBasicSuite *pica_basicP, A_long major_versionL, A_long minor_versionL,	AEGP_PluginID aegp_plugin_id, void *global_refconPV)
{
#ifdef _DEBUG
	av_log_set_level(AV_LOG_TRACE);
	av_log_set_callback(AvCallback);
#endif

	A_Err err = A_Err_NONE;
	AEIO_ModuleInfo info;
	AEIO_FunctionBlock4	funcs;
	AEGP_SuiteHandler suites(pica_basicP);

	AEFX_CLR_STRUCT(info);
	AEFX_CLR_STRUCT(funcs);

	ERR(suites.RegisterSuite5()->AEGP_RegisterDeathHook(aegp_plugin_id, DeathHook, 0));
	ERR(AEIO_ConstructModuleInfo(&info));
	ERR(AEIO_ConstructFunctionBlock(&funcs));
	ERR(suites.RegisterSuite5()->AEGP_RegisterIO(aegp_plugin_id, 0, &info, &funcs));
	ERR(suites.UtilitySuite3()->AEGP_RegisterWithAEGP(NULL, "VOUKODER_R2", &S_mem_id));
	
	// Init voukoder core
	Voukoder::Config::Get();

	return err;
}