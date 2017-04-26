// TipCalibDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "MESR_Project.h"
#include "TipCalibDlg.h"
#include "afxdialogex.h"

Mat intrinsic;                                          //����ڲ���  
Mat distortion_coeff;                                   //����������  
vector<Mat> rvecs;                                      //��ת����  
vector<Mat> tvecs;                                      //ƽ������  
vector<vector<Point2f>> corners;                        //����ͼ���ҵ��Ľǵ�ļ��� ��objRealPoint һһ��Ӧ  
vector<vector<Point3f>> objRealPoint;                   //����ͼ��Ľǵ��ʵ���������꼯��  
vector<Point2f> corner;                                 //ĳһ��ͼ���ҵ��Ľǵ�  
Mat rgbImage, grayImage;


IMPLEMENT_DYNAMIC(CTipCalibDlg, CDialogEx)
/********************************************************
*
*���ܣ�CTipCalibDlg �Ի����캯��
*���ڣ�2017-04-08
*˵����
*
*********************************************************/
CTipCalibDlg::CTipCalibDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CTipCalibDlg::IDD, pParent)
	, CalibStatus(_T(""))
	, imageWidth(1024)
	, imageHeight(768)
	, frameNumber(15)
	, squareSize(10)
{

}

/********************************************************
*
*���ܣ���������
*���ڣ�2017-04-08
*˵����
*
*********************************************************/
CTipCalibDlg::~CTipCalibDlg()
{

}

/********************************************************
*
*���ܣ��ؼ�-����
*���ڣ�2017-04-08
*˵����
*
*********************************************************/
void CTipCalibDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_CalibMessage, CalibStatus);
	DDX_Text(pDX, IDC_EDIT2, imageWidth);
	DDX_Text(pDX, IDC_EDIT3, imageHeight);
	DDX_Text(pDX, IDC_EDIT4, frameNumber);
	DDX_Text(pDX, IDC_EDIT5, squareSize);
	DDX_Control(pDX, IDC_CHECK1, m_check1);
	DDX_Control(pDX, IDC_CHECK2, m_check2);
}


/********************************************************
*
*���ܣ��ؼ�-�ۺ���
*���ڣ�2017-04-08
*˵����
*
*********************************************************/
BEGIN_MESSAGE_MAP(CTipCalibDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CTipCalibDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CTipCalibDlg::OnBnClickedCancel)
END_MESSAGE_MAP()


/********************************************************
*
*���ܣ�����궨����ģ���ʵ����������
*���ڣ�2017-04-08
*˵����
*
*********************************************************/
void calRealPoint(vector<vector<Point3f>>& obj, int boardwidth, int boardheight, int imgNumber, int squaresize)
{
	//  Mat imgpoint(boardheight, boardwidth, CV_32FC3,Scalar(0,0,0));  
	vector<Point3f> imgpoint;
	for (int rowIndex = 0; rowIndex < boardheight; rowIndex++)
	{
		for (int colIndex = 0; colIndex < boardwidth; colIndex++)
		{
			//  imgpoint.at<Vec3f>(rowIndex, colIndex) = Vec3f(rowIndex * squaresize, colIndex*squaresize, 0);  
			imgpoint.push_back(Point3f(rowIndex * squaresize, colIndex * squaresize, 0));
		}
	}
	for (int imgIndex = 0; imgIndex < imgNumber; imgIndex++)
	{
		obj.push_back(imgpoint);
	}
}


/********************************************************
*
*���ܣ�����궨
*���ڣ�2017-04-08
*˵����
*
*********************************************************/
void CTipCalibDlg::OnBnClickedOk()
{
	const int boardWidth = 9;                               //����Ľǵ���Ŀ  
	const int boardHeight = 6;                              //����Ľǵ�����  
	const int boardCorner = boardWidth * boardHeight;       //�ܵĽǵ�����    
	const Size boardSize = Size(boardWidth, boardHeight);     

	//��ǰͼ�����
	int captured_frames = 0;
	for (captured_frames = 0; captured_frames < frameNumber; captured_frames++)
	{
		// ����ת��
		string img_name;
		char s[12];
		_itoa(captured_frames + 1, s, 10);

		img_name = s;
		if (1 == m_check1.GetCheck())
		{
			img_name = "./calibImg/cam1_gray/pattern_cam1_im" + img_name + ".png";
		}

		if (1 == m_check2.GetCheck())
		{
			img_name = "./calibImg/cam2_gray/pattern_cam2_im" + img_name + ".png";
		}
		
		rgbImage = imread(img_name, CV_LOAD_IMAGE_COLOR);
		cvtColor(rgbImage, grayImage, CV_BGR2GRAY);

		bool isFind = findChessboardCorners(rgbImage, boardSize, corner, 0);
		if (isFind == true) //���нǵ㶼���ҵ� ˵�����ͼ���ǿ��е�  
		{
			cornerSubPix(grayImage, corner, Size(5, 5), Size(-1, -1), TermCriteria(CV_TERMCRIT_EPS | CV_TERMCRIT_ITER, 20, 0.1));
			//drawChessboardCorners(rgbImage, boardSize, corner, isFind);
			//imshow("chessboard", rgbImage);
			corners.push_back(corner);
		}
		else
		{
			return;
		}
	}

	/*����ʵ�ʵ�У�������ά����*/
	calRealPoint(objRealPoint, boardWidth, boardHeight, frameNumber, squareSize);
	cout << "cal real successful" << endl;
	/*�궨����ͷ*/
	calibrateCamera(objRealPoint, corners, Size(imageWidth, imageHeight), intrinsic, distortion_coeff, rvecs, tvecs, 0);
	
	/*���沢�������*/
	String filename;
	if (1 == m_check1.GetCheck())
	{
		filename = "calibCam1.yml";
	}

	if (1 == m_check2.GetCheck())
	{
		filename = "calibCam2.yml";
	}
	cv::FileStorage fs(filename, FileStorage::WRITE);
	fs << "intrinsic" << intrinsic;
	fs << "distortion_coeff" << distortion_coeff;
	fs.release();

	CalibStatus = "Calibation Finished!";
	UpdateData(FALSE);
}


/********************************************************
*
*���ܣ��˳�����
*���ڣ�2017-04-08
*˵����
*
*********************************************************/
void CTipCalibDlg::OnBnClickedCancel()
{
	CDialogEx::OnCancel();
}
