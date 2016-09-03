#include "D3DAudio.h"

CAudio::CAudio(bool _isAVI)
{
	m_bAVI = _isAVI;
	m_pGraph = NULL;
	m_pMediaControl = NULL;
	m_pMediaPosition = NULL;
	m_pVM = NULL;
	m_pMediaEvent = NULL;
	m_dwPauseTime = 0.0f;

	HRESULT hr = CoInitialize(NULL);

	if(FAILED(hr))
	{
		return;
	}
	CoCreateInstance(CLSID_FilterGraph , NULL, CLSCTX_INPROC, IID_IGraphBuilder,(void **)&m_pGraph);
	m_pGraph->QueryInterface(IID_IMediaControl , (void **)&m_pMediaControl);
	if (m_bAVI)
	{
		m_pGraph->QueryInterface(IID_IMediaEvent, (void **) &m_pMediaEvent);//AVI
		m_pGraph->QueryInterface(IID_IVideoWindow , (void **) &m_pVM);
	}
	else
	{
		m_pGraph->QueryInterface(IID_IMediaPosition, (void **) &m_pMediaPosition); //MP3
	}
}
void CAudio::initMedia(char *pFileName)
{
	WCHAR fileName[255];
	MultiByteToWideChar(CP_ACP, 0, pFileName, -1, fileName, 255);
	m_pGraph->RenderFile(fileName,NULL);
}

void CAudio::run()
{
	if (NULL == m_pMediaControl)
	{
		return ;
	}
	if (!m_bAVI)
	{
		m_pMediaPosition->put_CurrentPosition(0);
	}
	m_pMediaControl->Run();
}
void CAudio::pause()
{
	if (NULL == m_pMediaControl)
	{
		return ;
	}
	m_pMediaControl->Stop();
}
bool CAudio::isPlayFinish()
{
	double cur;
	double totle;
	m_pMediaPosition->get_CurrentPosition(&cur);
	m_pMediaPosition->get_Duration(&totle);
	if(cur >= totle)
	{

		return true;
	}
	return false;
}

bool CAudio::playPause()
{
	m_pMediaPosition->get_CurrentPosition(&m_dwPauseTime);
	m_pMediaPosition->put_StopTime(m_dwPauseTime);
	m_pMediaControl->Stop();
	return true;

}

void CAudio::repeat()
{
	if ( isPlayFinish() )
	{
		run();
	}
}

CAudio::~CAudio(void)
{
	m_pMediaControl->Stop();

	SAFE_RELEASE(m_pMediaEvent);
	SAFE_RELEASE(m_pVM);
	SAFE_RELEASE(m_pMediaPosition);
	SAFE_RELEASE(m_pMediaControl);
	SAFE_RELEASE(m_pGraph);
	CoUninitialize();
}