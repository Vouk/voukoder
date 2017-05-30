#pragma once

#include "ExporterX264Common.h"

#define FFMultiplexerTabGroup			"MultiplexerTab"
#define FFMultiplexerBasicGroup			"MultiplexerBasicGroup"
#define FFMultiplexer					"Multiplexer"
#define FFAudioEncoderGroup				"AudioEncoderGroup"
#define FFOptionFaststart				"OptionFaststart"
#define FFVersions						"Versions"
#define X264_Codec						"X264_Codec"
#define X264_Preset						"X264_Preset"
#define X264_Profile					"X264_Profile"
#define X264_Level						"X264_Level"
#define X264_Strategy					"X264_Strategy"
#define X264_Strategy_CRF				"X264_Strategy_CRF"
#define X264_Strategy_QP				"X264_Strategy_QP"


prMALError exGenerateDefaultParams(exportStdParms *stdParms, exGenerateDefaultParamRec *generateDefaultParamRec);
prMALError exPostProcessParams(exportStdParms *stdParmsP, exPostProcessParamsRec *postProcessParamsRecP);
prMALError exValidateParamChanged(exportStdParms *stdParmsP, exParamChangedRec	*validateParamChangedRecP);
