#include "d3dUtility.h"
#include "SnowSys.h"
#include "fps.h"
#include "Camera.h"
#include<cstdlib>
#include<ctime>



IDirect3DDevice9* g_pDevice = NULL;
int Width = 640;
int Height = 480;
Camera::CameraType gType = Camera::AIRCRAFT;
Camera theCamera(Camera::AIRCRAFT);
PSystem* g_pSystem = NULL;
FPSCounter* g_pFPSCounter = NULL;


// 退出循环之后是清理工作
void CleanUp()
{
	d3d9::Delete(g_pSystem);
	d3d9::Delete(g_pFPSCounter);
	d3d9::DrawBasicScene(NULL, 1.0f);
}

// 进入循环之前的设置
bool SetUp()
{
	srand((unsigned int)time(0));
	g_pFPSCounter = new FPSCounter(g_pDevice);
	if (!g_pFPSCounter)
		return false;
	BoundingBox box;
	box._min = D3DXVECTOR3(-10.0f, -10.0f, -10.0f);
	box._max = D3DXVECTOR3(10.0f, 10.0f, 10.0f);
	g_pSystem = new Snow(&box, 5000);
	g_pSystem->Init(g_pDevice, "snowflake.dds");

	d3d9::DrawBasicScene(g_pDevice, 1.0f);

	// 竖直视角,宽/高比,近裁剪面,远裁剪面
	D3DXMATRIX proj;
	D3DXMatrixPerspectiveFovLH(&proj, D3DX_PI*0.25f, (float)Width / (float)Height, 1.0f, 5000.0f);
	g_pDevice->SetTransform(D3DTS_PROJECTION, &proj);

	//theCamera.SetPos(&D3DXVECTOR3(0.0f, 100.0f, 0.0f));

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
			theCamera.Walk(100.0f * time);

		if (::GetAsyncKeyState('S') & 0x8000f)
			theCamera.Walk(-100.0f * time);

		if (::GetAsyncKeyState('A') & 0x8000f)
			theCamera.Strafe(-100.0f * time);

		if (::GetAsyncKeyState('D') & 0x8000f)
			theCamera.Strafe(100.0f * time);

		if (::GetAsyncKeyState('R') & 0x8000f)
			theCamera.Fly(100.0f * time);

		if (::GetAsyncKeyState('F') & 0x8000f)
			theCamera.Fly(-100.0f * time);

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

		g_pSystem->Update(time);
		//
		// Render
		//

		g_pDevice->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x00000000, 1.0f, 0);
		g_pDevice->BeginScene();

		D3DXMATRIX I;
		D3DXMatrixIdentity(&I);
		g_pDevice->SetTransform(D3DTS_WORLD, &I);
		d3d9::DrawBasicScene(g_pDevice, 1.0f);

		g_pDevice->SetTransform(D3DTS_WORLD, &I);
		g_pSystem->Render();
		if (g_pFPSCounter)
			g_pFPSCounter->Render(0xff00ff00, time);
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