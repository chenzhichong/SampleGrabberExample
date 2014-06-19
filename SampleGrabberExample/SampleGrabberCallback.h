#ifndef __SAMPLEGRABBERCALLBACK_H__
#define __SAMPLEGRABBERCALLBACK_H__
#include "common.h"
#include "CUYVY2BMP.h"
#include "CRTPSender.h"

#define TS_PACKET_SIZE 188
#define RTP_TS_PACKET_COUNT 7

class SampleGrabberCallback : public ISampleGrabberCB
{
public:
	ULONG STDMETHODCALLTYPE AddRef();
	ULONG STDMETHODCALLTYPE Release();
	HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid,void** ppvObject);

	HRESULT STDMETHODCALLTYPE SampleCB(double Time, IMediaSample *pSample);
	HRESULT STDMETHODCALLTYPE BufferCB(double Time, BYTE *pBuffer, long BufferLen);
	
	SampleGrabberCallback();
	HRESULT ConvertYUY2ToYUV420(BYTE *pSrc, BYTE *pDst, int iWidth, int iHeight);
	HRESULT ConvertUYVYToYUV420(BYTE *pSrc, BYTE *pDst, int iWidth, int iHeight);
	BOOL SaveBitmap(BYTE * pBuffer, long lBufferSize); //保存bitmap图片
	BOOL SaveRaw(BYTE * pBuffer, long lBufferSize); //保存一帧原始数据
	BOOL SaveRawToSequence(BYTE * pBuffer, long lBufferSize); //保存为视频序列
	BOOL SaveRawToTS(BYTE * pBuffer, long lBufferSize);//保存为ts流
	void SendTS(BYTE * pBuffer, long lBufferSize);//发到网络;
	BOOL HandleRaw(BYTE * pBuffer, long lBufferSize); // handle data
public:
	BOOL m_bOneFrame;  // is get a picture
	long m_lWidth;       //存储图片的宽度
	long m_lHeight;		 //存储图片的长度
	int  m_iBitCount;    //the number of bits per pixel (bpp)
	TCHAR m_chTempPath[MAX_PATH];
	TCHAR m_chSwapStr[MAX_PATH];
	TCHAR m_chDirName[MAX_PATH];
	long m_lTotalFrame;	//总帧数
	BOOL m_bIsFirst;	//保存为视频序列时，指示是否第一次创建文件
	//CUYVY2BMP m_CUYVY2BMP;
	CRTPSender m_CRTPSender;
	BYTE m_Remain; //每次发完一帧后剩余的数据；0-188
	BYTE m_RD[TS_PACKET_SIZE]; //remain data;
};

#endif //__SAMPLEGRABBERCALLBACK_H__