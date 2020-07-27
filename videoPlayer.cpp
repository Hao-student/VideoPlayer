// videoPlayer.cpp: 定义应用程序的入口点。
//

#include "videoPlayer.h"

//SDL库和FFmpeg库所需的声明
#define SDL_MAIN_HANDLED
#define __STDC_CONSTANT_MACROS

//所需的FFmpeg和SDL库的头文件
extern "C"
{
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
#include "SDL2/SDL.h"
#include "SDL2/SDL_video.h"
};

int main(int argc, char* argv[])
{
	//用户输入验证
	if (argc != 2)
	{
		printf("Usage：%s <path of the video file>\n", argv[0]);
	}

	//FFmpeg所需声明
	AVFormatContext* formatContext;//封装格式上下文结构体
	AVCodecContext* codecContext;//编解码器上下文结构体
	AVCodec* codec;//对应一种编解码器
	AVPacket* packet;//用于存储一帧压缩编码数据
	AVFrame* frame, * frameYUV;//用于存储一帧解码后像素数据
	uint8_t* buffer;//用于存储图像数据
	struct SwsContext* convertContext;//用于视频图像转换
	int videoindex = -1;//视频流索引
	int i, dec, picture;//用于循环

	//SDL所需声明
	int window_w, window_h;//窗口宽高     
	SDL_Window* sdlWindow;//窗口
	SDL_Renderer* sdlRenderer;//渲染器
	SDL_Texture* sdlTexture;//纹理
	SDL_Rect sdlRect;//矩形窗

	av_register_all();//注册所有组件
	avformat_network_init();
	formatContext = avformat_alloc_context();
	//打开输入视频文件
	if (avformat_open_input(&formatContext, argv[1], NULL, NULL) != 0)
	{
		printf("Couldn't open the input file\n");
		return -1;
	}
	//获取视频文件中的信息
	if (avformat_find_stream_info(formatContext, NULL) < 0)
	{
		printf("Couldn't find the stream information\n");
		return -1;
	}
	//在流信息中查找视频流，区分于音频流
	for (i = 0; i < formatContext->nb_streams; i++)
	{
		if (formatContext->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO)
		{
			videoindex = i;
			break;
		}
	}
	if (videoindex == -1)
	{
		printf("Couldn't find a video stream\n");
		return -1;
	}
	//查找解码器
	codecContext = formatContext->streams[videoindex]->codec;
	codec = avcodec_find_decoder(codecContext->codec_id);
	if (codec == NULL)
	{
		printf("Couldn't find decoder\n");
		return -1;
	}
	//打开解码器
	if (avcodec_open2(codecContext, codec, NULL) < 0)
	{
		printf("Couldn't open decoder\n");
		return -1;
	}
	//为图像帧分配内存并进行转换
	packet = (AVPacket*)av_malloc(sizeof(AVPacket));
	frame = av_frame_alloc();
	frameYUV = av_frame_alloc();
	buffer = (uint8_t*)av_malloc(avpicture_get_size(AV_PIX_FMT_YUV420P, codecContext->width, codecContext->height));
	avpicture_fill((AVPicture*)frameYUV, buffer, AV_PIX_FMT_YUV420P, codecContext->width, codecContext->height);
	convertContext = sws_getContext(codecContext->width, codecContext->height, codecContext->pix_fmt,
		codecContext->width, codecContext->height, AV_PIX_FMT_YUV420P, SWS_BICUBIC, NULL, NULL, NULL);

	//初始化SDL系统
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER))
	{
		printf("Couldn't initialize SDL\n");
		return -1;
	}
	//创建窗口
	window_w = codecContext->width;
	window_h = codecContext->height;
	sdlWindow = SDL_CreateWindow("Video Player", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, window_w,
		window_h, SDL_WINDOW_OPENGL);
	if (!sdlWindow)
	{
		printf("Couldn't create SDL window\n");
		return -1;
	}
	//创建渲染器
	sdlRenderer = SDL_CreateRenderer(sdlWindow, -1, 0);
	//创建纹理
	sdlTexture = SDL_CreateTexture(sdlRenderer, SDL_PIXELFORMAT_IYUV, SDL_TEXTUREACCESS_STREAMING,
		codecContext->width, codecContext->height);
	//设置矩形窗的位置和大小
	sdlRect.x = 0;
	sdlRect.y = 0;
	sdlRect.w = window_w;
	sdlRect.h = window_h;

	//下面进入循环：读取-解码-显示
	for (;;)
	{
		if (av_read_frame(formatContext, packet) >= 0)
		{
			if (packet->stream_index == videoindex)
			{
				dec = avcodec_decode_video2(codecContext, frame, &picture, packet);
				if (dec < 0)
				{
					printf("Decode failed\n");
					return -1;
				}
				if (picture)
				{
					//去除无效数据
					sws_scale(convertContext, (const uint8_t* const*)frame->data, frame->linesize, 0, codecContext->height, frameYUV->data, frameYUV->linesize);
					//SDL显示
					SDL_UpdateTexture(sdlTexture, NULL, frameYUV->data[0], frameYUV->linesize[0]);
					SDL_RenderClear(sdlRenderer);
					SDL_RenderCopy(sdlRenderer, sdlTexture, NULL, NULL);
					SDL_RenderPresent(sdlRenderer);
					SDL_Delay(40);
				}
			}
			else
				av_free_packet(packet);

		}
		else
			break;
	}

	sws_freeContext(convertContext);
	SDL_Quit();
	av_frame_free(&frameYUV);
	av_frame_free(&frame);
	avcodec_close(codecContext);
	avformat_close_input(&formatContext);

	return 0;
}
