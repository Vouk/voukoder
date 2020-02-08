#pragma once

#include <string>
#include <vector>
#include <wx/wx.h>
#include "ResourceInfo.h"

struct MuxerInfo : public ResourceInfo
{
	wxString extension;
	std::vector<std::string> videoCodecIds;
	std::vector<std::string> audioCodecIds;

	struct
	{
		bool faststart;
	} capabilities;
};
