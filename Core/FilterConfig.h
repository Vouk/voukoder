#pragma once

#include <vector>
#include <wx/tokenzr.h>
#include "OptionContainer.h"

#define FF_FILTER_SEPARATOR ","
#define FF_PARAM_SEPARATOR ':'
#define FF_VALUE_SEPARATOR '='

struct FilterConfig
{
	std::vector<OptionContainer*> filters;

	void Deserialize(const wxString input)
	{
		filters.clear();

		// Process each filter
		wxStringTokenizer tokenizer(input, FF_FILTER_SEPARATOR);
		while (tokenizer.HasMoreTokens())
		{
			OptionContainer* options = new OptionContainer;
			options->Deserialize(tokenizer.GetNextToken());

			filters.push_back(options);
		}
	}

	wxString Serialize()
	{
		wxString config;

		for (auto filter : filters)
			config << FF_FILTER_SEPARATOR << filter->Serialize(true);

		return config.length() > 0 ? config.substr(1) : (wxString)wxEmptyString;
	}

	wxString AsFilterString()
	{
		wxString config;

		for (auto filter : filters)
		{
			config << FF_FILTER_SEPARATOR << filter->at("_name");

			wxString params = filter->Serialize(false, "", FF_PARAM_SEPARATOR, FF_VALUE_SEPARATOR);
			if (!params.IsEmpty())
				config << FF_VALUE_SEPARATOR << params;
		}

		return config;
	}
};
