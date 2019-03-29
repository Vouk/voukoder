#include "stdafx.h"
#include "OptionFilterFactory.h"

using namespace std;

OptionFilter OptionFilterFactory::CreateFilter(const json filterConfig)
{
	string name = filterConfig["name"].get<string>();

	OptionFilter filter;
	if (name == "OnSelection")
	{
		filter = OnSelectionOptionFilter();
	}

	filter.Init(filterConfig["arguments"]);

	return filter;
}
