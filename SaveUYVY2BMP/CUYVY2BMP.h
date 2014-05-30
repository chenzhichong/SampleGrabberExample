#ifndef __CUYVY2BMP_H__  
#define __CUYVY2BMP_H__  

#include <windows.h>
#include <malloc.h>
#include <string>
using namespace std;
#define clip(min, x, max) x=(x < min) ? min : (x > max) ? max : x

typedef struct tagTABLE_UYVY2RGB
{
	unsigned short YtoR[256];
	unsigned short YtoG[256];
	unsigned short YtoB[256];
	unsigned short UtoG[256];
	unsigned short UtoB[256];
	unsigned short VtoR[256];
	unsigned short VtoG[256];
}TABLE_UYVY2RGB;

class CUYVY2BMP
{
public:
	CUYVY2BMP(int width, int height);
	~CUYVY2BMP();

	///accessor 
	int CUYVY2BMP::GetHeight();
	int CUYVY2BMP::GetWidth();
	void MakeConversionTable();
	BOOL UYVY2BMP(BYTE* pDst, BYTE *pSrc);
	BOOL WriteBMPFile(PCWSTR BMPFilename, BYTE *pRGBBuf, int iBitCount);
private:
	int m_nWidth;
	int m_nHeight;
	TABLE_UYVY2RGB m_table_UYVY2rgb;
};

#endif// __CUYVY2BMP_H__    