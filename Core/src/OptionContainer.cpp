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
#include "OptionContainer.h"
#include <string>
#include <sstream>

OptionContainer::OptionContainer(OptionContainer* options)
{
	this->insert(options->begin(), options->end());
}

void OptionContainer::Deserialize(const wxString input, const char parameterSeparator, const char valueSeparator)
{
	clear();

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
