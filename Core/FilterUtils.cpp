#include "FilterUtils.h"
#include "LanguageUtils.h"
#include "OptionResourceUtils.h"

bool FilterUtils::Create(FilterInfo &filterInfo, const json resource)
{
	// Parse filter info
	filterInfo.id = resource["id"].get<string>();
	filterInfo.name = resource["name"].get<string>();

	// Parse groups
	for (json group : resource["groups"])
	{
		EncoderGroupInfo encoderGroupInfo;
		encoderGroupInfo.id = group["id"].get<string>();
		encoderGroupInfo.name = Trans(encoderGroupInfo.id);
		encoderGroupInfo.groupClass = group["class"].get<string>();

		for (json obj2 : group["properties"])
		{
			EncoderOptionInfo encoderOptionInfo;
			if (OptionResourceUtils::CreateOptionInfo(encoderOptionInfo, obj2))
			{
				encoderGroupInfo.options.push_back(encoderOptionInfo);
			}
		}

		filterInfo.groups.push_back(encoderGroupInfo);
	}

	return true;
}