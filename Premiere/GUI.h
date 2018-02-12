#pragma once

#include "premiere_cs6\PrSDKExportParamSuite.h"
#include "premiere_cs6\PrSDKExportInfoSuite.h"
#include "premiere_cs6\PrSDKWindowSuite.h"
#include "EncoderSettings.h"

#define VKDRAdvVideoCodecTabGroup "VKDRAdvVideoCodecTabGroup"
#define VKDRTVStandard "VKDRTVStandard"
#define VKDRColorSpace "VKDRColorSpace"
#define VKDRColorRange "VKDRColorRange"
#define VKDRVideoSettings "VKDRVideoSettings"
#define VKDRAudioSettings "VKDRAudioSettings"
#define VKDRMultiplexerTabGroup "MultiplexerTab"
#define VKDRMultiplexerSettingsGroup "MultiplexerBasicGroup"
#define FFMultiplexer "Multiplexer"
#define FFMultiplexerBasicGroup "MultiplexerBasicGroup"

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
	GUI(csSDK_uint32 pluginId, Config *config);
	prMALError init(PrSDKExportParamSuite *exportParamSuite, PrSDKExportInfoSuite *exportInfoSuite, PrSDKTimeSuite *timeSuite, csSDK_int32 paramVersion);
	bool getCurrentEncoderSettings(PrSDKExportParamSuite *exportParamSuite, EncoderType encoderType, EncoderSettings *encoderSettings);
	void getExportSettings(PrSDKExportParamSuite *exportParamSuite, ExportSettings *exportSettings);
	prMALError update(PrSDKExportParamSuite *exportParamSuite, PrSDKTimeSuite *timeSuite);
	prMALError onParamChange(PrSDKExportParamSuite *exportParamSuite, exParamChangedRec *paramRecP);
	int showDialog(PrSDKWindowSuite *windowSuite, string text, string caption, uint32_t type = MB_OK);

private:
	csSDK_uint32 pluginId;
	csSDK_int32 groupIndex = 0;
	Config *config;
	exNewParamInfo createParamElement(IParamInfo paramInfo, csSDK_int32 hidden);
	void fillEncoderDropdown(PrSDKExportParamSuite *exportParamSuite, vector<EncoderInfo> encoderInfos);
	void initDynamicParameters(PrSDKExportParamSuite *exportParamSuite, vector<EncoderInfo> encoderInfos);
	void refreshEncoderSettings(PrSDKExportParamSuite *exportParamSuite);
	void updateDynamicParameters(PrSDKExportParamSuite *exportParamSuite, vector<EncoderInfo> encoderInfos);
	void updateSingleDynamicParameter(PrSDKExportParamSuite *exportParamSuite, IParamInfo *paramInfo);
};