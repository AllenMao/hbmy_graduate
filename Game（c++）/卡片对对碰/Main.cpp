#include "GameEngine.h"
#include "resource.h"		//������Դ�ļ�
#include<stdio.h>
#include<stdlib.h>
#include<time.h>
GameEngine *	g_pGE;		//ȫ������ָ��
/////////////////////////////////////////////////////

//����״̬
enum SELECTSTATE{INITSELECT=0,   //ѡ��״̬��ʼ��
				 NOSELECT,       //δѡ��
				 SELECTED,     //��ѡ��
				 SELECTOK     //��ȷƥ��
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

/*������Ϸȫ�ֱ���*/
char *g_strTitle="��Ƭ�Զ���";

int g_nCardPattern[ROWS][COLS];
RECT g_rCardRect[ROWS][COLS];
SELECTSTATE g_nCardState[ROWS][COLS];

int g_nSelectRow1,g_nSelectRow2,g_nSelectCol1,g_nSelectCol2;
int g_nSelected;

RECT g_rTimeBarFrame;		//ʱ�����߿���δ�С
int g_nTimeBarLength;		//ʱ��������
int g_nTimeCount;			//�涨ʱ�䳤��
int g_nStartTime;			//��ʼʱ��
long g_nTimeDelayStart;		//��ʱ����ʱ��



long g_lPreTime;			//�ϴ�ѭ��ʱ��
int g_nFrequency;			//Ƶ��


/*  ��Ϸ��ʼ�� */
BOOL GameInitialize(HINSTANCE hInstance)
{
	
	//��������
	g_pGE=new GameEngine(hInstance,TEXT("War"),g_strTitle,
			IDI_ICON1,IDI_ICON2,FALSE,32,800,600);
	if(g_pGE==NULL)
		return false;

	//��ʼ���������
	g_pGE->SetFrameDelay(15) ;		//������Ϸ֡��ʱΪ15����
	g_pGE->SetPause(false);			//������ͣ״̬Ϊfalse
	if(!g_pGE->CreateGameWindow())	//������Ϸ����
	{
		MessageBox(NULL,"�޸���ʾģʽʧ��!\r\n�����˳�!","����",MB_OK| MB_ICONERROR);
		return FALSE;		//�˳�����
	}

	return true;
}

/* ��Ϸ�߼� */
void GameAction(HWND hWnd)
{
	/*������Ϸ������ʱ���ʱ��������*/
	long nCurrentTimes=timeGetTime()-g_nStartTime;
	if(nCurrentTimes<g_nTimeCount)
		g_nTimeBarLength=nCurrentTimes*596/g_nTimeCount;

	/*���2�ű����������ж�2�ſ�Ƭͼ����ͬ��*/
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





	long nTick=timeGetTime();//��ȡ��ǰʱ��
	g_nFrequency=1000/(nTick-g_lPreTime);
	InvalidateRect(hWnd,NULL,FALSE);	//ǿ��ˢ�´���
	UpdateWindow(hWnd);

	/*�����Ϸʱ���Ƿ񳬳��涨ʱ��*/
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


/*  ��Ⱦ��Ϸ���� ***/
void GamePaint(HDC hDC)
{

	


	HDC dc=CreateCompatibleDC(hDC);
	HBITMAP hbm=(HBITMAP)LoadImage(g_pGE->GetInstance(),MAKEINTRESOURCE(IDB_BITMAP1),
		IMAGE_BITMAP,800,600,LR_DEFAULTCOLOR);
	SelectObject(dc,hbm);

	BitBlt(dc,0,0,800,600,dc,0,0,SRCCOPY);

	DeleteObject(SelectObject(dc,hbm));
	DeleteObject(hbm);

	SetBkMode(hDC,TRANSPARENT);  //���ñ���Ϊ͸��
	SetTextColor(hDC,RGB(255,255,0)); //��������Ϊ��ɫ
	char str[100];
	int nGameTime=timeGetTime()-g_pGE->GetStartTime(); //��Ϸ����ʱ��
	int nBits=GetDeviceCaps(hDC,BITSPIXEL);      //��ȡ��Ļ��ɫλ��
	int nScreenWidth=GetSystemMetrics(SM_CXSCREEN);    //��ȡ��Ļ���
	int nScreenHeight=GetSystemMetrics(SM_CYSCREEN);    //��ȡ��Ļ�߶�

	sprintf(str,"��Ϸ����ʱ�䣺 %d ms   ֡Ƶ�� %d   ��ʾģʽ�� %d bit,%d*%d",
		nGameTime,g_nFrequency,nBits,nScreenWidth,nScreenHeight);

	TextOut(hDC,10,10,str,strlen(str));

	DeleteDC(dc);
	/************************************************************************/
	/*        ����         ���                                                              */
	/************************************************************************/
/* ���ƴ��ڵ�ɫ */
	/*RECT rBG;
	GetClientRect(g_pGE->GetWindow(), &rBG);		//��ȡ���ھ��η�Χ	
	HBRUSH hbrBG=(HBRUSH)GetStockObject(WHITE_BRUSH);	//ѡ�ñ�׼��ɫ��ˢ
	FillRect(hDC,&rBG,hbrBG);		//���Ʊ���
	DeleteObject(hbrBG);			//ɾ����ˢ

	/* ���Ʊ��� */
	LOGFONT lf={0};			//����Ĭ���߼�����
	lf.lfHeight=80;			//��������߶�
	lf.lfWidth=60;				//����������
	lf.lfWeight=800;			//���������ϸ
	lf.lfCharSet=DEFAULT_CHARSET;		//�����ַ���
	strcpy(lf.lfFaceName,"����");			//��������	
	HFONT hFont=CreateFontIndirect(&lf);	//��������	
	HFONT hFontOld=(HFONT)SelectObject(hDC,hFont);	//ѡ������
	TextOut(hDC,100,30,g_strTitle,strlen(g_strTitle));	//�����ı�	

	SelectObject(hDC,hFontOld);			//�滻����
	DeleteObject(hFont);				//ɾ������




	/* ����ʱ���� */
	HBRUSH hbr=CreateSolidBrush(RGB(128,128,128));	//������ˢ
	HBRUSH hbrOld=(HBRUSH)SelectObject(hDC,hbr);	//ѡ�û�ˢ
	FrameRect(hDC,&g_rTimeBarFrame,hbr);				//���Ʊ߿�
	//����ʱ����
	Rectangle(hDC,g_rTimeBarFrame.left+2,g_rTimeBarFrame.top+2,g_rTimeBarFrame.left+2+g_nTimeBarLength,g_rTimeBarFrame.bottom-2);
	SelectObject(hDC,hbrOld);		//�����ˢ				

	/* ���ƿ�Ƭ */
	//������Ƭ���滭ˢ
	HBRUSH hbrBK=CreateHatchBrush(HS_BDIAGONAL,RGB(128,128,128)); 
	HBRUSH hbrCard;				//���忨Ƭ���滭ˢ
	RECT r;						//���忨Ƭ��ͼ���η�Χ

	for(int i=0;i<ROWS;i++)
	{
		for(int j=0;j<COLS;j++)
		{
			FrameRect(hDC,&g_rCardRect[i][j],hbr);//	���ƿ�Ƭ�߿�	
			r=g_rCardRect[i][j];			//���ÿ�Ƭͼ�����η�Χ

			/* �жϿ�Ƭ�Ƿ�δ������ѡ��*/
			if(g_nCardState[i][j]==NOSELECT)	//
			{		
				/*δ������ѡ��,���Ʊ���ͼ��*/
				InflateRect(&r,-10,-10);	//��С����	
				FillRect(hDC,&r,hbrBK);		//�����ñ���ͼ����ˢ���ľ���
			}
			else
			{	/*������ѡ��,��������ͼ��*/
				InflateRect(&r,-20,-20);	//��С����	
				switch(g_nCardPattern[i][j])	//ѡ����Ƶ�ָ����ͼ��
				{
				case 0:
					hbrCard=CreateSolidBrush(RGB(128,255,0));	//������ˢ
					SelectObject(hDC,hbrCard);					//ѡ�û�ˢ
					RoundRect(hDC,r.left,r.top,r.right,r.bottom, 10,10);	//����Բ�Ǿ���
					SelectObject(hDC,hbrOld);		//�����ˢ			

	
					break;

				case 1:
					hbrCard=CreateSolidBrush(RGB(255,128,255));
					SelectObject(hDC,hbrCard);		
					RoundRect(hDC,r.left,r.top,r.right,r.bottom, 10,10);	//����Բ�Ǿ���
					SelectObject(hDC,hbrOld);					
					break;

				case 2:
					hbrCard=CreateSolidBrush(RGB(255,128,128));
					SelectObject(hDC,hbrCard);		
					Rectangle(hDC,r.left,r.top,r.right,r.bottom);	//���ƾ���
					SelectObject(hDC,hbrOld);						
					break;

				case 3:
					hbrCard=CreateSolidBrush(RGB(0,128,128));
					SelectObject(hDC,hbrCard);		
					Rectangle(hDC,r.left,r.top,r.right,r.bottom);	//���ƾ���
					SelectObject(hDC,hbrOld);					
					break;

				case 4:
					hbrCard=CreateSolidBrush(RGB(128,0,255));
					SelectObject(hDC,hbrCard);		
					Ellipse(hDC,r.left,r.top,r.right,r.bottom);		//����Բ��
					SelectObject(hDC,hbrOld);			
					break;

				case 5:
					hbrCard=CreateSolidBrush(RGB(255,0,128));
					SelectObject(hDC,hbrCard);		
					Ellipse(hDC,r.left,r.top,r.right,r.bottom);		//����Բ��
					SelectObject(hDC,hbrOld);			
					break;

				default:
					/* ��Ƭͼ��Ϊ��,���ư�ɫ������	*/
					//ѡ�ñ�׼��ɫ��ˢ
					hbrCard=(HBRUSH)GetStockObject(WHITE_BRUSH);	
					FillRect(hDC,&g_rCardRect[i][j],hbrCard);		//���ư�ɫ����	
					break;	
				}
				DeleteObject(hbrCard);			//ɾ����ˢ	
			}
		}
	}
	
	//BitBlt(hDC,0,0,g_pGE->GetWidth(),g_pGE->GetHeight(),hMemDC,0,0,SRCCOPY);

	/*����GDI����*/
	DeleteObject(hbrBK);
	DeleteObject(hbr);

}

/* ��Ϸ��ʼ */
void GameStart(HWND hWnd)
{
	/* �������ָ��*/
	long lCur=(long)LoadCursor(g_pGE->GetInstance(),MAKEINTRESOURCE(IDC_CURSOR1));
	SetClassLong(hWnd, GCL_HCURSOR,lCur);

/*************************�ָ���*******************************/
	/*ѭ�����ÿ�Ƭ���κͳ�ʼ����Ƭѡ��״̬*/
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

	 //��ʼ�����������У��к�
	 g_nSelectRow1=g_nSelectRow2=g_nSelectCol1=g_nSelectCol2=-1; 
	 g_nSelected=0;
}


/* ��Ϸ���� */
void GameEnd()
{
	//�ͷ�λͼ��Դ
	

	delete g_pGE;	//�����Ϸ����ָ��	
}

/* ��Ϸ���� */
void GameActive(HWND hWnd)
{
	g_pGE->SetPause(FALSE);	

}

/* ��Ϸ��ͣ */
void GamePause(HWND hWnd)
{
	g_pGE->SetPause(TRUE);
}

/* ��Ϸ���ڹر� */
BOOL GameWindowClose(HWND hWnd)
{
	if(MessageBox(g_pGE->GetWindow(),"��ȷ��Ҫ�˳���","�˳���Ϸ",MB_YESNO|MB_DEFBUTTON2|MB_ICONASTERISK)==IDYES)
		return TRUE;
	else
		return FALSE;
}

/* ��굥���¼����� */
void MouseClick(HWND hWnd,int x,int y,WPARAM wParam)
{
	//g_nDialigIndex++;		//�Ի���ŵ���
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