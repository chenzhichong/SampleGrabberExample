#ifndef __COMMON_H__  
#define __COMMON_H__  

#include <winsock2.h>
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

//log
#include "GenewLog.h"
#pragma comment (lib,"GenewLoggingService.lib")

//jrtp lib
#ifdef _DEBUG
#pragma comment (lib, "../lib/RTP/Debug/jrtplib.lib")
#pragma comment (lib, "../lib/RTP/Debug/jthread.lib")
#else
#pragma comment (lib, "../lib/RTP/Release/jrtplib.lib")
#pragma comment (lib, "../lib/RTP/Release/jthread.lib")
#endif

#pragma comment(lib, "libav/avcodec.lib")
#pragma comment(lib, "libav/avformat.lib")
#pragma comment(lib, "libav/swscale.lib")
#pragma comment(lib, "libav/avutil.lib")

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

//x264 & jrtp
typedef unsigned char      uint8_t;

#define FRAME_RATE 25					//帧率
#define BI_I420 0x30323449				//定义I420
#define I420_320_240 115200				//I420格式 320*240 图像大小
#define MAX_DATA_LENGTH 65535			//最大数据长度
#define IP_LENGTH 40					//IP最大长度
#define RTP_BYE_DELAY_TIME  2000		//rtp 关闭通道延迟时间
#define RTP_RECV_PORT 12306				//RTP接收端口
#define HOST_NAME_LENGTH 128			//主机名最大长度
const char RECORD_FILEPATH[]="./Record";  //当前录像文件路径，不包含文件夹
const int FRAME_BUFFER_SIZE = 32*1024;	  //FFmpeg流数据BUFFER大小

//图像数据回调函数
typedef int (*IMAGE_CALLBACK)(void *pData, int Length, void *pContext);
//帧数据回调函数
typedef int (*FRAME_CALLBACK)(int FrameType, void *pData, int Length, void *pContext);
//RTP接受数据状态通知
typedef int (*RTP_RECV_STATUS_CALLBACK)(int Status, char *IP, int Port);

//转换矩阵
#define MY(a,b,c) (( a*  0.2989  + b*  0.5866  + c*  0.1145))
#define MU(a,b,c) (( a*(-0.1688) + b*(-0.3312) + c*  0.5000 + 128))
#define MV(a,b,c) (( a*  0.5000  + b*(-0.4184) + c*(-0.0816) + 128))

//大小判断
#define DY(a,b,c) (MY(a,b,c) > 255 ? 255 : (MY(a,b,c) < 0 ? 0 : MY(a,b,c)))
#define DU(a,b,c) (MU(a,b,c) > 255 ? 255 : (MU(a,b,c) < 0 ? 0 : MU(a,b,c)))
#define DV(a,b,c) (MV(a,b,c) > 255 ? 255 : (MV(a,b,c) < 0 ? 0 : MV(a,b,c)))
#define CLIP(a) ((a) > 255 ? 255 : ((a) < 0 ? 0 : (a)))

//视频数据
typedef struct ST_VideoData
{
	uint8_t pData[FRAME_BUFFER_SIZE];
	int len;
	ST_VideoData(const uint8_t *data, int length):len(length)
	{
		memcpy(pData,data,length);
	}
	ST_VideoData():len(0)
	{

	}
};

//帧数据类型
enum EN_FrameType 
{
	UNKNOW=0,
	I_FRAME=1,
	P_FRAME=2,
	B_FRAME=3,
};

enum EN_RTP_RECV_STATUS
{
	StartRecv=0,
	StopRecv,
};

#endif// __COMMON_H__  