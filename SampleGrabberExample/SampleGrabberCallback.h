#ifndef __SAMPLEGRABBERCALLBACK_H__
#define __SAMPLEGRABBERCALLBACK_H__
#include "common.h"
#include "CUYVY2BMP.h"
#include "H264Encoderc.h"
#include "RTPRecv.h"

class SampleGrabberCallback : public ISampleGrabberCB
{
public:
	ULONG STDMETHODCALLTYPE AddRef();
	ULONG STDMETHODCALLTYPE Release();
	HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid,void** ppvObject);

	HRESULT STDMETHODCALLTYPE SampleCB(double Time, IMediaSample *pSample);
	HRESULT STDMETHODCALLTYPE BufferCB(double Time, BYTE *pBuffer, long BufferLen);
	
	SampleGrabberCallback();
	static int X264FrameCallBack(int FrameType, void *pData, int Length, void *pContext);
	static int RTPFrameCallBack(int FrameType, void *pData, int Length, void *pContext);
	HRESULT ConvertYUY2ToYUV420(BYTE *pSrc, BYTE *pDst, int iWidth, int iHeight);
	BOOL SaveBitmap(BYTE * pBuffer, long lBufferSize ); //保存bitmap图片
	BOOL SaveRaw(BYTE * pBuffer, long lBufferSize ); //保存一帧原始数据
	BOOL HandleRaw(BYTE * pBuffer, long lBufferSize ); // handle data
public:
	BOOL m_bGetPicture;  // is get a picture
	long m_lWidth;       //存储图片的宽度
	long m_lHeight;		 //存储图片的长度
	int  m_iBitCount;    //the number of bits per pixel (bpp)
	TCHAR m_chTempPath[MAX_PATH];
	TCHAR m_chSwapStr[MAX_PATH];
	TCHAR m_chDirName[MAX_PATH];
	long m_lTotalFrame;
	//H264编码类
	CH264Encoderc m_Encodec;
	//RTP传输类
	CRTPRecv m_RTPRecv;
	//CUYVY2BMP m_CUYVY2BMP;
};

#endif //__SAMPLEGRABBERCALLBACK_H__