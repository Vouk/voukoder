#include "FilterUtils.h"
#include "LanguageUtils.h"
#include "OptionResourceUtils.h"
#include "lavf.h"

bool FilterUtils::Create(FilterInfo &filterInfo, const json resource)
{
	// Parse filter info
	filterInfo.id = resource["id"].get<string>();
	filterInfo.name = resource["name"].get<string>();
	filterInfo.type = GetMediaType(filterInfo.name);

	// Create a top info group
	EncoderGroupInfo groupInfo;
	groupInfo.id = "filter.info";
	groupInfo.name = Trans(groupInfo.id);
	groupInfo.groupClass = "basic";

	// Compute capability is read-only
	EncoderOptionInfo optionInfo;
	optionInfo.id = groupInfo.id + ".name";
	optionInfo.name = Trans(optionInfo.id, "label");
	optionInfo.description = wxEmptyString;
	optionInfo.parameter = "_name";
	optionInfo.isForced = true;
	optionInfo.control.type = EncoderOptionType::String;
	optionInfo.control.value.stringValue = filterInfo.name;
	optionInfo.control.enabled = false;
	groupInfo.options.push_back(optionInfo);

	filterInfo.groups.push_back(groupInfo);

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

static inline void show_help_children(const AVClass *c, FilterInfo &filterInfo)
{
	AVOptionRanges *r;
	const AVClass *child = NULL;
	if (c->option)
	{
		const AVOption *opt = NULL;
		const AVOption *optNext = av_opt_next(&c, opt);

		//
		EncoderGroupInfo groupInfo;
		groupInfo.id = "filter." + wxString(c->class_name);
		groupInfo.name = Trans(groupInfo.id);
		groupInfo.groupClass = "standard";

		//
		EncoderOptionInfo optionInfo;

		//
		while (optNext)
		{
			// Iterate
			opt = optNext;
			optNext = av_opt_next(&c, opt);

			// Create new filter option
			if (opt->type != AV_OPT_TYPE_CONST)
			{
				optionInfo.parameter = wxString(opt->name);
				optionInfo.id = groupInfo.id + "." + optionInfo.parameter;
				optionInfo.name = Trans(optionInfo.id, "label");
				optionInfo.description = wxString(opt->help);

				// Set type
				if (optNext && optNext->type == AV_OPT_TYPE_CONST)
				{
					optionInfo.control.type = EncoderOptionType::ComboBox;
					optionInfo.control.items.clear();
				}
				else
				{
					// Set option type
					switch (opt->type) {
					case AV_OPT_TYPE_INT:
					case AV_OPT_TYPE_INT64:
						optionInfo.control.type = EncoderOptionType::Integer;
						break;

					case AV_OPT_TYPE_BOOL:
						optionInfo.control.type = EncoderOptionType::Boolean;
						break;

					case AV_OPT_TYPE_FLOAT:
					case AV_OPT_TYPE_DOUBLE:
						optionInfo.control.type = EncoderOptionType::Float;
						break;

					case AV_OPT_TYPE_STRING:
					case AV_OPT_TYPE_DURATION:
					case AV_OPT_TYPE_CHANNEL_LAYOUT:
					case AV_OPT_TYPE_PIXEL_FMT:
					case AV_OPT_TYPE_SAMPLE_FMT:
						optionInfo.control.type = EncoderOptionType::String;
						break;

					default:
						optionInfo.control.type = EncoderOptionType::String; // TODO
					}

					groupInfo.options.push_back(optionInfo);
				}
			}
			else
			{
				// Create combo box item
				EncoderOptionInfo::ComboItem item;
				item.id = groupInfo.id + "._item_" + to_string(optionInfo.control.items.size());
				item.name = wxString(opt->name);
				item.value = wxString(opt->name);

				optionInfo.control.items.push_back(item);

				//
				if (!optNext || optNext->type != AV_OPT_TYPE_CONST)
				{
					groupInfo.options.push_back(optionInfo);
				}
			}

			//if (av_opt_query_ranges(&r, &c, opt->name, AV_OPT_SEARCH_FAKE_OBJ) >= 0) {
			//	switch (opt->type) {
			//	case AV_OPT_TYPE_INT:
			//	case AV_OPT_TYPE_INT64:
			//	case AV_OPT_TYPE_UINT64:
			//	case AV_OPT_TYPE_DOUBLE:
			//	case AV_OPT_TYPE_FLOAT:
			//	case AV_OPT_TYPE_RATIONAL:
			//		for (int i = 0; i < r->nb_ranges; i++) {
			//			OutputDebugStringA(wxString::Format("From %d to &d", r->range[i]->value_min, r->range[i]->value_max));
			//		}
			//		break;
			//	}
			//	av_opt_freep_ranges(&r);
			//}
		}

		filterInfo.groups.push_back(groupInfo);
	}
	
	while (child = av_opt_child_class_next(c, child))
	    show_help_children(child, filterInfo);
}

void FilterUtils::AutoFill(vector<FilterInfo> &filterInfos)
{
	const AVFilter *filter = NULL;
	void *opaque = NULL;

	while ((filter = av_filter_iterate(&opaque)))
	{
		// Support only filters of the same media type
		if (avfilter_pad_count(filter->inputs) != 1 ||
			avfilter_pad_count(filter->outputs) != 1 ||
			avfilter_pad_get_type(filter->inputs, 0) != avfilter_pad_get_type(filter->outputs, 0) ||
			filter->priv_class == NULL)
			continue;

		// Create filter info
		FilterInfo filterInfo;
		filterInfo.name = filter->name;
		filterInfo.id = "filter." + filterInfo.name;
		filterInfo.type = avfilter_pad_get_type(filter->inputs, 0);

		// Create a top info group
		EncoderGroupInfo groupInfo;
		groupInfo.id = "filter.info";
		groupInfo.name = Trans(groupInfo.id);
		groupInfo.groupClass = "basic";

		// Compute capability is read-only
		EncoderOptionInfo optionInfo;
		optionInfo.id = groupInfo.id + ".name";
		optionInfo.name = Trans(optionInfo.id, "label");
		optionInfo.description = wxEmptyString;
		optionInfo.parameter = "_name";
		optionInfo.isForced = true;
		optionInfo.control.type = EncoderOptionType::String;
		optionInfo.control.value.stringValue = filterInfo.name;
		optionInfo.control.enabled = false;
		groupInfo.options.push_back(optionInfo);

		filterInfo.groups.push_back(groupInfo);

		show_help_children(filter->priv_class, filterInfo);

		filterInfos.push_back(filterInfo);
	}
}