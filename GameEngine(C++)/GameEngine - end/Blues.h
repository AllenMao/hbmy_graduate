#pragma once

//-----------------------------------------------------------------
// Include Files
//-----------------------------------------------------------------
#include <windows.h>
#include "Resource.h"
#include "GameEngine.h"
#include "Bitmap.h"
#include "Sprite.h"
#include "Background.h"
#include "ScrollingBackground.h"
#include "MotorcycleSprite.h"
//-----------------------------------------------------------------
// Global Variables
//-----------------------------------------------------------------
HINSTANCE             g_hInstance;
GameEngine* g_pGame;
HDC                   g_hOffscreenDC;
HBITMAP               g_hOffscreenBitmap;
Bitmap*               g_pSplashBitmap;

BackgroundLayer*      g_pBGRoadLayer;
BackgroundLayer*      g_pBGTreesLayer;
BackgroundLayer*      g_pBGMountainsLayer;
BackgroundLayer*      g_pBGSkyLayer;

Bitmap*               g_pJumperBitmap;
Bitmap*               g_pBusBitmap;
Bitmap*               g_pRampBitmap[2];
Bitmap*               g_pGameOverBitmap;
ScrollingBackground*  g_pBackground;

int                   g_iInputDelay;
BOOL                  g_bGameOver;
BOOL                  g_bSplash;

//-----------------------------------------------------------------
// Function Declarations
//-----------------------------------------------------------------
void NewGame();
