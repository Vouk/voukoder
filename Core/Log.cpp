#include "Log.h"
#include <wx/dir.h>
#ifdef _WIN32
#include <intrin.h>
#endif
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
	file.Open(filename, wxFile::OpenMode::write_append);
	
	AddLine("=============================================");

	// Report voukoder version
	AddLine(wxString::Format("Voukoder %d.%d.%d",
		VKDR_VERSION_MAJOR,
		VKDR_VERSION_MINOR,
		VKDR_VERSION_PATCH));

	AddLine("by Daniel Stankewitz");
	AddSep();
#ifdef _WIN32
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
	AddLine(name);

	// Get CPU cores information
	SYSTEM_INFO sysInfo;
	GetSystemInfo(&sysInfo);
	AddLine(wxString::Format("%d logical cores", (int)sysInfo.dwNumberOfProcessors));

	// Get RAM information
	MEMORYSTATUSEX statex;
	statex.dwLength = sizeof(statex);
	GlobalMemoryStatusEx(&statex);
	AddLine(wxString::Format("%d MB system memory", (int)(statex.ullTotalPhys / 1024) / 1024));

	// Get GPU information
	DISPLAY_DEVICE DispDev;
	ZeroMemory(&DispDev, sizeof(DispDev));
	DispDev.cb = sizeof(DispDev);

	int nDeviceIndex = 0, gpu = 0;
	while (EnumDisplayDevices(NULL, nDeviceIndex, &DispDev, 0))
	{
		if (DispDev.StateFlags & DISPLAY_DEVICE_ACTIVE)
			AddLine(wxString::Format("Display #%d on: %S",
				gpu++,
				DispDev.DeviceString));

		nDeviceIndex++;
	}
#endif
	AddSep();
}

Log::~Log()
{
	if (file.IsOpened())
		file.Close();
}

void Log::AddSep()
{
	AddLine("---------------------------------------------");
}

void Log::AddLine(wxString line)
{
	if (file.IsOpened())
		file.Write ("[" + wxDateTime::Now().FormatISOTime() + "] " + line + "\n");
}

wxString Log::GetFilename()
{
	return filename;
}

wxString Log::GetBaseDir()
{
	wchar_t path[MAX_PATH];
	HMODULE hm = NULL;

	// Get Module instance
	if (GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, (LPCWSTR)& instance, &hm) != 0)
	{
		// Get instance (dll) filename
		if (GetModuleFileName(hm, path, sizeof(path)) != 0)
		{
			wxFileName filename(path);
			return filename.GetPath() + wxFileName::GetPathSeparator();
		}
	}

	return "";
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