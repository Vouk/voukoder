#pragma once

#include <Voukoder.h>
#include <ExportInfo.h>
#include <wxVoukoderDialog.h>
#include "Suites.h"

#define VKDRVoukoderTabGroup "VKDRVoukoderTabGroup"
#define VKDRVoukoderConfiguration "VKDRVideoConfiguration"
#define VKDRVideoColorRange "VKDRVideoColorRange"
#define VKDRVideoColorSpace "VKDRVideoColorSpace"
#define VKDRSelectedMuxer "VKDRSelectedMuxer"

#define CONFIG_MAX_SIZE 16384
#define ARB_VERSION 2

enum VKDRColorRanges {
	LIMITED,
	FULL
};

enum VKDRColorSpaces {
	BT601_PAL,
	BT601_NTSC,
	BT709,
	BT2020_CL,
	BT2020_NCL
};

const csSDK_int32 VideoAspects[][2] = { { 1, 1 }, { 10, 11 }, { 40, 33 }, { 768, 702 }, { 1024, 702 }, { 2, 1 }, { 4, 3 }, { 3, 2 } };
const PrTime VideoFramerates[][2] = { {5, 1}, {10, 1}, {12, 1}, {15, 1}, {18, 1}, {20, 1}, {24000, 1001}, {24, 1}, {25, 1}, {30000, 1001}, {30, 1}, {50, 1}, {60000, 1001}, {60, 1}, { 120000, 1001 }, { 120, 1} };
const csSDK_int32 VideoFieldTypes[] = { prFieldsNone, prFieldsUpperFirst, prFieldsLowerFirst };
const csSDK_int32 ColorRanges[] = { VKDRColorRanges::LIMITED, VKDRColorRanges::FULL };
const csSDK_int32 ColorSpaces[] = { VKDRColorSpaces::BT601_PAL, VKDRColorSpaces::BT601_NTSC, VKDRColorSpaces::BT709, VKDRColorSpaces::BT2020_CL, VKDRColorSpaces::BT2020_NCL };
const double AudioSamplingRates[] = { 32000.0, 44100.0, 48000.0, 96000.0 };
const csSDK_int32 AudioChannels[] = { kPrAudioChannelType_Mono, kPrAudioChannelType_Stereo, kPrAudioChannelType_51 };

struct ArbData
{
	prUTF16Char videoCodecId[16];
	prUTF16Char videoCodecOptions[16384];
	prUTF16Char videoSideData[16384] = { 0 };
	prUTF16Char audioCodecId[16];
	prUTF16Char audioCodecOptions[16384];
	prUTF16Char audioSideData[16384] = { 0 };
	prUTF16Char formatId[10];
	bool faststart;
	csSDK_uint32 version = 0;
	prUTF16Char videoFilters[16384] = { 0 };
	prUTF16Char audioFilters[16384] = { 0 };
};

#define prCreateIntParam(_identifier_, _groupName_, _paramflags_, _defaultValue_, _minValue_, _maxValue_, _disabled_, _hidden_) { \
	exParamValues val; \
	val.structVersion = 1; \
	val.value.intValue = _defaultValue_; \
	if (_minValue_ != NULL) val.rangeMin.intValue = _minValue_; \
	if (_maxValue_ != NULL) val.rangeMax.intValue = _maxValue_; \
	val.disabled = _disabled_; \
	val.hidden = _hidden_; \
	val.optionalParamEnabled = kPrFalse; \
	exNewParamInfo par; \
	par.structVersion = 1; \
	par.flags = _paramflags_; \
	par.paramType = exParamType_int; \
	par.paramValues = val; \
    strncpy(par.identifier, _identifier_, sizeof(_identifier_)); \
	suites->exportParamSuite->AddParam(pluginId, 0, _groupName_, &par); \
}

#define prCreateBoolParam(_identifier_, _groupName_, _paramflags_, _defaultValue_, _disabled_, _hidden_) { \
	exParamValues val; \
	val.structVersion = 1; \
	val.value.intValue = _defaultValue_; \
	val.disabled = _disabled_; \
	val.hidden = _hidden_; \
	val.optionalParamEnabled = kPrFalse; \
	exNewParamInfo par; \
	par.structVersion = 1; \
	par.flags = _paramflags_; \
	par.paramType = exParamType_bool; \
	par.paramValues = val; \
    strncpy(par.identifier, _identifier_, sizeof(_identifier_)); \
	suites->exportParamSuite->AddParam(pluginId, 0, _groupName_, &par); \
}

#define prCreateRatioParam(_identifier_, _groupName_, _paramflags_, _defaultValueNum_, _defaultValueDen_) { \
	exParamValues val; \
	val.structVersion = 1; \
	val.value.ratioValue.numerator = _defaultValueNum_; \
	val.value.ratioValue.denominator = _defaultValueDen_; \
	val.disabled = kPrFalse; \
	val.hidden = kPrFalse; \
	val.optionalParamEnabled = kPrFalse; \
	exNewParamInfo par; \
	par.structVersion = 1; \
	par.flags = _paramflags_; \
	par.paramType = exParamType_ratio; \
	par.paramValues = val; \
    strncpy(par.identifier, _identifier_, sizeof(_identifier_)); \
	suites->exportParamSuite->AddParam(pluginId, 0, _groupName_, &par); \
}

#define prCreateTimeParam(_identifier_, _groupName_, _paramflags_, _defaultValue_) { \
	exParamValues val; \
	val.structVersion = 1; \
	val.value.timeValue = _defaultValue_; \
	val.disabled = kPrFalse; \
	val.hidden = kPrFalse; \
	val.optionalParamEnabled = kPrFalse; \
	exNewParamInfo par; \
	par.structVersion = 1; \
	par.flags = _paramflags_; \
	par.paramType = exParamType_ticksFrameRate; \
	par.paramValues = val; \
    strncpy(par.identifier, _identifier_, sizeof(_identifier_)); \
	suites->exportParamSuite->AddParam(pluginId, 0, _groupName_, &par); \
}

#define prCreateFloatParam(_identifier_, _groupName_, _paramflags_, _defaultValue_) { \
	exParamValues val; \
	val.structVersion = 1; \
	val.value.floatValue = _defaultValue_; \
	val.disabled = kPrFalse; \
	val.hidden = kPrFalse; \
	val.optionalParamEnabled = kPrFalse; \
	exNewParamInfo par; \
	par.structVersion = 1; \
	par.flags = _paramflags_; \
	par.paramType = exParamType_float; \
	par.paramValues = val; \
    strncpy(par.identifier, _identifier_, sizeof(_identifier_)); \
	suites->exportParamSuite->AddParam(pluginId, 0, _groupName_, &par); \
}

#define prCreateButtonParam(_identifier_, _groupName_, _paramflags_) { \
	exParamValues val; \
	val.structVersion = 1; \
	val.disabled = kPrFalse; \
	val.hidden = kPrFalse; \
	val.optionalParamEnabled = kPrFalse; \
	exNewParamInfo par; \
	par.structVersion = 1; \
	par.flags = _paramflags_; \
	par.paramType = exParamType_button; \
	par.paramValues = val; \
    strncpy(par.identifier, _identifier_, sizeof(_identifier_)); \
	suites->exportParamSuite->AddParam(pluginId, 0, _groupName_, &par); \
}

#define prCreateStringParam(_identifier_, _groupName_, _paramflags_, _defaultValue_) { \
	exParamValues val; \
	val.structVersion = 1; \
	val.disabled = kPrFalse; \
	val.hidden = kPrFalse; \
	val.optionalParamEnabled = kPrFalse; \
	prUTF16CharCopy(val.paramString, _defaultValue_); \
	exNewParamInfo par; \
	par.structVersion = 1; \
	par.flags = _paramflags_; \
	par.paramType = exParamType_string; \
	par.paramValues = val; \
    strncpy(par.identifier, _identifier_, sizeof(_identifier_)); \
	suites->exportParamSuite->AddParam(pluginId, 0, _groupName_, &par); \
}

#define prSetGroupName(_identifier_, _translationId_) { \
    const std::wstring label = Trans(_translationId_); \
    suites->exportParamSuite->SetParamName(pluginId, 0, _identifier_, label.c_str()); \
}

#define prSetNameDescription(_identifier_, _translationId_) { \
    const std::wstring label = Trans(_translationId_, "label"); \
    const std::wstring description = Trans(_translationId_, "description"); \
    suites->exportParamSuite->SetParamName(pluginId, 0, _identifier_, label.c_str()); \
	suites->exportParamSuite->SetParamDescription(pluginId, 0, _identifier_, description.c_str()); \
}

#define prCreateParamGroup(_identifier_, _groupName_, _parentGroupName_) { \
    const std::wstring label = Trans(_identifier_); \
	params->AddParamGroup(pluginId, 0, _parentGroupName_, _groupName_, label.c_str(), kPrFalse, kPrFalse, kPrFalse); \
}

#define prSetDisabled(_identifier_, _is_disabled_) { \
	exParamValues value; \
	suites->exportParamSuite->GetParamValue(pluginId, 0, _identifier_, &value); \
	value.disabled = _is_disabled_; \
	suites->exportParamSuite->ChangeParam(pluginId, 0, _identifier_, &value); \
}

#define prSetHidden(_identifier_, _is_hidden_) { \
	exParamValues value; \
	suites->exportParamSuite->GetParamValue(pluginId, 0, _identifier_, &value); \
	value.hidden = _is_hidden_; \
	suites->exportParamSuite->ChangeParam(pluginId, 0, _identifier_, &value); \
}

class Gui
{
public:
	Gui(PrSuites *suites, csSDK_uint32 pluginId);
	bool ClearEncoderOptions(const char *dataId);
	prMALError Create();
	prMALError Update();
	prMALError ButtonAction(exParamButtonRec *paramButtonRecP);
	prMALError Validate();
	void CheckSettings();
	void ParamChange(exParamChangedRec *paramRecP);
	void GetExportInfo(ExportInfo &exportInfo);
	prMALError GetSelectedFileExtension(prUTF16Char *extension);
	void OpenVoukoderConfigDialog(exParamButtonRec *paramButtonRecP);
	bool ReadEncoderOptions(const char *dataId, ExportInfo &exportInfo);
	bool StoreEncoderOptions(const char *dataId, ExportInfo exportInfo);
	prMALError ReportMessage(wxString message, csSDK_uint32 type = kEventTypeError);

private:
	csSDK_uint32 pluginId;
	csSDK_int32 paramVersion;
	PrSuites *suites;
};
