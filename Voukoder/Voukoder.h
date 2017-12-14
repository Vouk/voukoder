#include <sstream>
#include <algorithm>
#include "Common.h"

typedef char fourchars[4];

union M128 {
	struct plane { fourchars a, y, u, v; } plane;
	__m128i i128;
};

// Scaling factors (note min. values are actually negative)
const float yuva_factors[4][4] = {
	{ 0.07306f, 1.09132f, 0.00000f, 1.00000f }, // Y
	{ 0.57143f, 0.57143f, 0.50000f, 0.50000f }, // U
	{ 0.57143f, 0.57143f, 0.50000f, 0.50000f }, // V
	{ 0.00000f, 1.00000f, 0.00000f, 1.00000f }  // A
};

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