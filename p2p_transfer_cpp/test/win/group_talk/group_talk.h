
// group_talk.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// Cgroup_talkApp:
// �йش����ʵ�֣������ group_talk.cpp
//

class Cgroup_talkApp : public CWinApp
{
public:
	Cgroup_talkApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern Cgroup_talkApp theApp;