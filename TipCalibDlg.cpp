// TipCalibDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "MESR_Project.h"
#include "TipCalibDlg.h"
#include "afxdialogex.h"

Mat intrinsic;                                          //相机内参数  
Mat distortion_coeff;                                   //相机畸变参数  
vector<Mat> rvecs;                                      //旋转向量  
vector<Mat> tvecs;                                      //平移向量  
vector<vector<Point2f>> corners;                        //各个图像找到的角点的集合 和objRealPoint 一一对应  
vector<vector<Point3f>> objRealPoint;                   //各副图像的角点的实际物理坐标集合  
vector<Point2f> corner;                                 //某一副图像找到的角点  
Mat rgbImage, grayImage;


IMPLEMENT_DYNAMIC(CTipCalibDlg, CDialogEx)
/********************************************************
*
*功能：CTipCalibDlg 对话框构造函数
*日期：2017-04-08
*说明：
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
*功能：析构函数
*日期：2017-04-08
*说明：
*
*********************************************************/
CTipCalibDlg::~CTipCalibDlg()
{

}

/********************************************************
*
*功能：控件-变量
*日期：2017-04-08
*说明：
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
*功能：控件-槽函数
*日期：2017-04-08
*说明：
*
*********************************************************/
BEGIN_MESSAGE_MAP(CTipCalibDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CTipCalibDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CTipCalibDlg::OnBnClickedCancel)
END_MESSAGE_MAP()


/********************************************************
*
*功能：计算标定板上模块的实际物理坐标
*日期：2017-04-08
*说明：
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
*功能：相机标定
*日期：2017-04-08
*说明：
*
*********************************************************/
void CTipCalibDlg::OnBnClickedOk()
{
	const int boardWidth = 9;                               //横向的角点数目  
	const int boardHeight = 6;                              //纵向的角点数据  
	const int boardCorner = boardWidth * boardHeight;       //总的角点数据    
	const Size boardSize = Size(boardWidth, boardHeight);     

	//当前图像序号
	int captured_frames = 0;
	for (captured_frames = 0; captured_frames < frameNumber; captured_frames++)
	{
		// 类型转换
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
		if (isFind == true) //所有角点都被找到 说明这幅图像是可行的  
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

	/*计算实际的校正点的三维坐标*/
	calRealPoint(objRealPoint, boardWidth, boardHeight, frameNumber, squareSize);
	cout << "cal real successful" << endl;
	/*标定摄像头*/
	calibrateCamera(objRealPoint, corners, Size(imageWidth, imageHeight), intrinsic, distortion_coeff, rvecs, tvecs, 0);
	
	/*保存并输出参数*/
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
*功能：退出程序
*日期：2017-04-08
*说明：
*
*********************************************************/
void CTipCalibDlg::OnBnClickedCancel()
{
	CDialogEx::OnCancel();
}
