#include "Main.h"

int main()
{
	printf("hello world!\n");	
	int s;
	printf(" ‰»Î ±º‰£®s£©£∫\n");
	scanf("%d", &s);
	HRESULT hr = g_CaptureVideo.SaveGraphToFile(L"d://test.grf");
	//hr = g_CaptureVideo.SetGrabberProperty();
	//hr = g_CaptureVideo.SetCrossbarProperty();
	//hr = g_CaptureVideo.SetSampleGrabberProperty();
	hr = g_CaptureVideo.Start();
	Sleep(s*1000);
	hr = g_CaptureVideo.Stop();
	if (FAILED(hr))
		ShowError(hr);
	return 0;
}