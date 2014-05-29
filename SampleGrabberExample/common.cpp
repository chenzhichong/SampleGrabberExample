#include "common.h"

void ShowError(HRESULT hr)
{
    if (FAILED(hr))
    {
        TCHAR szErr[MAX_ERROR_TEXT_LEN];
        DWORD res = AMGetErrorText(hr, szErr, MAX_ERROR_TEXT_LEN);
        if (res == 0)
        {
            StringCchPrintf(szErr, MAX_ERROR_TEXT_LEN, L"Unknown Error: 0x%2x", hr);
        }
        MessageBox(0, szErr, TEXT("Error!"), MB_OK | MB_ICONERROR);
    }
}