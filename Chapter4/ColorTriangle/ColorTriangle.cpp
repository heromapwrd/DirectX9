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

struct Vertex
{
	Vertex(){}
	Vertex(float _x, float _y, float _z,D3DCOLOR c)
	{
		x = _x;
		y = _y;
		z = _z;
		diffuse = c;
	}
	float x, y, z;
	D3DCOLOR diffuse;
	static const DWORD FVF;
};

const DWORD Vertex::FVF = D3DFVF_XYZ|D3DFVF_DIFFUSE;


// 进入循环之前的设置
bool SetUp()
{
	// 创建顶点缓存
	g_pDevice->CreateVertexBuffer(
		8 * sizeof(Vertex),
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
	pVertex[0] = Vertex(-1.0f, -1.0f, -1.0f,d3d9::WHITE_COLOR);
	pVertex[1] = Vertex(-1.0f, 1.0f, -1.0f,d3d9::RED_COLOR);
	pVertex[2] = Vertex(1.0f, 1.0f, -1.0f,d3d9::GREEN_COLOR);
	pVertex[3] = Vertex(1.0f, -1.0f, -1.0f,d3d9::BLUE_COLOR);
	pVertex[4] = Vertex(-1.0f, -1.0f, 1.0f,d3d9::YELLOW_COLOR);
	pVertex[5] = Vertex(-1.0f, 1.0f, 1.0f,d3d9::CYAN_COLOR);
	pVertex[6] = Vertex(1.0f, 1.0f, 1.0f,d3d9::BLACK_COLOR);
	pVertex[7] = Vertex(1.0f, -1.0f, 1.0f,d3d9::MAGENTA);
	VB->Unlock();

	// 初始化索引缓存
	DWORD* pIndex;
	IB->Lock(0, 0, (void**)&pIndex, 0);
	// 前面
	pIndex[0] = 0; pIndex[1] = 1; pIndex[2] = 2;
	pIndex[3] = 0; pIndex[4] = 2; pIndex[5] = 3;

	// 后面
	pIndex[6] = 4; pIndex[7] = 6; pIndex[8] = 5;
	pIndex[9] = 4; pIndex[10] = 7; pIndex[11] = 6;

	// 左面
	pIndex[12] = 4; pIndex[13] = 5; pIndex[14] = 1;
	pIndex[15] = 4; pIndex[16] = 1; pIndex[17] = 0;

	// 右面
	pIndex[18] = 3; pIndex[19] = 2; pIndex[20] = 6;
	pIndex[21] = 3; pIndex[22] = 6; pIndex[23] = 7;

	// 上面
	pIndex[24] = 1; pIndex[25] = 5; pIndex[26] = 6;
	pIndex[27] = 1; pIndex[28] = 6; pIndex[29] = 2;

	// 下面
	pIndex[30] = 4; pIndex[31] = 0; pIndex[32] = 3;
	pIndex[33] = 4; pIndex[34] = 3; pIndex[35] = 7;

	IB->Unlock();

	// 初始化视觉矩阵,投影矩阵
	// 相机位置,视点位置,竖直向上向量
	D3DXVECTOR3 eye(0.0f, 0.0f, -5.0f);
	D3DXVECTOR3 lookat(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 Up(0.0f, 1.0f, 0.0f);
	D3DXMATRIX view;
	D3DXMatrixLookAtLH(&view, &eye, &lookat, &Up);
	g_pDevice->SetTransform(D3DTS_VIEW, &view);

	// 竖直视角,宽/高比,近裁剪面,远裁剪面
	D3DXMATRIX proj;
	D3DXMatrixPerspectiveFovLH(&proj, D3DX_PI*0.5f, (float)Width / (float)Height, 1.0f, 1000.0f);
	g_pDevice->SetTransform(D3DTS_PROJECTION, &proj);

	// 设置图元绘制方式
	//g_pDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	g_pDevice->SetRenderState(D3DRS_LIGHTING, false);
	return true;
}

// 退出循环之后是清理工作
void CleanUp()
{
	d3d9::Release<IDirect3DVertexBuffer9*>(VB);
	d3d9::Release<IDirect3DIndexBuffer9*>(IB);
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

		D3DXMATRIX Translation;
		D3DXMatrixTranslation(&Translation, 0.0f, 0.0f, 5.0f);
		D3DXMATRIX world = Rx*Ry*Translation;
		//D3DXMATRIX world = Rx*Ry;

		g_pDevice->SetTransform(D3DTS_WORLD, &world);

		g_pDevice->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xffffffff, 1.0f, 0);
		g_pDevice->BeginScene();
		g_pDevice->SetStreamSource(0, VB, 0, sizeof(Vertex));
		g_pDevice->SetIndices(IB);
		g_pDevice->SetFVF(Vertex::FVF);
		g_pDevice->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_GOURAUD);
		g_pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 8, 0, 12);
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
