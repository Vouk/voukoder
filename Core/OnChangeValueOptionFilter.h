#pragma once

#include "OptionFilter.h"

class OnChangeValueOptionFilter : public OptionFilter
{
public:
	OnChangeValueOptionFilter(OptionFilterInfo filterInfo, wxPropertyGrid *propertyGrid) :
		OptionFilter(filterInfo, propertyGrid)
	{}

	void Execute()
	{
		wxPGProperty *optionProperty = propertyGrid->GetPropertyByName(filterInfo.ownerId);
		if (optionProperty)
		{
			wxVariant value = optionProperty->GetValue();

			for (auto& param : filterInfo.params)
			{
				for (auto& option : param.second)
				{
					wxString targetId = option.at("id").stringValue;
					wxPGProperty *targetProperty = propertyGrid->GetPropertyByName(targetId);
					wxVariant targetValue = targetProperty->GetValue();

					if (param.first == "LimitMinValue")
					{
						// Min is greater than Max
						if (targetValue.GetInteger() > value.GetInteger())
						{
							targetProperty->SetValue(value);
						}
					}
					else if (param.first == "LimitMaxValue")
					{
						// Max is less than Min
						if (targetValue.GetInteger() < value.GetInteger())
						{
							targetProperty->SetValue(value);
						}
					}
				}
			}
		}
	}
};
