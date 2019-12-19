#pragma once

#include <wx/wx.h>
#include <wx/notebook.h>
#include "EncoderInfo.h"
#include "wxOptionEditor.h"
#include "wxFilterPanel.h"
#include "wxEncoderPanel.h"

struct TrackSettings
{
	OptionContainer options;
	OptionContainer sideData;
	FilterConfig filters;
};

class wxEncoderPage : public wxPanel
{
public:
	wxEncoderPage(wxWindow* parent, std::vector<EncoderInfo> encoders, EncoderInfo sideData, std::vector<EncoderInfo> filterInfos, TrackSettings& settings);
	bool SetEncoder(wxString id);
	EncoderInfo* GetSelectedEncoder();
	void ApplyChanges();

protected:
	wxEncoderPanel* m_encoderPanel = NULL;
	wxOptionEditor* m_encoderOptions = NULL;
	wxOptionEditor* m_sideDataOptions = NULL;
	wxFilterPanel* m_filterPanel = NULL;

private:
	TrackSettings& settings;
	void OnEncoderChanged(wxEncoderChangedEvent& event);
	void OnApplyPreset(wxApplyPresetEvent& event);
};