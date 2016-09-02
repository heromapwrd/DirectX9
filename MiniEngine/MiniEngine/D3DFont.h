#ifndef _D3DFONT_H__
#define _D3DFONT_H__

#include<tchar.h>
#include "D3DCommon.h"
#include "D3DBitmap.h"

#define D3DFONT_BOLD	0x0001
#define D3DFONT_ITALIC	0x0002
#define D3DFONT_ZENABLE 0x0004

#define D3DFONT_TWOSIDES 0x0001

#define D3DTEXT_CENTER_X	0x0001
#define D3DTEXT_LEFT		0x0002
#define D3DTEXT_RIGHT		0x0004
#define D3DTEXT_LEFT_ONE	0x0008
#define D3DTEXT_RIGHT_ONE	0x0010

#define D3DTEXT_CENTER_Y	0x0020
#define D3DTEXT_TOP			0x0040
#define D3DTEXT_BOTTOM		0x0080
#define D3DTEXT_TOP_ONE		0x0100
#define D3DTEXT_BOTTOM_ONE	0x0200

#define D3DTEXT_HORIZONTAL	0x0001
#define D3DTEXT_VERTICAL	0x0002

#define D3DTEXT_HCENTER		0x0001
#define D3DTEXT_HLEFT		0x0002
#define D3DTEXT_HRIGHT		0x0004

#define D3DTEXT_VCENTER		0x0008
#define D3DTEXT_VTOP		0x0010
#define D3DTEXT_VBOTTOM		0x0020

class D3DFont
{
	TCHAR m_strFontName[80];
	DWORD m_dwFontHeight;
	DWORD m_dwFontFlags;

	LPDIRECT3DDEVICE9 m_pDevice;
	LPDIRECT3DTEXTURE9 m_pTexture;
	LPDIRECT3DVERTEXBUFFER9 m_pVB;

	DWORD m_dwTexWidth;
	DWORD m_dwTexHeight;
	DWORD m_dwSpacing;
	int m_iFontHeight;
	int m_iFontWidth;
	float m_fTexCord[128 - 32][4];

	LPDIRECT3DSTATEBLOCK9 m_pStateSaved;
	LPDIRECT3DSTATEBLOCK9 m_pDrawText;

public:
	D3DFont(TCHAR* strFontName, DWORD dwFontHeight, DWORD dwFlags);
	~D3DFont();

	HRESULT Init(LPDIRECT3DDEVICE9 pDevice);
	void GetTextExtent(TCHAR* str, SIZE& size, DWORD dwTextType = D3DTEXT_HORIZONTAL);
	HRESULT DrawText(int sx, int sy, TCHAR* str, D3DCOLOR color, DWORD dwTextType = D3DTEXT_HORIZONTAL, DWORD dwLeanFlags = D3DTEXT_TOP_ONE, DWORD dwAlignFlags = D3DTEXT_HLEFT);
	HRESULT DrawText3D(
		TCHAR* str, 
		D3DXVECTOR3 pos, 
		D3DCOLOR color,
		DWORD dwAlignFlags = D3DTEXT_HCENTER, 
		DWORD dwTowSides = D3DFONT_TWOSIDES,
		D3DXVECTOR3 n = D3DXVECTOR3(0.0f, 0.0f, -1.0f),
		D3DXVECTOR3 u = D3DXVECTOR3(1.0f, 0.0f, 0.0f),
		D3DXVECTOR3 v = D3DXVECTOR3(0.0f, -1.0f, 0.0f));
private:
	void RestoreDeviceState();
	void ReleaseDeviceObject();
	void DeleteDeviceObject();
	void GetTextExtentH(TCHAR* str, SIZE& size);
	void GetTextExtentV(TCHAR* str, SIZE& size);
	HRESULT DrawTextH(int sx, int sy, TCHAR* str, D3DCOLOR color, DWORD dwLeanFlags, DWORD dwAlignFlags);
	HRESULT DrawTextV(int sx, int sy, TCHAR* str, D3DCOLOR color, DWORD dwLeanFlags, DWORD dwAlignFlags);
	HRESULT DrawTextLineH(int sx, int sy, TCHAR* str, D3DCOLOR color);
	HRESULT DrawTextLineV(int sx, int sy, TCHAR* str, D3DCOLOR color);
	HRESULT DrawText3DH(
		TCHAR* str,
		D3DXVECTOR3 pos, 
		D3DCOLOR color,
		D3DXVECTOR3 n, 
		D3DXVECTOR3 u, 
		D3DXVECTOR3 v);

};

#endif // _D3DFONT_H__