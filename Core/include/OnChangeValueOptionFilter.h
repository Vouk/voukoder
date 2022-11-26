/**
 * Voukoder
 * Copyright (C) 2017-2022 Daniel Stankewitz, All Rights Reserved
 * https://www.voukoder.org
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 * http://www.gnu.org/copyleft/gpl.html
 */
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
