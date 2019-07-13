#pragma once

#include <wx/wx.h>
#include "lavf.h"
#include "OptionContainer.h"
#include "FilterConfig.h"
#include "json.hpp"

#define JSON_GET_STRING(field, def) \
  (j.find(#field) != j.end()) ? j[#field].get<std::string>() : def

#define JSON_GET_GROUP_STRING(group, field, def) \
  (j.find(#group) != j.end() && j[#group].find(#field) != j[#group].end()) ? j[#group][#field].get<std::string>() : def

#define JSON_GET_BOOL(group, field, def) \
  (j.find(#group) != j.end() && j[#group].find(#field) != j[#group].end()) ? j[#group][#field].get<bool>() : def

using namespace nlohmann;

struct ExportInfo
{
	wxString filename = wxEmptyString;
	wxString application = wxEmptyString;
	int passes = 0;

	struct Video
	{
		bool enabled = true;
		int width = 0;
		int height = 0;
		wxString id = wxEmptyString;
		OptionContainer options;
		OptionContainer sideData;
		FilterConfig filters;
		AVRational timebase = { 0, 0 };
		AVPixelFormat pixelFormat = AVPixelFormat::AV_PIX_FMT_NONE;
		AVRational sampleAspectRatio = { 0, 0 };
		AVFieldOrder fieldOrder = AVFieldOrder::AV_FIELD_UNKNOWN;
		AVColorRange colorRange = AVColorRange::AVCOL_RANGE_UNSPECIFIED;
		AVColorSpace colorSpace = AVColorSpace::AVCOL_SPC_UNSPECIFIED;
		AVColorPrimaries colorPrimaries = AVColorPrimaries::AVCOL_PRI_UNSPECIFIED;
		AVColorTransferCharacteristic colorTransferCharacteristics = AVColorTransferCharacteristic::AVCOL_TRC_UNSPECIFIED;
		int64_t ticksPerFrame = 0;
	} video;

	struct Audio
	{
		bool enabled = true;
		wxString id = wxEmptyString;
		OptionContainer options;
		OptionContainer sideData;
		FilterConfig filters;
		AVRational timebase = { 0, 0 };
		AVSampleFormat sampleFormat = AVSampleFormat::AV_SAMPLE_FMT_NONE;
		uint64_t channelLayout = 0;
	} audio;

	struct Format
	{
		wxString id = wxEmptyString;
		bool faststart = false;
	} format;

	bool Deserialize(std::string data)
	{
		auto j = json::parse(data, nullptr, true);

		// Error during parsing
		if (j == json::value_t::discarded || !j.is_object())
			return false;

		filename = JSON_GET_STRING(filename, "");

		video.enabled = JSON_GET_BOOL(video, enabled, true);
		video.id = JSON_GET_GROUP_STRING(video, id, "");
		video.options.Deserialize(JSON_GET_GROUP_STRING(video, options, ""));
		video.sideData.Deserialize(JSON_GET_GROUP_STRING(video, sidedata, ""));
		video.filters.Deserialize(JSON_GET_GROUP_STRING(video, filters, ""));
		
		wxString interlaced = JSON_GET_GROUP_STRING(video, interlaced, "");
		if (interlaced == "tff")
			video.fieldOrder = AV_FIELD_TT;
		else if (interlaced == "bff")
			video.fieldOrder = AV_FIELD_BB;
		else
			video.fieldOrder = AV_FIELD_PROGRESSIVE;
		
		audio.enabled = JSON_GET_BOOL(audio, enabled, true);
		audio.id = JSON_GET_GROUP_STRING(audio, id, "");
		audio.options.Deserialize(JSON_GET_GROUP_STRING(audio, options, ""));
		audio.sideData.Deserialize(JSON_GET_GROUP_STRING(audio, sidedata, ""));
		audio.filters.Deserialize(JSON_GET_GROUP_STRING(audio, filters, ""));

		format.id = JSON_GET_GROUP_STRING(format, id, "");
		format.faststart = JSON_GET_BOOL(format, faststart, false);

		return true;
	}

	std::string Serialize()
	{
		json j;

		j["filename"] = filename;

		// Video
		j["video"]["enabled"] = video.enabled;
		j["video"]["id"] = video.id;
		j["video"]["options"] = video.options.Serialize(true);
		j["video"]["sidedata"] = video.sideData.Serialize(true);
		j["video"]["filters"] = video.filters.Serialize();
		if (video.fieldOrder != AV_FIELD_PROGRESSIVE)
			j["video"]["interlaced"] = (video.fieldOrder != AV_FIELD_TT) ? "tff" : "bff";

		// Audio
		j["audio"]["enabled"] = audio.enabled;
		j["audio"]["id"] = audio.id;
		j["audio"]["options"] = audio.options.Serialize(true);
		j["audio"]["sidedata"] = audio.sideData.Serialize(true);
		j["audio"]["filters"] = audio.filters.Serialize();

		// Format
		j["format"]["id"] = format.id;
		j["format"]["faststart"] = format.faststart;

		return j.dump();
	}
};
