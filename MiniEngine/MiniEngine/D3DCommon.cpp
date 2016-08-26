#include "D3DCommon.h"
#include <d3dx9.h>
#include<vector>
using namespace std;
using std::vector;

HRESULT InitD3D(HWND hWnd, int Width, int Height, bool bWindowed, bool bMultiThread, D3DDEVTYPE type, IDirect3DDevice9** ppDevice)
{
	IDirect3D9* pD3D = NULL;
	HRESULT hr = 0;

	if (!hWnd || !ppDevice)
		return E_FAIL;

	//初始化IDirect3D9指针
	pD3D = Direct3DCreate9(D3D_SDK_VERSION);
	if (!pD3D)
		return E_FAIL;

	//获取屏幕分辨率
	int ScreenWidth = GetSystemMetrics(SM_CXSCREEN);
	int ScreenHeight = GetSystemMetrics(SM_CYSCREEN);

	//获取并检测设备性能(硬件级定点处理)
	D3DCAPS9 caps;
	hr = pD3D->GetDeviceCaps(D3DADAPTER_DEFAULT, type, &caps);
	if (FAILED(hr))
	{
		MessageBox(0, "Failed to Get Device capacilities", 0, 0);
		return E_FAIL;
	}

	DWORD VP = 0;
	if (caps.DevCaps&D3DDEVCAPS_HWTRANSFORMANDLIGHT)
	{
		VP = D3DCREATE_HARDWARE_VERTEXPROCESSING;
	}
	else
	{
		VP = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
	}

	if (bMultiThread)
	{
		VP |= D3DCREATE_MULTITHREADED;
	}

	// 检测当前显示模式
	D3DDISPLAYMODE curmode;
	hr = pD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &curmode);
	if (FAILED(hr))
	{
		MessageBox(0, "Failed to Get Adapter display mode!", 0, 0);
		return E_FAIL;
	}

	// 获取支持当前表面格式所支持的显示模式数量
	int iModeCount = pD3D->GetAdapterModeCount(D3DADAPTER_DEFAULT, curmode.Format);

	// 存储每一种显示模式
	D3DDISPLAYMODE mode;
	vector<D3DDISPLAYMODE> modes(0);
	for (int i = 0; i < iModeCount; i++)
	{
		pD3D->EnumAdapterModes(D3DADAPTER_DEFAULT, curmode.Format, i, &mode);
		modes.push_back(mode);
	}

	// 检测后是否支持特定的设备类型
	hr = pD3D->CheckDeviceType(D3DADAPTER_DEFAULT, type, curmode.Format, /*D3DFMT_A8R8G8B8*/curmode.Format, bWindowed);
	if (FAILED(hr))
	{
		MessageBox(0, "Adapter can not support corresponding device type!", 0, 0);
		return E_FAIL;
	}

	// 检测是否支持特定的格式
	hr = pD3D->CheckDeviceFormat(D3DADAPTER_DEFAULT, type, curmode.Format, D3DUSAGE_RENDERTARGET/*D3DUSAGE_DYNAMIC*/, D3DRTYPE_SURFACE, curmode.Format);
	if (FAILED(hr))
	{
		MessageBox(0, "Adapter can not support given format!", 0, 0);
		return E_FAIL;
	}

	// 检测是否支持多采样技术
	DWORD iMultiSampType = D3DMULTISAMPLE_4_SAMPLES;
	DWORD iMultiSampQuality = 4;
	DWORD iQualityResult;

	hr = pD3D->CheckDeviceMultiSampleType(
		D3DADAPTER_DEFAULT,
		type,
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


	// 填写D3DPRESENT_PARAMETERS参数
	D3DPRESENT_PARAMETERS params;
	ZeroMemory(&params, sizeof(params));

	if (bWindowed)
	{
		RECT ClientRect, WndRect;
		GetClientRect(hWnd, &ClientRect);
		GetWindowRect(hWnd, &WndRect);

		DWORD dwWidth = (WndRect.right - WndRect.left) + (Width - ClientRect.right);
		DWORD dwHeight = (WndRect.bottom - WndRect.top) + (Height - ClientRect.bottom);
		MoveWindow(hWnd, WndRect.left, WndRect.top, dwWidth, dwHeight, TRUE);

		params.AutoDepthStencilFormat = D3DFMT_D24S8;
		params.BackBufferCount = 1;
		params.BackBufferFormat = D3DFMT_A8R8G8B8;
		params.BackBufferHeight = Height;
		params.BackBufferWidth = Width;
		params.EnableAutoDepthStencil = true;
		params.Flags = 0;
		params.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
		params.hDeviceWindow = hWnd;
		params.MultiSampleQuality = iMultiSampQuality;
		params.MultiSampleType = (D3DMULTISAMPLE_TYPE)iMultiSampType;
		params.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
		params.SwapEffect = D3DSWAPEFFECT_DISCARD;
		params.Windowed = bWindowed;
	}
	else
	{
		params.AutoDepthStencilFormat = D3DFMT_D24S8;
		params.BackBufferCount = 1;
		params.BackBufferFormat = curmode.Format;
		params.BackBufferHeight = ScreenHeight;
		params.BackBufferWidth = ScreenWidth;
		params.EnableAutoDepthStencil = true;
		params.Flags = 0;
		params.FullScreen_RefreshRateInHz = curmode.RefreshRate;
		params.hDeviceWindow = hWnd;
		params.MultiSampleQuality = iMultiSampQuality;
		params.MultiSampleType = (D3DMULTISAMPLE_TYPE)iMultiSampType;
		params.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
		params.SwapEffect = D3DSWAPEFFECT_DISCARD;
		params.Windowed = bWindowed;
	}

	// 创建IDirect3DDevice设备
	hr = pD3D->CreateDevice(D3DADAPTER_DEFAULT, type, hWnd, VP, &params, ppDevice);
	if (FAILED(hr))
	{
		MessageBox(0, "Failed to create device!", 0, 0);
		return E_FAIL;
	}
	Release<IDirect3D9 *>(pD3D);

	if ((*ppDevice) == NULL)
	{
		MessageBox(0, "Device is equivalent to NULL!", 0, 0);
		return E_FAIL;
	}

	//// 获取当前设备应用的显示模式
	//D3DDISPLAYMODE currentmode;
	//hr = (*ppDevice)->GetDisplayMode(D3DSWAPEFFECT_FORCE_DWORD, &currentmode);
	//if (FAILED(hr))
	//{
	//	MessageBox(0, "Failed to get current d3d display mode!", 0, 0);
	//	return E_FAIL;
	//}

	return S_OK;
}


D3DMATERIAL9 InitMaterial(D3DXCOLOR a, D3DXCOLOR d, D3DXCOLOR s, D3DXCOLOR e, float p)
{
	D3DMATERIAL9 material;
	material.Ambient = a;
	material.Diffuse = d;
	material.Specular = s;
	material.Emissive = e;
	material.Power = p;
	return material;
}

D3DLIGHT9 InitDirLight(const D3DXVECTOR3* dir, const D3DXCOLOR* color)
{
	D3DLIGHT9 light;
	memset(&light, 0, sizeof(light));
	light.Type = D3DLIGHT_DIRECTIONAL;
	light.Ambient = *color*0.6f;
	light.Diffuse = *color;
	light.Specular = *color*0.6f;
	light.Direction = *dir;
	return light;
}

D3DLIGHT9 InitPointLight(const D3DXVECTOR3* pos, const D3DXCOLOR* color)
{
	D3DLIGHT9 light;
	memset(&light, 0, sizeof(light));
	light.Type = D3DLIGHT_POINT;
	light.Ambient = *color*0.6f;
	light.Diffuse = *color;
	light.Specular = *color*0.6f;
	light.Position = *pos;
	light.Range = 1000.0f;
	light.Falloff = 1.0f;
	light.Attenuation0 = 1.0f;
	light.Attenuation1 = 0.0f;
	light.Attenuation2 = 0.0f;
	return light;
}

D3DLIGHT9 InitSpotLight(const D3DXVECTOR3* pos, const D3DXVECTOR3* dir, const D3DXCOLOR* color)
{
	D3DLIGHT9 light;
	memset(&light, 0, sizeof(light));
	light.Type = D3DLIGHT_SPOT;
	light.Ambient = *color*0.6f;
	light.Diffuse = *color;
	light.Specular = *color*0.6f;
	light.Position = *pos;
	light.Direction = *dir;
	light.Range = 1000.0f;
	light.Falloff = 1.0f;
	light.Attenuation0 = 1.0f;
	light.Attenuation1 = 0.0f;
	light.Attenuation2 = 0.0f;
	light.Theta = 0.4f;
	light.Phi = 0.9f;
	return light;
}

float GetRandomFloat(float lowBound, float highBound)
{
	if (lowBound >= highBound)
	{
		return lowBound;
	}
	float r = (rand() % 10000)*0.0001f;
	return lowBound + (highBound - lowBound)*r;
}

void GetRandomVector(
	D3DXVECTOR3* out,
	D3DXVECTOR3* min,
	D3DXVECTOR3* max)
{
	out->x = GetRandomFloat(min->x, max->x);
	out->y = GetRandomFloat(min->y, max->y);
	out->z = GetRandomFloat(min->z, max->z);
}

DWORD FtoDw(float f)
{
	return *(DWORD*)(&f);
}

float Lerp(float a, float b, float t)
{
	return a - a*t + b*t;
}