
// OpenCVLearning.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// COpenCVLearningApp: 
// �йش����ʵ�֣������ OpenCVLearning.cpp
//

class COpenCVLearningApp : public CWinApp
{
public:
	COpenCVLearningApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern COpenCVLearningApp theApp;