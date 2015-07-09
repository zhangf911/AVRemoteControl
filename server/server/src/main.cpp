#include "precompile.hpp"
#include <stdio.h>

int main(int argc, char* argv[])
{
	//Initial ffmpeg
	av_register_all();
	avformat_network_init();
	avdevice_register_all();
	
	//FFmpeg  
	int ret, got_picture;
	AVFormatContext *pFormatCtx;
	int             i, videoindex;
	AVCodecContext  *pCodecCtx;
	AVCodec         *pCodec;
	AVFrame *pFrame, *pFrameYUV;
	AVPacket packet;
	struct SwsContext *img_convert_ctx;

	pFormatCtx = avformat_alloc_context();

	AVInputFormat *ifmt = av_find_input_format("gdigrab");
	//Set some options  
	AVDictionary* options = nullptr;
	//grabbing frame rate  
	av_dict_set(&options, "framerate", "5", 0);  
	//The distance from the left edge of the screen or desktop  
	//av_dict_set(&options,"offset_x","20",0);  
	//The distance from the top edge of the screen or desktop  
	//av_dict_set(&options,"offset_y","40",0);  
	//Video frame size. The default is to capture the full screen  
	av_dict_set(&options, "video_size", "640x480",0);  
	if (avformat_open_input(&pFormatCtx, "desktop", ifmt, &options) != 0)
	{
		printf("Couldn't open input stream.\n");
		return -1;
	}
	if(avformat_find_stream_info(pFormatCtx, NULL) < 0)
	{
		printf("Couldn't find stream information.\n");
		return -1;
	}
	videoindex = -1;
	for (i = 0; i < pFormatCtx->nb_streams; i++)
	{
		if (pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO){
			videoindex = i;
			break;
		}
	}
	if(videoindex == -1)
	{
		printf("Didn't find a video stream.\n");
		return -1;
	}
	pCodecCtx = pFormatCtx->streams[videoindex]->codec;
	pCodec = avcodec_find_decoder(pCodecCtx->codec_id);
	if (pCodec == NULL)
	{
		printf("Codec not found.\n");
		return -1;
	}
	if (avcodec_open2(pCodecCtx, pCodec, NULL) < 0)
	{
		printf("Could not open codec.\n");
		return -1;
	}

	pFrame = av_frame_alloc();
	pFrameYUV = av_frame_alloc();
	uint8_t *out_buffer=(uint8_t *)av_malloc(avpicture_get_size(PIX_FMT_YUV420P, pCodecCtx->width, pCodecCtx->height));  
	avpicture_fill((AVPicture *)pFrameYUV, out_buffer, PIX_FMT_YUV420P, pCodecCtx->width, pCodecCtx->height);  
	img_convert_ctx = sws_getContext(pCodecCtx->width, pCodecCtx->height, pCodecCtx->pix_fmt, pCodecCtx->width, pCodecCtx->height, PIX_FMT_YUV420P, SWS_BICUBIC, NULL, NULL, NULL);

	FILE *fp_yuv = nullptr;
	fp_yuv = fopen("output.yuv", "wb+");

	av_init_packet(&packet);
	//packet = (AVPacket *)av_malloc(sizeof(AVPacket));
	while (av_read_frame(pFormatCtx, &packet) >= 0)
	{
		if (packet.stream_index == videoindex)
		{
			//Decode
			ret = avcodec_decode_video2(pCodecCtx, pFrame, &got_picture, &packet);
			if (ret < 0){
				printf("Decode Error.\n");
				return -1;
			}
			if (got_picture)
			{
				ret = sws_scale(img_convert_ctx, (const uint8_t* const*)pFrame->data, pFrame->linesize, 0,
					pCodecCtx->height, pFrameYUV->data, pFrameYUV->linesize);

				int y_size = pCodecCtx->width*pCodecCtx->height;
				fwrite(pFrameYUV->data[0], 1, y_size / 4, fp_yuv);  //Y
				fwrite(pFrameYUV->data[1], 1, y_size / 4, fp_yuv);  //U
				fwrite(pFrameYUV->data[2], 1, y_size / 4, fp_yuv);  //V

				//Delay 40ms  
				std::this_thread::sleep_for(std::chrono::milliseconds(40));
			}
		}
		av_free_packet(&packet);
	}
	fclose(fp_yuv);
	
	return 0;
}
