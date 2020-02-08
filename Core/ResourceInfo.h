#pragma once

#include <wx/wx.h>

struct ResourceInfo
{
	wxString id;
	wxString name;

	bool operator< (const ResourceInfo& other) const
	{
		return (id < other.id);
	}

	bool operator== (const ResourceInfo& other) const
	{
		return (id == other.id);
	}
};