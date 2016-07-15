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
IDirect3DTexture9*      TX = 0;
ID3DXMesh* pTeaPot = 0;
D3DMATERIAL9 TeaPotMtrl;
D3DMATERIAL9 BGMtrl;

struct Vertex
{
	Vertex(){}
	Vertex(float _x, float _y, float _z, float _nx,float _ny,float _nz,float _u, float _v)
	{
		x = _x;
		y = _y;
		z = _z;
		nx = _nz;
		ny = _ny;
		nz = _nz;
		u = _u;
		v = _v;
	}
	float x, y, z;
	float nx, ny, nz;
	float u, v;

	static const DWORD FVF;
};

const DWORD Vertex::FVF = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1;


// 进入循环之前的设置
bool SetUp()
{
	// 创建茶壶网格缓存
	D3DXCreateTeapot(g_pDevice, &pTeaPot, 0);

	// 创建背景顶点缓存
	g_pDevice->CreateVertexBuffer(
		6 * sizeof(Vertex),
		D3DUSAGE_WRITEONLY,
		Vertex::FVF,
		D3DPOOL_MANAGED,
		&VB,
		0
		);

	// 初始化背景顶点缓存
	Vertex* pVertex;
	VB->Lock(0, 0, (void**)&pVertex, 0);
	pVertex[0] = Vertex(-10.0f, -10.0f, 5.0f, 0.0f,0.0f,-1.0f,0.0f, 1.0f);
	pVertex[1] = Vertex(-10.0f, 10.0f, 5.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f);
	pVertex[2] = Vertex(10.0f, 10.0f, 5.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);

	pVertex[3] = Vertex(-10.0f, -10.0f, 5.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
	pVertex[4] = Vertex(10.0f, 10.0f, 5.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);
	pVertex[5] = Vertex(10.0f, -10.0f, 5.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f);
	VB->Unlock();

	// 初始化背景材质
	BGMtrl = d3d9::WHITE_MTRL;
	// 初始化茶壶材质
	TeaPotMtrl = d3d9::RED_MTRL;
	TeaPotMtrl.Diffuse.a = 0.5f;
	// 初始化背景纹理
	D3DXCreateTextureFromFile(g_pDevice, "crate.jpg", &TX);
	// 纹理采样设置
	g_pDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	g_pDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	g_pDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

	// 设置融合模式(此处设置为透明模式)
	g_pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	g_pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	// 设置透明模式中Alpha通道来源
	g_pDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE);
	g_pDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);

	// 创建光源
	g_pDevice->SetRenderState(D3DRS_LIGHTING, true);
	D3DLIGHT9 light;
	memset(&light, 0, sizeof(light));
	light.Type = D3DLIGHT_DIRECTIONAL;
	light.Ambient = d3d9::WHITE_COLOR*0.6f;
	light.Specular = d3d9::WHITE_COLOR*0.2f;
	light.Diffuse = d3d9::WHITE_COLOR;
	light.Direction = D3DXVECTOR3(0.707f, 0.0f, 0.707f);
	// 注册光源
	g_pDevice->SetLight(0, &light);
	g_pDevice->LightEnable(0, true);
	g_pDevice->SetRenderState(D3DRS_LIGHTING, true);
	g_pDevice->SetRenderState(D3DRS_NORMALIZENORMALS, true);
	g_pDevice->SetRenderState(D3DRS_SPECULARENABLE, true);

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

	return true;
}

// 退出循环之后是清理工作
void CleanUp()
{
	d3d9::Release<IDirect3DVertexBuffer9*>(VB);
	d3d9::Release<IDirect3DTexture9*>(TX);
	d3d9::Release<ID3DXMesh*>(pTeaPot);
}

bool DisPlay(DWORD timeDelta)
{
	if (g_pDevice)
	{

		D3DXMATRIX IW,world;
		D3DXMatrixIdentity(&IW);

		static float y = 0.0f;
		D3DXMatrixRotationY(&world, y);
		float delta = (float)timeDelta / 1000.0f;
		y += delta;
		if (y > 2 * D3DX_PI)
			y = 0.0f;

		// 绘制图形
		g_pDevice->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x00000000, 1.0f, 0);
		g_pDevice->BeginScene();

		// 绘制背景
		g_pDevice->SetTransform(D3DTS_WORLD, &IW);      // 设置背景世界矩阵
		g_pDevice->SetStreamSource(0, VB, 0, sizeof(Vertex));
		g_pDevice->SetFVF(Vertex::FVF);
		g_pDevice->SetMaterial(&BGMtrl);				// 注册材质
		g_pDevice->SetTexture(0, TX);					// 注册纹理
		g_pDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 2);

		// 绘制茶壶
		g_pDevice->SetTransform(D3DTS_WORLD, &world);      // 设置背景世界矩阵
		g_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, true);   // 打开Alpha融合设置
		g_pDevice->SetMaterial(&TeaPotMtrl);				// 注册材质
		g_pDevice->SetTexture(0, 0);					// 注册纹理
		pTeaPot->DrawSubset(0);
		g_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, false);  // 关闭Alpha融合设置 

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
