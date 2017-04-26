/********************************************************
*
*功能：相机标定对话框程序
*作者：lyangucas
*说明：
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
#include <opencv2/calib3d.hpp>
#include "afxwin.h"

using namespace std;
using namespace cv;


// CTipCalibDlg 对话框
class CTipCalibDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CTipCalibDlg)

public:
	CTipCalibDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CTipCalibDlg();

	// 对话框数据
	enum { IDD = IDD_DigCalib };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	DECLARE_MESSAGE_MAP()
	

public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();

	CString CalibStatus;

	int imageWidth;
	int imageHeight;
	int frameNumber;
	int squareSize;
	CButton m_check1;
	CButton m_check2;
};
