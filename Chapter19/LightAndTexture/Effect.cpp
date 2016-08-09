#include "d3dUtility.h"
#include<vector>
using std::vector;

IDirect3DDevice9* g_pDevice = NULL;
int Width = 640;
int Height = 480;

ID3DXEffect* g_pEffect = NULL;

D3DXHANDLE g_hWorldMatrix = NULL;
D3DXHANDLE g_hViewMatrix = NULL;
D3DXHANDLE g_hProjMatrix = NULL;
D3DXHANDLE g_hTex = NULL;
D3DXHANDLE g_hLightAndTexture = NULL;

ID3DXMesh* g_pMesh;
vector<D3DXMATERIAL> Mtrls(0);


// 退出循环之后是清理工作
void CleanUp()
{
	d3d9::Release(g_pEffect);
	d3d9::Release(g_pMesh);
}

// 进入循环之前的设置
bool SetUp()
{
	HRESULT hr = 0;
	ID3DXBuffer* pMaterials = NULL;
	DWORD dwMtrlNum;
	hr = D3DXLoadMeshFromX(
		"mountain.x",
		D3DXMESH_MANAGED,
		g_pDevice,
		NULL,
		&pMaterials,
		NULL,
		&dwMtrlNum,
		&g_pMesh
		);
	if (FAILED(hr))
	{
		MessageBox(0, "Failed to load mesh!", 0, 0);
		return false;
	}
	
	if (pMaterials != NULL&&dwMtrlNum != 0)
	{
		D3DXMATERIAL* pMtrl = NULL;
		pMtrl = (D3DXMATERIAL*)pMaterials->GetBufferPointer();
		for (DWORD i = 0; i < dwMtrlNum; i++)
		{
			pMtrl[i].MatD3D.Ambient = pMtrl[i].MatD3D.Diffuse;
			Mtrls.push_back(pMtrl[i]);
		}
	}
	d3d9::Release(pMaterials);

	if (!d3d9::CheckShaderVersion(g_pDevice))
	{
		MessageBox(0, "Shader version can not support!", 0, 0);
		return false;
	}

	ID3DXBuffer* pErrBuff;
	hr = D3DXCreateEffectFromFile(
		g_pDevice,
		"light_tex.txt",
		0,
		0,
		D3DXSHADER_DEBUG,
		NULL,
		&g_pEffect,
		&pErrBuff
		);
	
	if (pErrBuff)
	{
		MessageBox(0, (char*)pErrBuff->GetBufferPointer(), 0, 0);
		d3d9::Release(pErrBuff);
		return false;
	}
	d3d9::Release(pErrBuff);

	if (FAILED(hr))
	{
		MessageBox(0, "Failed to create effect from file!", 0, 0);
		return false;
	}


	g_hWorldMatrix = g_pEffect->GetParameterByName(NULL, "WorldMatrix");
	if (!g_hWorldMatrix)
	{
		MessageBox(0, "Get WorldMatrix failed!", 0, 0);
		return false;
	}
	
	g_hViewMatrix = g_pEffect->GetParameterByName(NULL, "ViewMatrix");
	if (!g_hViewMatrix)
	{
		MessageBox(0, "Get ViewMatrix failed!", 0, 0);
		return false;
	}

	g_hProjMatrix = g_pEffect->GetParameterByName(NULL, "ProjMatrix");
	if (!g_hProjMatrix)
	{
		MessageBox(0, "Get ProjMatrix failed!", 0, 0);
		return false;
	}

	g_hTex = g_pEffect->GetParameterByName(NULL, "Tex");
	if (!g_hTex)
	{
		MessageBox(0, "Get Tex failed!", 0, 0);
		return false;
	}

	g_hLightAndTexture = g_pEffect->GetTechniqueByName("LightAndTexture");
	if (!g_hLightAndTexture)
	{
		MessageBox(0, "Get technique failed!", 0, 0);
		return false;
	}
	

	// Create And Set Texture 
	{
		IDirect3DTexture9* pTex = NULL;
		hr = D3DXCreateTextureFromFile(
			g_pDevice,
			"Terrain_3x_diffcol.jpg",
			&pTex
			);
		if (FAILED(hr))
			return false;

		hr = g_pEffect->SetTexture(g_hTex, pTex);
		if (FAILED(hr))
		{
			MessageBox(0, "Failed to set texture!", 0, 0);
			return false;
		}
		d3d9::Release(pTex);
	}

	D3DXMATRIX world;
	D3DXMatrixIdentity(&world);
	hr = g_pEffect->SetMatrix(g_hWorldMatrix, &world);
	if (FAILED(hr))
	{
		MessageBox(0, "Failed to set world matrix!", 0, 0);
		return false;
	}

	// 竖直视角,宽/高比,近裁剪面,远裁剪面
	D3DXMATRIX proj;
	D3DXMatrixPerspectiveFovLH(&proj, D3DX_PI*0.25f, (float)Width / (float)Height, 1.0f, 100.0f);
	hr = g_pEffect->SetMatrix(g_hProjMatrix, &proj);
	if (FAILED(hr))
	{
		MessageBox(0, "Failed to set proj matrix!", 0, 0);
		return false;
	}

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
		HRESULT hr = 0;
		hr = g_pEffect->SetMatrix(g_hViewMatrix, &view);
		if (FAILED(hr))
		{
			MessageBox(0, "Failed to set view matrix!", 0, 0);
			return false;
		}

		// Render

		g_pDevice->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x00000000, 1.0f, 0);
		g_pDevice->BeginScene();
		g_pEffect->SetTechnique(g_hLightAndTexture);
		UINT numpass = 0;

		g_pEffect->Begin(&numpass, 0);
		for (int i = 0; i < numpass; i++)
		{
			g_pEffect->BeginPass(i);
			for (int j = 0; j < Mtrls.size(); j++)
			{
				g_pMesh->DrawSubset(j);
			}
			g_pEffect->EndPass();
		}
		g_pEffect->End();

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