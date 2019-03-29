#include "NvidiaCustomOptions.h"
#include "Translation.h"

bool NvidiaCustomOptions::GetOptions(EncoderInfo &encoderInfo)
{
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
		string deviceName = name;
		wstring wdeviceName(deviceName.begin(), deviceName.end());

		// Create an entry per GPU
		EncoderOptionInfo::ComboItem comboItem;
		comboItem.id = groupInfo.id + "._item_" + to_string(i);
		comboItem.name = wdeviceName;
		comboItem.value = to_string(i);

		// Create a filter to change the compute capability together with the selected GPU
		OptionFilterInfo::Arguments arguments;
		arguments.insert(make_pair("id", groupInfo.id + ".computecapability"));
		arguments.insert(make_pair("value", to_string(major) + "." + to_string(minor)));

		vector<OptionFilterInfo::Arguments> argumentsList;
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
}
