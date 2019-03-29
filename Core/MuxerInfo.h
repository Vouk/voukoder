#pragma once

#include <string>
#include <vector>
#include <wx/wx.h>
#include "ResourceInfo.h"

using namespace std;

struct MuxerInfo : public ResourceInfo
{
	wxString extension;
	vector<string> videoCodecIds;
	vector<string> audioCodecIds;

	struct
	{
		bool faststart;
	} capabilities;
};
