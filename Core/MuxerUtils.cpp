#include "MuxerUtils.h"
#include "Log.h"

bool MuxerUtils::Create(MuxerInfo &muxerInfo, const json resource)
{
	string muxerId = resource["id"].get<string>();

	// Is this encoder supported?
	if (!IsAvailable(muxerId))
	{
		return false;
	}

	vkLogInfo("Loading: muxers/%s.json", muxerId.c_str());

	// Parse encoder info
	muxerInfo.id = muxerId;
	muxerInfo.name = resource["name"].get<string>();
	muxerInfo.extension = resource["extension"].get<string>();
	muxerInfo.videoCodecIds = resource["encoders"]["video"].get<vector<string>>();
	muxerInfo.audioCodecIds = resource["encoders"]["audio"].get<vector<string>>();
	muxerInfo.capabilities.faststart = resource["capabilities"]["faststart"].get<bool>();

	return true;
}

bool MuxerUtils::IsAvailable(const string id)
{
	return av_guess_format(id.c_str(), NULL, NULL) != NULL;
}