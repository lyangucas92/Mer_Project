/********************************************************
*
*���ܣ�ͼ����Ի������
*���ߣ�lyangucas
*˵����
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

#include "CvvImage.h"

using namespace cv;
using namespace std;


// CTipImage�Ի���
class CTipImage : public CDialogEx
{
	DECLARE_DYNAMIC(CTipImage)

public:
	CTipImage(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CTipImage();

	// �Ի�������
	enum { IDD = IDD_DigImage };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV֧��
	DECLARE_MESSAGE_MAP()

public:
	Mat leftImg;
	Mat rightImg;

public:
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedLoadimage();
	void DrawPicToHDC_Img(IplImage *img, UINT ID);
	afx_msg void OnBnClickedButtfilter();
};
