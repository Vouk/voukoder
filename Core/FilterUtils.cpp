#include "FilterUtils.h"
#include "LanguageUtils.h"
#include "OptionResourceUtils.h"
#include "lavf.h"
#include "Log.h"

bool FilterUtils::Create(FilterInfo &filterInfo, const json resource)
{
	// Parse filter info
	filterInfo.id = resource["id"].get<string>();
	filterInfo.name = resource["name"].get<string>();
	filterInfo.type = GetMediaType(filterInfo.name);

	vkLogInfo("Loading: filters/%s.json", filterInfo.name.c_str());

	// Default parameters
	for (auto& item : resource["defaults"].items())
	{
		string value = item.value().get<string>();
		filterInfo.defaults.insert(make_pair(item.key(), value));
	}

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

AVMediaType FilterUtils::GetMediaType(const wxString filterId)
{
	const AVFilter *filter = avfilter_get_by_name(filterId.c_str());
	if (filter && filter->inputs && avfilter_pad_count(filter->inputs) > 0)
	{
		return avfilter_pad_get_type(filter->inputs, 0);
	}

	return AVMEDIA_TYPE_UNKNOWN;
}