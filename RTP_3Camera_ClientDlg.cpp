
// RTP_3Camera_ClientDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "RTP_3Camera_Client.h"
#include "RTP_3Camera_ClientDlg.h"
#include "afxdialogex.h"
#include "RtpTransport.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define DEST_IP  "211.211.211.37"
using namespace cv;
using namespace std;

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

DWORD WINAPI DrawPicture(LPVOID lpparam)
{
	unsigned char ID = 0;
	CRTP_3Camera_ClientDlg* p = (CRTP_3Camera_ClientDlg*)lpparam;
	//IplImage* bgrImg = cvLoadImage("hello.jpg");

	p->OnInitDialog22();
	//p->OnOpen();
	//p->test(p->rtpTransport.m_uWidth, p->rtpTransport.m_uHeight, ID);
	while(1)
	{
		//p->DrawPicToHDC(bgrImg, IDC_ShowImg1);	
		
		p->rtpTransport.RecvRtpPacket( p->rtpTransport.m_uWidth, p->rtpTransport.m_uHeight, ID);
		if (p->rtpTransport.bgrImg->imageData == NULL)
			continue;
		//if( 0x01 == ID )
		//	p->DrawPicToHDC(p->rtpTransport.bgrImg, IDC_ShowImg);
		//if (0x02 == ID)
		//	/*if (p->rtpTransport.bgrImg)
		//	p->OnOpen(p->rtpTransport.bgrImg);*/
		//	p->DrawPicToHDC(p->rtpTransport.bgrImg, IDC_ShowImg1);
		//if( 0x03 == ID )
		//	p->DrawPicToHDC(p->rtpTransport.bgrImg, IDC_ShowImg2);
	}
	return 0x01;
}



void CRTP_3Camera_ClientDlg::OnOpen(IplImage * img)
{
	//IplImage * img = cvLoadImage("hello.jpg");//load image from file
	CvvImage cvvImg; //CvvImage类
	cvvImg.CopyOf(img);//复制图像到当前的CvvImage对象中  
	cvvImg.DrawToHDC(hDC, &rect); //绘制图像的ROI区域到DC的rect
}

void CRTP_3Camera_ClientDlg::OnTimer(UINT nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	if (1 == nIDEvent)
	{
		IplImage* img = 0;
		//img = cvQueryFrame(capture); //从摄像头或者文件中抓取并返回一帧
		//cvGrabFrame( capture ); //从摄像头或者视频文件中抓取帧
		//img=cvRetrieveFrame(capture); //取回由函数cvGrabFrame抓取的图像
		CvvImage m_CvvImage;
		m_CvvImage.CopyOf(img, 1); //复制该帧图像    
		m_CvvImage.DrawToHDC(hDC, &rect); //显示到设备的矩形框内
	}
	CDialog::OnTimer(nIDEvent);
}


BOOL CRTP_3Camera_ClientDlg::OnInitDialog22()
{
	//框架生成代码.......

	pStc = (CStatic *)GetDlgItem(IDC_ShowImg);//IDC_VIEW为Picture控件ID
	pStc->GetClientRect(&rect);//将CWind类客户区的坐标点传给矩形
	pDC = pStc->GetDC(); //得到Picture控件设备上下文
	hDC = pDC->GetSafeHdc(); //得到控件设备上下文的句柄 

	return   TRUE;
}

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CRTP_3Camera_ClientDlg 对话框



CRTP_3Camera_ClientDlg::CRTP_3Camera_ClientDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CRTP_3Camera_ClientDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CRTP_3Camera_ClientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CRTP_3Camera_ClientDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, &CRTP_3Camera_ClientDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CRTP_3Camera_ClientDlg 消息处理程序

BOOL CRTP_3Camera_ClientDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。
	
	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	rtpTransport.SetRtpInfo(DEST_IP,3000);
	rtpTransport.m_h264Coder.InitDecoder();

	DWORD dwthread;
	CreateThread(NULL, 0, DrawPicture, (LPVOID)this, 0, &dwthread);
	m_decoder = new Ffmpeg_Decoder;

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CRTP_3Camera_ClientDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CRTP_3Camera_ClientDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CRTP_3Camera_ClientDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CRTP_3Camera_ClientDlg::DrawPicToHDC(IplImage * img , UINT ID)
{
	CDC *pDC = GetDlgItem(ID)->GetDC();
	HDC hDC= pDC->GetSafeHdc();
	CRect rect;
	GetDlgItem(ID)->GetClientRect(&rect);
	CvvImage cimg;
	cimg.CopyOf( img ); // 复制图片
	cimg.DrawToHDC( hDC, &rect ); // 将图片绘制到显示控件的指定区域内
	ReleaseDC( pDC );
}

void CRTP_3Camera_ClientDlg::testDecoder()
{
	Ffmpeg_Decoder ffmpegobj;
	ffmpegobj.Ffmpeg_Decoder_Init();//初始化解码器
	FILE *pf = NULL;
	fopen_s(&pf, "myData2.h264", "rb");
	while (true)
	{
		ffmpegobj.nDataLen = fread(ffmpegobj.filebuf, 1, 1024 * 10, pf);//读取文件数据
		if (ffmpegobj.nDataLen <= 0)
		{
			fclose(pf);
			break;
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
									ffmpegobj.c->height, ffmpegobj.m_pRGBFrame->data, ffmpegobj.m_pRGBFrame->linesize);
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
	ffmpegobj.Ffmpeg_Decoder_Close();//关闭解码器
}


void CRTP_3Camera_ClientDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	CDialogEx::OnOK();
}
