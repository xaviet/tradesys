
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
#define LISTINWT _T("in_waiting")
#define LISTINOK _T("in_ok")
#define LISTOUTWT _T("out_waiting")
#define LISTOUTOK _T("out_ok")
#define WINLIMIT 1.0213
#define puteveryday _T("10000")

#define WM_sysenable	(WM_USER+1)
#define WM_sysdisable	(WM_USER+2)
#define TIMER1S	(WM_USER+3)
#define TIMER50MS	(WM_USER+4)