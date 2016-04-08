 
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
	, m_investment(_T(""))
	, m_optok(_T(""))
	, m_pwinopt(0)
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
	DDX_Text(pDX, IDC_Investment, m_investment);
	DDX_Text(pDX, IDC_optok, m_optok);
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
	ON_NOTIFY(NM_CLICK, IDC_msglist, &CtradesysDlg::OnClickMsglist)
	ON_COMMAND(ID_MENU_IN, &CtradesysDlg::OnMenuIn)
	ON_COMMAND(ID_MENU_OUT, &CtradesysDlg::OnMenuOut)
	ON_BN_CLICKED(IDC_btinput, &CtradesysDlg::OnBnClickedbtinput)
	ON_BN_CLICKED(IDC_btoutput, &CtradesysDlg::OnBnClickedbtoutput)
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
	m_ipsaddress = saddress;
	m_ipdaddress = daddress;
	m_investment = puteveryday;
	m_optok = _T("0");
	UpdateData(FALSE);
	memset(m_buff, 0, BUFFSIZE);
	m_msglist.SetExtendedStyle((m_msglist.GetExtendedStyle()) | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	m_msglist.InsertColumn(1, _T("code"), LVCFMT_LEFT, LIST1WIDTH);
	m_msglist.InsertColumn(2, _T("value"), LVCFMT_LEFT, LIST2WIDTH);
	m_msglist.InsertColumn(3, _T("volume"), LVCFMT_LEFT, LIST3WIDTH);
	m_msglist.InsertColumn(4, _T("input"), LVCFMT_LEFT, LIST4WIDTH);
	m_msglist.InsertColumn(5, _T("output"), LVCFMT_LEFT, LIST5WIDTH);
	m_msglist.InsertColumn(6, _T("status"), LVCFMT_LEFT, LIST6WIDTH);
	m_msglist.InsertColumn(7, _T("datetime"), LVCFMT_LEFT, LIST7WIDTH);
	SetTimer(TIMER1S, 1000, NULL);
	SetTimer(TIMER50MS, 50, NULL);

	initwinopt();
	initsqlite3();

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

int CtradesysDlg::initwinopt()
{
	m_pwinopt = AfxBeginThread(RUNTIME_CLASS(cwinopt), THREAD_PRIORITY_NORMAL, 0, CREATE_SUSPENDED, NULL);
	m_pwinopt->ResumeThread();
	return 0;
}

int CtradesysDlg::initsqlite3()
{
	if (sqlite3_open(CW2A(m_dbpath, CP_UTF8), &m_db) != SQLITE_OK)
	{
		AfxMessageBox(_T("db open error!"));
		return TRUE;
	}
	CString t_cs = _T("create table if not exists opt(datetime integer primary key,code int,volume int,input real,output real,status int);");
	if (sqlite3_exec(m_db, CW2A(t_cs, CP_UTF8), NULL, NULL, NULL) != SQLITE_OK)
	{
		AfxMessageBox(_T("db open error!"));
	}
	int t_nrow = 0, t_ncolumn = 0;
	char **t_dbresult = NULL;
	int t_i = 0;
	sqlite3_get_table(m_db, "select * from opt order by datetime", &t_dbresult, &t_nrow, &t_ncolumn, NULL);
	for (t_i = 1; t_i <= t_nrow; t_i++)
	{
		t_cs = CA2W(t_dbresult[(t_i)*t_ncolumn + 5], CP_UTF8);
		t_cs = status_db2view(t_cs);
		if (t_cs!= LISTOUTOK)
		{
			CString t_code=CA2W(t_dbresult[(t_i)*t_ncolumn + 1]);
			t_code.Format(_T("%06d"),_ttoi(t_code));
			CString t_value = _T("");
			CString t_volume = CA2W(t_dbresult[(t_i)*t_ncolumn + 2], CP_UTF8);
			CString t_input = CA2W(t_dbresult[(t_i)*t_ncolumn + 3], CP_UTF8);
			CString t_output = CA2W(t_dbresult[(t_i)*t_ncolumn + 4], CP_UTF8);
			CString t_status =  t_cs;
			CString t_datetime = CA2W(t_dbresult[(t_i)*t_ncolumn], CP_UTF8);			
			data2msglist(t_code, t_value, t_volume, t_input, t_output, t_status, t_datetime);
		}
	}
	sqlite3_free_table(t_dbresult);
	return 0;
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

int CtradesysDlg::data2msglist(CString v_code, CString v_value, CString v_volume, \
	CString v_input, CString v_output, CString v_status, CString v_datetime)
{
	m_msglist.InsertItem(0, v_code);
	m_msglist.SetItemText(0, 1, v_value);
	m_msglist.SetItemText(0, 2, v_volume);
	m_msglist.SetItemText(0, 3, v_input);
	m_msglist.SetItemText(0, 4, v_output);
	m_msglist.SetItemText(0, 5, v_status);
	m_msglist.SetItemText(0, 6, v_datetime);
	m_listcount++;
	m_msgcount.Format(_T("%d"), m_listcount);	
	if (v_status== LISTRDY)
	{
		struct optitem* t_optitem = new optitem;
		t_optitem->code = _ttoi(v_code);
		if (v_input.GetLength()>0)
		{
			t_optitem->price = _ttof(v_input);
		}
		else
		{
			CString t_in = v_value.Left(v_value.Find(_T("/")));
			t_in.Format(_T("%.2f"), _ttof(t_in) - maxeverybuy*0.00001);
			t_optitem->price = _ttof(t_in);
		}
		t_optitem->opt = 0;
		m_pwinopt->PostThreadMessage(WM_appendoptitem, (WPARAM)t_optitem, 0);
		m_msglist.SetItemText(0, 5, LISTINWT);
	}
	UpdateData(FALSE);
	return 0;
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
		m_pwinopt->PostThreadMessage(WM_winoptactive, 1, 0);
	}
	else
	{
		PostMessage(WM_sysdisable, 0, 0);
		m_pwinopt->PostThreadMessage(WM_winoptactive, 0, 0);
	}
}

int CtradesysDlg::msg2list(CString v_cstring)
{
	CString t_cs= v_cstring;
	while (t_cs.GetLength()>0)
	{
		CString t_tempcs = t_cs.Left(t_cs.Find(_T(',')));
		CString t_code = t_tempcs.Right(t_tempcs.GetLength() - 1);	
		t_cs = t_cs.Right(t_cs.GetLength()- t_tempcs.GetLength() -1);
		t_tempcs = t_cs.Left(t_cs.Find(_T(',')));
		CString t_value = t_tempcs;
		t_cs = t_cs.Right(t_cs.GetLength() - t_tempcs.GetLength() - 1);
		t_tempcs = t_cs.Left(t_cs.Find(_T(',')));
		CString t_volume = t_tempcs;
		CString t_input = _T("");
		CString t_output = _T("");
		t_cs = t_cs.Right(t_cs.GetLength() - t_tempcs.GetLength() - 1);
		t_tempcs = t_cs.Left(t_cs.Find(_T(']')));
		CString t_status = status_db2view(t_tempcs);
		CString t_datetime = _T("");
		t_cs = t_cs.Right(t_cs.GetLength() - t_tempcs.GetLength() - 1);
		data2msglist(t_code, t_value, t_volume, t_input, t_output, t_status, t_datetime);
	}
	return 0;
}

afx_msg LRESULT CtradesysDlg::OnSysenable(WPARAM wParam, LPARAM lParam)
{
	// messageformat[code,price,volume,successflag]
#ifdef msgloop	
	udpsendto(msgcontent);
#endif
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
		infosysrun();	
		break;
	case TIMER50MS:
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
	if (v_cs.GetLength()>=3)
	{
		m_in = v_cs.Left(v_cs.Find(_T("/")));
		m_in.Format(_T("%.2f"), _ttof(m_in) - maxeverybuy*0.00001);
		m_out = v_cs.Right(v_cs.GetLength() - v_cs.Find(_T("/")) - 1);
	}
	else
	{
		m_in = m_msglist.GetItemText(m_msglistactiveitem, 3);
		m_out.Format(_T("%.2f"),_ttof(m_in)*WINLIMIT+ maxeverybuy*0.00001);
	}
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

void CtradesysDlg::OnBnClickedbtoutput()
{
	// TODO: 在此添加命令处理程序代码
	if (m_msglistactiveitem>=0)
	{
		CString t_cs = view2out();
		m_msglist.SetItemText(m_msglistactiveitem,4, t_cs);
		m_msglist.SetItemText(m_msglistactiveitem, 5, LISTOUTWT);
	}
}


void CtradesysDlg::OnBnClickedbtinput()
{
	// TODO: 在此添加命令处理程序代码
	if (m_msglistactiveitem >= 0)
	{
		CString t_cs = view2in();
		m_msglist.SetItemText(m_msglistactiveitem, 3, t_cs);
		m_msglist.SetItemText(m_msglistactiveitem, 5, LISTINWT);
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


void CtradesysDlg::OnMenuIn()
{
	// TODO: 在此添加命令处理程序代码
	if (m_msglist.GetItemText(m_msglistactiveitem, 5)== LISTINWT)
	{
		m_msglist.SetItemText(m_msglistactiveitem, 5, LISTINOK);
		updatedb();
		double t_d = _ttof(m_investment) - _ttof(m_msglist.GetItemText(m_msglistactiveitem, 2))*_ttof(m_msglist.GetItemText(m_msglistactiveitem, 3));
		m_investment.Format(_T("%f"), t_d);
		UpdateData(FALSE);
	}
}


void CtradesysDlg::OnMenuOut()
{
	// TODO: 在此添加命令处理程序代码
	if (m_msglist.GetItemText(m_msglistactiveitem, 5) == LISTOUTWT)
	{
		m_msglist.SetItemText(m_msglistactiveitem, 5, LISTOUTOK);
		updatedb();
		double t_d = _ttof(m_investment) + _ttof(m_msglist.GetItemText(m_msglistactiveitem, 2))*_ttof(m_msglist.GetItemText(m_msglistactiveitem, 4));
		m_investment.Format(_T("%f"), t_d);
		int t_i = _ttoi(m_optok) + 1;
		m_optok.Format(_T("%d"), t_i);
		UpdateData(FALSE);
	}
}

CString CtradesysDlg::getdatetime()
{
	CString t_cstime;
	SYSTEMTIME	t_sysTime;
	GetLocalTime(&t_sysTime);
	t_cstime.Format(_T("%04ld%02ld%02ld%02ld%02ld%02ld%03ld"), t_sysTime.wYear, t_sysTime.wMonth, t_sysTime.wDay, t_sysTime.wHour, t_sysTime.wMinute, t_sysTime.wSecond, t_sysTime.wMilliseconds);
	return t_cstime;
}

int CtradesysDlg::status_cstring2int(CString v_cs)
{
	int t_rt = -1;
	if (v_cs == LISTRDY)
	{
		t_rt = 0;
	}
	else if (v_cs== LISTINWT)
	{
		t_rt = 1;
	}
	else if (v_cs == LISTINOK)
	{
		t_rt = 2;
	}
	else if (v_cs == LISTOUTWT)
	{
		t_rt = 3;
	}
	else if (v_cs == LISTOUTOK)
	{
		t_rt = 4;
	}
	else
	{
		AfxMessageBox(_T("status_cstring2int error!"));
	}
	return t_rt;
}

CString CtradesysDlg::status_db2view(CString v_cs)
{
	CString t_rt = _T("");
	if (v_cs == _T("0"))
	{
		t_rt = LISTRDY;
	}
	else if (v_cs == _T("1"))
	{
		t_rt = LISTINWT;
	}
	else if (v_cs == _T("2"))
	{
		t_rt = LISTINOK;
	}
	else if (v_cs == _T("3"))
	{
		t_rt = LISTOUTWT;
	}
	else if (v_cs == _T("4"))
	{
		t_rt = LISTOUTOK;
	}
	else
	{
		AfxMessageBox(_T("status_db2view error!"));
	}
	return t_rt;
}

int CtradesysDlg::gotest()
{
	//AfxMessageBox(_T("gotest"));
	m_pwinopt->PostThreadMessageW(WM_winoptgotest,0,0);
	return 0;
}

BOOL CtradesysDlg::PreTranslateMessage(MSG* pMsg)
{
	if (m_hAccTable)
	{
		if (::TranslateAccelerator(m_hWnd, m_hAccTable, pMsg))
		{
			return true;
		}
	}
	// 把Esc和Enter按键事件消息过滤掉，否则该消息会导致对应应用程序调用OnOK（）方法，结束应用程序  
	if (pMsg->message == WM_KEYDOWN)
	{
		switch (pMsg->wParam)
		{
		case VK_ESCAPE: //Esc按键事件
			gotest();
			return true;
		case VK_RETURN: //Enter按键事件  
			return true;
		default:
			break;
		}
	}
	return CDialogEx::PreTranslateMessage(pMsg);
}

void CtradesysDlg::updatedb()
{
	CString t_cs = _T("");
	CString t_datetime = m_msglist.GetItemText(m_msglistactiveitem, 6);
	t_datetime = (t_datetime.GetLength() == 0) ? getdatetime() : t_datetime;
	CString t_in = m_msglist.GetItemText(m_msglistactiveitem, 3);
	t_in = (t_in.GetLength() == 0) ? _T("0") : t_in;
	CString t_out = m_msglist.GetItemText(m_msglistactiveitem, 4);
	t_out = (t_out.GetLength() == 0) ? _T("0") : t_out;
	t_cs.Format(_T("replace into opt(datetime,code,volume,input,output,status) values(%s,%s,%s,%s,%s,%d);"), \
		t_datetime, \
		m_msglist.GetItemText(m_msglistactiveitem, 0), \
		m_msglist.GetItemText(m_msglistactiveitem, 2), \
		t_in, \
		t_out, \
		status_cstring2int(m_msglist.GetItemText(m_msglistactiveitem, 5)));
	if (sqlite3_exec(m_db, CW2A(t_cs, CP_UTF8), NULL, NULL, NULL) != SQLITE_OK)
	{
		AfxMessageBox(_T("replace db error!"));
	}
	m_msglist.SetItemText(m_msglistactiveitem, 6, t_datetime);
}

BOOL CtradesysDlg::OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	if (message == WM_CLOSE)
	{
		AfxMessageBox(_T("CtradesysDlg WM_CLOSE!"));
		if (m_pwinopt != NULL)
		{
			m_pwinopt = NULL;
		}
	}
	return CDialogEx::OnWndMsg(message, wParam, lParam, pResult);
}