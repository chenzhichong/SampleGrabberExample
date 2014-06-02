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
	BOOL SaveBitmap(BYTE * pBuffer, long lBufferSize ); //����bitmapͼƬ
	BOOL SaveRaw(BYTE * pBuffer, long lBufferSize ); //����һ֡ԭʼ����
	BOOL HandleRaw(BYTE * pBuffer, long lBufferSize ); // handle data
public:
	BOOL m_bGetPicture;  // is get a picture
	long m_lWidth;       //�洢ͼƬ�Ŀ��
	long m_lHeight;		 //�洢ͼƬ�ĳ���
	int  m_iBitCount;    //the number of bits per pixel (bpp)
	TCHAR m_chTempPath[MAX_PATH];
	TCHAR m_chSwapStr[MAX_PATH];
	TCHAR m_chDirName[MAX_PATH];
	long m_lTotalFrame;
	//H264������
	CH264Encoderc m_Encodec;
	//RTP������
	CRTPRecv m_RTPRecv;
	//CUYVY2BMP m_CUYVY2BMP;
};

#endif //__SAMPLEGRABBERCALLBACK_H__