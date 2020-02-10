/**
 * Voukoder
 * Copyright (C) 2017-2020 Daniel Stankewitz, All Rights Reserved
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
#include "wxEncoderPage.h"
#include "LanguageUtils.h"

wxDEFINE_EVENT(wxEVT_CHECKBOX_CHANGE, wxPropertyGridEvent);
wxDEFINE_EVENT(wxEVT_ENCODER_CHANGED, wxEncoderChangedEvent);
wxDEFINE_EVENT(wxEVT_APPLY_PRESET, wxApplyPresetEvent);

wxEncoderPage::wxEncoderPage(wxWindow* parent, std::vector<EncoderInfo> encoders, EncoderInfo sideData, std::vector<EncoderInfo> filterInfos, TrackSettings& settings) :
	wxPanel(parent), settings(settings)
{
	wxBoxSizer* bLayout = new wxBoxSizer(wxVERTICAL);

	wxNotebook* m_notebook = new wxNotebook(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0);
	m_encoderPanel = new wxEncoderPanel(m_notebook, encoders);
	m_encoderPanel->Bind(wxEVT_ENCODER_CHANGED, &wxEncoderPage::OnEncoderChanged, this);
	m_encoderPanel->Bind(wxEVT_APPLY_PRESET, &wxEncoderPage::OnApplyPreset, this);
	m_notebook->AddPage(m_encoderPanel, Trans("ui.voukoder.configuration.encoder"), true);

	// Encoder config
	m_encoderOptions = new wxOptionEditor(m_notebook);
	m_notebook->AddPage(m_encoderOptions, Trans("ui.voukoder.configuration.options"), false);

	// Side data
	if (sideData.groups.size() > 0)
	{
		m_sideDataOptions = new wxOptionEditor(m_notebook, false, false);
		m_notebook->AddPage(m_sideDataOptions, Trans("ui.voukoder.configuration.sidedata"), false);
		m_sideDataOptions->Configure(sideData, settings.sideData);
	}

	// Filters
	if (filterInfos.size() > 0)
	{
		m_filterPanel = new wxFilterPanel(m_notebook, filterInfos);
		m_notebook->AddPage(m_filterPanel, Trans("ui.voukoder.configuration.filters"), false);
		m_filterPanel->Configure(settings.filters);
	}

	bLayout->Add(m_notebook, 1, wxEXPAND | wxALL, 0);

	this->SetSizer(bLayout);
	this->Layout();
	this->Centre(wxBOTH);
}

bool wxEncoderPage::SetEncoder(wxString id)
{
	bool res = m_encoderPanel->SelectEncoder(id);
	if (res)
	{
		EncoderInfo* encoderInfo = GetSelectedEncoder();
		m_encoderOptions->Configure(*encoderInfo, settings.options);
	}

	return res;
}

EncoderInfo* wxEncoderPage::GetSelectedEncoder()
{
	return m_encoderPanel->GetSelectedEncoder();
}

void wxEncoderPage::ApplyChanges()
{
	// Set encoder config
	settings.options.clear();
	settings.options.insert(m_encoderOptions->results.begin(), m_encoderOptions->results.end());

	// Set side data config
	settings.sideData.clear();
	if (m_sideDataOptions)
		settings.sideData.insert(m_sideDataOptions->results.begin(), m_sideDataOptions->results.end());

	// Filters
	settings.filters.clear();
	if (m_filterPanel)
		m_filterPanel->GetFilterConfig(settings.filters);
}

void wxEncoderPage::OnEncoderChanged(wxEncoderChangedEvent& event)
{
	// Get selected encoder info
	auto encoderInfo = event.GetEncoderInfo();
	if (encoderInfo == NULL)
		return;

	// Unfold param groups
	for (auto& paramGroup : encoderInfo->paramGroups)
	{
		if (settings.options.find(paramGroup.first) != settings.options.end())
		{
			wxStringTokenizer tokens(settings.options[paramGroup.first], ":");
			while (tokens.HasMoreTokens())
			{
				wxString token = tokens.GetNextToken();
				settings.options.insert(
					std::make_pair(token.BeforeFirst('='), token.AfterFirst('=')));
			}

			settings.options.erase(paramGroup.first);
		}
	}

	m_encoderOptions->Configure(*encoderInfo, settings.options);

	wxPostEvent(this, event);
}

void wxEncoderPage::OnApplyPreset(wxApplyPresetEvent& event)
{
	// Get selected preset info
	auto presetInfo = event.GetPresetInfo();
	if (presetInfo == NULL)
		return;

	// Load preset options
	settings.options.clear();
	settings.options.insert(presetInfo->options.begin(), presetInfo->options.end());

	// Apply new options
	auto encoderInfo = m_encoderPanel->GetSelectedEncoder();
	m_encoderOptions->Configure(*encoderInfo, settings.options);

	wxMessageBox(Trans("ui.voukoder.configuration.encoder.confirmation"), VKDR_APPNAME, wxICON_INFORMATION);
}