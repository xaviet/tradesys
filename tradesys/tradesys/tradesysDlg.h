
// tradesysDlg.h : 头文件
//

#pragma once

#include "config.h"
#include "afxsock.h"
#include "afxwin.h"

// CtradesysDlg 对话框
class CtradesysDlg : public CDialogEx
{
// 构造
public:
	CtradesysDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TRADESYS_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持

// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
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
};
