#pragma once

#pragma  comment(lib,"dxguid.lib")
#pragma  comment(lib,"dinput8.lib")
#pragma  comment(lib,"Winmm.lib")
#pragma  comment(lib,"d3d9.lib")
#pragma  comment(lib,"d3dx9.lib")
#pragma comment (lib,"dsound.lib")
#pragma comment (lib,"strmiids.lib")

#include <time.h>
#include <windows.h>
#include <math.h>
#include <tchar.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <D3DX9Math.h>
#include <vector>
#include <set>
#include <map>
#include <list>
#include <d3d9.h>
#include <d3dx9.h>
//����汾
#define DIRECTINPUT_VERSION 0x0800
#include<dinput.h>

#include <dsound.h>


#define SafeRelease(p) if(p){p->Release(); (p) = NULL;}
#define SafeDelete(p) { if(p) { delete (p); (p)=NULL; } }
#define SafeDeleteArray(p) { if(p) { delete[] (p); (p)=NULL; } }

#define SAFE_DELETE(p)       { if(p) { delete (p);     (p)=NULL; } }
#define SAFE_DELETE_ARRAY(p) { if(p) { delete[] (p);   (p)=NULL; } }
#define SAFE_RELEASE(p)      { if(p) { (p)->Release(); (p)=NULL; } }