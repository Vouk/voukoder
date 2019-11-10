#pragma once

#include <wx/wx.h>
#include "lavf.h"
#include "OptionContainer.h"
#include "FilterConfig.h"
#include "json.hpp"

using namespace nlohmann;

#define JSON_CONTAINS(field) (j.find(#field) != j.end())
#define JSON_GROUP_CONTAINS(group, field) (j.find(#group) != j.end() && j[#group].find(#field) != j[#group].end())
#define JSON_GET(field, type) j[#field].get<##type>()
#define JSON_GROUP_GET(group, field, type) j[#group][#field].get<##type>()
#define JSON_IMPORT(field, type) if JSON_CONTAINS(##field) ##field = JSON_GET(##field, ##type)
#define JSON_GROUP_IMPORT(group, field, type) if JSON_GROUP_CONTAINS(##group, ##field) ##group.##field = JSON_GROUP_GET(##group, ##field, ##type)

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
		AVRational sampleAspectRatio = { 0, 0 };
		AVFieldOrder fieldOrder = AVFieldOrder::AV_FIELD_UNKNOWN;
		AVColorRange colorRange = AVColorRange::AVCOL_RANGE_UNSPECIFIED;
		AVColorSpace colorSpace = AVColorSpace::AVCOL_SPC_UNSPECIFIED;
		AVColorPrimaries colorPrimaries = AVColorPrimaries::AVCOL_PRI_UNSPECIFIED;
		AVColorTransferCharacteristic colorTransferCharacteristics = AVColorTransferCharacteristic::AVCOL_TRC_UNSPECIFIED;
		bool colorConvert = true;
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

		// General
		JSON_IMPORT(filename, std::string);
		JSON_IMPORT(application, std::string);

		// Video
		JSON_GROUP_IMPORT(video, enabled, bool);
		JSON_GROUP_IMPORT(video, width, int);
		JSON_GROUP_IMPORT(video, height, int);
		JSON_GROUP_IMPORT(video, id, std::string);
		if (JSON_GROUP_CONTAINS(video, options))
			video.options.Deserialize(JSON_GROUP_GET(video, options, std::string));
		if (JSON_GROUP_CONTAINS(video, sidedata))
			video.options.Deserialize(JSON_GROUP_GET(video, sidedata, std::string));
		if (JSON_GROUP_CONTAINS(video, filters))
			video.options.Deserialize(JSON_GROUP_GET(video, filters, std::string));
		if (JSON_GROUP_CONTAINS(video, timebase))
		{
			wxString timebase = JSON_GROUP_GET(video, timebase, std::string);
			//if (timebase.Matches())
		}
		if (JSON_GROUP_CONTAINS(video, sar))
		{
			wxString sar = JSON_GROUP_GET(video, sar, std::string);
			//if (timebase.Matches())
		}
		if (JSON_GROUP_CONTAINS(video, interlaced))
		{
			wxString interlaced = JSON_GROUP_GET(video, interlaced, std::string);
			if (interlaced == "tff")
				video.fieldOrder = AV_FIELD_TT;
			else if (interlaced == "bff")
				video.fieldOrder = AV_FIELD_BB;
		}
		if (JSON_GROUP_CONTAINS(video, colorRange))
			video.colorRange = JSON_GROUP_GET(video, colorRange, std::string) == "full" ? AVColorRange::AVCOL_RANGE_JPEG : AVColorRange::AVCOL_RANGE_MPEG;
		//if (JSON_GROUP_CONTAINS(video, colorSpace))
		//	video.colorSpace = avcolor(JSON_GROUP_GET(video, colorSpace, std::string).c_str());
		//if (JSON_GROUP_CONTAINS(video, colorPrimaries))
		//	video.colorSpace = avcolor(JSON_GROUP_GET(video, colorSpace, std::string).c_str());
		//if (JSON_GROUP_CONTAINS(video, colorTransferCharacteristics))
		//	video.colorSpace = avcolor(JSON_GROUP_GET(video, colorSpace, std::string).c_str());

		// Audio
		JSON_GROUP_IMPORT(audio, enabled, bool);
		JSON_GROUP_IMPORT(audio, id, std::string);
		if (JSON_GROUP_CONTAINS(audio, options))
			audio.options.Deserialize(JSON_GROUP_GET(audio, options, std::string));
		if (JSON_GROUP_CONTAINS(audio, sidedata))
			audio.options.Deserialize(JSON_GROUP_GET(audio, sidedata, std::string));
		if (JSON_GROUP_CONTAINS(audio, filters))
			audio.options.Deserialize(JSON_GROUP_GET(audio, filters, std::string));
		if (JSON_GROUP_CONTAINS(audio, timebase))
		{
			wxString timebase = JSON_GROUP_GET(audio, timebase, std::string);
			if (timebase.Matches("(\\d+):(\\d+)"))
			{

			}
			//wxRegEx reEmail = "([^@]+)@([[:alnum:].-_].)+([[:alnum:]]+)";
			//if (reEmail.Matches(text))
			//{
			//	wxString text = reEmail.GetMatch(email);
			//	wxString username = reEmail.GetMatch(email, 1);
			//	if (reEmail.GetMatch(email, 3) == "com") // .com TLD?
			//	{
			//		...
			//	}
			//}
		}
		if (JSON_GROUP_CONTAINS(audio, channelLayout))
			audio.channelLayout = av_get_channel_layout(JSON_GROUP_GET(audio, channelLayout, std::string).c_str());

		// Format
		JSON_GROUP_IMPORT(format, id, std::string);
		JSON_GROUP_IMPORT(format, faststart, bool);

		return true;
	}

	std::string Serialize()
	{
		json j;

		// General
		if (!filename.IsEmpty())
			j["filename"] = filename;
		if (!application.IsEmpty())
			j["application"] = filename;

		// Video
		j["video"]["enabled"] = video.enabled;
		if (video.width > 0)
			j["video"]["width"] = video.width;
		if (video.height > 0)
			j["video"]["height"] = video.height;
		if (!video.id.IsEmpty())
			j["video"]["id"] = video.id;
		if (video.options.size() > 0)
			j["video"]["options"] = video.options.Serialize(true);
		if (video.sideData.size() > 0)
			j["video"]["sidedata"] = video.sideData.Serialize(true);
		if (video.filters.size() > 0)
			j["video"]["filters"] = video.filters.Serialize();
		if (video.sampleAspectRatio.num > 0 && video.sampleAspectRatio.den > 0)
			j["video"]["sar"] = wxString::Format("%d:%d", video.sampleAspectRatio.num, video.sampleAspectRatio.den);
		if (video.timebase.num > 0 && video.timebase.den > 0)
			j["video"]["timebase"] = wxString::Format("%d:%d", video.timebase.num, video.timebase.den);
		if (video.colorRange != AVCOL_RANGE_UNSPECIFIED)
			j["video"]["colorRange"] = video.colorRange == AVColorRange::AVCOL_RANGE_JPEG ? "full" : "limited";
		if (video.colorSpace != AVCOL_SPC_UNSPECIFIED)
			j["video"]["colorSpace"] = ""; // TODO
		if (video.colorPrimaries != AVCOL_PRI_UNSPECIFIED)
			j["video"]["colorPrimaries"] = ""; // TODO
		if (video.colorTransferCharacteristics != AVCOL_TRC_UNSPECIFIED)
			j["video"]["colorTrc"] = ""; // TODO
		if (video.fieldOrder != AV_FIELD_PROGRESSIVE && video.fieldOrder != AV_FIELD_UNKNOWN)
			j["video"]["interlaced"] = (video.fieldOrder != AV_FIELD_TT) ? "tff" : "bff";

		// Audio
		j["audio"]["enabled"] = audio.enabled;
		if (!audio.id.IsEmpty())
			j["audio"]["id"] = audio.id;
		if (audio.options.size() > 0)
			j["audio"]["options"] = audio.options.Serialize(true);
		if (audio.sideData.size() > 0)
			j["audio"]["sidedata"] = audio.sideData.Serialize(true);
		if (audio.filters.size() > 0)
			j["audio"]["filters"] = audio.filters.Serialize();
		if (audio.timebase.num > 0 && audio.timebase.den > 0)
			j["audio"]["timebase"] = wxString::Format("%d:%d", audio.timebase.num, audio.timebase.den);
		if (audio.channelLayout > 0)
		{
			char buffer[32] = { 0 };
			int buffer_size = 0;
			av_get_channel_layout_string(buffer, buffer_size, NULL, audio.channelLayout);
			j["audio"]["layout"] = buffer;
		}

		// Format
		if (!format.id.IsEmpty())
			j["format"]["id"] = format.id;
		if (format.faststart)
			j["format"]["faststart"] = format.faststart;

		return j.dump();
	}
};
