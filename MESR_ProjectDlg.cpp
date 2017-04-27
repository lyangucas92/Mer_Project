#include "stdafx.h"
#include "MESR_Project.h"
#include "MESR_ProjectDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/*��----------------------ȫ�ֱ���--------------------------*/
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


//����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���
class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

	//�Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
		virtual void DoDataExchange(CDataExchange* pDX);//DDX/DDV ֧��

//ʵ��
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
*���ܣ�CMESR_ProjectDlg �Ի����캯��
*���ڣ�2017-03-23
*˵����
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

	//��ʼ������
	leftImageBuffer = new BYTE[MaxImageWidth * MaxImageHeigth * 3];
	rightImageBuffer = new BYTE[MaxImageWidth * MaxImageHeigth * 3];
	pImageLeftBuffer = new BYTE[MaxImageWidth * MaxImageHeigth * 3];
	pImageRightBuffer = new BYTE[MaxImageWidth * MaxImageHeigth * 3];

	//ԭʼ��С��Iplimage��ʽ��ͼ��
	leftImageViev = new BYTE[MaxImageWidth * MaxImageHeigth * 3];
	rightImageViev = new BYTE[MaxImageWidth * MaxImageHeigth * 3];
	viewleftimage = cvCreateImage(CvSize(MaxImageWidth / WChouYang, MaxImageHeigth / HChouYang), 8, 3);
	viewrightimage = cvCreateImage(CvSize(MaxImageWidth / WChouYang, MaxImageHeigth / HChouYang), 8, 3);
	
	//ͼ����ʾ
	CvSize czSize;
	czSize.width = 320;
	czSize.height = 240;
	frameA_show = cvCreateImage(czSize, viewleftimage->depth, viewleftimage->nChannels);
	frameB_show = cvCreateImage(czSize, viewrightimage->depth, viewrightimage->nChannels);

	//�������
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
*���ܣ���׼��������
*���ڣ�2017-03-23
*˵����
*
*********************************************************/
CMESR_ProjectDlg::~CMESR_ProjectDlg()
{
	GXCloseLib();
}


/********************************************************
*
*���ܣ��ؼ������ݽ���
*���ڣ�2017-03-23
*˵����
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
*���ܣ���ť�ۺ���
*���ڣ�2017-03-23
*˵����
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
*���ܣ�CMESR_ProjectDlg ��Ϣ�������
*���ڣ�2017-03-23
*˵����
*	(1)��ʼ��
*
*********************************************************/
BOOL CMESR_ProjectDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
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

	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO:  �ڴ���Ӷ���ĳ�ʼ������
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

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
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


//  �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�
void CMESR_ProjectDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}


//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CMESR_ProjectDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


/********************************************************
*
*���ܣ������
*���ڣ�2017-03-23
*˵����
*     �����-FrameA-1 �����-FrameB-2
*
*********************************************************/
void CMESR_ProjectDlg::OnBnClickedOpencam()
{
	//�����
	leftCamera = cam_Dev.OpenCamera(LeftCameraIndex, MaxImageWidth, MaxImageHeigth, WChouYang, HChouYang);	
	//�������
	cam_Dev.SetBalance(leftCamera, 1.40, 1.00, 1.52);
	cam_Dev.SeBrightness(leftCamera, 5, 10, 50000);
	cam_Dev.CameraMode(leftCamera);
	cam_Dev.SetOthersParameters(leftCamera);

	rightCamera = cam_Dev.OpenCamera(RightCameraIndex, MaxImageWidth, MaxImageHeigth, WChouYang, HChouYang);
	//�������
	cam_Dev.SetBalance(rightCamera, 1.40, 1.00, 1.52);
	cam_Dev.SeBrightness(rightCamera, 5, 10, 50000);
	cam_Dev.CameraMode(rightCamera);
	cam_Dev.SetOthersParameters(rightCamera);
	 
	Action_Satus = "Camera Open��";
	UpdateData(FALSE);
}


/********************************************************
*
*���ܣ���ȡ�豸��Ŀ
*���ڣ�2017-03-23
*˵����
*
*********************************************************/
void CMESR_ProjectDlg::OnBnClickedCheckcam()
{
	CamCounts = cam_Dev.GetCamera_Counts();
	Action_Satus = "Check OK��";
	UpdateData(FALSE);
}


/********************************************************
*
*���ܣ���ʼ�ɼ�
*���ڣ�2017-03-23
*˵����
*
*********************************************************/
void CMESR_ProjectDlg::OnBnClickedSnapstart()
{
	GXRegisterCaptureCallback(leftCamera, pImageLeftBuffer, getLeftImage);//ע��ص�
	GXSendCommand(leftCamera, GX_COMMAND_ACQUISITION_START);//��ʼ�ɼ�
	GXRegisterCaptureCallback(rightCamera, pImageRightBuffer, getRightImage);//ע��ص�
	GXSendCommand(rightCamera, GX_COMMAND_ACQUISITION_START);//��ʼ�ɼ�

	Action_Satus = "Start Snap��";
	UpdateData(FALSE);
}


/********************************************************
*
*���ܣ�������Ļص�����
*���ڣ�2017-03-23
*˵����
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
*���ܣ�������Ļص�����
*���ڣ�2017-03-23
*˵����
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
*���ܣ���ͼƬ��ʾ���Ի�������
*���ڣ�2016-03-03
*˵����
*
*********************************************************/
void CMESR_ProjectDlg::DrawPicToHDC(IplImage *img, UINT ID)
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
*���ܣ���ͼƬ��ʾ���Ի�������
*���ڣ�2017-03-23
*˵����
*
*********************************************************/
void CMESR_ProjectDlg::OnBnClickedImgshow()
{
	// ���ü�ʱ��
	SetTimer(1, 10, NULL);
	SetTimer(2, 15, NULL);
}


/********************************************************
*
*���ܣ���ʱ���ۺ���
*���ڣ�2017-03-23
*˵����
*	(1)���ھ�̬�ص������޷���ʾ��bug,������ͨ����ʱ��ʵ����ʾ
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
			//ͼ������
			resize(viewleftimage_gray, viewleftimage_gray, Size(1024, 768), 0, 0, CV_INTER_LINEAR);
			imwrite("./Grayimgdata/pattern_cam1_im" + name.str() + ".png", viewleftimage_gray);

			//ͼ�񱣴�
			Mat viewrightimage_gray = cvarrToMat(viewrightimage);
			cvtColor(viewrightimage_gray, viewrightimage_gray, COLOR_RGB2GRAY);
			//ͼ������
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

			//ͼ�񱣴�
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
*���ܣ�ֹͣ��ʾ
*���ڣ�2017-03-23
*˵����
*
*********************************************************/
void CMESR_ProjectDlg::OnBnClickedStopshow()
{
	KillTimer(1);
	KillTimer(2);

	Action_Satus = "Close Display��";
	UpdateData(FALSE);
}


/********************************************************
*
*���ܣ��ر����
*���ڣ�2017-03-23
*˵����
*
*********************************************************/
void CMESR_ProjectDlg::OnBnClickedClosecam()
{
	GXCloseDevice(leftCamera);
	GXCloseDevice(rightCamera);

	Action_Satus = "Close Camera��";
	UpdateData(FALSE);
}


/********************************************************
*
*���ܣ�ֹͣ�ɼ�
*���ڣ�2017-03-23
*˵����
*
*********************************************************/
void CMESR_ProjectDlg::OnBnClickedStopsnap()
{
	GXSendCommand(leftCamera, GX_COMMAND_ACQUISITION_STOP);//����ͣ������
	GXUnregisterCaptureCallback(leftCamera);//ע���ɼ��ص�	
	GXSendCommand(rightCamera, GX_COMMAND_ACQUISITION_STOP);//����ͣ������
	GXUnregisterCaptureCallback(rightCamera);//ע���ɼ��ص�

	Action_Satus = "Close Snap��";
	UpdateData(FALSE);
}


/********************************************************
*
*���ܣ������룺����������ͼ��
*���ڣ�2017-03-23
*˵����
*
*********************************************************/
void CMESR_ProjectDlg::OnBnClickedPattern(void)
{
	// �����ؼ��е����ݱ��浽��Ӧ�ı���   
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

	//����ͶӰ�Ǵ�����ʾ
	namedWindow("Pattern Window", WINDOW_NORMAL);
	resizeWindow("Pattern Window", params.width, params.height);
	moveWindow("Pattern Window", params.width + 316, -20);
	setWindowProperty("Pattern Window", WND_PROP_FULLSCREEN, WINDOW_FULLSCREEN);
}


/********************************************************
*
*���ܣ������룺������ͼ�������ֶ�����
*���ڣ�2017-03-23
*˵����
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
*���ܣ������룺������ͼ�������Զ�����
*���ڣ�2017-03-23
*˵����
*
*********************************************************/
void CMESR_ProjectDlg::OnBnClickedAutoplay()
{
	SetTimer(3, 3500, NULL);
}


/********************************************************
*
*���ܣ������룺ͼ�񱣴�
*���ڣ�2017-03-23
*˵����
*
*********************************************************/
void CMESR_ProjectDlg::OnBnClickedSaveimg()
{
	static unsigned int i = 0;
	ostringstream name;

	name << i + 1;

	//ͼ�񱣴�
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
*���ܣ����Ʒ�������ͼ��
*���ڣ�2017-03-26
*˵����
*
*********************************************************/
void CMESR_ProjectDlg::OnBnClickedPhasepattern()
{
	UpdateData(TRUE);

	//���Ʋ����ĳ�ʼ��
	int nSel;
	CString strWeb;

	//���Ʒ���
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

	//���뷽��
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
	
	//������ʼ��
	params_sin.width = Phase_WeighT;
	params_sin.height = Phase_Height;
	params_sin.nbrOfPeriods = Phase_Period;
	params_sin.setMarkers = MakerPattern;
	params_sin.horizontal = DirectionPattrn;
	params_sin.shiftValue = static_cast<float>(2 * CV_PI / 3);
	params_sin.nbrOfPixelsBetweenMarkers = 70;

	//������������ͼ��
	sinus = structured_light::SinusoidalPattern::create(params_sin);
	sinus->generate(patterns_sin);

	//����ͶӰ������ʾ
	namedWindow("pattern_sin", WINDOW_NORMAL);
	setWindowProperty("pattern_sin", WND_PROP_FULLSCREEN, WINDOW_FULLSCREEN);
	Phase_Status = "�������ɣ�";
	UpdateData(FALSE);
}


/********************************************************
*
*���ܣ����Ʒ���ͼ���ֶ�����
*���ڣ�2017-03-26
*˵����
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

		//ͼ�񱣴�
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
		Phase_Status = "Collection Finished��";
		UpdateData(FALSE);
	}
	count++;
}


/********************************************************
*
*���ܣ�����
*���ڣ�2017-04-05
*˵����
*    (1)�����ִ��Ч�ʺܵͣ�
*    (2)�����Ч��Ҳ���Ǻܺã�
*
*********************************************************/
void CMESR_ProjectDlg::OnBnClickedDecodephase()
{
	int nSel;
	CString strWeb;
	//����ͼ��
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

	//����ͼ������
	string ImgName;
	vector<string> imagelist;
	if (DirectionPattrn == false)
	{
		for (unsigned int i = 0; i < 30; i++)
		{
			//����ת��
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
			//����ת��
			string img_name;
			char s[12];
			_itoa(i + 1, s, 10);
			img_name = s;
			img_name = "./PhaseImg/phaseRight_im" + img_name + ".png";

			imagelist.push_back(img_name);
		}
	}

	// ����ͼ��
	vector<Mat> img(30);
	Size camSize(-1, -1);
	for (size_t i = 0; i < 30; i++)
		img[i] = imread(imagelist[i], IMREAD_GRAYSCALE);


	//����
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
				//������ʼ��
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
	Phase_Status = "Decoder OK��";
	UpdateData(FALSE);
}



/********************************************************
*
*���ܣ����Ʒ���ͼ���Զ�����
*���ڣ�2017-03-26
*˵����
*
*********************************************************/
void CMESR_ProjectDlg::OnBnClickedButton4()
{
	SetTimer(4, 4500, NULL);
}


/********************************************************
*
*���ܣ���ά���Ƶı���
*���ڣ�2017-04-05
*˵����
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
*���ܣ������룺3DR(У׼֮ǰ��ͼ��)
*���ڣ�2017-03-28
*˵����
*
*********************************************************/
void CMESR_ProjectDlg::OnBnClickedButdecoder()
{
	// ����ͼ������
	string ImgName;
	vector<string> imagelist;
	for (unsigned int i = 0; i < 88; i++)
	{
		// ����ת��
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

	// ����궨����
	if ((!R.data) || (!T.data) || (!cam1intrinsics.data) || (!cam2intrinsics.data) || (!cam1distCoeffs.data) || (!cam2distCoeffs.data))
	{
		return;
	}
	
	size_t numberOfPatternImages = graycode_Decoder->getNumberOfPatternImages();//42
	vector<vector<Mat>> captured_pattern;
	captured_pattern.resize(2);
	captured_pattern[0].resize(numberOfPatternImages);
	captured_pattern[1].resize(numberOfPatternImages);

	// ȫ��ͼ�񣬰�ɫͼ������
	Mat color = imread(imagelist[numberOfPatternImages], IMREAD_COLOR);
	// ͼ����ӳ��֮��
	Size imagesSize = color.size();

 	// Stereo rectify
	cout << "Rectifying images..." << endl;
	Mat R1, R2, P1, P2, Q;
	Rect validRoi[2];
	stereoRectify(cam1intrinsics, cam1distCoeffs, cam2intrinsics, cam2distCoeffs, imagesSize, R, T, R1, R2, P1, P2, Q, 0,
		-1, imagesSize, &validRoi[0], &validRoi[1]);

	// ����ͼ��
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

	// ����ȫ��ȫ�ڱ���ͼ��
	vector<Mat> blackImages;
	vector<Mat> whiteImages;
	blackImages.resize(2);
	whiteImages.resize(2);
	whiteImages[0] = imread(imagelist[numberOfPatternImages], IMREAD_GRAYSCALE);//42	
	blackImages[0] = imread(imagelist[numberOfPatternImages + 1], IMREAD_GRAYSCALE);//43
	whiteImages[1] = imread(imagelist[2 * numberOfPatternImages + 2], IMREAD_GRAYSCALE);//86
	blackImages[1] = imread(imagelist[2 * numberOfPatternImages + 2 + 1], IMREAD_GRAYSCALE);//87

	// ����
	Mat disparityMap;
	bool decoded = graycode_Decoder->decode(captured_pattern, disparityMap, blackImages, whiteImages, structured_light::DECODE_3D_UNDERWORLD);

	if (decoded)
	{
		// ��ɫ�Ӳ�ͼ��
		double min;
		double max;
		minMaxIdx(disparityMap, &min, &max);
		Mat cm_disp, scaledDisparityMap;
		convertScaleAbs(disparityMap, scaledDisparityMap, 255 / (max - min));
		applyColorMap(scaledDisparityMap, cm_disp, COLORMAP_JET);
		resize(cm_disp, cm_disp, Size(640, 480));
		imshow("cm disparity m", cm_disp);

		// ���Ƽ���
		Mat pointcloud;
		disparityMap.convertTo(disparityMap, CV_32FC1);
		reprojectImageTo3D(disparityMap, pointcloud, Q, true, -1);

		// ���ƵĴ洢
		saveXYZ("pointcloud.txt", pointcloud);

		// ȥ������
		Mat dst, thresholded_disp;
		threshold(scaledDisparityMap, thresholded_disp, 0, 255, THRESH_OTSU + THRESH_BINARY);
		resize(thresholded_disp, dst, Size(640, 480));
		imshow("threshold disp otsu", dst);

		// Apply the mask to the point cloud
		Mat pointcloud_tresh, color_tresh;
		pointcloud.copyTo(pointcloud_tresh, thresholded_disp);
		color.copyTo(color_tresh, thresholded_disp);

		// ���ƵĴ洢
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
*���ܣ�������ͼ��������У׼����
*���ڣ�2017-03-31
*˵����
*     (1)2017-04-07��ͼ�����ӳ��Bug��rmap����Ŀռ�̫С;
*     (2)��ɫͼ�񼴿�
*
*********************************************************/
void CMESR_ProjectDlg::OnBnClickedRectbut()
{
	// opencv��ʾ��
	cvNamedWindow("RectAfter", CV_WINDOW_NORMAL);	
	cvResizeWindow("RectAfter", 320, 240);
	cvNamedWindow("RectBefore", CV_WINDOW_NORMAL);
	cvResizeWindow("RectBefore", 320, 240);

	cvNamedWindow("Rect", CV_WINDOW_NORMAL);
	cvResizeWindow("Rect", 320, 240);

	// ����ͼ������
	string ImgName;
	vector<string> imagelist;
	for (unsigned int i = 0; i < 88; i++)
	{
		// ����ת��
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

	// ������ʼ��
	unsigned int numberOfPatternImages = 42;
	Mat color = imread(imagelist[numberOfPatternImages], IMREAD_COLOR);
	Size imagesSize = color.size();

	vector<vector<Mat> > captured_pattern;
	captured_pattern.resize(2);
	captured_pattern[0].resize(numberOfPatternImages);
	captured_pattern[1].resize(numberOfPatternImages);

	// ����У��
	Mat R1, R2, P1, P2, Q;
	Rect validRoi[2];
	stereoRectify(cam1intrinsics, cam1distCoeffs, cam2intrinsics, cam2distCoeffs, imagesSize, R, T, R1, R2, P1, P2, Q,
		0, -1, imagesSize, &validRoi[0], &validRoi[1]);
	Mat map1x, map1y, map2x, map2y;
	initUndistortRectifyMap(cam1intrinsics, cam1distCoeffs, R1, P1, imagesSize, CV_32FC1, map1x, map1y);
	initUndistortRectifyMap(cam2intrinsics, cam2distCoeffs, R2, P2, imagesSize, CV_32FC1, map2x, map2y);

	// ����У����ʼ��
	vector<vector<Mat> > captured_patternRecty;
	captured_patternRecty.resize(2);
	captured_patternRecty[0].resize(numberOfPatternImages);
	captured_patternRecty[1].resize(numberOfPatternImages);
	
	// ����ͼ��
	//captured_pattern[0][0] = imread(imagelist[0], IMREAD_GRAYSCALE);
	//captured_pattern[1][0] = imread(imagelist[numberOfPatternImages + 2], IMREAD_GRAYSCALE);
	captured_pattern[0][0] = imread("pattern_cam1_im43.png", IMREAD_GRAYSCALE);
	captured_pattern[1][0] = imread("pattern_cam2_im43.png", IMREAD_GRAYSCALE);
	
	// ͼ����ӳ��
	remap(captured_pattern[0][0], captured_patternRecty[0][0], map1x, map1y, CV_INTER_LINEAR, BORDER_CONSTANT, Scalar(0, 0, 0));
	remap(captured_pattern[1][0], captured_patternRecty[1][0], map1x, map1y, CV_INTER_LINEAR, BORDER_CONSTANT, Scalar(0, 0, 0));
	
	// ��ʾ���
	cvShowImage("RectBefore", &(IplImage(captured_pattern[0][0])));
	cvShowImage("RectAfter", &(IplImage(captured_patternRecty[0][0])));

	// ���߻���
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
*���ܣ�3DR(У׼֮���ͼ��)
*���ڣ�2017-04-07
*˵����
*    (1)��ɫͼ��
*
*********************************************************/
void CMESR_ProjectDlg::OnBnClickedBut3drRectfy()
{
	// ����ͼ������
	string ImgName;
	vector<string> imagelist;
	for (unsigned int i = 0; i < 88; i++)
	{
		// ����ת��
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
	
	// ��ʼ��
	size_t numberOfPatternImages = graycode_Decoder->getNumberOfPatternImages();//42
	vector<vector<Mat>> captured_pattern;
	captured_pattern.resize(2);
	captured_pattern[0].resize(numberOfPatternImages);
	captured_pattern[1].resize(numberOfPatternImages);
	 
	// ȫ��ͼ�񣬰�ɫͼ������
	Mat color = imread(imagelist[numberOfPatternImages], IMREAD_COLOR);
	Size imagesSize = color.size();

	// ����У��
	Mat R1, R2, P1, P2, Q;
	Rect validRoi[2];
	stereoRectify(cam1intrinsics, cam1distCoeffs, cam2intrinsics, cam2distCoeffs, imagesSize, R, T, R1, R2, P1, P2, Q,
		0, -1, imagesSize, &validRoi[0], &validRoi[1]);
	Mat map1x, map1y, map2x, map2y;
	initUndistortRectifyMap(cam1intrinsics, cam1distCoeffs, R1, P1, imagesSize, CV_32FC1, map1x, map1y);
	initUndistortRectifyMap(cam2intrinsics, cam2distCoeffs, R2, P2, imagesSize, CV_32FC1, map2x, map2y);


	// ����ͼ��
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

	// ����ȫ��ȫ�ڱ���ͼ��
	vector<Mat> blackImages;
	vector<Mat> whiteImages;
	blackImages.resize(2);
	whiteImages.resize(2);
	whiteImages[0] = imread(imagelist[numberOfPatternImages], IMREAD_GRAYSCALE);//42	
	blackImages[0] = imread(imagelist[numberOfPatternImages + 1], IMREAD_GRAYSCALE);//43
	whiteImages[1] = imread(imagelist[2 * numberOfPatternImages + 2], IMREAD_GRAYSCALE);//86
	blackImages[1] = imread(imagelist[2 * numberOfPatternImages + 2 + 1], IMREAD_GRAYSCALE);//87

	// ������INTER_NEAREST, BORDER_CONSTANT, Scalar()); 
	remap(color, color, map1x, map1y, INTER_NEAREST, BORDER_CONSTANT, Scalar());
	remap(whiteImages[0], whiteImages[0], map1x, map1y, INTER_NEAREST, BORDER_CONSTANT, Scalar());
	remap(whiteImages[1], whiteImages[1], map2x, map2y, INTER_NEAREST, BORDER_CONSTANT, Scalar());
	remap(blackImages[0], blackImages[0], map1x, map1y, INTER_NEAREST, BORDER_CONSTANT, Scalar());
	remap(blackImages[1], blackImages[1], map2x, map2y, INTER_NEAREST, BORDER_CONSTANT, Scalar());

	// ����
	Mat disparityMap;
	//bool decoded = graycode_Decoder->decode(captured_pattern, disparityMap, blackImages, whiteImages, structured_light::DECODE_3D_UNDERWORLD);
	bool decoded = decoder_V1(captured_pattern, disparityMap, blackImages, whiteImages, structured_light::DECODE_3D_UNDERWORLD);

	if (decoded)
	{
		// ��ȡ�����Сֵ
		double min;
		double max;
		minMaxIdx(disparityMap, &min, &max);

		// ʹ�����Ա任ת����������Ԫ�س�8λ�޷�������(0-255)
		Mat cm_disp, scaledDisparityMap;
		convertScaleAbs(disparityMap, scaledDisparityMap, 255 / (max - min));

		// ��ɫ�Ӳ�ͼ
		applyColorMap(scaledDisparityMap, cm_disp, COLORMAP_JET);
		resize(cm_disp, cm_disp, Size(640, 480));
		imshow("cm disparity m", cm_disp);

		// ���Ƽ���
		Mat pointcloud;
		disparityMap.convertTo(disparityMap, CV_32FC1);//��������ת��
		reprojectImageTo3D(disparityMap, pointcloud, Q, true, -1);

		// ���ƵĴ洢
		saveXYZ("pointcloud.txt", pointcloud);

		// ȥ������
		Mat dst, thresholded_disp;
		threshold(scaledDisparityMap, thresholded_disp, 0, 255, THRESH_OTSU + THRESH_BINARY);
		resize(thresholded_disp, dst, Size(640, 480));
		imshow("threshold disp otsu", dst);

		// ����ȥ��
		Mat pointcloud_tresh, color_tresh;
		pointcloud.copyTo(pointcloud_tresh, thresholded_disp);
		color.copyTo(color_tresh, thresholded_disp);

		// ��ά��ʾ
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
*���ܣ�ͼ����
*���ڣ�2017-04-14
*˵����
*
*********************************************************/
void CMESR_ProjectDlg::OnBnClickedButseam()
{
	CTipImage tipDlg;
	INT_PTR nRes;// ���ڱ���DoModal�����ķ���ֵ   
	nRes = tipDlg.DoModal();// �����Ի���  
}


/********************************************************
*
*���ܣ���Ŀ�궨
*���ڣ�2017-04-14
*˵����
*
*********************************************************/
void CMESR_ProjectDlg::OnBnClickedCamcalib()
{
	CTipCalibDlg camDlg;
	INT_PTR nRes;// ���ڱ���DoModal�����ķ���ֵ   
	nRes = camDlg.DoModal();// �����Ի��� 
}


/********************************************************
*
*���ܣ��˳�
*���ڣ�2017-04-25
*˵����
*
*********************************************************/
void CMESR_ProjectDlg::OnBnClickedButexit()
{
	CDialogEx::OnCancel();
}

/********************************************************
*
*���ܣ�ͶӰ�ǵı궨
*���ڣ�2017-04-25
*˵����
*
*********************************************************/
void CMESR_ProjectDlg::OnBnClickedButprojcalib()
{
	CProjCalibDlg projDlg;
	INT_PTR nRes;// ���ڱ���DoModal�����ķ���ֵ   
	nRes = projDlg.DoModal();// �����Ի��� 
}


/********************************************************
*
*���ܣ����������Ӿ������Ʒ�
*���ڣ�2017-04-27
*˵����
*
*********************************************************/
void CMESR_ProjectDlg::OnBnClickedButstereo3d()
{
	
}
