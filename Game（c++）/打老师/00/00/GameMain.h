#pragma once
#include<windows.h>
//#include"resource.h"
#include"GameEngine.h"
#include"Bitmap.h"
#include"Sprite.h"

GameEngine* GameEngine::m_pGameEngine = NULL;//静态成员初始化

HINSTANCE		g_hInstance;
GameEngine*		g_pGame;
HDC				g_hOffscreenDC;
HBITMAP			g_hOffscreenBitmap;

Bitmap*			g_pOfficeBitmap;
Bitmap*			g_pTargetBitmap[2];
Bitmap*			g_pToolBitmap[2];
Bitmap*			g_pGuyBitmaps[6];
Bitmap*			g_pSmallGuyBitmap;
Bitmap*			g_pStartBitmap;
Sprite*			g_pSprite[6];
Sprite*			g_pTargetSprite[2];
Sprite*			g_pToolSprite[2];

BOOL			g_bGameOver=FALSE;
BOOL			g_bGameStart=FALSE;
int 			g_bToolType=0;
int				g_iGuyDelay[6];
int				g_iMisses=0;
int				g_iHits=0;
