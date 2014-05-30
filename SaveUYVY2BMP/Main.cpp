#include "CUYVY2BMP.h"

int main()
{
	CUYVY2BMP test(720, 576);
	//int n = test.GetWidth()/2;
	HANDLE hOpenFile = (HANDLE)CreateFile(L"d://1.uyvy", GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, NULL, NULL);
    if (hOpenFile == INVALID_HANDLE_VALUE)
    {
        hOpenFile = NULL;
        MessageBoxA(NULL, "Can not open the file", "CUYVY2BMP", MB_OK);
		return -1;
    }
	BYTE * pSrcBuf=(BYTE *)malloc(test.GetHeight()*test.GetWidth()*2);
	BYTE * pDstBuf=(BYTE *)malloc(test.GetHeight()*test.GetWidth()*3);
	DWORD readsize;
	ReadFile(hOpenFile, pSrcBuf, test.GetHeight()*test.GetWidth()*2, &readsize, NULL);

	test.UYVY2BMP(pDstBuf,pSrcBuf);
	test.WriteBMPFile(L"D://402.bmp",pDstBuf,24);
	CloseHandle(hOpenFile);
	TCHAR testDtr[] = L"hello world";
	printf("%ls\n", testDtr);
	return 0;
}
