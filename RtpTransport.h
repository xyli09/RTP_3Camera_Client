#pragma once

#include "stdafx.h"
#include "rtpudpv4transmitter.h"
#include "rtpsessionparams.h"//这两个头文件定义了创建RTPSession所需的两个参数

#include "rtpipv4address.h" 

#include "rtppacket.h"           //定义了RTPPacket数据包
#include "rtpsourcedata.h"
#include "rtperrors.h"
#include "rtpsession.h"   


#include "H264Coder.h"
#include "Ffmpeg_Decode.h"    //myDecoder

#include <iostream>
#include <string>
using namespace std; 


typedef struct tagRtpPacket
{
   tagRtpPacket()
   {
      pktBuf = (unsigned char*)malloc(500*1020*sizeof(unsigned char));
	  m_nBufLen = 0;
   }

   ~tagRtpPacket()
   {
      if (NULL != pktBuf)
      {
		  free(pktBuf);
		  pktBuf = NULL;
      }
   }
   void AppendBuf(unsigned char *buf,int nLen)
   {
	   if (NULL != buf)
	   {
		   memcpy(pktBuf + m_nBufLen,buf,nLen);
		   //memcpy(pktBuf,buf,nLen);
		   m_nBufLen += nLen;
	   }
   }
   void ReleaseBuffer()
   {
	   m_nBufLen = 0;
   }

   unsigned char *pktBuf;
   unsigned int m_nBufLen;

}RtpPacket;


class CRtpTransport
{
public:
	CRtpTransport(void);
	~CRtpTransport(void);

private:
	void checkerror(int err);
	void ProcessRTPPacket(const jrtplib::RTPSourceData &srcdat,const jrtplib::RTPPacket &rtppack, unsigned char &ID);

public:
	void SetRtpInfo(string strIp,int nPort);
	//void RecvRtpPacket();
	void RecvRtpPacket(int nWidth, int nHeight, unsigned char &ID);
	void testDecoder(unsigned char *nalBuf, int nBufLen, unsigned char &ID);   //
	void m_decoder_show(unsigned char *nalBuf, int nBufLen, unsigned char &ID);


private:
	jrtplib::RTPSession m_rtpSess;
	jrtplib::RTPUDPv4TransmissionParams transparams;
	jrtplib::RTPSessionParams sessparams;
    jrtplib::RTPIPv4Address addr;

	unsigned long m_destIp;
	string m_strDestIp;
	int m_nDestPort;
	int m_nPortBase;

	RtpPacket m_rtpBuf;
public:
    CH264Coder m_h264Coder;
	IplImage* bgrImg;
	unsigned int m_uWidth;
	unsigned int m_uHeight;
	Ffmpeg_Decoder ffmpegobj;
};
