#include "GameEngine.h"
#include "resource.h"		//包含资源文件
#include<stdio.h>
#include<stdlib.h>
#include<time.h>
GameEngine *	g_pGE;		//全局引擎指针
/////////////////////////////////////////////////////

//定义状态
enum SELECTSTATE{INITSELECT=0,   //选择状态初始化
				 NOSELECT,       //未选择
				 SELECTED,     //已选择
				 SELECTOK     //正确匹配
				};
const int NULLPATTERN=-1;

const int CARDWIDTH=100;
const int CARDHEIGHT=100;
const int ROWS=3;
const int COLS=4;
const int ROWSPACE=20;
const int COLSPACE=50;
const int CARSTARTX=120;
const int CARSTARTY=200;

/*定义游戏全局变量*/
char *g_strTitle="卡片对对碰";

int g_nCardPattern[ROWS][COLS];
RECT g_rCardRect[ROWS][COLS];
SELECTSTATE g_nCardState[ROWS][COLS];

int g_nSelectRow1,g_nSelectRow2,g_nSelectCol1,g_nSelectCol2;
int g_nSelected;

RECT g_rTimeBarFrame;		//时间条边框矩形大小
int g_nTimeBarLength;		//时间条长度
int g_nTimeCount;			//规定时间长度
int g_nStartTime;			//起始时间
long g_nTimeDelayStart;		//延时启动时间



long g_lPreTime;			//上次循环时间
int g_nFrequency;			//频率


/*  游戏初始化 */
BOOL GameInitialize(HINSTANCE hInstance)
{
	
	//创建引擎
	g_pGE=new GameEngine(hInstance,TEXT("War"),g_strTitle,
			IDI_ICON1,IDI_ICON2,FALSE,32,800,600);
	if(g_pGE==NULL)
		return false;

	//初始化引擎参数
	g_pGE->SetFrameDelay(15) ;		//设置游戏帧延时为15毫秒
	g_pGE->SetPause(false);			//设置暂停状态为false
	if(!g_pGE->CreateGameWindow())	//创建游戏窗口
	{
		MessageBox(NULL,"修改显示模式失败!\r\n程序退出!","错误",MB_OK| MB_ICONERROR);
		return FALSE;		//退出程序
	}

	return true;
}

/* 游戏逻辑 */
void GameAction(HWND hWnd)
{
	/*计算游戏经过的时间和时间条长度*/
	long nCurrentTimes=timeGetTime()-g_nStartTime;
	if(nCurrentTimes<g_nTimeCount)
		g_nTimeBarLength=nCurrentTimes*596/g_nTimeCount;

	/*如果2张被单击过，判断2张卡片图案相同否？*/
	if(g_nSelectRow2!=-1)
	{
		if(g_nCardPattern[g_nSelectRow1][g_nSelectCol1]==g_nCardPattern[g_nSelectRow2]
			[g_nSelectCol2])
		{
			g_nCardState[g_nSelectRow1][g_nSelectCol1]=SELECTOK;
			g_nCardState[g_nSelectRow2][g_nSelectCol2]=SELECTOK;

			g_nCardPattern[g_nSelectRow1][g_nSelectCol1]=NULLPATTERN;
			g_nCardPattern[g_nSelectRow2][g_nSelectCol2]=NULLPATTERN;

			g_nSelected++;
			g_nSelectRow1=g_nSelectCol1=g_nSelectRow2=g_nSelectCol2=-1;

		}
		else
		{
			if(timeGetTime()-g_nTimeDelayStart>200)
			{
				g_nCardState[g_nSelectRow1][g_nSelectCol1]=NOSELECT;
				g_nCardState[g_nSelectRow2][g_nSelectCol2]=NOSELECT;

			
				g_nSelectRow1=g_nSelectCol1=g_nSelectRow2=g_nSelectCol2=-1;
			}
		}
	}





	long nTick=timeGetTime();//获取当前时间
	g_nFrequency=1000/(nTick-g_lPreTime);
	InvalidateRect(hWnd,NULL,FALSE);	//强制刷新窗口
	UpdateWindow(hWnd);

	/*检查游戏时间是否超出规定时间*/
	if(nCurrentTimes<g_nTimeCount)
	{
		if(g_nSelected==6)
		{
			if(MessageBox(g_pGE->GetWindow(),"Successfully!\r\n continue?","You Win!",
				MB_YESNO|MB_DEFBUTTON2|MB_ICONASTERISK)==IDYES)
				GameStart(hWnd);
			else DestroyWindow(hWnd);
		}
	}
	else
	{
		if(MessageBox(g_pGE->GetWindow(),"Excused Limited Time!\r\n continue?","You Fail!",
				MB_YESNO|MB_DEFBUTTON2|MB_ICONASTERISK)==IDYES)
				GameStart(hWnd);
		else DestroyWindow(hWnd);
	}
}


/*  渲染游戏画面 ***/
void GamePaint(HDC hDC)
{

	


	HDC dc=CreateCompatibleDC(hDC);
	HBITMAP hbm=(HBITMAP)LoadImage(g_pGE->GetInstance(),MAKEINTRESOURCE(IDB_BITMAP1),
		IMAGE_BITMAP,800,600,LR_DEFAULTCOLOR);
	SelectObject(dc,hbm);

	BitBlt(dc,0,0,800,600,dc,0,0,SRCCOPY);

	DeleteObject(SelectObject(dc,hbm));
	DeleteObject(hbm);

	SetBkMode(hDC,TRANSPARENT);  //设置背景为透明
	SetTextColor(hDC,RGB(255,255,0)); //设置字体为黄色
	char str[100];
	int nGameTime=timeGetTime()-g_pGE->GetStartTime(); //游戏运行时间
	int nBits=GetDeviceCaps(hDC,BITSPIXEL);      //获取屏幕颜色位数
	int nScreenWidth=GetSystemMetrics(SM_CXSCREEN);    //获取屏幕宽度
	int nScreenHeight=GetSystemMetrics(SM_CYSCREEN);    //获取屏幕高度

	sprintf(str,"游戏运行时间： %d ms   帧频： %d   显示模式： %d bit,%d*%d",
		nGameTime,g_nFrequency,nBits,nScreenWidth,nScreenHeight);

	TextOut(hDC,10,10,str,strlen(str));

	DeleteDC(dc);
	/************************************************************************/
	/*        后面         添加                                                              */
	/************************************************************************/
/* 绘制窗口底色 */
	/*RECT rBG;
	GetClientRect(g_pGE->GetWindow(), &rBG);		//获取窗口矩形范围	
	HBRUSH hbrBG=(HBRUSH)GetStockObject(WHITE_BRUSH);	//选用标准白色画刷
	FillRect(hDC,&rBG,hbrBG);		//绘制背景
	DeleteObject(hbrBG);			//删除画刷

	/* 绘制标题 */
	LOGFONT lf={0};			//创建默认逻辑字体
	lf.lfHeight=80;			//设置字体高度
	lf.lfWidth=60;				//设置字体宽度
	lf.lfWeight=800;			//设置字体粗细
	lf.lfCharSet=DEFAULT_CHARSET;		//设置字符集
	strcpy(lf.lfFaceName,"黑体");			//设置字体	
	HFONT hFont=CreateFontIndirect(&lf);	//创建字体	
	HFONT hFontOld=(HFONT)SelectObject(hDC,hFont);	//选用字体
	TextOut(hDC,100,30,g_strTitle,strlen(g_strTitle));	//绘制文本	

	SelectObject(hDC,hFontOld);			//替换字体
	DeleteObject(hFont);				//删除字体




	/* 绘制时间条 */
	HBRUSH hbr=CreateSolidBrush(RGB(128,128,128));	//创建画刷
	HBRUSH hbrOld=(HBRUSH)SelectObject(hDC,hbr);	//选用画刷
	FrameRect(hDC,&g_rTimeBarFrame,hbr);				//绘制边框
	//绘制时间条
	Rectangle(hDC,g_rTimeBarFrame.left+2,g_rTimeBarFrame.top+2,g_rTimeBarFrame.left+2+g_nTimeBarLength,g_rTimeBarFrame.bottom-2);
	SelectObject(hDC,hbrOld);		//替出画刷				

	/* 绘制卡片 */
	//创建卡片背面画刷
	HBRUSH hbrBK=CreateHatchBrush(HS_BDIAGONAL,RGB(128,128,128)); 
	HBRUSH hbrCard;				//定义卡片正面画刷
	RECT r;						//定义卡片绘图矩形范围

	for(int i=0;i<ROWS;i++)
	{
		for(int j=0;j<COLS;j++)
		{
			FrameRect(hDC,&g_rCardRect[i][j],hbr);//	绘制卡片边框	
			r=g_rCardRect[i][j];			//设置卡片图案矩形范围

			/* 判断卡片是否未被单击选择*/
			if(g_nCardState[i][j]==NOSELECT)	//
			{		
				/*未被单击选择,绘制背面图案*/
				InflateRect(&r,-10,-10);	//缩小矩形	
				FillRect(hDC,&r,hbrBK);		//绘制用背面图案画刷填充的矩形
			}
			else
			{	/*被单击选择,绘制正面图案*/
				InflateRect(&r,-20,-20);	//缩小矩形	
				switch(g_nCardPattern[i][j])	//选择绘制的指定的图案
				{
				case 0:
					hbrCard=CreateSolidBrush(RGB(128,255,0));	//创建画刷
					SelectObject(hDC,hbrCard);					//选用画刷
					RoundRect(hDC,r.left,r.top,r.right,r.bottom, 10,10);	//绘制圆角矩形
					SelectObject(hDC,hbrOld);		//替出画刷			

	
					break;

				case 1:
					hbrCard=CreateSolidBrush(RGB(255,128,255));
					SelectObject(hDC,hbrCard);		
					RoundRect(hDC,r.left,r.top,r.right,r.bottom, 10,10);	//绘制圆角矩形
					SelectObject(hDC,hbrOld);					
					break;

				case 2:
					hbrCard=CreateSolidBrush(RGB(255,128,128));
					SelectObject(hDC,hbrCard);		
					Rectangle(hDC,r.left,r.top,r.right,r.bottom);	//绘制矩形
					SelectObject(hDC,hbrOld);						
					break;

				case 3:
					hbrCard=CreateSolidBrush(RGB(0,128,128));
					SelectObject(hDC,hbrCard);		
					Rectangle(hDC,r.left,r.top,r.right,r.bottom);	//绘制矩形
					SelectObject(hDC,hbrOld);					
					break;

				case 4:
					hbrCard=CreateSolidBrush(RGB(128,0,255));
					SelectObject(hDC,hbrCard);		
					Ellipse(hDC,r.left,r.top,r.right,r.bottom);		//绘制圆形
					SelectObject(hDC,hbrOld);			
					break;

				case 5:
					hbrCard=CreateSolidBrush(RGB(255,0,128));
					SelectObject(hDC,hbrCard);		
					Ellipse(hDC,r.left,r.top,r.right,r.bottom);		//绘制圆形
					SelectObject(hDC,hbrOld);			
					break;

				default:
					/* 卡片图案为空,绘制白色填充矩形	*/
					//选用标准白色画刷
					hbrCard=(HBRUSH)GetStockObject(WHITE_BRUSH);	
					FillRect(hDC,&g_rCardRect[i][j],hbrCard);		//绘制白色矩形	
					break;	
				}
				DeleteObject(hbrCard);			//删除画刷	
			}
		}
	}
	
	//BitBlt(hDC,0,0,g_pGE->GetWidth(),g_pGE->GetHeight(),hMemDC,0,0,SRCCOPY);

	/*清理GDI对象*/
	DeleteObject(hbrBK);
	DeleteObject(hbr);

}

/* 游戏开始 */
void GameStart(HWND hWnd)
{
	/* 设置鼠标指针*/
	long lCur=(long)LoadCursor(g_pGE->GetInstance(),MAKEINTRESOURCE(IDC_CURSOR1));
	SetClassLong(hWnd, GCL_HCURSOR,lCur);

/*************************分割线*******************************/
	/*循环设置卡片矩形和初始化卡片选择状态*/
	int i,j,n,m;
	for(i=0;i<ROWS;++i)
	{
		for(j=0;j<COLS;++j)
		{
			g_rCardRect[i][j].left=CARSTARTX+(CARDWIDTH+COLSPACE)*j;
			g_rCardRect[i][j].right=g_rCardRect[i][j].left+CARDWIDTH;
			g_rCardRect[i][j].top=CARSTARTY+(CARDHEIGHT+ROWSPACE)*i;
			g_rCardRect[i][j].bottom=g_rCardRect[i][j].top+CARDHEIGHT;

			g_nCardState[i][j]=INITSELECT;
		}
	}

	srand((unsigned)time(NULL));
	for(i=0;i<2;++i)
	{
		for(j=0;j<ROWS*COLS/2;++j)
		{
			do
			{
				m=rand()%ROWS;
				n=rand()%COLS;
			}while(g_nCardState[m][n]!=INITSELECT);
			g_nCardPattern[m][n]=j;
			g_nCardState[m][n]=NOSELECT;
		}
	}
	 g_nStartTime=timeGetTime();
	 g_nTimeCount=20000;

	 SetRect(&g_rTimeBarFrame,100,150,700,170);
	 g_nTimeBarLength=0;

	 //初始化被单击的行，列号
	 g_nSelectRow1=g_nSelectRow2=g_nSelectCol1=g_nSelectCol2=-1; 
	 g_nSelected=0;
}


/* 游戏结束 */
void GameEnd()
{
	//释放位图资源
	

	delete g_pGE;	//清除游戏引擎指针	
}

/* 游戏激活 */
void GameActive(HWND hWnd)
{
	g_pGE->SetPause(FALSE);	

}

/* 游戏暂停 */
void GamePause(HWND hWnd)
{
	g_pGE->SetPause(TRUE);
}

/* 游戏窗口关闭 */
BOOL GameWindowClose(HWND hWnd)
{
	if(MessageBox(g_pGE->GetWindow(),"你确定要退出吗？","退出游戏",MB_YESNO|MB_DEFBUTTON2|MB_ICONASTERISK)==IDYES)
		return TRUE;
	else
		return FALSE;
}

/* 鼠标单击事件处理 */
void MouseClick(HWND hWnd,int x,int y,WPARAM wParam)
{
	//g_nDialigIndex++;		//对话序号递增
	POINT pt={x,y};
	for(int i=0;i<ROWS;++i)
	{
		for(int j=0;j<COLS;++j)
		{
			if(PtInRect(&g_rCardRect[i][j],pt))
			{
				if(g_nCardState[i][j]==NOSELECT&&g_nSelectRow2==-1)
				{
					if(g_nSelectCol1==-1)
					{
						g_nSelectRow1=i;
						g_nSelectCol1=j;
					}
					else
					{
						g_nSelectRow2=i;
						g_nSelectCol2=j;
						g_nTimeDelayStart=timeGetTime();
					}
					g_nCardState[i][j]=SELECTED;
					return;
				}
			}
		}
	}
}