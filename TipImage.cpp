// TipImage.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "MESR_Project.h"
#include "TipImage.h"
#include "afxdialogex.h"


IMPLEMENT_DYNAMIC(CTipImage, CDialogEx)
/********************************************************
*
*���ܣ�CTipImage �Ի����캯��
*���ڣ�2017-04-15
*˵����
*
*********************************************************/
CTipImage::CTipImage(CWnd* pParent /*=NULL*/)
	: CDialogEx(CTipImage::IDD, pParent)
{

}


/********************************************************
*
*���ܣ���������
*���ڣ�2017-04-15
*˵����
*
*********************************************************/
CTipImage::~CTipImage()
{

}


/********************************************************
*
*���ܣ����ݽ���
*���ڣ�2017-04-15
*˵����
*
*********************************************************/
void CTipImage::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


/********************************************************
*
*���ܣ���ť�ۺ���
*���ڣ�2017-04-15
*˵����
*
*********************************************************/
BEGIN_MESSAGE_MAP(CTipImage, CDialogEx)
	ON_BN_CLICKED(IDCANCEL, &CTipImage::OnBnClickedCancel)
	ON_BN_CLICKED(ID_LoadImage, &CTipImage::OnBnClickedLoadimage)
	ON_BN_CLICKED(IDC_ButtFilter, &CTipImage::OnBnClickedButtfilter)
END_MESSAGE_MAP()


/********************************************************
*
*���ܣ���ͼƬ��ʾ���Ի�������
*���ڣ�2016-03-03
*˵����
*
*********************************************************/
void CTipImage::DrawPicToHDC_Img(IplImage *img, UINT ID)
{
	CDC *pDC = GetDlgItem(ID)->GetDC();
	HDC hDC = pDC->GetSafeHdc();
	CRect rect;
	GetDlgItem(ID)->GetClientRect(&rect);
	CvvImage cimg;
	cimg.CopyOf(img); //����ͼƬ
	cimg.DrawToHDC(hDC, &rect); //��ͼƬ���Ƶ���ʾ�ؼ���ָ��������
	ReleaseDC(pDC);
}


/********************************************************
*
*���ܣ������˳�
*���ڣ�2017-04-15
*˵����
*
*********************************************************/
void CTipImage::OnBnClickedCancel()
{
	CDialogEx::OnCancel();
}


/********************************************************
*
*���ܣ�����ͼ��
*���ڣ�2017-04-15
*˵����
*
*********************************************************/
void CTipImage::OnBnClickedLoadimage()
{
	// ����ͼ��
	leftImg = imread("pattern_cam1_im43.png", IMREAD_GRAYSCALE);
	rightImg = imread("pattern_cam2_im43.png", IMREAD_GRAYSCALE);

	// ��ʾ
	DrawPicToHDC_Img(&(IplImage(leftImg)), IDC_STATIC_A);
	DrawPicToHDC_Img(&(IplImage(rightImg)), IDC_STATIC_B);
}


/********************************************************
*
*���ܣ�������ȡ�ĺ��������ά��Ϣ
*���ڣ�2017-04-15
*˵����
*     (1)reprojectImageTo3D����
*
*********************************************************/
void CTipImage::OnBnClickedButtfilter()
{
	/*
	// ��˹�˲�
	GaussianBlur(leftImg, leftImg, Size(5, 5), 1.5);
	GaussianBlur(rightImg, rightImg, Size(5, 5), 1.5);
	// ��ֵ�˲�
	medianBlur(leftImg, leftImg, 5);
	medianBlur(rightImg, rightImg, 5);
	// ��ʾ
	DrawPicToHDC_Img(&(IplImage(leftImg)), IDC_STATIC_A);
	DrawPicToHDC_Img(&(IplImage(rightImg)), IDC_STATIC_B);
	*/
}
