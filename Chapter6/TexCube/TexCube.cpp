/*
--------------------------------------------------------------
主函数部分
--------------------------------------------------------------
*/

#include "d3dUtility.h"
IDirect3DDevice9 * g_pDevice = 0;
const int Width = 640;
const int Height = 480;
IDirect3DVertexBuffer9* VB = 0;
IDirect3DIndexBuffer9 * IB = 0;
IDirect3DTexture9*      TX = 0;

struct Vertex
{
	Vertex(){}
	Vertex(float _x, float _y, float _z, float _u, float _v)
	{
		x = _x;
		y = _y;
		z = _z;
		u = _u;
		v = _v;
	}
	float x, y, z;
	float u, v;

	static const DWORD FVF;
};

const DWORD Vertex::FVF = D3DFVF_XYZ | D3DFVF_TEX1;


// 进入循环之前的设置
bool SetUp()
{

	// 创建顶点缓存
	g_pDevice->CreateVertexBuffer(
		24 * sizeof(Vertex),
		D3DUSAGE_WRITEONLY,
		Vertex::FVF,
		D3DPOOL_MANAGED,
		&VB,
		0
		);
	// 创建索引缓存
	g_pDevice->CreateIndexBuffer(
		36 * sizeof(DWORD),
		D3DUSAGE_WRITEONLY,
		D3DFMT_INDEX32,
		D3DPOOL_MANAGED,
		&IB,
		0
		);

	// 初始化顶点缓存
	Vertex* pVertex;
	VB->Lock(0, 0, (void**)&pVertex, 0);
	// frontface
	pVertex[0] = Vertex(-5.0f, 5.0f, -5.0f, 0.0f, 0.0f);
	pVertex[1] = Vertex(-5.0f, -5.0f, -5.0f, 1.0f, 0.0f);
	pVertex[2] = Vertex(5.0f, 5.0f, -5.0f, 0.0f, 1.0f);
	pVertex[3] = Vertex(5.0f, -5.0f, -5.0f, 1.0f, 1.0f);
	// backface
	pVertex[4] = Vertex(5.0f, 5.0f, 5.0f, 0.0f, 0.0f);
	pVertex[5] = Vertex(-5.0f, 5.0f, 5.0f, 1.0f, 0.0f);
	pVertex[6] = Vertex(5.0f, -5.0f, 5.0f, 0.0f, 1.0f);
	pVertex[7] = Vertex(-5.0f, -5.0f, 5.0f, 1.0f, 1.0f);
	// leftface
	pVertex[8] = Vertex(-5.0f, 5.0f, 5.0f, 0.0f, 0.0f);
	pVertex[9] = Vertex(-5.0f, 5.0f, -5.0f, 1.0f, 0.0f);
	pVertex[10] = Vertex(-5.0f, -5.0f, 5.0f, 0.0f, 1.0f);
	pVertex[11] = Vertex(-5.0f, -5.0f, -5.0f, 1.0f, 1.0f);
	// rightface
	pVertex[12] = Vertex(5.0f, 5.0f, -5.0f, 0.0f, 0.0f);
	pVertex[13] = Vertex(5.0f, 5.0f, 5.0f, 1.0f, 0.0f);
	pVertex[14] = Vertex(5.0f, -5.0f, -5.0f, 0.0f, 1.0f);
	pVertex[15] = Vertex(5.0f, -5.0f, 5.0f, 1.0f, 1.0f);
	// topface
	pVertex[16] = Vertex(-5.0f, 5.0f, 5.0f, 0.0f, 0.0f);
	pVertex[17] = Vertex(-5.0f, 5.0f, -5.0f, 1.0f, 0.0f);
	pVertex[18] = Vertex(5.0f, 5.0f, 5.0f, 0.0f, 1.0f);
	pVertex[19] = Vertex(5.0f, 5.0f, -5.0f, 1.0f, 1.0f);
	// bottomface
	pVertex[20] = Vertex(-5.0f, -5.0f, -5.0f, 0.0f, 0.0f);
	pVertex[21] = Vertex(-5.0f, -5.0f, 5.0f, 1.0f, 0.0f);
	pVertex[22] = Vertex(5.0f, -5.0f, -5.0f, 0.0f, 1.0f);
	pVertex[23] = Vertex(5.0f, -5.0f, 5.0f, 1.0f, 1.0f);
	VB->Unlock();

	// 初始化索引缓存
	DWORD* pIndex;
	IB->Lock(0, 0, (void**)&pIndex, 0);
	// 前面
	pIndex[0] = 0; pIndex[1] = 3; pIndex[2] = 1;
	pIndex[3] = 0; pIndex[4] = 2; pIndex[5] = 3;

	// 后面
	pIndex[6] = 4; pIndex[7] = 5; pIndex[8] = 7;
	pIndex[9] = 4; pIndex[10] = 7; pIndex[11] = 6;

	// 左面
	pIndex[12] = 8; pIndex[13] = 9; pIndex[14] = 11;
	pIndex[15] = 8; pIndex[16] = 11; pIndex[17] = 10;

	// 右面
	pIndex[18] = 12; pIndex[19] = 13; pIndex[20] = 15;
	pIndex[21] = 12; pIndex[22] = 15; pIndex[23] = 14;

	// 上面
	pIndex[24] = 16; pIndex[25] = 19; pIndex[26] = 17;
	pIndex[27] = 16; pIndex[28] = 18; pIndex[29] = 19;

	// 下面
	pIndex[30] = 20; pIndex[31] = 23; pIndex[32] = 21;
	pIndex[33] = 20; pIndex[34] = 22; pIndex[35] = 23;

	IB->Unlock();

	
	// 设置光照
	g_pDevice->SetRenderState(D3DRS_LIGHTING, true);
	D3DLIGHT9 light = d3d9::InitPointLight(&D3DXVECTOR3(-50.0f, 0.0f, 0.0f), &d3d9::WHITE_COLOR);
	g_pDevice->SetLight(0, &light);
	g_pDevice->LightEnable(0, true);
	g_pDevice->SetRenderState(D3DRS_NORMALIZENORMALS, true);
	g_pDevice->SetRenderState(D3DRS_SPECULARENABLE, true);
	// 设置材质
	g_pDevice->SetMaterial(&d3d9::WHITE_MTRL);
	// 创建纹理
	D3DXCreateTextureFromFile(g_pDevice, "crate.jpg", &TX);
	g_pDevice->SetTexture(0, TX);
	g_pDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR/*D3DTEXF_ANISOTROPIC*/);
	g_pDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR/*D3DTEXF_ANISOTROPIC*/);
	//g_pDevice->SetSamplerState(0, D3DSAMP_MAXANISOTROPY, 4);
	g_pDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);


	

	// 初始化视觉矩阵,投影矩阵
	// 相机位置,视点位置,竖直向上向量
	D3DXVECTOR3 eye(0.0f, 0.0f, -30.0f);
	D3DXVECTOR3 lookat(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 Up(0.0f, 1.0f, 0.0f);
	D3DXMATRIX view;
	D3DXMatrixLookAtLH(&view, &eye, &lookat, &Up);
	g_pDevice->SetTransform(D3DTS_VIEW, &view);

	// 竖直视角,宽/高比,近裁剪面,远裁剪面
	D3DXMATRIX proj;
	D3DXMatrixPerspectiveFovLH(&proj, D3DX_PI*0.5f, (float)Width / (float)Height, 1.0f, 1000.0f);
	g_pDevice->SetTransform(D3DTS_PROJECTION, &proj);

	return true;
}

// 退出循环之后是清理工作
void CleanUp()
{
	d3d9::Release<IDirect3DVertexBuffer9*>(VB);
	d3d9::Release<IDirect3DIndexBuffer9*>(IB);
	d3d9::Release<IDirect3DTexture9*>(TX);
}

bool DisPlay(DWORD timeDelta)
{
	if (g_pDevice)
	{

		D3DXMATRIX Rx, Ry;
		D3DXMatrixRotationX(&Rx, D3DX_PI / 4.0f);

		static float y = 0.0f;
		D3DXMatrixRotationY(&Ry, y);
		float delta = (float)timeDelta / 1000.0f;
		y += delta;
		if (y > 2 * D3DX_PI)
			y = 0.0f;
		D3DXMATRIX world = Rx*Ry;
		g_pDevice->SetTransform(D3DTS_WORLD, &world);

		g_pDevice->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x00000000, 1.0f, 0);
		g_pDevice->BeginScene();
		g_pDevice->SetStreamSource(0, VB, 0, sizeof(Vertex));
		g_pDevice->SetIndices(IB);
		g_pDevice->SetFVF(Vertex::FVF);
		g_pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 24, 0, 12);
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
	if (!d3d9::Init(hInstance, 640, 480, true, D3DDEVTYPE_HAL, &g_pDevice))
	{
		MessageBox(0, "Init Failed!", 0, 0);
		return 0;
	}
	if (!SetUp())
	{
		MessageBox(0, "SetUp Failed!", 0, 0);
		return 0;
	}
	d3d9::EnterMsgLoop(DisPlay);
	CleanUp();
	g_pDevice->Release();
	return 0;
}
