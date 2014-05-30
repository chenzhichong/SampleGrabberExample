#include "CUYVY2BMP.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CUYVY2BMP::CUYVY2BMP(int width, int height)
{
	m_nWidth = width;
	m_nHeight = height;
	MakeConversionTable();
}
CUYVY2BMP::~CUYVY2BMP()
{
}

///accessor 
int CUYVY2BMP::GetHeight()
{
	return m_nHeight;
}
int CUYVY2BMP::GetWidth()
{
	return m_nWidth;
}
void CUYVY2BMP::MakeConversionTable()
{
	for (long j = 0; j < 256; ++j) 
	{ 
		m_table_UYVY2rgb.YtoR[j] = m_table_UYVY2rgb.YtoG[j] = m_table_UYVY2rgb.YtoB[j] = (unsigned short)(j << 7); 
		m_table_UYVY2rgb.VtoR[j] = j * 180;     //180=1.402*128 
		m_table_UYVY2rgb.VtoG[j] = j * 91; 
		m_table_UYVY2rgb.UtoG[j] = j * 44;      //0.3437 = 44/128
		m_table_UYVY2rgb.UtoB[j] = j * 226;      //1.772 = 226/128
	} 
}
BOOL CUYVY2BMP::UYVY2BMP(BYTE* pDst, BYTE *pSrc)
{
	long m = 0; 
	long k = 0; 
	int n = m_nWidth/2; 
	int dec = m_nWidth*4; 

	int tmpR0 = 0; 
	int tmpG0 = 0; 
	int tmpB0 = 0; 
	int tmpR1 = 0; 
	int tmpG1 = 0; 
	int tmpB1 = 0; 
	//MakeConversionTable();
	k = (m_nHeight-1)*m_nWidth<<1;         //k指向最后一行
	for( int i=m_nHeight-1;i>-1;i--) 
	{ 

		for(int j=0;j<n;j++) 
		{ 
			tmpR0 = (m_table_UYVY2rgb.YtoR[pSrc[k + 1]] + m_table_UYVY2rgb.VtoR[pSrc[k + 2]] - 22906) >> 7; 
			tmpG0 = (m_table_UYVY2rgb.YtoG[pSrc[k + 1]] - m_table_UYVY2rgb.VtoG[pSrc[k + 2]] - m_table_UYVY2rgb.UtoG[pSrc[k + 0]] + 17264) >> 7; 
			tmpB0 = (m_table_UYVY2rgb.YtoB[pSrc[k + 1]] + m_table_UYVY2rgb.UtoB[pSrc[k + 0]] - 28928) >> 7; 

			tmpR1 = (m_table_UYVY2rgb.YtoR[pSrc[k + 3]] + m_table_UYVY2rgb.VtoR[pSrc[k + 2]] - 22906) >> 7; 
			tmpG1 = (m_table_UYVY2rgb.YtoG[pSrc[k + 3]] - m_table_UYVY2rgb.VtoG[pSrc[k + 2]] - m_table_UYVY2rgb.UtoG[pSrc[k + 0]] + 17264) >> 7; 
			tmpB1 = (m_table_UYVY2rgb.YtoB[pSrc[k + 3]] + m_table_UYVY2rgb.UtoB[pSrc[k + 0]] - 28928) >> 7; 

			clip(0, tmpR0, 255);
			clip(0, tmpG0, 255);
			clip(0, tmpB0, 255);
			clip(0, tmpR1, 255);
			clip(0, tmpG1, 255);
			clip(0, tmpB1, 255);

			pDst[m + 0] = tmpB0;   
			pDst[m + 1] = tmpG0;   
			pDst[m + 2] = tmpR0;   
			pDst[m + 3] = tmpB1;   
			pDst[m + 4] = tmpG1;   
			pDst[m + 5] = tmpR1;   

			k       +=       4;   
			m       +=       6;   
		}   
		k=k-dec; 
	}
	return 1;
}
BOOL CUYVY2BMP::WriteBMPFile(PCWSTR BMPFilename, BYTE *pRGBBuf, int iBitCount)
{
	long RGB_SIZE = m_nWidth * m_nHeight * 3;
	if(iBitCount == 24)
	{
		long count=0;
		BITMAPFILEHEADER bmpHeader;
		BITMAPINFO bmpInfo;

		HANDLE hOpenFile = (HANDLE)CreateFile(BMPFilename, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, 0, NULL);
		if (hOpenFile == INVALID_HANDLE_VALUE)
		{
			hOpenFile = NULL;
			//MessageBoxA(NULL, "Can not open the file", "CUYVY2BMP", MB_OK);
			return -1;
		}

		bmpHeader.bfType = 'MB';
		bmpHeader.bfSize = RGB_SIZE + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
		bmpHeader.bfReserved1 = 0;
		bmpHeader.bfReserved2 = 0;
		bmpHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
		bmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		bmpInfo.bmiHeader.biWidth = m_nWidth;
		bmpInfo.bmiHeader.biHeight = m_nHeight;
		bmpInfo.bmiHeader.biPlanes = 1;
		bmpInfo.bmiHeader.biBitCount = 24;
		bmpInfo.bmiHeader.biCompression = BI_RGB;
		bmpInfo.bmiHeader.biSizeImage = RGB_SIZE;
		bmpInfo.bmiHeader.biXPelsPerMeter = 0;
		bmpInfo.bmiHeader.biYPelsPerMeter = 0;
		bmpInfo.bmiHeader.biClrUsed = 0;
		bmpInfo.bmiHeader.biClrImportant = 0;

		DWORD dwWritten = 0;
		WriteFile(hOpenFile, &bmpHeader, sizeof(BITMAPFILEHEADER), &dwWritten, NULL);  
		WriteFile(hOpenFile, &(bmpInfo.bmiHeader), sizeof(BITMAPINFOHEADER), &dwWritten, NULL);   
		WriteFile(hOpenFile, pRGBBuf, RGB_SIZE, &dwWritten, NULL); 
		CloseHandle(hOpenFile);
		return TRUE;
	}
	else
		return FALSE;
}
