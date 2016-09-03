#include<windows.h>
#include<tchar.h>
#include<stdio.h>



#define BGWIDTH 800
#define BGHEIGHT 600
#define BOXSIDE 30
#define ROWS 19
#define LINES 10
#define MAXBOXSTYLE 5
#define MAXBLOCKSTYLE 7
#define IDTIMER 100


POINTS blockStyle[7][4]={
	{{0,0},{0,-1},{0,1},{0,2}},
	{{0,0},{-1,0},{-1,1},{0,1}},
	{{0,0},{1,0},{0,1},{0,2}},
	{{0,0},{-1,0},{0,1},{0,2}},
	{{0,0},{-1,0},{0,-1},{1,-1}},
	{{0,0},{-1,0},{0,1},{1,1}},
	{{0,0},{-1,0},{0,-1},{1,0}},
};


typedef unsigned int BOXSTYLE;
typedef unsigned int BLOCKSTYLE;

const TCHAR szWindowClass[]="First";
const TCHAR szWindowTitle[]="俄罗斯方块";

class CGame
{
private:
	HWND mhWnd; //窗口句柄
	HDC mhMemDC; //内存DC
	HDC mhBGDC; //背景位图内存DC
	HDC mhBoxDC[5];      //存放位图DC

	  //地图
	BOXSTYLE mMap[ROWS][LINES];
	 //行计数器
	UINT mnRow[ROWS];
	POINTS mCurPos[4];
	BLOCKSTYLE mCurBlockStyle;
	BOXSTYLE mCurBoxStyle;

	BLOCKSTYLE mNextBlockStyle;
	BOXSTYLE mNextBoxStyle;

public:
	CGame(HWND hwnd);
	VOID NewGame(void);
	~CGame(void);

	void OnPaint(HDC hdc);
	void OnTimer(void);
	void OnKeyUp(UINT key);

    BOOL ShiftDown(void);
	BOOL ShiftLeft(void);
	BOOL ShiftRight(void);
	BOOL Rotate(bool clockwise);

	BOOL QuickDown(void);

	BOOL DetectCollision(POINTS p);
	void NewBlock(void);

};

CGame::CGame(HWND hwnd)//加载图片
{
	HBITMAP h;
	mhWnd=hwnd;
	HDC hdc=GetDC(hwnd);

	h=(HBITMAP) LoadImage(NULL,"bg.bmp",IMAGE_BITMAP,0,0,LR_LOADFROMFILE|LR_DEFAULTSIZE);
	mhBGDC=CreateCompatibleDC(hdc);
	SelectObject(mhBGDC,h);
	DeleteObject(h);

		//创建缓存区DC
	mhMemDC=CreateCompatibleDC(hdc);
	h=CreateCompatibleBitmap(hdc,BGWIDTH,BGHEIGHT);
	SelectObject(mhMemDC,h);
	DeleteObject(h);

	for(int i=0;i<MAXBOXSTYLE;++i)
	{
		TCHAR temp[10];

		sprintf(temp,"%d.bmp",i+1);
		h=(HBITMAP) LoadImage(NULL,temp,IMAGE_BITMAP,0,0,LR_LOADFROMFILE|LR_DEFAULTSIZE);
		mhBoxDC[i]=CreateCompatibleDC(hdc);
		SelectObject(mhBoxDC[i],h);
		DeleteObject(h);
	}


	
	ReleaseDC(hwnd,hdc);

	NewGame();
}


CGame::~CGame(void)
{
	DeleteDC(mhBGDC);

	for(int i=0;i<5;++i) DeleteDC(mhBoxDC[i]);
	
	DeleteDC(mhMemDC);
}

void CGame::OnPaint(HDC hdc)
{
	
	//绘制背景
	BitBlt(mhMemDC,0,0,BGWIDTH,BGHEIGHT,mhBGDC,0,0,SRCCOPY);


	static COLORREF penColor=RGB(255,150,150);
	HPEN pen=CreatePen(PS_SOLID,1,penColor);
	HPEN oldPen=(HPEN) SelectObject(mhMemDC,pen);
	
	MoveToEx(mhMemDC,189,0,NULL);
	LineTo(mhMemDC,189,ROWS*BOXSIDE+6);
	LineTo(mhMemDC,490,ROWS*BOXSIDE+6);
	LineTo(mhMemDC,490,0);

	MoveToEx(mhMemDC,600,200,NULL);
	LineTo(mhMemDC,600,360);
	LineTo(mhMemDC,700,360);
	LineTo(mhMemDC,700,200);
	LineTo(mhMemDC,600,200);

	

	for(int line=0;line<LINES;++line)
	{
		for(int row=0;row<ROWS;++row)
		{
			if(mMap[row][line]<MAXBOXSTYLE)
			{
				BitBlt(mhMemDC,
					BOXSIDE*line+190,BOXSIDE*row+6,
					BOXSIDE,BOXSIDE,
					mhBoxDC[mMap[row][line]],
					0,0,SRCCOPY);
			}
		}
	}


	int i;
	for(i=0;i<4;++i)
	{
		BitBlt(mhMemDC,
			BOXSIDE*mCurPos[i].x+190,BOXSIDE*mCurPos[i].y+6,
			BOXSIDE,BOXSIDE,
			mhBoxDC[mCurBoxStyle],
			0,0,SRCCOPY);
	}

	

	for(i=0;i<4;++i)
	{
		BitBlt(mhMemDC,
			BOXSIDE*blockStyle[mNextBlockStyle][i].x+630,BOXSIDE*blockStyle[mNextBlockStyle][i].y+250,
			BOXSIDE,BOXSIDE,
			mhBoxDC[mNextBoxStyle],
			0,0,SRCCOPY);
	}

	

	BitBlt(hdc,0,0,BGWIDTH,BGHEIGHT,mhMemDC,0,0,SRCCOPY);
}


/*开始一个新游戏*/
void CGame::NewGame(void)
{

	for(int row=0;row<ROWS;++row)
	{
		mnRow[row]=0;
		for(int line=0;line<LINES;++line)
		{
			mMap[row][line]=MAXBOXSTYLE;
		}
	}

	//产生下一个BLOCK
	mNextBoxStyle=rand()%MAXBOXSTYLE;
	mNextBlockStyle=rand()%MAXBLOCKSTYLE;

	SetTimer(mhWnd,IDTIMER,1000,NULL);
	NewBlock();
}



/*响应定时器消息*/
void CGame::OnTimer(void)
{
	ShiftDown();
}


BOOL CGame::ShiftDown(void)
{
	POINTS temp[4];
	for(int i=0;i<4;++i)
	{
		temp[i]=mCurPos[i];
		mCurPos[i].y++;
		if(DetectCollision(mCurPos[i]))
		{
			int j;
			for(j=0;j<=i;j++) mCurPos[j]=temp[j];

			for(j=0;j<4;j++)
			{
				mMap[mCurPos[j].y][mCurPos[j].x]=mCurBoxStyle;
				mnRow[mCurPos[j].y]++;
			}


			for(int row=0;row<ROWS;row++)
			{
				if(mnRow[row]==LINES)
				{
					for(int y=row;y>=1;y--)
					{
						mnRow[y]=mnRow[y-1];
						for(int x=0;x<LINES;x++) mMap[y][x]=mMap[y-1][x];
					}
					mnRow[0]=0;
					for(int x=0;x<LINES;++x)
						mMap[0][x]=MAXBOXSTYLE;
				}
			}

			NewBlock();

			return FALSE;
		}
	}
	return TRUE;
}

/*碰撞检测*/
BOOL CGame::DetectCollision(POINTS p)
{
	if(p.x<0||p.x>=LINES) return TRUE;

	if(p.y>ROWS) return TRUE;

	if(p.y>=0&&mMap[p.y][p.x]!=MAXBOXSTYLE)  return TRUE;

	return FALSE;

}


/*产生新方块*/

void CGame::NewBlock(void)
{
	mCurPos[0].x=LINES/2;
	mCurPos[0].y=0;

	for(int i=0;i<4;++i)
	{
		mCurPos[i].x=mCurPos[0].x+blockStyle[mNextBlockStyle][i].x;
		mCurPos[i].y=mCurPos[0].y+blockStyle[mNextBlockStyle][i].y;
		if(DetectCollision(mCurPos[i]))
		{
			KillTimer(mhWnd,IDTIMER);
			MessageBox(mhWnd,"失败了~",szWindowTitle,MB_OK);
			PostQuitMessage(0);
		}
	}

	mCurBoxStyle=mNextBoxStyle;
	mCurBlockStyle=mNextBlockStyle;

	mNextBoxStyle=rand()%MAXBOXSTYLE;
	mNextBlockStyle=rand()%MAXBLOCKSTYLE;
		
}


BOOL CGame::ShiftLeft(void)
{
	POINTS temp[4];
	for(int i=0;i<4;++i)
	{
			temp[i]=mCurPos[i];
			mCurPos[i].x--;
			if(DetectCollision(mCurPos[i]))
			{
				for(int j=0;j<=i;++j) mCurPos[j]=temp[j];
				break;
			}
	}
	return TRUE;
}

BOOL CGame::ShiftRight(void)
{
	POINTS temp[4];
	for(int i=0;i<4;++i)
	{
			temp[i]=mCurPos[i];
			mCurPos[i].x++;
			if(DetectCollision(mCurPos[i]))
			{
				for(int j=0;j<=i;++j) mCurPos[j]=temp[j];
				break;
			}
	}
	return TRUE;
}

void CGame::OnKeyUp(UINT key)
{
	switch(key)
	{
	case VK_LEFT:
		ShiftLeft();
		break;

	case VK_RIGHT:
		ShiftRight();
		break;

	case VK_UP:
		Rotate(TRUE);
		break;

	case VK_DOWN:
		Rotate(FALSE);
		break;

	case VK_SPACE:
		QuickDown();
		break;
	}
}



BOOL CGame::Rotate(bool clockwise)
{
	if(mCurBlockStyle==1) return TRUE;
	
	POINTS temp[4];
	
	for(int i=1;i<4;++i)
	{
		temp[i]=mCurPos[i];
		UINT deltaX=mCurPos[i].x-mCurPos[0].x;
		UINT deltaY=mCurPos[i].y-mCurPos[0].y;

		if(clockwise)
		{
			mCurPos[i].x=mCurPos[0].x+deltaY;
			mCurPos[i].y=mCurPos[0].y-deltaX;
		}
		else
		{
			mCurPos[i].x=mCurPos[0].x-deltaY;
			mCurPos[i].y=mCurPos[0].y+deltaX;
		}

		if(DetectCollision(mCurPos[i]))
		{
			for(int j=1;j<=i;++j) mCurPos[j]=temp[j];
			break;
		}
	}
	return TRUE;
}

BOOL CGame::QuickDown(void)
{
	HDC hdc=GetDC(mhWnd);
	while(ShiftDown())
	{
		OnPaint(hdc);
		Sleep(50);
	}
	ReleaseDC(mhWnd,hdc);
	return TRUE;
}