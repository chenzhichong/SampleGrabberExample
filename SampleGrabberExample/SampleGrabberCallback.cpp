#include "SampleGrabberCallback.h"

SampleGrabberCallback::SampleGrabberCallback()
{
	m_bGetPicture = TRUE;
	//Get template path
	GetTempPath(MAX_PATH,m_chTempPath);
	StringCchCat(m_chTempPath,MAX_PATH,TEXT("SGPic"));
	CreateDirectory(m_chTempPath,NULL);
	m_lWidth = 1024;
	m_lHeight = 768;
	m_iBitCount = 24;
	m_lTotalFrame = 0;
}

ULONG STDMETHODCALLTYPE SampleGrabberCallback::AddRef()
{
	return 1;
}

ULONG STDMETHODCALLTYPE SampleGrabberCallback::Release()
{
	return 2;
}

HRESULT STDMETHODCALLTYPE SampleGrabberCallback::QueryInterface(REFIID riid,void** ppvObject)
{
	if (NULL == ppvObject) return E_POINTER;
	if (riid == __uuidof(IUnknown))
	{
		*ppvObject = static_cast<IUnknown*>(this);
		return S_OK;
	}
	if (riid == IID_ISampleGrabberCB)
	{
		*ppvObject = static_cast<ISampleGrabberCB*>(this);
		return S_OK;
	}
	return E_NOTIMPL;

}

HRESULT STDMETHODCALLTYPE SampleGrabberCallback::SampleCB(double Time, IMediaSample *pSample)
{
	m_lTotalFrame++;
	printf("[%s]:Frame[%d]\n", __FUNCTION__, m_lTotalFrame);
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE SampleGrabberCallback::BufferCB(double Time, BYTE *pBuffer, long BufferLen)
{
	m_lTotalFrame++;
	printf("[%s]:Frame[%d]\n", __FUNCTION__, m_lTotalFrame);
	if(FALSE == m_bGetPicture)  //�ж��Ƿ���Ҫ��ͼ
		return S_FALSE;
	if(!pBuffer)
		return E_POINTER;

	SaveBitmap(pBuffer,BufferLen);

	//m_bGetPicture = FALSE;
	return S_OK;
}

BOOL SampleGrabberCallback::SaveBitmap(BYTE * pBuffer, long lBufferSize )
{
	printf("[%s]:hello\n", __FUNCTION__);
	SYSTEMTIME sysTime;
	GetLocalTime(&sysTime);
	StringCchCopy(m_chSwapStr,MAX_PATH,m_chTempPath);
	StringCchPrintf(m_chDirName,MAX_PATH,TEXT("\\%04i%02i%02i%02i%02i%02i%03ione.bmp"),
					sysTime.wYear,sysTime.wMonth,sysTime.wDay,sysTime.wHour,
					sysTime.wMinute,sysTime.wSecond,sysTime.wMilliseconds);
	StringCchCat(m_chSwapStr,MAX_PATH,m_chDirName);
	//MessageBox(NULL,chTempPath,TEXT("Message"),MB_OK);
	//create picture file
	HANDLE hf = CreateFile(m_chSwapStr,GENERIC_WRITE,FILE_SHARE_WRITE,NULL,
		CREATE_ALWAYS,0,NULL);
	if(hf == INVALID_HANDLE_VALUE)
	{
		return E_FAIL;
	}

	BITMAPFILEHEADER bfh;  //Set bitmap header
	ZeroMemory(&bfh,sizeof(bfh));
	bfh.bfType = 'MB';
	bfh.bfSize = sizeof(bfh) + lBufferSize + sizeof(BITMAPFILEHEADER);
	bfh.bfOffBits = sizeof(BITMAPFILEHEADER)+sizeof(BITMAPFILEHEADER);
	// Write the file header.
	DWORD dwWritten = 0;
	WriteFile( hf, &bfh, sizeof( bfh ), &dwWritten, NULL );    
	// Write the file Format
	BITMAPINFOHEADER bih;
	ZeroMemory(&bih,sizeof(bih));
	bih.biSize = sizeof( bih );
	bih.biWidth = m_lWidth;
	bih.biHeight = m_lHeight;
	bih.biPlanes = 1;
	bih.biBitCount = m_iBitCount;  //Specifies the number of bits per pixel (bpp)
	WriteFile( hf, &bih, sizeof( bih ), &dwWritten, NULL );
	//Write the file Data
	WriteFile( hf, pBuffer, lBufferSize, &dwWritten, NULL );
	CloseHandle( hf );
	return 0;
}