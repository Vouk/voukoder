#include "MuxerInfo.h"

// reviewed 0.3.8
MuxerInfo::MuxerInfo(json muxer)
{
	this->muxer = muxer;

	id = muxer["id"].get<int>();
	name = muxer["name"].get<string>();
	extension = muxer["extension"].get<string>();
}