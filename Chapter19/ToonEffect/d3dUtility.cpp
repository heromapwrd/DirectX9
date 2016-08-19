/*
------------------------------------------------------------
d3dUtility.cpp
------------------------------------------------------------
*/

/////////////////////////////////////////////////////////////
//				Global variables
/////////////////////////////////////////////////////////////
#include "d3dUtility.h"
#include "Vertex.h"

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
		wc.style = CS_HREDRAW | CS_VREDRAW;
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

		// 检测是否支持多重纹理映射
		DWORD iMultiSampType = D3DMULTISAMPLE_4_SAMPLES;
		DWORD iMultiSampQuality = 4;
		DWORD iQualityResult;
		
		hr = pD3D->CheckDeviceMultiSampleType(
			D3DADAPTER_DEFAULT,
			D3DDEVTYPE_HAL/*D3DDEVTYPE_SW*/,
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

		// 设置设备参数
		D3DPRESENT_PARAMETERS d3dpp;
		d3dpp.BackBufferWidth = width;
		d3dpp.BackBufferHeight = height;
		d3dpp.BackBufferFormat = D3DFMT_A8R8G8B8;
		d3dpp.BackBufferCount = 1;
		d3dpp.MultiSampleType = (D3DMULTISAMPLE_TYPE)iMultiSampType;
		d3dpp.MultiSampleQuality = iMultiSampQuality;
		d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
		d3dpp.Windowed = bWindowed;
		d3dpp.hDeviceWindow = hWnd;
		d3dpp.EnableAutoDepthStencil = TRUE;
		d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;
		d3dpp.Flags = 0;
		d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
		d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

		// 创建设备
		hr = pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, vp, &d3dpp, device);
		if (FAILED(hr))
		{
			MessageBox(0, "Create Device Failed!", 0, 0);
			return false;
		}

		// 释放IDirect3D9指针
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
			if (!ptr_display(dDeltaTime))
				break;
		}
		return msg.wParam;
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

	bool DrawBasicScene(IDirect3DDevice9* device, float scale)
	{
		static IDirect3DVertexBuffer9* floor = 0;
		static IDirect3DTexture9*      tex = 0;
		static ID3DXMesh*              pillar = 0;

		HRESULT hr = 0;

		if (device == 0)
		{
			if (floor && tex && pillar)
			{
				// they already exist, destroy them
				d3d9::Release<IDirect3DVertexBuffer9*>(floor);
				d3d9::Release<IDirect3DTexture9*>(tex);
				d3d9::Release<ID3DXMesh*>(pillar);
			}
		}
		else if (!floor && !tex && !pillar)
		{
			// they don't exist, create them
			device->CreateVertexBuffer(
				6 * sizeof(Vertex),
				0,
				Vertex::FVF,
				D3DPOOL_MANAGED,
				&floor,
				0);

			Vertex* v = 0;
			floor->Lock(0, 0, (void**)&v, 0);

			v[0] = Vertex(-20.0f, -2.5f, -20.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f);
			v[1] = Vertex(-20.0f, -2.5f, 20.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f);
			v[2] = Vertex(20.0f, -2.5f, 20.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f);

			v[3] = Vertex(-20.0f, -2.5f, -20.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f);
			v[4] = Vertex(20.0f, -2.5f, 20.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f);
			v[5] = Vertex(20.0f, -2.5f, -20.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f);

			floor->Unlock();

			D3DXCreateCylinder(device, 0.5f, 0.5f, 5.0f, 20, 20, &pillar, 0);

			D3DXCreateTextureFromFile(
				device,
				"desert.bmp",
				&tex);
		}
		else
		{
			//
			// Pre-Render Setup
			//
			device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
			device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
			device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

			D3DXVECTOR3 dir(0.707f, -0.707f, 0.707f);
			D3DXCOLOR col(1.0f, 1.0f, 1.0f, 1.0f);
			D3DLIGHT9 light = d3d9::InitDirLight(&dir, &col);

			device->SetLight(0, &light);
			device->LightEnable(0, true);
			device->SetRenderState(D3DRS_NORMALIZENORMALS, true);
			device->SetRenderState(D3DRS_SPECULARENABLE, true);

			//
			// Render
			//

			D3DXMATRIX T, R, P, S;

			D3DXMatrixScaling(&S, scale, scale, scale);

			// used to rotate cylinders to be parallel with world's y-axis
			D3DXMatrixRotationX(&R, -D3DX_PI * 0.5f);

			// draw floor
			D3DXMatrixIdentity(&T);
			T = T * S;
			device->SetTransform(D3DTS_WORLD, &T);
			device->SetMaterial(&d3d9::WHITE_MTRL);
			device->SetTexture(0, tex);
			device->SetStreamSource(0, floor, 0, sizeof(Vertex));
			device->SetFVF(Vertex::FVF);
			device->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 2);

			// draw pillars
			device->SetMaterial(&d3d9::BLUE_MTRL);
			device->SetTexture(0, 0);
			for (int i = 0; i < 5; i++)
			{
				D3DXMatrixTranslation(&T, -5.0f, 0.0f, -15.0f + (i * 7.5f));
				P = R * T * S;
				device->SetTransform(D3DTS_WORLD, &P);
				pillar->DrawSubset(0);

				D3DXMatrixTranslation(&T, 5.0f, 0.0f, -15.0f + (i * 7.5f));
				P = R * T * S;
				device->SetTransform(D3DTS_WORLD, &P);
				pillar->DrawSubset(0);
			}
		}
		return true;
	}


	float GetRandomFloat(float lowBound, float highBound)
	{
		if (lowBound>=highBound)
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


	bool CheckShaderVersion(LPDIRECT3DDEVICE9 g_pd3dDevice)
	{
		// Get device capabilities
		D3DCAPS9 caps;
		g_pd3dDevice->GetDeviceCaps(&caps);
		
		// Make sure vertex shader version greater than 2.0
		if (caps.VertexShaderVersion < D3DVS_VERSION(2, 0))
		{
			    return false;
		}
		
		// Make sure pixel shader version greater than 2.0
		if (caps.PixelShaderVersion < D3DPS_VERSION(2, 0))
		{
			return false;
		}
	
		return true;
	};
}
