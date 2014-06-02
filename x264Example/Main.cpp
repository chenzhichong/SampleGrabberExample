/**
* @note x264�ı���ʾ��.
* ʹ��x264�İ汾Ϊlibx264-115
* 1. ʾ���Ǹ���ѭ��,��ԴԴ���ϵı���,Ȼ������д�ļ�.
* 2. ʾ������Ϊ��:����1000֡��,ȡ�ձ��뻺����,Ȼ��ѭ��ִ��������.
* 
*/
#define _CRT_SECURE_NO_WARNINGS
#include <cassert>
#include <iostream>
#include <string>
#include "stdint.h"
extern "C"
{
#include "x264.h"
};

#pragma comment (lib, "x264/libx264-142.lib")
unsigned int g_uiPTSFactor = 0;
int iNal   = 0;
x264_nal_t* pNals = NULL;
int encode(x264_t* p264, x264_picture_t* pIn, x264_picture_t* pOut);
int main(int argc, char** argv)
{
	int iResult = 0;
	x264_t* pX264Handle   = NULL;
	x264_param_t* pX264Param = new x264_param_t;
	assert(pX264Param);
	//* ���ò���
	//* ʹ��Ĭ�ϲ���
	x264_param_default(pX264Param);
	//* cpuFlags
	pX264Param->i_threads  = X264_SYNC_LOOKAHEAD_AUTO;//* ȡ�ջ���������ʹ�ò������ı�֤.
	//* video Properties
	pX264Param->i_width   = 320; //* ���.
	pX264Param->i_height  = 240; //* �߶�
	pX264Param->i_frame_total = 0; //* ������֡��.��֪����0.
	pX264Param->i_keyint_max = 10;
	//* bitstream parameters
	pX264Param->i_bframe  = 5;
	pX264Param->b_open_gop  = 0;
	pX264Param->i_bframe_pyramid = 0;
	pX264Param->i_bframe_adaptive = X264_B_ADAPT_TRELLIS;

	//* ��߱�,��Ч��,��������Ҫ��.
	//pX264Param->vui.i_sar_width = 1080;
	//pX264Param->vui.i_sar_height = 720;

	//* Log
	pX264Param->i_log_level  = X264_LOG_DEBUG;
	//* Rate control Parameters
	pX264Param->rc.i_bitrate = 1024 * 10;//* ����(������,��λKbps)
	//* muxing parameters
	pX264Param->i_fps_den  = 1; //* ֡�ʷ�ĸ
	pX264Param->i_fps_num  = 25;//* ֡�ʷ���
	pX264Param->i_timebase_den = pX264Param->i_fps_num;
	pX264Param->i_timebase_num = pX264Param->i_fps_den;

	//* ����Profile.ʹ��MainProfile
	x264_param_apply_profile(pX264Param, x264_profile_names[1]);

	//* �򿪱��������,ͨ��x264_encoder_parameters�õ����ø�X264
	//* �Ĳ���.ͨ��x264_encoder_reconfig����X264�Ĳ���
	pX264Handle = x264_encoder_open(pX264Param);
	assert(pX264Handle);

	//* ��ȡ��������PPS��SPS,����Ҫ���Բ�����.
	iResult = x264_encoder_headers(pX264Handle, &pNals, &iNal);
	assert(iResult >= 0);
	//* PPS SPS �ܹ�ֻ��36B,��ν���������?
	for (int i = 0; i < iNal; ++i)
	{
		switch (pNals[i].i_type)
		{
		case NAL_SPS:
			break;
		case  NAL_PPS:
			break;
		default:
			break;
		}
	}

	//* ��ȡ����������֡��.
	int iMaxFrames = x264_encoder_maximum_delayed_frames(pX264Handle);

	//* ������Ҫ�Ĳ���.
	iNal = 0;
	pNals = NULL;
	x264_picture_t* pPicIn = new x264_picture_t;
	x264_picture_t* pPicOut = new x264_picture_t;

	x264_picture_init(pPicOut);
	x264_picture_alloc(pPicIn, X264_CSP_I420, pX264Param->i_width, pX264Param->i_height);
	pPicIn->img.i_csp = X264_CSP_I420;
	pPicIn->img.i_plane = 3;

	//* �����ļ�,���ڴ洢��������
	FILE* pFile = fopen("agnt.264", "wb");
	assert(pFile);

	//* ʾ���ñ�������.
	int iDataLen = pX264Param->i_width * pX264Param->i_height;
	uint8_t* data = new uint8_t[iDataLen];

	unsigned int  uiComponent = 0;
	while (++uiComponent)
	{
		//* ������Ҫ�����Դ����(YUV420ɫ�ʸ�ʽ)
		::memset(data, uiComponent, iDataLen);
		::memcpy(pPicIn->img.plane[0], data, iDataLen);
		::memcpy(pPicIn->img.plane[1], data, iDataLen/4 );
		::memcpy(pPicIn->img.plane[2], data, iDataLen/4);

		if (uiComponent <= 1000)
		{
			pPicIn->i_pts = uiComponent + g_uiPTSFactor * 1000;
			encode(pX264Handle, pPicIn, pPicOut);
		}
		else
		{
			//* �����������ȡ��
			int iResult = encode(pX264Handle, NULL, pPicOut);
			if (0== iResult)
			{
				//break; //* ȡ��,����
				uiComponent = 0;
				++g_uiPTSFactor;

				/* {{ ����������ȡ�ջ�����,��ѹ����B֡������
				x264_encoder_reconfig(pX264Handle, pX264Param);
				x264_encoder_intra_refresh(pX264Handle);
				//* }} */
			}
		}

		//* ����������д���ļ�.
		for (int i = 0; i < iNal; ++i)
		{
			fwrite(pNals[i].p_payload, 1, pNals[i].i_payload, pFile);
		}
	}
	//* ���ͼ������
	x264_picture_clean(pPicIn);
	x264_picture_clean(pPicOut);
	//* �رձ��������
	x264_encoder_close(pX264Handle);
	pX264Handle = NULL;

	delete pPicIn ;
	pPicIn = NULL;

	delete pPicOut;
	pPicOut = NULL;

	delete pX264Param;
	pX264Param = NULL;

	delete [] data;
	data = NULL;
	return 0;
}

int encode(x264_t* pX264Handle, x264_picture_t* pPicIn, x264_picture_t* pPicOut)
{

	int iResult   = 0;
	iResult = x264_encoder_encode(pX264Handle, &pNals, &iNal, pPicIn, pPicOut);
	if (0 == iResult)
	{
		std::cout<<"����ɹ�,����������."<<std::endl;
		return iResult;
	}else{
		if(iResult < 0)
		{
			std::cout<<"�������"<<std::endl;
			return iResult;
		}else{
			if (iResult > 0)
			{
				std::cout<<"�õ���������"<<std::endl;
			}

			/* {{ ���ò���
			unsigned char* pNal = NULL;
			for (int i = 0;i < iNal; ++i)
			{
			int iData = 1024 * 32;
			x264_nal_encode(pX264Handle, pNal,&pNals[i]);
			}
			//* }} */

			//* ��ȡX264�л���֡��.
			int iFrames = x264_encoder_delayed_frames(pX264Handle);
			std::cout<<"��ǰ�������л�������:"<<iFrames<<"֡\n";
			return iFrames;
		}
	}
}

