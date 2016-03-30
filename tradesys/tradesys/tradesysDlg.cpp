
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
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CtradesysDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_ipaddress, m_ipsaddress);
	DDV_MaxChars(pDX, m_ipsaddress, 32);
	DDX_Check(pDX, IDC_sysenable, m_sysenable);
	DDX_Text(pDX, IDC_ipdaddress, m_ipdaddress);
	DDV_MaxChars(pDX, m_ipdaddress, 32);
}

BEGIN_MESSAGE_MAP(CtradesysDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_sysenable, &CtradesysDlg::OnBnClickedsysenable)
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
	m_ipsaddress = _T("192.168.255.1");
	m_ipdaddress = _T("192.168.255.128");
	UpdateData(FALSE);

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
	m_socketaddr.sin_addr.s_addr = inet_addr(m_buff);
	if (SOCKET_ERROR == bind(m_udpsocket, (SOCKADDR*)&m_socketaddr, sizeof(SOCKADDR)))
	{
		AfxMessageBox(_T("udp socket bind fail"));
		return FALSE;
	}
	// destination ip
	cstring2chars(m_ipdaddress);
	m_socketaddr.sin_addr.s_addr = inet_addr(m_buff);
	return TRUE;
}

int CtradesysDlg::closeudpsocket()
{
	closesocket(m_udpsocket);
	//WSACleanup();
	return 0;
}

int CtradesysDlg::udpsendto(char* v_char,int v_len)
{
	sendto(m_udpsocket, v_char, v_len, 0, (SOCKADDR*)&m_socketaddr, sizeof(SOCKADDR));
	memset(m_buff,0, BUFFSIZE);
	return TRUE;
}

int CtradesysDlg::udprecvfrom()
{
	int t_recvlen = 0;
	if (rxbuffok())
	{
		// udp data
		memset(m_buff, 0, BUFFSIZE);
		int t_addrlen = sizeof(SOCKADDR);
		t_recvlen=recvfrom(m_udpsocket, (char*)m_buff, BUFFSIZE, 0, (SOCKADDR*)&m_socketaddr, &t_addrlen);
	}
	//this->PostThreadMessage(WM_udprx, NULL, NULL);
	return t_recvlen;
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
			closeudpsocket();
		}
		udpsendto(m_buff,strlen(m_buff));
		udprecvfrom();
		AfxMessageBox(chars2cstring(m_buff));
	}
	else
	{
		closeudpsocket();
	}
}
