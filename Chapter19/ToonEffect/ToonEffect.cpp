#include "d3dUtility.h"
#include<vector>
using std::vector;

IDirect3DDevice9* g_pDevice = NULL;
int Width = 640;
int Height = 480;
enum OBJECTTYPE
{
	OBJ_TEAPOT = 0,
	OBJ_SPHERE,
	OBJ_TORUS,
	OBJ_CYLINDER,
	OBJ_NUM,
};

ID3DXMesh* g_ppMesh[OBJ_NUM] = {NULL};
D3DXVECTOR4  g_Color[OBJ_NUM];
D3DXMATRIX g_WorldMatrix[OBJ_NUM];

ID3DXEffect* g_pEffect = NULL;

D3DXHANDLE g_hWorldViewMatrix = NULL;
D3DXHANDLE g_hColor = NULL;

D3DXHANDLE g_hProjMatrix = NULL;
D3DXHANDLE g_hLightDir = NULL;
D3DXHANDLE g_hTex = NULL;

D3DXHANDLE g_hToonEffect = NULL;



// 退出循环之后是清理工作
void CleanUp()
{
	d3d9::Release(g_pEffect);
	for (int i = 0; i < OBJ_NUM; i++)
	{
		d3d9::Release(g_ppMesh[i]);
	}
}

// 进入循环之前的设置
bool SetUp()
{
	HRESULT hr = 0;
	
	hr = D3DXCreateTeapot(g_pDevice, &g_ppMesh[OBJ_TEAPOT], NULL);
	if (FAILED(hr))
	{
		MessageBox(0, "Failed to create teapot!", 0, 0);
		return false;
	}

	hr = D3DXCreateSphere(g_pDevice, 1.0f,20,20,&g_ppMesh[OBJ_SPHERE], NULL);
	if (FAILED(hr))
	{
		MessageBox(0, "Failed to create Sphere!", 0, 0);
		return false;
	}

	hr = D3DXCreateTorus(g_pDevice, 0.5f, 1.0f,20, 20, &g_ppMesh[OBJ_TORUS], NULL);
	if (FAILED(hr))
	{
		MessageBox(0, "Failed to create torus!", 0, 0);
		return false;
	}

	hr = D3DXCreateCylinder(g_pDevice, 0.5f, 0.5f,2.0f,20, 20, &g_ppMesh[OBJ_CYLINDER], NULL);
	if (FAILED(hr))
	{
		MessageBox(0, "Failed to create cylinder!", 0, 0);
		return false;
	}

	D3DXMatrixTranslation(&g_WorldMatrix[OBJ_TEAPOT], 0.0f, 2.0f, 0.0f);
	D3DXMatrixTranslation(&g_WorldMatrix[OBJ_SPHERE], 0.0f, -2.0f, 0.0f);
	D3DXMatrixTranslation(&g_WorldMatrix[OBJ_TORUS], -3.0f, 0.0f, 0.0f);
	D3DXMatrixTranslation(&g_WorldMatrix[OBJ_CYLINDER], 3.0f, 0.0f, 0.0f);

	g_Color[OBJ_TEAPOT] = D3DXVECTOR4(1.0f, 0.0f, 0.0f, 1.0f);
	g_Color[OBJ_SPHERE] = D3DXVECTOR4(0.0f, 1.0f, 0.0f, 1.0f);
	g_Color[OBJ_TORUS] = D3DXVECTOR4(1.0f, 0.0f, 1.0f, 1.0f);
	g_Color[OBJ_CYLINDER] = D3DXVECTOR4(1.0f, 1.0f, 0.0f, 1.0f);



	ID3DXBuffer* pErrBuff = NULL;
	hr = D3DXCreateEffectFromFile(
		g_pDevice,
		"ToonEffect.txt",
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
		return false;
	}
	d3d9::Release(pErrBuff);
	if (FAILED(hr))
	{
		MessageBox(0, "Failed to create effect file!", 0, 0);
		return false;
	}

	g_hWorldViewMatrix = g_pEffect->GetParameterByName(NULL, "WorldViewMatrix");
	if (!g_hWorldViewMatrix)
	{
		MessageBox(0, "Failed to get WorldViewMatrix!", 0, 0);
		return false;
	}

	g_hProjMatrix = g_pEffect->GetParameterByName(NULL, "ProjMatrix");
	if (!g_hProjMatrix)
	{
		MessageBox(0, "Failed to get ProjMatrix!", 0, 0);
		return false;
	}

	g_hLightDir = g_pEffect->GetParameterByName(NULL, "LightDir");
	if (!g_hLightDir)
	{
		MessageBox(0, "Failed to get LightDir!", 0, 0);
		return false;
	}

	g_hColor = g_pEffect->GetParameterByName(NULL, "Color");
	if (!g_hColor)
	{
		MessageBox(0, "Failed to get Color!", 0, 0);
		return false;
	}

	g_hTex = g_pEffect->GetParameterByName(NULL, "Tex");
	if (!g_hTex)
	{
		MessageBox(0, "Failed to get Tex!", 0, 0);
		return false;
	}

	g_hToonEffect = g_pEffect->GetTechniqueByName("ToonEffect");
	if (!g_hToonEffect)
	{
		MessageBox(0, "Failed to get ToonEffect!", 0, 0);
		return false;
	}

	D3DXVECTOR4 lightdir(-0.57f, 0.57f, -0.57f, 0.0f);

	hr = g_pEffect->SetVector(g_hLightDir, &lightdir);
	if (FAILED(hr))
	{
		MessageBox(0, "Failed to set LightDir!", 0, 0);
		return false;
	}

	IDirect3DTexture9* pTex = NULL;
	hr = D3DXCreateTextureFromFile(
		g_pDevice,
		"toonshade.bmp",
		&pTex
		);
	if (FAILED(hr))
	{
		MessageBox(0, "Failed to create texture from file!", 0, 0);
		return false;
	}
	hr = g_pEffect->SetTexture(g_hTex, pTex);
	if (FAILED(hr))
	{
		MessageBox(0, "Failed to set Tex!", 0, 0);
		return false;
	}
	d3d9::Release(pTex);
	// 竖直视角,宽/高比,近裁剪面,远裁剪面
	D3DXMATRIX proj;
	D3DXMatrixPerspectiveFovLH(&proj, D3DX_PI*0.25f, (float)Width / (float)Height, 1.0f, 1000.0f);
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
		D3DXMATRIX WorldViewMatrix;

		// Render

		g_pDevice->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x00000000, 1.0f, 0);
		g_pDevice->BeginScene();

		g_pEffect->SetTechnique(g_hToonEffect);

		UINT numpass = 0;
		g_pEffect->Begin(&numpass, 0);
		for (int i = 0; i < numpass; i++)
		{
			g_pEffect->BeginPass(i);
				WorldViewMatrix = g_WorldMatrix[OBJ_TEAPOT] * view;
				g_pEffect->SetVector(g_hColor, &g_Color[OBJ_TEAPOT]);
				g_pEffect->SetMatrix(g_hWorldViewMatrix, &WorldViewMatrix);
				g_ppMesh[OBJ_TEAPOT]->DrawSubset(0);
			g_pEffect->EndPass();
		}
		g_pEffect->End();

		g_pEffect->Begin(&numpass, 0);
		for (int i = 0; i < numpass; i++)
		{
			g_pEffect->BeginPass(i);
				WorldViewMatrix = g_WorldMatrix[OBJ_SPHERE] * view;
				g_pEffect->SetVector(g_hColor, &g_Color[OBJ_SPHERE]);
				g_pEffect->SetMatrix(g_hWorldViewMatrix, &WorldViewMatrix);
				g_ppMesh[OBJ_SPHERE]->DrawSubset(0);
			g_pEffect->EndPass();
		}
		g_pEffect->End();

		g_pEffect->Begin(&numpass, 0);
		for (int i = 0; i < numpass; i++)
		{
			g_pEffect->BeginPass(i);
			WorldViewMatrix = g_WorldMatrix[OBJ_TORUS] * view;
			g_pEffect->SetVector(g_hColor, &g_Color[OBJ_TORUS]);
			g_pEffect->SetMatrix(g_hWorldViewMatrix, &WorldViewMatrix);
			g_ppMesh[OBJ_TORUS]->DrawSubset(0);
			g_pEffect->EndPass();
		}
		g_pEffect->End();

		g_pEffect->Begin(&numpass, 0);
		for (int i = 0; i < numpass; i++)
		{
			g_pEffect->BeginPass(i);
			WorldViewMatrix = g_WorldMatrix[OBJ_CYLINDER] * view;
			g_pEffect->SetVector(g_hColor, &g_Color[OBJ_CYLINDER]);
			g_pEffect->SetMatrix(g_hWorldViewMatrix, &WorldViewMatrix);
			g_ppMesh[OBJ_CYLINDER]->DrawSubset(0);
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
