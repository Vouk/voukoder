#pragma once

#ifdef BUILD_CS6_COMPATIBLE
#include "premiere_cs6\PrSDKExportParamSuite.h"
#include "premiere_cs6\PrSDKExportInfoSuite.h"
#include "premiere_cs6\PrSDKWindowSuite.h"
#else
#include "premiere_cc2015\PrSDKExportParamSuite.h"
#include "premiere_cc2015\PrSDKExportInfoSuite.h"
#include "premiere_cc2015\PrSDKWindowSuite.h"
#endif
#include "EncoderSettings.h"
#include "FilterSettings.h"

#define VKDRAdvVideoCodecTabGroup "VKDRAdvVideoCodecTabGroup"
#define VKDRTVStandard "VKDRTVStandard"
#define VKDRVideoFrameSize "VKDRVideoFrameSize"
#define VKDRColorSpace "VKDRColorSpace"
#define VKDRColorRange "VKDRColorRange"
#define VKDRUpdateButton "VKDRUpdateButton"
#define VKDRMultiplexerTabGroup "MultiplexerTab"
#define VKDRMultiplexerSettingsGroup "MultiplexerBasicGroup"
#define MultiplexerPipeGroup "MultiplexerPipeGroup"
#define VKDRPipeCommand "VKDRPipeCommand"
#define VKDRPipeButton "VKDRPipeButton"
#define VKDRPipeArguments "VKDRPipeArguments"
#define FFMultiplexer "Multiplexer"
#define FFMultiplexerBasicGroup "MultiplexerBasicGroup"

#define FilterTabGroup "FilterTabGroup"

enum TVStandard {
	NTSC,
	PAL
};

enum ColorSpace {
	bt601,
	bt709,
	bt2020
};

enum ColorRange {
	Limited,
	Full
};

class GUI
{
public:
	GUI(csSDK_uint32 pluginId, Config *config, PluginUpdate *pluginUpdate, csSDK_int32 paramVersion);
	prMALError createParameters(PrSDKExportParamSuite *exportParamSuite, PrSDKExportInfoSuite *exportInfoSuite, PrSDKTimeSuite *timeSuite);
	prMALError updateParameters(PrSDKExportParamSuite *exportParamSuite, PrSDKTimeSuite *timeSuite);
	prMALError onButtonPress(exParamButtonRec *paramButtonRecP, PrSDKExportParamSuite *exportParamSuite, PrSDKWindowSuite *windowSuite);


	bool getCurrentFilterSettings(PrSDKExportParamSuite *exportParamSuite, ExportSettings *exportSettings);
	bool getCurrentEncoderSettings(PrSDKExportParamSuite *exportParamSuite, prFieldType fieldType, EncoderType encoderType, EncoderSettings *encoderSettings);
	void getExportSettings(PrSDKExportParamSuite *exportParamSuite, ExportSettings *exportSettings);
	prMALError onParamChange(PrSDKExportParamSuite *exportParamSuite, PrSDKWindowSuite *windowSuite, exParamChangedRec *paramRecP);
	int showDialog(PrSDKWindowSuite *windowSuite, string text, string caption, uint32_t type = MB_OK);

private:
	csSDK_uint32 pluginId;
	csSDK_int32 paramVersion;
	csSDK_int32 groupIndex = 0;
	Config *config;
	PluginUpdate *pluginUpdate;
	void createParametersFromConfig(PrSDKExportParamSuite *exportParamSuite, IParamContainer encoderInfos, int selectedId);
	exNewParamInfo createParameter(IParamInfo paramInfo, csSDK_int32 hidden);
	prSuiteError setParameterVisibility(PrSDKExportParamSuite *exportParamSuite, IParamInfo *info, const bool hidden);



	void fillEncoderDropdown(PrSDKExportParamSuite *exportParamSuite, vector<EncoderInfo> encoderInfos);
	void refreshEncoderSettings(PrSDKExportParamSuite *exportParamSuite);
	void updateDynamicParameters(PrSDKExportParamSuite *exportParamSuite, IParamContainer encoderInfo);
	void updateSingleDynamicParameter(PrSDKExportParamSuite *exportParamSuite, IParamInfo *paramInfo, const bool isCacheValid);
};