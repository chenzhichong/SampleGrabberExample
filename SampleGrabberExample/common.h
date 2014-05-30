#ifndef __COMMON_H__  
#define __COMMON_H__  

#include <windows.h>
#include <dshow.h>
#include <dshowutil.h>
#include <initguid.h>
#pragma include_alias( "dxtrans.h", "qedit.h" )
#define __IDxtCompositor_INTERFACE_DEFINED__
#define __IDxtAlphaSetter_INTERFACE_DEFINED__
#define __IDxtJpeg_INTERFACE_DEFINED__
#define __IDxtKey_INTERFACE_DEFINED__
#include "qedit.h"

//设备名称
const wchar_t CAPTURE_DEVICE_NAME[][64] = {L"SMI Grabber Device", L"AVerMedia 7231 Analog Capture"};
const wchar_t CROSSBAR_DEVICE_NAME[][64] = {L"SM BDA Crossbar Filter", L"AVerMedia 7231 Analog Xbar"};
const wchar_t CAPTURE_DEVICE_VEDIO_PIN_IN_NAME[][64] = {L"Video Tuner In", L"模拟视频输入"};
const wchar_t CAPTURE_DEVICE_VEDIO_PIN_OUT_NAME[][64] = {L"Capture Out", L"捕获"};
const wchar_t CROSSBAR_DEVICE_PIN_OUT_NAME[][64] = {L"0: Video Decoder Out", L"0: Video Decoder Out"};

const int WHICH_DEVICE = 1;

// SMI Grabber Device
DEFINE_GUID(CLSID_VideoCaptureSources,
0x860BB310, 0x5D01, 0x11D0, 0xBD, 0x3B, 0x00, 0xA0, 0xC9, 0x11, 0xCE, 0x86);
// (A799A801-A46D-11D0-A18C-00A02401DCD4}
// SM BDA Crossbar Filter
DEFINE_GUID(CLSID_WDM,
0xA799A801, 0xA46D, 0x11D0, 0xA1, 0x8C, 0x00, 0XA0, 0x24, 0x01, 0xDC, 0XD4);

//// (B87BEB7B—8D29—423F—AEdD—6582C10175AC)
//DEFINE_GUID(CLSID_VideoRenderer,
//			0xB87BEB7B, 0x8D29, 0x423F, OXAE, 0x4D, 0x65, 0x82, 0xC1, 0x01, 0x75, OXAC): //quartz.dll


template <class T> void SafeRelease(T **ppT)
{
	if (*ppT)
	{
		(*ppT)->Release();
		*ppT = NULL;
	}
}

void ShowError(HRESULT hr);

#endif// __COMMON_H__  