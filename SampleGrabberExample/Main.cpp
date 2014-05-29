#include "CaptureVideo.h"

CaptureVideo g_CaptureVideo;
int main()
{
	printf("hello world!\n");
	HRESULT hr = g_CaptureVideo.SaveGraphToFile(L"d://test.grf");
	hr = g_CaptureVideo.SetGrabberProperty();
	hr = g_CaptureVideo.SetCrossbarProperty();
	hr = g_CaptureVideo.SetSampleGrabberProperty();
	hr = g_CaptureVideo.Start();
	Sleep(5000);
	hr = g_CaptureVideo.Stop();
	if (FAILED(hr))
		ShowError(hr);
	return 0;
}