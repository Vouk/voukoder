#pragma once

#include <chrono>
#include "AEConfig.h"
#include "entry.h"

#ifdef _WIN32
#include <windows.h>
#endif

#include "AE_IO.h"
#include "AE_Macros.h"
#include "AEGP_SuiteHandler.h"
#include <ExportInfo.h>

#define ARB_VERSION 2

#ifdef __cplusplus
extern "C" {
#endif
	typedef struct 
	{
		char videoCodecId[16] = { 0 };
		char videoCodecOptions[16384] = { 0 };
		char audioCodecId[16] = { 0 };
		char audioCodecOptions[16384] = { 0 };
		char formatId[10] = { 0 };
		bool faststart = false;
		A_long version = 0;
		char videoFilters[16384] = { 0 };
		char audioFilters[16384] = { 0 };
		char videoSideData[16384] = { 0 };
		char audioSideData[16384] = { 0 };
	} ArbData;

	DllExport A_Err GPMain_IO(
			struct SPBasicSuite		*pica_basicP,
			A_long				 	major_versionL,
			A_long					minor_versionL,
			AEGP_PluginID			aegp_plugin_id,
			void					*global_refconPV);
#ifdef __cplusplus
}
#endif

static wxString GetFileName(AEIO_BasicData* basic_dataP, AEIO_OutSpecH outH);