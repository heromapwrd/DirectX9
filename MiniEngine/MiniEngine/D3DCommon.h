#ifndef _D3DCOMMON_H__
#define _D3DCOMMON_H__
#include<d3d9.h>
#include<d3dx9.h>

#define D3DDELETE(p) {if(p) { delete (p); (p)=NULL;}}
#define D3DDELETEARRAY(p) {if(p) {delete [](p);(p)=NULL;}}
#define D3DRELEASE(p) {if(p) {(p)->Release();(p)=NULL;}}
#define GETTIMESTAMP timeGetTime


struct Vertex
{
	Vertex(){}
	Vertex(float _x, float _y, float _z, D3DCOLOR c)
	{
		x = _x;
		y = _y;
		z = _z;
		diffuse = c;
	}
	float x, y, z;
	D3DCOLOR diffuse;
	static const DWORD FVF = D3DFVF_XYZ | D3DFVF_DIFFUSE;
};

template<class T>
void Release(T t)
{
	if (t)
		t->Release();
	t = NULL;
}

template<class T>
void Delete(T t)
{
	if (t)
		delete t;
	t = NULL;
}

template<class T>
void DeleteArray(T t)
{
	if (t)
		delete[] t;
	t = NULL;
}

const D3DXCOLOR WHITE_COLOR = D3DCOLOR_ARGB(255, 255, 255, 255);
const D3DXCOLOR BLACK_COLOR = D3DCOLOR_ARGB(255, 0, 0, 0);
const D3DXCOLOR RED_COLOR = D3DCOLOR_ARGB(255, 255, 0, 0);
const D3DXCOLOR GREEN_COLOR = D3DCOLOR_ARGB(255, 0, 255, 0);
const D3DXCOLOR BLUE_COLOR = D3DCOLOR_ARGB(255, 0, 0, 255);
const D3DXCOLOR YELLOW_COLOR = D3DCOLOR_ARGB(255, 255, 255, 0);
const D3DXCOLOR CYAN_COLOR = D3DCOLOR_ARGB(255, 0, 255, 255);
const D3DXCOLOR MAGENTA_COLOR = D3DCOLOR_ARGB(255, 255, 0, 255);
const D3DXCOLOR BEACH_SAND = D3DCOLOR_ARGB(255, 255, 249, 157);
const D3DXCOLOR DESERT_SAND = D3DCOLOR_ARGB(255, 250, 205, 135);

const D3DXCOLOR LIGHTGREEN = D3DCOLOR_ARGB(255, 60, 184, 120);
const D3DXCOLOR PUREGREEN = D3DCOLOR_ARGB(255, 0, 166, 81);
const D3DXCOLOR DARKGREEN = D3DCOLOR_ARGB(255, 0, 114, 54);

const D3DXCOLOR LIGHT_YELLOW_GREEN = D3DCOLOR_ARGB(255, 124, 197, 118);
const D3DXCOLOR PURE_YELLOW_GREEN = D3DCOLOR_ARGB(255, 57, 181, 74);
const D3DXCOLOR DARK_YELLOW_GREEN = D3DCOLOR_ARGB(255, 25, 123, 48);

const D3DXCOLOR LIGHT_BROWN = D3DCOLOR_ARGB(255, 198, 156, 109);
const D3DXCOLOR DARK_BROWN = D3DCOLOR_ARGB(255, 115, 100, 87);



D3DMATERIAL9 InitMaterial(D3DXCOLOR a, D3DXCOLOR d, D3DXCOLOR s, D3DXCOLOR e, float p);
const D3DMATERIAL9 WHITE_MTRL = InitMaterial(WHITE_COLOR, WHITE_COLOR, WHITE_COLOR, BLACK_COLOR, 2.0f);
const D3DMATERIAL9 RED_MTRL = InitMaterial(RED_COLOR, RED_COLOR, RED_COLOR, BLACK_COLOR, 2.0f);
const D3DMATERIAL9 GREEN_MTRL = InitMaterial(GREEN_COLOR, GREEN_COLOR, GREEN_COLOR, BLACK_COLOR, 2.0f);
const D3DMATERIAL9 BLUE_MTRL = InitMaterial(BLUE_COLOR, BLUE_COLOR, BLUE_COLOR, BLACK_COLOR, 2.0f);
const D3DMATERIAL9 YELLOW_MTRL = InitMaterial(YELLOW_COLOR, YELLOW_COLOR, YELLOW_COLOR, BLACK_COLOR, 2.0f);
const D3DMATERIAL9 CYAN_MTRL = InitMaterial(CYAN_COLOR, CYAN_COLOR, CYAN_COLOR, BLACK_COLOR, 2.0f);
const D3DMATERIAL9 MAGENTA_MTRL = InitMaterial(MAGENTA_COLOR, MAGENTA_COLOR, MAGENTA_COLOR, BLACK_COLOR, 2.0f);

D3DLIGHT9 InitDirLight(const D3DXVECTOR3* dir, const D3DXCOLOR* color);
D3DLIGHT9 InitPointLight(const D3DXVECTOR3* pos, const D3DXCOLOR* color);
D3DLIGHT9 InitSpotLight(const D3DXVECTOR3* pos, const D3DXVECTOR3* dir, const D3DXCOLOR* color);

//
// Constants
//

const float D3DINFINITY = FLT_MAX;
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

float Lerp(float a, float b, float t);

HRESULT InitD3D(HWND hWnd, int Width, int Height, bool bWindowed, bool bMultiThread, D3DDEVTYPE type, IDirect3DDevice9** ppDevice);

#endif //_D3DCOOMMON_H__