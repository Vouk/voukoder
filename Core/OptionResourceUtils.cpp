#include <map>
#include "OptionResourceUtils.h"
#include "LanguageUtils.h"

bool OptionResourceUtils::CreateOptionInfo(EncoderOptionInfo &optionInfo, const json resource)
{
	optionInfo.id = resource["id"].get<std::string>();
	optionInfo.name = Trans(optionInfo.id, "label");
	optionInfo.description = Trans(optionInfo.id, "description");

	// Optional: Is a multiplication factor set?
	if (resource.find("multiplicationFactor") != resource.end())
	{
		optionInfo.multiplicationFactor = resource["multiplicationFactor"].get<int>();
	}

	// Optional: Is it a forced parameter?
	if (resource.find("forced") != resource.end())
	{
		optionInfo.isForced = resource["forced"].get<bool>();
	}

	// Optional: Is this property assigned to a parameter name?
	if (resource.find("parameter") != resource.end())
	{
		optionInfo.parameter = resource["parameter"].get<std::string>();
	}

	// Optional: 
	if (resource.find("preprendNoIfFalse") != resource.end())
	{
		optionInfo.preprendNoIfFalse = resource["preprendNoIfFalse"].get<bool>();
	}

	// Get the control type
	std::string type = resource["control"]["type"].get<std::string>();

	// Parse data types
	if (type == "integer")
	{
		optionInfo.control.type = EncoderOptionType::Integer;
		optionInfo.control.minimum.intValue = resource["control"]["minimum"].get<int>();
		optionInfo.control.maximum.intValue = resource["control"]["maximum"].get<int>();
		optionInfo.control.singleStep.intValue = resource["control"]["singleStep"].get<int>();
		optionInfo.control.value.intValue = resource["control"]["value"].get<int>();
	}
	else if (type == "float")
	{
		optionInfo.control.type = EncoderOptionType::Float;
		optionInfo.control.minimum.floatValue = resource["control"]["minimum"].get<float>();
		optionInfo.control.maximum.floatValue = resource["control"]["maximum"].get<float>();
		optionInfo.control.singleStep.floatValue = resource["control"]["singleStep"].get<float>();
		optionInfo.control.value.floatValue = resource["control"]["value"].get<float>();
	}
	else if (type == "boolean")
	{
		optionInfo.control.type = EncoderOptionType::Boolean;
		optionInfo.control.value.boolValue = resource["control"]["value"].get<bool>();
	}
	else if (type == "string")
	{
		optionInfo.control.type = EncoderOptionType::String;
		optionInfo.control.value.stringValue = resource["control"]["value"].get<std::string>();

		if (resource["control"].find("regex") != resource["control"].end())
		{
			optionInfo.control.regex = resource["control"]["regex"].get<std::string>();
		}
	}
	else if (type == "combobox")
	{
		optionInfo.control.type = EncoderOptionType::ComboBox;

		// Default selected item
		optionInfo.control.selectedIndex = resource["control"]["selectedIndex"].get<int>();

		// All items
		int idx = 0;
		for (json item : resource["control"]["items"])
		{
			// Create combo item
			EncoderOptionInfo::ComboItem comboItem;
			comboItem.id = optionInfo.id + "._item_" + std::to_string(idx++);
			comboItem.name = Trans(comboItem.id);

			if (item.find("value") != item.end())
			{
				comboItem.value = item["value"].get<std::string>();
			}

			// Parse filters
			CreateOptionFilterInfos(comboItem.filters, item, optionInfo.id);

			optionInfo.control.items.push_back(comboItem);
		}
	}

	// Parse filters
	CreateOptionFilterInfos(optionInfo.filters, resource, optionInfo.id);

	return true;
}

bool OptionResourceUtils::CreateOptionFilterInfos(std::vector<OptionFilterInfo> &filters, json resource, std::string id)
{
	if (resource.find("filters") != resource.end())
	{
		for (json filterDefinition : resource["filters"])
		{
			OptionFilterInfo optionFilterInfo;
			if (CreateOptionFilterInfo(optionFilterInfo, filterDefinition, id))
			{
				filters.push_back(optionFilterInfo);
			}
			else
			{
				return false;
			}
		}
	}

	return true;
}

bool OptionResourceUtils::CreateOptionFilterInfo(OptionFilterInfo &optionFilterInfo, const json resource, const std::string ownerId)
{
	optionFilterInfo.name = resource["filter"].get<std::string>();
	optionFilterInfo.ownerId = ownerId;

	OptionFilterInfo::Params params;

	for (auto param : resource["params"].items())
	{
		std::vector<OptionFilterInfo::Arguments> options;

		for (auto& item : param.value())
		{
			OptionFilterInfo::Arguments option;

			for (auto b : item.items())
			{
				json data = b.value();

				OptionValue value;
				if (data.is_string())
				{
					value.stringValue = data.get<std::string>();
				}
				else if (data.is_number_integer())
				{
					value.intValue = data.get<int>();
				}
				else if (data.is_number_float())
				{
					value.floatValue = data.get<float>();
				}
				else if (data.is_boolean())
				{
					value.boolValue = data.get<bool>();
				}

				option.insert(make_pair(b.key(), value));
			}

			options.push_back(option);
		}

		optionFilterInfo.params.insert(make_pair(param.key(), options));
	}

	return true;
}
