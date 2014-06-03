#define _CRT_SECURE_NO_WARNINGS
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
extern "C" {
    #include "x264.h"
}

#pragma comment (lib, "x264/libx264-142.lib")

void YUY2toI420(int inWidth, int inHeight, uint8_t *pSrc, uint8_t *pDest)
{
	int i, j;
	uint8_t *u = pDest + (inWidth * inHeight);
	uint8_t *v = u + (inWidth * inHeight) / 4;
	for (i = 0; i < inHeight/2; i++)
	{
		uint8_t *src_l1 = pSrc + inWidth*2*2*i;
		uint8_t *src_l2 = src_l1 + inWidth*2;
		uint8_t *y_l1 = pDest + inWidth*2*i;
		uint8_t *y_l2 = y_l1 + inWidth;
		for (j = 0; j < inWidth/2; j++)
		{
			// two pels in one go
			*y_l1++ = src_l1[0];
			*u++ = src_l1[1];
			*y_l1++ = src_l1[2];
			*v++ = src_l1[3];
			*y_l2++ = src_l2[0];
			*y_l2++ = src_l2[2];
			src_l1 += 4;
			src_l2 += 4;
		}
	}
}

int main(int argc, char *argv[]) {
	int width = 480;
	int height = 360;
	int fps = 25;
	size_t yuv_size = width * height * 3 / 2;
	x264_t *encoder;
	x264_picture_t pic_in, pic_out;
	FILE* inf;
	FILE* outf;
	uint8_t *yuv_buffer;

	/*if (argc != 3) {
	    printf("usage: %s input output\n", argv[0]);
	    return -1;
	}*/

	//初始化encoder
	x264_param_t param;
	x264_param_default_preset(&param, "veryfast", "zerolatency");
	param.i_threads = 1;
	param.i_width = width;
	param.i_height = height;
	param.i_fps_num = fps;
	param.i_fps_den = 1;

	param.i_keyint_max = 25;
	param.b_intra_refresh = 1;

	param.b_annexb = 1;

	x264_param_apply_profile(&param, "baseline");
	encoder = x264_encoder_open(&param);

	//初始化pic_in
	x264_picture_alloc(&pic_in, X264_CSP_I420, width, height);

	yuv_buffer = (uint8_t*)malloc(yuv_size);

	pic_in.img.plane[0] = yuv_buffer;
	pic_in.img.plane[1] = pic_in.img.plane[0] + width * height;
	pic_in.img.plane[2] = pic_in.img.plane[1] + width * height / 4;

	//初始化文件描述符
	inf = fopen("d://video", "rb");
	if (!inf) {
		printf("open in.yuv failed!");
	    return -1;
	}
	outf = fopen("d://out.264", "wb");
	if (!outf) {
		printf("open out.264 failed!");
	    return -1;
	}

	//编码
	/*int64_t i_pts = 0;

	x264_nal_t *nals;
	int nnal;
	while (fread(yuv_buffer, yuv_size, 1, inf) > 0) {
	    pic_in.i_pts = i_pts++;
	    x264_encoder_encode(encoder, &nals, &nnal, &pic_in, &pic_out);
	    x264_nal_t *nal;
	    for (nal = nals; nal < nals + nnal; nal++) {
			fwrite(nal->p_payload, nal->i_payload, 1, outf);
	    }
	}*/

	x264_nal_t* nals;
	int i_nals;
	while (fread(yuv_buffer, yuv_size, 1, inf) > 0) {
		int frame_size = x264_encoder_encode(encoder, &nals, &i_nals, &pic_in, &pic_out);
		if (frame_size >= 0)
		{
			// OK
			printf("good![%d]\n", nals->i_payload);
			fwrite(nals->p_payload, nals->i_payload, 1, outf);
		}
	}

	//clean
	x264_encoder_close(encoder);
	fclose(inf);
	fclose(outf);
	free(yuv_buffer);
	return 0;
}