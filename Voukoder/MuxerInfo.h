#pragma once

#include <string>
#include "json.hpp"

using namespace std;
using json = nlohmann::json;

// reviewed 0.3.8
class MuxerInfo
{
public:
	int id;
	string name;
	string extension;
	bool movFastStart = false;
	MuxerInfo(json muxer);

private:
	json muxer;
};

