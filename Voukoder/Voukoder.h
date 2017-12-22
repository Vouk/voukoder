#include <sstream>
#include <algorithm>
#include "Common.h"

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
prMALError SetupEncoderInstance(InstanceRec *instRec, csSDK_uint32 exID, Encoder *encoder, EncoderConfig *videoConfig, EncoderConfig *audioConfig);
prMALError RenderAndWriteAllFrames(exDoExportRec *exportInfoP, Encoder *encoder, csSDK_int32 pass, csSDK_int32 maxPasses);

// reviewed 0.4.2
class Voukoder
{
public:
	Voukoder();

};