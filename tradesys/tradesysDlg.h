
// tradesysDlg.h : ͷ�ļ�
//

#pragma once

#include "config.h"
#include "afxsock.h"
#include "afxwin.h"
#include "../sqlite3_lib/sqlite3.h"

// CtradesysDlg �Ի���
class CtradesysDlg : public CDialogEx
{
// ����
public:
	CtradesysDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TRADESYS_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��

// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

private:
	BOOL m_sysenable;
	CString m_ipdaddress;
	CString m_ipsaddress;
	char m_buff[BUFFSIZE];
	SOCKET m_udpsocket;
	SOCKADDR_IN m_socketaddr;
	int initudpsocket();
	int closeudpsocket();
	int udpsendto(CString v_cs);
	CString udprecvfrom();
	int rxbuffok();
	CString chars2cstring(char v_buff[BUFFSIZE]);
	int cstring2chars(CString v_cstring);
	int infosysrun();
	int m_listcount;
	int msg2list(CString v_cstring);
	CString m_rxbuff;
	int m_msglistactiveitem;
	void data2view(CString v_cs);
	CString view2in(void);
	CString view2out(void);
	int initsqlite3();
	CString getdatetime();
	void updatedb();
	int status_cstring2int(CString v_cs);
	CString status_db2view(CString v_cs);

public:
	afx_msg void OnBnClickedsysenable();

protected:
	afx_msg LRESULT OnSysenable(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnSysdisable(WPARAM wParam, LPARAM lParam);
	afx_msg void OnTimer(UINT_PTR nIDEvent);

public:
	CListCtrl m_msglist;
	CString m_status;
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	CString m_msgcount;
	afx_msg void OnRclickMsglist(NMHDR *pNMHDR, LRESULT *pResult);
	CString m_in;
	CString m_out;
	CString m_dbpath;
	sqlite3 *m_db;
	afx_msg void OnClickMsglist(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnMenuIn();
	afx_msg void OnMenuOut();
	afx_msg void OnBnClickedbtinput();
	afx_msg void OnBnClickedbtoutput();
	CString m_investment;
	CString m_optok;
};
