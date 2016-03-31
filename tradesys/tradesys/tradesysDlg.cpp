
// tradesysDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "tradesys.h"
#include "tradesysDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CtradesysDlg �Ի���

CtradesysDlg::CtradesysDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_TRADESYS_DIALOG, pParent)
	, m_udpsocket(0)
	, m_ipsaddress(_T(""))
	, m_ipdaddress(_T(""))
	, m_sysenable(FALSE)
	, m_status(_T(""))
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
}

BEGIN_MESSAGE_MAP(CtradesysDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_sysenable, &CtradesysDlg::OnBnClickedsysenable)
	ON_MESSAGE(WM_sysenable, &CtradesysDlg::OnSysenable)
	ON_MESSAGE(WM_sysdisable, &CtradesysDlg::OnSysdisable)
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()

// CtradesysDlg ��Ϣ�������

BOOL CtradesysDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	m_ipsaddress = _T("192.168.255.1");
	m_ipdaddress = _T("192.168.255.128");
	UpdateData(FALSE);
	memset(m_buff, 0, BUFFSIZE);
	m_msglist.SetExtendedStyle((m_msglist.GetExtendedStyle()) | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	m_msglist.InsertColumn(1, _T("code"), LVCFMT_LEFT, LIST1WIDTH);
	m_msglist.InsertColumn(2, _T("value"), LVCFMT_LEFT, LIST2WIDTH);
	m_msglist.InsertColumn(3, _T("volume"), LVCFMT_LEFT, LIST3WIDTH);
	m_msglist.InsertColumn(4, _T("status"), LVCFMT_LEFT, LIST4WIDTH);

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CtradesysDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
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
	// ��ʼ��udp
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

int CtradesysDlg::udpsendto(char* v_char,int v_len)
{
	sendto(m_udpsocket, v_char, v_len, 0, (SOCKADDR*)&m_socketaddr, sizeof(SOCKADDR));
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
	// select ����
	int t_iRet = 0;
	int t_ifds = 0;
	fd_set t_readfds;
	fd_set FAR *t_pwritefds = NULL;
	fd_set FAR *t_pexceptfds = NULL;
	struct timeval t_timeout;
	// ��ʱʱ��1����
	t_timeout.tv_sec = 0;
	t_timeout.tv_usec = 1000;
	t_readfds.fd_count = 1;
	t_readfds.fd_array[0] = m_udpsocket;  //SOCKET���
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
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	UpdateData(TRUE);
	if (m_sysenable)
	{
		if (!initudpsocket())
		{
			m_sysenable = FALSE;
			UpdateData(FALSE);
			closeudpsocket();
		}
		PostMessage(WM_sysenable, 0, 0);
	}
	else
	{
		PostMessage(WM_sysdisable, 0, 0);
	}
}

afx_msg LRESULT CtradesysDlg::OnSysenable(WPARAM wParam, LPARAM lParam)
{
	//AfxMessageBox(_T("OnSysenable"));
	CString t_cs = udprecvfrom();
	if (t_cs.GetLength()>0)
	{

	}
	PostMessage(WM_sysenable, 0, 0);
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
	for (int t_i = 0; t_i<100; t_i++)
	{
		m_msglist.InsertItem(t_i, _T("0"));
		m_msglist.SetItemText(t_i,1,_T("1"));
		m_msglist.SetItemText(t_i,2,  _T("2"));
		m_msglist.SetItemText(t_i, 3,_T("3"));
		m_msglist.SetItemText(1, 1, _T("411111111111111"));
	}
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	UpdateData(TRUE);
	CPoint t_opt;
	GetCursorPos(&t_opt);
	CString t_cs;
	t_cs.Format(_T("%d, %d"), t_opt.x, t_opt.y);
	m_status = t_cs;
	UpdateData(FALSE);
	CDialogEx::OnMouseMove(nFlags, point);
}
