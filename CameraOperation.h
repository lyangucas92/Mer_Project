/********************************************************
*
*功能：相机操作程序
*作者：lyangucas
*说明：
*
*********************************************************/

#include "GxIAPI.h"
#include "DxImageProc.h"

class CameraOperation
{
public:
	CameraOperation();
	~CameraOperation();

	int GetCamera_Counts(void);
	GX_DEV_HANDLE OpenCamera(uint32_t index, int64_t ImageWidth, int64_t ImageHeight, int64_t WidthChouYang, int64_t HdightChouYang);
	void SetBalance(GX_DEV_HANDLE Camera, double R, double G, double B);
	void SeBrightness(GX_DEV_HANDLE Camera, int64_t Speed, int64_t Gain, double BaoGuangTime);
	void CameraMode(GX_DEV_HANDLE Camera);
	void SetOthersParameters(GX_DEV_HANDLE Camera);
	void SnapControl(GX_DEV_HANDLE Camera, bool isStart);
};