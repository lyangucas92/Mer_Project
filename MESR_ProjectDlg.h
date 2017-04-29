/********************************************************
*
*功能：主对话框程序
*作者：lyangucas
*说明：
*	1.MESR_ProjectDlg.h : 头文件
*
*********************************************************/

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

#include "grayCode.h"
#include "TipImage.h"
#include "TipCalibDlg.h"
#include "CameraOperation.h"
#include "CvvImage.h"
#include "ProjCalibDlg.h"

using namespace std;
using namespace cv;

#define MaxImageWidth	2592
#define MaxImageHeigth  1944
#define WChouYang 1
#define HChouYang 1
#define ON	true
#define OFF false
#define LeftCameraIndex  1
#define RightCameraIndex 2
#define Sanp_Mode 0


// CMESR_ProjectDlg对话框
class CMESR_ProjectDlg : public CDialogEx
{
// 构造
public:
	CMESR_ProjectDlg(CWnd* pParent = NULL);	// 标准构造函数
	~CMESR_ProjectDlg(void);

	// 对话框数据
	enum { IDD = IDD_MESR_PROJECT_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持

public:
	CameraOperation cam_Dev;
	GX_DEV_HANDLE leftCamera;
	GX_DEV_HANDLE rightCamera;
	GX_DEV_HANDLE sinCamera;

	// 格雷码图案产生
	Ptr<structured_light::GrayCodePattern> graycode;
	structured_light::GrayCodePattern::Params params;
	vector<Mat> pattern;
	unsigned int frame_count;

	// 相移图案
	structured_light::SinusoidalPattern::Params params_sin;
	phase_unwrapping::HistogramPhaseUnwrapping::Params paramsUnwrapping;
	Ptr<phase_unwrapping::HistogramPhaseUnwrapping> phaseUnwrapping;
	Ptr<structured_light::SinusoidalPattern> sinus;
	vector<Mat> patterns_sin;
	Mat shadowMask;
	Mat unwrappedPhaseMap, unwrappedPhaseMap8;
	Mat wrappedPhaseMap, wrappedPhaseMap8;
	Mat reliabilities;
	Mat reliabilities8;
	vector<Mat> captures;
	unsigned int nbrOfImages;

	//立体相移法
	Mat shadowMask_L, shadowMask_R;
	Mat unwrappedPhaseMap_L, unwrappedPhaseMap8_L, unwrappedPhaseMap_R, unwrappedPhaseMap8_R;
	Mat wrappedPhaseMap_L, wrappedPhaseMap8_L, wrappedPhaseMap_R, wrappedPhaseMap8_R;
	Mat reliabilities_L, reliabilities_R;
	Mat reliabilities8_L, reliabilities8_R;

	// 相机标定参数
	Mat cam1intrinsics, cam1distCoeffs, cam2intrinsics, cam2distCoeffs, R, T;

	// 显示和保存
	void DrawPicToHDC(IplImage *img, UINT ID);//函数
	void saveXYZ(const char* filename, const Mat& mat);

// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	static void __stdcall getLeftImage(GX_FRAME_CALLBACK_PARAM* pFrame);
	static void __stdcall getRightImage(GX_FRAME_CALLBACK_PARAM* pFrame);
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnBnClickedOpencam();
	afx_msg void OnBnClickedCheckcam();
	afx_msg void OnBnClickedSnapstart();
	afx_msg void OnBnClickedImgshow();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedStopshow();
	afx_msg void OnBnClickedClosecam();
	afx_msg void OnBnClickedStopsnap();
	afx_msg void OnBnClickedPattern();
	afx_msg void OnBnClickedPlayimage();
	afx_msg void OnBnClickedAutoplay();
	afx_msg void OnBnClickedSaveimg();		
	afx_msg void OnBnClickedPhasepattern();
	afx_msg void OnBnClickedSinplay();
	afx_msg void OnBnClickedButton4();
	afx_msg void OnBnClickedButdecoder();
	afx_msg void OnBnClickedRectbut();	
	afx_msg void OnBnClickedDecodephase();
	afx_msg void OnBnClickedBut3drRectfy();
	afx_msg void OnBnClickedButseam();
	afx_msg void OnBnClickedCamcalib();

public:
	CComboBox DecoderDir;
	CComboBox Phase_ID;
	CComboBox Phase_Maker;
	CComboBox Phase_Direction;

	CString Phase_Status;
	int Phase_Height;
	int Phase_WeighT;
	int Phase_Period;
	CString Action_Satus;
	int Project_Width;
	int Project_Height;
	int CamCounts;
	afx_msg void OnBnClickedButexit();
	afx_msg void OnBnClickedButprojcalib();
	afx_msg void OnBnClickedButstereo3d();
};
