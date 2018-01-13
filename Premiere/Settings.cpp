#include "Settings.h"

void Settings::initFromResources(HMODULE hModule)
{
	mainConfig = LoadSingleResource(hModule, MAKEINTRESOURCE(ID_TEXT), MAKEINTRESOURCE(IDR_ID_TEXT3));

	// Create MuxerInfo list
	const json muxerConfig = LoadSingleResource(hModule, MAKEINTRESOURCE(ID_TEXT), MAKEINTRESOURCE(IDR_ID_TEXT2));
	for (json muxer : muxerConfig)
	{
		muxerInfos.push_back(MuxerInfo(muxer));
	}

	// Create EncoderInfo list
	const json encoderConfig = LoadSingleResource(hModule, MAKEINTRESOURCE(ID_TEXT), MAKEINTRESOURCE(IDR_ID_TEXT1));
	loadEncoderInfos(encoderConfig["video"], &videoEncoderInfos);
	loadEncoderInfos(encoderConfig["audio"], &audioEncoderInfos);
}

// reviewed 0.5.3
void Settings::loadEncoderInfos(json encoders, vector<EncoderInfo> *encoderInfos)
{
	for (json encoder : encoders)
	{
		EncoderInfo encoderInfo = EncoderInfo(encoder);
#if !defined(_DEBUG) 
		if (!encoderInfo.experimental)
		{
#endif
			if (IsEncoderAvailable(encoderInfo))
			{
				encoderInfos->push_back(encoderInfo);
			}
#if !defined(_DEBUG) 
		}
#endif
	}
}

// reviewed 0.3.8
json Settings::LoadSingleResource(HMODULE hModule, LPCWSTR lpType, LPCWSTR lpName)
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

// reviewed 0.5.3
bool Settings::IsEncoderAvailable(EncoderInfo encoderinfo)
{
	bool ret = false;

	// Find codec by name
	AVCodec *codec = avcodec_find_encoder_by_name(encoderinfo.name.c_str());
	if (codec != NULL)
	{
		// Create codec context
		AVCodecContext *codecContext = avcodec_alloc_context3(codec);

		// Type specific codec context settings
		if (codec->type == AVMEDIA_TYPE_VIDEO)
		{
			codecContext->width = 1920;
			codecContext->height = 1080;
			codecContext->time_base = { 25 , 1 };
			codecContext->pix_fmt = codec->pix_fmts ? codec->pix_fmts[0] : AV_PIX_FMT_YUV420P;
		}
		else if (codec->type == AVMEDIA_TYPE_AUDIO)
		{
			codecContext->channel_layout = AV_CH_LAYOUT_STEREO;
			codecContext->channels = 2;
			codecContext->sample_rate = 44100;
			codecContext->sample_fmt = codec->sample_fmts ? codec->sample_fmts[0] : AV_SAMPLE_FMT_FLTP;
			codecContext->bit_rate = 0;
		}

		// Open the codec
		ret = (avcodec_open2(codecContext, codec, NULL) == 0);

		// Close the codec
		avcodec_free_context(&codecContext);
	}

	return ret;
}