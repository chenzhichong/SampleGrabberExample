#ifndef __MAIN_H__
#define __MAIN_H__

#include "CaptureVideo.h"
static int ImageCallBack(void *data, int len);
static int X264FrameCallBack(int FrameType, void *data, int len);

CaptureVideo g_CaptureVideo;

#endif //__MAIN_H__