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
#include "Log.h"
#include <wx/dir.h>
#include <intrin.h>
#include "Version.h"

#define LOGDIR "Logs"

Log* Log::instance()
{
	static Log instance;
	return &instance;
}

Log::Log()
{
	// Delete all logfiles older than 7 days
	Clean(7);

	// Open log file for writing / appending
	filename = CreateFileName();
	auto file = new wxFile(filename, wxFile::OpenMode::write_append);
	files.insert_or_assign(filename, file);

	Init(file);
}

Log::~Log()
{
	// Close all open files
	for (auto& file : files)
	{
		if (file.second->IsOpened())
			file.second->Close();
	}

	files.clear();
}

void Log::Init(wxFile* file)
{
	AddLineToFile(file, "=============================================");

	// Report voukoder version
	AddLineToFile(file, wxString::Format("Voukoder %s (%d.%d.%d)",
		VKDR_VERSION_PUBLIC,
		VKDR_VERSION_MAJOR,
		VKDR_VERSION_MINOR,
		VKDR_VERSION_PATCH));

	AddLineToFile(file, "by Daniel Stankewitz");
	AddLineToFile(file, "---------------------------------------------");

	int CPUInfo[4] = { -1 };
	unsigned   nExIds, i = 0;
	char name[0x40];
	__cpuid(CPUInfo, 0x80000000);
	nExIds = CPUInfo[0];
	for (i = 0x80000000; i <= nExIds; ++i)
	{
		__cpuid(CPUInfo, i);
		if (i == 0x80000002)
			memcpy(name, CPUInfo, sizeof(CPUInfo));
		else if (i == 0x80000003)
			memcpy(name + 16, CPUInfo, sizeof(CPUInfo));
		else if (i == 0x80000004)
			memcpy(name + 32, CPUInfo, sizeof(CPUInfo));
	}
	AddLineToFile(file, name);

	// Get CPU cores information
	SYSTEM_INFO sysInfo;
	GetSystemInfo(&sysInfo);
	AddLineToFile(file, wxString::Format("%d logical cores", (int)sysInfo.dwNumberOfProcessors));

	// Get RAM information
	MEMORYSTATUSEX statex;
	statex.dwLength = sizeof(statex);
	GlobalMemoryStatusEx(&statex);
	AddLineToFile(file, wxString::Format("%d MB system memory", (int)(statex.ullTotalPhys / 1024) / 1024));

	// Get GPU information
	DISPLAY_DEVICE DispDev;
	ZeroMemory(&DispDev, sizeof(DispDev));
	DispDev.cb = sizeof(DispDev);

	int nDeviceIndex = 0, gpu = 0;
	while (EnumDisplayDevices(NULL, nDeviceIndex, &DispDev, 0))
	{
		if (DispDev.StateFlags & DISPLAY_DEVICE_ACTIVE)
			AddLineToFile(file, wxString::Format("Display #%d on: %S",
				gpu++,
				DispDev.DeviceString));

		nDeviceIndex++;
	}

	AddLineToFile(file, "---------------------------------------------");
}

void Log::AddFile(wxString filename)
{
	auto file = new wxFile(filename, wxFile::OpenMode::write);
	files.insert_or_assign(filename, file);

	Init(file);
}

void Log::RemoveFile(wxString filename)
{
	wxFile* file = NULL;
	try
	{
		file = files.at(filename);
		file->Flush();
		file->Close();

		files.erase(filename);
	}
	catch (const std::out_of_range & e)
	{
		AddLine("Unable to remove file " + filename + " from logging.");
	}
}

void Log::AddSep()
{
	AddLine("---------------------------------------------");
}

void Log::AddLine(wxString line)
{
	// Append to all open files
	for (auto& file : files)
		AddLineToFile(file.second, line);
}

void Log::AddLineToFile(wxFile* file, wxString line)
{
	// Append to a single file
	if (file->IsOpened())
		file->Write("[" + wxDateTime::Now().FormatISOTime() + "] " + line + "\n");
}

wxString Log::GetFilename()
{
	return filename;
}

wxString Log::GetBaseDir()
{
	wxString baseDir;
	if (!wxGetEnv("LOCALAPPDATA", &baseDir))
		return "";

	// Does the logs dir exist?
	if (!wxDirExists(baseDir + wxFileName::GetPathSeparator() + "Voukoder"))
		wxMkDir(baseDir + wxFileName::GetPathSeparator() + "Voukoder");

	return baseDir + wxFileName::GetPathSeparator() + "Voukoder" + wxFileName::GetPathSeparator();
}

wxString Log::CreateFileName()
{
	wxString baseDir = GetBaseDir();

	// Proceed if we have a basedir
	if (baseDir.IsEmpty())
		return "";

	// Does the logs dir exist?
	if (!wxDirExists(baseDir + LOGDIR))
		wxMkDir(baseDir + LOGDIR);

	return baseDir + LOGDIR + wxFileName::GetPathSeparator() + wxDateTime::Now().FormatISODate() + ".log";
}

void Log::Clean(int days)
{
	// Define the directory we log to
	wxString logDir = GetBaseDir() + LOGDIR + wxFileName::GetPathSeparator();

	// Skip if we don't have a log dir
	if (!wxFileName::Exists(logDir))
		return;

	wxDir dir(logDir);
	wxDateTime date;

	// Loop over all files in that directory
	wxString name;
	while (dir.GetNext(&name))
	{
		// Process only log files
		if (!name.EndsWith(".log"))
			continue;

		// Parse filename as date
		if (date.ParseDate(name.Before('.')))
		{
			// Delete all files older than <days> days
			if (date.Add(wxTimeSpan::Days(days)) < wxDateTime::Now())
				wxRemoveFile(logDir + name);
		}
	}
}