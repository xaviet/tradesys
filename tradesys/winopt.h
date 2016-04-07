#pragma once

#include "config.h"

// cwinopt

class cwinopt : public CWinThread
{
	DECLARE_DYNCREATE(cwinopt)

protected:
	cwinopt();           // 动态创建所使用的受保护的构造函数
	virtual ~cwinopt();

public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

protected:
	DECLARE_MESSAGE_MAP()

private:
	int Run();
	int m_activeflag;
	int winoptactive(WPARAM v_i);
	int winoptrun();
	struct optitem m_optitem[optitemnum];
	int clearoptitem(int v_i);
	int appendoptitem(struct optitem* v_optitem);
};


