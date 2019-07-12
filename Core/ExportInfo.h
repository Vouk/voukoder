#pragma once

#include <wx/wx.h>
#include "lavf.h"
#include "OptionContainer.h"
#include "FilterConfig.h"
#include "json.hpp"

#define JSON_GET_STRING(group, field) \
  (j.find(#group) != j.end() && j[#group].find(#field) != j[#group].end()) ? j[#group][#field].get<std::string>() : ""

#define JSON_GET_BOOL(group, field) \
  (j.find(#group) != j.end() && j[#group].find(#field) != j[#group].end()) ? j[#group][#field].get<bool>() : false

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
		auto j = json::parse(data, nullptr, false);

		// Error during parsing
		if (j == json::value_t::discarded || !j.is_object())
			return false;

		video.id = JSON_GET_STRING(video, id);
		video.options.Deserialize(JSON_GET_STRING(video, options));
		video.sideData.Deserialize(JSON_GET_STRING(video, sidedata));
		video.filters.Deserialize(JSON_GET_STRING(video, filters));
		audio.id = JSON_GET_STRING(audio, id);
		audio.options.Deserialize(JSON_GET_STRING(audio, options));
		audio.sideData.Deserialize(JSON_GET_STRING(audio, sidedata));
		audio.filters.Deserialize(JSON_GET_STRING(audio, filters));
		format.id = JSON_GET_STRING(format, id);
		format.faststart = JSON_GET_BOOL(format, faststart);

		return true;
	}

	std::string Serialize()
	{
		json j;

		// Video
		j["video"]["id"] = video.id;
		j["video"]["options"] = video.options.Serialize();
		j["video"]["sidedata"] = video.sideData.Serialize();
		j["video"]["filters"] = video.filters.Serialize();

		// Audio
		j["audio"]["id"] = audio.id;
		j["audio"]["options"] = audio.options.Serialize();
		j["audio"]["sidedata"] = audio.sideData.Serialize();
		j["audio"]["filters"] = audio.filters.Serialize();

		// Format
		j["format"]["id"] = format.id;
		j["format"]["faststart"] = format.faststart;

		return j.dump();
	}
};
