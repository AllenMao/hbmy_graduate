#pragma once
typedef struct sWaveHeader_tag {
  char  RiffID[4];         //'RIFF'��
  long  WaveformChunkSize;  //4���ֽ�
  char  WaveID[4];         //.wav�ļ�ӦΪ'WAVE'����
  char  FormatID[4];       //'fmt '��(ĩβ��һ���ո�)
  long  FormatChunkSize;    //16���ֽ�
  short FormatTag;          //.wav�ļ�ӦΪWAVE_FORMAT_PCM
  short Channels;           //������
  long  SampleRate;         //����Ƶ��
  long  BytesPerSec;        //ÿ��������ֽ���
  short BlockAlign;         //������С
  short BitsPerSample;      //bitλ��(8bit��16bit)
  char  DataID[4];         //'data'��
  long  DataSize;           //data��Ĵ�С(ʵ�ʵ���������)
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