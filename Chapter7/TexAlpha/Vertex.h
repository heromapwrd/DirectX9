/*
-----------------------------------------------------
Vertex.h 灵活定义定点结构
-----------------------------------------------------
*/
#ifndef _VERTEX_H__
#define _VERTEX_H__
#include<d3dx9.h>

struct Vertex
{
	Vertex(){}
	Vertex(float _x, float _y, float _z, float _nx, float _ny, float _nz, float _u, float _v)
	{
		x = _x;
		y = _y;
		z = _z;
		nx = _nz;
		ny = _ny;
		nz = _nz;
		u = _u;
		v = _v;
	}
	float x, y, z;
	float nx, ny, nz;
	float u, v;

	static const DWORD FVF = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1;
};

//const DWORD Vertex::FVF = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1;

#endif