#include "CImage.h"

//--------------------------------------------------------------
// Name:			CIMAGE::Create - public
// Description:		Create space for use with a new texture
// Arguments:		-uiWidth, uiHeight: the dimensions of the new image
//					-uiBPP: the bits per pixel for the new image
// Return Value:	A boolean variable: -true: memory was successfully allocated
//									    -false: memory was not successfully allocated
//--------------------------------------------------------------
bool CImage::Create(unsigned int uiWidth, unsigned int uiHeight, unsigned int uiBPP)
{
	m_uiWidth = uiWidth;
	m_uiHeight = uiHeight;
	m_uiBPP = uiBPP;

	m_ucpData = new unsigned char[uiWidth * uiHeight * (uiBPP / 8)];
	if(! m_ucpData)
	{

		return false;
	}

	m_bIsLoaded = true;

	return true;
}

//--------------------------------------------------------------
// Name:			CIMAGE::LoadData - public
// Description:		Load only the data for a new image (do not create an
//					OpenGL texture)
// Arguments:		-szFilename: the file to load in
// Return Value:	A boolean variable: -true: data was successfully loaded
//									    -false: data was not successfully loaded
//--------------------------------------------------------------
bool CImage::LoadData( char *szFileName )
{
	FILE *pFile;
	int iStart;
	int iEnd;
	int iSize;

	//open the file for reading (in binary mode)
	pFile= fopen( szFileName, "rb" );

	//check to see if we were able to open the file
	if( pFile==NULL )
	{
		
		return false;
	}
		
	//Get file length
	fseek( pFile, 0, SEEK_END );
	iEnd = ftell( pFile );
	fseek( pFile, 0, SEEK_SET );
	iStart = ftell( pFile );
	iSize = iEnd - iStart;

	//allocate the data buffer (temporary)
	m_ucpData= new unsigned char [iSize];

	//read the image's data into the buffer
	if( fread( m_ucpData, sizeof( unsigned char ), iSize, pFile ) != ( unsigned )iSize )
	{
		//the file is corrupted
		if( m_ucpData )
		{
			delete[] m_ucpData;
			m_ucpData = NULL;
		}
		return false;
	}

	//check to see if the file is in the BMP format
	if( memcmp( m_ucpData, "BM", 2 )==0 )
	{
		//load the BMP using the BMP-loading routine
		if( !LoadBMP( ) )
		{
			//could not load the BMP file
			if( m_ucpData )
			{
				delete[] m_ucpData;
				m_ucpData= NULL;
			}
			return false;
		}
	}
	else 
	{
		if( m_ucpData )
		{
			delete[] m_ucpData;
			m_ucpData= NULL;
		}
		return false;
	}

	//the file's data was successfully loaded
	m_bIsLoaded= true;

	return true;
}

//--------------------------------------------------------------
// Name:			CIMAGE::LoadBMP - private
// Description:		The routine to load a windows bitmap (BMP)
// Arguments:		None
// Return Value:	A boolean variable: -true: BMP was loaded
//									    -false: BMP was not loaded
//--------------------------------------------------------------
bool CImage::LoadBMP( void )
{
	BMPFileHeader* fileHeader;
	BMPInfoHeader* infoHeader;
	unsigned int iImageSize;
	unsigned char uiTempRGB;
	unsigned int i;
	unsigned char* ucpFile= m_ucpData;

	//load in the file header
	fileHeader= ( BMPFileHeader* )ucpFile;

	//advance the buffer, and load in the file information header
	ucpFile  += sizeof( BMPFileHeader );
	infoHeader= ( BMPInfoHeader* )ucpFile;
	
	//advance the buffer to load int he actual image data
	ucpFile  += sizeof( BMPInfoHeader );
	ucpFile  += fileHeader->uiOffBits;

	//initialize the image memory
	iImageSize= fileHeader->uiSize - fileHeader->uiOffBits;
	Create( infoHeader->lWidth, infoHeader->lHeight, infoHeader->usBitCount );

	//copy the data to the class's data buffer
	memcpy( m_ucpData, ucpFile, iImageSize );

	//swap the R and B values to get RGB since the bitmap color format is in BGR
	for( i=0; i<infoHeader->uiSizeImage; i+=3 )
	{
		uiTempRGB	  = m_ucpData[i];
		m_ucpData[i]  = m_ucpData[i+2];
		m_ucpData[i+2]= uiTempRGB;
	}

	//the BMP has been successfully loaded
	return true;
}

//--------------------------------------------------------------
// Name:			CIMAGE::SaveBMP - private
// Description:		The routine to save a windows bitmap (BMP)
// Arguments:		-szFilename: Filename to save the image to
// Return Value:	A boolean variable: -true: BMP was saved
//									    -false: BMP was not saved
//--------------------------------------------------------------
bool CImage::SaveBMP( char* szFilename )
{
	FILE*		   pFile;
	BMPFileHeader bitmapFileHeader;
	BMPInfoHeader bitmapInfoHeader;
	unsigned char  ucTempRGB;
	unsigned int   i;

	//open a file that we can write to
	pFile= fopen( szFilename, "wb" );
	if( !pFile )
	{
		
		return false;
	}

	//define the BMP file header
	bitmapFileHeader.uiSize	    = m_uiWidth*m_uiHeight*3+sizeof( BMPFileHeader )+sizeof( BMPInfoHeader );
	bitmapFileHeader.usType	    = 0x4d42;//¡°BM¡±
	bitmapFileHeader.usReserved1= 0;
	bitmapFileHeader.usReserved2= 0;
	bitmapFileHeader.uiOffBits  = sizeof( BMPFileHeader )+sizeof( BMPInfoHeader );

	//write the .bmp file header to the file
	fwrite( &bitmapFileHeader, 1, sizeof( BMPFileHeader ), pFile );

	//define the BMP information header
	bitmapInfoHeader.uiSize		   = sizeof( BMPInfoHeader );
	bitmapInfoHeader.usPlanes	   = 1;
	bitmapInfoHeader.usBitCount	   = 24;
	bitmapInfoHeader.uiCompression = BI_RGB;
	bitmapInfoHeader.uiSizeImage   = m_uiWidth*m_uiHeight*3;
	bitmapInfoHeader.lXPelsPerMeter= 0;
	bitmapInfoHeader.lYPelsPerMeter= 0;
	bitmapInfoHeader.uiClrUsed	   = 0;
	bitmapInfoHeader.uiClrImportant= 0;
	bitmapInfoHeader.lWidth		   = m_uiWidth;
	bitmapInfoHeader.lHeight	   = m_uiHeight;

	//write the .bmp file information header to the file
	fwrite( &bitmapInfoHeader, 1, sizeof( BMPInfoHeader ), pFile );

	//swap the image bit order (RGB to BGR)
	for( i=0; i<bitmapInfoHeader.uiSizeImage; i+=3 )
	{
		ucTempRGB	  = m_ucpData[i];
		m_ucpData[i]  = m_ucpData[i+2];
		m_ucpData[i+2]= ucTempRGB;
	}

	//now write the actual image data
	fwrite( m_ucpData, 1, bitmapInfoHeader.uiSizeImage, pFile );

	//the file has been successfully saved
	fclose( pFile );
	return true;
}