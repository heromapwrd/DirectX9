/*
------------------------------------------------------------
d3dUtility.cpp
------------------------------------------------------------
*/

/////////////////////////////////////////////////////////////
//				Global variables
/////////////////////////////////////////////////////////////
#include "d3dUtility.h"

namespace d3d9 
{
	bool Init(
		HINSTANCE hInstance,
		int width, int height,
		bool bWindowed,
		D3DDEVTYPE eDeviceType,
		IDirect3DDevice9 ** device
		)
	{
		// 创建窗口类
		WNDCLASS wc;
		wc.style = CS_HREDRAW | CS_HREDRAW;
		wc.lpfnWndProc = (WNDPROC)WndProc;
		wc.hInstance = hInstance;
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;
		wc.hIcon = LoadIcon(0, IDI_APPLICATION);
		wc.hCursor = LoadCursor(0, IDC_ARROW);
		wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
		wc.lpszMenuName = 0;
		wc.lpszClassName = "Direct3D9App";

		// 注册窗口类
		if (!RegisterClass(&wc))
		{
			::MessageBox(NULL, "Register Class Faild!", 0, 0);
			return false;
		}

		// 创建窗口
		HWND hWnd =NULL;
		hWnd = CreateWindow(
			"Direct3D9App",
			"Direct3D9App",
			WS_OVERLAPPEDWINDOW,
			0, 0, width, height,
			NULL,
			NULL,
			hInstance,
			NULL
			);
		if (!hWnd)
		{
			MessageBox(NULL, "Create Window Failed!", 0, 0);
			return false;
		}

		// 显示和更新窗口
		ShowWindow(hWnd, SW_SHOW);
		UpdateWindow(hWnd);



		// 初始化IDirect3D9指针
		IDirect3D9* pD3D;
		pD3D = Direct3DCreate9(D3D_SDK_VERSION);

		// 获取并检测设备性能
		D3DCAPS9 caps;
		HRESULT hr = pD3D->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &caps);
		if (FAILED(hr))
		{
			MessageBox(0, "Get Deviece Caps Failed!", 0, 0);
			return false;
		}
			

		int vp;
		if (caps.DevCaps&D3DDEVCAPS_HWTRANSFORMANDLIGHT)
		{
			vp = D3DCREATE_HARDWARE_VERTEXPROCESSING;
		}
		else
		{
			vp = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
		}

		DWORD iMultiSampType = D3DMULTISAMPLE_4_SAMPLES;
		DWORD iMultiSampQuality = 4;
		DWORD iQualityResult;
		hr = pD3D->CheckDeviceMultiSampleType(
			D3DADAPTER_DEFAULT,
			D3DDEVTYPE_HAL,
			D3DFMT_A8R8G8B8,
			bWindowed,
			D3DMULTISAMPLE_4_SAMPLES,
			&iQualityResult
			);
		if (FAILED(hr))
		{
			MessageBox(0, "Get MultiSamp Information Error!", 0, 0);
			return false;
		}

		if (iQualityResult < iMultiSampQuality)
		{
			iMultiSampType = D3DMULTISAMPLE_NONE;
			iMultiSampQuality = 0;
		}

		D3DPRESENT_PARAMETERS d3dpp;
		d3dpp.BackBufferWidth = width;
		d3dpp.BackBufferHeight = height;
		d3dpp.BackBufferFormat = D3DFMT_A8R8G8B8;
		d3dpp.BackBufferCount = 1;
		d3dpp.MultiSampleType = (D3DMULTISAMPLE_TYPE)iMultiSampType;
		d3dpp.MultiSampleQuality = iMultiSampQuality;
		/*d3dpp.MultiSampleType = D3DMULTISAMPLE_NONE;
		d3dpp.MultiSampleQuality = 0;*/
		d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
		d3dpp.Windowed = bWindowed;
		d3dpp.hDeviceWindow = hWnd;
		d3dpp.EnableAutoDepthStencil = TRUE;
		d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;
		d3dpp.Flags = 0;
		d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
		d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

		hr = pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, vp, &d3dpp, device);
		if (FAILED(hr))
		{
			MessageBox(0, "Create Device Failed!", 0, 0);
			return false;
		}

		pD3D->Release();
		return true;
	}

	int EnterMsgLoop(
		bool(*ptr_display)(DWORD timeDelta))
	{
		MSG msg;
		memset(&msg, 0, sizeof(msg));
		DWORD dLastTime = timeGetTime();
		while (true)
		{
			while (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
			{
				if (msg.message == WM_QUIT)
					break;
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}

			if (msg.message == WM_QUIT)
				break;
			DWORD dCurTime = timeGetTime();
			DWORD dDeltaTime = dCurTime - dLastTime;
			dLastTime = dCurTime;
			ptr_display(dDeltaTime);
		}
		return msg.wParam;
	}
}
