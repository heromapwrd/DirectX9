#include "D3DFrame.h"
#include <rmxfguid.h>
#include <rmxftmpl.h>

D3DFrame::D3DFrame([IN] TCHAR* pFilenaem /*= _T("D3DFrame")*/)
{
	_tcsncpy(m_strName, pFilenaem, sizeof(m_strName));
	m_strName[sizeof(m_strName)-1] = '\0';

	D3DXMatrixIdentity(&m_Matrix);
	m_ListMeshs.resize(0);
	m_pParent	= NULL;
	m_pChild	= NULL;
	m_pPrevious = NULL;
	m_pNext		= NULL;
}

D3DFrame::~D3DFrame()
{
	Destroy();
}

void D3DFrame::Destroy()
{
	Delete<D3DFrame*>(m_pNext);
	Delete<D3DFrame*>(m_pChild);
	m_pParent = NULL;
	m_pPrevious = NULL;
	list<D3DMesh*>::iterator it = m_ListMeshs.begin();
	for (; it != m_ListMeshs.end(); it++)
	{
		Delete<D3DMesh*>(*it);
	}
}

bool D3DFrame::LoadFromFile(IDirect3DDevice9* pDevice, TCHAR* pFileName)
{
	if (NULL == pDevice || NULL == pFileName || strlen(pFileName) < 1)
		return false;
	ID3DXFile* pFile = NULL;
	if (FAILED(D3DXFileCreate(&pFile)))
		return false;
	ID3DXFileEnumObject* pFileEnumObj = NULL;
	if (FAILED(pFile->CreateEnumObject(pFileName, D3DXF_FILELOAD_FROMFILE, &pFileEnumObj)))
		goto END;
	SIZE_T len = 0;
	if (FAILED(pFileEnumObj->GetChildren(&len)))
		goto END;
	m_pParent = NULL;
	m_pPrevious = NULL;
	for (SIZE_T i = 0; i < len; i++)
	{
		ID3DXFileData* pFileObj = NULL;
		if (FAILED(pFileEnumObj->GetChild(i, &pFileObj)))
			return false;
		if (pFileObj->IsReference())
		{
			Release<ID3DXFileData*>(pFileObj);
			continue;
		}

		TCHAR* strName = NULL;
		SIZE_T length = 0;
		pFileObj->GetName(NULL, &length);
		strName = new TCHAR[length + 1];
		pFileObj->GetName(strName, &length);
		strName[length + 1] = '\0';

		GUID* pObjGUID;
		pFileObj->GetId(pObjGUID);
		if (TID_D3DRMMesh == *pObjGUID)
		{
			D3DMesh* pNewMesh = new D3DMesh(strName);
			pNewMesh->Create(pDevice, pFileObj);
			m_ListMeshs.push_back(pNewMesh);
		}

		if (TID_D3DRMFrameTransformMatrix == *pObjGUID)
		{
			SIZE_T len = 0;
			D3DXMATRIX* pMatrix = NULL;
			pFileObj->Lock(&len, (void**)&pMatrix);
			m_Matrix = *pMatrix;
			pFileObj->Unlock();
		}

		if (TID_D3DRMFrame == *pObjGUID)
		{
			D3DFrame* pNewFrame = new D3DFrame(strName);
			D3DFrame* pChild = GetLastChild();
			if (NULL == pChild)
			{
				m_pChild = pNewFrame;
				pNewFrame->LoadFromData(pDevice, pFileObj, this, NULL);
			}
			else
			{
				/*pChild->m_pNext = pNewFrame;*/
				pNewFrame->LoadFromData(pDevice, pFileObj, this, pChild);
			}
		}

		if (TID_D3DRMFramePosition == *pObjGUID)
		{

		}

		if (TID_D3DRMFrameRotation == *pObjGUID)
		{

		}

		if (TID_D3DRMFrameVelocity == *pObjGUID)
		{

		}

		Delete<TCHAR*>(strName);
		Release<ID3DXFileData*>(pFileObj);
	}

	END:Release(pFileEnumObj);
	Release<ID3DXFile*>(pFile);
	return true;
}

bool D3DFrame::LoadFromData(IDirect3DDevice9* pDevice, ID3DXFileData* pFileData, D3DFrame* pParent /*= NULL*/, D3DFrame* pPrevious /*= NULL*/)
{
	if (NULL == pDevice||NULL == pFileData)
		return false;
	m_pParent = pParent;
	m_pPrevious = pPrevious;
	if (m_pPrevious)
		m_pPrevious->m_pNext = this;
	SIZE_T iObjNum = 0;
	if (FAILED(pFileData->GetChildren(&iObjNum)))
		return false;
	for (SIZE_T i = 0; i < iObjNum; i++)
	{
		ID3DXFileData* pFileObj = NULL;
		if (FAILED(pFileData->GetChild(i, &pFileObj)))
			return false;
		if (pFileObj->IsReference())
		{
			Release<ID3DXFileData*>(pFileObj);
			continue;
		}

		TCHAR* strName = NULL;
		SIZE_T length = 0;
		pFileObj->GetName(NULL, &length);
		strName = new TCHAR[length + 1];
		pFileObj->GetName(strName, &length);
		strName[length + 1] = '\0';

		GUID* pObjGUID;
		pFileObj->GetId(pObjGUID);
		if (TID_D3DRMMesh == *pObjGUID)
		{
			D3DMesh* pNewMesh = new D3DMesh(strName);
			pNewMesh->Create(pDevice, pFileObj);
			m_ListMeshs.push_back(pNewMesh);
		}

		if (TID_D3DRMFrameTransformMatrix == *pObjGUID)
		{
			SIZE_T len = 0;
			D3DXMATRIX* pMatrix = NULL;
			pFileObj->Lock(&len,(void**)&pMatrix);
			m_Matrix = *pMatrix;
			pFileObj->Unlock();
		}

		if (TID_D3DRMFrame == *pObjGUID)
		{
			D3DFrame* pNewFrame = new D3DFrame(strName);
			D3DFrame* pChild = GetLastChild();
			if (NULL == pChild)
			{
				m_pChild = pNewFrame;
				pNewFrame->LoadFromData(pDevice, pFileObj, this, NULL);
			}
			else
			{
				/*pChild->m_pNext = pNewFrame;*/
				pNewFrame->LoadFromData(pDevice, pFileObj, this, pChild);
			}
		}

		if (TID_D3DRMFramePosition == *pObjGUID)
		{

		}

		if (TID_D3DRMFrameRotation == *pObjGUID)
		{

		}

		if (TID_D3DRMFrameVelocity == *pObjGUID)
		{

		}

		Delete<TCHAR*>(strName);
		Release<ID3DXFileData*>(pFileObj);
	}
	
	return true;
}

D3DFrame* D3DFrame::GetLastChild()
{
	if (!m_pChild)
		return NULL;
	D3DFrame* pChild = m_pChild;
	while (pChild->m_pNext)
		pChild = pChild->m_pNext;
	return pChild;
}


void D3DFrame::FindFrame(TCHAR* pFrameName, D3DFrame* pFrame)
{
	pFrame = NULL;
	if (NULL == pFrameName)
		return;
	if (!lstrcmpi(m_strName, pFrameName))
	{
		pFrame = this;
		return;
	}
	if (m_pChild)
	{
		m_pChild->FindFrame(pFrameName, pFrame);
		if (pFrame)
			return;
	}
	if (m_pNext)
	{
		m_pNext->FindFrame(pFrameName, pFrame);
	}
}

void D3DFrame::FindLocalMesh(TCHAR* pMeshName, D3DMesh* pMesh)
{
	pMesh = NULL;
	if (pMeshName == NULL)
		return;
	list<D3DMesh*>::iterator it = m_ListMeshs.begin();
	for (; it != m_ListMeshs.end(); it++)
	{
		if (!lstrcmpi((*it)->GetName(), pMeshName))
		{
			pMesh = *it;
			return;
		}
	}
}

void D3DFrame::FindMesh(TCHAR* pMeshName, D3DMesh* pMesh)
{
	pMesh = NULL;
	if (NULL == pMeshName)
		return;
	FindLocalMesh(pMeshName, pMesh);
	if (pMesh)
		return;
	if (m_pChild)
	{
		m_pChild->FindMesh(pMeshName, pMesh);
		if (pMesh)
			return;
	}
	if (m_pNext)
	{
		m_pNext->FindMesh(pMeshName, pMesh);
	}
}

void D3DFrame::FindMesh(TCHAR* pFrameName, TCHAR* pMeshName, D3DMesh* pMesh)
{
	pMesh = NULL;
	if (NULL == pFrameName || NULL == pMeshName)
		return;
	D3DFrame* pFrame = NULL;
	FindFrame(pFrameName, pFrame);
	if (NULL == pFrame)
		return;
	pFrame->FindMesh(pMeshName, pMesh);
}

bool D3DFrame::Render(IDirect3DDevice9* pDevice, D3DXMATRIX* pWorldMatix /*= NULL*/, bool bDrawOpaqueSubsets /*= true*/, bool bDrawAlphaSubsets /*= true*/)
{
	if (NULL == pDevice)
		return false;
	D3DXMATRIX mParentMatrix, mFrameMarix;
	if (NULL == pWorldMatix)
	{
		pDevice->GetTransform(D3DTS_WORLD, &mParentMatrix);
	}
	else
	{
		mParentMatrix = *pWorldMatix;
	}

	mFrameMarix = mParentMatrix*m_Matrix;
	pDevice->SetTransform(D3DTS_WORLD, &mFrameMarix);
	list<D3DMesh*>::iterator it = m_ListMeshs.begin();
	for (; it != m_ListMeshs.end(); it++)
	{
		(*it)->Render(pDevice, bDrawOpaqueSubsets, bDrawAlphaSubsets);
	}
	if (m_pChild)
	{
		m_pChild->Render(pDevice, &mFrameMarix, bDrawOpaqueSubsets, bDrawAlphaSubsets);
	}
	if (m_pNext)
	{
		m_pNext->Render(pDevice, &mParentMatrix, bDrawOpaqueSubsets, bDrawAlphaSubsets);
	}
	return true;
}

bool D3DFrame::EnumMesh(bool(*EnumMeshs)(D3DMesh* pMesh, void* pData),void* pArg)
{
	if (NULL == EnumMeshs)
		return false;
	list<D3DMesh*>::iterator it = m_ListMeshs.begin();
	for (; it != m_ListMeshs.end(); it++)
	{
		EnumMeshs((*it), pArg);
	}
	if (m_pChild)
		m_pChild->EnumMesh(EnumMeshs, pArg);
	if (m_pNext)
		m_pNext->EnumMesh(EnumMeshs, pArg);
	return true;
}

bool D3DFrame::EnumFrame(bool(*EnumFrames)(D3DFrame* pFrame, void* pData),void* pArg)
{
	if (NULL == EnumFrames)
		return false;
	EnumFrames(this, pArg);
	if (m_pChild)
		m_pChild->EnumFrame(EnumFrames, pArg);
	if (m_pNext)
		m_pNext->EnumFrame(EnumFrames, pArg);
	return true;
}

bool D3DFrame::SaveMeshToLocal(IDirect3DDevice9* pDevice)
{
	if (NULL == pDevice)
		return false;
	list<D3DMesh*>::iterator it = m_ListMeshs.begin();
	for (; it != m_ListMeshs.end(); it++)
	{
		(*it)->SaveMeshToLocal(pDevice);
	}
	if (m_pChild)
		m_pChild->SaveMeshToLocal(pDevice);
	if (m_pNext)
		m_pNext->SaveMeshToLocal(pDevice);
	return true;
}

void D3DFrame::DeleteLocalMesh()
{
	list<D3DMesh*>::iterator it = m_ListMeshs.begin();
	for (; it != m_ListMeshs.end(); it++)
	{
		(*it)->DeleteLocalMesh();
	}
	if (m_pChild)
		m_pChild->DeleteLocalMesh();
	if (m_pNext)
		m_pNext->DeleteLocalMesh();
}

