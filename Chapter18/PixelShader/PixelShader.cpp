#include "d3dUtility.h"

IDirect3DDevice9* g_pDevice = NULL;
int Width = 640;
int Height = 480;

IDirect3DVertexBuffer9* g_pVB = NULL;
IDirect3DPixelShader9* g_pPixelShader = NULL;
ID3DXConstantTable* g_pConstTable = NULL;
IDirect3DTexture9* g_pBase = NULL;
IDirect3DTexture9* g_pLight = NULL;
IDirect3DTexture9* g_pString = NULL;

D3DXHANDLE g_hBaseTex = NULL;
D3DXHANDLE g_hLightTex = NULL;
D3DXHANDLE g_hStringTex = NULL;

D3DXCONSTANT_DESC g_BaseDesc;
D3DXCONSTANT_DESC g_LightDesc;
D3DXCONSTANT_DESC g_StringDesc;

struct Tex_Vertex 
{
	float x, y, z;
	float u1, v1;
	float u2, v2;
	float u3, v3;
	Tex_Vertex(float _x, float _y, float _z, float _u1, float _v1, float _u2, float _v2, float _u3, float _v3)
	{
		x = _x;
		y = _y;
		z = _z;
		u1 = _u1;
		v1 = _v1;
		u2 = _u2;
		v2 = _v2;
		u3 = _u3;
		v3 = _v3;
	}
	static const DWORD FVF = D3DFVF_XYZ | D3DFVF_TEX3;
};


// 退出循环之后是清理工作
void CleanUp()
{
	d3d9::Release(g_pVB);
	d3d9::Release(g_pPixelShader);
	d3d9::Release(g_pConstTable);
}

// 进入循环之前的设置
bool SetUp()
{
	HRESULT hr = 0;
	hr = g_pDevice->CreateVertexBuffer(
		6 * sizeof(Tex_Vertex),
		D3DUSAGE_WRITEONLY,
		Tex_Vertex::FVF,
		D3DPOOL_MANAGED,
		&g_pVB,
		NULL);
	if (FAILED(hr))
	{
		MessageBox(0, "Failed to create vertex buff!", 0, 0);
		return false;
	}

	Tex_Vertex* pVertices = NULL;
	g_pVB->Lock(0, 0, (void**)&pVertices, 0);
	pVertices[0] = Tex_Vertex(-10.0f, -10.0f, 5.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f);
	pVertices[1] = Tex_Vertex(-10.0f, 10.0f, 5.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	pVertices[2] = Tex_Vertex(10.0f, 10.0f, 5.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f);

	pVertices[3] = Tex_Vertex(-10.0f, -10.0f, 5.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f);
	pVertices[4] = Tex_Vertex(10.0f, 10.0f, 5.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f);
	pVertices[5] = Tex_Vertex(10.0f, -10.0f, 5.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f);

	g_pDevice->SetRenderState(D3DRS_LIGHTING, false);

	g_pVB->Unlock();

	if (!d3d9::CheckShaderVersion(g_pDevice))
	{
		MessageBox(0, "Shader version can not support!", 0, 0);
		return false;
	}

	ID3DXBuffer *pShaderCache;
	ID3DXBuffer *pErrorMsg;
	hr = D3DXCompileShaderFromFile(
		"PixelShader.txt",
		NULL,
		NULL,
		"Main",
		"ps_2_0",
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

	hr = g_pDevice->CreatePixelShader((DWORD*)pShaderCache->GetBufferPointer(), &g_pPixelShader);

	if (FAILED(hr))
	{
		MessageBox(0, "Failed to create pixel shader!", 0, 0);
		d3d9::Release(pShaderCache);
		return false;
	}
	d3d9::Release(pShaderCache);


	g_hBaseTex = g_pConstTable->GetConstantByName(NULL, "BaseTex");
	if (!g_hBaseTex)
	{
		MessageBox(0, "Get BaseTex failed!", 0, 0);
		return false;
	}

	g_hLightTex = g_pConstTable->GetConstantByName(NULL, "LightTex");
	if (!g_hLightTex)
	{
		MessageBox(0, "Get LightTex failed!", 0, 0);
		return false;
	}

	g_hStringTex = g_pConstTable->GetConstantByName(NULL, "StringTex");
	if (!g_hStringTex)
	{
		MessageBox(0, "Get StringTex failed!", 0, 0);
		return false;
	}

	g_pConstTable->SetDefaults(g_pDevice);

	// Create Texture 
	{
		hr = D3DXCreateTextureFromFile(
			g_pDevice,
			"crate.bmp",
			&g_pBase
			);
		if (FAILED(hr))
			return false;

		hr = D3DXCreateTextureFromFile(
			g_pDevice,
			"spotlight.bmp",
			&g_pLight
			);
		if (FAILED(hr))
			return false;

		hr = D3DXCreateTextureFromFile(
			g_pDevice,
			"text.bmp",
			&g_pString
			);
		if (FAILED(hr))
			return false;
	}

	UINT count;
	hr = g_pConstTable->GetConstantDesc(g_hBaseTex, &g_BaseDesc, &count);
	if (FAILED(hr))
		return false;
	hr = g_pConstTable->GetConstantDesc(g_hLightTex, &g_LightDesc, &count);
	if (FAILED(hr))
		return false;
	hr = g_pConstTable->GetConstantDesc(g_hStringTex, &g_StringDesc, &count);
	if (FAILED(hr))
		return false;

	// Set Sampler Attribute
	g_pDevice->SetTexture(g_BaseDesc.RegisterIndex, g_pBase);
	g_pDevice->SetSamplerState(g_BaseDesc.RegisterIndex, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	g_pDevice->SetSamplerState(g_BaseDesc.RegisterIndex, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	g_pDevice->SetSamplerState(g_BaseDesc.RegisterIndex, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

	g_pDevice->SetTexture(g_LightDesc.RegisterIndex, g_pLight);
	g_pDevice->SetSamplerState(g_LightDesc.RegisterIndex, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	g_pDevice->SetSamplerState(g_LightDesc.RegisterIndex, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	g_pDevice->SetSamplerState(g_LightDesc.RegisterIndex, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

	g_pDevice->SetTexture(g_StringDesc.RegisterIndex, g_pString);
	g_pDevice->SetSamplerState(g_StringDesc.RegisterIndex, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	g_pDevice->SetSamplerState(g_StringDesc.RegisterIndex, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	g_pDevice->SetSamplerState(g_StringDesc.RegisterIndex, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

	// 竖直视角,宽/高比,近裁剪面,远裁剪面
	D3DXMATRIX proj;
	D3DXMatrixPerspectiveFovLH(&proj, D3DX_PI*0.25f, (float)Width / (float)Height, 1.0f, 100.0f);
	g_pDevice->SetTransform(D3DTS_PROJECTION, &proj);

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
		g_pDevice->SetTransform(D3DTS_VIEW, &view);

		// Render

		g_pDevice->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x00000000, 1.0f, 0);
		g_pDevice->BeginScene();
		g_pDevice->SetFVF(Tex_Vertex::FVF);
		g_pDevice->SetPixelShader(g_pPixelShader);
		g_pDevice->SetStreamSource(0, g_pVB, 0, sizeof(Tex_Vertex));
		g_pDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 2);
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