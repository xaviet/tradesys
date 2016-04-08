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
	for (int t_i = 0; t_i < optitemnum; t_i++)
	{
		clearoptitem(t_i);
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

int cwinopt::notinoptitem(struct optitem* v_optitem)
{
	int t_rt = 1;
	for (int t_i = 0; t_i < optitemnum; t_i++)
	{
		if (m_optitem[t_i].code == v_optitem->code)
		{
			t_rt = 0;
		}
	}
	return t_rt;
}

int cwinopt::clearoptitem(int v_i)
{
	m_optitem[v_i].code = 0;
	m_optitem[v_i].price = 0;
	m_optitem[v_i].opt = 0;
	return 0;
}

int cwinopt::appendoptitem(struct optitem* v_optitem)
{
	if (notinoptitem(v_optitem))
	{
		for (int t_i = 0; t_i < optitemnum; t_i++)
		{
			if (m_optitem[t_i].code == 0)
			{
				m_optitem[t_i].code = v_optitem->code;
				m_optitem[t_i].price = v_optitem->price;
				m_optitem[t_i].opt = v_optitem->opt;
				break;
			}
		}
	}
	free(v_optitem);
	return 0;
}

int cwinopt::actkeyboard(int v_key, int v_act)
{
	keybd_event(v_key, 0, v_act, 0);	//VK_LWIN,KEYEVENTF_KEYUP
	return 0;
}

int cwinopt::optactkeyboard(struct optact v_optact)
{
	switch (v_optact.act)
	{
	case optact_kc:
		actkeyboard(v_optact.x, 0);
		actkeyboard(v_optact.x, KEYEVENTF_KEYUP);
		break;
	case optact_kd:
		actkeyboard(v_optact.x, 0);
		break;
	case optact_ku:
		actkeyboard(v_optact.x, KEYEVENTF_KEYUP);
		break;
	default:
		break;
	}
	return 0;
}

int cwinopt::actmouse(int v_act)
{
	mouse_event(v_act, 0, 0, 0, 0);
	return 0;
}

int cwinopt::optactmouse(struct optact v_optact)
{
	SetCursorPos(v_optact.x, v_optact.y);
	switch (v_optact.act)
	{
	case optact_lc:
		actmouse(MOUSEEVENTF_LEFTDOWN);
		actmouse(MOUSEEVENTF_LEFTUP);
		break;
	case optact_ld:
		actmouse(MOUSEEVENTF_LEFTDOWN);
		break;
	case optact_lu:
		actmouse(MOUSEEVENTF_LEFTUP);
		break;
	case optact_rd:
		actmouse(MOUSEEVENTF_RIGHTDOWN);
		break;
	case optact_ru:
		actmouse(MOUSEEVENTF_RIGHTUP);
		break;
	default:
		break;
	}
	return 0;
}

int cwinopt::optact(struct optact v_optact)
{
	switch (v_optact.type)
	{
	case optact_keyboard:
		optactkeyboard(v_optact);
		Sleep(optact_delay);
		break;
	case optact_mouse:
		optactmouse(v_optact);
		Sleep(optact_delay);
		break;
	case optact_sleep:
		Sleep(v_optact.x);
		break;
	default:
		break;
	}
	return 0;
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
		case WM_winoptappendoptitem:
			appendoptitem((struct optitem*)t_msg.wParam);
			break;
		case WM_winoptgotest:
			winoptactive(t_msg.wParam);
			break;
		default:
			break;
		}
	}
	return CWinThread::Run();
}