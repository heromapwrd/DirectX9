#include "d3dUtility.h"
#include "Bound.h"
#include "Vertex.h"
#include "Camera.h"
#include<fstream>
#include<vector>
using namespace std;
using std::vector;
using std::ofstream;

IDirect3DDevice9* g_pDevice = NULL;
int Width = 640;
int Height = 480;
Camera::CameraType gType = Camera::AIRCRAFT;

Camera theCamera(Camera::AIRCRAFT);

bool DrawBasicScene(IDirect3DDevice9* device, float scale);

// 退出循环之后是清理工作
void CleanUp()
{
	DrawBasicScene(NULL, 0.0f);
}

// 进入循环之前的设置
bool SetUp()
{
	DrawBasicScene(g_pDevice, 0.0f);

	// 竖直视角,宽/高比,近裁剪面,远裁剪面
	D3DXMATRIX proj;
	D3DXMatrixPerspectiveFovLH(&proj, D3DX_PI*0.5f, (float)Width / (float)Height, 1.0f, 1000.0f);
	g_pDevice->SetTransform(D3DTS_PROJECTION, &proj);

	return true;
}

bool DisPlay(DWORD timeDelta)
{
	if (g_pDevice)
	{
		//
		// Update: Update the camera.
		//
		float time = float(timeDelta) / 1000.0f;
		
		if (::GetAsyncKeyState('W') & 0x8000f)
			theCamera.Walk(4.0f * time);

		if (::GetAsyncKeyState('S') & 0x8000f)
			theCamera.Walk(-4.0f * time);

		if (::GetAsyncKeyState('A') & 0x8000f)
			theCamera.Strafe(-4.0f * time);

		if (::GetAsyncKeyState('D') & 0x8000f)
			theCamera.Strafe(4.0f * time);

		if (::GetAsyncKeyState('R') & 0x8000f)
			theCamera.Fly(4.0f * time);

		if (::GetAsyncKeyState('F') & 0x8000f)
			theCamera.Fly(-4.0f * time);

		if (::GetAsyncKeyState(VK_UP) & 0x8000f)
			theCamera.Pitch(1.0f * time);

		if (::GetAsyncKeyState(VK_DOWN) & 0x8000f)
			theCamera.Pitch(-1.0f * time);

		if (::GetAsyncKeyState(VK_LEFT) & 0x8000f)
			theCamera.Yaw(-1.0f * time);

		if (::GetAsyncKeyState(VK_RIGHT) & 0x8000f)
			theCamera.Yaw(1.0f * time);

		if (::GetAsyncKeyState('N') & 0x8000f)
			theCamera.Roll(1.0f * time);

		if (::GetAsyncKeyState('M') & 0x8000f)
			theCamera.Roll(-1.0f * time);

		// Update the view matrix representing the cameras 
		// new position/orientation.
		D3DXMATRIX V;
		theCamera.GetViewMatrix(&V);
		g_pDevice->SetTransform(D3DTS_VIEW, &V);

		//
		// Render
		//

		g_pDevice->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x00000000, 1.0f, 0);
		g_pDevice->BeginScene();

		DrawBasicScene(g_pDevice, 1.0f);

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
		if (wParam == VK_SPACE)
		{
			if (gType == Camera::AIRCRAFT)
			{
				gType = Camera::LANDOBJECT;
			}
			else
			{
				gType = Camera::AIRCRAFT;
			}
			theCamera.SetCamereType(gType);
		}
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


bool DrawBasicScene(IDirect3DDevice9* device, float scale)
{
	static IDirect3DVertexBuffer9* floor = 0;
	static IDirect3DTexture9*      tex = 0;
	static ID3DXMesh*              pillar = 0;

	HRESULT hr = 0;

	if (device == 0)
	{
		if (floor && tex && pillar)
		{
			// they already exist, destroy them
			d3d9::Release<IDirect3DVertexBuffer9*>(floor);
			d3d9::Release<IDirect3DTexture9*>(tex);
			d3d9::Release<ID3DXMesh*>(pillar);
		}
	}
	else if (!floor && !tex && !pillar)
	{
		// they don't exist, create them
		device->CreateVertexBuffer(
			6 * sizeof(Vertex),
			0,
			Vertex::FVF,
			D3DPOOL_MANAGED,
			&floor,
			0);

		Vertex* v = 0;
		floor->Lock(0, 0, (void**)&v, 0);

		v[0] = Vertex(-20.0f, -2.5f, -20.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f);
		v[1] = Vertex(-20.0f, -2.5f, 20.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f);
		v[2] = Vertex(20.0f, -2.5f, 20.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f);

		v[3] = Vertex(-20.0f, -2.5f, -20.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f);
		v[4] = Vertex(20.0f, -2.5f, 20.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f);
		v[5] = Vertex(20.0f, -2.5f, -20.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f);

		floor->Unlock();

		D3DXCreateCylinder(device, 0.5f, 0.5f, 5.0f, 20, 20, &pillar, 0);

		D3DXCreateTextureFromFile(
			device,
			"desert.bmp",
			&tex);
	}
	else
	{
		//
		// Pre-Render Setup
		//
		device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
		device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
		device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_POINT);

		D3DXVECTOR3 dir(0.707f, -0.707f, 0.707f);
		D3DXCOLOR col(1.0f, 1.0f, 1.0f, 1.0f);
		D3DLIGHT9 light = d3d9::InitDirLight(&dir, &col);

		device->SetLight(0, &light);
		device->LightEnable(0, true);
		device->SetRenderState(D3DRS_NORMALIZENORMALS, true);
		device->SetRenderState(D3DRS_SPECULARENABLE, true);

		//
		// Render
		//

		D3DXMATRIX T, R, P, S;

		D3DXMatrixScaling(&S, scale, scale, scale);

		// used to rotate cylinders to be parallel with world's y-axis
		D3DXMatrixRotationX(&R, -D3DX_PI * 0.5f);

		// draw floor
		D3DXMatrixIdentity(&T);
		T = T * S;
		device->SetTransform(D3DTS_WORLD, &T);
		device->SetMaterial(&d3d9::WHITE_MTRL);
		device->SetTexture(0, tex);
		device->SetStreamSource(0, floor, 0, sizeof(Vertex));
		device->SetFVF(Vertex::FVF);
		device->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 2);

		// draw pillars
		device->SetMaterial(&d3d9::BLUE_MTRL);
		device->SetTexture(0, 0);
		for (int i = 0; i < 5; i++)
		{
			D3DXMatrixTranslation(&T, -5.0f, 0.0f, -15.0f + (i * 7.5f));
			P = R * T * S;
			device->SetTransform(D3DTS_WORLD, &P);
			pillar->DrawSubset(0);

			D3DXMatrixTranslation(&T, 5.0f, 0.0f, -15.0f + (i * 7.5f));
			P = R * T * S;
			device->SetTransform(D3DTS_WORLD, &P);
			pillar->DrawSubset(0);
		}
	}
	return true;
}
