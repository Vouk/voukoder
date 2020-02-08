#pragma once

#include <set>
#include <wx/wx.h>
#include "EncoderInfo.h"
#include "PresetInfo.h"

class wxEncoderChangedEvent;
wxDECLARE_EVENT(wxEVT_ENCODER_CHANGED, wxEncoderChangedEvent);

class wxEncoderChangedEvent : public wxCommandEvent
{
public:
    wxEncoderChangedEvent(wxEventType commandType = wxEVT_ENCODER_CHANGED, int id = 0)
        : wxCommandEvent(commandType, id) {}

    wxEncoderChangedEvent(const wxEncoderChangedEvent& event)
        : wxCommandEvent(event),
        m_encoderInfo(event.m_encoderInfo) {}

    wxEvent* Clone() const { return new wxEncoderChangedEvent(*this); }

    EncoderInfo* GetEncoderInfo() const { return m_encoderInfo; }
    void SetEncoderInfo(EncoderInfo* encoderInfo) { m_encoderInfo = encoderInfo; }

protected:
    EncoderInfo* m_encoderInfo;
};

class wxApplyPresetEvent;
wxDECLARE_EVENT(wxEVT_APPLY_PRESET, wxApplyPresetEvent);

class wxApplyPresetEvent : public wxCommandEvent
{
public:
    wxApplyPresetEvent(wxEventType commandType = wxEVT_APPLY_PRESET, int id = 0)
        : wxCommandEvent(commandType, id) {}

    wxApplyPresetEvent(const wxApplyPresetEvent& event)
        : wxCommandEvent(event),
        m_presetInfo(event.m_presetInfo) {}

    wxEvent* Clone() const { return new wxApplyPresetEvent(*this); }

    PresetInfo* GetPresetInfo() const { return m_presetInfo; }
    void SetPresetInfo(PresetInfo* encoderInfo) { m_presetInfo = encoderInfo; }

protected:
    PresetInfo* m_presetInfo;
};

class wxEncoderPanel : public wxPanel
{
public:
	wxEncoderPanel(wxWindow* parent, std::set<EncoderInfo> encoders);
	bool SelectEncoder(wxString id);
    EncoderInfo* GetSelectedEncoder();

protected:
	wxChoice* m_encoderChoice = NULL;
	wxListBox* m_presetList = NULL;
    wxButton* m_apply = NULL;

private:
	std::set<EncoderInfo> encoders;
    void PopulatePresets(std::vector<PresetInfo>& presets);
	void OnEncoderChanged(wxCommandEvent& event);
    void OnApplyPresetClick(wxCommandEvent& event);
};