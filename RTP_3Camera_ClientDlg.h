
// RTP_3Camera_ClientDlg.h : 头文件
//

#pragma once
#include "RtpTransport.h"
#include "cv.h"
#include "highgui.h"
#include "CvvImage.h"
#include "Ffmpeg_Decode.h"


// CRTP_3Camera_ClientDlg 对话框
class CRTP_3Camera_ClientDlg : public CDialogEx
{
// 构造
public:
	CRTP_3Camera_ClientDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_RTP_3CAMERA_CLIENT_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	CRect rect;
	HICON m_hIcon;
	CStatic* pStc; //标识图像显示的Picture控件
	CDC* pDC; //视频显示控件设备上下文
	HDC hDC; //视频显示控件设备句柄
	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	Ffmpeg_Decoder *m_decoder;
	
	DECLARE_MESSAGE_MAP()
public:
	CRtpTransport rtpTransport;
	void DrawPicToHDC(IplImage * img , UINT ID);
	afx_msg void OnBnClickedOk();

	void OnOpen(IplImage * img);//test
	void OnTimer(UINT nIDEvent);//test
	BOOL OnInitDialog22();//test
	void testDecoder();
};
