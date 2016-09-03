#include "Common.h"
#include "Sound.h"


//�ȳ�ʼ��m_pSoundManager,�����������Ӵ���,��̬��Ա
CSoundManager* CSoundManager::m_pSoundManager=NULL;

CSoundManager::CSoundManager()
{
	m_pSoundManager=this;  //���캯����ʼ��
	m_pDirectSound=NULL;
}

CSoundManager::~CSoundManager()
{
	SafeRelease(m_pDirectSound);
}
//��ʼ��������
bool CSoundManager::Init(HWND hWnd,DWORD dwCoopLevel)
{
	//����IDirectSound����
	if(FAILED(DirectSoundCreate8(NULL,&m_pDirectSound,NULL)))
		return false;
	//����Э������(DSSCL_NORMAL���Զ�����һ����������)
	if(FAILED(m_pDirectSound->SetCooperativeLevel(hWnd,dwCoopLevel)))
		return false;
	return true;
}
//��������������ʽ
bool CSoundManager::SetPrimaryBufferFormat(
	WORD wChannels,
	DWORD dwSamplesPerSec,
	WORD wBitsPerSample)
{
		if(m_pDirectSound==NULL)
			return false;
		//��д���������������ṹ��
		DSBUFFERDESC bufferDesc;
		ZeroMemory(&bufferDesc,sizeof(DSBUFFERDESC));
		bufferDesc.dwSize=sizeof(DSBUFFERDESC);
		bufferDesc.dwFlags=DSBCAPS_PRIMARYBUFFER;  //��ʾ��������
		bufferDesc.dwBufferBytes=0;  //����������������Ϊ0
		bufferDesc.lpwfxFormat=NULL; //����������������ΪNULL
		//ȡ����������
		IDirectSoundBuffer* pDirectSoundBuf=NULL;
		if(FAILED(m_pDirectSound->CreateSoundBuffer(&bufferDesc,&pDirectSoundBuf,NULL)))
			return false;
		//���������������Զ����ʽ
		WAVEFORMATEX waveFormat;
		ZeroMemory(&waveFormat,sizeof(WAVEFORMATEX)); 
		waveFormat.wFormatTag=WAVE_FORMAT_PCM; 
		waveFormat.nChannels=wChannels; 
		waveFormat.nSamplesPerSec=dwSamplesPerSec; 
		waveFormat.wBitsPerSample=wBitsPerSample; 
		waveFormat.nBlockAlign=(waveFormat.wBitsPerSample / 8) * waveFormat.nChannels;
		waveFormat.nAvgBytesPerSec=waveFormat.nSamplesPerSec * waveFormat.nBlockAlign;
		if(FAILED(pDirectSoundBuf->SetFormat(&waveFormat)))
			return false;
		//�ͷ����������ӿڶ���(���������ڴ�������������)
		SafeRelease(pDirectSoundBuf);
		return true;
}


CSound::CSound()
{

}

CSound::~CSound()
{
	SafeRelease(m_pDirectSoundBuf8);
}

bool CSound::LoadWaveFile(TCHAR* szWaveFile)
{
	//��Wave�ļ�	
	FILE *fp;
	if((fp=fopen(szWaveFile,"rb"))==NULL)
		return false;
	//ȡ�ôλ�����
	if(!GetSecondaryBuffer(fp)){
		fclose(fp);  //�ر�Wave�ļ�
		return false;
	}
	//����Wave��������
	if(!LoadDataInBuffer(fp)){
		fclose(fp);
		return false;
	}
	//�ر��ļ�
	fclose(fp);
	return true;
}

bool CSound::GetSecondaryBuffer(FILE *fp)
{
	//���ļ�ͷ��Wav��ʽ
	sWaveHeader* pWaveHeader=new sWaveHeader();
	fseek(fp,0,SEEK_SET);
	fread(pWaveHeader,1,sizeof(sWaveHeader),fp);
	//����Ƿ�ΪWav�ļ�
	if(memcmp(pWaveHeader->RiffID, "RIFF", 4) || 
		memcmp(pWaveHeader->WaveID, "WAVE", 4) ||
		memcmp(pWaveHeader->FormatID, "fmt ", 4) ||
		memcmp(pWaveHeader->DataID, "data",4))
		return false;
	//����������ʽ
	WAVEFORMATEX waveFormat;
	ZeroMemory(&waveFormat,sizeof(WAVEFORMATEX));
	waveFormat.wFormatTag=WAVE_FORMAT_PCM;
	waveFormat.nChannels=pWaveHeader->Channels;  //������
	waveFormat.nSamplesPerSec=pWaveHeader->SampleRate;  //����Ƶ��
	waveFormat.wBitsPerSample=pWaveHeader->BitsPerSample;  //λ��
	waveFormat.nBlockAlign=(waveFormat.wBitsPerSample / 8) * waveFormat.nChannels;
	waveFormat.nAvgBytesPerSec=waveFormat.nSamplesPerSec * waveFormat.nBlockAlign;
	//��д�����������ṹ��(�����ɿ��ơ������ļ���С��������ʽ)
	DSBUFFERDESC bufferDesc;
	ZeroMemory(&bufferDesc,sizeof(DSBUFFERDESC));
	bufferDesc.dwSize=sizeof(DSBUFFERDESC);
	bufferDesc.dwFlags=DSBCAPS_CTRLVOLUME|DSBCAPS_GLOBALFOCUS|DSBCAPS_STATIC;
	bufferDesc.dwBufferBytes=pWaveHeader->DataSize;
	bufferDesc.lpwfxFormat=&waveFormat;
	//��ȡ��IDirectSoundBuffer�������ӿڶ���
	IDirectSoundBuffer* pDirectSoundBuf=NULL;
	if(FAILED(CSoundManager::GetSoundManager()->GetDirectSound()->CreateSoundBuffer(&bufferDesc,&pDirectSoundBuf,NULL)))
		return false;
	//��ȡ��IDirectSoundBuffer8�������ӿڶ���
	if(FAILED(pDirectSoundBuf->QueryInterface(IID_IDirectSoundBuffer8, 
		(void**)&m_pDirectSoundBuf8))){
			pDirectSoundBuf->Release();
			return false;
	}
	delete pWaveHeader;
	return true;
}

bool CSound::LoadDataInBuffer(FILE* fp)
{
	//��Wave�ļ���С
	sWaveHeader* pWaveHeader=new sWaveHeader();
	fseek(fp,0,SEEK_SET);
	fread(pWaveHeader,1,sizeof(sWaveHeader),fp);
	//����������
	BYTE *pSoundPtr1=NULL;
	BYTE *pSoundPtr2=NULL;
	DWORD dwSoundSize1,dwSoundSize2;
	if(FAILED(m_pDirectSoundBuf8->Lock(0,pWaveHeader->DataSize,
		(void**)&pSoundPtr1, &dwSoundSize1,
		(void**)&pSoundPtr2, &dwSoundSize2,0)))
		return false;
	//�����ݵ�������
	fread(pSoundPtr1,1,dwSoundSize1,fp);
	if(pSoundPtr2 != NULL)
		fread(pSoundPtr2,1,dwSoundSize2,fp);
	//����
	m_pDirectSoundBuf8->Unlock(pSoundPtr1,dwSoundSize1,pSoundPtr2,dwSoundSize2);
	delete pWaveHeader;
	return true;
}

bool CSound::Play(bool bLoop)
{
	if(m_pDirectSoundBuf8==NULL)
		return false;
	m_pDirectSoundBuf8->SetCurrentPosition(0);
	if(FAILED(m_pDirectSoundBuf8->Play(0,0,bLoop?true:false)))
		return false;
	return true;
}

bool CSound::SetVolume(long lVolume)
{
	if(FAILED(m_pDirectSoundBuf8->SetVolume(lVolume)))
		return false;
	return true;
}

bool CSound::Stop()
{
	if(FAILED(m_pDirectSoundBuf8->Stop()))
		return false;
	return true;
}