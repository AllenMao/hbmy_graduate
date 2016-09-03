#pragma once
#include "Common.h"

class CAudio
{
private:
	IGraphBuilder*	m_pGraph;			 //视频流播放图像过滤器
	IMediaControl*	m_pMediaControl;	 //	视频流播放控制器
	IMediaPosition* m_pMediaPosition;	 //	视频流播放位置
	IVideoWindow*	m_pVM;				 //视频流播放窗口
	IMediaEventEx*	m_pMediaEvent;		 //流媒体触发的事件
	bool	m_bAVI;
	double	m_dwPauseTime;
public:
	//false - mp3    true - avi
	CAudio(bool _isAVI);
	~CAudio(void);
	void fullScreen(HWND hWnd);
	void run( );
	void pause();
	void initMedia(char *pFileName);
	bool isPlayFinish();
	bool playPause();
	void repeat();//重复播放
};
