// winopt.cpp : 实现文件
//

#include "stdafx.h"
#include "tradesys.h"
#include "winopt.h"

// cwinopt

IMPLEMENT_DYNCREATE(cwinopt, CWinThread)

cwinopt::cwinopt()
	: m_activeflag(0)
{
	for (int t_i = 0; t_i<optitemnum; t_i++)
	{
		m_optitem[t_i].code= 0;
		m_optitem[t_i].price = 0;
		m_optitem[t_i].opt = 0;
	}
}

cwinopt::~cwinopt()
{
}

BOOL cwinopt::InitInstance()
{
	// TODO:    在此执行任意逐线程初始化
	PostThreadMessage(WM_winoptrun, 0, 0);
	return TRUE;
}

int cwinopt::ExitInstance()
{
	// TODO:    在此执行任意逐线程清理
	return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(cwinopt, CWinThread)
END_MESSAGE_MAP()

int cwinopt::winoptactive(WPARAM v_i)
{
	m_activeflag = v_i;
	return 0;
}

int cwinopt::winoptrun()
{
	//AfxMessageBox(_T("winoptactive"));
	return 0;
}

// cwinopt 消息处理程序

int cwinopt::Run()
{
	//	BOOL t_bRet; 
	MSG t_msg;
	while ((GetMessage(&t_msg, NULL, 0, 0)) != 0)
	{
		switch (t_msg.message)
		{
		case WM_winoptactive:
			winoptactive(t_msg.wParam);
			break;
		case WM_winoptrun:
			if (m_activeflag)
			{
				winoptrun();
			}
			PostThreadMessage(WM_winoptrun, 0, 0);
			break;
		default:
			break;
		}
	}
	return CWinThread::Run();
}