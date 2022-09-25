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
#include "wxEncoderPanel.h"
#include "LanguageUtils.h"
#include <wx/gbsizer.h>
#include <wx/statline.h>

wxEncoderPanel::wxEncoderPanel(wxWindow* parent, std::vector<EncoderInfo> encoders):
	wxPanel(parent),
	encoders(encoders)
{
	wxBoxSizer* bLayout = new wxBoxSizer(wxVERTICAL);

	wxSize minLabelWidth = wxDLG_UNIT(this, wxSize(56, -1));

	// Encoder

	wxPanel* m_encoderPanel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
	wxStaticBoxSizer* sbEncoderSizer = new wxStaticBoxSizer(new wxStaticBox(m_encoderPanel, wxID_ANY, Trans("ui.voukoder.configuration.encoder.codec")), wxVERTICAL);

	wxArrayString m_encoderChoiceChoices;
	m_encoderChoice = new wxChoice(m_encoderPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_encoderChoiceChoices, wxCB_SORT);
	m_encoderChoice->Bind(wxEVT_COMMAND_CHOICE_SELECTED, &wxEncoderPanel::OnEncoderChanged, this);
	sbEncoderSizer->Add(m_encoderChoice, 0, wxEXPAND | wxALL, 5);

	// Populate encoders
	for (auto& encoder : this->encoders)
		m_encoderChoice->Append(encoder.name, (void*)&encoder);

	m_encoderPanel->SetSizer(sbEncoderSizer);
	m_encoderPanel->Layout();
	sbEncoderSizer->Fit(m_encoderPanel);
	bLayout->Add(m_encoderPanel, 0, wxEXPAND | wxALL, 5);

	// Presets

	wxPanel* m_presetsPanel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
	wxStaticBoxSizer* sbPresetsSizer = new wxStaticBoxSizer(new wxStaticBox(m_presetsPanel, wxID_ANY, Trans("ui.voukoder.configuration.encoder.presets")), wxVERTICAL);

	m_presetList = new wxListBox(m_presetsPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0 , NULL, 0);
	m_presetList->SetMinSize(wxDLG_UNIT(this, wxSize(-1, 100)));
	sbPresetsSizer->Add(m_presetList, 0, wxEXPAND | wxALL, 5);

	m_apply = new wxButton(m_presetsPanel, wxID_ANY, Trans("ui.voukoder.configuration.encoder.apply"), wxDefaultPosition, wxDefaultSize, 0);
	m_apply->Bind(wxEVT_BUTTON, &wxEncoderPanel::OnApplyPresetClick, this);
	m_apply->Enable(false);
	sbPresetsSizer->Add(m_apply, 0, wxALIGN_RIGHT | wxALL, 5);
	m_presetsPanel->SetSizer(sbPresetsSizer);
	m_presetsPanel->Layout();
	sbPresetsSizer->Fit(m_presetsPanel);
	bLayout->Add(m_presetsPanel, 0, wxEXPAND | wxALL, 5);

	// End

	SetSizer(bLayout);
	Layout();
	
	bLayout->Fit(this);
}

bool wxEncoderPanel::SelectEncoder(wxString id)
{
	for (auto& encoder : encoders)
	{
		if (encoder.id == id)
		{
			PopulatePresets(encoder.presets);

			return m_encoderChoice->SetStringSelection(encoder.name);
		}
	}

	return false;
}

EncoderInfo* wxEncoderPanel::GetSelectedEncoder()
{
	auto sel = m_encoderChoice->GetSelection();
	if (sel == wxNOT_FOUND)
		return NULL;

	return reinterpret_cast<EncoderInfo*>(m_encoderChoice->GetClientData(sel));
}

void wxEncoderPanel::PopulatePresets(std::vector<PresetInfo>& presets)
{
	m_presetList->Clear();

	for (auto& preset : presets)
		m_presetList->Append(preset.name, (void*)&preset);

	m_apply->Enable(m_presetList->GetCount() > 0);

	if (m_presetList->GetCount() > 0)
		m_presetList->Select(0);
}

void wxEncoderPanel::OnEncoderChanged(wxCommandEvent& event)
{
	EncoderInfo* encoderInfo = GetSelectedEncoder();

	PopulatePresets(encoderInfo->presets);

	wxEncoderChangedEvent e;
	e.SetEncoderInfo(encoderInfo);

	wxPostEvent(this, e);
}

void wxEncoderPanel::OnApplyPresetClick(wxCommandEvent& event)
{
	auto sel = m_presetList->GetSelection();
	if (sel == wxNOT_FOUND)
		return;

	wxApplyPresetEvent e;
	e.SetPresetInfo(reinterpret_cast<PresetInfo*>(m_presetList->GetClientData(sel)));

	wxPostEvent(this, e);
}