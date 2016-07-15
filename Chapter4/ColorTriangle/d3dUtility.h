/*
-----------------------------------------------------
		d3dUtility.h 完成程序创建,消息循环工作
-----------------------------------------------------
*/

#ifndef _D3DUTILITY_H__
#define _D3DUTILITY_H__
#include<d3dx9.h>
#include<string>

namespace d3d9 {

	bool Init(
		HINSTANCE hInstance,
		int width,int height,
		bool bWindowed,
		D3DDEVTYPE eDeviceType,
		IDirect3DDevice9 ** device
		);

	int EnterMsgLoop(
		bool(*ptr_display)(DWORD timeDelta));

	LRESULT CALLBACK WndProc(
		HWND hWnd,
		UINT msg,
		WPARAM wParam,
		LPARAM lParam);

	template<class T>
	void Release(T t)
	{
		if (t)
		{
			t->Release();
			t = 0;
		}

	}

	template<class T>
	void Delete(T t)
	{
		if (t)
		{
			delete t;
			t = 0;
		}
	}
	const D3DXCOLOR WHITE_COLOR	 = D3DCOLOR_ARGB(255, 255, 255, 255);
	const D3DXCOLOR BLACK_COLOR	 = D3DCOLOR_ARGB(255, 0, 0, 0);
	const D3DXCOLOR RED_COLOR	 = D3DCOLOR_ARGB(255, 255, 0, 0);
	const D3DXCOLOR GREEN_COLOR	 = D3DCOLOR_ARGB(255, 0, 255, 0);
	const D3DXCOLOR BLUE_COLOR	 = D3DCOLOR_ARGB(255, 0, 0, 255);
	const D3DXCOLOR YELLOW_COLOR = D3DCOLOR_ARGB(255, 255, 255, 0);
	const D3DXCOLOR CYAN_COLOR	 = D3DCOLOR_ARGB(255, 0, 255, 255);
	const D3DXCOLOR MAGENTA		 = D3DCOLOR_ARGB(255, 255, 0, 255);
}




#endif // !_D3DUTILITY_H__
