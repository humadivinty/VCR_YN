
// Test_VCR_YN.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CTest_VCR_YNApp: 
// �йش����ʵ�֣������ Test_VCR_YN.cpp
//

class CTest_VCR_YNApp : public CWinApp
{
public:
	CTest_VCR_YNApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CTest_VCR_YNApp theApp;