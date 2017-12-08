#include "Settings.h"

// reviewed 0.3.8
Settings::Settings(HMODULE hModule)
{
	// Load the JSON resource file
	mainConfig = loadResource(hModule, MAKEINTRESOURCE(ID_TEXT), MAKEINTRESOURCE(IDR_ID_TEXT3));
	muxerConfig = loadResource(hModule, MAKEINTRESOURCE(ID_TEXT), MAKEINTRESOURCE(IDR_ID_TEXT2));
	encoderConfig = loadResource(hModule, MAKEINTRESOURCE(ID_TEXT), MAKEINTRESOURCE(IDR_ID_TEXT1));;
}

// reviewed 0.3.8
vector<EncoderInfo> Settings::getEncoders(EncoderType encoderType)
{
	// Get right encoder list
	json encoders;
	switch (encoderType)
	{
	case EncoderType::VIDEO:
		encoders = encoderConfig["video"];
		break;

	case EncoderType::AUDIO:
		encoders = encoderConfig["audio"];
		break;
	}

	// Create EncoderInfo list
	vector<EncoderInfo> encoderInfos;
	for (json encoder : encoders)
	{
		EncoderInfo encoderInfo = EncoderInfo(encoder);
#if !defined(_DEBUG) 
		if (!encoderInfo.experimental)
		{
#endif
			if (encoderInfo.dependencies)
			{
				encoderInfos.push_back(encoderInfo);
			}
#if !defined(_DEBUG) 
		}
#endif
	}

	return encoderInfos;
}

// reviewed 0.3.8
vector<MuxerInfo> Settings::getMuxers()
{
	vector<MuxerInfo> muxersInfos;

	// Create EncoderInfo list
	for (json muxer: muxerConfig)
	{
		muxersInfos.push_back(MuxerInfo(muxer));
	}

	return muxersInfos;
}

// reviewed 0.3.8
json Settings::getConfiguration()
{
	return mainConfig;
}

// reviewed 0.3.8
json Settings::loadResource(HMODULE hModule, LPCWSTR lpType, LPCWSTR lpName)
{
	const HRSRC hRes = FindResourceEx(hModule, lpType, lpName, MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL));
	if (NULL != hRes)
	{
		HGLOBAL hData = LoadResource(hModule, hRes);
		if (NULL != hData)
		{
			const DWORD dataSize = SizeofResource(hModule, hRes);
			char *resource = new char[dataSize + 1];
			memcpy(resource, LockResource(hData), dataSize);
			resource[dataSize] = 0;

			return json::parse(resource);
		}
	}

	return NULL;
}