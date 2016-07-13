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
}




#endif // !_D3DUTILITY_H__
