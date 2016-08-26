#ifndef _D3DAPP_H__
#define _D3DAPP_H__
#include <d3d9.h>

HRESULT LoadContent();
void UnLoadContent();
bool Tick(DWORD deltatimes);
bool Render(DWORD deltatimes);
HRESULT SetUp();
void CleanUp();
int EnterMsgLoop();

HRESULT InitInstance(
	HINSTANCE hInstance, 
	int Width, 
	int Height, 
	bool bWindowed, 
	bool bMultiThread, 
	D3DDEVTYPE type, 
	IDirect3DDevice9** ppDevice
	);

LRESULT CALLBACK WndProc(
	HWND hWnd,
	UINT msg,
	WPARAM wParam,
	LPARAM lParam
	);

#endif // _D3DAPP_H__