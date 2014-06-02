#include "Main.h"

int main()
{
	printf("hello world!\n");
	//初始化日志库
	GenewLog::Initialize("log.properties","Genewcptbtptp");
	printf("初始化日志库OK!\n");
	
	HRESULT hr = g_CaptureVideo.SaveGraphToFile(L"d://test.grf");
	hr = g_CaptureVideo.SetGrabberProperty();
	hr = g_CaptureVideo.SetCrossbarProperty();
	hr = g_CaptureVideo.SetSampleGrabberProperty();
	hr = g_CaptureVideo.Start();
	Sleep(500000);
	hr = g_CaptureVideo.Stop();
	if (FAILED(hr))
		ShowError(hr);
	return 0;
}