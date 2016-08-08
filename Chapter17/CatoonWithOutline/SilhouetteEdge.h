#ifndef _SILHOUETTEEDGE_H__
#define _SILHOUETTEEDGE_H__
#include "d3dUtility.h"
struct MeshVertex
{
	D3DXVECTOR3 position;
	D3DXVECTOR3 normal;
	static const DWORD FVF = D3DFVF_XYZ | D3DFVF_NORMAL;
};

struct EdgeVertex
{
	D3DXVECTOR3 position;
	D3DXVECTOR3 normal;
	D3DXVECTOR3 facenormal1;
	D3DXVECTOR3 facenormal2;
};

class SilhouetteEdge
{
public:
	SilhouetteEdge(IDirect3DDevice9* pDevice,ID3DXMesh* pMesh,ID3DXBuffer* pAdjBuffer);
	~SilhouetteEdge();
	void Render();
private:
	bool CreateVertexDeclaration();
	void GetFaceNormal(ID3DXMesh* pMesh, DWORD dwFaceIndex, D3DXVECTOR3* pFaceNormal);
	void GetFaceNormals(ID3DXMesh* pMesh, ID3DXBuffer* pAdjBuff, DWORD dwFaceIndex, D3DXVECTOR3* pCurFaceNormal, D3DXVECTOR3 (*pAdjFaceNormal)[3]);
	void GenEdgeVertices(ID3DXMesh* pMesh, ID3DXBuffer* pAdjBuff);
	void GenEdgeIndices(ID3DXMesh* pMesh);
private:
	IDirect3DDevice9* m_pDevice;
	IDirect3DVertexBuffer9* m_pVB;
	IDirect3DIndexBuffer9* m_pIB;
	IDirect3DVertexDeclaration9* m_pVD;

	DWORD m_dwFaceNum;
	DWORD m_dwVertexNum;
	DWORD m_dwIndexNum;
};
#endif
