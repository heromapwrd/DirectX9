#ifndef _D3DFRAME_H__
#define _D3DFRAME_H__
#include "D3DMesh.h"
#include<list>
using namespace std;
using std::list;

class D3DFrame
{
	TCHAR m_strName[MAX_PATH];
	list<D3DMesh*> m_ListMeshs;
	D3DXMATRIX m_Matrix;
	D3DFrame* m_pParent;
	D3DFrame* m_pChild;
	D3DFrame* m_pPrevious;
	D3DFrame* m_pNext;
public:
	D3DFrame(TCHAR* pFilenaem = _T("D3DFrame"));
	virtual ~D3DFrame();
public:
	void FindMesh(TCHAR* pMeshName, D3DMesh* pMesh);
	void FindMesh(TCHAR* pFrameName, TCHAR* pMeshName, D3DMesh* pMesh);
	void FindLocalMesh(TCHAR* pMeshName, D3DMesh* pMesh);
	void FindFrame(TCHAR* pFrameName, D3DFrame* pFrame);
	bool LoadFromData(IDirect3DDevice9* pDevice, ID3DXFileData* pFileData, D3DFrame* pParent = NULL, D3DFrame* pPrevious = NULL);
	bool LoadFromFile(IDirect3DDevice9* pDevice, TCHAR* pFileName);
	bool Render(IDirect3DDevice9* pDevice, D3DXMATRIX* pWorldMatix = NULL,  bool bDrawOpaqueSubsets = true,  bool bDrawAlphaSubsets = true);
	bool EnumMesh(bool(*EnumMeshs)(D3DMesh* pMesh, void* pData),void* pArg);
	bool EnumFrame(bool(*EnumFrames)(D3DFrame* pFrame, void* pData),void* pArg);
	D3DFrame* GetLastChild();
	bool SaveMeshToLocal(IDirect3DDevice9* pDevice);
	void DeleteLocalMesh();
	void Destroy();
};


#endif //_D3DFRAME_H__