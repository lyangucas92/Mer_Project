// ProjCalibDlg.cpp : ʵ���ļ�

/******************ע��ʵ��*********************************
" cam settings are parameters about the chessboard that needs to be detected to"
" calibrate the camera and proj setting are the same kind of parameters about the chessboard"
" that needs to be detected to calibrate the projector"
**********************************************************/

#include "stdafx.h"
#include "MESR_Project.h"
#include "ProjCalibDlg.h"
#include "afxdialogex.h"

#include "CvvImage.h"
#include "CameraOperation.h"
#include "projectCalib.h"


/********************************************************
*
*���ܣ����̸����ýṹ��
*���ڣ�2017-04-25
*˵����
*     (1)����ɼ������̸��ͶӰ��Ͷ�������̸���һ����
*        ������Ҫ����һ�£�
*
*********************************************************/
struct Settings
{
	Settings();
	int patternType;
	Size patternSize;
	Size subpixelSize;
	Size imageSize;
	float squareSize;
	int nbrOfFrames;
};


Settings::Settings(void)
{
	patternType = CHESSBOARD;
	patternSize = Size(9, 6);
	subpixelSize = Size(11, 11);
	squareSize = 50;
	nbrOfFrames = 12;
}


//ȫ�ֱ���
CameraOperation cam_DevP;
GX_DEV_HANDLE leftCameraP;
BYTE *leftImageBufferP;
BYTE* pImageLeftBufferP;
BYTE *leftImageVievP;
IplImage* viewleftimageP;
IplImage* frameA_showP;

Mat pattern;
vector<Point3f> tempCam;
vector<Point2f> tempProj;
Settings camSettings, projSettings;
vector< vector<Point2f> > imagePointsCam, imagePointsProj, PointsInProj, pointsInProjN, imagePointsProjN;
vector< vector<Point3f> > objectPointsCam, worldPointsProj;
vector<Mat> T2, T1;
vector<Mat> projInProj, projInCam;
vector<Mat> projInProjN, projInCamN;
vector<Mat> images;
vector<Mat> rVecs, tVecs, projectorRVecs, projectorTVecs;
Mat cameraMatrix, distCoeffs, projectorMatrix, projectorDistCoeffs;


IMPLEMENT_DYNAMIC(CProjCalibDlg, CDialogEx)
/********************************************************
*
*���ܣ�CProjCalibDlg �Ի����캯��
*���ڣ�2017-04-25
*˵����
*
*********************************************************/
CProjCalibDlg::CProjCalibDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CProjCalibDlg::IDD, pParent)
	, CalibMessage(_T(""))
{
	//��ʼ������
	leftImageBufferP = new BYTE[MaxImageWidth * MaxImageHeigth * 3];
	pImageLeftBufferP = new BYTE[MaxImageWidth * MaxImageHeigth * 3];

	//ԭʼ��С��Iplimage��ʽ��ͼ��
	leftImageVievP = new BYTE[MaxImageWidth * MaxImageHeigth * 3];
	viewleftimageP = cvCreateImage(CvSize(MaxImageWidth / WChouYang, MaxImageHeigth / HChouYang), 8, 3);

	//ͼ����ʾ
	CvSize czSize;
	czSize.width = 320;
	czSize.height = 240;
	frameA_showP = cvCreateImage(czSize, viewleftimageP->depth, viewleftimageP->nChannels);
}


/********************************************************
*
*���ܣ���������
*���ڣ�2017-04-25
*˵����
*
*********************************************************/
CProjCalibDlg::~CProjCalibDlg()
{

}


/********************************************************
*
*���ܣ��ؼ�--����
*���ڣ�2017-04-25
*˵����
*
*********************************************************/
void CProjCalibDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, CalibMessage);
}


/********************************************************
*
*���ܣ��ؼ�-�ۺ���
*���ڣ�2017-04-25
*˵����
*
*********************************************************/
BEGIN_MESSAGE_MAP(CProjCalibDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CProjCalibDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CProjCalibDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_ButOpenCam, &CProjCalibDlg::OnBnClickedButopencam)
	ON_BN_CLICKED(IDC_BUTTON2, &CProjCalibDlg::OnBnClickedButton2)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_CloseCam, &CProjCalibDlg::OnBnClickedClosecam)
	ON_BN_CLICKED(IDC_closeDisp, &CProjCalibDlg::OnBnClickedclosedisp)
	ON_BN_CLICKED(IDC_ImageCap, &CProjCalibDlg::OnBnClickedImagecap)
END_MESSAGE_MAP()


/********************************************************
*
*���ܣ�ͶӰ�Ǳ궨
*���ڣ�2017-04-25
*˵����
*
*********************************************************/
void CProjCalibDlg::OnBnClickedOk()
{
	//����ͶӰ������ʾ
	namedWindow("pattern_proj", WINDOW_NORMAL);
	resizeWindow("pattern_proj", 912, 1140);
	moveWindow("pattern_proj", 912 + 316, -20);
	setWindowProperty("pattern_proj", WND_PROP_FULLSCREEN, WINDOW_FULLSCREEN);

	pattern = imread("pattern.png");
	imshow("pattern_proj", pattern);

	projSettings.imageSize = Size(pattern.rows, pattern.cols);

	createObjectPoints(tempCam, camSettings.patternSize,
		camSettings.squareSize, camSettings.patternType);
	createProjectorObjectPoints(tempProj, projSettings.patternSize,
		projSettings.squareSize, projSettings.patternType);

	//��������
	Mat viewleftimage_gray = cvarrToMat(viewleftimageP);
	if (camSettings.imageSize.height == 0 || camSettings.imageSize.width == 0)
	{
		camSettings.imageSize = Size(viewleftimage_gray.rows, viewleftimage_gray.cols);
	}
}


/********************************************************
*
*���ܣ�ͼ���ɼ����궨
*���ڣ�2017-04-26
*˵����
*
*********************************************************/
void CProjCalibDlg::OnBnClickedImagecap()
{
	//ͼ����ʾ
	namedWindow("camera view", WINDOW_NORMAL);
	//ͼ��ɼ�
	static int nbrOfValidFrames = 0;
	if (nbrOfValidFrames < camSettings.nbrOfFrames)
	{
		//����ͼ��
		Mat viewleftimage_gray = cvarrToMat(viewleftimageP);	

		bool foundProj, foundCam;
		vector<Point2f> projPointBuf;
		vector<Point2f> camPointBuf;
		if (camSettings.patternType == CHESSBOARD && projSettings.patternType == CHESSBOARD)
		{
			//Ѱ�ҽǵ�
			int calibFlags = CALIB_CB_ADAPTIVE_THRESH;
			foundCam = findChessboardCorners(viewleftimage_gray, camSettings.patternSize,
				camPointBuf, calibFlags);
			foundProj = findChessboardCorners(viewleftimage_gray, projSettings.patternSize,
				projPointBuf, calibFlags);

			if (foundCam && foundProj)
			{
				Mat gray;
				cvtColor(viewleftimage_gray, gray, COLOR_BGR2GRAY);

				Mat projCorners, camCorners;
				cornerSubPix(gray, camPointBuf, camSettings.subpixelSize, Size(-1, -1),
					TermCriteria(TermCriteria::COUNT + TermCriteria::EPS, 30, 0.1));
				cornerSubPix(gray, projPointBuf, projSettings.subpixelSize, Size(-1, -1),
					TermCriteria(TermCriteria::COUNT + TermCriteria::EPS, 30, 0.1));

				drawChessboardCorners(gray, camSettings.patternSize, camPointBuf, foundCam);
				drawChessboardCorners(gray, projSettings.patternSize, projPointBuf, foundProj);
				imshow("camera view", gray);

				nbrOfValidFrames += 1;//��һ֡����
					
				imagePointsCam.push_back(camPointBuf);
				imagePointsProj.push_back(projPointBuf);
				objectPointsCam.push_back(tempCam);
				PointsInProj.push_back(tempProj);
				images.push_back(viewleftimage_gray);

				Mat ptsProjProj, ptsProjCam;
				Mat ptsProjProjN, ptsProjCamN;
				Mat TProjProj, TProjCam;
				vector<Point2f> ptsProjProjVec;
				vector<Point2f> ptsProjCamVec;

				fromVectorToMat(tempProj, ptsProjProj);
				normalize(ptsProjProj, 2, ptsProjProjN, TProjProj);
				fromMatToVector(ptsProjProjN, ptsProjProjVec);
				pointsInProjN.push_back(ptsProjProjVec);
				T2.push_back(TProjProj);
				projInProj.push_back(ptsProjProj);
				projInProjN.push_back(ptsProjProjN);

				fromVectorToMat(projPointBuf, ptsProjCam);
				normalize(ptsProjCam, 2, ptsProjCamN, TProjCam);
				fromMatToVector(ptsProjCamN, ptsProjCamVec);
				imagePointsProjN.push_back(ptsProjCamVec);
				T1.push_back(TProjCam);
				projInCam.push_back(ptsProjCam);
				projInCamN.push_back(ptsProjCamN);	

				ostringstream name;
				name << nbrOfValidFrames;
				String count_cap = "The Image Counts: " + name.str();
				CalibMessage = count_cap.c_str();
				UpdateData(FALSE);
			}
			else
			{
				imshow("camera view", viewleftimage_gray);
				CalibMessage = "Capture Failed!";
				UpdateData(FALSE);
			}
		}
	}
	else if (nbrOfValidFrames == camSettings.nbrOfFrames)
	{
		nbrOfValidFrames += 1;
		CalibMessage = "Capture Finished! Start Calib";
		UpdateData(FALSE);
	}
	else
	{
		saveCalibrationData("./_points.yml", T1, T2, projInCam, projInProj, projInCamN, projInProjN);
		double rms = calibrate(objectPointsCam, imagePointsCam, cameraMatrix, distCoeffs,
			rVecs, tVecs, camSettings.imageSize);
		//cout << "rms = " << rms << endl;
		//cout << "camera matrix = \n" << cameraMatrix << endl;
		//cout << "dist coeffs = \n" << distCoeffs << endl;

		fromCamToWorld(cameraMatrix, rVecs, tVecs, imagePointsProj, worldPointsProj);

		rms = calibrate(worldPointsProj, PointsInProj, projectorMatrix, projectorDistCoeffs,
			projectorRVecs, projectorTVecs, projSettings.imageSize);
		//cout << "rms = " << rms << endl;
		//cout << "projector matrix = \n" << projectorMatrix << endl;
		//cout << "projector dist coeffs = \n" << distCoeffs << endl;

		Mat stereoR, stereoT, essential, fundamental;
		Mat RCam, RProj, PCam, PProj, Q;
		rms = stereoCalibrate(worldPointsProj, imagePointsProj, PointsInProj, cameraMatrix, distCoeffs,
			projectorMatrix, projectorDistCoeffs, camSettings.imageSize, stereoR, stereoT,
			essential, fundamental);
		//cout << "stereo calibrate: \n" << fundamental << endl;

		saveCalibrationResults("./CalibrationResults", cameraMatrix, distCoeffs, projectorMatrix, projectorDistCoeffs, fundamental);
		
		nbrOfValidFrames = 0;
		CalibMessage = "Calib Finished!";
		UpdateData(FALSE);
	}
}


/********************************************************
*
*���ܣ��˳�ͶӰ�Ǳ궨
*���ڣ�2017-04-25
*˵����
*
*********************************************************/
void CProjCalibDlg::OnBnClickedCancel()
{
	CDialogEx::OnCancel();
}


/********************************************************
*
*���ܣ������
*���ڣ�2017-04-25
*˵����
*
*********************************************************/
void CProjCalibDlg::OnBnClickedButopencam()
{
	//�����
	leftCameraP = cam_DevP.OpenCamera(LeftCameraIndex, MaxImageWidth, MaxImageHeigth, WChouYang, HChouYang);
	
	//�������
	cam_DevP.SetBalance(leftCameraP, 1.40, 1.00, 1.52);
	cam_DevP.SeBrightness(leftCameraP, 5, 10, 50000);
	cam_DevP.CameraMode(leftCameraP);
	cam_DevP.SetOthersParameters(leftCameraP);

	//��ʼ�ɼ�
	GXRegisterCaptureCallback(leftCameraP, pImageLeftBufferP, getLeftImageV1);//ע��ص�
	GXSendCommand(leftCameraP, GX_COMMAND_ACQUISITION_START);//��ʼ�ɼ�
}


/********************************************************
*
*���ܣ�����Ļص�����
*���ڣ�2017-03-23
*˵����
*
*********************************************************/
void __stdcall CProjCalibDlg::getLeftImageV1(GX_FRAME_CALLBACK_PARAM* pFrame)
{
	CProjCalibDlg *pDlg = (CProjCalibDlg*)(pFrame->pUserParam);
	if (pFrame->status == GX_FRAME_STATUS_SUCCESS)
	{
		memcpy(leftImageBufferP, pFrame->pImgBuf, pFrame->nImgSize);
		DxRaw8toRGB24(leftImageBufferP, leftImageVievP, MaxImageWidth / WChouYang, MaxImageHeigth / HChouYang, RAW2RGB_NEIGHBOUR, BAYERGR, FALSE);
		viewleftimageP->imageData = (char*)leftImageVievP;
		cvResize(viewleftimageP, frameA_showP, CV_INTER_AREA);
	}
}


/********************************************************
*
*���ܣ���ͼƬ��ʾ���Ի�������
*���ڣ�2016-03-03
*˵����
*
*********************************************************/
void CProjCalibDlg::DrawPicToHDC(IplImage *img, UINT ID)
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
*���ܣ��������ʾ
*���ڣ�2017-04-25
*˵����
*
*********************************************************/
void CProjCalibDlg::OnBnClickedButton2()
{
	SetTimer(1, 10, NULL);
}


/********************************************************
*
*���ܣ���ʱ���ۺ���
*���ڣ�2017-04-25
*˵����
*
*********************************************************/
void CProjCalibDlg::OnTimer(UINT_PTR nIDEvent)
{
	switch (nIDEvent)
	{
	case 1:
		DrawPicToHDC(frameA_showP, IDC_STATIC_P);
		break;
	}
	CDialogEx::OnTimer(nIDEvent);
}


/********************************************************
*
*���ܣ��ر����
*���ڣ�2017-04-25
*˵����
*
*********************************************************/
void CProjCalibDlg::OnBnClickedClosecam()
{
	GXCloseDevice(leftCameraP);
}


/********************************************************
*
*���ܣ��ر���ʾ
*���ڣ�2017-04-25
*˵����
*
*********************************************************/
void CProjCalibDlg::OnBnClickedclosedisp()
{
	KillTimer(1);
}



