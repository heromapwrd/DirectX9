#include "D3DFont.h"

struct Font2DVertex
{
	D3DXVECTOR4 pos;
	D3DCOLOR diff;
	float tu, tv;

	Font2DVertex(D3DXVECTOR4 p, D3DCOLOR color, float u, float v)
	{
		pos = p;
		diff = color;
		tu = u;
		tv = v;
	}
};

struct Font3DVertex
{
	D3DXVECTOR3 pos;
	D3DXVECTOR3 normal;
	D3DCOLOR color;
	float tu, tv;

	Font3DVertex(D3DXVECTOR3 p, D3DXVECTOR3 n, D3DCOLOR c ,float u, float v)
	{
		pos = p;
		normal = n;
		color = c;
		tu = u;
		tv = v;
	}
};

#define D3DFVF_FONT2DVERTEX (D3DFVF_XYZRHW|D3DFVF_DIFFUSE|D3DFVF_TEX1)
#define D3DFVF_FONT3DVERTEX (D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_DIFFUSE|D3DFVF_TEX1)

#define MAXVERTEX 180
#define SCALE3D	0.1f

D3DFont::D3DFont(TCHAR* strFontName, DWORD dwFontHeight, DWORD dwFlags)
{
	_tcsncpy(m_strFontName, strFontName, sizeof(m_strFontName) / sizeof(TCHAR));
	m_strFontName[sizeof(m_strFontName) / sizeof(TCHAR)-1] = '\0';
	m_dwFontHeight	= dwFontHeight;
	m_dwFontFlags	= dwFlags;
	m_dwSpacing		= 0;
	m_dwTexWidth	= 0;
	m_dwTexHeight	= 0;
	m_iFontHeight	= 0;
	m_iFontWidth	= 0;

	m_pDevice		= NULL;
	m_pTexture		= NULL;
	m_pVB			= NULL;

	m_pStateSaved	= NULL;
	m_pDrawText		= NULL;
}

D3DFont::~D3DFont()
{
	ReleaseDeviceObject();
	DeleteDeviceObject();
	m_pDevice = NULL;
}

HRESULT D3DFont::Init(LPDIRECT3DDEVICE9 pDevice)
{
	if (!pDevice)
		return E_FAIL;

	m_pDevice = pDevice;
	if (m_dwFontHeight > 60)
	{
		m_dwTexWidth = m_dwTexHeight = 2048;
	}
	else if (m_dwFontHeight > 30)
	{
		m_dwTexWidth = m_dwTexHeight = 1024;
	}
	else if (m_dwFontHeight > 15)
	{
		m_dwTexWidth = m_dwTexHeight = 512;
	}
	else
	{
		m_dwTexWidth = m_dwTexHeight = 256;
	}

	LRESULT hr = 0;
	hr = m_pDevice->CreateTexture(
		m_dwTexWidth, 
		m_dwTexHeight, 
		0, 
		0, 
		D3DFMT_A4R4G4B4,
		D3DPOOL_MANAGED, 
		&m_pTexture, 
		NULL
		);
	if (FAILED(hr))
	{
		MessageBox(0, "Failed to create texture!", 0, 0);
		return E_FAIL;
	}

	DWORD* pBitmapBits = NULL;
	BITMAPINFO biminfo;
	memset(&biminfo.bmiHeader,0, sizeof(BITMAPINFOHEADER));
	biminfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	biminfo.bmiHeader.biWidth = (int)m_dwTexWidth;
	biminfo.bmiHeader.biHeight = -(int)m_dwTexHeight;
	biminfo.bmiHeader.biPlanes = 1;
	biminfo.bmiHeader.biBitCount = 32;
	biminfo.bmiHeader.biCompression = BI_RGB;
	biminfo.bmiHeader.biClrUsed = 0;
	biminfo.bmiHeader.biClrImportant = 0;
	biminfo.bmiHeader.biSizeImage = 0;
	biminfo.bmiHeader.biXPelsPerMeter = 0;
	biminfo.bmiHeader.biYPelsPerMeter = 0;

	HDC hDC = CreateCompatibleDC(NULL);
	HBITMAP hBitmap = CreateDIBSection(
		hDC,
		&biminfo, 
		DIB_RGB_COLORS, 
		(void**)&pBitmapBits, 
		NULL, 
		0);
	SetMapMode(hDC, MM_TEXT);

	int nFontHeight = -MulDiv(m_dwFontHeight, GetDeviceCaps(hDC, LOGPIXELSX), 72);
	DWORD dwBold = m_dwFontFlags&D3DFONT_BOLD ? FW_BOLD : FW_NORMAL;
	DWORD dwItalic = m_dwFontFlags&D3DFONT_ITALIC ? true : false;
	HFONT hFont = CreateFont(nFontHeight, 0, 0, 0, dwBold, dwItalic,
		false, false, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY,
		VARIABLE_PITCH, m_strFontName);
	if (!hFont)
	{
		MessageBox(0, "Failed to create new font!", 0, 0);
		return E_FAIL;
	}

	HGDIOBJ hBmpOld = SelectObject(hDC, hBitmap);
	HGDIOBJ hFontOld = SelectObject(hDC, hFont);

	SetTextColor(hDC, RGB(255, 255, 255));
	SetBkColor(hDC, RGB(0,0,0));
	SetTextAlign(hDC, TA_TOP);

	TCHAR str[2] = " ";
	SIZE size;
	GetTextExtentPoint32(hDC, str, 1, &size);
	m_iFontHeight = size.cy;
	m_iFontWidth = size.cx;
	m_dwSpacing = (DWORD)ceil(0.3f*m_iFontHeight);
	int x, y;
	x = m_dwSpacing;
	y = 0;
	for (int i = 32; i < 128; i++)
	{
		str[0] = i;
		GetTextExtentPoint32(hDC, str, 1, &size);
		if (x + size.cx + m_dwSpacing>m_dwTexWidth)
		{
			x = m_dwSpacing;
			y += size.cy + 1;
		}

		ExtTextOut(hDC, x, y, ETO_OPAQUE, NULL, str, 1, NULL);

		m_fTexCord[i - 32][0] = float(x - m_dwSpacing) / float(m_dwTexWidth);
		m_fTexCord[i - 32][1] = float(y) / float(m_dwTexHeight);
		m_fTexCord[i - 32][2] = float(x + size.cx + m_dwSpacing) / float(m_dwTexWidth);
		m_fTexCord[i - 32][3] = float(y+size.cy) / float(m_dwTexHeight);
		
		x += size.cx + 2 * m_dwSpacing;
	}

	D3DLOCKED_RECT ddrect;
	m_pTexture->LockRect(0, &ddrect, NULL, 0);
	BYTE* pRectBits = NULL;
	pRectBits = (BYTE*)ddrect.pBits;
	WORD* pTexBits = NULL;
	
	BYTE Alpha;
	for (int i = 0; i < m_dwTexHeight; i++)
	{
		pTexBits = (WORD*)pRectBits;
		for (int j = 0; j < m_dwTexWidth; j++)
		{
			Alpha = (BYTE)((pBitmapBits[i*m_dwTexWidth + j] & 0xff) >> 4);
			if (Alpha>0)
				*pTexBits++ = (WORD)((Alpha << 12) | 0xfff);
			else
				*pTexBits++ = 0x0000;
		}
		pRectBits += ddrect.Pitch;
		//pTexBits = (WORD*)(pRectBits + ddrect.Pitch);
	}

	m_pTexture->UnlockRect(0);
	SelectObject(hDC, hBmpOld);
	SelectObject(hDC, hFontOld);
	DeleteObject(hBitmap);
	DeleteObject(hFont);
	DeleteDC(hDC);


	int vertexsize = max(sizeof(Font2DVertex), sizeof(Font3DVertex));
	hr = m_pDevice->CreateVertexBuffer(
		MAXVERTEX*vertexsize, 
		D3DUSAGE_WRITEONLY|D3DUSAGE_DYNAMIC,
		0,
		D3DPOOL_DEFAULT,
		&m_pVB, 
		NULL);
	if (FAILED(hr))
	{
		MessageBox(0, "Failed to create vertex buffer!", 0, 0);
		return E_FAIL;
	}
	RestoreDeviceState();

	return S_OK;
}

void D3DFont::RestoreDeviceState()
{
	for (UINT which = 0; which < 2; which++)
	{
		m_pDevice->BeginStateBlock();
		m_pDevice->SetTexture(0, m_pTexture);

		if (D3DFONT_ZENABLE & m_dwFontFlags)
			m_pDevice->SetRenderState(D3DRS_ZENABLE, TRUE);
		else
			m_pDevice->SetRenderState(D3DRS_ZENABLE, FALSE);

		m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		m_pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		m_pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
		m_pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
		m_pDevice->SetRenderState(D3DRS_ALPHAREF, 0x08);
		m_pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL);
		m_pDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
		m_pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
		m_pDevice->SetRenderState(D3DRS_STENCILENABLE, FALSE);
		m_pDevice->SetRenderState(D3DRS_CLIPPING, TRUE);
		m_pDevice->SetRenderState(D3DRS_CLIPPLANEENABLE, FALSE);
		m_pDevice->SetRenderState(D3DRS_VERTEXBLEND, D3DVBF_DISABLE);
		m_pDevice->SetRenderState(D3DRS_INDEXEDVERTEXBLENDENABLE, FALSE);
		m_pDevice->SetRenderState(D3DRS_FOGENABLE, FALSE);
		m_pDevice->SetRenderState(D3DRS_COLORWRITEENABLE,
			D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_GREEN |
			D3DCOLORWRITEENABLE_BLUE | D3DCOLORWRITEENABLE_ALPHA);
		m_pDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
		m_pDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		m_pDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
		m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
		m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
		m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
		m_pDevice->SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, 0);
		m_pDevice->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);
		m_pDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
		m_pDevice->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
		m_pDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
		m_pDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
		m_pDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

		if (which == 0)
			m_pDevice->EndStateBlock(&m_pStateSaved);
		else
			m_pDevice->EndStateBlock(&m_pDrawText);
	}
}

void D3DFont::DeleteDeviceObject()
{

}

void D3DFont::ReleaseDeviceObject()
{
	Release<IDirect3DTexture9*>(m_pTexture);
	Release<IDirect3DVertexBuffer9*>(m_pVB);
	Release<IDirect3DStateBlock9*>(m_pStateSaved);
	Release<IDirect3DStateBlock9*>(m_pDrawText);
}

void D3DFont::GetTextExtent(TCHAR* str, SIZE& size, DWORD dwTextType)
{
	switch (dwTextType)
	{
	case D3DTEXT_HORIZONTAL:
		GetTextExtentH(str, size);
		break;
	case D3DTEXT_VERTICAL:
		GetTextExtentV(str, size);
		break;
	default:
		GetTextExtentH(str, size);
		break;
	}
}

void D3DFont::GetTextExtentH(TCHAR* str, SIZE& size)
{
	memset(&size, 0, sizeof(size));
	if (NULL == str)
		return;

	int width = 0, height = 0;
	int x = 0, y = 0;
	int iCharNum = 0;
	while (*str)
	{
		TCHAR c = *str++;
		if (c == '\n' || c == '\r')
		{
			x = 0;
			y += (m_fTexCord[0][3] - m_fTexCord[0][1])*m_dwTexHeight;
			iCharNum = 0;
		}

		if (c < 32 || c >= 128)
			continue;
		iCharNum++;
		float pixelX1 = m_fTexCord[c - 32][0];
		float pixelX2 = m_fTexCord[c - 32][2];
		float pixelY1 = m_fTexCord[c - 32][1];
		float pixelY2 = m_fTexCord[c - 32][3];

		int w = (pixelX2 - pixelX1)*m_dwTexWidth - 2 * m_dwSpacing;
		x += w;
		if (x>width)
			width = x;
	}
	if (iCharNum > 0)
		height = y + (m_fTexCord[0][3] - m_fTexCord[0][1])*m_dwTexHeight;
	else
		height = y;
	size.cx = width;
	size.cy = height;
}

void D3DFont::GetTextExtentV(TCHAR* str, SIZE& size)
{
	memset(&size, 0, sizeof(size));
	if (NULL == str)
		return;
	int width = 0, height = 0;
	int x = 0, y = 0;
	int iCharNum = 0;
	while (*str)
	{
		TCHAR c = *str++;
		if (c == '\n'||c == '\r')
		{
			if (iCharNum > 0)
				width += x+m_iFontWidth;
			else
				width += 2*m_iFontWidth;
			y = 0;
			x = 0;
			iCharNum = 0;
		}
		if (c < 32 || c >= 128)
			continue;
		
		iCharNum++;
		float pixelX1 = m_fTexCord[c - 32][0];
		float pixelX2 = m_fTexCord[c - 32][2];
		float pixelY1 = m_fTexCord[c - 32][1];
		float pixelY2 = m_fTexCord[c - 32][3];

		y += (pixelY2 - pixelY1)*m_dwTexHeight;
		if (y>height)
			height = y;

		int w = (pixelX2 - pixelX1)*m_dwTexWidth - 2 * m_dwSpacing;
		x = max(w, x);
	}
	width += x;
	size.cx = width;
	size.cy = height;
}


HRESULT D3DFont::DrawText(int sx, int sy, TCHAR* str, D3DCOLOR color, DWORD dwTextType, DWORD dwLeanFlags, DWORD dwAlignFlags)
{
	if (NULL == m_pDevice)
		return E_FAIL;
	switch (dwTextType)
	{
	case D3DTEXT_HORIZONTAL:
		return DrawTextH(sx, sy, str, color, dwLeanFlags, dwAlignFlags);
		break;
	case D3DTEXT_VERTICAL:
		return DrawTextV(sx, sy, str, color, dwLeanFlags, dwAlignFlags);
		break;
	default:
		return DrawTextH(sx, sy, str, color, D3DTEXT_TOP_ONE, D3DTEXT_HLEFT);
		break;
	}
}

HRESULT D3DFont::DrawTextH(int sx, int sy, TCHAR* str, D3DCOLOR color, DWORD dwLeanFlags, DWORD dwAlignFlags)
{
	m_pStateSaved->Capture();
	m_pDrawText->Apply();
	m_pDevice->SetFVF(D3DFVF_FONT2DVERTEX);
	m_pDevice->SetPixelShader(NULL);
	m_pDevice->SetStreamSource(0, m_pVB, 0, sizeof(Font2DVertex));

	SIZE size;
	GetTextExtentH(str, size);

	if (size.cx == 0 && size.cy == 0)
		return S_OK;

	D3DVIEWPORT9 vp;
	m_pDevice->GetViewport(&vp);
	int iWindowWidth = vp.Width;
	int iWindowHeight = vp.Height;

	if (dwLeanFlags&D3DTEXT_CENTER_X)
	{
		sx = (iWindowWidth - size.cx) / 2;
	}
	else if (dwLeanFlags&D3DTEXT_LEFT)
	{
		sx = 0;
	}
	else if (dwLeanFlags&D3DTEXT_LEFT_ONE)
	{
		sx = m_iFontWidth;
	}
	else if (dwLeanFlags&D3DTEXT_RIGHT)
	{
		sx = iWindowWidth - size.cx;
	}
	else if (dwLeanFlags&D3DTEXT_RIGHT_ONE)
	{
		sx = iWindowWidth - size.cx - m_iFontWidth;
	}

	if (dwLeanFlags&D3DTEXT_CENTER_Y)
	{
		sy = (iWindowHeight - size.cy) / 2;
	}
	else if (dwLeanFlags&D3DTEXT_TOP)
	{
		sy = 0;
	}
	else if (dwLeanFlags&D3DTEXT_BOTTOM)
	{
		sy = iWindowHeight - size.cy;
	}
	else if (dwLeanFlags&D3DTEXT_TOP_ONE)
	{
		sy = m_iFontHeight;
	}
	else if (dwLeanFlags&D3DTEXT_BOTTOM_ONE)
	{
		sy = iWindowHeight - size.cy - m_iFontHeight;
	}

	int iLineNum = 0;
	int iCharNum = 0;
	
	while (*str)
	{
		iLineNum++;
		iCharNum = 1;
		while (*(str + iCharNum - 1) != '\n'&&*(str + iCharNum - 1) != '\r'&&*(str + iCharNum - 1) != '\0')
		{
			iCharNum++;
		}
		TCHAR* strLine = new TCHAR[iCharNum + 1];
		memcpy(strLine, str, iCharNum);
		strLine[iCharNum] = '\0';
		SIZE sizeline;
		GetTextExtentH(strLine, sizeline);

		int iLineX, iLineY;
		if (dwAlignFlags&D3DTEXT_HCENTER)
		{
			iLineX = sx + (size.cx - sizeline.cx) / 2;
		}
		else if (dwAlignFlags&D3DTEXT_HRIGHT)
		{
			
			iLineX = sx + size.cx - sizeline.cx;
		}
		else
		{
			iLineX = sx;
		}
		iLineY = sy + (iLineNum - 1)*m_iFontHeight;

		DrawTextLineH(iLineX, iLineY, strLine, color);
		if (strLine)
		{
			delete[] strLine;
			strLine = NULL;
		}

		if (*(str + iCharNum - 1) == '\0')
			str += iCharNum - 1;
		else
			str += iCharNum;
	}

	m_pStateSaved->Apply();
	return S_OK;
}

HRESULT D3DFont::DrawTextV(int sx, int sy, TCHAR* str, D3DCOLOR color, DWORD dwLeanFlags, DWORD dwAlignFlags)
{
	m_pStateSaved->Capture();
	m_pDrawText->Apply();
	m_pDevice->SetFVF(D3DFVF_FONT2DVERTEX);
	m_pDevice->SetPixelShader(NULL);
	m_pDevice->SetStreamSource(0, m_pVB, 0, sizeof(Font2DVertex));

	SIZE size;
	GetTextExtentV(str, size);

	if (size.cx == 0 && size.cy == 0)
		return S_OK;

	D3DVIEWPORT9 vp;
	m_pDevice->GetViewport(&vp);
	int iWindowWidth = vp.Width;
	int iWindowHeight = vp.Height;

	if (dwLeanFlags&D3DTEXT_CENTER_X)
	{
		sx = (iWindowWidth - size.cx) / 2;
	}
	else if (dwLeanFlags&D3DTEXT_LEFT)
	{
		sx = 0;
	}
	else if (dwLeanFlags&D3DTEXT_LEFT_ONE)
	{
		sx = m_iFontWidth;
	}
	else if (dwLeanFlags&D3DTEXT_RIGHT)
	{
		sx = iWindowWidth - size.cx;
	}
	else if (dwLeanFlags&D3DTEXT_RIGHT_ONE)
	{
		sx = iWindowWidth - size.cx - m_iFontWidth;
	}

	if (dwLeanFlags&D3DTEXT_CENTER_Y)
	{
		sy = (iWindowHeight - size.cy) / 2;
	}
	else if (dwLeanFlags&D3DTEXT_TOP)
	{
		sy = 0;
	}
	else if (dwLeanFlags&D3DTEXT_BOTTOM)
	{
		sy = iWindowHeight - size.cy;
	}
	else if (dwLeanFlags&D3DTEXT_TOP_ONE)
	{
		sy = m_iFontHeight;
	}
	else if (dwLeanFlags&D3DTEXT_BOTTOM_ONE)
	{
		sy = iWindowHeight - size.cy - m_iFontHeight;
	}

	int iLineNum = 0;
	int iCharNum = 0;

	int iLineX = sx, iLineY = 0;
	while (*str)
	{
		iLineNum++;
		iCharNum = 1;
		while (*(str + iCharNum - 1) != '\n'&&*(str + iCharNum - 1) != '\r'&&*(str + iCharNum - 1) != '\0')
		{
			iCharNum++;
		}
		TCHAR* strLine = new TCHAR[iCharNum + 1];
		memcpy(strLine, str, iCharNum);
		strLine[iCharNum] = '\0';
		SIZE sizeline;
		GetTextExtentV(strLine, sizeline);

		if (dwAlignFlags&D3DTEXT_VCENTER)
		{
			iLineY = sy + (size.cy - sizeline.cy) / 2;
		}
		else if (dwAlignFlags&D3DTEXT_VBOTTOM)
		{

			iLineY = sy + size.cy - sizeline.cy;
		}
		else
		{
			iLineY = sy;
		}
		
		DrawTextLineV(iLineX, iLineY, strLine, color);
		iLineX += sizeline.cx;

		if (strLine)
		{
			delete[] strLine;
			strLine = NULL;
		}

		if (*(str + iCharNum - 1) == '\0')
			str += iCharNum - 1;
		else
			str += iCharNum;
	}

	m_pStateSaved->Apply();
	return S_OK;
}


HRESULT D3DFont::DrawTextLineH(int sx, int sy, TCHAR* str, D3DCOLOR color)
{
	if (!str)
		return E_FAIL;
	LRESULT hr = 0;
	int iNumTri = 0;
	Font2DVertex* pVertex = NULL;
	hr = m_pVB->Lock(0, 0, (void**)&pVertex, D3DLOCK_DISCARD);
	if (FAILED(hr))
		return E_FAIL;
	
	while (*str)
	{
		TCHAR c = *str++;
		if (c<32 || c >= 128)
			continue;
		float iCharX1 = m_fTexCord[c - 32][0];
		float iCharX2 = m_fTexCord[c - 32][2];
		float iCharY1 = m_fTexCord[c - 32][1];
		float iCharY2 = m_fTexCord[c - 32][3];

		float tu1 = iCharX1 + (float)m_dwSpacing / (float)m_dwTexWidth;
		float tu2 = iCharX2 - (float)m_dwSpacing / (float)m_dwTexWidth;
		float tv1 = iCharY1;
		float tv2 = iCharY2;

		int w = (iCharX2 - iCharX1)*m_dwTexWidth - 2 * m_dwSpacing;
		int h = (iCharY2 - iCharY1)*m_dwTexHeight;

		*pVertex++ = Font2DVertex(D3DXVECTOR4(sx, sy + h, 0.001f, 1.0f), color, tu1, tv2);
		*pVertex++ = Font2DVertex(D3DXVECTOR4(sx, sy, 0.001f, 1.0f), color, tu1, tv1);
		*pVertex++ = Font2DVertex(D3DXVECTOR4(sx + w, sy + h, 0.001f, 1.0f), color, tu2, tv2);
		*pVertex++ = Font2DVertex(D3DXVECTOR4(sx + w, sy + h, 0.001f, 1.0f), color, tu2, tv2);
		*pVertex++ = Font2DVertex(D3DXVECTOR4(sx, sy, 0.001f, 1.0f), color, tu1, tv1);
		*pVertex++ = Font2DVertex(D3DXVECTOR4(sx + w, sy, 0.001f, 1.0f), color, tu2, tv1);

		iNumTri += 2;
		if (iNumTri * 3 >= (MAXVERTEX-6))
		{
			hr = m_pVB->Unlock();
			if (FAILED(hr))
				return E_FAIL;
			hr = m_pDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, iNumTri);
			if (FAILED(hr))
				return E_FAIL;
			pVertex = NULL;
			hr = m_pVB->Lock(0, 0, (void**)pVertex, D3DLOCK_DISCARD);
			if (FAILED(hr))
				return E_FAIL;
			iNumTri = 0;
		}
		sx += w;
	}

	hr = m_pVB->Unlock();
	if (FAILED(hr))
		return E_FAIL;
	if (iNumTri > 0)
	{
		hr = m_pDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, iNumTri);
		if (FAILED(hr))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT D3DFont::DrawTextLineV(int sx, int sy, TCHAR* str, D3DCOLOR color)
{
	if (!str)
		return E_FAIL;
	LRESULT hr = 0;
	int iNumTri = 0;
	Font2DVertex* pVertex = NULL;
	hr = m_pVB->Lock(0, 0, (void**)&pVertex, D3DLOCK_DISCARD);
	if (FAILED(hr))
		return E_FAIL;
	while (*str)
	{
		TCHAR c = *str++;
		if (c < 32 || c >= 128)
			continue;
		float iCharX1 = m_fTexCord[c - 32][0];
		float iCharX2 = m_fTexCord[c - 32][2];
		float iCharY1 = m_fTexCord[c - 32][1];
		float iCharY2 = m_fTexCord[c - 32][3];

		float tu1 = iCharX1 + (float)m_dwSpacing / (float)m_dwTexWidth;
		float tu2 = iCharX2 - (float)m_dwSpacing / (float)m_dwTexWidth;

		int w = (iCharX2 - iCharX1)*m_dwTexWidth - 2 * m_dwSpacing;
		int h = (iCharY2 - iCharY1)*m_dwTexHeight;

		*pVertex++ = Font2DVertex(D3DXVECTOR4(sx, sy + h, 0.9f, 1.0f), color, tu1, iCharY2);
		*pVertex++ = Font2DVertex(D3DXVECTOR4(sx, sy, 0.9f, 1.0f), color, tu1, iCharY1);
		*pVertex++ = Font2DVertex(D3DXVECTOR4(sx + w, sy + h, 0.9f, 1.0f), color, tu2, iCharY2);
		*pVertex++ = Font2DVertex(D3DXVECTOR4(sx + w, sy + h, 0.9f, 1.0f), color, tu2, iCharY2);
		*pVertex++ = Font2DVertex(D3DXVECTOR4(sx, sy, 0.9f, 1.0f), color, tu1, iCharY1);
		*pVertex++ = Font2DVertex(D3DXVECTOR4(sx + w, sy, 0.9f, 1.0f), color, tu2, iCharY1);
		
		iNumTri += 2;
		if (iNumTri * 3 >= (MAXVERTEX - 6))
		{
			hr = m_pVB->Unlock();
			if (FAILED(hr))
				return E_FAIL;
			hr = m_pDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, iNumTri);
			if (FAILED(hr))
				return E_FAIL;
			pVertex = NULL;
			hr = m_pVB->Lock(0, 0, (void**)pVertex, D3DLOCK_DISCARD);
			if (FAILED(hr))
				return E_FAIL;
			iNumTri = 0;
		}
		sy += h;
	}

	hr = m_pVB->Unlock();
	if (FAILED(hr))
		return E_FAIL;
	if (iNumTri > 0)
	{
		hr = m_pDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, iNumTri);
		if (FAILED(hr))
			return E_FAIL;
	}

	return S_OK;
}



HRESULT D3DFont::DrawText3D(
	TCHAR* str,
	D3DXVECTOR3 pos,
	D3DCOLOR color,
	DWORD dwAlignFlags, 
	DWORD dwTowSides,
	D3DXVECTOR3 n,
	D3DXVECTOR3 u,
	D3DXVECTOR3 v)
{
	if (!m_pDevice)
		return E_FAIL;
	m_pStateSaved->Capture();
	m_pDrawText->Apply();
	m_pDevice->SetFVF(D3DFVF_FONT3DVERTEX);
	m_pDevice->SetPixelShader(NULL);
	m_pDevice->SetStreamSource(0, m_pVB, 0, sizeof(Font3DVertex));

	if (dwTowSides & D3DFONT_TWOSIDES)
		m_pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	SIZE size;
	GetTextExtentH(str, size);
	if (size.cx == 0 && size.cy == 0)
		return S_OK;

	int iLineNum = 0;
	int iCharNum = 0;

	while (*str)
	{
		iLineNum++;
		iCharNum = 1;
		while (*(str + iCharNum - 1) != '\n'&&*(str + iCharNum - 1) != '\r'&&*(str + iCharNum - 1) != '\0')
		{
			iCharNum++;
		}
		TCHAR* strLine = new TCHAR[iCharNum + 1];
		memcpy(strLine, str, iCharNum);
		strLine[iCharNum] = '\0';
		SIZE sizeline;
		GetTextExtentH(strLine, sizeline);

		D3DXVECTOR3 linepos;
		int iLineX, iLineY;
		if (dwAlignFlags&D3DTEXT_HCENTER)
		{
			linepos = pos + (SCALE3D*((float)(size.cx - sizeline.cx) )/ 2.0f)*u;
		}
		else if (dwAlignFlags&D3DTEXT_HRIGHT)
		{

			linepos = pos + ((float)(size.cx - sizeline.cx))*SCALE3D*u;
		}
		else
		{
			linepos = pos;
		}
		linepos += ((float)((iLineNum - 1)*m_iFontHeight))*SCALE3D*v;

		DrawText3DH(strLine, linepos, color, n, u, v);
		if (strLine)
		{
			delete[] strLine;
			strLine = NULL;
		}

		if (*(str + iCharNum - 1) == '\0')
			str += iCharNum - 1;
		else
			str += iCharNum;
	}


	m_pStateSaved->Apply();
	return S_OK;
}


HRESULT D3DFont::DrawText3DH(
	TCHAR* str, 
	D3DXVECTOR3 pos, 
	D3DCOLOR color,
	D3DXVECTOR3 n, 
	D3DXVECTOR3 u, 
	D3DXVECTOR3 v)
{
	if (!str)
		return E_FAIL;
	float fDotNxU, fDotNxV, fDotUxV;
	fDotNxU = D3DXVec3Dot(&n, &u);
	fDotNxV = D3DXVec3Dot(&n, &v);
	fDotUxV = D3DXVec3Dot(&u, &v);
	if (fDotUxV > 0.1f || fDotNxU > 0.1f || fDotNxV > 0.1f)
		return E_FAIL;

	LRESULT hr = 0;
	int iNumTri = 0;
	Font3DVertex* pVertex = NULL;
	hr = m_pVB->Lock(0, 0, (void**)&pVertex, D3DLOCK_DISCARD);
	if (FAILED(hr))
		return E_FAIL;

	while (*str)
	{
		TCHAR c = *str++;
		if (c == '\n' || c == '\r')
			continue;
		float iCharX1 = m_fTexCord[c - 32][0];
		float iCharX2 = m_fTexCord[c - 32][2];
		float iCharY1 = m_fTexCord[c - 32][1];
		float iCharY2 = m_fTexCord[c - 32][3];

		float tu1 = iCharX1 + (float)m_dwSpacing / (float)m_dwTexWidth;
		float tu2 = iCharX2 - (float)m_dwSpacing / (float)m_dwTexWidth;
		float tv1 = iCharY1;
		float tv2 = iCharY2;

		float w = (iCharX2 - iCharX1)*m_dwTexWidth - 2 * m_dwSpacing;
		float h = (iCharY2 - iCharY1)*m_dwTexHeight;
		w *= SCALE3D;
		h *= SCALE3D;

		*pVertex++ = Font3DVertex(pos + h*v, n, color,tu1, tv2);
		*pVertex++ = Font3DVertex(pos, n, color,tu1, tv1);
		*pVertex++ = Font3DVertex(pos + w*u + h*v, n, color,tu2, tv2);
		*pVertex++ = Font3DVertex(pos + w*u + h*v, n, color,tu2, tv2);
		*pVertex++ = Font3DVertex(pos, n, color,tu1, tv1);
		*pVertex++ = Font3DVertex(pos + w*u, n, color,tu2, tv1);

		iNumTri += 2;

		if (iNumTri * 3 >= (MAXVERTEX - 6))
		{
			hr = m_pVB->Unlock();
			if (FAILED(hr))
				return E_FAIL;
			hr = m_pDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, iNumTri);
			if (FAILED(hr))
				return E_FAIL;
			pVertex = NULL;
			hr = m_pVB->Lock(0, 0, (void**)pVertex, D3DLOCK_DISCARD);
			if (FAILED(hr))
				return E_FAIL;
			iNumTri = 0;
		}

		pos += w*u;
	}

	hr = m_pVB->Unlock();
	if (FAILED(hr))
		return E_FAIL;
	if (iNumTri > 0)
	{
		hr = m_pDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, iNumTri);
		if (FAILED(hr))
			return E_FAIL;
	}
	return S_OK;
}
