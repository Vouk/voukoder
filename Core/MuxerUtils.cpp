#include "MuxerUtils.h"
#include "Log.h"

bool MuxerUtils::Create(MuxerInfo &muxerInfo, const json resource)
{
	std::string muxerId = resource["id"].get<std::string>();

	// Is this encoder supported?
	if (!IsAvailable(muxerId))
	{
		return false;
	}

	vkLogInfoVA("Loading: muxers/%s.json", muxerId.c_str());

	// Parse encoder info
	muxerInfo.id = muxerId;
	muxerInfo.extension = resource["extension"].get<std::string>();
	muxerInfo.name = resource["name"].get<std::string>() + " (." + muxerInfo.extension + ")";
	muxerInfo.videoCodecIds = resource["encoders"]["video"].get<std::vector<std::string>>();
	muxerInfo.audioCodecIds = resource["encoders"]["audio"].get<std::vector<std::string>>();
	muxerInfo.capabilities.faststart = resource["capabilities"]["faststart"].get<bool>();

	return true;
}

bool MuxerUtils::IsAvailable(const std::string id)
{
	return av_guess_format(id.c_str(), NULL, NULL) != NULL;
}
