#include "precompile.hpp"

class InputStream
{
public:
	InputStream(InputStreamType aInputStreamType = InputStreamType::Desktop,
		std::map<std::string, std::string> aParameters = std::map<std::string, std::string>());
	~InputStream();

	bool operator >> (AVFrame* aFrame);

private:
	InputStreamType  _inputStreamType;

	AVFormatContext* _formatCtx;
	AVCodecContext*  _videoCodecCtx;
	AVCodecContext*  _audioCodecCtx;
	AVCodec*         _codec;
	AVFrame*         _videoFrame;
	AVFrame*         _videoFrameYUV;
	AVFrame*         _audioFrame;
	AVPacket         _packet;
	SwsContext*      _swsCtx;
	uint8_t*         _out_buffer_YUV;

	int              _videoStreamIndex;
	int              _audioStreamIndex;
};
