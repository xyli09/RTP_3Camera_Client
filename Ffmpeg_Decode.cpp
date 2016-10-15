#include "stdafx.h"
#include "Ffmpeg_Decode.h"

void Ffmpeg_Decoder::Ffmpeg_Decoder_Init()
{
	avcodec_register_all();     //注册编解码器
	av_init_packet(&avpkt);     //初始化包结构
	m_pRGBFrame = new AVFrame[1];//RGB帧数据赋值 
	m_pYUVFrame = avcodec_alloc_frame();
	filebuf = new uint8_t[1024 * 1024];//初始化文件缓存数据区
	pbuf = new uint8_t[200 * 1024];//初始化帧数据区
	yuv_buff = new uint8_t[200 * 1024];//初始化YUV图像数据区
	rgb_buff = new uint8_t[1024 * 1024];//初始化RGB图像帧数据区
	pCodecH264 = avcodec_find_decoder(CODEC_ID_H264);     //查找h264解码器
	if (!pCodecH264)
	{
		fprintf(stderr, "h264 codec not found\n");
		exit(1);
	}
	avParserContext = av_parser_init(CODEC_ID_H264);
	if (!pCodecH264)return;
	c = avcodec_alloc_context3(pCodecH264);//函数用于分配一个AVCodecContext并设置默认值，如果失败返回NULL，并可用av_free()进行释放
	c->max_b_frames = 0;
	
	if (pCodecH264->capabilities&CODEC_CAP_TRUNCATED)
		c->flags |= CODEC_FLAG_TRUNCATED;	/* we do not send complete frames */
	if (avcodec_open2(c, pCodecH264, NULL) < 0)return;
	nDataLen = 0;
	picture1 = avcodec_alloc_frame();//

	
}

void Ffmpeg_Decoder::Ffmpeg_Decoder_Show(AVFrame *pFrame, int width, int height)
{
	CvSize  rectangle_img_size;
	rectangle_img_size.height = height;
	rectangle_img_size.width = width;

	img = cvCreateImage(rectangle_img_size, IPL_DEPTH_8U, 3);
	uchar* imgdata = (uchar*)(img->imageData);     //图像的数据指针

	for (int y = 0; y<height; y++)
	{
		memcpy(imgdata + y*width * 3, pFrame->data[0] + y*pFrame->linesize[0], width * 3);
	}
	cvShowImage("解码图像", img);
	cvWaitKey(10);//可以将图像停留时间设的长点以便观察
	cvReleaseImage(&img);
	imgdata = NULL;
}

void Ffmpeg_Decoder::m_decoder_show(unsigned char *nalBuf, int nBufLen, unsigned char &ID)
{
	int  got_picture;		/*是否解码一帧图像*/
	int  consumed_bytes; /*解码器消耗的码流长度*/
	int cnt = 0;
	int i;

	AVPacket avPkt;
	avPkt.data = (uint8_t *)malloc(1024 * 1024 * 4 * sizeof(uint8_t));
	avPkt.size = 0;
	//ID = nalBuf[nBufLen - 1];
	//ID = 0X01;
	memcpy(avPkt.data, nalBuf, nBufLen);
	avPkt.size = nBufLen ;

	//if (0x01 == ID)
	{
		consumed_bytes = avcodec_decode_video2(c, picture1, &got_picture, &avPkt);
		static IplImage* bgrImg1 = cvCreateImage(cvSize(c->width, c->height), IPL_DEPTH_8U, 3);//有问题，不能有效传递长宽

		AVFrame* pic_BGR = avcodec_alloc_frame();
		avpicture_fill((AVPicture*)pic_BGR, (uint8_t*)bgrImg1->imageData, PIX_FMT_BGR24, c->width, c->height); //此处修改bgrimg1为c1，解决上述bug
		SwsContext* swc_YUV2BGR = sws_getContext(bgrImg1->width, bgrImg1->height, PIX_FMT_YUV420P, bgrImg1->width, bgrImg1->height, PIX_FMT_BGR24, SWS_POINT, NULL, NULL, NULL); //init swscontext
		sws_scale(swc_YUV2BGR, picture1->data, picture1->linesize, 0, c->height, pic_BGR->data, pic_BGR->linesize);
		avpicture_layout((AVPicture*)pic_BGR, PIX_FMT_BGR24, bgrImg1->width, bgrImg1->height, (uchar*)bgrImg1->imageData, bgrImg1->imageSize);  //copy data from avpicture into buffer
		sws_freeContext(swc_YUV2BGR);
		av_frame_free(&pic_BGR);
		if (NULL != avPkt.data)
		{
			free(avPkt.data);
			avPkt.data = NULL;
		}
		
		cvShowImage("解码图像", bgrImg1);
		cvWaitKey(10);//可以将图像停留时间设的长点以便观察
	}

}


void Ffmpeg_Decoder::Ffmpeg_Decoder_Close()
{
	delete[]filebuf;
	delete[]pbuf;
	delete[]yuv_buff;
	delete[]rgb_buff;
	av_free(m_pYUVFrame);//释放帧资源
	avcodec_close(c);//关闭解码器
	av_free(c);
}
