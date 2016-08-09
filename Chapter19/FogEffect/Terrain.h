#ifndef _TERRAIN_H__
#define _TERRAIN_H__
#include "d3dUtility.h"
#include<string>
#include<vector>
using std::string;
using std::vector;

class Terrain
{
public:
	Terrain(
		IDirect3DDevice9* pDevice,
		string sFileName,
		int iVertsPerRow,
		int iVertsPerCol,
		int iCellSpacing,
		float fScale
		);
	~Terrain();

	int GetVertexHeight(int row, int col);
	void SetVertexHeight(int row, int col, int value);
	float GetHeight(float x, float z);

	bool LoadTexture(string filename);
	bool GenerateTexture(D3DXVECTOR3* pDirToLight);
	bool Draw(D3DXMATRIX* pWorld, bool bDrawTris = false);

private:
	bool ReadHeightMapFile(string filename);
	bool ComputeVertices();
	bool ComputeIndices();
	float ComputeShade(int cellrow, int cellcol,D3DXVECTOR3* pDirToLight);
	bool LightTerrain(D3DXVECTOR3* pDirToLight);


private:
	IDirect3DDevice9* m_pDevice;
	IDirect3DTexture9* m_pTex;
	IDirect3DVertexBuffer9* m_pVB;
	IDirect3DIndexBuffer9* m_pIB;

	string m_sHeightMapFileName;

	int m_nVertsPerRow;
	int m_nVertsPerCol;
	int m_nCellSpacing;

	int m_nCellPerRow;
	int m_nCellPerCol;
	int m_nWitdh;
	int m_nHeight;
	int m_nNumVertices;
	int m_nNumTriangles;

	float m_fHeightScale;

	vector<int> m_vHeightMap;

};

#endif