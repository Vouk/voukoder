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
#include "wxBanner.h"

wxBanner::wxBanner(wxWindow* parent, wxWindowID id, const wxGDIImage& label, BannerInfo info):
	wxStaticBitmap(parent, id, label, wxDefaultPosition, wxDefaultSize, 0L),
	info(info)
{
	Bind(wxEVT_LEFT_UP, &wxBanner::OnClick, this);
	Bind(wxEVT_ENTER_WINDOW, &wxBanner::OnEnter, this);
	Bind(wxEVT_LEAVE_WINDOW, &wxBanner::OnLeave, this);
}

wxBanner::~wxBanner()
{}

void wxBanner::OnClick(wxMouseEvent& event)
{
	wxLaunchDefaultBrowser(info.actionUrl, wxBROWSER_NEW_WINDOW);
}

void wxBanner::OnEnter(wxMouseEvent& event)
{
	SetCursor(wxCURSOR_HAND);
}

void wxBanner::OnLeave(wxMouseEvent& event)
{
	SetCursor(wxCURSOR_DEFAULT);
}