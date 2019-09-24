#include "OptionContainer.h"
#include <string>
#include <sstream>

OptionContainer::OptionContainer(OptionContainer* options)
{
	this->insert(options->begin(), options->end());
}

void OptionContainer::Deserialize(const wxString input, const char parameterSeparator, const char valueSeparator)
{
	std::istringstream iss(input.ToStdString());

	std::string token;
	while (getline(iss, token, parameterSeparator))
	{
		size_t pos = token.find(valueSeparator);
		std::string key = token.substr(0, pos);
		std::string value = token.substr(pos + 1);
		insert(make_pair(key, value));
	}
}

const wxString OptionContainer::Serialize(const bool includePrivate, wxString prefix, char parameterSeparator, char valueSeparator)
{
	std::stringstream out;

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
