#pragma once

#pragma  comment(lib,"dxguid.lib")
#pragma  comment(lib,"dinput8.lib")
#pragma  comment(lib,"Winmm.lib")
#pragma  comment(lib,"d3d9.lib")
#pragma  comment(lib,"d3dx9.lib")
#pragma comment (lib,"dsound.lib")
#pragma comment (lib,"strmiids.lib")

#include <windows.h>
#include <assert.h>

#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include <iostream>
#include <vector>
#include <set>
#include <map>
#include <list>

#include <tchar.h>

#include <D3DX9Math.h>
#include <d3d9.h>
#include <d3dx9.h>
// ‰»Î∞Ê±æ
#define DIRECTINPUT_VERSION 0x0800
#include<dinput.h>
#include <dsound.h>

//Audio & Vedio
#include <DShow.h>
#pragma comment (lib,"dxguid.lib")
#pragma comment (lib,"strmiids.lib")


#define SafeRelease(p) if(p){p->Release(); (p) = NULL;}
#define SafeDelete(p) { if(p) { delete (p); (p)=NULL; } }
#define SafeDeleteArray(p) { if(p) { delete[] (p); (p)=NULL; } }

#define SAFE_DELETE(p)       { if(p) { delete (p);     (p)=NULL; } }
#define SAFE_DELETE_ARRAY(p) { if(p) { delete[] (p);   (p)=NULL; } }
#define SAFE_RELEASE(p)      { if(p) { (p)->Release(); (p)=NULL; } }