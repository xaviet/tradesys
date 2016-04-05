 
// tradesysDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "tradesys.h"
#include "tradesysDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CtradesysDlg 对话框

CtradesysDlg::CtradesysDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_TRADESYS_DIALOG, pParent)
	, m_udpsocket(0)
	, m_ipsaddress(_T(""))
	, m_ipdaddress(_T(""))
	, m_sysenable(FALSE)
	, m_status(_T(""))
	, m_listcount(0)
	, m_msgcount(_T(""))
	, m_msglistactiveitem(0)
	, m_in(_T(""))
	, m_out(_T(""))
	, m_dbpath(_T("bv.sdb"))
	, m_db(NULL)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CtradesysDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_ipsaddress, m_ipsaddress);
	DDV_MaxChars(pDX, m_ipsaddress, 32);
	DDX_Check(pDX, IDC_sysenable, m_sysenable);
	DDX_Text(pDX, IDC_ipdaddress, m_ipdaddress);
	DDV_MaxChars(pDX, m_ipdaddress, 32);
	DDX_Control(pDX, IDC_msglist, m_msglist);
	DDX_Text(pDX, IDC_status, m_status);
	DDV_MaxChars(pDX, m_status, 32);
	DDX_Text(pDX, IDC_msgcount, m_msgcount);
	DDX_Text(pDX, IDC_input, m_in);
	DDV_MaxChars(pDX, m_in, 32);
	DDX_Text(pDX, IDC_output, m_out);
	DDV_MaxChars(pDX, m_out, 32);
}

BEGIN_MESSAGE_MAP(CtradesysDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_TIMER()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_sysenable, &CtradesysDlg::OnBnClickedsysenable)
	ON_MESSAGE(WM_sysenable, &CtradesysDlg::OnSysenable)
	ON_MESSAGE(WM_sysdisable, &CtradesysDlg::OnSysdisable)
	ON_WM_MOUSEMOVE()
	ON_NOTIFY(NM_RCLICK, IDC_msglist, &CtradesysDlg::OnRclickMsglist)
	ON_COMMAND(ID_MENU_output, &CtradesysDlg::OnMenuoutput)
	ON_COMMAND(ID_MENU_input, &CtradesysDlg::OnMenuinput)
	ON_NOTIFY(NM_CLICK, IDC_msglist, &CtradesysDlg::OnClickMsglist)
END_MESSAGE_MAP()

// CtradesysDlg 消息处理程序

BOOL CtradesysDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	m_ipsaddress = _T("192.168.137.1");
	m_ipdaddress = _T("192.168.137.33");
	UpdateData(FALSE);
	memset(m_buff, 0, BUFFSIZE);
	m_msglist.SetExtendedStyle((m_msglist.GetExtendedStyle()) | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	m_msglist.InsertColumn(1, _T("code"), LVCFMT_LEFT, LIST1WIDTH);
	m_msglist.InsertColumn(2, _T("value"), LVCFMT_LEFT, LIST2WIDTH);
	m_msglist.InsertColumn(3, _T("volume"), LVCFMT_LEFT, LIST3WIDTH);
	m_msglist.InsertColumn(4, _T("input"), LVCFMT_LEFT, LIST4WIDTH);
	m_msglist.InsertColumn(5, _T("output"), LVCFMT_LEFT, LIST5WIDTH);
	m_msglist.InsertColumn(6, _T("status"), LVCFMT_LEFT, LIST6WIDTH);
	SetTimer(TIMER1S, 1000, NULL);
	SetTimer(TIMER50MS, 50, NULL);

	//this->unicode2char(t_cs,t_ch);
	if (sqlite3_open(CW2A(m_dbpath, CP_UTF8), &m_db) != SQLITE_OK)
	{
		MessageBox(_T("db open error!"));
		return TRUE;
	}

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CtradesysDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CtradesysDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

int CtradesysDlg::initudpsocket()
{
	if (!AfxSocketInit())
	{
		AfxMessageBox(_T("AfxSocketInit fail"));
		return FALSE;
	}
	// 初始化udp
	m_udpsocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (INVALID_SOCKET == m_udpsocket)
	{
		AfxMessageBox(_T("udp socket fail"));
		return FALSE;
	}
	m_socketaddr.sin_family = AF_INET;
	m_socketaddr.sin_port = htons(UDPPORT);
	// source ip
	cstring2chars(m_ipsaddress);
	inet_pton(AF_INET, m_buff, &(m_socketaddr.sin_addr.s_addr));
	if (SOCKET_ERROR == bind(m_udpsocket, (SOCKADDR*)&m_socketaddr, sizeof(SOCKADDR)))
	{
		AfxMessageBox(_T("udp socket bind fail"));
		return FALSE;
	}
	// destination ip
	cstring2chars(m_ipdaddress);
	//m_socketaddr.sin_addr.s_addr = inet_addr(m_buff);
	inet_pton(AF_INET, m_buff, &(m_socketaddr.sin_addr.s_addr));
	return TRUE;
}

int CtradesysDlg::closeudpsocket()
{
	closesocket(m_udpsocket);
	//WSACleanup();
	return 0;
}

int CtradesysDlg::udpsendto(CString v_cs)
{
	cstring2chars(v_cs);
	sendto(m_udpsocket, m_buff, strlen(m_buff), 0, (SOCKADDR*)&m_socketaddr, sizeof(SOCKADDR));
	memset(m_buff,0, BUFFSIZE);
	return 0;
}

CString CtradesysDlg::udprecvfrom()
{
	CString t_cstring = _T("");
	int t_recvlen = 0;
	if (rxbuffok())
	{
		// udp data
		memset(m_buff, 0, BUFFSIZE);
		int t_addrlen = sizeof(SOCKADDR);
		t_recvlen=recvfrom(m_udpsocket, (char*)m_buff, BUFFSIZE, 0, (SOCKADDR*)&m_socketaddr, &t_addrlen);
		if (t_recvlen > 0)
		{
			t_cstring = chars2cstring(m_buff);
			memset(m_buff, 0, BUFFSIZE);
		}
	}
	return t_cstring;
}

int CtradesysDlg::rxbuffok()
{
	// select 参数
	int t_iRet = 0;
	int t_ifds = 0;
	fd_set t_readfds;
	fd_set FAR *t_pwritefds = NULL;
	fd_set FAR *t_pexceptfds = NULL;
	struct timeval t_timeout;
	// 超时时间1毫秒
	t_timeout.tv_sec = 0;
	t_timeout.tv_usec = 1000;
	t_readfds.fd_count = 1;
	t_readfds.fd_array[0] = m_udpsocket;  //SOCKET句柄
	t_iRet = select(t_ifds, &t_readfds, t_pwritefds, t_pexceptfds, &t_timeout);
	return t_iRet;
}

CString CtradesysDlg::chars2cstring(char v_buff[BUFFSIZE])
{
	return CString(v_buff);
}

int CtradesysDlg::cstring2chars(CString v_cstring)
{
	memset(m_buff, 0, BUFFSIZE);
	WideCharToMultiByte(CP_OEMCP, 0, (LPCTSTR)v_cstring, -1, m_buff, BUFFSIZE, 0, false);
	return strlen(m_buff);
}

void CtradesysDlg::OnBnClickedsysenable()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);
	if (m_sysenable)
	{
		if (!initudpsocket())
		{
			m_sysenable = FALSE;
			UpdateData(FALSE);
		}
	}
	else
	{
		PostMessage(WM_sysdisable, 0, 0);
	}
}

int CtradesysDlg::msg2list(CString v_cstring)
{
	CString t_cs= v_cstring;
	while (t_cs.GetLength()>0)
	{
		CString t_tempcs = t_cs.Left(t_cs.Find(_T(',')));
		m_msglist.InsertItem(0, t_tempcs.Right(t_tempcs.GetLength()-1));
		t_cs = t_cs.Right(t_cs.GetLength()- t_tempcs.GetLength() -1);
		t_tempcs = t_cs.Left(t_cs.Find(_T(',')));
		m_msglist.SetItemText(0, 1, t_tempcs);
		t_cs = t_cs.Right(t_cs.GetLength() - t_tempcs.GetLength() - 1);
		t_tempcs = t_cs.Left(t_cs.Find(_T(',')));
		m_msglist.SetItemText(0, 2, t_tempcs);
		t_cs = t_cs.Right(t_cs.GetLength() - t_tempcs.GetLength() - 1);
		t_tempcs = t_cs.Left(t_cs.Find(_T(']')));
		m_msglist.SetItemText(0, 3, t_tempcs);
		t_cs = t_cs.Right(t_cs.GetLength() - t_tempcs.GetLength() - 1);
		m_listcount++;
	}
	m_msgcount.Format(_T("%d"), m_listcount);
	UpdateData(FALSE);
	return 0;
}

afx_msg LRESULT CtradesysDlg::OnSysenable(WPARAM wParam, LPARAM lParam)
{
	// messageformat[code,price,volume,successflag]
	udpsendto(_T("[601066,22.333/4444.22,7777777][600166,2,5.1/9.27,6][600066,9.07/9.27,200]"));
	m_rxbuff = udprecvfrom();
	if (m_rxbuff.GetLength()>0 && m_rxbuff[m_rxbuff.GetLength()-1]==_T(']'))
	{
		msg2list(m_rxbuff);
		m_rxbuff.Empty();
	}
	return 0;
}

afx_msg LRESULT CtradesysDlg::OnSysdisable(WPARAM wParam, LPARAM lParam)
{
	//AfxMessageBox(_T("OnSysdisable"));
	closeudpsocket();
	return 0;
}

void CtradesysDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	UpdateData(TRUE);
	CPoint t_opt;
	GetCursorPos(&t_opt);
	CString t_cs;
	t_cs.Format(_T("%d, %d"), t_opt.x, t_opt.y);
	m_status = t_cs;
	UpdateData(FALSE);
	CDialogEx::OnMouseMove(nFlags, point);
}

void CtradesysDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: ÔÚ´ËÌí¼ÓÏûÏ¢´¦Àí³ÌÐò´úÂëºÍ/»òµ÷ÓÃÄ¬ÈÏÖµ
	switch (nIDEvent)
	{
	case TIMER1S:
		//infosysrun();	
		break;
	case TIMER50MS:
		infosysrun();
		break;
	default:
		break;
	}
	CDialogEx::OnTimer(nIDEvent);
}

int CtradesysDlg::infosysrun()
{
	UpdateData(TRUE);
	if (m_sysenable)
	{
		PostMessage(WM_sysenable, 0, 0);
	}
	return 0;
}

void CtradesysDlg::OnRclickMsglist(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	
	CMenu t_menu;
	if (t_menu.m_hMenu == NULL)
	{
		POINT t_pt = { 0 };
		GetCursorPos(&t_pt);
		t_menu.LoadMenu(IDR_MENU);
		t_menu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN, t_pt.x, t_pt.y, this);
	}
	m_msglistactiveitem = pNMItemActivate->iItem;
	if (m_msglistactiveitem>=0)
	{
		data2view(m_msglist.GetItemText(m_msglistactiveitem, 1));
	}
	*pResult = 0;
}

void CtradesysDlg::data2view(CString v_cs)
{
	m_in = v_cs.Left(v_cs.Find(_T("/")));
	m_out = v_cs.Right(v_cs.GetLength()-v_cs.Find(_T("/"))-1);
	UpdateData(FALSE);
}

CString CtradesysDlg::view2in(void)
{
	UpdateData(TRUE);
	CString t_cs = m_in;
	return t_cs;
}

CString CtradesysDlg::view2out(void)
{
	UpdateData(TRUE);
	CString t_cs = m_out;
	return t_cs;
}

void CtradesysDlg::OnMenuoutput()
{
	// TODO: 在此添加命令处理程序代码
	CString t_cs = _T("");
	t_cs.Format(_T("%d"), m_msglistactiveitem);
	if (m_msglistactiveitem>=0)
	{
		t_cs = view2out();
		m_msglist.SetItemText(m_msglistactiveitem,5, t_cs);
	}
}


void CtradesysDlg::OnMenuinput()
{
	// TODO: 在此添加命令处理程序代码
	CString t_cs = _T("");
	t_cs.Format(_T("%d"), m_msglistactiveitem);
	if (m_msglistactiveitem >= 0)
	{
		t_cs = view2in();
		m_msglist.SetItemText(m_msglistactiveitem, 4, t_cs);
	}
}


void CtradesysDlg::OnClickMsglist(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	m_msglistactiveitem = pNMItemActivate->iItem;
	if (m_msglistactiveitem >= 0)
	{
		data2view(m_msglist.GetItemText(m_msglistactiveitem, 1));
	}
	*pResult = 0;
}
