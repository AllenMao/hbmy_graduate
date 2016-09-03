#pragma once
#include "Common.h"

#pragma pack( push, 1 )

struct BMPFileHeader
{
	unsigned short usType; //类型标志
    unsigned int uiSize; //文件大小
    unsigned short usReserved1; 
    unsigned short usReserved2; 
    unsigned int uiOffBits; //位图点阵偏移量
};

#pragma pack( pop )

struct BMPInfoHeader
{
	unsigned int  uiSize;  //结构体字节数
    long   lWidth; 
    long   lHeight; 
    unsigned short usPlanes; 
    unsigned short usBitCount; //每个像素所占二进制位数，可能是1,4,8或 24 
    unsigned int uiCompression; 
    unsigned int uiSizeImage; //像素点阵大小
    long lXPelsPerMeter; // 水平像素数
    long lYPelsPerMeter; 
    unsigned int uiClrUsed; //用的颜色数
    unsigned int uiClrImportant; //重要颜色数
};

class CImage
{
private:
	unsigned char	*m_ucpData;
	unsigned int	m_uiWidth;
	unsigned int	m_uiHeight;
	unsigned int	m_uiBPP;
	bool			m_bIsLoaded;
public:
	bool LoadData( char *szFileName );
	bool LoadBMP( void );
	bool SaveBMP( char* szFilename );
	bool Create( unsigned int uiWidth, unsigned int uiHeight, unsigned int uiBPP );
	
	//--------------------------------------------------------------
	// Name:			CIMAGE::GetColor - public
	// Description:		Get the color (RGB triplet) from a texture pixel
	// Arguments:		-x, y: position to get color from
	//					-ucpRed, ucpGreen, ucpBlue: place to store the RGB
	//					 values that are extracted from the texture
	// Return Value:	None
	//--------------------------------------------------------------
	inline void GetColor( unsigned int x, unsigned  int y, unsigned char* ucpRed, unsigned char* ucpGreen, unsigned char* ucpBlue )
	{
		unsigned int uiBPP= m_uiBPP/8;

		if( ( x<m_uiWidth ) && ( y<m_uiHeight ) )
		{
			*ucpRed  = m_ucpData[( ( y*m_uiHeight )+x )*uiBPP];
			*ucpGreen= m_ucpData[( ( y*m_uiHeight )+x )*uiBPP+1];
			*ucpBlue = m_ucpData[( ( y*m_uiHeight )+x )*uiBPP+2];
		}
	}

	//--------------------------------------------------------------
	// Name:			CIMAGE::SetColor - public
	// Description:		Set the color (RGB triplet) for a texture pixel
	// Arguments:		-x, y: position to set color at
	//					-ucpRed, ucpGreen, ucpBlue: color to set at pixel
	// Return Value:	None
	//--------------------------------------------------------------
	inline void SetColor( unsigned int x, unsigned  int y, unsigned char ucRed, unsigned char ucGreen, unsigned char ucBlue )
	{
		unsigned int uiBPP= m_uiBPP/8;

		if( ( x<m_uiWidth ) && ( y<m_uiHeight ) )
		{
			m_ucpData[( ( y*m_uiHeight )+x )*uiBPP]  = ucRed;
			m_ucpData[( ( y*m_uiHeight )+x )*uiBPP+1]= ucGreen;
			m_ucpData[( ( y*m_uiHeight )+x )*uiBPP+2]= ucBlue;
		}
	}

	//--------------------------------------------------------------
	// Name:			CIMAGE::GetData - public
	// Description:		Get a pointer to the image's data buffer
	// Arguments:		None
	// Return Value:	An unsigned char buffer (the texture's data)
	//--------------------------------------------------------------
	inline unsigned char* GetData( void )
	{	return m_ucpData;	}

	//--------------------------------------------------------------
	// Name:			CIMAGE::GetWidth - public
	// Description:		Get the texture's width (size along the x-axis)
	// Arguments:		None
	// Return Value:	An unsigned int value: the texture's size along the x-axis
	//--------------------------------------------------------------
	inline unsigned int GetWidth( void )
	{	return m_uiWidth;	}

	//--------------------------------------------------------------
	// Name:			CIMAGE::GetHeight - public
	// Description:		Get the texture's height (size along the y-axis)
	// Arguments:		None
	// Return Value:	An unsigned int value: the texture's size along the y-axis
	//--------------------------------------------------------------
	inline unsigned int GetHeight( void )
	{	return m_uiHeight;	}

	//--------------------------------------------------------------
	// Name:			CIMAGE::GetBPP - public
	// Description:		Get the texture's number of bits per pixel
	// Arguments:		None
	// Return Value:	An unsigned int value: the texture's bits per pixel
	//--------------------------------------------------------------
	inline unsigned int GetBPP( void )
	{	return m_uiBPP;	}

	inline bool IsLoaded( void )
	{	return m_bIsLoaded;	}
};