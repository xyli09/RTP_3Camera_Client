#include "StdAfx.h"
#include "H264Coder.h"


CH264Coder::CH264Coder(void)
{

}
CH264Coder::~CH264Coder(void)
{

	/*3. 关闭CODEC，释放资源,调用decode_end本地函数*/
	if(c1) 
	{
		avcodec_close(c1); 
		av_free(c1);
		c1 = NULL;
	} 
	if(c2) 
	{
		avcodec_close(c2); 
		av_free(c2);
		c2 = NULL;
	} 
	if(c3) 
	{
		avcodec_close(c3); 
		av_free(c3);
		c3 = NULL;
	} 

	if (NULL != m_outFile)
	{
		fclose(m_outFile);
		m_outFile = NULL;
	}

	if (NULL != picture1)
	{
		free(picture1);
		picture1 = NULL;
	}

	if (NULL != picture2)
	{
		free(picture2);
		picture2 = NULL;
	}

	if (NULL != picture3)
	{
		free(picture3);
		picture3 = NULL;
	}


}
bool CH264Coder::InitDecoder()
{
	/*CODEC的初始化，初始化一些常量表*/
	//avcodec_init(); 

	/*注册CODEC*/
	avcodec_register_all(); 

	/*查找 H264 CODEC*/
	codec = avcodec_find_decoder(CODEC_ID_H264);

	if (!codec) 
		return 0; 

	/*初始化CODEC的默认参数*/
	c1 = avcodec_alloc_context3(codec); 
	c1->max_b_frames = 0;
	//if (c1->priv_data){}
	if(!c1)  
		return 0;

	/*1. 打开CODEC，这里初始化H.264解码器，调用decode_init本地函数*/
	if (avcodec_open2(c1, codec, NULL) < 0) 	
		return 0;  

	/*为AVFrame申请空间，并清零*/
	picture1   = avcodec_alloc_frame();

	if(!picture1) 	
		return 0;
	///////////////////////////////////////////////////////////
	c2 = avcodec_alloc_context3(codec); 
	c2->max_b_frames = 0;
	if(!c2)  
		return 0;

	/*1. 打开CODEC，这里初始化H.264解码器，调用decode_init本地函数*/
	if (avcodec_open2(c2, codec, NULL) < 0) 	
		return 0;  

	/*为AVFrame申请空间，并清零*/
	picture2   = avcodec_alloc_frame();

	if(!picture2) 	
		return 0;
	////////////////////////////////////////////////////////////
	c3 = avcodec_alloc_context3(codec); 
	c3->max_b_frames = 0;
	if(!c3)  
		return 0;

	/*1. 打开CODEC，这里初始化H.264解码器，调用decode_init本地函数*/
	if (avcodec_open2(c3, codec, NULL) < 0) 	
		return 0;  

	/*为AVFrame申请空间，并清零*/
	picture3   = avcodec_alloc_frame();

	if(!picture3) 	
		return 0;
	return 1;
} 

bool CH264Coder::InputFilePath( char *path)
{
	bool bRet = true;
  
    m_outFile = fopen("clientRecv.yuv","wb");
	if (NULL == m_outFile)
	{
		bRet = false;
	}

    return bRet;

}

//自己搞得
void CH264Coder::YUV2BGR( IplImage* bgrImg, int nWidth, int nHeight, AVFrame* pic_YUV )
{
	AVFrame* pic_BGR = avcodec_alloc_frame();
	//avpicture_alloc( pic_BGR, PIX_FMT_BGR24, img->width, img->height );
	avpicture_fill( (AVPicture*)pic_BGR, (uint8_t*)bgrImg->imageData, PIX_FMT_BGR24, nWidth, nHeight );
	SwsContext* swc_YUV2BGR = sws_getContext(nWidth, nHeight, PIX_FMT_YUV420P, 
		nWidth, nHeight, PIX_FMT_BGR24,  SWS_POINT ,NULL,NULL,NULL);
	sws_scale(swc_YUV2BGR, pic_YUV->data, pic_YUV->linesize, 0, nHeight, pic_BGR->data, pic_BGR->linesize );
	avpicture_layout( (AVPicture*)pic_BGR, PIX_FMT_BGR24, nWidth, nHeight, (uchar*)bgrImg->imageData, bgrImg->imageSize );
	sws_freeContext(swc_YUV2BGR);
	av_frame_free(&pic_BGR);
}



IplImage* CH264Coder::RecvNalBuffer( unsigned char *nalBuf, int nBufLen, unsigned char &ID )
{
	int  got_picture;		/*是否解码一帧图像*/
	int  consumed_bytes; /*解码器消耗的码流长度*/
	int cnt=0;
	int i;

    AVPacket avPkt;
	avPkt.data = (uint8_t *)malloc(1024*1024*4*sizeof(uint8_t));
	avPkt.size = 0;
	ID = nalBuf[nBufLen-1];
	//ID = 0X01;
	memcpy(avPkt.data, nalBuf, nBufLen-1);
	avPkt.size = nBufLen-1;	
	
	if(0x01 == ID)
	{
		consumed_bytes = avcodec_decode_video2(c1, picture1, &got_picture, &avPkt);
		static IplImage* bgrImg1 = cvCreateImage(cvSize( c1->width, c1->height ), IPL_DEPTH_8U, 3);//有问题，不能有效传递长宽

		AVFrame* pic_BGR = avcodec_alloc_frame();
		avpicture_fill( (AVPicture*)pic_BGR, (uint8_t*)bgrImg1->imageData, PIX_FMT_BGR24, c1->width, c1->height ); //此处修改bgrimg1为c1，解决上述bug
		SwsContext* swc_YUV2BGR = sws_getContext(bgrImg1->width, bgrImg1->height, PIX_FMT_YUV420P, bgrImg1->width, bgrImg1->height, PIX_FMT_BGR24,  SWS_POINT ,NULL,NULL,NULL); //init swscontext
		sws_scale(swc_YUV2BGR, picture1->data, picture1->linesize, 0, c1->height, pic_BGR->data, pic_BGR->linesize );
		avpicture_layout( (AVPicture*)pic_BGR, PIX_FMT_BGR24, bgrImg1->width, bgrImg1->height, (uchar*)bgrImg1->imageData, bgrImg1->imageSize );  //copy data from avpicture into buffer
		sws_freeContext(swc_YUV2BGR);
		av_frame_free(&pic_BGR);
		if (NULL != avPkt.data)
		{
			free(avPkt.data);
			avPkt.data = NULL;
		}
		cvShowImage("解码图像", bgrImg1);
		cvWaitKey(20);
		return bgrImg1;
	}
	
	if(0x02 == ID)
	{
		consumed_bytes = avcodec_decode_video2(c2, picture2, &got_picture, &avPkt);
		static IplImage* bgrImg2 = cvCreateImage(cvSize( c2->width, c2->height ), IPL_DEPTH_8U, 3);

		AVFrame* pic_BGR = avcodec_alloc_frame();
		avpicture_fill( (AVPicture*)pic_BGR, (uint8_t*)bgrImg2->imageData, PIX_FMT_BGR24, c2->width, c2->height );
		SwsContext* swc_YUV2BGR = sws_getContext(bgrImg2->width, bgrImg2->height, PIX_FMT_YUV420P, 
			bgrImg2->width, bgrImg2->height, PIX_FMT_BGR24,  SWS_POINT ,NULL,NULL,NULL);
		sws_scale(swc_YUV2BGR, picture2->data, picture2->linesize, 0, c2->height, pic_BGR->data, pic_BGR->linesize );
		avpicture_layout( (AVPicture*)pic_BGR, PIX_FMT_BGR24, bgrImg2->width, bgrImg2->height, (uchar*)bgrImg2->imageData, bgrImg2->imageSize );
		sws_freeContext(swc_YUV2BGR);
		av_frame_free(&pic_BGR);
		if (NULL != avPkt.data)
		{
			free(avPkt.data);
			avPkt.data = NULL;
		}
		return bgrImg2;
	}

	if(0x03 == ID)
	{
		consumed_bytes = avcodec_decode_video2(c3, picture3, &got_picture, &avPkt);
		static IplImage* bgrImg3 = cvCreateImage(cvSize( c3->width, c3->height ), IPL_DEPTH_8U, 3);

		AVFrame* pic_BGR = avcodec_alloc_frame();
		avpicture_fill( (AVPicture*)pic_BGR, (uint8_t*)bgrImg3->imageData, PIX_FMT_BGR24, bgrImg3->width, bgrImg3->height );
		SwsContext* swc_YUV2BGR = sws_getContext(bgrImg3->width, bgrImg3->height, PIX_FMT_YUV420P, 
			bgrImg3->width, bgrImg3->height, PIX_FMT_BGR24,  SWS_POINT ,NULL,NULL,NULL);
		sws_scale(swc_YUV2BGR, picture3->data, picture3->linesize, 0, c3->height, pic_BGR->data, pic_BGR->linesize );
		avpicture_layout( (AVPicture*)pic_BGR, PIX_FMT_BGR24, bgrImg3->width, bgrImg3->height, (uchar*)bgrImg3->imageData, bgrImg3->imageSize );
		sws_freeContext(swc_YUV2BGR);
		av_frame_free(&pic_BGR);
		if (NULL != avPkt.data)
		{
			free(avPkt.data);
			avPkt.data = NULL;
		}
		return bgrImg3;
	}
	

	/*2. NAL解码,调用decode_frame本地函数*/
	//consumed_bytes = avcodec_decode_video2(c1, picture, &got_picture, &avPkt);

	//cnt++;
		
	/*输出当前的解码信息*/

	//static IplImage* bgrImg = cvCreateImage(cvSize( c->width, c->height ), IPL_DEPTH_8U, 3);

	//YUV2BGR( bgrimg, c->width, c->height, picture );
	//AVFrame* pic_BGR = avcodec_alloc_frame();
	//avpicture_alloc( pic_BGR, PIX_FMT_BGR24, img->width, img->height );

	/*
	avpicture_fill( (AVPicture*)pic_BGR, (uint8_t*)bgrImg->imageData, PIX_FMT_BGR24, bgrImg->width, bgrImg->height );
	SwsContext* swc_YUV2BGR = sws_getContext(bgrImg->width, bgrImg->height, PIX_FMT_YUV420P, 
		bgrImg->width, bgrImg->height, PIX_FMT_BGR24,  SWS_POINT ,NULL,NULL,NULL);
	sws_scale(swc_YUV2BGR, picture->data, picture->linesize, 0, c->height, pic_BGR->data, pic_BGR->linesize );
	avpicture_layout( (AVPicture*)pic_BGR, PIX_FMT_BGR24, bgrImg->width, bgrImg->height, (uchar*)bgrImg->imageData, bgrImg->imageSize );
	sws_freeContext(swc_YUV2BGR);
	av_frame_free(&pic_BGR);
	
	if (NULL != avPkt.data)
	{
		free(avPkt.data);
		avPkt.data = NULL;
	}
	return bgrImg;
	*/
}

bool YV12ToBGR24_FFmpeg(unsigned char* pYUV, unsigned char* pBGR24, int width, int height)
{
	if (width < 1 || height < 1 || pYUV == NULL || pBGR24 == NULL)
		return false;
	//int srcNumBytes,dstNumBytes;
	//uint8_t *pSrc,*pDst;
	AVPicture pFrameYUV, pFrameBGR;

	//pFrameYUV = avpicture_alloc();
	//srcNumBytes = avpicture_get_size(PIX_FMT_YUV420P,width,height);
	//pSrc = (uint8_t *)malloc(sizeof(uint8_t) * srcNumBytes);
	avpicture_fill(&pFrameYUV, pYUV, PIX_FMT_YUV420P, width, height);

	//U,V互换
	uint8_t * ptmp = pFrameYUV.data[1];
	pFrameYUV.data[1] = pFrameYUV.data[2];
	pFrameYUV.data[2] = ptmp;

	//pFrameBGR = avcodec_alloc_frame();
	//dstNumBytes = avpicture_get_size(PIX_FMT_BGR24,width,height);
	//pDst = (uint8_t *)malloc(sizeof(uint8_t) * dstNumBytes);
	avpicture_fill(&pFrameBGR, pBGR24, PIX_FMT_BGR24, width, height);

	struct SwsContext* imgCtx = NULL;
	imgCtx = sws_getContext(width, height, PIX_FMT_YUV420P, width, height, PIX_FMT_BGR24, SWS_BILINEAR, 0, 0, 0);

	if (imgCtx != NULL){
		sws_scale(imgCtx, pFrameYUV.data, pFrameYUV.linesize, 0, height, pFrameBGR.data, pFrameBGR.linesize);
		if (imgCtx){
			sws_freeContext(imgCtx);
			imgCtx = NULL;
		}
		return true;
	}
	else{
		sws_freeContext(imgCtx);
		imgCtx = NULL;
		return false;
	}
}