/********************************************************
*
*���ܣ�MESR_Project.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
*���ߣ�MFC
*˵����
*
*********************************************************/

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CMESR_ProjectApp: 
// �йش����ʵ�֣������ MESR_Project.cpp


class CMESR_ProjectApp : public CWinApp
{
public:
	CMESR_ProjectApp();
	 
// ��д
public:
	virtual BOOL InitInstance();

// ʵ��
	DECLARE_MESSAGE_MAP()
};

extern CMESR_ProjectApp theApp;