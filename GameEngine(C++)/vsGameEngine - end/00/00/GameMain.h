#pragma once
#include<windows.h>
//#include"resource.h"
#include"GameEngine.h"
#include"Bitmap.h"
#include"Sprite.h"

GameEngine* GameEngine::m_pGameEngine = NULL;//��̬��Ա��ʼ��

HINSTANCE		g_hInstance;
GameEngine*		g_pGame;

Bitmap*			pBitmap;
Bitmap*			pBitmap2;
Bitmap*			pBitmap3;
Bitmap*			pBitmap4;
Sprite*			pSprite;
Sprite*			pSprite2;
