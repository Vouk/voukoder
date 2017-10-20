#include <sstream>
#include <algorithm>
#include "ExporterX264Common.h"

extern "C" DllExport PREMPLUGENTRY xSDKExport(csSDK_int32 selector, exportStdParms *stdParms, void *param1, void *param2);

prMALError exStartup(exportStdParms *stdParmsP, exExporterInfoRec *infoRecP);
prMALError exShutdown();
prMALError exBeginInstance(exportStdParms *stdParmsP, exExporterInstanceRec *instanceRecP);
prMALError exEndInstance(exportStdParms *stdParmsP, exExporterInstanceRec *instanceRecP);
prMALError exExport(exportStdParms *stdParmsP, exDoExportRec *exportInfoP);
prMALError exFileExtension(exportStdParms *stdParmsP, exQueryExportFileExtensionRec *exportFileExtensionRecP);
prMALError exQueryOutputSettings(exportStdParms *stdParmsP, exQueryOutputSettingsRec *outputSettingsP);
prMALError exGenerateDefaultParams(exportStdParms *stdParms, exGenerateDefaultParamRec *generateDefaultParamRec);
prMALError exPostProcessParams(exportStdParms *stdParmsP, exPostProcessParamsRec *postProcessParamsRecP);
prMALError exValidateParamChanged(exportStdParms *stdParmsP, exParamChangedRec	*validateParamChangedRecP);
prMALError exGetParamSummary(exportStdParms *stdParmsP, exParamSummaryRec *summaryRecP);
prMALError exValidateOutputSettings(exportStdParms *stdParmsP, exValidateOutputSettingsRec *validateOutputSettingsRec);
void CreateEncoderConfiguration(InstanceRec *instRec, csSDK_uint32 pluginId, csSDK_int32 groupIndex, json options, std::vector<std::string> *encoderConfiguration);
prMALError SetupEncoderInstance(InstanceRec *instRec, csSDK_uint32 exID, Encoder *encoder);
prMALError RenderAndWriteAllFrames(exDoExportRec *exportInfoP, Encoder *encoder);
PrPixelFormat GetPremierePixelFormats(const char* format, prFieldType fieldType, vkdrColorSpace colorSpace, vkdrColorRange colorRange);
std::string getVideoConfiguration(InstanceRec *instRec, csSDK_uint32 pluginId, csSDK_int32 groupIndex, json params);
std::string getAudioConfiguration(InstanceRec *instRec, csSDK_uint32 pluginId, csSDK_int32 groupIndex, json params);