#pragma once

#include "AEConfig.h"
#include "entry.h"

#ifdef _WIN32
#include <windows.h>
#endif

#include "AE_IO.h"
#include "AE_Macros.h"
//#include "AE_EffectCBSuites.h"
#include "AEGP_SuiteHandler.h"
#include <ExportInfo.h>

#ifdef __cplusplus
extern "C" {
#endif
	typedef struct
	{
		char videoCodecId[16];
		char videoCodecOptions[16384];
		char audioCodecId[16];
		char audioCodecOptions[16384];
		char formatId[10];
		bool faststart;
	} ArbDataHeader;

	typedef struct 
	{
		A_long isFlat;
		char videoCodecId[16];
		char videoCodecOptions[16384];
		char audioCodecId[16];
		char audioCodecOptions[16384];
		char formatId[10];
		bool faststart;
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