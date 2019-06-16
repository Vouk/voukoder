#pragma once
  
#include <chrono>
#include <wx/wx.h>
#include <premiere_cs6/PrSDKExport.h>
#include <EncoderInfo.h>
#include "Suites.h"
#include "Gui.h"

extern "C" DllExport PREMPLUGENTRY xSDKExport(csSDK_int32 selector, exportStdParms *stdParms, void *param1, void *param2);

class CPremierePluginApp
{
public:
        CPremierePluginApp(csSDK_uint32 pluginId);
        prMALError beginInstance(SPBasicSuite *spBasic, exExporterInstanceRec *instanceRecP);
        prMALError endInstance();
        prMALError generateDefaultParams(exGenerateDefaultParamRec *paramRecP);
        prMALError postProcessParams(exPostProcessParamsRec *paramRecP);
        prMALError validateParamChanged(exParamChangedRec *paramRecP);
        prMALError getParamSummary(exParamSummaryRec *paramRecP);
        prMALError StartExport(exDoExportRec *exportRecP);
        prMALError queryExportFileExtension(exQueryExportFileExtensionRec *exportFileExtensionRecP);
        prMALError queryOutputSettings(exQueryOutputSettingsRec *outputSettingsRecP);
        prMALError validateOutputSettings(exValidateOutputSettingsRec *outputSettingsRecP);
        prMALError buttonAction(exParamButtonRec *paramButtonRecP);

private:
        csSDK_uint32 pluginId;
        PrSuites suites;
        Gui *gui;
        std::chrono::time_point<std::chrono::steady_clock> tp_framePrev;
        const wxString GetFilename(csSDK_uint32 fileObject);
};
