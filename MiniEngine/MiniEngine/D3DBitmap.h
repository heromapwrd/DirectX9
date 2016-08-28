//-------------------------------------------------------------------------------
//	FILE:D3DBitmap.h
//	AUTHOR:Liu Quanbo
//	DESC:D3DBitmap 可以读取.BMP文件，并且可以保存.BMP文件,8,24,32位像素格式，8位的时候用
//		 调色板，图像数据部分每个像素为1个字节，像素值为像素颜色在调色板中的索引，24位和32位
//		 时无调色板，图像数据每个像素占用3个和4个字节，为真实的像素颜色数据,由于BMP文件每行像
//		 素字节数必须为4的倍数，所以每行字节数可能比width*bitcount/8要大
//-------------------------------------------------------------------------------
#ifndef _D3DBITMAP_H__
#define _D3DBITMAP_H__
#include "D3DCommon.h"
#include<wingdi.h>
#include<tchar.h>

enum{
	BITCOUNT8,
	BITCOUNT24,
	BITCOUNT32,
};

class D3DBitmap
{
	BITMAPFILEHEADER m_FileHeader;
	BITMAPINFOHEADER m_InfoHeader;
	RGBQUAD* m_pPallet;
	BYTE* m_pImageData;

public:
	bool LoadFromImage(TCHAR* filename);
	bool LoadFromData(BYTE* pData, int width, int height, int bitcount, bool bAlign = false);
	bool SaveToImage(TCHAR* filename);
	bool ConvertFrom24To32(BYTE* pAlpha);
	bool ConvertFrom32To24();
	bool Draw(HDC hDestDC,int left = 0,int top = 0);
	SIZE GetImageSize();
	int GetBitCount();
	BYTE* GetImageData();
	RGBQUAD* GetImagePallet();
private:
	void Delete();
public:
	D3DBitmap();
	~D3DBitmap();
	D3DBitmap(const D3DBitmap& bitmap);
	D3DBitmap& operator =(const D3DBitmap& bitmap);

};


#endif // _D3DBITMAP_H__