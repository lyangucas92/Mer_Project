// TipImage.cpp : 实现文件
//

#include "stdafx.h"
#include "MESR_Project.h"
#include "TipImage.h"
#include "afxdialogex.h"


IMPLEMENT_DYNAMIC(CTipImage, CDialogEx)
/********************************************************
*
*功能：CTipImage 对话框构造函数
*日期：2017-04-15
*说明：
*
*********************************************************/
CTipImage::CTipImage(CWnd* pParent /*=NULL*/)
	: CDialogEx(CTipImage::IDD, pParent)
{

}


/********************************************************
*
*功能：析构函数
*日期：2017-04-15
*说明：
*
*********************************************************/
CTipImage::~CTipImage()
{

}


/********************************************************
*
*功能：数据交互
*日期：2017-04-15
*说明：
*
*********************************************************/
void CTipImage::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


/********************************************************
*
*功能：按钮槽函数
*日期：2017-04-15
*说明：
*
*********************************************************/
BEGIN_MESSAGE_MAP(CTipImage, CDialogEx)
	ON_BN_CLICKED(IDCANCEL, &CTipImage::OnBnClickedCancel)
	ON_BN_CLICKED(ID_LoadImage, &CTipImage::OnBnClickedLoadimage)
	ON_BN_CLICKED(IDC_ButtFilter, &CTipImage::OnBnClickedButtfilter)
END_MESSAGE_MAP()


/********************************************************
*
*功能：将图片显示到对话框里面
*日期：2016-03-03
*说明：
*
*********************************************************/
void CTipImage::DrawPicToHDC_Img(IplImage *img, UINT ID)
{
	CDC *pDC = GetDlgItem(ID)->GetDC();
	HDC hDC = pDC->GetSafeHdc();
	CRect rect;
	GetDlgItem(ID)->GetClientRect(&rect);
	CvvImage cimg;
	cimg.CopyOf(img); //复制图片
	cimg.DrawToHDC(hDC, &rect); //将图片绘制到显示控件的指定区域内
	ReleaseDC(pDC);
}


/********************************************************
*
*功能：程序退出
*日期：2017-04-15
*说明：
*
*********************************************************/
void CTipImage::OnBnClickedCancel()
{
	CDialogEx::OnCancel();
}


/********************************************************
*
*功能：导入图像
*日期：2017-04-15
*说明：
*
*********************************************************/
void CTipImage::OnBnClickedLoadimage()
{
	// 读入图像
	leftImg = imread("pattern_cam1_im43.png", IMREAD_GRAYSCALE);
	rightImg = imread("pattern_cam2_im43.png", IMREAD_GRAYSCALE);

	// 显示
	DrawPicToHDC_Img(&(IplImage(leftImg)), IDC_STATIC_A);
	DrawPicToHDC_Img(&(IplImage(rightImg)), IDC_STATIC_B);
}


/********************************************************
*
*功能：根据提取的焊缝计算三维信息
*日期：2017-04-15
*说明：
*     (1)reprojectImageTo3D函数
*
*********************************************************/
void CTipImage::OnBnClickedButtfilter()
{
	/*
	// 高斯滤波
	GaussianBlur(leftImg, leftImg, Size(5, 5), 1.5);
	GaussianBlur(rightImg, rightImg, Size(5, 5), 1.5);
	// 中值滤波
	medianBlur(leftImg, leftImg, 5);
	medianBlur(rightImg, rightImg, 5);
	// 显示
	DrawPicToHDC_Img(&(IplImage(leftImg)), IDC_STATIC_A);
	DrawPicToHDC_Img(&(IplImage(rightImg)), IDC_STATIC_B);
	*/
}
