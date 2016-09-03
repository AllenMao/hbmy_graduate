#pragma once
typedef struct sWaveHeader_tag {
  char  RiffID[4];         //'RIFF'块
  long  WaveformChunkSize;  //4个字节
  char  WaveID[4];         //.wav文件应为'WAVE'类型
  char  FormatID[4];       //'fmt '块(末尾有一个空格)
  long  FormatChunkSize;    //16个字节
  short FormatTag;          //.wav文件应为WAVE_FORMAT_PCM
  short Channels;           //声道数
  long  SampleRate;         //采样频率
  long  BytesPerSec;        //每秒采样的字节数
  short BlockAlign;         //块对齐大小
  short BitsPerSample;      //bit位数(8bit或16bit)
  char  DataID[4];         //'data'块
  long  DataSize;           //data块的大小(实际的声音数据)
}sWaveHeader;

class CSoundManager{
private:
	static CSoundManager* m_pSoundManager;
	IDirectSound8 *m_pDirectSound;
public:
	CSoundManager();
	~CSoundManager();
	bool Init(HWND hWnd,DWORD dwCoopLevel=DSSCL_NORMAL);
	static CSoundManager* GetSoundManager(){return m_pSoundManager;}
	inline IDirectSound8* GetDirectSound(){return m_pDirectSound;}
	bool SetPrimaryBufferFormat(WORD wChannels=1,DWORD dwSamplesPerSec=22050,WORD wBitsPerSample=16);
};

class CSound{
private:
	IDirectSoundBuffer8 *m_pDirectSoundBuf8;
	bool GetSecondaryBuffer(FILE* fp);
	bool LoadDataInBuffer(FILE* fp);
public:
	CSound();
	~CSound();
	bool LoadWaveFile(TCHAR* szWaveFile);
	bool Play(bool bLoop);
	bool SetVolume(long lVolume=DSBVOLUME_MAX);
	bool Stop();
};