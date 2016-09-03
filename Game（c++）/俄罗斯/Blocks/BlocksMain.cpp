#include<time.h>
#include "Game.h"



int WINAPI WinMain(HINSTANCE,HINSTANCE,LPSTR,int);
LRESULT WINAPI WndProc(HWND,UINT,WPARAM,LPARAM);

int WINAPI _tWinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nShowCmd)
//ʵ�����//ǰһʵ����� //ָ���������ָ�� //���ڵ���ʾ״̬
{
	srand((unsigned)time(NULL));



	WNDCLASS wc;//������
	if(!hPrevInstance)
	{
		wc.style=CS_HREDRAW|CS_VREDRAW;//��ֱ��ˮƽ�ػ�
		//WndProcΪ������
		wc.lpfnWndProc=WndProc;//ָ������һ���ʹ��ڵĹ��̺������ص���������ָ���Լ�ר�õĺ���
		
		//�����ڴ�  һ�㲻��Ҫ ����Ϊ0
		wc.cbClsExtra=0;
		wc.cbWndExtra=0;

		wc.hInstance=hInstance;
		wc.hIcon=LoadIcon(NULL,IDI_APPLICATION);//��׼ͼ�꼴��һ��������ΪNULL���������Ͻ�ͼ��
		wc.hCursor=LoadCursor(NULL,IDC_ARROW); //��꣬ͬ��
		wc.hbrBackground=(HBRUSH)GetStockObject(WHITE_BRUSH);//��ˢ���(���ڱ���)
		wc.lpszMenuName=NULL;
		wc.lpszClassName="Sdk ʵ��";
		//ע�ᴰ��
		RegisterClass(&wc);
	}


	
	HWND hWnd;//�����ھ��
	hWnd=CreateWindow("Sdk ʵ��",              //��������
					"Win32 SDK Ӧ�ó���ʵ��", //���ڱ���
					WS_OVERLAPPEDWINDOW&(~WS_MAXIMIZEBOX),      //���ڸ�ʽ
					//WS_OVERLAPPEDWINDOW|WS_VSCROLL|WS_HSCROLL,
					CW_USEDEFAULT,            //��ʼ��x����
					CW_USEDEFAULT,            //��ʼ��y����
					1280,			  //��ʼ�����ڿ��
					720,				//��ʼ�����ڸ߶�
					NULL,						//�����ھ��
					NULL,						//���ڲ˵����
					hInstance,					//����ʵ�����
					NULL);	
	
	if(!hWnd) return FALSE;
	//��������
	//��ʾ����
	ShowWindow(hWnd,nShowCmd);//nShowCmd  SW_SHOW
	//���������ڿͻ���
	UpdateWindow(hWnd);//һ�㲻��


	MSG msg;//������Ϣ
	//��ʼ��Ϣѭ��
	while(GetMessage(&msg,NULL,0,0))
	{
		TranslateMessage(&msg);  //����Ϣ�ӽ���ת�� -��WM_CHAR��
		DispatchMessage(&msg);   //�ٴν�msg�ַ���OS�����ô��ڹ��̼�WndProc
	}
	return msg.wParam;
}


LRESULT WINAPI WndProc(HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam)//��������ϢСд��P
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
			
			hBgDC=CreateCompatibleDC(hDC);//���������ڴ�DC
			hBitmap=(HBITMAP) LoadImage(NULL,"1.bmp",IMAGE_BITMAP,0,0,LR_LOADFROMFILE|LR_DEFAULTSIZE);
			SelectObject(hBgDC,hBitmap);
			ReleaseDC(hWnd,hDC);
			
			GetObject(hBitmap,sizeof(BITMAP),&bm);*/

			game=new CGame(hWnd);
			int xFrame=GetSystemMetrics(SM_CXSIZEFRAME);//�߿���
			int yFrame=GetSystemMetrics(SM_CYSIZEFRAME);
			int yCaption=GetSystemMetrics(SM_CYCAPTION);//����߶�
			int yMenu=GetSystemMetrics(SM_CYMENU);//�˵��߶�
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

	case WM_PAINT://�����ػ�ͻ���������Ϣ
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

			PostQuitMessage(0);//��һ��WM_QUIT��Ϣ���뵽������Ϣ������
			break;
		}
	default:
		break;
	}
	return DefWindowProc(hWnd,msg,wParam,lParam);//��OS����ȱʡ����
}
