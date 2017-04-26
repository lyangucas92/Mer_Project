#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <stdlib.h>
#include <cstdlib>
#include <fstream>

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/structured_light.hpp>
#include <opencv2/calib3d.hpp>
#include <opencv2/opencv_modules.hpp>
#include <opencv2/viz.hpp>
#include "afxwin.h"

#include "GxIAPI.h"
#include "DxImageProc.h"


#define MaxImageWidth  2592
#define MaxImageHeigth 1944
#define WChouYang 1
#define HChouYang 1
#define ON	true
#define OFF false
#define LeftCameraIndex  1
#define RightCameraIndex 2
#define Sanp_Mode 0

using namespace std;
using namespace cv;

// CProjCalibDlg 对话框
class CProjCalibDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CProjCalibDlg)

public:
	CProjCalibDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CProjCalibDlg();
	
	// 对话框数据
	enum { IDD = IDD_ProjectorCalib };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	static void __stdcall getLeftImageV1(GX_FRAME_CALLBACK_PARAM* pFrame);
	DECLARE_MESSAGE_MAP()

public:
	void DrawPicToHDC(IplImage *img, UINT ID);

public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedButopencam();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedClosecam();
	afx_msg void OnBnClickedclosedisp();
	afx_msg void OnBnClickedImagecap();
	CString CalibMessage;
};
