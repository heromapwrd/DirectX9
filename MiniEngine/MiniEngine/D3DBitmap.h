//-------------------------------------------------------------------------------
//	FILE:D3DBitmap.h
//	AUTHOR:Liu Quanbo
//	DESC:D3DBitmap ���Զ�ȡ.BMP�ļ������ҿ��Ա���.BMP�ļ�,8,24,32λ���ظ�ʽ��8λ��ʱ����
//		 ��ɫ�壬ͼ�����ݲ���ÿ������Ϊ1���ֽڣ�����ֵΪ������ɫ�ڵ�ɫ���е�������24λ��32λ
//		 ʱ�޵�ɫ�壬ͼ������ÿ������ռ��3����4���ֽڣ�Ϊ��ʵ��������ɫ����,����BMP�ļ�ÿ����
//		 ���ֽ�������Ϊ4�ı���������ÿ���ֽ������ܱ�width*bitcount/8Ҫ��
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