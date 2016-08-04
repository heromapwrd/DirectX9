#include "d3dUtility.h"
#include "Ray.h"

IDirect3DDevice9* g_pDevice = NULL;
int Width = 640;
int Height = 480;
ID3DXMesh* g_pTeapot;
ID3DXMesh* g_pSephere;

D3DXMATRIX g_World;
BoundingSphere g_BSephere;



// 退出循环之后是清理工作
void CleanUp()
{
	d3d9::Release(g_pTeapot);
	d3d9::Release(g_pSephere);
}

// 进入循环之前的设置
bool SetUp()
{
	HRESULT hr = 0;
	hr = D3DXCreateTeapot(g_pDevice, &g_pTeapot,NULL);
	if (FAILED(hr))
	{
		MessageBox(0, "Create teapot failed!", 0, 0);
		return false;
	}

	BYTE* v = NULL;
	g_pTeapot->LockVertexBuffer(0, (void**)&v);
	hr = D3DXComputeBoundingSphere(
		(D3DXVECTOR3*)v,
		g_pTeapot->GetNumVertices(),
		D3DXGetFVFVertexSize(g_pTeapot->GetFVF()),
		&g_BSephere._center,
		&g_BSephere._radius
		);

	g_pTeapot->UnlockVertexBuffer();

	if (FAILED(hr))
	{
		MessageBox(0, "Compute Sephere failed!", 0, 0);
		return false;
	}
	hr = D3DXCreateSphere(g_pDevice, g_BSephere._radius, 20, 20, &g_pSephere, NULL);
	if (FAILED(hr))
	{
		MessageBox(0, "Create Sephere failed!", 0, 0);
		return false;
	}

	D3DXVECTOR3 dir(0.707f, -0.0f, 0.707f);
	D3DLIGHT9 light = d3d9::InitDirLight(&dir, &d3d9::WHITE_COLOR);

	g_pDevice->SetLight(0, &light);
	g_pDevice->LightEnable(0, true); 
	g_pDevice->SetRenderState(D3DRS_NORMALIZENORMALS, true);
	g_pDevice->SetRenderState(D3DRS_SPECULARENABLE, false);

	D3DXVECTOR3 pos(0.0f, 0.0f, -10.0f);
	D3DXVECTOR3 target(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);

	D3DXMATRIX view;
	D3DXMatrixLookAtLH(&view, &pos, &target, &up);
	g_pDevice->SetTransform(D3DTS_VIEW, &view);

	// 竖直视角,宽/高比,近裁剪面,远裁剪面
	D3DXMATRIX proj;
	D3DXMatrixPerspectiveFovLH(&proj, D3DX_PI*0.25f, (float)Width / (float)Height, 1.0f, 1000.0f);
	g_pDevice->SetTransform(D3DTS_PROJECTION, &proj);

	return true;
}

bool DisPlay(DWORD timeDelta)
{
	if (g_pDevice)
	{
		static float r = 0.0f;
		static float v = 1.0f;
		static float angle = 0.0f;
		D3DXMatrixTranslation(&g_World, cosf(angle)*r, sinf(angle)*r, 10.0f);

		g_BSephere._center = D3DXVECTOR3(cosf(angle)*r, sinf(angle)*r, 10.0f);

		float time = (float)timeDelta / 1000.0f;
		r += v*time;
		if (r >= 8.0f || r <= 0.0f)
			v = -v;
		angle += 1.0f*D3DX_PI*time;
		if (angle >= D3DX_PI*2.0f)
			angle = 0.0f;
		// Render

		g_pDevice->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x00000000, 1.0f, 0);
		g_pDevice->BeginScene();

		g_pDevice->SetTransform(D3DTS_WORLD, &g_World);
		g_pDevice->SetMaterial(&d3d9::BLUE_MTRL);
		g_pTeapot->DrawSubset(0);

		g_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
		g_pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		g_pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

		D3DMATERIAL9 blue = d3d9::BLUE_MTRL;
		blue.Diffuse.a = 0.25f;
		g_pDevice->SetMaterial(&blue);
		g_pSephere->DrawSubset(0);

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
	case WM_LBUTTONDOWN:
	{
		Ray ray;
		if (CalcPickingRay(LOWORD(lParam), HIWORD(lParam), g_pDevice, &ray))
		{
			D3DXMATRIX view;
			g_pDevice->GetTransform(D3DTS_VIEW, &view);
			D3DXMATRIX inverse;
			float determinant = D3DXMatrixDeterminant(&view);
			D3DXMatrixInverse(&inverse, &determinant, &view);
			TransformRay(&ray, &inverse);

			if (RayHitSephere(&ray, &g_BSephere))
			{
				MessageBox(0, "Hit!", "HIT", 0);
			}
		}
		break;
	}
		
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