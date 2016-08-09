#include "d3dUtility.h"
#include "Camera.h"
#include "Terrain.h"
#include<vector>
using std::vector;

IDirect3DDevice9* g_pDevice = NULL;
int Width = 640;
int Height = 480;

ID3DXEffect* g_pFogEffect = NULL;
D3DXHANDLE g_hTechFog = NULL;

Terrain* g_pTerrain = NULL;
Camera TheCamera(Camera::AIRCRAFT);

// 退出循环之后是清理工作
void CleanUp()
{
	d3d9::Release(g_pFogEffect);
	d3d9::Delete(g_pTerrain);
}

// 进入循环之前的设置
bool SetUp()
{
	HRESULT hr = 0;
	D3DXVECTOR3 lightDirection(0.0f, 1.0f, 0.0f);
	g_pTerrain = new Terrain(g_pDevice, "coastMountain64.raw", 64, 64, 6, 0.5f);
	g_pTerrain->GenerateTexture(&lightDirection);

	g_pDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	g_pDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	g_pDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

	D3DXVECTOR3 pos(100.0f, 100.0f, -250.0f);
	TheCamera.SetPos(&pos);

	ID3DXBuffer* errorBuffer = 0;
	hr = D3DXCreateEffectFromFile(
		g_pDevice,
		"Fog.txt",
		0,                // no preprocessor definitions
		0,                // no ID3DXInclude interface
		D3DXSHADER_DEBUG, // compile flags
		0,                // don't share parameters
		&g_pFogEffect,
		&errorBuffer);

	// output any error messages
	if (errorBuffer)
	{
		::MessageBox(0, (char*)errorBuffer->GetBufferPointer(), 0, 0);
		d3d9::Release<ID3DXBuffer*>(errorBuffer);
	}

	if (FAILED(hr))
	{
		::MessageBox(0, "D3DXCreateEffectFromFile() - FAILED", 0, 0);
		return false;
	}

	g_hTechFog = g_pFogEffect->GetTechniqueByName("Fog");

	// 竖直视角,宽/高比,近裁剪面,远裁剪面
	D3DXMATRIX proj;
	D3DXMatrixPerspectiveFovLH(&proj, D3DX_PI*0.25f, (float)Width / (float)Height, 1.0f, 10000.0f);
	g_pDevice->SetTransform(D3DTS_PROJECTION, &proj);

	return true;
}

bool DisPlay(DWORD timeDelta)
{
	if (g_pDevice)
	{
		float time = (float)timeDelta / 1000.0f;
		if (::GetAsyncKeyState(VK_UP) & 0x8000f)
			TheCamera.Walk(100.0f * time);

		if (::GetAsyncKeyState(VK_DOWN) & 0x8000f)
			TheCamera.Walk(-100.0f * time);

		if (::GetAsyncKeyState(VK_LEFT) & 0x8000f)
			TheCamera.Yaw(-1.0f * time);

		if (::GetAsyncKeyState(VK_RIGHT) & 0x8000f)
			TheCamera.Yaw(1.0f * time);

		if (::GetAsyncKeyState('A') & 0x8000f)
			TheCamera.Strafe(-100.0f * time);

		if (::GetAsyncKeyState('D') & 0x8000f)
			TheCamera.Strafe(100.0f * time);

		if (::GetAsyncKeyState('W') & 0x8000f)
			TheCamera.Pitch(1.0f * time);

		if (::GetAsyncKeyState('S') & 0x8000f)
			TheCamera.Pitch(-1.0f * time);

		D3DXMATRIX V;
		TheCamera.GetViewMatrix(&V);
		g_pDevice->SetTransform(D3DTS_VIEW, &V);

		// Render
		D3DXMATRIX I;
		D3DXMatrixIdentity(&I);
		g_pDevice->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x00000000, 1.0f, 0);
		g_pDevice->BeginScene();

		g_pFogEffect->SetTechnique(g_hTechFog);
		UINT numpass = 0;

		g_pFogEffect->Begin(&numpass, 0);
		for (int i = 0; i < numpass; i++)
		{
			g_pFogEffect->BeginPass(i);
			g_pTerrain->Draw(&I, false);
			g_pFogEffect->EndPass();
		}
		g_pFogEffect->End();

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