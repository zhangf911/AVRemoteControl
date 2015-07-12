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
	, _videoFrame(nullptr)
	, _videoFrameYUV(nullptr)
	, _audioFrame(nullptr)
	, _swsCtx(nullptr)
	, _out_buffer_YUV(nullptr)
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
			std::cout << "Couldn't open input stream." << std::endl;
			return;
		}
	}
	else if (aInputStreamType == InputStreamType::Camera)
	{
		//to do: list_devices dshow video=Integrated Camera
	}

	if (avformat_find_stream_info(_formatCtx, NULL) < 0)
	{
		std::cout << "Couldn't find stream information." << std::endl;
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
		std::cout << "Didn't find a video stream." << std::endl;
		return;
	}
	_videoCodecCtx = _formatCtx->streams[_videoStreamIndex]->codec;
	_codec = avcodec_find_decoder(_videoCodecCtx->codec_id);
	if (_codec == nullptr)
	{
		std::cout << "Codec not found." << std::endl;
		return;
	}
	if (avcodec_open2(_videoCodecCtx, _codec, NULL) < 0)
	{
		std::cout << "Could not open codec." << std::endl;
		return;
	}
	av_init_packet(&_packet);
	_videoFrame = av_frame_alloc();
	_out_buffer_YUV = (uint8_t *)av_malloc(avpicture_get_size(PIX_FMT_YUV420P, _videoCodecCtx->width, _videoCodecCtx->height));
	avpicture_fill((AVPicture *)_videoFrameYUV, _out_buffer_YUV, PIX_FMT_YUV420P, _videoCodecCtx->width, _videoCodecCtx->height);
	_swsCtx = sws_getContext(_videoCodecCtx->width, _videoCodecCtx->height, _videoCodecCtx->pix_fmt,
		_videoCodecCtx->width, _videoCodecCtx->height, PIX_FMT_YUV420P, SWS_BICUBIC, NULL, NULL, NULL);

}

InputStream::~InputStream()
{
	if (_formatCtx != nullptr) avformat_free_context(_formatCtx);
	_videoCodecCtx = nullptr;
	_codec = nullptr;
	if (_swsCtx != nullptr) sws_freeContext(_swsCtx);
	if (_out_buffer_YUV != nullptr) av_free(_out_buffer_YUV);
	if (_videoFrame != nullptr) av_frame_free(&_videoFrame);
	if (_audioFrame != nullptr) av_frame_free(&_audioFrame);
	av_free_packet(&_packet);
}

bool InputStream::operator >> (AVFrame* aFrame)
{
	aFrame = nullptr;
	int theResult = 0, theGot_picture = 0;
	if (av_read_frame(_formatCtx, &_packet) >= 0)
	{
		if (_packet.stream_index == _videoStreamIndex)
		{
			theResult = avcodec_decode_video2(_videoCodecCtx, _videoFrame, &theGot_picture, &_packet);
			if (theResult < 0)
			{
				std::cout << "Decode Video Packet Error." << std::endl;
				goto LoopClearup;
			}
			if (theGot_picture)
			{
				theResult = sws_scale(_swsCtx, (const uint8_t* const*)_videoFrame->data, _videoFrame->linesize, 0,
					_videoCodecCtx->height, _videoFrameYUV->data, _videoFrameYUV->linesize);
			}
			aFrame = _videoFrame;
		}
		else if (_packet.stream_index == _videoStreamIndex)
		{
			theResult = avcodec_decode_audio4(_audioCodecCtx, _audioFrame, &theGot_picture, &_packet);
			if (theResult < 0)
			{
				std::cout << "Decode Audio Packet Error." << std::endl;
				goto LoopClearup;
			}
			if (theGot_picture)
			{
				//todo audio
			}
			aFrame = _audioFrame;
		}
		else
		{
			std::cout << "Not Support Stream." << std::endl;
			return false;
		}

	LoopClearup:
		av_free_packet(&_packet);
		return aFrame != nullptr;
	}
	return false;
}
