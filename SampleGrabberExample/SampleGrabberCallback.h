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
	BOOL SaveBitmap(BYTE * pBuffer, long lBufferSize); //����bitmapͼƬ
	BOOL SaveRaw(BYTE * pBuffer, long lBufferSize); //����һ֡ԭʼ����
	BOOL SaveRawToSequence(BYTE * pBuffer, long lBufferSize); //����Ϊ��Ƶ����
	BOOL SaveRawToTS(BYTE * pBuffer, long lBufferSize);//����Ϊts��
	void SendTS(BYTE * pBuffer, long lBufferSize);//��������;
	BOOL HandleRaw(BYTE * pBuffer, long lBufferSize); // handle data
public:
	BOOL m_bOneFrame;  // is get a picture
	long m_lWidth;       //�洢ͼƬ�Ŀ��
	long m_lHeight;		 //�洢ͼƬ�ĳ���
	int  m_iBitCount;    //the number of bits per pixel (bpp)
	TCHAR m_chTempPath[MAX_PATH];
	TCHAR m_chSwapStr[MAX_PATH];
	TCHAR m_chDirName[MAX_PATH];
	long m_lTotalFrame;	//��֡��
	BOOL m_bIsFirst;	//����Ϊ��Ƶ����ʱ��ָʾ�Ƿ��һ�δ����ļ�
	//CUYVY2BMP m_CUYVY2BMP;
	CRTPSender m_CRTPSender;
	BYTE m_Remain; //ÿ�η���һ֡��ʣ������ݣ�0-188
	BYTE m_RD[TS_PACKET_SIZE]; //remain data;
};

#endif //__SAMPLEGRABBERCALLBACK_H__