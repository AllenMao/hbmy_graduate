#include<time.h>
#include "Game.h"



int WINAPI WinMain(HINSTANCE,HINSTANCE,LPSTR,int);
LRESULT WINAPI WndProc(HWND,UINT,WPARAM,LPARAM);

int WINAPI _tWinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nShowCmd)
//实例句柄//前一实例句柄 //指向命令参数指针 //窗口的显示状态
{
	srand((unsigned)time(NULL));



	WNDCLASS wc;//窗口类
	if(!hPrevInstance)
	{
		wc.style=CS_HREDRAW|CS_VREDRAW;//垂直、水平重绘
		//WndProc为函数名
		wc.lpfnWndProc=WndProc;//指定了这一类型窗口的过程函数（回调函数），指定自己专用的函数
		
		//额外内存  一般不需要 所以为0
		wc.cbClsExtra=0;
		wc.cbWndExtra=0;

		wc.hInstance=hInstance;
		wc.hIcon=LoadIcon(NULL,IDI_APPLICATION);//标准图标即第一个参数赋为NULL，窗口左上角图标
		wc.hCursor=LoadCursor(NULL,IDC_ARROW); //光标，同上
		wc.hbrBackground=(HBRUSH)GetStockObject(WHITE_BRUSH);//画刷句柄(窗口背景)
		wc.lpszMenuName=NULL;
		wc.lpszClassName="Sdk 实例";
		//注册窗口
		RegisterClass(&wc);
	}


	
	HWND hWnd;//主窗口句柄
	hWnd=CreateWindow("Sdk 实例",              //窗口类名
					"Win32 SDK 应用程序实例", //窗口标题
					WS_OVERLAPPEDWINDOW&(~WS_MAXIMIZEBOX),      //窗口格式
					//WS_OVERLAPPEDWINDOW|WS_VSCROLL|WS_HSCROLL,
					CW_USEDEFAULT,            //初始化x坐标
					CW_USEDEFAULT,            //初始化y坐标
					1280,			  //初始化窗口宽度
					720,				//初始化窗口高度
					NULL,						//父窗口句柄
					NULL,						//窗口菜单句柄
					hInstance,					//程序实例句柄
					NULL);	
	
	if(!hWnd) return FALSE;
	//创建参数
	//显示窗口
	ShowWindow(hWnd,nShowCmd);//nShowCmd  SW_SHOW
	//更新主窗口客户区
	UpdateWindow(hWnd);//一般不用


	MSG msg;//窗口消息
	//开始消息循环
	while(GetMessage(&msg,NULL,0,0))
	{
		TranslateMessage(&msg);  //对消息队进行转换 -》WM_CHAR型
		DispatchMessage(&msg);   //再次将msg分发给OS，调用窗口过程即WndProc
	}
	return msg.wParam;
}


LRESULT WINAPI WndProc(HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam)//函数名消息小写了P
{
	static CGame *game=NULL;


	static HDC hBgDC;
	static HDC hManDC;
	static BITMAP bm;
	static HBITMAP hBitmap;


	HDC hDC;
	PAINTSTRUCT ps;
	
	switch(msg)
	{
	case WM_CREATE:
		{
			/*hDC=GetDC(hWnd);
			
			hBgDC=CreateCompatibleDC(hDC);//创建背景内存DC
			hBitmap=(HBITMAP) LoadImage(NULL,"1.bmp",IMAGE_BITMAP,0,0,LR_LOADFROMFILE|LR_DEFAULTSIZE);
			SelectObject(hBgDC,hBitmap);
			ReleaseDC(hWnd,hDC);
			
			GetObject(hBitmap,sizeof(BITMAP),&bm);*/

			game=new CGame(hWnd);
			int xFrame=GetSystemMetrics(SM_CXSIZEFRAME);//边宽宽度
			int yFrame=GetSystemMetrics(SM_CYSIZEFRAME);
			int yCaption=GetSystemMetrics(SM_CYCAPTION);//标题高度
			int yMenu=GetSystemMetrics(SM_CYMENU);//菜单高度
			int xScreen=GetSystemMetrics(SM_CXSCREEN);
			int yScreen=GetSystemMetrics(SM_CYSCREEN);
			
			int cx=xFrame+BGWIDTH+xFrame;
			int cy=yFrame+yCaption+BGHEIGHT+yFrame;

			SetWindowPos(hWnd,HWND_TOP,
						(xScreen-cx)/2,(yScreen-cy)/2,
						cx,cy,
						SWP_SHOWWINDOW);
			
			return 0;
		}

	case WM_TIMER:
		{
			game->OnTimer();
			InvalidateRect(hWnd,NULL,FALSE);
		}

	case WM_KEYUP:
		{
			game->OnKeyUp(wParam);
			InvalidateRect(hWnd,NULL,FALSE);
			return 0;
		}

	case WM_PAINT://窗口重绘就会出现这个消息
		{
			hDC=BeginPaint(hWnd,&ps);

			game->OnPaint(hDC);
		
			EndPaint(hWnd,&ps);
			break;
		}
		
	case WM_DESTROY:
		{
			KillTimer(hWnd,IDTIMER);
			delete game;

			PostQuitMessage(0);//将一个WM_QUIT消息插入到程序消息队列中
			break;
		}
	default:
		break;
	}
	return DefWindowProc(hWnd,msg,wParam,lParam);//对OS进行缺省处理
}
