#include "stdafx.h"
#include "AfterEffectsPlugin.h"
//#include <Voukoder.h>
#include "AEGP_SuiteHandler.cpp"
#include "MissingSuiteError.cpp"
#include <Windows.h>
#include <wx/wx.h>
#include <wxVoukoderDialog.h>
#include <EncoderEngine.h>

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

class actctx_activator
{
protected:
	ULONG_PTR m_cookie; // Cookie for context deactivation

public:
	// Construct the activator and activates the given activation context
	actctx_activator(_In_ HANDLE hActCtx)
	{
		if (!ActivateActCtx(hActCtx, &m_cookie))
			m_cookie = 0;
	}

	// Deactivates activation context and destructs the activator
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

static A_Err DeathHook(
	AEGP_GlobalRefcon unused1,
	AEGP_DeathRefcon unused2)
{
	return A_Err_NONE;
}

static A_Err
AEIO_ConstructModuleInfo(
	AEIO_ModuleInfo	*info)
{
	A_Err err = A_Err_NONE;

	if (info)
	{
		std::vector<MuxerInfo> muxerInfos = Voukoder::Config::Get().muxerInfos;

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
			AEIO_MFlag_AUDIO |
			AEIO_MFlag_NO_TIME;

		info->create_kind.type = 'VKDR';
		info->create_kind.creator = 'VOUK';

		// Set plugin name
		strcpy_s(info->name, "Voukoder R2");

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

static A_Err
My_InitOutputSpec(
	AEIO_BasicData *basic_dataP,
	AEIO_OutSpecH outH,
	A_Boolean *user_interacted)
{
	A_Err err = A_Err_NONE;

	AEGP_SuiteHandler suites(basic_dataP->pica_basicP);

	AEIO_Handle old_optionsH = NULL, new_optionsH = NULL;
	ERR(suites.MemorySuite1()->AEGP_NewMemHandle(S_mem_id, "InitOutputSpec options", sizeof(ArbData), AEGP_MemFlag_CLEAR, &new_optionsH));
	ERR(suites.IOOutSuite4()->AEGP_GetOutSpecOptionsHandle(outH, reinterpret_cast<void**>(&old_optionsH)));

	ArbData *arbData = NULL;
	ERR(suites.MemorySuite1()->AEGP_LockMemHandle(new_optionsH, reinterpret_cast<void**>(&arbData)));

	// Create a new data object
	memset(arbData, 0, sizeof(ArbData));
	strcpy_s(arbData->videoCodecId, DefaultVideoEncoder);
	strcpy_s(arbData->videoCodecOptions, "");
	strcpy_s(arbData->audioCodecId, DefaultAudioEncoder);
	strcpy_s(arbData->audioCodecOptions, "");
	strcpy_s(arbData->formatId, DefaultMuxer);
	arbData->faststart = FALSE;

	ERR(suites.MemorySuite1()->AEGP_UnlockMemHandle(new_optionsH));

	ERR(suites.IOOutSuite4()->AEGP_SetOutSpecOptionsHandle(outH, new_optionsH, reinterpret_cast<void**>(&old_optionsH)));

	//AEIO_Handle old_optionsH = NULL, new_optionsH = NULL;
	//ERR(suites.IOOutSuite4()->AEGP_GetOutSpecOptionsHandle(outH, reinterpret_cast<void**>(&old_optionsH)));
	
	//if (!err)
	//{
	//	ERR(suites.MemorySuite1()->AEGP_NewMemHandle(S_mem_id, "InitOutputSpec options", sizeof(ArbData), AEGP_MemFlag_CLEAR, &new_optionsH));

	//	if (!err && new_optionsH)
	//	{
	//		ERR(suites.MemorySuite1()->AEGP_LockMemHandle(new_optionsH, reinterpret_cast<void**>(&arbData)));

	//		if (!err && arbData) {

	//			if (!old_optionsH) {
	//				//ERR(PretendToReadFileHeader(basic_dataP, reinterpret_cast<IO_FileHeader *>(&(new_optionsP->name))));
	//				arbData->isFlat = FALSE;
	//			}
	//			else {
	//				ERR(suites.MemorySuite1()->AEGP_LockMemHandle(old_optionsH, reinterpret_cast<void**>(&arbDataOld)));

	//				if (!err && arbData && arbDataOld) {
	//					memcpy(arbData, arbDataOld, sizeof(ArbData));
	//					arbData->isFlat = FALSE;

	//					//// If you modify the output options, then set this to true to tell AE that the project has changed
	//					//if (rand() % 3) {
	//					//	*user_interacted = TRUE;
	//					//}
	//					//else {
	//					//	*user_interacted = FALSE;
	//					//}

	//					ERR(suites.MemorySuite1()->AEGP_UnlockMemHandle(old_optionsH));
	//				}
	//			}
	//			ERR(suites.MemorySuite1()->AEGP_UnlockMemHandle(new_optionsH));

	//			ERR(suites.IOOutSuite4()->AEGP_SetOutSpecOptionsHandle(outH, new_optionsH, reinterpret_cast<void**>(&old_optionsH)));
	//		}
	//	}
	//}

	/*
	AEIO_Handle optionsH = NULL, old_optionsH = 0, old_old_optionsH = NULL;
	ArbData *arbData = NULL, *old_arbData = NULL;
	AEGP_SuiteHandler suites(basic_dataP->pica_basicP);



	// Get the old data
	ERR(suites.IOOutSuite4()->AEGP_GetOutSpecOptionsHandle(outH, reinterpret_cast<void**>(&old_optionsH)));
	
	// Get the existing data size
	AEGP_MemSize old_size;
	ERR(suites.MemorySuite1()->AEGP_GetMemHandleSize(old_optionsH, &old_size));

	// make new options handle
	suites.MemorySuite1()->AEGP_NewMemHandle(S_mem_id, "Output Options",
		sizeof(ArbData),
		AEGP_MemFlag_CLEAR, &optionsH);

	// 
	AEGP_MemSize mem_size;
	ERR(suites.MemorySuite1()->AEGP_GetMemHandleSize(optionsH, &mem_size));

	ERR(suites.MemorySuite1()->AEGP_LockMemHandle(optionsH, (void**)&arbData));

	if (old_optionsH && old_size == mem_size)
	{
		suites.MemorySuite1()->AEGP_LockMemHandle(old_optionsH, (void**)&old_arbData);

		memcpy((char*)arbData, (char*)old_arbData, mem_size);

		suites.MemorySuite1()->AEGP_UnlockMemHandle(old_optionsH);
	}
	else
	{
		// Create a new data object
		memset(arbData, 0, sizeof(ArbData));
		strcpy_s(arbData->videoCodecId, DefaultVideoEncoder);
		strcpy_s(arbData->videoCodecOptions, "");
		strcpy_s(arbData->audioCodecId, DefaultAudioEncoder);
		strcpy_s(arbData->audioCodecOptions, "");
		strcpy_s(arbData->formatId, DefaultMuxer);
		arbData->faststart = FALSE;
	}

	suites.MemorySuite1()->AEGP_UnlockMemHandle(optionsH);

	// set the options handle
	suites.IOOutSuite4()->AEGP_SetOutSpecOptionsHandle(outH, (void*)optionsH, (void**)&old_old_optionsH);
	
	// so now AE wants me to delete this. whatever.
	if (old_old_optionsH)
		suites.MemorySuite1()->AEGP_FreeMemHandle(old_old_optionsH);
	*/

	return err;
}

static A_Err
My_GetFlatOutputOptions(
	AEIO_BasicData	*basic_dataP,
	AEIO_OutSpecH	outH,
	AEIO_Handle		*new_optionsPH)
{
	A_Err						err = A_Err_NONE;
	AEIO_Handle					old_optionsH = NULL;
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

static A_Err
My_DisposeOutputOptions(
	AEIO_BasicData *basic_dataP,
	void *optionsPV)
{
	AEGP_SuiteHandler suites(basic_dataP->pica_basicP);
	AEIO_Handle optionsH = reinterpret_cast<AEIO_Handle>(optionsPV);
	A_Err err = A_Err_NONE;

	if (optionsH) {
		ERR(suites.MemorySuite1()->AEGP_FreeMemHandle(optionsH));
	}
	return err;
};

static int
ShowVoukoderDialog(ExportInfo &exportInfo)
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
		dialog.SetConfiguration();
		result = dialog.ShowModal();

		wxTopLevelWindows.DeleteObject(&parent);
		parent.SetHWND((WXHWND)NULL);
	}

	// Clean-up and return.
	wxEntryCleanup();

	return result;
}

static A_Err
My_UserOptionsDialog(
	AEIO_BasicData *basic_dataP,
	AEIO_OutSpecH outH,
	const PF_EffectWorld *sample0,
	A_Boolean *user_interacted0)
{
	A_Err err = A_Err_NONE;
	AEGP_SuiteHandler suites(basic_dataP->pica_basicP);
	AEIO_Handle optionsH = NULL, old_optionsH = 0;
	ArbData *arbData = NULL;

	ERR(suites.IOOutSuite4()->AEGP_GetOutSpecOptionsHandle(outH, reinterpret_cast<void**>(&optionsH)));
	if (!err) {
		ERR(suites.MemorySuite1()->AEGP_LockMemHandle(optionsH, reinterpret_cast<void**>(&arbData)));
		
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

		// Show voukoder dialog
		int result = ShowVoukoderDialog(exportInfo);

		ERR(suites.MemorySuite1()->AEGP_UnlockMemHandle(optionsH));

		// On OK click
		if (result == (int)wxID_OK)
		{
			// Fill ArbData
			strcpy_s(arbData->videoCodecId, exportInfo.video.id);
			strcpy_s(arbData->videoCodecOptions, exportInfo.video.options.Serialize().c_str());
			strcpy_s(arbData->audioCodecId, exportInfo.audio.id);
			strcpy_s(arbData->audioCodecOptions, exportInfo.audio.options.Serialize().c_str());
			strcpy_s(arbData->formatId, exportInfo.format.id);
			arbData->faststart = exportInfo.format.faststart;

			// Store configuration
			ERR(suites.IOOutSuite4()->AEGP_SetOutSpecOptionsHandle(outH, optionsH, reinterpret_cast<void**>(&old_optionsH)));
		}
	}

	return err;
};

static A_Err
My_GetOutputInfo(
	AEIO_BasicData *basic_dataP,
	AEIO_OutSpecH outH,
	AEIO_Verbiage *verbiageP)
{
	A_Err err = A_Err_NONE;
	AEGP_SuiteHandler suites(basic_dataP->pica_basicP);

	wxString formatId = DefaultMuxer,
		videoCodecId = DefaultVideoEncoder;

	// Load config from storage
	AEIO_Handle optionsH = NULL;
	ERR(suites.IOOutSuite4()->AEGP_GetOutSpecOptionsHandle(outH, reinterpret_cast<void**>(&optionsH)));
	if (!err) {
		ArbData *arbData = NULL;
		ERR(suites.MemorySuite1()->AEGP_LockMemHandle(optionsH, reinterpret_cast<void**>(&arbData)));
		formatId = arbData->formatId;
		videoCodecId = arbData->videoCodecId;
		ERR(suites.MemorySuite1()->AEGP_UnlockMemHandle(optionsH));
	}

	// TODO
	suites.ANSICallbacksSuite1()->strcpy(verbiageP->name, "vouk123");

	// Find format
	for (auto& muxer : Voukoder::Config::Get().muxerInfos)
	{
		if (muxer.id == formatId)
		{
			suites.ANSICallbacksSuite1()->strcpy(verbiageP->type, muxer.name);
			break;
		}
	}

	// Find encoder
	for (auto& encoder : Voukoder::Config::Get().encoderInfos)
	{
		if (encoder.id == videoCodecId)
		{
			suites.ANSICallbacksSuite1()->strcpy(verbiageP->sub_type, encoder.name);
			break;
		}
	}

	return err;
};


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

static A_Err
My_SetOutputFile(
	AEIO_BasicData		*basic_dataP,
	AEIO_OutSpecH		outH,
	const A_UTF16Char	*file_pathZ)
{
	return AEIO_Err_USE_DFLT_CALLBACK;
}

static A_Err My_StartAdding(AEIO_BasicData *basic_dataP, AEIO_OutSpecH outH, A_long flags)
{
	A_Err err = A_Err_NONE;
	AEGP_SuiteHandler suites(basic_dataP->pica_basicP);
	
	ExportInfo exportInfo;
	exportInfo.application = "After Effects";
	exportInfo.filename = "c:\\users\\Daniel\\Desktop\\test\\afx.mp4";
	exportInfo.passes = 1;
	exportInfo.video.enabled = true;
	exportInfo.video.id = "libx264";
	//exportInfo.video.options;
	exportInfo.video.width = 1920;
	exportInfo.video.height = 1080;
	exportInfo.video.timebase = { 1, 25 };
	exportInfo.video.pixelFormat = AV_PIX_FMT_YUV420P;
	exportInfo.video.colorRange = AVCOL_RANGE_MPEG;
	exportInfo.video.colorSpace = AVCOL_SPC_BT709;
	exportInfo.video.colorPrimaries = AVCOL_PRI_BT709;
	exportInfo.video.colorTransferCharacteristics = AVCOL_TRC_BT709;
	exportInfo.video.sampleAspectRatio = { 1,1 };
	exportInfo.video.fieldOrder = AV_FIELD_PROGRESSIVE;
	exportInfo.audio.enabled = false;
	//exportInfo.audio.id = "libx264";
	//exportInfo.audio.options;

	

	encoderEngine = new EncoderEngine(exportInfo);
	encoderEngine->open();

	//IO_FileHeader		header;
	//A_Time				duration = { 0,1 };
	//A_short				depth = 0;
	//A_Fixed				fps = 0;
	//A_long				widthL = 0,
	//	heightL = 0;
	//A_FpLong			soundRateF = 0.0;
	//A_char				name[AEGP_MAX_PATH_SIZE] = { '\0' };
	//AEGP_SuiteHandler	suites(basic_dataP->pica_basicP);

	//AEGP_ProjectH		my_projH = 0;
	//AEGP_TimeDisplay3	time_display;
	//A_long				start_frameL = 0;

	//AEGP_MemHandle				file_pathH = NULL;
	//A_Boolean					file_reservedB;
	//A_UTF16Char					*file_pathZ;

	//AEFX_CLR_STRUCT(time_display);
	//AEFX_CLR_STRUCT(header);

	//ERR(suites.IOOutSuite4()->AEGP_GetOutSpecDuration(outH, &duration));
	//ERR(suites.IOOutSuite4()->AEGP_GetOutSpecDimensions(outH, &widthL, &heightL));
	//ERR(suites.IOOutSuite4()->AEGP_GetOutSpecDepth(outH, &depth));
	//ERR(suites.IOOutSuite4()->AEGP_GetOutSpecSoundRate(outH, &soundRateF));

	//// If video
	//if (!err && name && widthL && heightL) {
	//	header.hasVideo = TRUE;
	//	header.widthLu = (A_u_long)widthL;
	//	header.heightLu = (A_u_long)heightL;

	//	ERR(suites.IOOutSuite4()->AEGP_GetOutSpecFPS(outH, &fps));
	//	header.fpsT.value = duration.value;
	//	header.fpsT.scale = duration.scale;

	//	if (depth > 32) {
	//		header.rowbytesLu = (unsigned long)(8 * widthL);
	//	}
	//	else {
	//		header.rowbytesLu = (unsigned long)(4 * widthL);
	//	}
	//}

	//if (!err && soundRateF > 0) {
	//	header.hasAudio = TRUE;
	//	header.rateF = soundRateF;
	//	ERR(suites.IOOutSuite4()->AEGP_GetOutSpecSoundChannels(outH, &header.num_channels));
	//	ERR(suites.IOOutSuite4()->AEGP_GetOutSpecSoundSampleSize(outH, &header.bytes_per_sample));

	//	header.encoding = AEIO_E_UNSIGNED_PCM;
	//}

	//// Get timecode
	//if (!err) {
	//	ERR(suites.ProjSuite6()->AEGP_GetProjectByIndex(0, &my_projH));
	//	ERR(suites.ProjSuite6()->AEGP_GetProjectTimeDisplay(my_projH, &time_display));

	//	ERR(suites.IOOutSuite4()->AEGP_GetOutSpecStartFrame(outH, &start_frameL));
	//}

	//if (!err) {
	//	ERR(suites.IOOutSuite4()->AEGP_GetOutSpecFilePath(outH, &file_pathH, &file_reservedB));

	//	ERR(suites.MemorySuite1()->AEGP_LockMemHandle(file_pathH, reinterpret_cast<void**>(&file_pathZ)));
	//	/*
	//		+	Open file
	//		+	Write header
	//		+	(keep file open)
	//	*/
	//	ERR(suites.MemorySuite1()->AEGP_UnlockMemHandle(file_pathH));
	//	ERR(suites.MemorySuite1()->AEGP_FreeMemHandle(file_pathH));
	//}
	return err;
};

static A_Err My_AddFrame(AEIO_BasicData *basic_dataP, AEIO_OutSpecH outH, A_long frame_index, A_long frames, const PF_EffectWorld *wP, const A_LPoint *origin0, A_Boolean was_compressedB, AEIO_InterruptFuncs *inter0)
{
	// Create a frame with argb data
	AVFrame* frame = av_frame_alloc();
	frame->width = wP->width;
	frame->height = wP->height;
	frame->pts = frame_index;
	frame->format = AV_PIX_FMT_ARGB;
	frame->data[0] = reinterpret_cast<uint8_t*>(wP->data);
	frame->linesize[0] = frame->width * PF_WORLD_IS_DEEP(wP) ? 8 : 4;

	A_Err err = A_Err_NONE;

	// Encode & write video frame
	if (encoderEngine->writeVideoFrame(frame) < 0)
	{
		//vkLog
		err = A_Err_GENERIC;
	}

	// Free the frame from memory again
	av_frame_free(&frame);
	
	return err;
};

static A_Err My_EndAdding(AEIO_BasicData *basic_dataP, AEIO_OutSpecH outH, A_long flags)
{
	// Shutdown encoders
	if (encoderEngine)
	{
		encoderEngine->finalize();
		encoderEngine->close();

		delete(encoderEngine);
	}

	return A_Err_NONE;
}

static A_Err
My_OutputFrame(
	AEIO_BasicData			*basic_dataP,
	AEIO_OutSpecH			outH,
	const PF_EffectWorld	*wP)
{
	A_Err		err = A_Err_NONE;

	/*
		+	Re-interpret the supplied PF_World in your own
			format, and save it out to the outH's path.

	*/
	return err;
};

static A_Err
My_WriteLabels(
	AEIO_BasicData	*basic_dataP,
	AEIO_OutSpecH	outH,
	AEIO_LabelFlags	*written)
{
	return AEIO_Err_USE_DFLT_CALLBACK;
};

static A_Err
My_GetSizes(
	AEIO_BasicData	*basic_dataP,
	AEIO_OutSpecH	outH,
	A_u_longlong	*free_space,
	A_u_longlong	*file_size)
{
	return AEIO_Err_USE_DFLT_CALLBACK;
};

static A_Err
My_Flush(
	AEIO_BasicData	*basic_dataP,
	AEIO_OutSpecH	outH)
{
	/*	free any temp buffers you kept around for
		writing.
	*/
	return A_Err_NONE;
};

static A_Err
My_AddSoundChunk(
	AEIO_BasicData	*basic_dataP,
	AEIO_OutSpecH	outH,
	const A_Time	*start,
	A_u_long		num_samplesLu,
	const void		*dataPV)
{
	A_Err err = A_Err_NONE, err2 = A_Err_NONE;

	AEGP_SuiteHandler	suites(basic_dataP->pica_basicP);

	AEIO_Handle					optionsH = NULL;
	ArbData	*optionsP = NULL;

	ERR(suites.IOOutSuite4()->AEGP_GetOutSpecOptionsHandle(outH, reinterpret_cast<void**>(&optionsH)));

	if (!err && optionsH) {
		ERR(suites.MemorySuite1()->AEGP_LockMemHandle(optionsH, reinterpret_cast<void**>(&optionsP)));
		if (!err) {
			A_char report[AEGP_MAX_ABOUT_STRING_SIZE] = { '\0' };
			suites.ANSICallbacksSuite1()->sprintf(report, "SDK_IO : Pretended to write %d samples of audio requested.", num_samplesLu);
			ERR(suites.UtilitySuite3()->AEGP_ReportInfo(basic_dataP->aegp_plug_id, report));
		}
	}
	ERR2(suites.MemorySuite1()->AEGP_UnlockMemHandle(optionsH));

	return err;
};


static A_Err
My_Idle(
	AEIO_BasicData *basic_dataP,
	AEIO_ModuleSignature sig,
	AEIO_IdleFlags *idle_flags0)
{
	return A_Err_NONE;
};


static A_Err
My_GetDepths(
	AEIO_BasicData *basic_dataP,
	AEIO_OutSpecH outH,
	AEIO_SupportedDepthFlags *which)
{
	*which = AEIO_SupportedDepthFlags_DEPTH_24;

	return A_Err_NONE;
};

static A_Err
My_GetOutputSuffix(
	AEIO_BasicData *basic_dataP,
	AEIO_OutSpecH outH,
	A_char *suffix)
{
	A_Err err = AEIO_Err_USE_DFLT_CALLBACK;

	AEGP_SuiteHandler suites(basic_dataP->pica_basicP);

	wxString formatId = DefaultMuxer;

	// Load config from storage
	AEIO_Handle optionsH = NULL;
	ERR(suites.IOOutSuite4()->AEGP_GetOutSpecOptionsHandle(outH, reinterpret_cast<void**>(&optionsH)));
	if (!err) {
		ArbData *arbData = NULL;
		ERR(suites.MemorySuite1()->AEGP_LockMemHandle(optionsH, reinterpret_cast<void**>(&arbData)));
		formatId = arbData->formatId;
		ERR(suites.MemorySuite1()->AEGP_UnlockMemHandle(optionsH));
	}

	// Find suffix
	for (auto& muxer : Voukoder::Config::Get().muxerInfos)
	{
		if (muxer.id == formatId)
		{
			suites.ANSICallbacksSuite1()->strcpy(suffix, muxer.extension);
			break;
		}
	}

	return err;
};

static A_Err
My_CloseSourceFiles(
	AEIO_BasicData	*basic_dataP,
	AEIO_InSpecH			seqH)
{
	return A_Err_NONE;
};		// TRUE for close, FALSE for unclose

static A_Err
My_SetUserData(
	AEIO_BasicData	*basic_dataP,
	AEIO_OutSpecH			outH,
	A_u_long				typeLu,
	A_u_long				indexLu,
	const AEIO_Handle		dataH)
{
	return A_Err_NONE;
};

static A_Err
My_AddMarker(
	AEIO_BasicData	*basic_dataP,
	AEIO_OutSpecH 			outH,
	A_long 					frame_index,
	AEIO_MarkerType 		marker_type,
	void					*marker_dataPV,
	AEIO_InterruptFuncs		*inter0)
{
	/*	The type of the marker is in marker_type,
		and the text they contain is in
		marker_dataPV.
	*/
	return A_Err_NONE;
};

static A_Err 
AEIO_ConstructFunctionBlock(
	AEIO_FunctionBlock4 *funcs)
{
	if (funcs)
	{
		funcs->AEIO_AddFrame = My_AddFrame;
		//funcs->AEIO_AddMarker = My_AddMarker;
		funcs->AEIO_AddSoundChunk = My_AddSoundChunk;
		funcs->AEIO_DisposeOutputOptions = My_DisposeOutputOptions;
		funcs->AEIO_EndAdding = My_EndAdding;
		funcs->AEIO_Flush = My_Flush;
		funcs->AEIO_GetDepths = My_GetDepths;
		funcs->AEIO_GetOutputInfo = My_GetOutputInfo;
		funcs->AEIO_GetOutputSuffix = My_GetOutputSuffix;
		funcs->AEIO_GetSizes = My_GetSizes;
		funcs->AEIO_Idle = My_Idle;
		funcs->AEIO_OutputFrame = My_OutputFrame; // single frame
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

A_Err 
GPMain_IO(
	struct SPBasicSuite *pica_basicP,
	A_long major_versionL,
	A_long minor_versionL,
	AEGP_PluginID aegp_plugin_id,
	void *global_refconPV)
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