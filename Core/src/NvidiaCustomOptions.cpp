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
#include "NvidiaCustomOptions.h"
#include "LanguageUtils.h"

bool NvidiaCustomOptions::GetOptions(EncoderInfo &encoderInfo)
{
#ifdef _WIN32

	// Load cuda
	HMODULE cuda = LoadLibraryW(L"nvcuda.dll");
	if (cuda == NULL)
	{
		return false;
	}

	// Functions from nvcuda.dll
	__cuDeviceGetCount cuDeviceGetCount = (__cuDeviceGetCount)GetProcAddress(cuda, "cuDeviceGetCount");
	__cuDeviceGetName cuDeviceGetName = (__cuDeviceGetName)GetProcAddress(cuda, "cuDeviceGetName");
	__cuDeviceComputeCapability cuDeviceComputeCapability = (__cuDeviceComputeCapability)GetProcAddress(cuda, "cuDeviceComputeCapability");

	// Create a top hardware group
	EncoderGroupInfo groupInfo;
	groupInfo.id = encoderInfo.id + ".hardware";
	groupInfo.name = Trans(groupInfo.id);
	groupInfo.groupClass = "basic";

	// Create GPU option
	EncoderOptionInfo optionInfo;
	optionInfo.id = groupInfo.id + ".gpu";
	optionInfo.name = Trans(optionInfo.id, "label");
	optionInfo.description = Trans(optionInfo.id, "description");
	optionInfo.parameter = "gpu";
	optionInfo.isForced = true;
	optionInfo.control.type = EncoderOptionType::ComboBox;

	// Get number of installed cuda devices
	int deviceCount = 0;
	cuDeviceGetCount(&deviceCount);

	char name[100];
	int len = sizeof(name);
	int major = 0;
	int minor = 0;

	// Iterate cuda devices and get names and compute capability
	for (int i = 0; i < deviceCount; i++)
	{
		// Get the device name and versions
		cuDeviceGetName(name, len, i);
		cuDeviceComputeCapability(&major, &minor, 0);

		// Convert to wstring
		std::string deviceName = name;
		std::wstring wdeviceName(deviceName.begin(), deviceName.end());

		// Create an entry per GPU
		EncoderOptionInfo::ComboItem comboItem;
		comboItem.id = groupInfo.id + "._item_" + std::to_string(i);
		comboItem.name = wdeviceName;
		comboItem.value = std::to_string(i);

		// Create a filter to change the compute capability together with the selected GPU
		OptionFilterInfo::Arguments arguments;
		arguments.insert(std::make_pair("id", groupInfo.id + ".computecapability"));
		arguments.insert(std::make_pair("value", std::to_string(major) + "." + std::to_string(minor)));

		std::vector<OptionFilterInfo::Arguments> argumentsList;
		argumentsList.push_back(arguments);

		OptionFilterInfo filterInfo;
		filterInfo.name = "OnSelection";
		filterInfo.params.insert(make_pair("SetValue", argumentsList));

		comboItem.filters.push_back(filterInfo);

		optionInfo.control.items.push_back(comboItem);
	}

	groupInfo.options.push_back(optionInfo);

	// Compute capability is read-only
	EncoderOptionInfo optionInfo2;
	optionInfo2.id = groupInfo.id + ".computecapability";
	optionInfo2.name = Trans(optionInfo2.id);
	optionInfo2.name = Trans(optionInfo2.id, "label");
	optionInfo2.description = Trans(optionInfo2.id, "description");
	optionInfo2.parameter = "_computeCapability";
	optionInfo2.isForced = true;
	optionInfo2.control.type = EncoderOptionType::String;
	optionInfo2.control.value = "";
	optionInfo2.control.enabled = false;
	groupInfo.options.push_back(optionInfo2);

	encoderInfo.groups.push_back(groupInfo);

	// Unload cuda
	FreeLibrary(cuda);

	return true;

#else

		return false;

#endif
}
