/*
--------------------------------------------------------------
����������
--------------------------------------------------------------
*/

#include "d3dUtility.h"
IDirect3DDevice9 * g_pDevice = 0;
const int Width = 640;
const int Height = 480;
// �����������������ָ��
ID3DXMesh* g_pObj[5] = { NULL, NULL, NULL, NULL, NULL };
// λ�ƾ���
D3DXMATRIX g_Matrices[5];
// ����
D3DMATERIAL9 g_Materials[5];


// ����ѭ��֮ǰ������
bool SetUp()
{
	// ��������
	D3DXCreateTeapot(g_pDevice, &g_pObj[0], 0);
	D3DXCreateBox(g_pDevice, 2.0f, 2.0f, 2.0f, &g_pObj[1], 0);
	D3DXCreateCylinder(g_pDevice, 3.0f, 1.0f, 4.0f, 10, 10, &g_pObj[2], 0);
	D3DXCreateTorus(g_pDevice, 1.0f, 3.0f, 10, 10, &g_pObj[3], 0);
	D3DXCreateSphere(g_pDevice, 3.0f, 10, 10, &g_pObj[4], 0);

	// ��ͬ�����λ�ƾ���
	D3DXMatrixTranslation(&g_Matrices[0], 0.0f, 0.0f, -10.0f);
	D3DXMatrixTranslation(&g_Matrices[1], 0.0f, 10.0f, 0.0f);
	D3DXMatrixTranslation(&g_Matrices[2], 0.0f, -10.0f, 0.0f);
	D3DXMatrixTranslation(&g_Matrices[3], 10.0f, 0.0f, 0.0f);
	D3DXMatrixTranslation(&g_Matrices[4], -10.0f, 0.0f, 0.0f);

	// ��ͬ����Ĳ���
	g_Materials[0] = d3d9::RED_MTRL;
	g_Materials[1] = d3d9::GREEN_MTRL;
	g_Materials[2] = d3d9::BLUE_MTRL;
	g_Materials[3] = d3d9::YELLO_MTRL;
	g_Materials[4] = d3d9::CYAN_MTRL;



	// ���ù�����Ч
	g_pDevice->SetRenderState(D3DRS_LIGHTING, true);
	// �����
	D3DLIGHT9 light = d3d9::InitDirLight(&D3DXVECTOR3(1.0f, -1.0f, -1.0f), &d3d9::WHITE_COLOR);
	// ���Դ
	//D3DLIGHT9 light = d3d9::InitPointLight(&D3DXVECTOR3(-30.0f, 30.0f, 10.0f), &d3d9::WHITE_COLOR);
	// �۹��
	//D3DLIGHT9 light = d3d9::InitSpotLight(&D3DXVECTOR3(-30.0f, 30.0f, 10.0f), &D3DXVECTOR3(1.0f, -1.0f, -1.0f), &d3d9::WHITE_COLOR);

	g_pDevice->SetLight(0, &light);
	g_pDevice->LightEnable(0, true);
	g_pDevice->SetRenderState(D3DRS_NORMALIZENORMALS, true);
	g_pDevice->SetRenderState(D3DRS_SPECULARENABLE, true);

	// ��ʼ���Ӿ�����,ͶӰ����
	// ���λ��,�ӵ�λ��,��ֱ��������
	D3DXVECTOR3 eye(0.0f, 0.0f, -20.0f);
	D3DXVECTOR3 lookat(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 Up(0.0f, 1.0f, 0.0f);
	D3DXMATRIX view;
	D3DXMatrixLookAtLH(&view, &eye, &lookat, &Up);
	g_pDevice->SetTransform(D3DTS_VIEW, &view);

	// ��ֱ�ӽ�,��/�߱�,���ü���,Զ�ü���
	D3DXMATRIX proj;
	D3DXMatrixPerspectiveFovLH(&proj, D3DX_PI*0.5f, (float)Width / (float)Height, 1.0f, 1000.0f);
	g_pDevice->SetTransform(D3DTS_PROJECTION, &proj);

	return true;
}

// �˳�ѭ��֮����������
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


		g_pDevice->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x00000000, 1.0f, 0);
		g_pDevice->BeginScene();
		for (int i = 0; i < 5; i++)
		{
			g_pDevice->SetMaterial(&g_Materials[i]);
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
