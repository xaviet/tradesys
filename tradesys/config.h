
// config.h : 基本配置文件
//

#define UDPPORT	32674 //829+727+1118+30000=32674
#define BUFFSIZE	65535

#define LIST1WIDTH	64
#define LIST2WIDTH	96
#define LIST3WIDTH	80
#define LIST4WIDTH	60
#define LIST5WIDTH	60
#define LIST6WIDTH	80
#define LIST7WIDTH	160
#define LISTINWT	_T("in_waiting")
#define LISTINOK	_T("in_ok")
#define LISTOUTWT	_T("out_waiting")
#define LISTOUTOK	_T("out_ok")
#define WINLIMIT	1.0213
#define puteveryday	_T("10000")
#define maxeverybuy	2000

#define WM_sysenable	(WM_USER+1)
#define WM_sysdisable	(WM_USER+2)

#define TIMER1S	(WM_USER+101)
#define TIMER50MS	(WM_USER+102)

#define WM_winoptactive	(WM_USER+201)
#define WM_winoptrun	(WM_USER+202)

struct optitem
{
	int code;
	double price;	//0:current,more than 0:special
	int opt;	//0:none,1:inwt,2:inok,3:outwt,4:outok,127:ok
};
#define optitemnum	2048