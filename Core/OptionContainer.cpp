#include "OptionContainer.h"
#include <string>
#include <sstream>

void OptionContainer::fromString(const wxString input, const char parameterSeparator, const char valueSeparator)
{
	istringstream iss(input.ToStdString());

	string token;
	while (getline(iss, token, parameterSeparator))
	{
		size_t pos = token.find(valueSeparator);
		string key = token.substr(0, pos);
		string value = token.substr(pos + 1);
		insert(make_pair(key, value));
	}
}

const wxString OptionContainer::toString(const bool includePrivate, wxString prefix, char parameterSeparator, char valueSeparator)
{
	stringstream out;

	//
	for (auto item : *this)
	{
		if (!includePrivate && item.first.at(0) == '_')
		{
			continue;
		}

		out << parameterSeparator << prefix << item.first << valueSeparator << item.second;
	}

	return (out.str().length() > 0) ? out.str().substr(1) : "";
}