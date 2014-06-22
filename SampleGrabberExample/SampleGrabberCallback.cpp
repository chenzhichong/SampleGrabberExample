#include "SampleGrabberCallback.h"

SampleGrabberCallback::SampleGrabberCallback()
{
	m_bOneFrame = FALSE;
	//Get template path
	GetTempPath(MAX_PATH,m_chTempPath);
	StringCchCat(m_chTempPath,MAX_PATH,TEXT("SGPic"));
	CreateDirectory(m_chTempPath,NULL);
	m_lWidth = 720;
	m_lHeight = 576;
	m_iBitCount = 24;
	m_lTotalFrame = 0;
	m_bIsFirst = TRUE;
	m_Remain = 0;
	m_CRTPSender.Initialize(DEST_PORT, DEST_IP);
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
	//CUYVY2BMP CUYVY2BMP(m_lWidth, m_lHeight);

	printf("[%s]:Frame[%d]\n", __FUNCTION__, m_lTotalFrame);
	if(!pBuffer)
		return E_POINTER;
	if(TRUE == m_bOneFrame)  //�ж��Ƿ���Ҫdumpһ֡
		SaveRaw(pBuffer, BufferLen);
	//SaveBitmap(pBuffer,BufferLen);
	//SaveRaw(pBuffer, BufferLen);
	//HandleRaw(pBuffer, BufferLen);
	//SaveRawToSequence(pBuffer, BufferLen);
	//SaveRawToTS(pBuffer, BufferLen);
	//SendTS(pBuffer, BufferLen);
	m_bOneFrame = FALSE;
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

BOOL SampleGrabberCallback::SaveRaw(BYTE * pBuffer, long lBufferSize )
{
	//printf("[%s]:hello\n", __FUNCTION__);
	SYSTEMTIME sysTime;
	GetLocalTime(&sysTime);
	StringCchCopy(m_chSwapStr,MAX_PATH,m_chTempPath);
	StringCchPrintf(m_chDirName,MAX_PATH,TEXT("\\%04i%02i%02i%02i%02i%02i%03ione.raw"),
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

	//Write the file Data
	/*
	BYTE *pYUV420Buffer;
	long YUV420BufferSize = m_lWidth*m_lHeight*1.5;
	pYUV420Buffer = (BYTE*) malloc(YUV420BufferSize);
	ZeroMemory(pYUV420Buffer, YUV420BufferSize);
	ConvertYUY2ToYUV420(pBuffer, pYUV420Buffer, m_lWidth, m_lHeight);
	DWORD dwWritten = 0;
	WriteFile(hf, pYUV420Buffer, YUV420BufferSize, &dwWritten, NULL);
	free(pYUV420Buffer);
	*/
	DWORD dwWritten = 0;
	WriteFile(hf, pBuffer, lBufferSize, &dwWritten, NULL);
	CloseHandle(hf);
	return 0;
}
HRESULT SampleGrabberCallback::ConvertYUY2ToYUV420(BYTE *pSrc, BYTE *pDst, int iWidth, int iHeight )
{
	if (!pDst && !pSrc)
		return E_POINTER;
	int iImgSize = iWidth * iHeight * 2;
	int iWidthStep422 = iWidth * 2;

	const BYTE* p422 = pSrc;
	BYTE* p420y = pDst;
	BYTE* p420u = pDst + iImgSize / 2; 
	BYTE* p420v = p420u + iImgSize / 8;

	for(int i = 0; i < iHeight; i += 2)
	{
		p422 = pSrc + i * iWidthStep422;
		for(int j = 0; j < iWidthStep422; j+=4)
		{
			*(p420y++) = p422[j];
			*(p420u++) = p422[j+1];
			*(p420y++) = p422[j+2];
		}
		p422 += iWidthStep422;
		for(int j = 0; j < iWidthStep422; j+=4)
		{
			*(p420y++) = p422[j];
			*(p420v++) = p422[j+3];
			*(p420y++) = p422[j+2];
		}
	}
	return S_OK;
}
HRESULT SampleGrabberCallback::ConvertUYVYToYUV420(BYTE *pSrc, BYTE *pDst, int iWidth, int iHeight )
{
	if (!pDst && !pSrc)
		return E_POINTER;
	int iImgSize = iWidth * iHeight * 2;
	int iWidthStep422 = iWidth * 2;

	const BYTE* p422 = pSrc;
	BYTE* p420y = pDst;
	BYTE* p420u = pDst + iImgSize / 2; 
	BYTE* p420v = p420u + iImgSize / 8;

	for(int i = 0; i < iHeight; i += 2)
	{
		p422 = pSrc + i * iWidthStep422;
		for(int j = 0; j < iWidthStep422; j+=4)
		{
			*(p420y++) = p422[j+1];
			*(p420u++) = p422[j];
			*(p420y++) = p422[j+3];
		}
		p422 += iWidthStep422;
		for(int j = 0; j < iWidthStep422; j+=4)
		{
			*(p420y++) = p422[j+1];
			*(p420v++) = p422[j+2];
			*(p420y++) = p422[j+3];
		}
	}
	return S_OK;
}

BOOL SampleGrabberCallback::HandleRaw(BYTE * pBuffer, long lBufferSize )
{
	BYTE *pYUV420Buffer;
	long YUV420BufferSize = m_lWidth*m_lHeight*1.5;
	pYUV420Buffer = (BYTE*) malloc(YUV420BufferSize);
	ZeroMemory(pYUV420Buffer, YUV420BufferSize);
	switch (WHICH_DEVICE)
	{
	case 0:
		ConvertUYVYToYUV420(pBuffer, pYUV420Buffer, m_lWidth, m_lHeight);
		break;
	case 1:
		ConvertYUY2ToYUV420(pBuffer, pYUV420Buffer, m_lWidth, m_lHeight);
		break;
	default:
		printf("%ls\n", L"û����Ӧ���豸��");
		free(pYUV420Buffer);
		return S_FALSE;
		break;
	}
	free(pYUV420Buffer);
	return TRUE;
}
BOOL SampleGrabberCallback::SaveRawToSequence(BYTE * pBuffer, long lBufferSize )
{
	//printf("[%s]:hello\n", __FUNCTION__);
	if (!m_bIsFirst)
	{
		SYSTEMTIME sysTime;
		GetLocalTime(&sysTime);
		StringCchCopy(m_chSwapStr,MAX_PATH,m_chTempPath);
		/*
		StringCchPrintf(m_chDirName,MAX_PATH,TEXT("\\%04i%02i%02i%02i%02i%02i%03i_Sequence.yuv"),
		sysTime.wYear,sysTime.wMonth,sysTime.wDay,sysTime.wHour,
		sysTime.wMinute,sysTime.wSecond,sysTime.wMilliseconds);
		*/
		StringCchPrintf(m_chDirName,MAX_PATH,TEXT("\\Sequence.yuv"));
		StringCchCat(m_chSwapStr,MAX_PATH,m_chDirName);
		m_bIsFirst = TRUE;
	}
	//MessageBox(NULL,chTempPath,TEXT("Message"),MB_OK);
	//create picture file
	HANDLE hf = CreateFile(m_chSwapStr,GENERIC_WRITE|GENERIC_READ, FILE_SHARE_READ, NULL,
		OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if(hf == INVALID_HANDLE_VALUE)
	{
		return E_FAIL;
	}
	SetFilePointer(hf,0,NULL,FILE_END);
	//Write the file Data
	BYTE *pYUV420Buffer;
	long YUV420BufferSize = m_lWidth*m_lHeight*1.5;
	pYUV420Buffer = (BYTE*) malloc(YUV420BufferSize);
	ZeroMemory(pYUV420Buffer, YUV420BufferSize);
	switch (WHICH_DEVICE)
	{
	case 0:
		ConvertUYVYToYUV420(pBuffer, pYUV420Buffer, m_lWidth, m_lHeight);
		break;
	case 1:
		ConvertYUY2ToYUV420(pBuffer, pYUV420Buffer, m_lWidth, m_lHeight);
		break;
	default:
		printf("%ls\n", L"û����Ӧ���豸��");
		free(pYUV420Buffer);
		CloseHandle(hf);
		return -1;
		break;
	}
	DWORD dwWritten = 0;
	//WriteFile(hf, pBuffer, lBufferSize, &dwWritten, NULL);
	WriteFile(hf, pYUV420Buffer, YUV420BufferSize, &dwWritten, NULL);
	free(pYUV420Buffer);
	CloseHandle(hf);
	return 0;
}
BOOL SampleGrabberCallback::SaveRawToTS(BYTE * pBuffer, long lBufferSize )
{
	//printf("[%s]:hello\n", __FUNCTION__);
	if (m_bIsFirst)
	{
		SYSTEMTIME sysTime;
		GetLocalTime(&sysTime);
		StringCchCopy(m_chSwapStr,MAX_PATH,m_chTempPath);
		/*
		StringCchPrintf(m_chDirName,MAX_PATH,TEXT("\\%04i%02i%02i%02i%02i%02i%03i_Sequence.yuv"),
		sysTime.wYear,sysTime.wMonth,sysTime.wDay,sysTime.wHour,
		sysTime.wMinute,sysTime.wSecond,sysTime.wMilliseconds);
		*/
		StringCchPrintf(m_chDirName,MAX_PATH,TEXT("\\Sequence.ts"));
		StringCchCat(m_chSwapStr,MAX_PATH,m_chDirName);
		m_bIsFirst = FALSE;
	}
	//MessageBox(NULL,chTempPath,TEXT("Message"),MB_OK);
	//create picture file
	HANDLE hf = CreateFile(m_chSwapStr,GENERIC_WRITE|GENERIC_READ, FILE_SHARE_READ, NULL,
		OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if(hf == INVALID_HANDLE_VALUE)
	{
		return E_FAIL;
	}
	SetFilePointer(hf,0,NULL,FILE_END);

	DWORD dwWritten = 0;
	WriteFile(hf, pBuffer, lBufferSize, &dwWritten, NULL);
	CloseHandle(hf);
	return 0;
}
void SampleGrabberCallback::SendTS(BYTE * pBuffer, long lBufferSize)
{
	int iLen = TS_PACKET_SIZE*RTP_TS_PACKET_COUNT;
	int iPos = 0;
	long lActualSize = m_Remain ? lBufferSize-(TS_PACKET_SIZE-m_Remain) : lBufferSize;
	if (lActualSize != lBufferSize)
	{
		memcpy(m_RD+m_Remain, pBuffer, TS_PACKET_SIZE-m_Remain);
		m_CRTPSender.Send(m_RD, TS_PACKET_SIZE);
		//��λ��һ֡47��λ��
		iPos = TS_PACKET_SIZE-m_Remain;
	}

	int iCount = lActualSize/iLen;
	int iRD = lActualSize%iLen;
	//�ȷ���7���İ�
	for(int i = 0; i < iCount; i++)
	{
		m_CRTPSender.Send(pBuffer+iPos, iLen);
		iPos += iLen;
	}
	//ʣ�µ�
	if (iRD)
	{
		lActualSize = lActualSize - iCount*iLen;
		iCount = lActualSize/TS_PACKET_SIZE;
		m_CRTPSender.Send(pBuffer+iPos, iCount*TS_PACKET_SIZE);
		iPos += iCount*TS_PACKET_SIZE;
		iRD = lActualSize%TS_PACKET_SIZE;
		if (iRD)
		{
			memcpy(m_RD, pBuffer+iPos, iRD);
			m_Remain = iRD;
		} else
		{
			memset(m_RD, 0, TS_PACKET_SIZE);
			m_Remain = 0;
		}
	}
}