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
#pragma once

#include <vector>
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
	wxEncoderPanel(wxWindow* parent, std::vector<EncoderInfo> encoders);
	bool SelectEncoder(wxString id);
    EncoderInfo* GetSelectedEncoder();

protected:
	wxChoice* m_encoderChoice = NULL;
	wxListBox* m_presetList = NULL;
    wxButton* m_apply = NULL;

private:
	std::vector<EncoderInfo> encoders;
    void PopulatePresets(std::vector<PresetInfo>& presets);
	void OnEncoderChanged(wxCommandEvent& event);
    void OnApplyPresetClick(wxCommandEvent& event);
};