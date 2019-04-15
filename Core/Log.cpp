#include "Log.h"
#include <intrin.h>  

Log* Log::instance()
{
	static Log instance;
	return &instance;
}

Log::Log()
{
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
			AddLine(wxString::Format("GPU #%d: %S", 
				gpu++,
				DispDev.DeviceString));

		nDeviceIndex++;
	}
	
	AddLine("---------------------------------------------");
}

void Log::AddLine(wxString line)
{
	_log.Append("[" + wxDateTime::Now().FormatISOTime() + "] " + line + "\n");
}

void Log::Clear()
{
	_log.Clear();
}

wxString Log::GetAsString()
{
	return _log;
}