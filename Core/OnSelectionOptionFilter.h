#pragma once

#include "OptionFilter.h"

class OnSelectionOptionFilter : public OptionFilter
{
public:
	OnSelectionOptionFilter(OptionFilterInfo filterInfo, wxPropertyGrid *propertyGrid) : 
		OptionFilter(filterInfo, propertyGrid)
	{}

	void Execute()
	{
		// Process all params
		for (auto& param : filterInfo.params)
		{
			for (auto& option : param.second)
			{
				wxString targetId = option.at("id").stringValue;
				wxPGProperty *targetProperty = propertyGrid->GetPropertyByName(targetId);

				if (param.first == "ShowOptions")
				{
					bool hidden = !option.at("visible").boolValue;
					propertyGrid->HideProperty(targetProperty, hidden);
				}
				else if (param.first == "SetValue")
				{
					string value = option.at("value").stringValue;
					targetProperty->SetValueFromString(value);
				}
				else
				{
					continue;
				}
			}
		}
	}
};
