#include "stdafx.h"
#include "CameraOperation.h"

/********************************************************
*
*功能：CameraOperation的构造函数
*日期：2017-03-23
*说明：
*
*********************************************************/
CameraOperation::CameraOperation(void)
{
	//摄像头库函数初始化
	GXInitLib();
}


/********************************************************
*
*功能：CameraOperation的析构函数
*日期：2017-03-23
*说明：
*
*********************************************************/
CameraOperation::~CameraOperation(void)
{

}

/********************************************************
*
*功能：获取相机数目
*日期：2017-03-23
*说明：
*
*********************************************************/
int CameraOperation::GetCamera_Counts(void)
{
	uint32_t nDeviceNum = 0;
	GX_STATUS status = GXUpdateDeviceList(&nDeviceNum, 1000);
	return nDeviceNum;
}


/********************************************************
*
*功能：打开相机
*日期：2017-03-23
*说明：
*
*********************************************************/
GX_DEV_HANDLE CameraOperation::OpenCamera(uint32_t index, int64_t ImageWidth, int64_t ImageHeight, int64_t WidthChouYang, int64_t HdightChouYang)
{
	GX_DEV_HANDLE Camera;

	//打开数字相机
	GXOpenDeviceByIndex(index, &Camera);
	GXSetInt(Camera, GX_INT_WIDTH, ImageWidth);
	GXSetInt(Camera, GX_INT_HEIGHT, ImageHeight);

	//设置抽样比例
	GXSetInt(Camera, GX_INT_DECIMATION_HORIZONTAL, WidthChouYang);
	GXSetInt(Camera, GX_INT_DECIMATION_VERTICAL, HdightChouYang);

	return Camera;
}

/********************************************************
*
*功能：设置白平衡
*日期：2017-03-23
*说明：
*
*********************************************************/
void CameraOperation::SetBalance(GX_DEV_HANDLE Camera, double R, double G, double B)
{
	GXSetEnum(Camera, GX_ENUM_BALANCE_RATIO_SELECTOR, GX_BALANCE_RATIO_SELECTOR_RED);
	GXSetFloat(Camera, GX_FLOAT_BALANCE_RATIO, R);
	GXSetEnum(Camera, GX_ENUM_BALANCE_RATIO_SELECTOR, GX_BALANCE_RATIO_SELECTOR_GREEN);
	GXSetFloat(Camera, GX_FLOAT_BALANCE_RATIO, G);
	GXSetEnum(Camera, GX_ENUM_BALANCE_RATIO_SELECTOR, GX_BALANCE_RATIO_SELECTOR_BLUE);
	GXSetFloat(Camera, GX_FLOAT_BALANCE_RATIO, B);
}

/********************************************************
*
*功能：设置亮度
*日期：2017-03-23
*说明：
*
*********************************************************/
void CameraOperation::SeBrightness(GX_DEV_HANDLE Camera, int64_t Speed, int64_t Gain, double BaoGuangTime)
{
	GXSetInt(Camera, GX_INT_ACQUISITION_SPEED_LEVEL, Speed);//设置采集速度0--22，default13
	GXSetInt(Camera, GX_INT_GAIN, Gain);//增益范围9--63，default9
	GXSetFloat(Camera, GX_FLOAT_EXPOSURE_TIME, BaoGuangTime);//曝光时间us
}

/********************************************************
*
*功能：相机连续模式的设置
*日期：2017-03-23
*说明：
*
*********************************************************/
void CameraOperation::CameraMode(GX_DEV_HANDLE Camera)
{
	GXSetEnum(Camera, GX_ENUM_ACQUISITION_MODE, GX_ACQ_MODE_CONTINUOUS);//设置采集模式为连续采集
	GXSetEnum(Camera, GX_ENUM_TRIGGER_MODE, GX_TRIGGER_MODE_OFF);//关闭触发就会一直不停的采集
}


/********************************************************
*
*功能：其他相关参数的设置
*日期：2017-03-23
*说明：
*
*********************************************************/
void CameraOperation::SetOthersParameters(GX_DEV_HANDLE Camera)
{
	GXSetEnum(Camera, GX_ENUM_COLOR_CORRECT, GX_COLOR_CORRECT_ON);
}


/********************************************************
*
*功能：采集控制
*日期：2017-03-23
*说明：
*
*********************************************************/
void CameraOperation::SnapControl(GX_DEV_HANDLE Camera, bool isStart)
{
	if (isStart)
	{
		GXSendCommand(Camera, GX_COMMAND_ACQUISITION_START);
	}
	else
	{
		GXSendCommand(Camera, GX_COMMAND_ACQUISITION_STOP);
	}
}