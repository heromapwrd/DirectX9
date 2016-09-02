#ifndef _D3DMESH_H__
#define _D3DMESH_H__

#include <tchar.h>
#include<d3dx9mesh.h>
#include "D3DCommon.h"

class D3DMesh
{
	TCHAR		m_strName[MAX_PATH];
	ID3DXMesh*	m_pSysMesh;
	ID3DXMesh*	m_pLocalMesh;
	IDirect3DTexture9** m_ppTextures;
	D3DMATERIAL9*		m_pMaterials;
	DWORD*	m_pAdjacency;
	DWORD	m_dwMatNum;
	bool	m_bUserMaterial;
public:
	bool Create(IDirect3DDevice9* pDevice, const TCHAR* pFilename);
	bool Create(IDirect3DDevice9* pDevice, ID3DXFileData* pFileData);
	bool SetFVF(IDirect3DDevice9* pDevice, DWORD dwFVF);
	bool SaveMeshToLocal(IDirect3DDevice9* pDevice);
	bool Render(IDirect3DDevice9* pDevice, bool bDrawOpaqueSubsets = true, bool bDrawAlphaSubsets = true);
	
	void UseMeshMaterials(bool bUseMaterial){
		m_bUserMaterial = bUseMaterial; 
	}
	void DeleteLocalMesh();
	ID3DXMesh* GetSysMesh();
	ID3DXMesh* GetLocalMesh();
	TCHAR* GetName();
	void Destroy();
	

public:
	D3DMesh(TCHAR* filename = _T("D3DMesh"));
	virtual ~D3DMesh();
};



#endif //_D3DMESH_H__