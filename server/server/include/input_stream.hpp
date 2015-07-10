#include "precompile.hpp"

class InputStream
{
public:
	InputStream(InputStreamType aInputStreamType = InputStreamType::Desktop,
		std::map<std::string, std::string> aParameters = std::map<std::string, std::string>());
	~InputStream();

private:
	InputStreamType  _inputStreamType;

	AVFormatContext* _formatCtx;
	AVCodecContext*  _videoCodecCtx;
	AVCodecContext*  _audioCodecCtx;
	AVCodec*         _codec;
	AVFrame*         _frame;
	AVPacket         _packet;
	SwsContext*      _swsCtx;

	int              _videoStreamIndex;
	int              _audioStreamIndex;
};
