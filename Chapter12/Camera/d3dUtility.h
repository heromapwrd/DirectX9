/*
-----------------------------------------------------
		d3dUtility.h 完成程序创建,消息循环工作
-----------------------------------------------------
*/

#ifndef _D3DUTILITY_H__
#define _D3DUTILITY_H__
#include<d3dx9.h>
#include<string>
#include<limits.h>

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
	const D3DXCOLOR WHITE_COLOR	  = D3DCOLOR_ARGB(255, 255, 255, 255);
	const D3DXCOLOR BLACK_COLOR	  = D3DCOLOR_ARGB(255, 0, 0, 0);
	const D3DXCOLOR RED_COLOR	  = D3DCOLOR_ARGB(255, 255, 0, 0);
	const D3DXCOLOR GREEN_COLOR	  = D3DCOLOR_ARGB(255, 0, 255, 0);
	const D3DXCOLOR BLUE_COLOR	  = D3DCOLOR_ARGB(255, 0, 0, 255);
	const D3DXCOLOR YELLOW_COLOR  = D3DCOLOR_ARGB(255, 255, 255, 0);
	const D3DXCOLOR CYAN_COLOR	  = D3DCOLOR_ARGB(255, 0, 255, 255);
	const D3DXCOLOR MAGENTA_COLOR = D3DCOLOR_ARGB(255, 255, 0, 255);

	D3DMATERIAL9 InitMaterial(D3DXCOLOR a, D3DXCOLOR d, D3DXCOLOR s, D3DXCOLOR e, float p);
	const D3DMATERIAL9 WHITE_MTRL	= InitMaterial(WHITE_COLOR, WHITE_COLOR, WHITE_COLOR, BLACK_COLOR, 2.0f);
	const D3DMATERIAL9 RED_MTRL		= InitMaterial(RED_COLOR, RED_COLOR, RED_COLOR, BLACK_COLOR, 2.0f);
	const D3DMATERIAL9 GREEN_MTRL	= InitMaterial(GREEN_COLOR, GREEN_COLOR, GREEN_COLOR, BLACK_COLOR, 2.0f);
	const D3DMATERIAL9 BLUE_MTRL	= InitMaterial(BLUE_COLOR, BLUE_COLOR, BLUE_COLOR, BLACK_COLOR, 2.0f);
	const D3DMATERIAL9 YELLOW_MTRL	= InitMaterial(YELLOW_COLOR, YELLOW_COLOR, YELLOW_COLOR, BLACK_COLOR, 2.0f);
	const D3DMATERIAL9 CYAN_MTRL	= InitMaterial(CYAN_COLOR, CYAN_COLOR, CYAN_COLOR, BLACK_COLOR, 2.0f);
	const D3DMATERIAL9 MAGENTA_MTRL = InitMaterial(MAGENTA_COLOR, MAGENTA_COLOR, MAGENTA_COLOR, BLACK_COLOR, 2.0f);

	D3DLIGHT9 InitDirLight(const D3DXVECTOR3* dir, const D3DXCOLOR* color);
	D3DLIGHT9 InitPointLight(const D3DXVECTOR3* pos, const D3DXCOLOR* color);
	D3DLIGHT9 InitSpotLight(const D3DXVECTOR3* pos, const D3DXVECTOR3* dir, const D3DXCOLOR* color);

	//
	// Constants
	//

	const float _INFINITY = FLT_MAX;
	const float EPSILON = 0.001f;

	//
	// Randomness
	//

	// Desc: Return random float in [lowBound, highBound] interval.
	float GetRandomFloat(float lowBound, float highBound);


	// Desc: Returns a random vector in the bounds specified by min and max.
	void GetRandomVector(
		D3DXVECTOR3* out,
		D3DXVECTOR3* min,
		D3DXVECTOR3* max);

	//
	// Conversion
	//
	DWORD FtoDw(float f);
}




#endif // !_D3DUTILITY_H__
