#include "stdafx.h"
#include "MESR_Project.h"
#include "MESR_ProjectDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/*—----------------------全局变量--------------------------*/
BYTE *leftImageBuffer;
BYTE *rightImageBuffer;

BYTE* pImageLeftBuffer;
BYTE* pImageRightBuffer;
BYTE *leftImageViev;
BYTE *rightImageViev;

IplImage* viewleftimage;
IplImage* viewrightimage;
IplImage* frameA_show;
IplImage* frameB_show;


//用于应用程序“关于”菜单项的 CAboutDlg 对话框
class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

	//对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
		virtual void DoDataExchange(CDataExchange* pDX);//DDX/DDV 支持

//实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{

}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


/********************************************************
*
*功能：CMESR_ProjectDlg 对话框构造函数
*日期：2017-03-23
*说明：
*
*********************************************************/
CMESR_ProjectDlg::CMESR_ProjectDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CMESR_ProjectDlg::IDD, pParent)
	, CamCounts(0)
	, Action_Satus(_T(""))
	, Project_Width(912)
	, Project_Height(1140)
	, Phase_Status(_T(""))
	, Phase_Height(1140)
	, Phase_WeighT(912)
	, Phase_Period(32)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	//初始化参数
	leftImageBuffer = new BYTE[MaxImageWidth * MaxImageHeigth * 3];
	rightImageBuffer = new BYTE[MaxImageWidth * MaxImageHeigth * 3];
	pImageLeftBuffer = new BYTE[MaxImageWidth * MaxImageHeigth * 3];
	pImageRightBuffer = new BYTE[MaxImageWidth * MaxImageHeigth * 3];

	//原始大小的Iplimage格式的图像
	leftImageViev = new BYTE[MaxImageWidth * MaxImageHeigth * 3];
	rightImageViev = new BYTE[MaxImageWidth * MaxImageHeigth * 3];
	viewleftimage = cvCreateImage(CvSize(MaxImageWidth / WChouYang, MaxImageHeigth / HChouYang), 8, 3);
	viewrightimage = cvCreateImage(CvSize(MaxImageWidth / WChouYang, MaxImageHeigth / HChouYang), 8, 3);
	
	//图像显示
	CvSize czSize;
	czSize.width = 320;
	czSize.height = 240;
	frameA_show = cvCreateImage(czSize, viewleftimage->depth, viewleftimage->nChannels);
	frameB_show = cvCreateImage(czSize, viewrightimage->depth, viewrightimage->nChannels);

	//相机参数
	cam1intrinsics = (Mat_<double>(3, 3) << 3810.7126, 0, 1384.0708,
											0, 3784.5732, 955.8291,
											0, 0, 1);
	cam1distCoeffs = (Mat_<double>(5, 1) << -0.1248, -0.0433, 0.0021, 0.0047, -0.0047);

	cam2intrinsics = (Mat_<double>(3, 3) << 3772.4958, 0, 1259.7412,
											0, 3739.4814, 981.5157,
											0, 0, 1);
	cam2distCoeffs = (Mat_<double>(5, 1) << -0.1684, 0.8806, 0.0019, -0.0024, -4.5638);

	R = (Mat_<double>(3, 3) << 0.9456, -0.0096, 0.3253,
							   0.0120, 0.9999, -0.0053,
							  -0.3252, 0.0090, 0.9456);
	T = (Mat_<double>(3, 1) << 98.4069, 0.1741, 18.9018);
}


/********************************************************
*
*功能：标准析构函数
*日期：2017-03-23
*说明：
*
*********************************************************/
CMESR_ProjectDlg::~CMESR_ProjectDlg()
{
	GXCloseLib();
}


/********************************************************
*
*功能：控件的数据交互
*日期：2017-03-23
*说明：
*
*********************************************************/
void CMESR_ProjectDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, CamCounts);
	DDX_Text(pDX, IDC_EDIT2, Action_Satus);
	DDX_Text(pDX, IDC_EDIT3, Project_Width);
	DDX_Text(pDX, IDC_EDIT4, Project_Height);
	DDX_Control(pDX, IDC_COMBO2, Phase_ID);
	DDX_Text(pDX, IDC_EDIT7, Phase_Status);
	DDX_Text(pDX, IDC_EDIT6, Phase_Height);
	DDX_Text(pDX, IDC_EDIT5, Phase_WeighT);
	DDX_Text(pDX, IDC_EDIT8, Phase_Period);
	DDX_Control(pDX, IDC_COMBO3, Phase_Maker);
	DDX_Control(pDX, IDC_COMBO4, Phase_Direction);
	DDX_Control(pDX, IDC_PhaseDire, DecoderDir);
}


/********************************************************
*
*功能：按钮槽函数
*日期：2017-03-23
*说明：
*
*********************************************************/
BEGIN_MESSAGE_MAP(CMESR_ProjectDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(OpenCam, &CMESR_ProjectDlg::OnBnClickedOpencam)
	ON_BN_CLICKED(IDC_CheckCam, &CMESR_ProjectDlg::OnBnClickedCheckcam)
	ON_BN_CLICKED(IDC_SnapStart, &CMESR_ProjectDlg::OnBnClickedSnapstart)
	ON_BN_CLICKED(IDC_ImgShow, &CMESR_ProjectDlg::OnBnClickedImgshow)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_StopShow, &CMESR_ProjectDlg::OnBnClickedStopshow)
	ON_BN_CLICKED(IDC_CloseCam, &CMESR_ProjectDlg::OnBnClickedClosecam)
	ON_BN_CLICKED(IDC_StopSnap, &CMESR_ProjectDlg::OnBnClickedStopsnap)
	ON_BN_CLICKED(Gererate_Pattern, &CMESR_ProjectDlg::OnBnClickedPattern)
	ON_BN_CLICKED(IDC_PlayImage, &CMESR_ProjectDlg::OnBnClickedPlayimage)
	ON_BN_CLICKED(IDC_AutoPlay, &CMESR_ProjectDlg::OnBnClickedAutoplay)
	ON_BN_CLICKED(IDC_SaveImg, &CMESR_ProjectDlg::OnBnClickedSaveimg)
	ON_BN_CLICKED(IDC_PhasePattern, &CMESR_ProjectDlg::OnBnClickedPhasepattern)
	ON_BN_CLICKED(IDC_SinPlay, &CMESR_ProjectDlg::OnBnClickedSinplay)
	ON_BN_CLICKED(IDC_BUTTON4, &CMESR_ProjectDlg::OnBnClickedButton4)
	ON_BN_CLICKED(IDC_ButDecoder, &CMESR_ProjectDlg::OnBnClickedButdecoder)
	ON_BN_CLICKED(IDC_RectBut, &CMESR_ProjectDlg::OnBnClickedRectbut)
	ON_BN_CLICKED(IDC_DecodePhase, &CMESR_ProjectDlg::OnBnClickedDecodephase)
	ON_BN_CLICKED(IDC_But3DR_Rectfy, &CMESR_ProjectDlg::OnBnClickedBut3drRectfy)
	ON_BN_CLICKED(IDC_ButSeam, &CMESR_ProjectDlg::OnBnClickedButseam)
	ON_BN_CLICKED(IDC_CamCalib, &CMESR_ProjectDlg::OnBnClickedCamcalib)
	ON_BN_CLICKED(IDC_ButExit, &CMESR_ProjectDlg::OnBnClickedButexit)
	ON_BN_CLICKED(IDC_ButProjCalib, &CMESR_ProjectDlg::OnBnClickedButprojcalib)
	ON_BN_CLICKED(IDC_ButStereo_3D, &CMESR_ProjectDlg::OnBnClickedButstereo3d)
END_MESSAGE_MAP()


/********************************************************
*
*功能：CMESR_ProjectDlg 消息处理程序
*日期：2017-03-23
*说明：
*	(1)初始化
*
*********************************************************/
BOOL CMESR_ProjectDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO:  在此添加额外的初始化代码
	Phase_ID.AddString(_T("FTP"));
	Phase_ID.AddString(_T("PSP"));
	Phase_ID.AddString(_T("FASP"));
	Phase_ID.SetCurSel(0);

	Phase_Maker.AddString(_T("Maker"));
	Phase_Maker.AddString(_T("No Maker"));
	Phase_Maker.SetCurSel(0);

	Phase_Direction.AddString(_T("Horizontal"));
	Phase_Direction.AddString(_T("Vertical"));
	Phase_Direction.SetCurSel(0);

	DecoderDir.AddString(_T("left"));
	DecoderDir.AddString(_T("right"));
	DecoderDir.SetCurSel(0);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}


void CMESR_ProjectDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}


//  如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。
void CMESR_ProjectDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}


//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CMESR_ProjectDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


/********************************************************
*
*功能：打开相机
*日期：2017-03-23
*说明：
*     左相机-FrameA-1 右相机-FrameB-2
*
*********************************************************/
void CMESR_ProjectDlg::OnBnClickedOpencam()
{
	//打开相机
	leftCamera = cam_Dev.OpenCamera(LeftCameraIndex, MaxImageWidth, MaxImageHeigth, WChouYang, HChouYang);	
	//设置相机
	cam_Dev.SetBalance(leftCamera, 1.40, 1.00, 1.52);
	cam_Dev.SeBrightness(leftCamera, 5, 10, 50000);
	cam_Dev.CameraMode(leftCamera);
	cam_Dev.SetOthersParameters(leftCamera);

	rightCamera = cam_Dev.OpenCamera(RightCameraIndex, MaxImageWidth, MaxImageHeigth, WChouYang, HChouYang);
	//设置相机
	cam_Dev.SetBalance(rightCamera, 1.40, 1.00, 1.52);
	cam_Dev.SeBrightness(rightCamera, 5, 10, 50000);
	cam_Dev.CameraMode(rightCamera);
	cam_Dev.SetOthersParameters(rightCamera);
	 
	Action_Satus = "Camera Open！";
	UpdateData(FALSE);
}


/********************************************************
*
*功能：获取设备数目
*日期：2017-03-23
*说明：
*
*********************************************************/
void CMESR_ProjectDlg::OnBnClickedCheckcam()
{
	CamCounts = cam_Dev.GetCamera_Counts();
	Action_Satus = "Check OK！";
	UpdateData(FALSE);
}


/********************************************************
*
*功能：开始采集
*日期：2017-03-23
*说明：
*
*********************************************************/
void CMESR_ProjectDlg::OnBnClickedSnapstart()
{
	GXRegisterCaptureCallback(leftCamera, pImageLeftBuffer, getLeftImage);//注册回调
	GXSendCommand(leftCamera, GX_COMMAND_ACQUISITION_START);//开始采集
	GXRegisterCaptureCallback(rightCamera, pImageRightBuffer, getRightImage);//注册回调
	GXSendCommand(rightCamera, GX_COMMAND_ACQUISITION_START);//开始采集

	Action_Satus = "Start Snap！";
	UpdateData(FALSE);
}


/********************************************************
*
*功能：右相机的回调函数
*日期：2017-03-23
*说明：
*
*********************************************************/
void __stdcall CMESR_ProjectDlg::getRightImage(GX_FRAME_CALLBACK_PARAM* pFrame)
{
	CMESR_ProjectDlg *pDlg = (CMESR_ProjectDlg*)(pFrame->pUserParam);
	if (pFrame->status == GX_FRAME_STATUS_SUCCESS)
	{
		memcpy(rightImageBuffer, pFrame->pImgBuf, pFrame->nImgSize);
		DxRaw8toRGB24(rightImageBuffer, rightImageViev, MaxImageWidth / WChouYang, MaxImageHeigth / HChouYang, RAW2RGB_NEIGHBOUR, BAYERGR, FALSE);
		viewrightimage->imageData = (char*)rightImageViev;
		cvResize(viewrightimage, frameB_show, CV_INTER_AREA);
	}
}


/********************************************************
*
*功能：左相机的回调函数
*日期：2017-03-23
*说明：
*
*********************************************************/
void __stdcall CMESR_ProjectDlg::getLeftImage(GX_FRAME_CALLBACK_PARAM* pFrame)
{
	CMESR_ProjectDlg *pDlg = (CMESR_ProjectDlg*)(pFrame->pUserParam);
	if (pFrame->status == GX_FRAME_STATUS_SUCCESS)
	{
		memcpy(leftImageBuffer, pFrame->pImgBuf, pFrame->nImgSize);
		DxRaw8toRGB24(leftImageBuffer, leftImageViev, MaxImageWidth / WChouYang, MaxImageHeigth / HChouYang, RAW2RGB_NEIGHBOUR, BAYERGR, FALSE);
		viewleftimage->imageData = (char*)leftImageViev;
		cvResize(viewleftimage, frameA_show, CV_INTER_AREA);
	}
}


/********************************************************
*
*功能：将图片显示到对话框里面
*日期：2016-03-03
*说明：
*
*********************************************************/
void CMESR_ProjectDlg::DrawPicToHDC(IplImage *img, UINT ID)
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
*功能：将图片显示到对话框里面
*日期：2017-03-23
*说明：
*
*********************************************************/
void CMESR_ProjectDlg::OnBnClickedImgshow()
{
	// 设置计时器
	SetTimer(1, 10, NULL);
	SetTimer(2, 15, NULL);
}


/********************************************************
*
*功能：定时器槽函数
*日期：2017-03-23
*说明：
*	(1)由于静态回调函数无法显示的bug,在这里通过定时器实现显示
*
*********************************************************/
void CMESR_ProjectDlg::OnTimer(UINT_PTR nIDEvent)
{
	switch (nIDEvent)
	{
	case 1:
		DrawPicToHDC(frameA_show, IDC_STATICB);
		break;
	case 2:
		DrawPicToHDC(frameB_show, IDC_STATICa);
		break;
	case 3:
		if (frame_count < (int)pattern.size())
		{
			imshow("Pattern Window", pattern[frame_count]);
			cvWaitKey(1000);

			ostringstream name;
			name << frame_count + 1;

			Mat viewleftimage_gray = cvarrToMat(viewleftimage);
			cvtColor(viewleftimage_gray, viewleftimage_gray, COLOR_RGB2GRAY);
			//图像缩放
			resize(viewleftimage_gray, viewleftimage_gray, Size(1024, 768), 0, 0, CV_INTER_LINEAR);
			imwrite("./Grayimgdata/pattern_cam1_im" + name.str() + ".png", viewleftimage_gray);

			//图像保存
			Mat viewrightimage_gray = cvarrToMat(viewrightimage);
			cvtColor(viewrightimage_gray, viewrightimage_gray, COLOR_RGB2GRAY);
			//图像缩放
			resize(viewrightimage_gray, viewrightimage_gray, Size(1024, 768), 0, 0, CV_INTER_LINEAR);
			imwrite("./Grayimgdata/pattern_cam2_im" + name.str() + ".png", viewrightimage_gray);
		
			frame_count++;
		}
		else
		{
			KillTimer(3);
			frame_count = 0;
		}
		break;
	case 4:
		if (nbrOfImages < 30)
		{
			imshow("pattern_sin", patterns_sin[nbrOfImages%3]);
			cvWaitKey(2000);

			ostringstream name;
			name << nbrOfImages + 1;

			//图像保存
			Mat viewleftimage_gray = cvarrToMat(viewleftimage);
			cvtColor(viewleftimage_gray, viewleftimage_gray, COLOR_RGB2GRAY);
			resize(viewleftimage_gray, viewleftimage_gray, Size(1024, 768), 0, 0, CV_INTER_LINEAR);
			imwrite("./PhaseImg/phaseLeft_im" + name.str() + ".png", viewleftimage_gray);

			Mat viewrightimage_gray = cvarrToMat(viewrightimage);
			cvtColor(viewrightimage_gray, viewrightimage_gray, COLOR_RGB2GRAY);
			resize(viewrightimage_gray, viewrightimage_gray, Size(1024, 768), 0, 0, CV_INTER_LINEAR);
			imwrite("./PhaseImg/phaseRight_im" + name.str() + ".png", viewrightimage_gray);	

			nbrOfImages++;
		}
		else
		{
			KillTimer(4);
			nbrOfImages = 0;
		}
		break;
	}
	CDialogEx::OnTimer(nIDEvent);
}


/********************************************************
*
*功能：停止显示
*日期：2017-03-23
*说明：
*
*********************************************************/
void CMESR_ProjectDlg::OnBnClickedStopshow()
{
	KillTimer(1);
	KillTimer(2);

	Action_Satus = "Close Display！";
	UpdateData(FALSE);
}


/********************************************************
*
*功能：关闭相机
*日期：2017-03-23
*说明：
*
*********************************************************/
void CMESR_ProjectDlg::OnBnClickedClosecam()
{
	GXCloseDevice(leftCamera);
	GXCloseDevice(rightCamera);

	Action_Satus = "Close Camera！";
	UpdateData(FALSE);
}


/********************************************************
*
*功能：停止采集
*日期：2017-03-23
*说明：
*
*********************************************************/
void CMESR_ProjectDlg::OnBnClickedStopsnap()
{
	GXSendCommand(leftCamera, GX_COMMAND_ACQUISITION_STOP);//发送停采命令
	GXUnregisterCaptureCallback(leftCamera);//注销采集回调	
	GXSendCommand(rightCamera, GX_COMMAND_ACQUISITION_STOP);//发送停采命令
	GXUnregisterCaptureCallback(rightCamera);//注销采集回调

	Action_Satus = "Close Snap！";
	UpdateData(FALSE);
}


/********************************************************
*
*功能：格雷码：产生格雷码图案
*日期：2017-03-23
*说明：
*
*********************************************************/
void CMESR_ProjectDlg::OnBnClickedPattern(void)
{
	// 将各控件中的数据保存到相应的变量   
	UpdateData(TRUE);

	params.width = Project_Width;
	params.height = Project_Height;

	graycode = structured_light::GrayCodePattern::create(params);
	graycode->generate(pattern);

	//shadows mask computation
	Mat white;
	Mat black;
	graycode->getImagesForShadowMasks(black, white);
	pattern.push_back(white);
	pattern.push_back(black);

	//设置投影仪窗口显示
	namedWindow("Pattern Window", WINDOW_NORMAL);
	resizeWindow("Pattern Window", params.width, params.height);
	moveWindow("Pattern Window", params.width + 316, -20);
	setWindowProperty("Pattern Window", WND_PROP_FULLSCREEN, WINDOW_FULLSCREEN);
}


/********************************************************
*
*功能：格雷码：格雷码图案序列手动播放
*日期：2017-03-23
*说明：
*
*********************************************************/
void CMESR_ProjectDlg::OnBnClickedPlayimage(void)
{
	static unsigned int count = 0;
	if (count < (int)pattern.size())
	{
		imshow("Pattern Window", pattern[count]);
		cvWaitKey(20);
	}
	else
	{
		count = 0;
	}
	count++;
}


/********************************************************
*
*功能：格雷码：格雷码图案序列自动播放
*日期：2017-03-23
*说明：
*
*********************************************************/
void CMESR_ProjectDlg::OnBnClickedAutoplay()
{
	SetTimer(3, 3500, NULL);
}


/********************************************************
*
*功能：格雷码：图像保存
*日期：2017-03-23
*说明：
*
*********************************************************/
void CMESR_ProjectDlg::OnBnClickedSaveimg()
{
	static unsigned int i = 0;
	ostringstream name;

	name << i + 1;

	//图像保存
	Mat viewrightimage_gray = cvarrToMat(viewrightimage);
	resize(viewrightimage_gray, viewrightimage_gray, Size(1024, 768), 0, 0, CV_INTER_LINEAR);
	//cvtColor(viewrightimage_gray, viewrightimage_gray, COLOR_RGB2GRAY);
	imwrite("./Rawimgdata/Raw_cam2_im" + name.str() + ".png", viewrightimage_gray);

	Mat viewleftimage_gray = cvarrToMat(viewleftimage);
	resize(viewleftimage_gray, viewleftimage_gray, Size(1024, 768), 0, 0, CV_INTER_LINEAR);
	//cvtColor(viewleftimage_gray, viewleftimage_gray, COLOR_RGB2GRAY);
	imwrite("./Rawimgdata/Raw_cam1_im" + name.str() + ".png", viewleftimage_gray);

	i++;
}


/********************************************************
*
*功能：相移法：产生图案
*日期：2017-03-26
*说明：
*
*********************************************************/
void CMESR_ProjectDlg::OnBnClickedPhasepattern()
{
	UpdateData(TRUE);

	//相移参数的初始化
	int nSel;
	CString strWeb;

	//条纹方向
	nSel = Phase_Direction.GetCurSel();  	
	Phase_Direction.GetLBText(nSel, strWeb);
	bool DirectionPattrn;
	if (strWeb == "Vertical")
	{
		DirectionPattrn = false;
	}
	else if (strWeb == "Horizontal")
	{
		DirectionPattrn = true;
	}

	//Maker
	nSel = Phase_Maker.GetCurSel();
	Phase_Maker.GetLBText(nSel, strWeb);
	bool MakerPattern;
	if (strWeb == "Maker")
	{
		MakerPattern = true;
	}
	else if (strWeb == "No Maker")
	{
		MakerPattern = false;
	}

	//解码方法
	nSel = Phase_ID.GetCurSel();
	Phase_ID.GetLBText(nSel, strWeb);
	if (strWeb == "FTP")
	{
		params_sin.methodId = structured_light::FTP;
	}
	else if (strWeb == "PSP")
	{
		params_sin.methodId = structured_light::PSP;
	}
	else if (strWeb == "FASP")
	{
		params_sin.methodId = structured_light::FAPS;
	}
	
	//参数初始化
	params_sin.width = Phase_WeighT;
	params_sin.height = Phase_Height;
	params_sin.nbrOfPeriods = Phase_Period;
	params_sin.setMarkers = MakerPattern;
	params_sin.horizontal = DirectionPattrn;
	params_sin.shiftValue = static_cast<float>(2 * CV_PI / 3);
	params_sin.nbrOfPixelsBetweenMarkers = 70;

	//产生正弦条纹图案
	sinus = structured_light::SinusoidalPattern::create(params_sin);
	sinus->generate(patterns_sin);

	//设置投影窗口显示
	namedWindow("pattern_sin", WINDOW_NORMAL);
	setWindowProperty("pattern_sin", WND_PROP_FULLSCREEN, WINDOW_FULLSCREEN);
	Phase_Status = "条纹生成！";
	UpdateData(FALSE);
}


/********************************************************
*
*功能：相移法：图案手动播放
*日期：2017-03-26
*说明：
*
*********************************************************/
void CMESR_ProjectDlg::OnBnClickedSinplay()
{
	static int count = 0;
	ostringstream name;

	name << count + 1;

	if (count < 30)
	{
		imshow("pattern_sin", patterns_sin[count%3]);
		cvWaitKey(2000);

		//图像保存
		Mat viewleftimage_gray = cvarrToMat(viewleftimage);
		cvtColor(viewleftimage_gray, viewleftimage_gray, COLOR_RGB2GRAY);
		resize(viewleftimage_gray, viewleftimage_gray, Size(1024, 768), 0, 0, CV_INTER_LINEAR);
		imwrite("./PhaseImg/phaseLeft_im" + name.str() + ".png", viewleftimage_gray);

		Mat viewrightimage_gray = cvarrToMat(viewrightimage);
		cvtColor(viewrightimage_gray, viewrightimage_gray, COLOR_RGB2GRAY);
		resize(viewrightimage_gray, viewrightimage_gray, Size(1024, 768), 0, 0, CV_INTER_LINEAR);
		imwrite("./PhaseImg/phaseRight_im" + name.str() + ".png", viewrightimage_gray);
	}
	else
	{
		count = 0;
		Phase_Status = "Collection Finished！";
		UpdateData(FALSE);
	}
	count++;
}


/********************************************************
*
*功能：解相
*日期：2017-04-05
*说明：
*    (1)代码的执行效率很低！
*    (2)解码的效果也不是很好！
*
*********************************************************/
void CMESR_ProjectDlg::OnBnClickedDecodephase()
{
	int nSel;
	CString strWeb;
	//解码图案
	nSel = DecoderDir.GetCurSel();
	DecoderDir.GetLBText(nSel, strWeb);
	bool DirectionPattrn;
	if (strWeb == "left")
	{
		DirectionPattrn = false;
	}
	else if (strWeb == "right")
	{
		DirectionPattrn = true;
	}

	//读入图像序列
	string ImgName;
	vector<string> imagelist;
	if (DirectionPattrn == false)
	{
		for (unsigned int i = 0; i < 30; i++)
		{
			//类型转换
			string img_name;
			char s[12];
			_itoa(i + 1, s, 10);
			img_name = s;
			img_name = "./PhaseImg/phaseLeft_im" + img_name + ".png";

			imagelist.push_back(img_name);
		}
	}
	else
	{
		for (unsigned int i = 0; i < 30; i++)
		{
			//类型转换
			string img_name;
			char s[12];
			_itoa(i + 1, s, 10);
			img_name = s;
			img_name = "./PhaseImg/phaseRight_im" + img_name + ".png";

			imagelist.push_back(img_name);
		}
	}

	// 导入图像
	vector<Mat> img(30);
	Size camSize(-1, -1);
	for (size_t i = 0; i < 30; i++)
		img[i] = imread(imagelist[i], IMREAD_GRAYSCALE);


	//解相
	switch (params_sin.methodId)
	{
	case structured_light::FTP:
		for (int i = 0; i < 30; ++i)
		{
			vector<Mat> captures;
			if (i == nbrOfImages - 2)
			{
				captures.push_back(img[i]);
				captures.push_back(img[i - 1]);
				captures.push_back(img[i + 1]);
			}
			else if (i == nbrOfImages - 1)
			{
				captures.push_back(img[i]);
				captures.push_back(img[i - 1]);
				captures.push_back(img[i - 2]);
			}
			else
			{
				captures.push_back(img[i]);
				captures.push_back(img[i + 1]);
				captures.push_back(img[i + 2]);
			}
			sinus->computePhaseMap(captures, wrappedPhaseMap, shadowMask);

			if (camSize.height == -1)
			{
				//参数初始化
				camSize.height = img[0].rows;
				camSize.width = img[0].cols;
				paramsUnwrapping.height = camSize.height;
				paramsUnwrapping.width = camSize.width;

				phaseUnwrapping =
					phase_unwrapping::HistogramPhaseUnwrapping::create(paramsUnwrapping);
			}
			sinus->unwrapPhaseMap(wrappedPhaseMap, unwrappedPhaseMap, camSize, shadowMask);
			phaseUnwrapping->unwrapPhaseMap(wrappedPhaseMap, unwrappedPhaseMap, shadowMask);
			phaseUnwrapping->getInverseReliabilityMap(reliabilities);

			reliabilities.convertTo(reliabilities8, CV_8U, 255, 128);
			unwrappedPhaseMap.convertTo(unwrappedPhaseMap8, CV_8U, 1, 128);
			wrappedPhaseMap.convertTo(wrappedPhaseMap8, CV_8U, 255, 128);

			ostringstream name;
			name << i;

			imwrite("./phaseResults/FTP_reliabilities8_" + name.str()+ ".png", reliabilities8);
			imwrite("./phaseResults/FTP_unwrappedPhaseMap8_" + name.str() + ".png", unwrappedPhaseMap8);
			imwrite("./phaseResults/FTP_wrappedPhaseMap8_" + name.str() + ".png", wrappedPhaseMap8);
		}
		break;

	case structured_light::PSP:
	case structured_light::FAPS:	
		for (int i = 0; i < 30 - 2; ++i)
		{
			captures.push_back(img[i]);
			captures.push_back(img[i + 1]);
			captures.push_back(img[i + 2]);

			sinus->computePhaseMap(captures, wrappedPhaseMap, shadowMask);

			if (camSize.height == -1)
			{
				camSize.height = img[0].rows;
				camSize.width = img[0].cols;
				paramsUnwrapping.height = camSize.height;
				paramsUnwrapping.width = camSize.width;
				phaseUnwrapping =
					phase_unwrapping::HistogramPhaseUnwrapping::create(paramsUnwrapping);
			}
			sinus->unwrapPhaseMap(wrappedPhaseMap, unwrappedPhaseMap, camSize, shadowMask);
			unwrappedPhaseMap.convertTo(unwrappedPhaseMap8, CV_8U, 1, 128);
			wrappedPhaseMap.convertTo(wrappedPhaseMap8, CV_8U, 255, 128);
			phaseUnwrapping->unwrapPhaseMap(wrappedPhaseMap, unwrappedPhaseMap, shadowMask);

			Mat reliabilities, reliabilities8;
			phaseUnwrapping->getInverseReliabilityMap(reliabilities);
			reliabilities.convertTo(reliabilities8, CV_8U, 255, 128);

			ostringstream name;
			name << i;

			if (params_sin.methodId == structured_light::PSP)
				imwrite("./phaseResults/PSP_reliabilities8_" + name.str() + ".png", reliabilities8);
			else
				imwrite("./phaseResults/FAPS_reliabilities8_" + name.str() + ".png", reliabilities8);

			if (params_sin.methodId == structured_light::PSP)
				imwrite("./phaseResults/PSP_unwrappedPhaseMap8_" + name.str() + ".png", unwrappedPhaseMap8);
			else
				imwrite("./phaseResults/FASP_unwrappedPhaseMap8_" + name.str() + ".png", unwrappedPhaseMap8);

			if (params_sin.methodId == structured_light::PSP)
				imwrite("./phaseResults/PSP_wrappedPhaseMap8_" + name.str() + ".png", wrappedPhaseMap8);
			else
				imwrite("./phaseResults/FASP_wrappedPhaseMap8_" + name.str() + ".png", wrappedPhaseMap8);
		}
		break;
	}
	Phase_Status = "Decoder OK！";
	UpdateData(FALSE);
}



/********************************************************
*
*功能：相移法：图案自动播放
*日期：2017-03-26
*说明：
*
*********************************************************/
void CMESR_ProjectDlg::OnBnClickedButton4()
{
	SetTimer(4, 4500, NULL);
}


/********************************************************
*
*功能：三维点云的保存
*日期：2017-04-05
*说明：
*
*********************************************************/
void CMESR_ProjectDlg::saveXYZ(const char* filename, const Mat& mat)
{
	const double max_z = 1.0e4;
	FILE* fp = fopen(filename, "wt");
	for (int y = 0; y < mat.rows; y++)
	{
		for (int x = 0; x < mat.cols; x++)
		{
			Vec3f point = mat.at<Vec3f>(y, x);
			if (fabs(point[2] - max_z) < FLT_EPSILON || fabs(point[2]) > max_z) continue;
			fprintf(fp, "%f %f %f\n", point[0], point[1], point[2]);
		}
	}
	fclose(fp);
}


/********************************************************
*
*功能：格雷码：3DR(校准之前的图像)
*日期：2017-03-28
*说明：
*
*********************************************************/
void CMESR_ProjectDlg::OnBnClickedButdecoder()
{
	// 读入图像序列
	string ImgName;
	vector<string> imagelist;
	for (unsigned int i = 0; i < 88; i++)
	{
		// 类型转换
		string img_name;		
		char s[12];
		
		if (i < 44)
		{
			_itoa(i + 1, s, 10);
			img_name = s;
			img_name = "./imgdata/pattern_cam1_im" + img_name + ".png";
		}
		else
		{
			_itoa((i + 1)-44, s, 10);
			img_name = s;
			img_name = "./imgdata/pattern_cam2_im" + img_name + ".png";
		}
		imagelist.push_back(img_name);
	}
	
	structured_light::GrayCodePattern::Params params_Decode;
	params_Decode.width = Project_Width;
	params_Decode.height = Project_Height;
	Ptr<structured_light::GrayCodePattern> graycode_Decoder = structured_light::GrayCodePattern::create(params_Decode);

	// 相机标定参数
	if ((!R.data) || (!T.data) || (!cam1intrinsics.data) || (!cam2intrinsics.data) || (!cam1distCoeffs.data) || (!cam2distCoeffs.data))
	{
		return;
	}
	
	size_t numberOfPatternImages = graycode_Decoder->getNumberOfPatternImages();//42
	vector<vector<Mat>> captured_pattern;
	captured_pattern.resize(2);
	captured_pattern[0].resize(numberOfPatternImages);
	captured_pattern[1].resize(numberOfPatternImages);

	// 全白图像，白色图案编码
	Mat color = imread(imagelist[numberOfPatternImages], IMREAD_COLOR);
	// 图像重映射之用
	Size imagesSize = color.size();

 	// Stereo rectify
	cout << "Rectifying images..." << endl;
	Mat R1, R2, P1, P2, Q;
	Rect validRoi[2];
	stereoRectify(cam1intrinsics, cam1distCoeffs, cam2intrinsics, cam2distCoeffs, imagesSize, R, T, R1, R2, P1, P2, Q, 0,
		-1, imagesSize, &validRoi[0], &validRoi[1]);

	// 导入图像
	for (size_t i = 0; i < numberOfPatternImages; i++)
	{
		//0-41
		captured_pattern[0][i] = imread(imagelist[i], IMREAD_GRAYSCALE);
		//44-87
		captured_pattern[1][i] = imread(imagelist[i + numberOfPatternImages + 2], IMREAD_GRAYSCALE);
		if ((!captured_pattern[0][i].data) || (!captured_pattern[1][i].data))
		{
			return;
		}
	}

	// 导入全白全黑背景图案
	vector<Mat> blackImages;
	vector<Mat> whiteImages;
	blackImages.resize(2);
	whiteImages.resize(2);
	whiteImages[0] = imread(imagelist[numberOfPatternImages], IMREAD_GRAYSCALE);//42	
	blackImages[0] = imread(imagelist[numberOfPatternImages + 1], IMREAD_GRAYSCALE);//43
	whiteImages[1] = imread(imagelist[2 * numberOfPatternImages + 2], IMREAD_GRAYSCALE);//86
	blackImages[1] = imread(imagelist[2 * numberOfPatternImages + 2 + 1], IMREAD_GRAYSCALE);//87

	// 解码
	Mat disparityMap;
	bool decoded = graycode_Decoder->decode(captured_pattern, disparityMap, blackImages, whiteImages, structured_light::DECODE_3D_UNDERWORLD);

	if (decoded)
	{
		// 彩色视差图案
		double min;
		double max;
		minMaxIdx(disparityMap, &min, &max);
		Mat cm_disp, scaledDisparityMap;
		convertScaleAbs(disparityMap, scaledDisparityMap, 255 / (max - min));
		applyColorMap(scaledDisparityMap, cm_disp, COLORMAP_JET);
		resize(cm_disp, cm_disp, Size(640, 480));
		imshow("cm disparity m", cm_disp);

		// 点云计算
		Mat pointcloud;
		disparityMap.convertTo(disparityMap, CV_32FC1);
		reprojectImageTo3D(disparityMap, pointcloud, Q, true, -1);

		// 点云的存储
		saveXYZ("pointcloud.txt", pointcloud);

		// 去除背景
		Mat dst, thresholded_disp;
		threshold(scaledDisparityMap, thresholded_disp, 0, 255, THRESH_OTSU + THRESH_BINARY);
		resize(thresholded_disp, dst, Size(640, 480));
		imshow("threshold disp otsu", dst);

		// Apply the mask to the point cloud
		Mat pointcloud_tresh, color_tresh;
		pointcloud.copyTo(pointcloud_tresh, thresholded_disp);
		color.copyTo(color_tresh, thresholded_disp);

		// 点云的存储
		//saveXYZ("pointcloud_tresh.txt", color_tresh);

		// Show the point cloud on viz
		viz::Viz3d myWindow("Point cloud with color");
		myWindow.setBackgroundMeshLab();
		myWindow.showWidget("coosys", viz::WCoordinateSystem());
		myWindow.showWidget("pointcloud", viz::WCloud(pointcloud_tresh, color_tresh));
		myWindow.showWidget("text2d", viz::WText("Point cloud", Point(20, 20), 20, viz::Color::green()));
		myWindow.spin();
	}
}


/********************************************************
*
*功能：格雷码图案的立体校准测试
*日期：2017-03-31
*说明：
*     (1)2017-04-07：图像的重映射Bug，rmap分配的空间太小;
*     (2)彩色图像即可
*
*********************************************************/
void CMESR_ProjectDlg::OnBnClickedRectbut()
{
	// opencv显示框
	cvNamedWindow("RectAfter", CV_WINDOW_NORMAL);	
	cvResizeWindow("RectAfter", 320, 240);
	cvNamedWindow("RectBefore", CV_WINDOW_NORMAL);
	cvResizeWindow("RectBefore", 320, 240);

	cvNamedWindow("Rect", CV_WINDOW_NORMAL);
	cvResizeWindow("Rect", 320, 240);

	// 读入图像序列
	string ImgName;
	vector<string> imagelist;
	for (unsigned int i = 0; i < 88; i++)
	{
		// 类型转换
		string img_name;
		char s[12];

		if (i < 44)
		{
			_itoa(i + 1, s, 10);
			img_name = s;
			img_name = "./imgdata/pattern_cam1_im" + img_name + ".png";
		}
		else
		{
			_itoa((i + 1) - 44, s, 10);
			img_name = s;
			img_name = "./imgdata/pattern_cam2_im" + img_name + ".png";
		}
		imagelist.push_back(img_name);
	}

	// 参数初始化
	unsigned int numberOfPatternImages = 42;
	Mat color = imread(imagelist[numberOfPatternImages], IMREAD_COLOR);
	Size imagesSize = color.size();

	vector<vector<Mat> > captured_pattern;
	captured_pattern.resize(2);
	captured_pattern[0].resize(numberOfPatternImages);
	captured_pattern[1].resize(numberOfPatternImages);

	// 立体校正
	Mat R1, R2, P1, P2, Q;
	Rect validRoi[2];
	stereoRectify(cam1intrinsics, cam1distCoeffs, cam2intrinsics, cam2distCoeffs, imagesSize, R, T, R1, R2, P1, P2, Q,
		0, -1, imagesSize, &validRoi[0], &validRoi[1]);
	Mat map1x, map1y, map2x, map2y;
	initUndistortRectifyMap(cam1intrinsics, cam1distCoeffs, R1, P1, imagesSize, CV_32FC1, map1x, map1y);
	initUndistortRectifyMap(cam2intrinsics, cam2distCoeffs, R2, P2, imagesSize, CV_32FC1, map2x, map2y);

	// 极线校正初始化
	vector<vector<Mat> > captured_patternRecty;
	captured_patternRecty.resize(2);
	captured_patternRecty[0].resize(numberOfPatternImages);
	captured_patternRecty[1].resize(numberOfPatternImages);
	
	// 读入图像
	//captured_pattern[0][0] = imread(imagelist[0], IMREAD_GRAYSCALE);
	//captured_pattern[1][0] = imread(imagelist[numberOfPatternImages + 2], IMREAD_GRAYSCALE);
	captured_pattern[0][0] = imread("pattern_cam1_im43.png", IMREAD_GRAYSCALE);
	captured_pattern[1][0] = imread("pattern_cam2_im43.png", IMREAD_GRAYSCALE);
	
	// 图像重映射
	remap(captured_pattern[0][0], captured_patternRecty[0][0], map1x, map1y, CV_INTER_LINEAR, BORDER_CONSTANT, Scalar(0, 0, 0));
	remap(captured_pattern[1][0], captured_patternRecty[1][0], map1x, map1y, CV_INTER_LINEAR, BORDER_CONSTANT, Scalar(0, 0, 0));
	
	// 显示结果
	cvShowImage("RectBefore", &(IplImage(captured_pattern[0][0])));
	cvShowImage("RectAfter", &(IplImage(captured_patternRecty[0][0])));

	// 极线绘制
	Mat showImage(imagesSize.height, 2 * imagesSize.width, CV_8UC1);
	Rect rectLeft(0, 0, imagesSize.width, imagesSize.height);
	Rect rectRight(imagesSize.width, 0, imagesSize.width, imagesSize.height);

	captured_patternRecty[0][0].copyTo(showImage(rectLeft));
	captured_patternRecty[1][0].copyTo(showImage(rectRight));

	for (int i = 0; i < imagesSize.height; i += 32)
	{
		line(showImage, Point(0, i), Point(2 * imagesSize.width, i), Scalar(0, 255, 0), 1, 8);
	}
	cvShowImage("Rect", &(IplImage(showImage)));
}



/********************************************************
*
*功能：3DR(校准之后的图像)
*日期：2017-04-07
*说明：
*    (1)彩色图像；
*
*********************************************************/
void CMESR_ProjectDlg::OnBnClickedBut3drRectfy()
{
	// 读入图像序列
	string ImgName;
	vector<string> imagelist;
	for (unsigned int i = 0; i < 88; i++)
	{
		// 类型转换
		string img_name;
		char s[12];

		if (i < 44)
		{
			_itoa(i + 1, s, 10);
			img_name = s;
			img_name = "./imgdata/pattern_cam1_im" + img_name + ".png";
		}
		else
		{
			_itoa((i + 1) - 44, s, 10);
			img_name = s;
			img_name = "./imgdata/pattern_cam2_im" + img_name + ".png";
		}
		imagelist.push_back(img_name);
	}

	structured_light::GrayCodePattern::Params params_Decode;
	params_Decode.width = Project_Width;
	params_Decode.height = Project_Height;
	Ptr<structured_light::GrayCodePattern> graycode_Decoder = structured_light::GrayCodePattern::create(params_Decode);
	
	// 初始化
	size_t numberOfPatternImages = graycode_Decoder->getNumberOfPatternImages();//42
	vector<vector<Mat>> captured_pattern;
	captured_pattern.resize(2);
	captured_pattern[0].resize(numberOfPatternImages);
	captured_pattern[1].resize(numberOfPatternImages);
	 
	// 全白图像，白色图案编码
	Mat color = imread(imagelist[numberOfPatternImages], IMREAD_COLOR);
	Size imagesSize = color.size();

	// 立体校正
	Mat R1, R2, P1, P2, Q;
	Rect validRoi[2];
	stereoRectify(cam1intrinsics, cam1distCoeffs, cam2intrinsics, cam2distCoeffs, imagesSize, R, T, R1, R2, P1, P2, Q,
		0, -1, imagesSize, &validRoi[0], &validRoi[1]);
	Mat map1x, map1y, map2x, map2y;
	initUndistortRectifyMap(cam1intrinsics, cam1distCoeffs, R1, P1, imagesSize, CV_32FC1, map1x, map1y);
	initUndistortRectifyMap(cam2intrinsics, cam2distCoeffs, R2, P2, imagesSize, CV_32FC1, map2x, map2y);


	// 导入图像
	for (size_t i = 0; i < numberOfPatternImages; i++)
	{
		// 0-41
		captured_pattern[0][i] = imread(imagelist[i], IMREAD_GRAYSCALE);
		// 44-87
		captured_pattern[1][i] = imread(imagelist[i + numberOfPatternImages + 2], IMREAD_GRAYSCALE);
		if ((!captured_pattern[0][i].data) || (!captured_pattern[1][i].data))
		{
			return;
		}
		remap(captured_pattern[0][i], captured_pattern[0][i], map1x, map1y, INTER_NEAREST, BORDER_CONSTANT, Scalar());
		remap(captured_pattern[1][i], captured_pattern[1][i], map2x, map2y, INTER_NEAREST, BORDER_CONSTANT, Scalar());
	}

	// 导入全白全黑背景图案
	vector<Mat> blackImages;
	vector<Mat> whiteImages;
	blackImages.resize(2);
	whiteImages.resize(2);
	whiteImages[0] = imread(imagelist[numberOfPatternImages], IMREAD_GRAYSCALE);//42	
	blackImages[0] = imread(imagelist[numberOfPatternImages + 1], IMREAD_GRAYSCALE);//43
	whiteImages[1] = imread(imagelist[2 * numberOfPatternImages + 2], IMREAD_GRAYSCALE);//86
	blackImages[1] = imread(imagelist[2 * numberOfPatternImages + 2 + 1], IMREAD_GRAYSCALE);//87

	// 参数：INTER_NEAREST, BORDER_CONSTANT, Scalar()); 
	remap(color, color, map1x, map1y, INTER_NEAREST, BORDER_CONSTANT, Scalar());
	remap(whiteImages[0], whiteImages[0], map1x, map1y, INTER_NEAREST, BORDER_CONSTANT, Scalar());
	remap(whiteImages[1], whiteImages[1], map2x, map2y, INTER_NEAREST, BORDER_CONSTANT, Scalar());
	remap(blackImages[0], blackImages[0], map1x, map1y, INTER_NEAREST, BORDER_CONSTANT, Scalar());
	remap(blackImages[1], blackImages[1], map2x, map2y, INTER_NEAREST, BORDER_CONSTANT, Scalar());

	// 解码
	Mat disparityMap;
	//bool decoded = graycode_Decoder->decode(captured_pattern, disparityMap, blackImages, whiteImages, structured_light::DECODE_3D_UNDERWORLD);
	bool decoded = decoder_V1(captured_pattern, disparityMap, blackImages, whiteImages, structured_light::DECODE_3D_UNDERWORLD);

	if (decoded)
	{
		// 求取最大最小值
		double min;
		double max;
		minMaxIdx(disparityMap, &min, &max);

		// 使用线性变换转换输入数组元素成8位无符号整型(0-255)
		Mat cm_disp, scaledDisparityMap;
		convertScaleAbs(disparityMap, scaledDisparityMap, 255 / (max - min));

		// 彩色视差图
		applyColorMap(scaledDisparityMap, cm_disp, COLORMAP_JET);
		resize(cm_disp, cm_disp, Size(640, 480));
		imshow("cm disparity m", cm_disp);

		// 点云计算
		Mat pointcloud;
		disparityMap.convertTo(disparityMap, CV_32FC1);//数据类型转换
		reprojectImageTo3D(disparityMap, pointcloud, Q, true, -1);

		// 点云的存储
		saveXYZ("pointcloud.txt", pointcloud);

		// 去除背景
		Mat dst, thresholded_disp;
		threshold(scaledDisparityMap, thresholded_disp, 0, 255, THRESH_OTSU + THRESH_BINARY);
		resize(thresholded_disp, dst, Size(640, 480));
		imshow("threshold disp otsu", dst);

		// 背景去除
		Mat pointcloud_tresh, color_tresh;
		pointcloud.copyTo(pointcloud_tresh, thresholded_disp);
		color.copyTo(color_tresh, thresholded_disp);

		// 三维显示
		viz::Viz3d myWindow("Point cloud with color");
		myWindow.setBackgroundMeshLab();
		myWindow.showWidget("coosys", viz::WCoordinateSystem());
		myWindow.showWidget("pointcloud", viz::WCloud(pointcloud_tresh, color_tresh));
		myWindow.showWidget("text2d", viz::WText("Point cloud", Point(20, 20), 20, viz::Color::green()));
		myWindow.spin();
	}
}


/********************************************************
*
*功能：图像处理
*日期：2017-04-14
*说明：
*
*********************************************************/
void CMESR_ProjectDlg::OnBnClickedButseam()
{
	CTipImage tipDlg;
	INT_PTR nRes;// 用于保存DoModal函数的返回值   
	nRes = tipDlg.DoModal();// 弹出对话框  
}


/********************************************************
*
*功能：单目标定
*日期：2017-04-14
*说明：
*
*********************************************************/
void CMESR_ProjectDlg::OnBnClickedCamcalib()
{
	CTipCalibDlg camDlg;
	INT_PTR nRes;// 用于保存DoModal函数的返回值   
	nRes = camDlg.DoModal();// 弹出对话框 
}


/********************************************************
*
*功能：退出
*日期：2017-04-25
*说明：
*
*********************************************************/
void CMESR_ProjectDlg::OnBnClickedButexit()
{
	CDialogEx::OnCancel();
}

/********************************************************
*
*功能：投影仪的标定
*日期：2017-04-25
*说明：
*
*********************************************************/
void CMESR_ProjectDlg::OnBnClickedButprojcalib()
{
	CProjCalibDlg projDlg;
	INT_PTR nRes;// 用于保存DoModal函数的返回值   
	nRes = projDlg.DoModal();// 弹出对话框 
}


/********************************************************
*
*功能：基于立体视觉的相移法
*日期：2017-04-27
*说明：
*
*********************************************************/
void CMESR_ProjectDlg::OnBnClickedButstereo3d()
{
	
}
