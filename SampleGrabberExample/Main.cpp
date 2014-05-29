#include "CaptureVideo.h"

SampleGrabberCallback g_sampleGrabberCB;  //CallBack
CaptureVideo g_CaptureVideo;

HRESULT WriteBitmap(PCWSTR, BITMAPINFOHEADER*, size_t, BYTE*, size_t);

HRESULT GrabVideoBitmap(PCWSTR pszVideoFile, PCWSTR pszBitmapFile)
{
	IGraphBuilder *pGraph = NULL;
	IMediaControl *pControl = NULL;
	IMediaEventEx *pEvent = NULL;
	IBaseFilter *pGrabberF = NULL;
	ISampleGrabber *pGrabber = NULL;
	IBaseFilter *pSourceF = NULL;
	IEnumPins *pEnum = NULL;
	IPin *pPin = NULL;
	IBaseFilter *pNullF = NULL;

	BYTE *pBuffer = NULL;

	 

	HRESULT hr = CoInitialize(NULL);
	if (FAILED(hr))
	{
		goto done;
	}

	hr = CoCreateInstance(CLSID_FilterGraph, NULL, 
		CLSCTX_INPROC_SERVER,IID_PPV_ARGS(&pGraph));
	if (FAILED(hr))
	{
		goto done;
	}

	hr = pGraph->QueryInterface(IID_PPV_ARGS(&pControl));
	if (FAILED(hr))
	{
		goto done;
	}

	hr = pGraph->QueryInterface(IID_PPV_ARGS(&pEvent));
	if (FAILED(hr))
	{
		goto done;
	}

	// Create the Sample Grabber filter.
	hr = CoCreateInstance(CLSID_SampleGrabber, NULL, CLSCTX_INPROC_SERVER,
		IID_PPV_ARGS(&pGrabberF));
	if (FAILED(hr))
	{
		goto done;
	}

	hr = pGraph->AddFilter(pGrabberF, L"Sample Grabber");
	if (FAILED(hr))
	{
		goto done;
	}

	hr = pGrabberF->QueryInterface(IID_PPV_ARGS(&pGrabber));
	if (FAILED(hr))
	{
		goto done;
	}

	AM_MEDIA_TYPE mt;
	ZeroMemory(&mt, sizeof(mt));
	mt.majortype = MEDIATYPE_Video;
	mt.subtype = MEDIASUBTYPE_RGB24;

	hr = pGrabber->SetMediaType(&mt);
	if (FAILED(hr))
	{
		goto done;
	}

	hr = pGraph->AddSourceFilter(pszVideoFile, L"Source", &pSourceF);
	if (FAILED(hr))
	{
		goto done;
	}

	hr = pSourceF->EnumPins(&pEnum);
	if (FAILED(hr))
	{
		goto done;
	}

	while (S_OK == pEnum->Next(1, &pPin, NULL))
	{
		hr = ConnectFilters(pGraph, pPin, pGrabberF);
		SafeRelease(&pPin);
		if (SUCCEEDED(hr))
		{
			break;
		}
	}

	if (FAILED(hr))
	{
		goto done;
	}

	hr = CoCreateInstance(CLSID_NullRenderer, NULL, CLSCTX_INPROC_SERVER, 
		IID_PPV_ARGS(&pNullF));
	if (FAILED(hr))
	{
		goto done;
	}

	hr = pGraph->AddFilter(pNullF, L"Null Filter");
	if (FAILED(hr))
	{
		goto done;
	}

	hr = ConnectFilters(pGraph, pGrabberF, pNullF);
	if (FAILED(hr))
	{
		goto done;
	}

	/*hr = pGrabber->SetOneShot(TRUE);
	if (FAILED(hr))
	{
	goto done;
	}*/

	hr = pGrabber->SetBufferSamples(TRUE);
	if (FAILED(hr))
	{
		goto done;
	}

	hr = pGrabber->SetCallback(&g_sampleGrabberCB, 1);
	if (FAILED(hr))
	{
		goto done;
	}

	hr = pControl->Run();
	if (FAILED(hr))
	{
		goto done;
	}

	long evCode;
	hr = pEvent->WaitForCompletion(5000, &evCode);

	// Find the required buffer size.
	//long cbBuffer;
	//hr = pGrabber->GetCurrentBuffer(&cbBuffer, NULL);
	//if (FAILED(hr))
	//{
	//	goto done;
	//}

	//pBuffer = (BYTE*)CoTaskMemAlloc(cbBuffer);
	//if (!pBuffer) 
	//{
	//	hr = E_OUTOFMEMORY;
	//	goto done;
	//}


	//hr = pGrabber->GetCurrentBuffer(&cbBuffer, (long*)pBuffer);
	//if (FAILED(hr))
	//{
	//	goto done;
	//}

	//hr = pGrabber->GetConnectedMediaType(&mt);
	//if (FAILED(hr))
	//{
	//	goto done;
	//}

	//// Examine the format block.
	//if ((mt.formattype == FORMAT_VideoInfo) && 
	//	(mt.cbFormat >= sizeof(VIDEOINFOHEADER)) &&
	//	(mt.pbFormat != NULL)) 
	//{
	//	VIDEOINFOHEADER *pVih = (VIDEOINFOHEADER*)mt.pbFormat;
	//	wchar_t buffer[64];
	//	swprintf(buffer, L"d://dd/Test%d.bmp", i);
	//	hr = WriteBitmap(buffer, &pVih->bmiHeader, 
	//		mt.cbFormat - SIZE_PREHEADER, pBuffer, cbBuffer);
	//}
	//else 
	//{
	//	// Invalid format.
	//	hr = VFW_E_INVALIDMEDIATYPE; 
	//}

	//FreeMediaType(mt);

	hr = pControl->Stop();
	if (FAILED(hr))
	{
		goto done;
	}

	hr = SaveGraphFile(pGraph, L"D://test.grf");
	if (FAILED(hr))
	{
		printf("保存grf文件失败！");
		goto done;
	}

done:
	CoTaskMemFree(pBuffer);
	SafeRelease(&pPin);
	SafeRelease(&pEnum);
	SafeRelease(&pNullF);
	SafeRelease(&pSourceF);
	SafeRelease(&pGrabber);
	SafeRelease(&pGrabberF);
	SafeRelease(&pControl);
	SafeRelease(&pEvent);
	SafeRelease(&pGraph);
	CoUninitialize();
	return hr;
};

// Writes a bitmap file
//  pszFileName:  Output file name.
//  pBMI:         Bitmap format information (including pallete).
//  cbBMI:        Size of the BITMAPINFOHEADER, including palette, if present.
//  pData:        Pointer to the bitmap bits.
//  cbData        Size of the bitmap, in bytes.

HRESULT WriteBitmap(PCWSTR pszFileName, BITMAPINFOHEADER *pBMI, size_t cbBMI,
					BYTE *pData, size_t cbData)
{
	HANDLE hFile = CreateFile(pszFileName, GENERIC_WRITE, 0, NULL, 
		CREATE_ALWAYS, 0, NULL);
	if (hFile == NULL)
	{
		return HRESULT_FROM_WIN32(GetLastError());
	}

	BITMAPFILEHEADER bmf = { };

	bmf.bfType = 'MB';
	bmf.bfSize = cbBMI+ cbData + sizeof(bmf); 
	bmf.bfOffBits = sizeof(bmf) + cbBMI; 

	DWORD cbWritten = 0;
	BOOL result = WriteFile(hFile, &bmf, sizeof(bmf), &cbWritten, NULL);
	if (result)
	{
		result = WriteFile(hFile, pBMI, cbBMI, &cbWritten, NULL);
	}
	if (result)
	{
		result = WriteFile(hFile, pData, cbData, &cbWritten, NULL);
	}

	HRESULT hr = result ? S_OK : HRESULT_FROM_WIN32(GetLastError());

	CloseHandle(hFile);

	return hr;
}

int main()
{
	printf("hello world!\n");
	//HRESULT hr = GrabVideoBitmap(L"D://0.avi", L"d://test");
	HRESULT hr = g_CaptureVideo.SaveGraphToFile(L"d://test.grf");
	hr = g_CaptureVideo.SetGrabberProperty();
	if (FAILED(hr))
		//printf("调用GrabVideoBitmap失败!\n");
		ShowError(hr);
	return 0;
}