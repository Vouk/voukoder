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

#include <wx/wx.h>
#include <wx/propgrid/propgrid.h>
#include <wx/richtext/richtextctrl.h>
#include "wxPGOptionProperty.h"
#include "EncoderInfo.h"

#define VKDR_GRID_COLOR_BASIC 0xffffff
#define VKDR_GRID_COLOR_STANDARD 0xcdffff
#define VKDR_GRID_COLOR_OTHER 0xfff0f0

class wxOptionEditor : public wxPanel
{
public:
	OptionContainer results;
	wxOptionEditor() : wxOptionEditor(NULL) {};
	wxOptionEditor(wxWindow *parent, bool hasPreview = true, bool hasAdvancedSwitch = true);
	void Configure(EncoderInfo encoderInfo, OptionContainer options);
	void SetAdvancedMode(bool advanced);

protected:
	wxPropertyGrid* m_propertyGrid = NULL;
	wxRichTextCtrl* m_preview = NULL;
	wxButton* m_btnReset = NULL;
	wxButton* m_btnClear = NULL;
	wxCheckBox* m_advCheck = NULL;

private:
	EncoderInfo encoderInfo;
	OptionContainer options;
	bool hasPreview = true;
	bool hasAdvancedSwitch = true;
	bool hasAdvancedOptions = false;
	void DeflateGroups(EncoderInfo encoderInfo, OptionContainer& options);
	void ExecuteFilters(wxOptionProperty *optionProperty);
	void OnPropertyGridChanged(wxPropertyGridEvent& event);
	void OnPropertyGridCheckboxChanged(wxPropertyGridEvent& event);
	void OnLeftDown(wxMouseEvent& event);
	void OnResetClick(wxCommandEvent& event);
	void OnClearClick(wxCommandEvent& event);
	void OnShowAdvancedOptions(wxCommandEvent& event);
	void RefreshResults();
	bool SendEvent(wxEventType eventType, wxPGProperty* p);

	wxDECLARE_DYNAMIC_CLASS(wxOptionEditor);
	wxDECLARE_EVENT_TABLE();
};