#include "Common.h"
#include "Sound.h"


//先初始化m_pSoundManager,否则会产生链接错误,静态成员
CSoundManager* CSoundManager::m_pSoundManager=NULL;

CSoundManager::CSoundManager()
{
	m_pSoundManager=this;  //构造函数初始化
	m_pDirectSound=NULL;
}

CSoundManager::~CSoundManager()
{
	SafeRelease(m_pDirectSound);
}
//初始化管理器
bool CSoundManager::Init(HWND hWnd,DWORD dwCoopLevel)
{
	//创建IDirectSound对象
	if(FAILED(DirectSoundCreate8(NULL,&m_pDirectSound,NULL)))
		return false;
	//设置协调级别(DSSCL_NORMAL会自动设置一个主缓冲区)
	if(FAILED(m_pDirectSound->SetCooperativeLevel(hWnd,dwCoopLevel)))
		return false;
	return true;
}
//设置主缓冲区格式
bool CSoundManager::SetPrimaryBufferFormat(
	WORD wChannels,
	DWORD dwSamplesPerSec,
	WORD wBitsPerSample)
{
		if(m_pDirectSound==NULL)
			return false;
		//填写主缓冲区的描述结构体
		DSBUFFERDESC bufferDesc;
		ZeroMemory(&bufferDesc,sizeof(DSBUFFERDESC));
		bufferDesc.dwSize=sizeof(DSBUFFERDESC);
		bufferDesc.dwFlags=DSBCAPS_PRIMARYBUFFER;  //表示主缓冲区
		bufferDesc.dwBufferBytes=0;  //主缓冲区必须设置为0
		bufferDesc.lpwfxFormat=NULL; //主缓冲区必须设置为NULL
		//取得主缓冲区
		IDirectSoundBuffer* pDirectSoundBuf=NULL;
		if(FAILED(m_pDirectSound->CreateSoundBuffer(&bufferDesc,&pDirectSoundBuf,NULL)))
			return false;
		//设置主缓冲区的自定义格式
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
		//释放主缓冲区接口对象(已在声卡内存分配好主缓冲区)
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
	//打开Wave文件	
	FILE *fp;
	if((fp=fopen(szWaveFile,"rb"))==NULL)
		return false;
	//取得次缓冲区
	if(!GetSecondaryBuffer(fp)){
		fclose(fp);  //关闭Wave文件
		return false;
	}
	//读入Wave声音数据
	if(!LoadDataInBuffer(fp)){
		fclose(fp);
		return false;
	}
	//关闭文件
	fclose(fp);
	return true;
}

bool CSound::GetSecondaryBuffer(FILE *fp)
{
	//读文件头的Wav格式
	sWaveHeader* pWaveHeader=new sWaveHeader();
	fseek(fp,0,SEEK_SET);
	fread(pWaveHeader,1,sizeof(sWaveHeader),fp);
	//检查是否为Wav文件
	if(memcmp(pWaveHeader->RiffID, "RIFF", 4) || 
		memcmp(pWaveHeader->WaveID, "WAVE", 4) ||
		memcmp(pWaveHeader->FormatID, "fmt ", 4) ||
		memcmp(pWaveHeader->DataID, "data",4))
		return false;
	//设置声音格式
	WAVEFORMATEX waveFormat;
	ZeroMemory(&waveFormat,sizeof(WAVEFORMATEX));
	waveFormat.wFormatTag=WAVE_FORMAT_PCM;
	waveFormat.nChannels=pWaveHeader->Channels;  //声道数
	waveFormat.nSamplesPerSec=pWaveHeader->SampleRate;  //播放频率
	waveFormat.wBitsPerSample=pWaveHeader->BitsPerSample;  //位数
	waveFormat.nBlockAlign=(waveFormat.wBitsPerSample / 8) * waveFormat.nChannels;
	waveFormat.nAvgBytesPerSec=waveFormat.nSamplesPerSec * waveFormat.nBlockAlign;
	//填写缓冲区描述结构体(音量可控制、声音文件大小和声音格式)
	DSBUFFERDESC bufferDesc;
	ZeroMemory(&bufferDesc,sizeof(DSBUFFERDESC));
	bufferDesc.dwSize=sizeof(DSBUFFERDESC);
	bufferDesc.dwFlags=DSBCAPS_CTRLVOLUME|DSBCAPS_GLOBALFOCUS|DSBCAPS_STATIC;
	bufferDesc.dwBufferBytes=pWaveHeader->DataSize;
	bufferDesc.lpwfxFormat=&waveFormat;
	//先取得IDirectSoundBuffer缓冲区接口对象
	IDirectSoundBuffer* pDirectSoundBuf=NULL;
	if(FAILED(CSoundManager::GetSoundManager()->GetDirectSound()->CreateSoundBuffer(&bufferDesc,&pDirectSoundBuf,NULL)))
		return false;
	//再取得IDirectSoundBuffer8缓冲区接口对象
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
	//读Wave文件大小
	sWaveHeader* pWaveHeader=new sWaveHeader();
	fseek(fp,0,SEEK_SET);
	fread(pWaveHeader,1,sizeof(sWaveHeader),fp);
	//锁定缓冲区
	BYTE *pSoundPtr1=NULL;
	BYTE *pSoundPtr2=NULL;
	DWORD dwSoundSize1,dwSoundSize2;
	if(FAILED(m_pDirectSoundBuf8->Lock(0,pWaveHeader->DataSize,
		(void**)&pSoundPtr1, &dwSoundSize1,
		(void**)&pSoundPtr2, &dwSoundSize2,0)))
		return false;
	//读数据到缓冲区
	fread(pSoundPtr1,1,dwSoundSize1,fp);
	if(pSoundPtr2 != NULL)
		fread(pSoundPtr2,1,dwSoundSize2,fp);
	//解锁
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