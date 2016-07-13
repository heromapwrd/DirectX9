/*
--------------------------------------------------------------
主函数部分
--------------------------------------------------------------
*/

#include "d3dUtility.h"
IDirect3DDevice9 * g_pDevice = 0;
const int Width = 640;
const int Height = 480;
// 五中物体的网格数据指针
ID3DXMesh* g_pObj[5] = {NULL,NULL,NULL,NULL,NULL};
//
D3DXMATRIX g_Matrices[5];

// 进入循环之前的设置
bool SetUp()
{
	D3DXCreateTeapot(g_pDevice, &g_pObj[0], 0);
	D3DXCreateBox(g_pDevice, 2.0f, 2.0f, 2.0f, &g_pObj[1], 0);
	D3DXCreateCylinder(g_pDevice, 3.0f, 1.0f, 4.0f, 10, 10, &g_pObj[2], 0);
	D3DXCreateTorus(g_pDevice, 1.0f, 3.0f, 10, 10, &g_pObj[3], 0);
	D3DXCreateSphere(g_pDevice, 3.0f, 10, 10, &g_pObj[4], 0);

	D3DXMatrixTranslation(&g_Matrices[0], 0.0f, 0.0f, -10.0f);
	D3DXMatrixTranslation(&g_Matrices[1], 0.0f, 10.0f, 0.0f);
	D3DXMatrixTranslation(&g_Matrices[2], 0.0f, -10.0f, 0.0f);
	D3DXMatrixTranslation(&g_Matrices[3], 10.0f, 0.0f, 0.0f);
	D3DXMatrixTranslation(&g_Matrices[4], -10.0f, 0.0f, 0.0f);

	// 初始化视觉矩阵,投影矩阵
	// 相机位置,视点位置,竖直向上向量
	D3DXVECTOR3 eye(0.0f, 0.0f, -20.0f);
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
	g_pDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);

	return true;
}

// 退出循环之后是清理工作
void CleanUp()
{
	for (int i = 0; i < 5; i++)
	{
		d3d9::Release<ID3DXMesh*>(g_pObj[i]);
	}
	
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
		D3DXMATRIX Rotation = Rx*Ry;
		

		g_pDevice->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xffffffff, 1.0f, 0);
		g_pDevice->BeginScene();
		for (int i = 0; i < 5; i++)
		{
			D3DXMATRIX world = Rotation*g_Matrices[i];
			g_pDevice->SetTransform(D3DTS_WORLD, &world);
			g_pObj[i]->DrawSubset(0);
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
