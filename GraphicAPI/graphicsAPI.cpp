#include "graphicsAPI.h"

graphicsAPI *graphicsAPI::instance = NULL;

graphicsAPI::~graphicsAPI()
{
	if(pixels)
	{
		delete[] pixels;
		pixels=NULL;
	}
}

graphicsAPI::graphicsAPI() :width(0),height(0),rectSize(0)
{
	this->pixels=NULL;
}

void graphicsAPI::init(int Width,int Height)
{
	width=Width;
	height=Height;
	halfwidth=width>>1;
	halfheight=height>>1;
	rectSize=width*height;

	pixels=new BYTE[4*rectSize];
	memset(pixels,0,4*rectSize*sizeof(BYTE));

	
	m_bitmapInfo.bmiHeader.biBitCount =32;
    m_bitmapInfo.bmiHeader.biSize =sizeof(m_bitmapInfo.bmiHeader );
    m_bitmapInfo.bmiHeader.biCompression =BI_RGB;
	m_bitmapInfo.bmiHeader.biPlanes =1;
	m_bitmapInfo.bmiHeader.biWidth =width;
	m_bitmapInfo.bmiHeader.biHeight =4*height;
	m_bitmapInfo.bmiHeader.biSizeImage=rectSize;
}

graphicsAPI *graphicsAPI::getInstance()
{
	if(!instance)
	{
		instance = new graphicsAPI;
	}
	
	return instance;	
}


//bool graphicsAPI::storePixel(int x, int y, RGBQUAD color)
//{
//	//x+=halfwidth;
//	y=height-y;
//	if(x<0 || x>=width || y<0 || y>=height ) 
//		return false;
//
//	BYTE *p=pixels+4*(x+y*width);
//	*(p++) = color.rgbBlue;
//	*(p++) = color.rgbGreen ;
//	*p = color.rgbRed ;
//
//	return true;
//}

void graphicsAPI::flush()
{
	memset(pixels,0,4*rectSize*sizeof(BYTE));
}

void graphicsAPI::draw(HDC hDC, int left, int top)
{
	HDC mydc;
	HBITMAP mybmp;
	HGDIOBJ sele;

	mydc=CreateCompatibleDC(0);
    mybmp = CreateDIBSection(mydc,&m_bitmapInfo,0,0,0,0);
    sele = SelectObject(mydc, mybmp);

	SetBitmapBits(mybmp,4*rectSize,pixels);
    BitBlt( hDC, left, top, width, height, mydc, 0, 0, SrcCopy);
    DeleteObject(SelectObject(mydc,mybmp));
	DeleteObject(mybmp);
	DeleteDC(mydc);
	ReleaseDC(0,mydc);
}