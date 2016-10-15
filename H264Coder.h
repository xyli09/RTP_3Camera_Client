#pragma once
#include <opencv\cv.h>
#include <opencv\highgui.h>

extern "C"
{

#include "libavcodec\avcodec.h"
#include "libavutil\avutil.h"
#include "libswscale\swscale.h"

#pragma comment(lib,"avcodec.lib")
#pragma comment(lib,"avutil.lib")
//#pragma comment(lib,"swscale.lib")

}



class CH264Coder
{
public:
	CH264Coder(void);
	~CH264Coder(void);
	bool InitDecoder();
	bool InputFilePath(char *path);
	//void RecvNalBuffer(unsigned char *nalBuf,int nBufLen);
	IplImage* RecvNalBuffer( unsigned char *nalBuf, int nBufLen, unsigned char &ID );
	void YUV2BGR( IplImage* bgrImg, int nWidth, int nHeight, AVFrame* pic );
	
private:
	
	
	
   
private:
	AVCodec *codec;			  /* 编解码CODEC*/
	AVCodecContext *c1;		  /* 编解码CODEC context*/
	AVCodecContext *c2;		  /* 编解码CODEC context*/
	AVCodecContext *c3;		  /* 编解码CODEC context*/
	AVFrame *picture1;		  /* 解码后的图像*/	
	AVFrame *picture2;		  /* 解码后的图像*/	
	AVFrame *picture3;		  /* 解码后的图像*/	

	FILE *m_inFile;
	FILE * m_outFile;

public:

};
