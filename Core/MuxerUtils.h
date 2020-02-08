#pragma once

#include "MuxerInfo.h"
#include "json.hpp"
#include "lavf.h"
#include <string>

using namespace nlohmann;

class MuxerUtils
{
public:
	static bool Create(MuxerInfo &muxerInfo, const json json);

private:
	static bool IsAvailable(const wxString codecId);
};
