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
#pragma once

#include <wx/wx.h>
#include <wx/notebook.h>
#include "Voukoder.h"
#include "OptionContainer.h"
#include "wxOptionEditor.h"

class wxEditFilterDialog : public wxDialog
{
public:
	wxEditFilterDialog(wxWindow* parent, EncoderInfo filterInfo, OptionContainer** options);
	void ApplyChanges();

protected:
	wxOptionEditor* m_editor = NULL;

private:
	EncoderInfo filterInfo;
	OptionContainer** options = NULL;
	void OnOkayClick(wxCommandEvent& event);
};
