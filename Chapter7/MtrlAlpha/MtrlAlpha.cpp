/*
--------------------------------------------------------------
����������
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


// ����ѭ��֮ǰ������
bool SetUp()
{
	// ����������񻺴�
	D3DXCreateTeapot(g_pDevice, &pTeaPot, 0);

	// �����������㻺��
	g_pDevice->CreateVertexBuffer(
		6 * sizeof(Vertex),
		D3DUSAGE_WRITEONLY,
		Vertex::FVF,
		D3DPOOL_MANAGED,
		&VB,
		0
		);

	// ��ʼ���������㻺��
	Vertex* pVertex;
	VB->Lock(0, 0, (void**)&pVertex, 0);
	pVertex[0] = Vertex(-10.0f, -10.0f, 5.0f, 0.0f,0.0f,-1.0f,0.0f, 1.0f);
	pVertex[1] = Vertex(-10.0f, 10.0f, 5.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f);
	pVertex[2] = Vertex(10.0f, 10.0f, 5.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);

	pVertex[3] = Vertex(-10.0f, -10.0f, 5.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
	pVertex[4] = Vertex(10.0f, 10.0f, 5.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);
	pVertex[5] = Vertex(10.0f, -10.0f, 5.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f);
	VB->Unlock();

	// ��ʼ����������
	BGMtrl = d3d9::WHITE_MTRL;
	// ��ʼ���������
	TeaPotMtrl = d3d9::RED_MTRL;
	TeaPotMtrl.Diffuse.a = 0.5f;
	// ��ʼ����������
	D3DXCreateTextureFromFile(g_pDevice, "crate.jpg", &TX);
	// �����������
	g_pDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	g_pDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	g_pDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

	// �����ں�ģʽ(�˴�����Ϊ͸��ģʽ)
	g_pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	g_pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	// ����͸��ģʽ��Alphaͨ����Դ
	g_pDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE);
	g_pDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);

	// ������Դ
	g_pDevice->SetRenderState(D3DRS_LIGHTING, true);
	D3DLIGHT9 light;
	memset(&light, 0, sizeof(light));
	light.Type = D3DLIGHT_DIRECTIONAL;
	light.Ambient = d3d9::WHITE_COLOR*0.6f;
	light.Specular = d3d9::WHITE_COLOR*0.2f;
	light.Diffuse = d3d9::WHITE_COLOR;
	light.Direction = D3DXVECTOR3(0.707f, 0.0f, 0.707f);
	// ע���Դ
	g_pDevice->SetLight(0, &light);
	g_pDevice->LightEnable(0, true);
	g_pDevice->SetRenderState(D3DRS_LIGHTING, true);
	g_pDevice->SetRenderState(D3DRS_NORMALIZENORMALS, true);
	g_pDevice->SetRenderState(D3DRS_SPECULARENABLE, true);

	// ��ʼ���Ӿ�����,ͶӰ����
	// ���λ��,�ӵ�λ��,��ֱ��������
	D3DXVECTOR3 eye(0.0f, 0.0f, -5.0f);
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

		// ����ͼ��
		g_pDevice->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x00000000, 1.0f, 0);
		g_pDevice->BeginScene();

		// ���Ʊ���
		g_pDevice->SetTransform(D3DTS_WORLD, &IW);      // ���ñ����������
		g_pDevice->SetStreamSource(0, VB, 0, sizeof(Vertex));
		g_pDevice->SetFVF(Vertex::FVF);
		g_pDevice->SetMaterial(&BGMtrl);				// ע�����
		g_pDevice->SetTexture(0, TX);					// ע������
		g_pDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 2);

		// ���Ʋ��
		g_pDevice->SetTransform(D3DTS_WORLD, &world);      // ���ñ����������
		g_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, true);   // ��Alpha�ں�����
		g_pDevice->SetMaterial(&TeaPotMtrl);				// ע�����
		g_pDevice->SetTexture(0, 0);					// ע������
		pTeaPot->DrawSubset(0);
		g_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, false);  // �ر�Alpha�ں����� 

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
