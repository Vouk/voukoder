#include "Settings.h"

Settings::Settings()
{
	// Get the DLL module handle
	MEMORY_BASIC_INFORMATION mbi;
	static int dummy;
	VirtualQuery(&dummy, &mbi, sizeof(mbi));
	HMODULE hModule = reinterpret_cast<HMODULE>(mbi.AllocationBase);

	// Load the JSON resource file
	HRSRC hRes = FindResourceEx(hModule, MAKEINTRESOURCE(ID_TEXT), MAKEINTRESOURCE(IDR_ID_TEXT1), MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL));
	if (NULL != hRes)
	{
		HGLOBAL hData = LoadResource(hModule, hRes);
		if (NULL != hData)
		{
			const DWORD dataSize = SizeofResource(hModule, hRes);
			char *resource = new char[dataSize + 1];
			memcpy(resource, LockResource(hData), dataSize);
			resource[dataSize] = 0;

			this->settings = json::parse(resource);
		}
	}
}

Settings::~Settings()
{
	this->settings = NULL;
}

json Settings::getConfiguration()
{
	return this->settings["configuration"];
}

json Settings::getDefaultAudioCodecId()
{
	return this->settings["defaults"]["audioEncoder"].get<int>();
}

json Settings::getDefaultMuxerId()
{
	return this->settings["defaults"]["muxer"].get<int>();
}

json Settings::getDefaultVideoCodecId()
{
	return this->settings["defaults"]["videoEncoder"].get<int>();
}

json Settings::getMuxers()
{
	return this->settings["muxers"];
}

json Settings::getVideoEncoders()
{
	return this->settings["videoEncoders"];
}

json Settings::getAudioEncoders()
{
	return this->settings["audioEncoders"];
}

json Settings::filterArrayById(json array, int id)
{
	for (auto iterator = array.begin(); iterator != array.end(); ++iterator)
	{
		json obj = *iterator;

		if (obj["id"].get<int>() == id)
		{
			return obj;
		}
	}

	return json::object();
}

json Settings::filterArrayByName(json array, std::string name)
{
	for (auto iterator = array.begin(); iterator != array.end(); ++iterator)
	{
		json obj = *iterator;

		if (obj["name"].get<std::string>() == name)
		{
			return obj;
		}
	}

	return json::object();
}

json Settings::filterArrayInArrayByName(json array, std::string name, std::string subname)
{
	for (auto iterator = array.begin(); iterator != array.end(); ++iterator)
	{
		json obj = *iterator;

		for (auto iterator2 = obj[name].begin(); iterator2 != obj[name].end(); ++iterator2)
		{
			json obj2 = *iterator2;

			if (obj2["name"].get<std::string>() == subname)
			{
				return obj2;
			}
		}
	}

	return json::object();
}

int Settings::getArrayIndex(json array, std::string value)
{
	int i = 0;
	for (auto iterator = array.begin(); iterator != array.end(); ++iterator, i++)
	{
		if (*iterator == value)
		{
			return i;
		}
	}

	return -1;
}

json Settings::find(json obj, std::string field, std::string value)
{
	for (auto iterator = obj.begin(); iterator != obj.end(); ++iterator)
	{
		if (obj.is_object())
		{
			if (iterator.key() == field && iterator.value() == value)
			{
				return obj;
			}
		}

		if (obj.is_object() || obj.is_array())
		{
			const json result = Settings::find(*iterator, field, value);

			if (!result.empty())
			{
				return result;
			}
		}
	}

	return json::object();
}