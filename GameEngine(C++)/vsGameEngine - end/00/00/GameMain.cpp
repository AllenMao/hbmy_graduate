#include"GameMain.h"

BOOL GameInitialize(HINSTANCE hInstance)
{
	g_pGame=new GameEngine(hInstance,TEXT("1"),TEXT("2"),NULL,NULL,600,400);//最后两个参数定义窗口大小
	if(g_pGame==NULL) return false;
	g_pGame->SetFrameRate(30);
	return true;
}
void GameStart(HWND hWindow)
{
	HDC hDC=GetDC(hWindow);
	pBitmap=new Bitmap(hDC,L"Res\\Galaxy.bmp");
	pBitmap2=new Bitmap(hDC,L"Res\\Bus.bmp");
	pBitmap3=new Bitmap(hDC,L"Res\\Background_Trees.bmp");
	pBitmap4=new Bitmap(hDC,L"Res\\Guy5.bmp");

	RECT rcBounds={0,0,600,400};
	pSprite=new Sprite(pBitmap2,rcBounds,BA_BOUNCE);
	pSprite->SetPosition(50, 0);
	pSprite->SetVelocity(3,3);

	pSprite2=new Sprite(pBitmap4,rcBounds,BA_BOUNCE);
	pSprite2->SetPosition(0,200);
	pSprite2->SetVelocity(3,0);
	pSprite2->SetNumFrames(4);
	pSprite2->SetFrameDelay(10);

	g_pGame->PlayMIDISong(L"Music.mid",1);
}
void GameEnd()
{
	delete g_pGame;
	delete pBitmap;
	delete pBitmap2;
	delete pBitmap3;
	delete pSprite;
}
void GameActivate(HWND hWindow)
{
	
}


void GameDeactivate(HWND hWindow)
{

}

void GamePaint(HDC hDC)
{
	pBitmap->Draw(hDC,0,0,FALSE);
	pBitmap3->Draw(hDC,0,150,TRUE);

	pSprite->Draw(hDC);
	pSprite2->Draw(hDC);
}

void GameCycle()
{
	pSprite->Update();
	pSprite2->Update();
	// Force a repaint to redraw the planets
	InvalidateRect(g_pGame->GetWindow(), NULL, FALSE);
}

void HandleKeys()
{
	
}

void MouseButtonDown(int x, int y, BOOL bLeft)
{
}

void MouseButtonUp(int x, int y, BOOL bLeft)
{
}

void MouseMove(int x, int y)
{
}

BOOL SpriteCollision(Sprite* pSpriteHitter,Sprite* pSpriteHittee)
{
	Bitmap* pHitter=pSpriteHitter->GetBitmap();
	Bitmap* pHittee=pSpriteHittee->GetBitmap();
	// Only check for collision between sprites that aren't hidden
	if(pSpriteHitter->IsHidden()&&pSpriteHittee->IsHidden())
	{
		//检测物体相撞
		;
	}
	return FALSE;
}
