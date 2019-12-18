#include "Voukoder.h"
#include "Callback.h"
#include "EncoderUtils.h"
#include "MuxerUtils.h"
#include "LanguageUtils.h"

using namespace nlohmann;

Voukoder::Config::Config()
{
	// Set current version
	Version curVersion;
	curVersion.number.major = VKDR_VERSION_MAJOR;
	curVersion.number.minor = VKDR_VERSION_MINOR;
	curVersion.number.patch = VKDR_VERSION_PATCH;

	// Check for update
	(new wxApp());
	CheckForUpdate(curVersion, &update);

#ifdef _WIN32
	// Load translations first
	LoadResources(GetCurrentModule(), MAKEINTRESOURCE(ID_TRANSLATION));

	// Initialize translation module
	LanguageUtils::InitTranslation(languageInfos);

	// Load encoder, muxer and filter infos
	LoadResources(GetCurrentModule(), MAKEINTRESOURCE(ID_MISC));
	LoadResources(GetCurrentModule(), MAKEINTRESOURCE(ID_ENCODER));
	LoadResources(GetCurrentModule(), MAKEINTRESOURCE(ID_MUXER));
	LoadResources(GetCurrentModule(), MAKEINTRESOURCE(ID_FILTER));
#endif
}

#ifdef _WIN32
BOOL Voukoder::Config::EnumNamesFunc(HMODULE hModule, LPCTSTR lpType, LPTSTR lpName, LONG_PTR lParam)
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
			FreeResource(hData);

			try
			{
				json jsonResource = json::parse(resource);

				// Create configs and store them
				if (lpType == MAKEINTRESOURCE(ID_MISC))
				{
					if (lpName == MAKEINTRESOURCE(IDR_VIDEO_SIDE_DATA))
						EncoderUtils::Create(videoSideData, jsonResource);
					else if (lpName == MAKEINTRESOURCE(IDR_AUDIO_SIDE_DATA))
						EncoderUtils::Create(audioSideData, jsonResource);
					//else if (lpName == MAKEINTRESOURCE(IDR_PRESETS))
					//	presets = jsonResource["presets"].get<std::vector<Preset>>();
				}
				else if (lpType == MAKEINTRESOURCE(ID_ENCODER))
				{
					wxString id = jsonResource["id"].get<std::string>();

					vkLogInfoVA("Loading: encoders/%s.json", id);

					// Is this encoder supported?
					if (EncoderUtils::IsEncoderAvailable(id))
					{
						EncoderInfo encoderInfo;
						if (EncoderUtils::Create(encoderInfo, jsonResource))
						{
							if (encoderInfo.type == AVMediaType::AVMEDIA_TYPE_VIDEO)
								videoEncoderInfos.push_back(encoderInfo);
							else if (encoderInfo.type == AVMediaType::AVMEDIA_TYPE_AUDIO)
								audioEncoderInfos.push_back(encoderInfo);
						}
					}
					else
						vkLogInfoVA("Unloading: encoders/%s.json", id);
				}
				else if (lpType == MAKEINTRESOURCE(ID_MUXER))
				{
					MuxerInfo muxerInfo;
					if (MuxerUtils::Create(muxerInfo, jsonResource))
						muxerInfos.push_back(muxerInfo);
				}
				else if (lpType == MAKEINTRESOURCE(ID_FILTER))
				{
					wxString id = jsonResource["name"].get<std::string>();

					vkLogInfoVA("Loading: filters/%s.json", id);

					EncoderInfo filterInfo;
					if (EncoderUtils::Create(filterInfo, jsonResource))
					{
						if (filterInfo.type == AVMEDIA_TYPE_VIDEO)
							videoFilterInfos.push_back(filterInfo);
						else if (filterInfo.type == AVMEDIA_TYPE_AUDIO)
							audioFilterInfos.push_back(filterInfo);
						else
							vkLogInfoVA("Unloading: filters/%s.json", id);
					}
				}
				else if (lpType == MAKEINTRESOURCE(ID_TRANSLATION))
				{
					LanguageInfo languageInfo;
					if (LanguageUtils::Create(languageInfo, jsonResource))
						languageInfos.push_back(languageInfo);
				}
			}
			catch (json::parse_error p)
			{
				OutputDebugStringA(p.what());
				return TRUE;
			}
		}
	}

	return TRUE;
}

bool Voukoder::Config::LoadResources(HMODULE hModule, LPTSTR lpType)
{
	// Create C conform callback
	Callback<BOOL(HMODULE hModule, LPCTSTR lpType, LPTSTR lpName, LONG_PTR lParam)>::func = bind(&Voukoder::Config::EnumNamesFunc, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);

	return EnumResourceNames(hModule, lpType, static_cast<ENUMRESNAMEPROC>(Callback<BOOL(HMODULE hModule, LPCTSTR lpType, LPTSTR lpName, LONG_PTR lParam)>::callback), NULL);
}
#endif