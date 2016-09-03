#include"GameMain.h"

BOOL GameInitialize(HINSTANCE hInstance)
{
	g_pGame=new GameEngine(hInstance,TEXT("1"),TEXT("2"),NULL,NULL,500,400);//最后两个参数定义窗口大小
	if(g_pGame==NULL) return false;
	g_pGame->SetFrameRate(30);
	return true;
}
void GameStart(HWND hWindow)
{
	HDC hDC=GetDC(hWindow);
	g_hOffscreenDC=CreateCompatibleDC(hDC);
	g_hOffscreenBitmap = CreateCompatibleBitmap(GetDC(hWindow),
    g_pGame->GetWidth(), g_pGame->GetHeight());
	SelectObject(g_hOffscreenDC, g_hOffscreenBitmap);

	g_pOfficeBitmap=new Bitmap(hDC,L"Res\\Office.bmp");
	g_pTargetBitmap[0]= new Bitmap(hDC,L"Res\\Target.bmp");
	g_pTargetBitmap[1]= new Bitmap(hDC,L"Res\\Target2.bmp");
	g_pToolBitmap[0]=new Bitmap(hDC,L"Res\\Gun.bmp");
	g_pToolBitmap[1]=new Bitmap(hDC,L"Res\\Stick.bmp");
	g_pGuyBitmaps[3]=new Bitmap(hDC,L"Res\\Guy3.bmp");
	g_pGuyBitmaps[5]=new Bitmap(hDC,L"Res\\Guy5.bmp");
	g_pSmallGuyBitmap=new Bitmap(hDC,L"Res\\SmallGuy.bmp");
	g_pStartBitmap=new Bitmap(hDC,L"Res\\Start.bmp");

	RECT rcBounds={0,0,500,400};
	g_pTargetSprite[0] = new Sprite(g_pTargetBitmap[0], rcBounds, BA_STOP);
	g_pTargetSprite[0]->SetZOrder(4);
	g_pGame->AddSprite(g_pTargetSprite[0]);
	g_pTargetSprite[1] = new Sprite(g_pTargetBitmap[1], rcBounds, BA_STOP);
	g_pTargetSprite[1]->SetHidden(TRUE);
	g_pTargetSprite[1]->SetPosition(0,0);
	g_pTargetSprite[1]->SetZOrder(4);
	g_pGame->AddSprite(g_pTargetSprite[1]);

	g_pToolSprite[0]=new Sprite(g_pToolBitmap[0],rcBounds, BA_STOP);
	g_pToolSprite[0]->SetZOrder(4);
	g_pToolSprite[0]->SetPosition(0,0);
	g_pGame->AddSprite(g_pToolSprite[0]);
	g_pToolSprite[1]=new Sprite(g_pToolBitmap[1],rcBounds, BA_STOP);
	g_pToolSprite[1]->SetZOrder(4);
	g_pToolSprite[1]->SetPosition(77,0);
	g_pGame->AddSprite(g_pToolSprite[1]);

	g_pSprite[3]=new Sprite(g_pGuyBitmaps[3],rcBounds);
	g_pSprite[3]->SetPosition(394,61);
	g_pSprite[3]->SetZOrder(3);
	g_pSprite[3]->SetHidden(TRUE);
	g_pGame->AddSprite(g_pSprite[3]);

	g_pSprite[5]=new Sprite(g_pGuyBitmaps[5],rcBounds,BA_WRAP);
	g_pSprite[5]->SetPosition(260,60);
	g_pSprite[5]->SetVelocity(5,0);
	g_pSprite[5]->SetNumFrames(4);
	g_pSprite[5]->SetFrameDelay(5);
	g_pSprite[5]->SetZOrder(2);
	g_pSprite[5]->SetHidden(TRUE);
	g_pGame->AddSprite(g_pSprite[5]);
}
void GameEnd()
{
	// Close the MIDI player for the background music
	g_pGame->CloseMIDIPlayer();

	delete g_pOfficeBitmap;
	delete g_pTargetBitmap[0];
	delete g_pTargetBitmap[1];
	for (int i = 0; i < 6; i++)
		delete g_pGuyBitmaps[i];
	// Cleanup the sprites
	g_pGame->CleanupSprites();
	delete g_pGame;
}
void GameActivate(HWND hWindow)
{

}


void GameDeactivate(HWND hWindow)
{

}

void GamePaint(HDC hDC)
{
	g_pOfficeBitmap->Draw(hDC,0,0,FALSE);

	if(!g_bGameStart) g_pStartBitmap->Draw(hDC,150,50,FALSE);
	else
	{
		// Draw the sprites
		g_pGame->DrawSprites(hDC);

		// Draw the number of guys who were hit
		TCHAR szText[64];
		RECT rect={237,360,301,390};
		wsprintf(szText,L"%d", g_iHits);
		DrawText(hDC, szText, -1, &rect, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
		// Draw the number of guys who were missed (got away)
		for (int i = 0; i < g_iMisses; i++)
			g_pSmallGuyBitmap->Draw(hDC, 389 + (g_pSmallGuyBitmap->GetWidth() * i), 359, TRUE);
	}
}

void GameCycle()
{
	if(!g_bGameOver&&g_bGameStart)
	{
		for (int i = 1; i < 6; i++)
		{
			if ((i==3||i==5)&&g_pSprite[i]->IsHidden())
			{
				if(rand()%60==0)
				{
					g_pSprite[i]->SetHidden(FALSE);
					if (i ==5)
					{
						// Start the guy running left
						g_iGuyDelay[i] = 45;
						g_pSprite[i]->SetPosition(260, 60);
					}
					else
						g_iGuyDelay[i] =20+ (rand() %10);
				}
			}
			else if(i==3||i==5)
			{
				if(--g_iGuyDelay[i] == 0)
				{
					g_pSprite[i]->SetHidden(TRUE);
					// Increment the misses
					if (++g_iMisses == 5)
					{
						// Play a sound for the game ending
						//PlaySound((LPCSTR)IDW_BOO, g_hInstance, SND_ASYNC | SND_RESOURCE);

						// End the game
						for (int i = 0; i < 5; i++)
						{
							if(i==3||i==5)
								g_pSprite[i]->SetHidden(TRUE);
						}
						g_bGameOver = TRUE;

						// Pause the background music
						g_pGame->PauseMIDISong();
					}
				}
			}

		}
		// Update the sprites
		g_pGame->UpdateSprites();
		// Force a repaint to redraw the planets
		//InvalidateRect(g_pGame->GetWindow(), NULL, FALSE);替换
		HWND hWindow=g_pGame->GetWindow();
		HDC   hDC = GetDC(hWindow);
		GamePaint(g_hOffscreenDC);
		BitBlt(hDC,0,0,g_pGame->GetWidth(), g_pGame->GetHeight(),g_hOffscreenDC, 0, 0, SRCCOPY);
		ReleaseDC(hWindow, hDC);
	}
}

void HandleKeys()
{
	if (GetAsyncKeyState(VK_RETURN) < 0)
	{
		if(!g_bGameStart)
		{
			g_bGameStart=TRUE;
			NewGame();
		}
	}
}

void MouseButtonDown(int x, int y, BOOL bLeft)
{
	if(g_pToolSprite[0]->IsPointInside(x,y))
	{
		g_bToolType=0;
		g_pTargetSprite[0]->SetHidden(FALSE);
		g_pTargetSprite[1]->SetHidden(TRUE);
		return;
	}
	else if(g_pToolSprite[1]->IsPointInside(x,y))
	{
		g_bToolType=1;
		g_pTargetSprite[1]->SetHidden(FALSE);
		g_pTargetSprite[0]->SetHidden(TRUE);
		return;
	}
	if(!g_bGameOver&&bLeft)
	{
		g_pTargetSprite[g_bToolType]->SetHidden(TRUE);
		Sprite* pSprite;
		if ((pSprite = g_pGame->IsPointInSprite(x, y)) != NULL)
		{
			pSprite->SetHidden(TRUE);
			++g_iHits;
		}
		// Show the target sprite again
		g_pTargetSprite[g_bToolType]->SetHidden(FALSE);
	}
	else if(g_bGameOver&&!bLeft)
	{
		// Start a new game
		g_bGameOver = FALSE;
		g_iHits = 0;
		g_iMisses = 0;

		// Restart the background music
		g_pGame->PlayMIDISong();
	}
}

void MouseButtonUp(int x, int y, BOOL bLeft)
{
}

void MouseMove(int x, int y)
{
	// Track the mouse with the target sprite
	g_pTargetSprite[g_bToolType]->SetPosition(x - (g_pTargetSprite[g_bToolType]->GetWidth() / 2),
		y - (g_pTargetSprite[g_bToolType]->GetHeight() / 2));
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

void NewGame()
{
	if(g_bGameStart)
		g_pGame->PlayMIDISong(L"Music.mid",1);
}
