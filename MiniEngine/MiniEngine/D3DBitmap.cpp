#include "D3DBitmap.h"
#include<stdio.h>
#include<windows.h>

#define SrcCopy 0xcc0020

D3DBitmap::D3DBitmap()
{
	ZeroMemory(&m_FileHeader, sizeof(m_FileHeader));
	ZeroMemory(&m_InfoHeader, sizeof(m_InfoHeader));
	m_pImageData = NULL;
	m_pPallet = NULL;
}

D3DBitmap::~D3DBitmap()
{
	Delete();
}

D3DBitmap::D3DBitmap(const D3DBitmap& bitmap)
{
	ZeroMemory(&m_FileHeader, sizeof(m_FileHeader));
	ZeroMemory(&m_InfoHeader, sizeof(m_InfoHeader));
	m_pImageData = NULL;
	m_pPallet = NULL;
	memcpy((void*)&m_FileHeader, (void*)&bitmap.m_FileHeader, sizeof(BITMAPFILEHEADER));
	memcpy((void*)&m_InfoHeader, (void*)&bitmap.m_InfoHeader, sizeof(BITMAPINFOHEADER));
	if (bitmap.m_pPallet)
	{
		m_pPallet = new RGBQUAD[256];
		memset((void*)m_pPallet, 0, 256 * sizeof(RGBQUAD));
		memcpy((void*)m_pPallet, (void*)bitmap.m_pPallet, 256 * sizeof(RGBQUAD));
	}
	if (bitmap.m_pImageData)
	{
		int iChannnels = bitmap.m_InfoHeader.biBitCount / 8;
		int size = iChannnels*bitmap.m_InfoHeader.biWidth*bitmap.m_InfoHeader.biHeight;
		m_pImageData = new BYTE[size];
		memset((void*)m_pImageData, 0, size);
		memcpy((void*)m_pImageData, (void*)bitmap.m_pImageData, size);
	}
}

D3DBitmap& D3DBitmap::operator =(const D3DBitmap& bitmap)
{
	ZeroMemory(&m_FileHeader, sizeof(m_FileHeader));
	ZeroMemory(&m_InfoHeader, sizeof(m_InfoHeader));
	m_pImageData = NULL;
	m_pPallet = NULL;
	Delete();
	memcpy((void*)&m_FileHeader, (void*)&bitmap.m_FileHeader, sizeof(BITMAPFILEHEADER));
	memcpy((void*)&m_InfoHeader, (void*)&bitmap.m_InfoHeader, sizeof(BITMAPINFOHEADER));
	if (bitmap.m_pPallet)
	{
		m_pPallet = new RGBQUAD[256];
		memset((void*)m_pPallet, 0, 256 * sizeof(RGBQUAD));
		memcpy((void*)m_pPallet, (void*)bitmap.m_pPallet, 256 * sizeof(RGBQUAD));
	}
	if (bitmap.m_pImageData)
	{
		int iChannnels = bitmap.m_InfoHeader.biBitCount / 8;
		int size = iChannnels*bitmap.m_InfoHeader.biWidth*bitmap.m_InfoHeader.biHeight;
		m_pImageData = new BYTE[size];
		memset((void*)m_pImageData, 0, size);
		memcpy((void*)m_pImageData, (void*)bitmap.m_pImageData, size);
	}
	return *this;
}

bool D3DBitmap::LoadFromImage(TCHAR* filename)
{
	if (!filename)
		return false;
	FILE* pFile;
	if ((pFile = fopen(filename, "rb")) == NULL)
		return false;
	if (!fread((void*)&m_FileHeader, sizeof(BITMAPFILEHEADER), 1, pFile))
	{
		fclose(pFile);
		return false;
	}
	if (!fread((void*)&m_InfoHeader, sizeof(BITMAPINFOHEADER), 1, pFile))
	{
		fclose(pFile);
		return false;
	}
	m_InfoHeader.biWidth = abs(m_InfoHeader.biWidth);
	m_InfoHeader.biHeight = abs(m_InfoHeader.biHeight);
	if (m_InfoHeader.biBitCount != 8 && m_InfoHeader.biBitCount != 24 && m_InfoHeader.biBitCount != 32)
	{
		fclose(pFile);
		return false;
	}
	if (m_InfoHeader.biCompression != BI_RGB)
	{
		fclose(pFile);
		return false;
	}
	if (m_InfoHeader.biBitCount==8)
	{
		m_pPallet = new RGBQUAD[256];
		if (!m_pPallet)
		{
			Delete();
			fclose(pFile);
			return false;
		}
		memset((void*)m_pPallet, 0, 256 * sizeof(RGBQUAD));
		for (int i = 0; i < 256; i++)
		{
			m_pPallet[i].rgbRed = m_pPallet[i].rgbGreen = m_pPallet[i].rgbBlue = i;
		}
		if (!fseek(pFile, 256 * sizeof(RGBQUAD), SEEK_CUR))
		{
			fclose(pFile);
			return false;
		}
	}
	
	int iChannels = m_InfoHeader.biBitCount / 8;
	m_pImageData = new BYTE[iChannels*m_InfoHeader.biWidth*m_InfoHeader.biHeight];
	if (!m_pImageData)
	{
		Delete();
		fclose(pFile);
		return false;
	}

	int WidthBytes = iChannels*m_InfoHeader.biWidth;
	int Offset = WidthBytes % 4;
	if (Offset != 0)
		Offset = 4 - Offset;

	BYTE skip = 0;
	for (int y = 0; y < m_InfoHeader.biHeight; y++)
	{
		for (int x = 0; x < m_InfoHeader.biWidth; x++)
		{
			if (!fread((void*)&m_pImageData[y*m_InfoHeader.biWidth*iChannels + x*iChannels], iChannels*sizeof(BYTE), 1, pFile))
			{
				Delete();
				fclose(pFile);
				return false;
			}
		}

		if (Offset!=0)
		{
			for (int i = 0; i < Offset; i++)
			{
				if (!fread((void*)&skip, sizeof(BYTE), 1, pFile))
				{
					Delete();
					fclose(pFile);
					return false;
				}
			}
		}
	}
	fclose(pFile);
	return true;
}

bool D3DBitmap::LoadFromData(BYTE* pData, int width, int height, int bitcount,bool bAlign)
{
	if (!pData)
		return false;
	if (width <= 0 || height <= 0)
		return false;
	int iChannels = bitcount / 8;
	int WidthBytes = width*iChannels;
	int Offset = WidthBytes % 4;
	if (Offset != 0)
		Offset = 4 - Offset; 
	int iLineLength = WidthBytes + Offset;
	int size = iLineLength*height;

	m_FileHeader.bfType = 0x424D;
	m_FileHeader.bfReserved1 = 0;
	m_FileHeader.bfReserved2 = 0;

	m_InfoHeader.biBitCount = bitcount;
	m_InfoHeader.biCompression = BI_RGB;
	m_InfoHeader.biHeight = height;
	m_InfoHeader.biWidth = width;
	m_InfoHeader.biPlanes = 1;
	m_InfoHeader.biSize = sizeof(BITMAPINFOHEADER);
	m_InfoHeader.biSizeImage = size;
	m_InfoHeader.biXPelsPerMeter = 0;
	m_InfoHeader.biYPelsPerMeter = 0;

	if (bitcount == 8)
	{
		m_pPallet = new RGBQUAD[256];
		if (!m_pPallet)
		{
			Delete();
			return false;
		}
		memset((void*)m_pPallet, 0, 256 * sizeof(RGBQUAD));
		for (int i = 0; i < 256; i++)
		{
			m_pPallet[i].rgbRed = m_pPallet[i].rgbGreen = m_pPallet[i].rgbBlue = i;
		}
		m_FileHeader.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + 256 * sizeof(RGBQUAD) + size;
		m_FileHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + 256 * sizeof(RGBQUAD);
		m_InfoHeader.biClrUsed = 256;
		m_InfoHeader.biClrImportant = 256;
	}
	else
	{
		m_FileHeader.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + size;
		m_FileHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) ;
		m_InfoHeader.biClrUsed = 0;
		m_InfoHeader.biClrImportant = 0;
	}

	m_pImageData = new BYTE[iChannels*width*height];
	if (!m_pImageData)
	{
		Delete();
		return false;
	}

	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			if (bAlign)
			{
				for (int i = 0; i < iChannels; i++)
				{
					m_pImageData[y*width*iChannels + x*iChannels + i] = pData[y*iLineLength + x*iChannels + i];
				}
			}
			else
			{
				for (int i = 0; i < iChannels; i++)
				{
					m_pImageData[y*width*iChannels + x*iChannels + i] = pData[y*width*iChannels + x*iChannels + i];
				}
			}
		}
	}
	return true;
}

bool D3DBitmap::SaveToImage(TCHAR* filename)
{
	FILE* pFile;
	if ((pFile = fopen(filename, "wb")) == NULL)
		return false;
	BITMAPFILEHEADER fileheader = m_FileHeader;
	BITMAPINFOHEADER infoheader = m_InfoHeader;
	infoheader.biHeight = -infoheader.biHeight;
	if (!fwrite((void*)&fileheader, sizeof(BITMAPFILEHEADER), 1, pFile))
	{
		fclose(pFile);
		return false;
	}
	if (!fwrite((void*)&infoheader, sizeof(BITMAPINFOHEADER), 1, pFile))
	{
		fclose(pFile);
		return false;
	}

	if (m_InfoHeader.biBitCount==8)
	{
		if (!fwrite((void*)m_pPallet, 256 * sizeof(RGBQUAD), 1, pFile))
		{
			fclose(pFile);
			return false;
		}
	}

	int iChannels = m_InfoHeader.biBitCount / 8;
	int iWidthBytes = m_InfoHeader.biWidth*iChannels;
	int Offset = iWidthBytes % 4;
	if (Offset != 0)
		Offset = 4 - Offset;
	for (int y = 0; y < m_InfoHeader.biHeight; y++)
	{
		if (!fwrite((void*)(m_pImageData + y*m_InfoHeader.biWidth*iChannels), m_InfoHeader.biWidth*iChannels, 1, pFile))
		{
			fclose(pFile);
			return false;
		}
		BYTE iValue = 0;
		for (int i = 0; i < Offset; i++)
		{
			if (!fwrite((void*)&iValue, sizeof(BYTE), 1, pFile))
			{
				fclose(pFile);
				return false;
			}
		}
	}
	fclose(pFile);
	return true;
}

SIZE D3DBitmap::GetImageSize()
{
	SIZE size;
	size.cx = m_InfoHeader.biWidth;
	size.cy = m_InfoHeader.biHeight;
	return size;
}

int D3DBitmap::GetBitCount()
{
	return m_InfoHeader.biBitCount;
}

BYTE* D3DBitmap::GetImageData()
{
	return m_pImageData;
}

RGBQUAD* D3DBitmap::GetImagePallet()
{
	return m_pPallet;
}

bool D3DBitmap::ConvertFrom24To32(BYTE* pAlpha)
{
	int iChannels = 4;
	int iChannelsOld = 3;
	BYTE* pNewBuff = new BYTE[iChannels * m_InfoHeader.biWidth*m_InfoHeader.biHeight];
	if (!pNewBuff)
		return false;
	memset((void*)pNewBuff, 0, iChannels * m_InfoHeader.biWidth*m_InfoHeader.biHeight);
	for (int y = 0; y < m_InfoHeader.biHeight; y++)
	{
		for (int x = 0; x < m_InfoHeader.biWidth; x++)
		{
			if (NULL == pAlpha)
			{
				pNewBuff[y*iChannels*m_InfoHeader.biWidth + x*iChannels + 0] = 0;
			}
			else
			{
				pNewBuff[y*iChannels*m_InfoHeader.biWidth + x*iChannels + 0] = pAlpha[y*m_InfoHeader.biWidth + x];
			}
			
			pNewBuff[y*iChannels*m_InfoHeader.biWidth + x*iChannels + 1] = m_pImageData[y*iChannels*m_InfoHeader.biWidth + x*iChannels + 0];
			pNewBuff[y*iChannels*m_InfoHeader.biWidth + x*iChannels + 2] = m_pImageData[y*iChannels*m_InfoHeader.biWidth + x*iChannels + 1];
			pNewBuff[y*iChannels*m_InfoHeader.biWidth + x*iChannels + 3] = m_pImageData[y*iChannels*m_InfoHeader.biWidth + x*iChannels + 2];
		}
	}

	
	int iOffset = (iChannels*m_InfoHeader.biWidth) % 4;
	if (iOffset != 0)
		iOffset = 4 - iOffset;
	int iLine = m_InfoHeader.biWidth*iChannels + iOffset;

	int iOffsetOld = (iChannelsOld*m_InfoHeader.biWidth) % 4;
	if (iOffsetOld != 0)
		iOffsetOld = 4 - iOffsetOld;
	int iLineOld = m_InfoHeader.biWidth*iChannelsOld + iOffsetOld;
	int iLineDelta = iLine - iLineOld;

	int iSizeDelta = iLineDelta*m_InfoHeader.biHeight;

	m_FileHeader.bfSize += iSizeDelta;
	m_InfoHeader.biSizeImage + iSizeDelta;
	m_InfoHeader.biBitCount = 8 * iChannels;
	DeleteArray(m_pImageData);
	m_pImageData = pNewBuff;
}

bool D3DBitmap::ConvertFrom32To24()
{
	int iChannels = 3;
	int iChannelsOld = 4;
	BYTE* pNewBuff = new BYTE[iChannels * m_InfoHeader.biWidth*m_InfoHeader.biHeight];
	if (!pNewBuff)
		return false;
	memset((void*)pNewBuff, 0, iChannels * m_InfoHeader.biWidth*m_InfoHeader.biHeight);
	for (int y = 0; y < m_InfoHeader.biHeight; y++)
	{
		for (int x = 0; x < m_InfoHeader.biWidth; x++)
		{

			pNewBuff[y*iChannels*m_InfoHeader.biWidth + x*iChannels + 0] = m_pImageData[y*iChannels*m_InfoHeader.biWidth + x*iChannels + 1];
			pNewBuff[y*iChannels*m_InfoHeader.biWidth + x*iChannels + 1] = m_pImageData[y*iChannels*m_InfoHeader.biWidth + x*iChannels + 2];
			pNewBuff[y*iChannels*m_InfoHeader.biWidth + x*iChannels + 2] = m_pImageData[y*iChannels*m_InfoHeader.biWidth + x*iChannels + 3];
		}
	}


	int iOffset = (iChannels*m_InfoHeader.biWidth) % 4;
	if (iOffset != 0)
		iOffset = 4 - iOffset;
	int iLine = m_InfoHeader.biWidth*iChannels + iOffset;

	int iOffsetOld = (iChannelsOld*m_InfoHeader.biWidth) % 4;
	if (iOffsetOld != 0)
		iOffsetOld = 4 - iOffsetOld;
	int iLineOld = m_InfoHeader.biWidth*iChannelsOld + iOffsetOld;
	int iLineDelta = iLine - iLineOld;

	int iSizeDelta = iLineDelta*m_InfoHeader.biHeight;

	m_FileHeader.bfSize += iSizeDelta;
	m_InfoHeader.biSizeImage + iSizeDelta;
	m_InfoHeader.biBitCount = 8 * iChannels;
	DeleteArray(m_pImageData);
	m_pImageData = pNewBuff;
}

void D3DBitmap::Delete()
{
	DeleteArray<RGBQUAD*>(m_pPallet);
	DeleteArray<BYTE*>(m_pImageData);
}

bool D3DBitmap::Draw(HDC hDestDC,int left,int top)
{
	if (NULL == m_pImageData)
		return false;
	if (m_InfoHeader.biBitCount != 32)
		return false;
	HDC hDC = CreateCompatibleDC(NULL);
	BITMAPINFO info;
	info.bmiHeader = m_InfoHeader;
	HBITMAP hBitmap = CreateDIBSection(hDC, &info, 0, 0, 0, 0);
	HGDIOBJ hOldBitmap = SelectObject(hDC,hBitmap);

	int iChannels = m_InfoHeader.biBitCount / 8;
	SetBitmapBits(hBitmap, iChannels*m_InfoHeader.biWidth*m_InfoHeader.biHeight, (void*)m_pImageData);

	BitBlt(hDestDC, left, top, m_InfoHeader.biWidth, m_InfoHeader.biHeight, hDC, 0, 0, SrcCopy);
	SelectObject(hDC,hOldBitmap);

	DeleteObject(hBitmap);
	DeleteDC(hDC);
	ReleaseDC(0, hDC);
}


