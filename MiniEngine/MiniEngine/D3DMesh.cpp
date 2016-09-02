#include "D3DMesh.h"

D3DMesh::D3DMesh(TCHAR* filename /*= _T("D3DMesh")*/)
{
	_tcsncpy(m_strName, filename, sizeof(m_strName));
	m_strName[sizeof(m_strName)-1] = '\0';

	m_pSysMesh = NULL;
	m_pLocalMesh = NULL;
	m_pAdjacency = NULL;
	m_ppTextures = NULL;
	m_pMaterials = NULL;
	m_dwMatNum = 0;
	m_bUserMaterial = true;
}

D3DMesh::~D3DMesh()
{
	Destroy();
}

bool D3DMesh::Create(IDirect3DDevice9* pDevice, const TCHAR* pFilename)
{
	if (!pDevice || !pFilename || strlen(pFilename) < 1)
		return false;
	HRESULT hr = 0;
	ID3DXBuffer* pMaterialBuff = NULL;
	hr = D3DXLoadMeshFromX(pFilename, D3DXMESH_MANAGED, pDevice, NULL, &pMaterialBuff, NULL, &m_dwMatNum, &m_pSysMesh);
	if (FAILED(hr))
	{
		MessageBox(0, TEXT("Failed to create mesh!"), 0, 0);
		return false;
	}

	hr = m_pSysMesh->OptimizeInplace(D3DXMESHOPT_COMPACT | D3DXMESHOPT_ATTRSORT, NULL, NULL, NULL,NULL);
	if (FAILED(hr))
	{
		Release<ID3DXMesh*>(m_pSysMesh);
		MessageBox(0, TEXT("Failed to Optimize mesh!"), 0, 0);
		return false;
	}
	m_pAdjacency = new DWORD[m_pSysMesh->GetNumFaces() * 3];
	hr = m_pSysMesh->GenerateAdjacency(0.001f, m_pAdjacency);
	if (FAILED(hr))
	{
		Delete<DWORD*>(m_pAdjacency);
		Release<ID3DXMesh*>(m_pSysMesh);
		MessageBox(0, TEXT("Failed to Optimize mesh!"), 0, 0);
		return false;
	}

	if (pMaterialBuff&&m_dwMatNum > 0)
	{
		m_pMaterials = new D3DMATERIAL9[m_dwMatNum];
		m_ppTextures = new IDirect3DTexture9*[m_dwMatNum];
		D3DXMATERIAL* pMaterial = (D3DXMATERIAL*)pMaterialBuff->GetBufferPointer();
		for (DWORD i = 0; i < m_dwMatNum; i++)
		{
			m_pMaterials[i] = pMaterial[i].MatD3D;
			m_ppTextures[i] = NULL;
			hr = D3DXCreateTextureFromFile(pDevice, pMaterial[i].pTextureFilename, &m_ppTextures[i]);
				if (FAILED(hr))
				{
					MessageBox(0, TEXT("Faild to create texture!"), 0, 0);
					return false;
				}
		}
	}
	
	Release<ID3DXBuffer*>(pMaterialBuff);
	return SaveMeshToLocal(pDevice);
}

bool D3DMesh::Create(IDirect3DDevice9* pDevice, ID3DXFileData* pFileData)
{
	if (!pDevice || !pFileData)
		return false;
	HRESULT hr = 0;
	ID3DXBuffer* pMaterialBuff = NULL;
	hr = D3DXLoadMeshFromXof(pFileData, D3DXMESH_MANAGED, pDevice, NULL, &pMaterialBuff, NULL, &m_dwMatNum, &m_pSysMesh);
	if (FAILED(hr))
	{
		MessageBox(0, TEXT("Failed to create mesh!"), 0, 0);
		return false;
	}

	hr = m_pSysMesh->OptimizeInplace(D3DXMESHOPT_COMPACT | D3DXMESHOPT_ATTRSORT, NULL, NULL, NULL, NULL);
	if (FAILED(hr))
	{
		Release<ID3DXMesh*>(m_pSysMesh);
		MessageBox(0, TEXT("Failed to Optimize mesh!"), 0, 0);
		return false;
	}
	m_pAdjacency = new DWORD[m_pSysMesh->GetNumFaces() * 3];
	hr = m_pSysMesh->GenerateAdjacency(0.001f, m_pAdjacency);
	if (FAILED(hr))
	{
		Delete<DWORD*>(m_pAdjacency);
		Release<ID3DXMesh*>(m_pSysMesh);
		MessageBox(0, TEXT("Failed to Optimize mesh!"), 0, 0);
		return false;
	}

	if (pMaterialBuff&&m_dwMatNum > 0)
	{
		m_pMaterials = new D3DMATERIAL9[m_dwMatNum];
		m_ppTextures = new IDirect3DTexture9*[m_dwMatNum];
		D3DXMATERIAL* pMaterial = (D3DXMATERIAL*)pMaterialBuff->GetBufferPointer();
		for (DWORD i = 0; i < m_dwMatNum; i++)
		{
			m_pMaterials[i] = pMaterial[i].MatD3D;
			m_ppTextures[i] = NULL;
			hr = D3DXCreateTextureFromFile(pDevice, pMaterial[i].pTextureFilename, &m_ppTextures[i]);
			if (FAILED(hr))
			{
				MessageBox(0, TEXT("Faild to create texture!"), 0, 0);
				return false;
			}
		}
	}

	Release<ID3DXBuffer*>(pMaterialBuff);
	return SaveMeshToLocal(pDevice);
}

bool D3DMesh::SetFVF(IDirect3DDevice9* pDevice, DWORD dwFVF)
{
	if (NULL == pDevice)
		return false;
	ID3DXMesh* pTempSysMesh = NULL;
	ID3DXMesh* pTempLocalMesh = NULL;
	HRESULT hr = 0;
	if (m_pSysMesh)
	{
		hr = m_pSysMesh->CloneMeshFVF(D3DXMESH_MANAGED, dwFVF, pDevice, &pTempSysMesh);
		if (FAILED(hr))
		{
			MessageBox(0, TEXT("Faild to Clone m_pSysMesh to Temp!"), 0, 0);
			return false;
		}
	}

	if (m_pLocalMesh)
	{
		hr = m_pLocalMesh->CloneMeshFVF(D3DXMESH_MANAGED, dwFVF, pDevice, &pTempLocalMesh);
		if (FAILED(hr))
		{
			Release<ID3DXMesh*>(pTempSysMesh);
			MessageBox(0, TEXT("Failed to Clone m_pLocalMesh to Temp!"), 0, 0);
			return false;
		}
	}
	return true;
}

bool D3DMesh::SaveMeshToLocal(IDirect3DDevice9* pDevice)
{
	if (NULL == m_pSysMesh||NULL == pDevice)
		return false;
	DeleteLocalMesh();
	HRESULT hr = 0;
	hr = m_pSysMesh->CloneMeshFVF(D3DXMESH_MANAGED, m_pSysMesh->GetFVF(), pDevice, &m_pLocalMesh);
	if (FAILED(hr))
	{
		MessageBox(0, TEXT("Failed to Clone To Local!"), 0, 0);
		return false;
	}
	return true;
}


bool D3DMesh::Render(IDirect3DDevice9* pDevice, bool bDrawOpaqueSubsets /*= true*/, bool bDrawAlphaSubsets /*= true*/)
{
	if (!pDevice)
		return false;
	// Render Opaque Subsets
	if (bDrawOpaqueSubsets)
	{
		for (DWORD i = 0; i < m_dwMatNum; i++)
		{
			if (m_pMaterials[i].Diffuse.a < 1.0f)
				continue;
			pDevice->SetMaterial(&m_pMaterials[i]);
			pDevice->SetTexture(0, m_ppTextures[i]);
			m_pLocalMesh->DrawSubset(i);
		}
	}
	
	// Render Alpha Subsets
	if (bDrawAlphaSubsets&&m_bUserMaterial)
	{
		for (DWORD i = 0; i < m_dwMatNum; i++)
		{
			if (m_pMaterials[i].Diffuse.a == 1.0f)
				continue;
			pDevice->SetMaterial(&m_pMaterials[i]);
			pDevice->SetTexture(0, m_ppTextures[i]);
			m_pLocalMesh->DrawSubset(i);
		}
	}
	return true;
}

void D3DMesh::DeleteLocalMesh()
{
	Release<ID3DXMesh*>(m_pLocalMesh);
}

ID3DXMesh* D3DMesh::GetSysMesh()
{
	return m_pSysMesh;
}

ID3DXMesh* D3DMesh::GetLocalMesh()
{
	return m_pLocalMesh;
}

void D3DMesh::Destroy()
{
	DeleteLocalMesh();
	Release<ID3DXMesh*>(m_pSysMesh);
	for (DWORD i = 0; i < m_dwMatNum; i++)
	{
		Release<IDirect3DTexture9*>(m_ppTextures[i]);
	}
	Delete<IDirect3DTexture9**>(m_ppTextures);
	Delete<D3DMATERIAL9*>(m_pMaterials);
	Delete<DWORD*>(m_pAdjacency);
}

TCHAR* D3DMesh::GetName()
{
	return m_strName;
}
