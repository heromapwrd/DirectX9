#ifndef __GRAPHICS_API_H__
#define __GRAPHICS_API_H__

#include <windows.h>

#define SrcCopy 0xcc0020

class graphicsAPI
{
public:

	void init(int Width, int Height);
	static graphicsAPI *getInstance();

	inline bool storePixel(int x, int y, RGBQUAD color)
	{
		//x+=halfwidth;
		y=height-1-y;
		//if(x<0 || x>=width || y<0 || y>=height ) 
		//	return false;

		BYTE *p=pixels+4*(x+y*width);
		*(p++) = color.rgbBlue;
		*(p++) = color.rgbGreen ;
		*p = color.rgbRed ;

		return true;
	}

	void flush();
	void draw(HDC hDC, int left=0, int top=0);


private:

	graphicsAPI();
	~graphicsAPI();

	static graphicsAPI *instance;

	BYTE *pixels;
	int width,halfwidth;
	int height,halfheight;
	int rectSize;

	BITMAPINFO m_bitmapInfo;
};
	

#endif
