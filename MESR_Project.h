/********************************************************
*
*功能：MESR_Project.h : PROJECT_NAME 应用程序的主头文件
*作者：MFC
*说明：
*
*********************************************************/

#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"		// 主符号


// CMESR_ProjectApp: 
// 有关此类的实现，请参阅 MESR_Project.cpp


class CMESR_ProjectApp : public CWinApp
{
public:
	CMESR_ProjectApp();
	 
// 重写
public:
	virtual BOOL InitInstance();

// 实现
	DECLARE_MESSAGE_MAP()
};

extern CMESR_ProjectApp theApp;