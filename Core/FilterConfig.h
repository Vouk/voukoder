/**
 * Voukoder
 * Copyright (C) 2017-2020 Daniel Stankewitz, All Rights Reserved
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

#include <vector>
#include <wx/tokenzr.h>
#include "Voukoder.h"

#define FF_FILTER_SEPARATOR ","
#define FF_ID_SEPARATOR '#'
#define FF_PARAM_SEPARATOR ':'
#define FF_VALUE_SEPARATOR '='

class FilterConfig : public std::vector<OptionContainer*>
{
public:
	void Deserialize(const wxString input)
	{
		clear();

		// Tokenize serialized string
		wxStringTokenizer tokenizer(input, FF_FILTER_SEPARATOR);
		while (tokenizer.HasMoreTokens())
		{
			wxString token = tokenizer.GetNextToken();

			// Create option container
			OptionContainer* options = new OptionContainer;
			options->id = token.Before(FF_ID_SEPARATOR);
			options->Deserialize(token.After(FF_ID_SEPARATOR));

			push_back(options);
		}
	}

	wxString Serialize()
	{
		wxString config;

		// Serialize stored options
		for (auto options : *this)
			config << FF_FILTER_SEPARATOR << options->id << FF_ID_SEPARATOR << options->Serialize(true);

		return config.length() > 0 ? config.substr(1) : "";
	}

	wxString AsFilterString()
	{
		wxString config;

		// Process each filter
		for (auto options : *this)
		{
			// Ignore filter as we only need it to set it in the codec context
			if (options->id == "filter.setparams")
				continue;

			// Filter name
			config << FF_FILTER_SEPARATOR << options->id.After('.');

			// Filter options
			wxString params = options->Serialize(false, "", FF_PARAM_SEPARATOR, FF_VALUE_SEPARATOR);
			if (!params.IsEmpty())
				config << FF_VALUE_SEPARATOR << params;
		}

		return config;
	}
};
