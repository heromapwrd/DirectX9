#include "SilhouetteEdge.h"
//#include "silhouetteEdges.h"

IDirect3DDevice9* g_pDevice = NULL;
int Width = 640;
int Height = 480;
enum OBJECT
{
	OBJ_TEAPOT = 0,
	OBJ_SPHERE,
	OBJ_TORUS,
	OBJ_CYLINDER,
	OBJ_NUM
};
ID3DXMesh* g_ppMesh[OBJ_NUM] = { NULL };
D3DXMATRIX g_WorldMatrix[OBJ_NUM];
D3DXVECTOR4 g_Color[OBJ_NUM];

IDirect3DVertexShader9* g_pCatoonShader = NULL;
ID3DXConstantTable* g_pCatoonTable = NULL;
IDirect3DTexture9* g_pShadeTex = NULL;

D3DXHANDLE g_hCatoonWorldViewMatrix = NULL;
D3DXHANDLE g_hCatoonWorldViewProjMatrix = NULL;
D3DXHANDLE g_hLightDir = NULL;
D3DXHANDLE g_hColor = NULL;

D3DXMATRIX g_ProjMatrix;


IDirect3DVertexShader9* g_pEdgeShader = NULL;
ID3DXConstantTable* g_pEdgeTable = NULL;

D3DXHANDLE g_hEdgeWorldViewMatrix = NULL;
D3DXHANDLE g_hEdgeProjMatrix = NULL;
SilhouetteEdge* g_ppEdge[OBJ_NUM] = { NULL };



// 退出循环之后是清理工作
void CleanUp()
{
	for (int i = 0; i < OBJ_NUM; i++)
	{
		d3d9::Release(g_ppMesh[i]);
		d3d9::Delete(g_ppEdge[i]);
	}
	d3d9::Release(g_pShadeTex);
	d3d9::Release(g_pCatoonShader);
	d3d9::Release(g_pCatoonTable);
	d3d9::Release(g_pEdgeShader);
	d3d9::Release(g_pEdgeTable);
}

// 进入循环之前的设置
bool SetUp()
{
	HRESULT hr = 0;

	ID3DXBuffer* ppAdjBuff[OBJ_NUM] = { NULL };
	hr = D3DXCreateTeapot(g_pDevice, &g_ppMesh[OBJ_TEAPOT], &ppAdjBuff[OBJ_TEAPOT]);
	if (FAILED(hr))
		return false;
	hr = D3DXCreateSphere(g_pDevice, 1.0f, 20, 20, &g_ppMesh[OBJ_SPHERE], &ppAdjBuff[OBJ_SPHERE]);
	if (FAILED(hr))
		return false;
	hr = D3DXCreateTorus(g_pDevice, 0.5f, 1.0f, 20, 20, &g_ppMesh[OBJ_TORUS], &ppAdjBuff[OBJ_TORUS]);
	if (FAILED(hr))
		return false;
	hr = D3DXCreateCylinder(g_pDevice, 0.5f, 1.0f, 2.0f, 20, 20, &g_ppMesh[OBJ_CYLINDER], &ppAdjBuff[OBJ_CYLINDER]);
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

	for (int i = 0; i < OBJ_NUM; i++)
	{
		g_ppEdge[i] = new SilhouetteEdge(g_pDevice, g_ppMesh[i], ppAdjBuff[i]);

		if (!g_ppEdge[i])
		{
			MessageBox(0, "Create edge failed!", 0, 0);
			return false;
		}
	}

	for (int i = 0; i < OBJ_NUM; i++)
	{
		d3d9::Release(ppAdjBuff[i]);
	}

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
		&g_pCatoonTable
		);
	if (pErrorMsg)
	{
		MessageBox(0, (char*)pErrorMsg->GetBufferPointer(), 0, 0);
		d3d9::Release(pErrorMsg);
		return false;
	}
	d3d9::Release(pErrorMsg);

	if (!g_pCatoonTable)
	{
		MessageBox(0, "Catoon const table is NULL!", 0, 0);
		return false;
	}

	if (FAILED(hr))
	{
		MessageBox(0, "Failed to compile catoon shader from file!", 0, 0);
		return false;
	}

	hr = g_pDevice->CreateVertexShader((DWORD*)pShaderCache->GetBufferPointer(), &g_pCatoonShader);

	if (FAILED(hr))
	{
		MessageBox(0, "Failed to create catoon shader!", 0, 0);
		d3d9::Release(pShaderCache);
		return false;
	}
	d3d9::Release(pShaderCache);

	ID3DXBuffer *pEdgeShaderCache;
	ID3DXBuffer *pEdgeErrorMsg;
	hr = D3DXCompileShaderFromFile(
		"Outline.txt",
		NULL,
		NULL,
		"Main",
		"vs_1_1",
		D3DXSHADER_DEBUG,
		&pEdgeShaderCache,
		&pEdgeErrorMsg,
		&g_pEdgeTable
		);
	if (pEdgeErrorMsg)
	{
		MessageBox(0, (char*)pEdgeErrorMsg->GetBufferPointer(), 0, 0);
		d3d9::Release(pEdgeErrorMsg);
		return false;
	}
	d3d9::Release(pEdgeErrorMsg);

	if (!g_pEdgeTable)
	{
		MessageBox(0, "Edge const table is NULL!", 0, 0);
		return false;
	}

	if (FAILED(hr))
	{
		MessageBox(0, "Failed to compile edge shader from file!", 0, 0);
		return false;
	}

	hr = g_pDevice->CreateVertexShader((DWORD*)pEdgeShaderCache->GetBufferPointer(), &g_pEdgeShader);

	if (FAILED(hr))
	{
		MessageBox(0, "Failed to create edge shader!", 0, 0);
		d3d9::Release(pEdgeShaderCache);
		return false;
	}
	d3d9::Release(pEdgeShaderCache);

	hr = D3DXCreateTextureFromFile(g_pDevice, "toonshade.bmp", &g_pShadeTex);
	if (FAILED(hr))
	{
		MessageBox(0, "Failed to create texture from file!", 0, 0);
		return false;
	}

	g_pDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
	g_pDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
	g_pDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_NONE);

	g_hCatoonWorldViewMatrix = g_pCatoonTable->GetConstantByName(NULL, "WorldViewMatrix");
	if (!g_hCatoonWorldViewMatrix)
	{
		MessageBox(0, "Get CatoonWorldViewMatrix failed!", 0, 0);
		return false;
	}

	g_hCatoonWorldViewProjMatrix = g_pCatoonTable->GetConstantByName(NULL, "WorldViewProjMatrix");
	if (!g_hCatoonWorldViewProjMatrix)
	{
		MessageBox(0, "Get CatoonWorldViewProjMatrix failed!", 0, 0);
		return false;
	}

	g_hColor = g_pCatoonTable->GetConstantByName(NULL, "Color");
	if (!g_hColor)
	{
		MessageBox(0, "Get Color failed!", 0, 0);
		return false;
	}

	g_hLightDir = g_pCatoonTable->GetConstantByName(NULL, "DirToLight");
	if (!g_hLightDir)
	{
		MessageBox(0, "Get DirToLight failed!", 0, 0);
		return false;
	}

	g_hEdgeWorldViewMatrix = g_pEdgeTable->GetConstantByName(NULL, "WorldViewMatrix");
	if (!g_hEdgeWorldViewMatrix)
	{
		MessageBox(0, "Get edge worldviewmatrix failed!", 0, 0);
		return false;
	}
	g_hEdgeProjMatrix = g_pEdgeTable->GetConstantByName(NULL, "ProjMatrix");
	if (!g_hEdgeProjMatrix)
	{
		MessageBox(0, "Get edge projmatrix failed!", 0, 0);
		return false;
	}

	g_pCatoonTable->SetDefaults(g_pDevice);
	g_pEdgeTable->SetDefaults(g_pDevice);

	D3DXVECTOR4 dirtolight(-0.57f, 0.57f, -0.57f, 0.0f);
	g_pCatoonTable->SetVector(g_pDevice, g_hLightDir, &dirtolight);


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

		g_pDevice->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xffffffff, 1.0f, 0);
		g_pDevice->BeginScene();

		// Render OBJ
		g_pDevice->SetVertexShader(g_pCatoonShader);
		g_pDevice->SetTexture(0, g_pShadeTex);
		for (int i = 0; i < OBJ_NUM; i++)
		{
			D3DXMATRIX worldview, worldviewproj;
			worldview = g_WorldMatrix[i] * view;
			worldviewproj = g_WorldMatrix[i] * view*g_ProjMatrix;
			g_pCatoonTable->SetMatrix(g_pDevice, g_hCatoonWorldViewMatrix, &worldview);
			g_pCatoonTable->SetMatrix(g_pDevice, g_hCatoonWorldViewProjMatrix, &worldviewproj);
			g_pCatoonTable->SetVector(g_pDevice, g_hColor, &g_Color[i]);
			g_ppMesh[i]->DrawSubset(0);
		}

		//Render Edge
		g_pDevice->SetVertexShader(g_pEdgeShader);
		g_pDevice->SetTexture(0, 0);
		g_pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
		for (int i = 0; i < OBJ_NUM; i++)
		{
			D3DXMATRIX worldview = g_WorldMatrix[i] * view;
			g_pEdgeTable->SetMatrix(g_pDevice, g_hEdgeWorldViewMatrix, &worldview);
			g_pEdgeTable->SetMatrix(g_pDevice, g_hEdgeProjMatrix, &g_ProjMatrix);
			g_ppEdge[i]->Render();

		}
		g_pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

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