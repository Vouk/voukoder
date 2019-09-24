#pragma once

#include <wx/wx.h>

struct EncoderOptionPreset {
	wxString id;
	wxString name;
	wxString options;
};

struct EncoderOptionPresetGroup {
	wxString id;
	wxString name;
	std::vector<EncoderOptionPresetGroup> group;
	std::vector<EncoderOptionPreset> presets;
};
