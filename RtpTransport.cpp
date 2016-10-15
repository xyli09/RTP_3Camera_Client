#include "StdAfx.h"
#include "RtpTransport.h"

#ifdef _DEBUG
#define TRACE(s) OutputDebugStringA(s)
#else // _DEBUG
#define TRACE(s)
#endif // _DEBUG

CRtpTransport::CRtpTransport(void)
:m_destIp(0)
,m_strDestIp("")
,m_nDestPort(0)
,m_nPortBase(3000)
{
	m_uWidth = 640;             //自己定义的，我晕！！
	m_uHeight = 480;
	bgrImg = cvCreateImage(cvSize( m_uWidth, m_uHeight ), IPL_DEPTH_8U, 3);
	WSADATA wsaData;
	WORD wVersionRequested = MAKEWORD( 2, 2 );
	WSAStartup(wVersionRequested, &wsaData);
	ffmpegobj.Ffmpeg_Decoder_Init();//初始化解码器
}

CRtpTransport::~CRtpTransport(void)
{
	  WSACleanup();
	  ffmpegobj.Ffmpeg_Decoder_Close();//关闭解码器
}

void CRtpTransport::checkerror( int err )
{
	if (err < 0) 
	{   
		string errstr = jrtplib::RTPGetErrorString(err);    
		printf("Error:%s\\n", errstr);    
		exit(-1);  
	}   
}

void CRtpTransport::SetRtpInfo( string strIp,int ndestPort )
{
    //用于保存用inet_addr转换后的ip地址
 //   unsigned long destIp =  inet_addr(strIp.c_str());
	//if (destIp == INADDR_NONE)//INADDR_NONE表示255.255.255.255
	//{
	//	std::cerr << "Bad IP address specified" << std::endl;
	//	return ;
	//}

	//inet_addr返回的是网络字节的地址，这里必须转换为主机字节
    //destIp = ntohl(destIp);
   
	transparams.SetPortbase(m_nPortBase);
	sessparams.SetUsePollThread(1);
	sessparams.SetOwnTimestampUnit(1.0 / 10.0);
	int status = m_rtpSess.Create(sessparams,&transparams);	
	checkerror(status);
	

}



//testMyDecoder
void CRtpTransport::testDecoder(unsigned char *nalBuf, int nBufLen, unsigned char &ID)
{
	
	
	//FILE *pf = NULL;
	//fopen_s(&pf, "myData2.h264", "rb");
	ffmpegobj.filebuf = nalBuf;
	ffmpegobj.nDataLen = nBufLen;
	//while (true)
	{
		//ffmpegobj.nDataLen = fread(ffmpegobj.filebuf, 1, 1024 * 10, pf);//读取文件数据
		if (ffmpegobj.nDataLen <= 0)
		{
			//fclose(pf);
			//break;
		}
		else
		{
			ffmpegobj.haveread = 0;
			while (ffmpegobj.nDataLen > 0)
			{
				int nLength = av_parser_parse2(ffmpegobj.avParserContext, ffmpegobj.c, &ffmpegobj.yuv_buff,
					&ffmpegobj.nOutSize, ffmpegobj.filebuf + ffmpegobj.haveread, ffmpegobj.nDataLen, 0, 0, 0);//查找帧头
				ffmpegobj.nDataLen -= nLength;//查找过后指针移位标志
				ffmpegobj.haveread += nLength;
				if (ffmpegobj.nOutSize <= 0)
				{
					continue;
				}
				ffmpegobj.avpkt.size = ffmpegobj.nOutSize;//将帧数据放进包中
				ffmpegobj.avpkt.data = ffmpegobj.yuv_buff;
				while (ffmpegobj.avpkt.size > 0)
				{
					ffmpegobj.decodelen = avcodec_decode_video2(ffmpegobj.c, ffmpegobj.m_pYUVFrame, &ffmpegobj.piclen, &ffmpegobj.avpkt);//解码
					if (ffmpegobj.decodelen < 0)
					{
						break;
					}
					if (ffmpegobj.piclen)
					{
						ffmpegobj.scxt = sws_getContext(ffmpegobj.c->width, ffmpegobj.c->height, ffmpegobj.c->pix_fmt, ffmpegobj.c->width, ffmpegobj.c->height, PIX_FMT_BGR24, SWS_POINT, NULL, NULL, NULL);//初始化格式转换函数
						if (ffmpegobj.scxt != NULL)
						{
							avpicture_fill((AVPicture*)ffmpegobj.m_pRGBFrame, (uint8_t*)ffmpegobj.rgb_buff, PIX_FMT_RGB24, ffmpegobj.c->width, ffmpegobj.c->height);//将rgb_buff填充到m_pRGBFrame
							if (avpicture_alloc((AVPicture *)ffmpegobj.m_pRGBFrame, PIX_FMT_RGB24, ffmpegobj.c->width, ffmpegobj.c->height) >= 0)
							{
								sws_scale(ffmpegobj.scxt, ffmpegobj.m_pYUVFrame->data, ffmpegobj.m_pYUVFrame->linesize, 0,
									ffmpegobj.c->height, ffmpegobj.m_pRGBFrame->data, ffmpegobj.m_pRGBFrame->linesize);   //编码
								//wcout<<ffmpegobj.m_pRGBFrame->pts;
								//stringstream sstr;
								

								//sstr << ffmpegobj.m_pRGBFrame->pts;
								//string str = sstr.str();
								//const char *k = str.c_str();
								//TRACE(k);
								ffmpegobj.Ffmpeg_Decoder_Show(ffmpegobj.m_pRGBFrame, ffmpegobj.c->width, ffmpegobj.c->height);//解码图像显示
							}
							sws_freeContext(ffmpegobj.scxt);//释放格式转换器资源
							avpicture_free((AVPicture *)ffmpegobj.m_pRGBFrame);//释放帧资源
							av_free_packet(&ffmpegobj.avpkt);//释放本次读取的帧内存
						}
					}
					ffmpegobj.avpkt.size -= ffmpegobj.decodelen;
					ffmpegobj.avpkt.data += ffmpegobj.decodelen;
				}
			}
		}
	}
	
}

void CRtpTransport::ProcessRTPPacket(const jrtplib::RTPSourceData &srcdat,const jrtplib::RTPPacket &rtppack, unsigned char &ID)
{
	// You can inspect the packet and the source's info here
	cout << "Got packet " << rtppack.GetPayloadData() << " from SSRC " << srcdat.GetSSRC() << std::endl;
    bool bMark = rtppack.HasMarker();
	if (!bMark)
	{
		m_rtpBuf.AppendBuf(rtppack.GetPayloadData(),rtppack.GetPayloadLength());
		//testDecoder(m_rtpBuf.pktBuf, m_rtpBuf.m_nBufLen, ID);
	}
	else
	{
        m_rtpBuf.AppendBuf(rtppack.GetPayloadData(),rtppack.GetPayloadLength());
		//testDecoder(m_rtpBuf.pktBuf, m_rtpBuf.m_nBufLen, ID); //mydecoder
		//ffmpegobj.m_decoder_show(m_rtpBuf.pktBuf, m_rtpBuf.m_nBufLen, ID);
		bgrImg = m_h264Coder.RecvNalBuffer(m_rtpBuf.pktBuf, m_rtpBuf.m_nBufLen, ID);   //test

		m_rtpBuf.ReleaseBuffer();
	}
}
void CRtpTransport::RecvRtpPacket(int nWidth, int nHeight, unsigned char &ID)
{
	transparams.SetPortbase(m_nPortBase);
	sessparams.SetUsePollThread(1);
	sessparams.SetOwnTimestampUnit(1.0 / 10.0);
	int status = m_rtpSess.Create(sessparams,&transparams);	

	Sleep(5);
#ifndef RTP_SUPPORT_THREAD
	status = m_rtpSess.Poll();
	checkerror(status);
#endif
	//while(1)
	{
		m_rtpSess.BeginDataAccess();
		// check incoming packets
		if (m_rtpSess.GotoFirstSourceWithData())
		{
			do
			{
				jrtplib::RTPPacket *pack;
				jrtplib::RTPSourceData *srcdat;				
				srcdat = m_rtpSess.GetCurrentSourceInfo();				
				while ((pack = m_rtpSess.GetNextPacket()) != NULL)
				{					
					ProcessRTPPacket(*srcdat, *pack, ID);
					m_rtpSess.DeletePacket(pack);
				}
			} 
			while (m_rtpSess.GotoNextSourceWithData());
		}

		m_rtpSess.EndDataAccess();
	}
}