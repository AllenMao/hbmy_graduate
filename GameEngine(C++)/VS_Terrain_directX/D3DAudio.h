#pragma once
#include "Common.h"

class CAudio
{
private:
	IGraphBuilder*	m_pGraph;			 //��Ƶ������ͼ�������
	IMediaControl*	m_pMediaControl;	 //	��Ƶ�����ſ�����
	IMediaPosition* m_pMediaPosition;	 //	��Ƶ������λ��
	IVideoWindow*	m_pVM;				 //��Ƶ�����Ŵ���
	IMediaEventEx*	m_pMediaEvent;		 //��ý�崥�����¼�
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
	void repeat();//�ظ�����
};
