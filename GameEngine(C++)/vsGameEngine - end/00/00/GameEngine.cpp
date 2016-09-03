#include"GameEngine.h"
int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,
					PSTR szCmdLine,int iCmdShow)
{
	MSG				msg;
	static int		iTickTrigger=0;
	int				iTickCount;

	if(GameInitialize(hInstance))
	{
		if(!GameEngine::GetEngine()->Initialize(iCmdShow))
			return FALSE;
		while(TRUE)
		{
			if(PeekMessage(&msg,NULL,0,0,PM_REMOVE))
			{
				if(msg.message==WM_QUIT) break;
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			else
			{
				if(!GameEngine::GetEngine()->GetSleep())
				{
					iTickCount=GetTickCount();
					if(iTickCount>iTickTrigger)
					{
						iTickTrigger=iTickCount+
							GameEngine::GetEngine()->GetFrameDelay();
						HandleKeys();
						GameCycle();
					}
				}
			}
		}
		return (int)msg.wParam;
	}
	GameEnd();
	return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWindow,UINT msg,WPARAM wparam,LPARAM lparam)
{
	return GameEngine::GetEngine()->HandleEvent(hWindow,msg,wparam,lparam);
}


//-----------------------------------------------------------------
// Game Engine Helper Methods
//-----------------------------------------------------------------
BOOL GameEngine::CheckSpriteCollision(Sprite* pTestSprite)
{
	// See if the sprite has collided with any other sprites
	vector<Sprite*>::iterator siSprite;
	for (siSprite = m_vSprites.begin(); siSprite != m_vSprites.end(); siSprite++)
	{
		// Make sure not to check for collision with itself
		if (pTestSprite == (*siSprite))
			continue;
		
		// Test the collision
		if (pTestSprite->TestCollision(*siSprite))
			// Collision detected
			return SpriteCollision((*siSprite), pTestSprite);
	}
	
	// No collision
	return FALSE;
}

GameEngine::GameEngine(HINSTANCE hInstance,LPTSTR szWindowClass,
							LPTSTR szTitle,WORD wIcon,WORD wSmallIcon,
							int iWidth,int iHeight)
{
	//static GameEngine*  GameEngine::m_pGameEngine=NULL;
	m_pGameEngine=this;
	m_hInstance=hInstance;
	m_hWindow=NULL;
	if(lstrlen(szWindowClass)>0)
		lstrcpy(m_szWindowClass, szWindowClass);
	if (lstrlen(szTitle) > 0)
		lstrcpy(m_szTitle, szTitle);
	m_wIcon = wIcon;
	m_wSmallIcon = wSmallIcon;
	m_iWidth = iWidth;
	m_iHeight = iHeight;
	m_iFrameDelay =20;   // 20 FPS default
	m_bSleep = TRUE;
	//m_vSprites.reserve(100);
	m_uiMIDIPlayerID = 0;
}
GameEngine::~GameEngine()
{
}

BOOL GameEngine::Initialize(int iCmdShow)
{
	WNDCLASSEX wndclass;

	wndclass.cbSize			=sizeof(wndclass);
	wndclass.style			=CS_HREDRAW|CS_VREDRAW;
	wndclass.lpfnWndProc	=WndProc;
	wndclass.cbClsExtra     = 0;
	wndclass.cbWndExtra     = 0;
	wndclass.hInstance      = m_hInstance;
	wndclass.hIcon			=LoadIcon(m_hInstance,
						MAKEINTRESOURCE(GetIcon()));
	wndclass.hIconSm        = LoadIcon(m_hInstance,
						MAKEINTRESOURCE(GetSmallIcon()));
	wndclass.hCursor        = LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground  = (HBRUSH)(COLOR_WINDOW + 2);
	wndclass.lpszMenuName   = NULL;
	wndclass.lpszClassName  = m_szWindowClass;
	
	// Register the window class
	if (!RegisterClassEx(&wndclass))
		return FALSE;

	int iWindowWidth=m_iWidth+ GetSystemMetrics(SM_CXFIXEDFRAME) * 2;
	int iWindowHeight = m_iHeight + GetSystemMetrics(SM_CYFIXEDFRAME) * 2 +
							 GetSystemMetrics(SM_CYCAPTION);
	if (wndclass.lpszMenuName != NULL)
		iWindowHeight += GetSystemMetrics(SM_CYMENU);
	int iXWindowPos = (GetSystemMetrics(SM_CXSCREEN) - iWindowWidth) / 2;
	int iYWindowPos = (GetSystemMetrics(SM_CYSCREEN) - iWindowHeight) / 2;
	
	
	// Create the window
	m_hWindow = CreateWindow(m_szWindowClass, m_szTitle, WS_POPUPWINDOW |
		WS_CAPTION | WS_MINIMIZEBOX, iXWindowPos, iYWindowPos, iWindowWidth,
		iWindowHeight, NULL, NULL, m_hInstance, NULL);
	if (!m_hWindow)
		return FALSE;
	
	// Show and update the window
	ShowWindow(m_hWindow, iCmdShow);
	UpdateWindow(m_hWindow);
	
	return TRUE;
}

LRESULT GameEngine::HandleEvent(HWND hWindow, UINT msg, WPARAM wParam, LPARAM lParam)
{
	// Route Windows messages to game engine member functions
	switch (msg)
	{
    case WM_CREATE:
		// Set the game window and start the game
		SetWindow(hWindow);
		GameStart(hWindow);
		return 0;
		
    case WM_SETFOCUS:
		// Activate the game and update the Sleep status
		GameActivate(hWindow);
		SetSleep(FALSE);
		return 0;
		
    case WM_KILLFOCUS:
		// Deactivate the game and update the Sleep status
		GameDeactivate(hWindow);
		SetSleep(TRUE);
		return 0;
		
    case WM_PAINT:
		HDC         hDC;
		PAINTSTRUCT ps;
		hDC = BeginPaint(hWindow, &ps);
		
		// Paint the game
		GamePaint(hDC);
		
		EndPaint(hWindow, &ps);
		return 0;
		
    case WM_LBUTTONDOWN:
		// Handle left mouse button press
		MouseButtonDown(LOWORD(lParam), HIWORD(lParam), TRUE);
		return 0;
		
    case WM_LBUTTONUP:
		// Handle left mouse button release
		MouseButtonUp(LOWORD(lParam), HIWORD(lParam), TRUE);
		return 0;
		
    case WM_RBUTTONDOWN:
		// Handle right mouse button press
		MouseButtonDown(LOWORD(lParam), HIWORD(lParam), FALSE);
		return 0;
		
    case WM_RBUTTONUP:
		// Handle right mouse button release
		MouseButtonUp(LOWORD(lParam), HIWORD(lParam), FALSE);
		return 0;
		
    case WM_MOUSEMOVE:
		// Handle mouse movement
		MouseMove(LOWORD(lParam), HIWORD(lParam));
		return 0;
		
    case WM_DESTROY:
		// End the game and exit the application
		GameEnd();
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hWindow, msg, wParam, lParam);
}

void GameEngine::ErrorQuit(LPTSTR szErrorMsg)
{
	MessageBox(GetWindow(), szErrorMsg, TEXT("Critical Error"), MB_OK | MB_ICONERROR);
	PostQuitMessage(0);
}

//Sprite Control
void GameEngine::AddSprite(Sprite* pSprite)
{
	// Add a sprite to the sprite vector
	if (pSprite != NULL)
	{
		// See if there are sprites already in the sprite vector
		if (m_vSprites.size() > 0)
		{
			// Find a spot in the sprite vector to insert the sprite by its z-order
			vector<Sprite*>::iterator siSprite;
			for (siSprite = m_vSprites.begin(); siSprite != m_vSprites.end(); siSprite++)
				if (pSprite->GetZOrder() < (*siSprite)->GetZOrder())
				{
					// Insert the sprite into the sprite vector
					m_vSprites.insert(siSprite, pSprite);
					return;
				}
		}
		// The sprite's z-order is highest, so add it to the end of the vector
		m_vSprites.push_back(pSprite);
		
	}
}
void GameEngine::DrawSprites(HDC hDC)
{
	// Draw the sprites in the sprite vector
	vector<Sprite*>::iterator siSprite;
	for (siSprite = m_vSprites.begin(); siSprite != m_vSprites.end(); siSprite++)
		(*siSprite)->Draw(hDC);
}
void GameEngine::UpdateSprites()
{
	// Expand the capacity of the sprite vector, if necessary
	if (m_vSprites.size() >= (m_vSprites.capacity() / 2))
		m_vSprites.reserve(m_vSprites.capacity() * 2);
	
	// Update the sprites in the sprite vector
	RECT          rcOldSpritePos;
	SPRITEACTION  saSpriteAction;
	vector<Sprite*>::iterator siSprite;
	for (siSprite = m_vSprites.begin(); siSprite != m_vSprites.end(); siSprite++)
	{
		// Save the old sprite position in case we need to restore it
		rcOldSpritePos = (*siSprite)->GetPosition();
		// Update the sprite
		saSpriteAction = (*siSprite)->Update();
		
		// Handle the SA_ADDSPRITE sprite action
		if (saSpriteAction & SA_ADDSPRITE)
			// Allow the sprite to add its sprite
			AddSprite((*siSprite)->AddSprite());
		
		// Handle the SA_KILL sprite action
		if (saSpriteAction & SA_KILL)
		{
			// Notify the game that the sprite is dying
			//SpriteDying(*siSprite);
			
			// Kill the sprite
			delete (*siSprite);
			m_vSprites.erase(siSprite);
			siSprite--;
			continue;
		}
		
		// See if the sprite collided with any others
		if (CheckSpriteCollision(*siSprite))
			// Restore the old sprite position
			(*siSprite)->SetPosition(rcOldSpritePos);
	}
}
void GameEngine::CleanupSprites()
{
	// Delete and remove the sprites in the sprite vector
	vector<Sprite*>::iterator siSprite;
	for (siSprite = m_vSprites.begin(); siSprite != m_vSprites.end(); siSprite++)
	{
		delete (*siSprite);
		m_vSprites.erase(siSprite);
		siSprite--;
	}
}
Sprite* GameEngine::IsPointInSprite(int x, int y)
{
	// See if the point is in a sprite in the sprite vector
	vector<Sprite*>::reverse_iterator siSprite;
	for (siSprite = m_vSprites.rbegin(); siSprite != m_vSprites.rend(); siSprite++)
		if (!(*siSprite)->IsHidden() && (*siSprite)->IsPointInside(x, y))
			return (*siSprite);
		
		// The point is not in a sprite
		return NULL;
}

//Song Control
void GameEngine::PlayMIDISong(LPTSTR szMIDIFileName,BOOL bRestart)
{
	if(m_uiMIDIPlayerID==0)
	{
		MCI_OPEN_PARMS mciOpenParms;
		mciOpenParms.lpstrDeviceType =L"sequencer";
		mciOpenParms.lpstrElementName=szMIDIFileName;
		if(mciSendCommand(NULL,MCI_OPEN,MCI_OPEN_TYPE|MCI_OPEN_ELEMENT,
			(DWORD_PTR)&mciOpenParms) == 0)
			m_uiMIDIPlayerID=mciOpenParms.wDeviceID;
		else
			return;
	}
	if(bRestart)
	{
		MCI_SEEK_PARMS mciSeekParms;
		if (mciSendCommand(m_uiMIDIPlayerID, MCI_SEEK, MCI_SEEK_TO_START,
			(DWORD_PTR)&mciSeekParms) != 0)
			// There was a problem, so close the MIDI player
			CloseMIDIPlayer();
	}

	// Play the MIDI song
	MCI_PLAY_PARMS mciPlayParms;
	if (mciSendCommand(m_uiMIDIPlayerID, MCI_PLAY, 0,
		(DWORD_PTR)&mciPlayParms) != 0)
		// There was a problem, so close the MIDI player
		CloseMIDIPlayer();
}
void GameEngine::PauseMIDISong()
{
	// Pause the currently playing song, if possible
	if (m_uiMIDIPlayerID != 0)
		mciSendCommand(m_uiMIDIPlayerID, MCI_PAUSE, 0, NULL);
}

void GameEngine::CloseMIDIPlayer()
{
	// Close the MIDI player, if possible
	if (m_uiMIDIPlayerID != 0)
	{
		mciSendCommand(m_uiMIDIPlayerID, MCI_CLOSE, 0, NULL);
		m_uiMIDIPlayerID = 0;
	}
}