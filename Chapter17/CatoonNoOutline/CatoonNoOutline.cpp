#include "d3dUtility.h"

IDirect3DDevice9* g_pDevice = NULL;
int Width = 640;
int Height = 480;
enum OBJECT
{
	TEAPOT = 0,
	SPHERE,
	TORUS,
	CYLINDER,
	OBJECT_NUM
};
ID3DXMesh* g_ppMesh[OBJECT_NUM] = { NULL };
D3DXMATRIX g_WorldMatrix[OBJECT_NUM];
IDirect3DVertexShader9* g_pVertexShader = NULL;
ID3DXConstantTable* g_pConstTable = NULL;

D3DXHANDLE g_hWorldViewMatrix = NULL;
D3DXHANDLE g_hWorldViewProjMatrix = NULL;
D3DXHANDLE g_hLightDir = NULL;
D3DXHANDLE g_hColor = NULL;
IDirect3DTexture9* g_pShadeTex = NULL;

D3DXVECTOR4 g_Color[OBJECT_NUM];
D3DXMATRIX g_ProjMatrix;


// 退出循环之后是清理工作
void CleanUp()
{
	for (int i = 0; i < OBJECT_NUM; i++)
	{
		d3d9::Release(g_ppMesh[i]);
	}
	d3d9::Release(g_pShadeTex);
	d3d9::Release(g_pVertexShader);
	d3d9::Release(g_pConstTable);
}

// 进入循环之前的设置
bool SetUp()
{
	HRESULT hr = 0;

	hr = D3DXCreateTeapot(g_pDevice, &g_ppMesh[0], NULL);
	if (FAILED(hr))
		return false;
	hr = D3DXCreateSphere(g_pDevice, 1.0f, 20, 20, &g_ppMesh[1], NULL);
	if (FAILED(hr))
		return false;
	hr = D3DXCreateTorus(g_pDevice, 0.5f, 1.0f, 20, 20, &g_ppMesh[2], NULL);
	if (FAILED(hr))
		return false;
	hr = D3DXCreateCylinder(g_pDevice, 0.5f, 1.0f, 2.0f, 20, 20, &g_ppMesh[3], NULL);
	if (FAILED(hr))
		return false;

	D3DXMatrixTranslation(&g_WorldMatrix[0], 0.0f, 2.0f, 0.0f);
	D3DXMatrixTranslation(&g_WorldMatrix[1], 0.0f, -2.0f, 0.0f);
	D3DXMatrixTranslation(&g_WorldMatrix[2], -3.0f, 0.0f, 0.0f);
	D3DXMatrixTranslation(&g_WorldMatrix[3], 3.0f, 0.0f, 0.0f);

	g_Color[0] = D3DXVECTOR4(1.0f, 0.0f, 0.0f, 1.0f);
	g_Color[1] = D3DXVECTOR4(0.0f, 1.0f, 0.0f, 1.0f);
	g_Color[2] = D3DXVECTOR4(0.0f, 0.0f, 1.0f, 1.0f);
	g_Color[3] = D3DXVECTOR4(1.0f, 1.0f, 0.0f, 1.0f);


	if (!d3d9::CheckShaderVersion(g_pDevice))
	{
		MessageBox(0, "Shader version can not support!", 0, 0);
		return false;
	}
	
	ID3DXBuffer *pShaderCache;
	ID3DXBuffer *pErrorMsg;
	hr = D3DXCompileShaderFromFile(
		"Toon.txt",
		NULL,
		NULL,
		"Main",
		"vs_1_1",
		D3DXSHADER_DEBUG,
		&pShaderCache,
		&pErrorMsg,
		&g_pConstTable
		);
	if (pErrorMsg)
	{
		MessageBox(0, (char*)pErrorMsg->GetBufferPointer(), 0, 0);
		d3d9::Release(pErrorMsg);
		return false;
	}
	d3d9::Release(pErrorMsg);

	if (!g_pConstTable)
	{
		MessageBox(0, "Const table is NULL!", 0, 0);
		return false;
	}

	if (FAILED(hr))
	{
		MessageBox(0, "Failed to compile shader from file!", 0, 0);
		return false;
	}

	hr = g_pDevice->CreateVertexShader((DWORD*)pShaderCache->GetBufferPointer(), &g_pVertexShader);

	if (FAILED(hr))
	{
		MessageBox(0, "Failed to create vertex shader!", 0, 0);
		d3d9::Release(pShaderCache);
		return false;
	}
	d3d9::Release(pShaderCache);

	hr = D3DXCreateTextureFromFile(g_pDevice, "toonshade.bmp", &g_pShadeTex);
	if (FAILED(hr))
	{
		MessageBox(0, "Failed to create texture from file!", 0, 0);
		return false;
	}

	g_pDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
	g_pDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
	g_pDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_NONE);

	g_hWorldViewMatrix = g_pConstTable->GetConstantByName(NULL, "WorldViewMatrix");
	if (!g_hWorldViewMatrix)
	{
		MessageBox(0, "Get WorldViewMatrix failed!", 0, 0);
		return false;
	}

	g_hWorldViewProjMatrix = g_pConstTable->GetConstantByName(NULL, "WorldViewProjMatrix");
	if (!g_hWorldViewProjMatrix)
	{
		MessageBox(0, "Get WorldViewProjMatrix failed!", 0, 0);
		return false;
	}

	g_hColor = g_pConstTable->GetConstantByName(NULL, "Color");
	if (!g_hColor)
	{
		MessageBox(0, "Get Color failed!", 0, 0);
		return false;
	}

	g_hLightDir = g_pConstTable->GetConstantByName(NULL, "DirToLight");
	if (!g_hLightDir)
	{
		MessageBox(0, "Get DirToLight failed!", 0, 0);
		return false;
	}
	g_pConstTable->SetDefaults(g_pDevice);

	D3DXVECTOR4 dirtolight(-0.57f, 0.57f, -0.57f, 0.0f);
	g_pConstTable->SetVector(g_pDevice, g_hLightDir, &dirtolight);


	// 竖直视角,宽/高比,近裁剪面,远裁剪面
	D3DXMatrixPerspectiveFovLH(&g_ProjMatrix, D3DX_PI*0.25f, (float)Width / (float)Height, 1.0f, 100.0f);

	return true;
}

bool DisPlay(DWORD timeDelta)
{
	if (g_pDevice)
	{
		float time = (float)timeDelta / 1000.0f;
		static float height = 5.0f;
		static float angle = (3.0f*D3DX_PI) / 2.0f;
		if (GetAsyncKeyState(VK_LEFT) & 0x8000f)
			angle += 0.5f*time;
		if (GetAsyncKeyState(VK_RIGHT) & 0x8000f)
			angle -= 0.5f*time;
		if (GetAsyncKeyState(VK_UP) & 0x8000f)
			height += 5.0f*time;
		if (GetAsyncKeyState(VK_DOWN) & 0x8000f)
			height -= 5.0f*time;

		D3DXVECTOR3 pos(10.0f * cosf(angle), height, 10.0f*sinf(angle));
		D3DXVECTOR3 target(0.0f, 0.0f, 0.0f);
		D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);
		D3DXMATRIX view;
		D3DXMatrixLookAtLH(&view, &pos, &target, &up);

		// Render

		g_pDevice->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x00000000, 1.0f, 0);
		g_pDevice->BeginScene();
		g_pDevice->SetVertexShader(g_pVertexShader);
		g_pDevice->SetTexture(0, g_pShadeTex);
		for (int i = 0; i < OBJECT_NUM; i++)
		{
			D3DXMATRIX worldview, worldviewproj;
			worldview = g_WorldMatrix[i] * view;
			worldviewproj = g_WorldMatrix[i]*view*g_ProjMatrix;
			g_pConstTable->SetMatrix(g_pDevice, g_hWorldViewMatrix, &worldview);
			g_pConstTable->SetMatrix(g_pDevice, g_hWorldViewProjMatrix, &worldviewproj);
			g_pConstTable->SetVector(g_pDevice, g_hColor, &g_Color[i]);
			g_ppMesh[i]->DrawSubset(0);

		}
		g_pDevice->EndScene();
		g_pDevice->Present(0, 0, 0, 0);
	}
	return true;
}

LRESULT CALLBACK d3d9::WndProc(
	HWND hWnd,
	UINT msg,
	WPARAM wParam,
	LPARAM lParam)
{
	switch (msg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE)
			DestroyWindow(hWnd);
		break;
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

int WINAPI WinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPSTR lpCmdLine,
	_In_ int nShowCmd)
{
	if (!d3d9::Init(hInstance, Width, Height, true, D3DDEVTYPE_HAL, &g_pDevice))
	{
		MessageBox(0, "Init Failed!", 0, 0);
		return 0;
	}
	if (!SetUp())
	{
		MessageBox(0, "SetUp Failed!", 0, 0);
		goto Exit;
	}
	d3d9::EnterMsgLoop(DisPlay);
Exit:CleanUp();
	g_pDevice->Release();
	return 0;
}