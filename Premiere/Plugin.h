#pragma once

#include "premiere_cs6\PrSDKExport.h"
#include "premiere_cs6\PrSDKMemoryManagerSuite.h"
#include "premiere_cs6\PrSDKExportFileSuite.h"
#include "premiere_cs6\PrSDKExportInfoSuite.h"
#include "premiere_cs6\PrSDKExportParamSuite.h"
#include "premiere_cs6\PrSDKExportProgressSuite.h"
#include "premiere_cs6\PrSDKErrorSuite.h"
#include "premiere_cs6\PrSDKMarkerSuite.h"
#include "premiere_cs6\PrSDKSequenceRenderSuite.h"
#include "premiere_cs6\PrSDKSequenceAudioSuite.h"
#include "premiere_cs6\PrSDKClipRenderSuite.h"
#include "premiere_cs6\PrSDKPPixCreatorSuite.h"
#include "premiere_cs6\PrSDKPPixCacheSuite.h"
#include "premiere_cs6\PrSDKPPix2Suite.h"
#include "premiere_cs6\PrSDKMemoryManagerSuite.h"
#include "premiere_cs6\PrSDKWindowSuite.h"
#include "premiere_cs6\PrSDKExporterUtilitySuite.h"
#include "..\LibVKDR\LibVKDR.h"
#include "GUI.h"

#define VKDR_APPNAME L"Voukoder 0.6.0"
#define VKDR_PARAM_VERSION 5

using namespace LibVKDR;

extern "C" DllExport PREMPLUGENTRY xSDKExport(csSDK_int32 selector, exportStdParms *stdParms, void *param1, void *param2);

typedef struct PrSuites
{
	SPBasicSuite *spBasic;
	PrSDKMemoryManagerSuite *memorySuite;
	PrSDKTimeSuite *timeSuite;
	PrSDKExportParamSuite *exportParamSuite;
	PrSDKExportInfoSuite *exportInfoSuite;
	PrSDKSequenceAudioSuite *sequenceAudioSuite;
	PrSDKExportFileSuite *exportFileSuite;
	PrSDKPPixSuite *ppixSuite;
	PrSDKPPix2Suite *ppix2Suite;
	PrSDKExportProgressSuite *exportProgressSuite;
	PrSDKWindowSuite *windowSuite;
	PrSDKExporterUtilitySuite *exporterUtilitySuite;
} PrSuites;

class Plugin
{
public:
	PrSuites *suites;
	Plugin(csSDK_uint32 pluginId);
	~Plugin();
	prMALError beginInstance(SPBasicSuite *spBasic, exExporterInstanceRec *instanceRecP);
	prMALError endInstance();
	prMALError generateDefaultParams(exGenerateDefaultParamRec *paramRecP);
	prMALError postProcessParams(exPostProcessParamsRec *paramRecP);
	prMALError validateParamChanged(exParamChangedRec *paramRecP);
	prMALError getParamSummary(exParamSummaryRec *paramRecP);
	prMALError doExport(exDoExportRec *exportRecP);
	prMALError queryExportFileExtension(exQueryExportFileExtensionRec *exportFileExtensionRecP);
	prMALError queryOutputSettings(exQueryOutputSettingsRec *outputSettingsRecP);
	prMALError validateOutputSettings(exValidateOutputSettingsRec *outputSettingsRecP);

private:
	csSDK_uint32 pluginId;
	csSDK_int32 groupIndex = 0;
	SPBasicSuite *spBasicSuite;
	Logger logger;
	GUI *gui;
	Config *config;
};

