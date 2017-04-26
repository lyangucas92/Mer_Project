#include "stdafx.h"
#include "CameraOperation.h"

/********************************************************
*
*���ܣ�CameraOperation�Ĺ��캯��
*���ڣ�2017-03-23
*˵����
*
*********************************************************/
CameraOperation::CameraOperation(void)
{
	//����ͷ�⺯����ʼ��
	GXInitLib();
}


/********************************************************
*
*���ܣ�CameraOperation����������
*���ڣ�2017-03-23
*˵����
*
*********************************************************/
CameraOperation::~CameraOperation(void)
{

}

/********************************************************
*
*���ܣ���ȡ�����Ŀ
*���ڣ�2017-03-23
*˵����
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
*���ܣ������
*���ڣ�2017-03-23
*˵����
*
*********************************************************/
GX_DEV_HANDLE CameraOperation::OpenCamera(uint32_t index, int64_t ImageWidth, int64_t ImageHeight, int64_t WidthChouYang, int64_t HdightChouYang)
{
	GX_DEV_HANDLE Camera;

	//���������
	GXOpenDeviceByIndex(index, &Camera);
	GXSetInt(Camera, GX_INT_WIDTH, ImageWidth);
	GXSetInt(Camera, GX_INT_HEIGHT, ImageHeight);

	//���ó�������
	GXSetInt(Camera, GX_INT_DECIMATION_HORIZONTAL, WidthChouYang);
	GXSetInt(Camera, GX_INT_DECIMATION_VERTICAL, HdightChouYang);

	return Camera;
}

/********************************************************
*
*���ܣ����ð�ƽ��
*���ڣ�2017-03-23
*˵����
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
*���ܣ���������
*���ڣ�2017-03-23
*˵����
*
*********************************************************/
void CameraOperation::SeBrightness(GX_DEV_HANDLE Camera, int64_t Speed, int64_t Gain, double BaoGuangTime)
{
	GXSetInt(Camera, GX_INT_ACQUISITION_SPEED_LEVEL, Speed);//���òɼ��ٶ�0--22��default13
	GXSetInt(Camera, GX_INT_GAIN, Gain);//���淶Χ9--63��default9
	GXSetFloat(Camera, GX_FLOAT_EXPOSURE_TIME, BaoGuangTime);//�ع�ʱ��us
}

/********************************************************
*
*���ܣ��������ģʽ������
*���ڣ�2017-03-23
*˵����
*
*********************************************************/
void CameraOperation::CameraMode(GX_DEV_HANDLE Camera)
{
	GXSetEnum(Camera, GX_ENUM_ACQUISITION_MODE, GX_ACQ_MODE_CONTINUOUS);//���òɼ�ģʽΪ�����ɼ�
	GXSetEnum(Camera, GX_ENUM_TRIGGER_MODE, GX_TRIGGER_MODE_OFF);//�رմ����ͻ�һֱ��ͣ�Ĳɼ�
}


/********************************************************
*
*���ܣ�������ز���������
*���ڣ�2017-03-23
*˵����
*
*********************************************************/
void CameraOperation::SetOthersParameters(GX_DEV_HANDLE Camera)
{
	GXSetEnum(Camera, GX_ENUM_COLOR_CORRECT, GX_COLOR_CORRECT_ON);
}


/********************************************************
*
*���ܣ��ɼ�����
*���ڣ�2017-03-23
*˵����
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