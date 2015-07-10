#include "precompile.hpp"
#include "input_stream.hpp"

namespace
{
#if defined(_WIN32) || defined(_WIN64)
	static const char* gDesktopInputFormatString = "gdigrab";
#else
	static const char* gDesktopInputFormatString = "x11grab";
#endif
}

InputStream::InputStream(InputStreamType aInputStreamType, std::map<std::string, std::string> aParameters)
	: _inputStreamType(aInputStreamType)
	, _formatCtx(nullptr)
	, _videoCodecCtx(nullptr)
	, _audioCodecCtx(nullptr)
	, _codec(nullptr)
	, _frame(nullptr)
	, _swsCtx(nullptr)
	, _videoStreamIndex(-1)
	, _audioStreamIndex(-1)
{ 
	_formatCtx = avformat_alloc_context();
	AVInputFormat *theInputfmt = nullptr;
	AVDictionary* theOptions = nullptr;
	for (auto& theParam : aParameters)
	{
		av_dict_set(&theOptions, theParam.first.c_str(), theParam.second.c_str(), 0);
	}
	if (aInputStreamType == InputStreamType::Desktop) //desktop
	{
		av_find_input_format(gDesktopInputFormatString);
		if (avformat_open_input(&_formatCtx, "desktop", theInputfmt, &theOptions) != 0)
		{
			printf("Couldn't open input stream.\n");
			return;
		}
	}
	else if (aInputStreamType == InputStreamType::Camera)
	{
		//to do: list_devices dshow video=Integrated Camera
	}

	if (avformat_find_stream_info(_formatCtx, NULL) < 0)
	{
		printf("Couldn't find stream information.\n");
		return;
	}
	for (std::size_t i = 0; i < _formatCtx->nb_streams; i++)
	{
		if (_formatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO)
		{
			_videoStreamIndex = i;
		}
		else if (_formatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_AUDIO)
		{
			_audioStreamIndex = i;
		}
	}
	if (_videoStreamIndex == -1 && _audioStreamIndex == -1)
	{
		printf("Didn't find a video stream.\n");
		return;
	}
	_videoCodecCtx = _formatCtx->streams[_videoStreamIndex]->codec;
	_codec = avcodec_find_decoder(_videoCodecCtx->codec_id);
	if (_codec == nullptr)
	{
		printf("Codec not found.\n");
		return;
	}
	if (avcodec_open2(_videoCodecCtx, _codec, NULL) < 0)
	{
		printf("Could not open codec.\n");
		return;
	}
	av_init_packet(&_packet);
}

InputStream::~InputStream()
{
	if (_formatCtx != nullptr) avformat_free_context(_formatCtx);
	_videoCodecCtx = nullptr;
	_codec = nullptr;
	if (_swsCtx != nullptr) sws_freeContext(_swsCtx);
	if (_frame != nullptr) av_frame_free(&_frame);
	av_free_packet(&_packet);
}
