#include "CaptureVideo.h"

SampleGrabberCallback g_sampleGrabberCB;  //CallBack

CaptureVideo::CaptureVideo()
{
	//COM Library Initialize
	if (FAILED(CoInitialize(NULL)))
	{
		printf("CoInitialize Failed!\n");
		return;
	}
	//initialize member variable
	//m_nCaptureDeviceNumber = 0;
	m_pGrabberDevFilter = NULL;
	m_pRenderFilter = NULL;
	m_pCrossbarFilter = NULL;
	m_pCaptureGB = NULL;
	m_pGraphBuilder = NULL;
	m_pMediaControl = NULL;
	//m_pMediaEvent = NULL;
	m_pSampGrabberFilter = NULL;
	m_pSampGrabber = NULL;
	//m_pVideoWindow = NULL;
	//m_App = NULL;
	m_bGetOneShot = FALSE;
	//m_bConnect = FALSE;
	InitializeEnv();
}

CaptureVideo::~CaptureVideo()
{
	CloseInterface();
	CoUninitialize();
}

//HRESULT CaptureVideo::EnumAllDevices(HWND hCombox)
//{
//	if (!hCombox)
//		return S_FALSE;
//	ICreateDevEnum *pDevEnum;
//	IEnumMoniker   *pEnumMon;
//	IMoniker	   *pMoniker;
//	HRESULT hr = CoCreateInstance(CLSID_SystemDeviceEnum,NULL,CLSCTX_INPROC_SERVER,
//			IID_ICreateDevEnum,(LPVOID*)&pDevEnum);
//	if (SUCCEEDED(hr))
//	{
//		hr = pDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory,&pEnumMon, 0);
//		if (hr == S_FALSE)
//		{
//			hr = VFW_E_NOT_FOUND;
//			return hr;
//		}
//		pEnumMon->Reset();
//		ULONG cFetched;
//		while(hr=pEnumMon->Next(1,&pMoniker,&cFetched),hr == S_OK)
//		{
//			IPropertyBag *pProBag;
//			hr = pMoniker->BindToStorage(0,0,IID_IPropertyBag,(LPVOID*)&pProBag);
//			if (SUCCEEDED(hr))
//			{
//				VARIANT varTemp;
//				varTemp.vt = VT_BSTR;
//				hr = pProBag->Read(L"FriendlyName",&varTemp,NULL);
//				if (SUCCEEDED(hr))
//				{
//					//int nStrSize = WideCharToMultiByte(CP_ACP,0,varTemp.bstrVal,-1,0,0,NULL,FALSE);
//					//char *strName = new char[nStrSize];
//					//WideCharToMultiByte(CP_ACP,0,varTemp.bstrVal,-1,strName,nStrSize,NULL,FALSE);
//					//m_vecCapDeviceName.push_back(string(strName)); //push the device name to vector
//					
//					StringCchCopy(m_pCapDeviceName[m_nCaptureDeviceNumber],MAX_PATH,varTemp.bstrVal);
//					m_nCaptureDeviceNumber++; //Device number add 1
//					::SendMessage(hCombox, CB_ADDSTRING, 0,(LPARAM)varTemp.bstrVal);
//					SysFreeString(varTemp.bstrVal);
//				}
//				pProBag->Release();
//			}
//			pMoniker->Release();
//		}
//		pEnumMon->Release();
//	}
//	return hr;
//}

HRESULT CaptureVideo::InitializeEnv()
{
	HRESULT hr = S_OK;

	//Create the filter graph
	hr = CoCreateInstance(CLSID_FilterGraph,NULL,CLSCTX_INPROC_SERVER,
		IID_IGraphBuilder,(LPVOID*)&m_pGraphBuilder);
	if(FAILED(hr))
		return hr;

	//Create the capture graph builder
	hr = CoCreateInstance(CLSID_CaptureGraphBuilder2,NULL,CLSCTX_INPROC_SERVER,
		IID_ICaptureGraphBuilder2,(LPVOID*)&m_pCaptureGB);
	if(FAILED(hr))
		return hr;

	//Obtain interfaces for media control and Video Window
	hr = m_pGraphBuilder->QueryInterface(IID_IMediaControl,(LPVOID*)&m_pMediaControl);
	if(FAILED(hr))
		return hr;

	/*hr = m_pGraphBuilder->QueryInterface(IID_IVideoWindow,(LPVOID*)&m_pVideoWindow);
	if(FAILED(hr))
	return hr;*/

	/*hr = m_pGraphBuilder->QueryInterface(IID_IMediaEventEx,(LPVOID*)&m_pMediaEvent);
	if(FAILED(hr))
	return hr;*/

	/*hr = m_pMediaEvent->SetNotifyWindow((OAHWND)m_App,WM_GRAPHNOTIFY,0);
	if(FAILED(hr))
	return hr;*/

	m_pCaptureGB->SetFiltergraph(m_pGraphBuilder);
	if(FAILED(hr))
		return hr;

	//add Grabber devcie 
	hr = CreateFilterByName(L"SMI Grabber Device", CLSID_VideoCaptureSources, &m_pGrabberDevFilter);
	if(FAILED(hr))
	{
		printf("´´½¨SMI Grabber DeviceÊ§°Ü£¡");
		return hr;
	}
	hr = m_pGraphBuilder->AddFilter(m_pGrabberDevFilter, L"SMI Grabber Device");
	if(FAILED(hr))
	{
		printf("Ìí¼ÓSMI Grabber DeviceÊ§°Ü£¡");
		return hr;
	}

	//add SM BDA Crossbar Filter 
	hr = CreateFilterByName(L"SM BDA Crossbar Filter", CLSID_WDM, &m_pCrossbarFilter);
	if(FAILED(hr))
	{
		printf("´´½¨SM BDA Crossbar FilterÊ§°Ü£¡");
		return hr;
	}
	hr = m_pGraphBuilder->AddFilter(m_pCrossbarFilter, L"SM BDA Crossbar Filter");
	if(FAILED(hr))
	{
		printf("Ìí¼ÓSM BDA Crossbar FilterÊ§°Ü£¡");
		return hr;
	}

	//add SampleGrabber
	hr = CoCreateInstance(CLSID_SampleGrabber, NULL, CLSCTX_INPROC_SERVER,
		IID_IBaseFilter, (LPVOID*)&m_pSampGrabberFilter);
	if(FAILED(hr))
	{
		printf("´´½¨SampleGrabber FilterÊ§°Ü£¡");
		return hr;
	}
	hr = m_pGraphBuilder->AddFilter(m_pSampGrabberFilter, L"SampleGrabber");
	if(FAILED(hr))
	{
		printf("Ìí¼ÓSampleGrabber FilterÊ§°Ü£¡");
		return hr;
	} 
	hr = m_pSampGrabberFilter->QueryInterface(IID_ISampleGrabber, (LPVOID*)&m_pSampGrabber);
	if (FAILED(hr))
	{
		printf("²éÑ¯ISampleGrabber½Ó¿ÚÊ§°Ü£¡");
		return hr;
	}	

	//add Null Renderer
	hr = CoCreateInstance(CLSID_NullRenderer, NULL, CLSCTX_INPROC_SERVER,
		IID_IBaseFilter, (LPVOID*)&m_pRenderFilter);
	if(FAILED(hr))
	{
		printf("´´½¨Null Renderer FilterÊ§°Ü£¡");
		return hr;
	}
	hr = m_pGraphBuilder->AddFilter(m_pRenderFilter, L"Null Renderer");
	if(FAILED(hr))
	{
		printf("Ìí¼ÓNull Renderer FilterÊ§°Ü£¡");
		return hr;
	}

	//connect SM BDA Crossbar Filter and SMI Grabber Device
	IPin *pPinOut = NULL;
	IPin *pPinIn = NULL;
	hr = GetPin(m_pCrossbarFilter, L"0: Video Decoder Out", &pPinOut);
	if(FAILED(hr))
	{
		printf("[0: Video Decoder Out],GetPinÊ§°Ü£¡");
		return hr;
	}
	hr = GetPin(m_pGrabberDevFilter, L"Video Tuner In", &pPinIn);
	if(FAILED(hr))
	{
		printf("[Video Tuner In],GetPinÊ§°Ü£¡");
		return hr;
	}
	hr = m_pGraphBuilder->ConnectDirect(pPinOut, pPinIn, NULL);
	if(FAILED(hr))
	{
		printf("Á¬½ÓÊ§°Ü£¡");
		SafeRelease(&pPinOut);
		SafeRelease(&pPinIn);
		return hr;
	}
	SafeRelease(&pPinOut);
	SafeRelease(&pPinIn);

	//connect SMI Grabber Device and SampleGrabber
	hr = GetPin(m_pGrabberDevFilter, L"Capture Out", &pPinOut);
	if(FAILED(hr))
	{
		printf("[Capture Out],GetPinÊ§°Ü£¡");
		return hr;
	}
	hr = GetPin(m_pSampGrabberFilter, L"Input", &pPinIn);
	if(FAILED(hr))
	{
		printf("[Input],GetPinÊ§°Ü£¡");
		return hr;
	}
	hr = m_pGraphBuilder->ConnectDirect(pPinOut, pPinIn, NULL);
	if(FAILED(hr))
	{
		printf("Á¬½ÓÊ§°Ü£¡");
		SafeRelease(&pPinOut);
		SafeRelease(&pPinIn);
		return hr;
	}
	SafeRelease(&pPinOut);
	SafeRelease(&pPinIn);

	//connect SampleGrabber and Null Render
	hr = GetPin(m_pSampGrabberFilter, L"Output", &pPinOut);
	if(FAILED(hr))
	{
		printf("[Output],GetPinÊ§°Ü£¡");
		return hr;
	}
	hr = GetPin(m_pRenderFilter, L"In", &pPinIn);
	if(FAILED(hr))
	{
		printf("[In],GetPinÊ§°Ü£¡");
		return hr;
	}
	hr = m_pGraphBuilder->ConnectDirect(pPinOut, pPinIn, NULL);
	if(FAILED(hr))
	{
		printf("Á¬½ÓÊ§°Ü£¡");
		SafeRelease(&pPinOut);
		SafeRelease(&pPinIn);
		return hr;
	}
	SafeRelease(&pPinOut);
	SafeRelease(&pPinIn);

	return hr;
}

HRESULT CaptureVideo::CreateFilterByName(PCWSTR pszFilterName, const GUID& clsidDeviceClass, IBaseFilter** ppFilter)
{
	HRESULT hr = S_OK;
	if (!ppFilter)
		return E_POINTER;

	// Create the System Device Enumerator.
	ICreateDevEnum *pSysDevEnum = NULL;
	hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC_SERVER,
		IID_ICreateDevEnum, (void **)&pSysDevEnum);
	if (FAILED(hr))
	{
		return hr;
	}

	// Obtain a class enumerator for the video compressor category.
	IEnumMoniker *pEnumCat = NULL;
	hr = pSysDevEnum->CreateClassEnumerator(clsidDeviceClass, &pEnumCat, 0);

	if (hr == S_OK) 
	{
		// Enumerate the monikers.
		IMoniker *pMoniker = NULL;
		ULONG cFetched;
		while(pEnumCat->Next(1, &pMoniker, &cFetched) == S_OK)
		{
			IPropertyBag *pPropBag;
			hr = pMoniker->BindToStorage(0, 0, IID_IPropertyBag, 
				(void **)&pPropBag);
			if (SUCCEEDED(hr))
			{
				// To retrieve the filter's friendly name, do the following:
				VARIANT varName;
				VariantInit(&varName);
				hr = pPropBag->Read(L"FriendlyName", &varName, 0);
				if (SUCCEEDED(hr))
				{
					if (wcscmp(pszFilterName, varName.bstrVal) == 0)
					{
						// To create an instance of the filter, do the following:
						hr = pMoniker->BindToObject(NULL, NULL, IID_IBaseFilter,
							(void**)ppFilter);
						if (FAILED(hr))
						{
							printf("Can't bind moniker to filter object.\n");
						}
						printf("Success bind moniker [%s] to filter object.\n", pszFilterName);
						pPropBag->Release();
						pMoniker->Release();
						pEnumCat->Release();
						pSysDevEnum->Release();
						return hr;
						// Now add the filter to the graph. 
					}
				}
				VariantClear(&varName);
				//Remember to release pFilter later.
				pPropBag->Release();
			}
			pMoniker->Release();
		}
		pEnumCat->Release();
	}
	pSysDevEnum->Release();
	return hr;
}

HRESULT CaptureVideo::GetPin(IBaseFilter* pFilter, PCWSTR pszPinName, IPin** ppPin)
{
	HRESULT hr = S_OK;
	if (!ppPin)
		return E_POINTER;

	IEnumPins *pEnum;
	hr = pFilter->EnumPins(&pEnum);
	if (FAILED(hr))
	{
		printf("Can't enumerate pins.");
		return hr;
	}

	while (pEnum->Next(1, ppPin, 0) == S_OK)
	{
		PIN_INFO pInfo;
		hr = (*ppPin)->QueryPinInfo(&pInfo);
		BOOL bFound = !wcsicmp(pszPinName, pInfo.achName);
		if (pInfo.pFilter)
		{
			pInfo.pFilter->Release();
		}
		if (bFound)
		{
			return hr;
		}
	}
	return hr;
}

HRESULT CaptureVideo::SaveGraphToFile(WCHAR *wszPath)
{
	HRESULT hr = S_OK;
	if (!m_pGraphBuilder)
		return E_POINTER;

	hr = SaveGraphFile(m_pGraphBuilder, wszPath);
	if (FAILED(hr))
	{
		printf("±£´æGraphÊ§°Ü£¡");
		return hr;
	}
	return hr;
}

HRESULT CaptureVideo::SetProperty(IBaseFilter* pFilter)
{
	HRESULT hr = S_OK;
	if (!pFilter)
		return E_POINTER;

	hr = ShowFilterPropertyPage(pFilter, NULL);
	if (FAILED(hr))
	{
		//printf("ÏÔÊ¾ÊôÐÔ¶Ô»°¿òÊ§°Ü£¡");
		return hr;
	}
	return hr;
}

HRESULT CaptureVideo::SetGrabberProperty()
{
	HRESULT hr = S_OK;
	hr = ShowFilterPropertyPage(m_pGrabberDevFilter, NULL);
	if (FAILED(hr))
	{
		printf("ÏÔÊ¾GrabberDevFilterÊôÐÔ¶Ô»°¿òÊ§°Ü£¡");
		return hr;
	}
	return hr;
}

HRESULT CaptureVideo::SetCrossbarProperty()
{
	HRESULT hr = S_OK;
	hr = ShowFilterPropertyPage(m_pCrossbarFilter, NULL);
	if (FAILED(hr))
	{
		printf("ÏÔÊ¾CrossbarÊôÐÔ¶Ô»°¿òÊ§°Ü£¡");
		return hr;
	}
	return hr;
}

HRESULT CaptureVideo::SetSampleGrabberProperty()
{
	HRESULT hr = S_OK;
	if (!m_pSampGrabber)
		return E_POINTER;

	AM_MEDIA_TYPE mt;
	ZeroMemory(&mt, sizeof(mt));
	mt.majortype = MEDIATYPE_Video;
	mt.subtype = MEDIASUBTYPE_UYVY;

	hr = m_pSampGrabber->SetMediaType(&mt);
	if (FAILED(hr))
	{
		printf("ÉèÖÃsample¸ñÊ½Ê§°Ü£¡");
		return hr;
	}
	hr = m_pSampGrabber->SetBufferSamples(TRUE);
	if (FAILED(hr))
	{
		printf("ÉèÖÃbufferÊ§°Ü£¡");
		return hr;
	}
	hr = m_pSampGrabber->SetCallback(&g_sampleGrabberCB, 1);
	if (FAILED(hr))
	{
		printf("ÉèÖÃcallbackÊ§°Ü£¡");
		return hr;
	}
	return hr;
}

HRESULT CaptureVideo::Start()
{
	HRESULT hr = S_OK;
	if (!m_pMediaControl)
		return E_POINTER;
	hr = m_pMediaControl->Run();
	return hr;
}
HRESULT CaptureVideo::Pause()
{
	HRESULT hr = S_OK;
	if (!m_pMediaControl)
		return E_POINTER;
	hr = m_pMediaControl->Pause();
	return hr;
}
HRESULT CaptureVideo::Stop()
{
	HRESULT hr = S_OK;
	if (!m_pMediaControl)
		return E_POINTER;
	hr = m_pMediaControl->Stop();
	return hr;
}


void CaptureVideo::CloseInterface()
{
	m_bGetOneShot = FALSE;

	if (m_pMediaControl)
		m_pMediaControl->Stop();
	/*if(m_pVideoWindow)
	{
	m_pVideoWindow->get_Visible(FALSE);
	m_pVideoWindow->put_Owner(NULL);
	}*/

	/*if(m_pMediaEvent)
	m_pMediaEvent->SetNotifyWindow(NULL,WM_GRAPHNOTIFY,0);*/

	//release interface
	SafeRelease(&m_pGrabberDevFilter);
	SafeRelease(&m_pCrossbarFilter);
	SafeRelease(&m_pCaptureGB);
	SafeRelease(&m_pGraphBuilder);
	SafeRelease(&m_pMediaControl);
	//SafeRelease(&m_pMediaEvent);
	SafeRelease(&m_pSampGrabber);
	//SafeRelease(m_pVideoWindow);
}

//HRESULT CaptureVideo::BindFilter(int deviceID, IBaseFilter **pBaseFilter)
//{
//	ICreateDevEnum *pDevEnum;
//	IEnumMoniker   *pEnumMon;
//	IMoniker	   *pMoniker;
//	HRESULT hr = CoCreateInstance(CLSID_SystemDeviceEnum,NULL,CLSCTX_INPROC_SERVER,
//		IID_ICreateDevEnum,(LPVOID*)&pDevEnum);
//	if (SUCCEEDED(hr))
//	{
//		hr = pDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory,&pEnumMon, 0);
//		if (hr == S_FALSE)
//		{
//			hr = VFW_E_NOT_FOUND;
//			return hr;
//		}
//		pEnumMon->Reset();
//		ULONG cFetched;
//		int index = 0;
//		while(hr=pEnumMon->Next(1,&pMoniker,&cFetched),hr == S_OK, index<= deviceID)
//		{
//			IPropertyBag *pProBag;
//			hr = pMoniker->BindToStorage(0,0,IID_IPropertyBag,(LPVOID*)&pProBag);
//			if (SUCCEEDED(hr))
//			{
//				if (index == deviceID)
//				{
//					pMoniker->BindToObject(0,0,IID_IBaseFilter,(LPVOID*)pBaseFilter);
//				}
//			}
//			pMoniker->Release();
//			index++;
//		}
//		pEnumMon->Release();
//	}
//	return hr;
//}

//HRESULT CaptureVideo::SetupVideoWindow(LONG nLeft, LONG nTop, LONG nWidth, LONG nHeight)
//{
//	HRESULT hr;
//	hr = m_pVideoWindow->put_Owner((OAHWND)m_App);
//	if (FAILED(hr))
//		return hr;
//
//	hr = m_pVideoWindow->put_WindowStyle(WS_CHILD | WS_CLIPCHILDREN);
//	if(FAILED(hr))
//		return hr;
//
//	ResizeVideoWindow(nLeft,nTop,nWidth,nHeight);
//
//	hr = m_pVideoWindow->put_Visible(OATRUE);
//	if(FAILED(hr))
//		return hr;
//
//	return hr;
//}

//void CaptureVideo::ResizeVideoWindow(LONG nLeft, LONG nTop, LONG nWidth, LONG nHeight)
//{
//	if(m_pVideoWindow)
//	{
//		m_pVideoWindow->SetWindowPosition(nLeft,nTop,nWidth,nHeight);
//	}
//}

//HRESULT CaptureVideo::OpenDevice(int deviceID, LONG nLeft, LONG nTop, LONG nWide, LONG nHeight)
//{
//	HRESULT hr;
//	IBaseFilter *pSampleGrabberFilter;
//	if (m_bConnect)
//	{
//		CloseInterface();
//		InitializeEnv();
//	}
//
//	hr = CoCreateInstance(CLSID_SampleGrabber,NULL,CLSCTX_INPROC_SERVER,
//		IID_IBaseFilter, (LPVOID*)&pSampleGrabberFilter);
//	if(FAILED(hr))
//		return hr;
//	//bind device filter
//	hr = BindFilter(deviceID,&m_pDevFilter);
//	if (FAILED(hr))
//		return hr;
//	hr = m_pGraphBuilder->AddFilter(m_pDevFilter,L"Video Filter");
//	if (FAILED(hr))
//		return hr;
//
//	hr = m_pGraphBuilder->AddFilter(pSampleGrabberFilter,L"Sample Grabber");
//	if (FAILED(hr))
//		return hr;
//
//	hr = pSampleGrabberFilter->QueryInterface(IID_ISampleGrabber,(LPVOID*)&m_pSampGrabber);
//	if(FAILED(hr))
//		return hr;
//
//	//set media type
//	AM_MEDIA_TYPE mediaType;
//	ZeroMemory(&mediaType,sizeof(AM_MEDIA_TYPE));
//	//Find the current bit depth
//	HDC hdc=GetDC(NULL);
//	int iBitDepth=GetDeviceCaps(hdc, BITSPIXEL);
//	g_sampleGrabberCB.m_iBitCount = iBitDepth;
//	ReleaseDC(NULL,hdc);
//	//Set the media type
//	mediaType.majortype = MEDIATYPE_Video;
//	switch(iBitDepth)
//	{
//	case  8:
//		mediaType.subtype=MEDIASUBTYPE_RGB8;
//		break;
//	case 16:
//		mediaType.subtype=MEDIASUBTYPE_RGB555;
//		break;
//	case 24:
//		mediaType.subtype=MEDIASUBTYPE_RGB24;
//		break;
//	case 32:
//		mediaType.subtype=MEDIASUBTYPE_RGB32;
//		break;
//	default:
//		return E_FAIL;
//	}
//	mediaType.formattype = FORMAT_VideoInfo;
//	hr = m_pSampGrabber->SetMediaType(&mediaType);
//
//	hr = m_pCaptureGB->RenderStream(&PIN_CATEGORY_PREVIEW,&MEDIATYPE_Video,
//		m_pDevFilter,pSampleGrabberFilter,NULL);
//	if(FAILED(hr))
//		return hr;
//
//	hr = m_pSampGrabber->GetConnectedMediaType(&mediaType);
//	if(FAILED(hr))
//		return hr;
//
//	VIDEOINFOHEADER * vih = (VIDEOINFOHEADER*) mediaType.pbFormat;
//	g_sampleGrabberCB.m_lWidth = vih->bmiHeader.biWidth;
//	g_sampleGrabberCB.m_lHeight = vih->bmiHeader.biHeight;
//	// Configure the Sample Grabber
//	hr = m_pSampGrabber->SetOneShot(FALSE);
//	if (FAILED(hr))
//		return hr;
//	hr = m_pSampGrabber->SetBufferSamples(TRUE);
//	if (FAILED(hr))
//		return hr;
//	// 1 = Use the BufferCB callback method.
//	hr = m_pSampGrabber->SetCallback(&g_sampleGrabberCB,1);
//
//	//set capture video Window
//	SetupVideoWindow(nLeft,nTop,nWide,nHeight);
//	hr = m_pMediaControl->Run();
//	if(FAILED(hr))
//		return hr;
//
//	if (mediaType.cbFormat != 0)
//	{
//		CoTaskMemFree((PVOID)mediaType.pbFormat);
//		mediaType.cbFormat = 0;
//		mediaType.pbFormat = NULL;
//	}
//	if (mediaType.pUnk != NULL)
//	{
//		mediaType.pUnk->Release();
//		mediaType.pUnk = NULL;
//	}
//	m_bConnect = TRUE;
//	return hr;
//
//}

void CaptureVideo::GrabOneFrame(BOOL bGrab)
{
	//m_bGetOneShot = bGrab;
	//g_sampleGrabberCB.m_bGetPicture = bGrab;
}

//HRESULT CaptureVideo::HandleGraphCapturePicture(void)
//{
//	//////////////////////////////////////////////////////////////////////////
//	HRESULT hr;
//	long evCode = 0;
//	long lBufferSize = 0;
//	BYTE *p;
//	hr = m_pMediaEvent->WaitForCompletion(INFINITE, &evCode); //
//	if (SUCCEEDED(hr))
//	{
//		switch(evCode)
//		{
//		case EC_COMPLETE:
//			m_pSampGrabber->GetCurrentBuffer(&lBufferSize, NULL);
//			p = new BYTE[lBufferSize];
//			m_pSampGrabber->GetCurrentBuffer(&lBufferSize, (LONG*)p);// get Current buffer
//			g_sampleGrabberCB.SaveBitmap(p,lBufferSize); //save bitmap
//			break;
//		default:
//			break;
//		}
//	}
//	//////////////////////////////////////////////////////////////////////////
//	return hr;
//}
