#include "SilhouetteEdge.h"

SilhouetteEdge::SilhouetteEdge(IDirect3DDevice9* pDevice, ID3DXMesh* pMesh, ID3DXBuffer* pAdjBuffer)
{
	m_pDevice = pDevice;
	m_pIB = NULL;
	m_pVB = NULL;
	m_pVD = NULL;
	m_dwFaceNum = 0;
	m_dwIndexNum = NULL;
	m_dwVertexNum = NULL;

	CreateVertexDeclaration();
	GenEdgeVertices(pMesh, pAdjBuffer);
	GenEdgeIndices(pMesh);
}

SilhouetteEdge::~SilhouetteEdge()
{
	d3d9::Release(m_pVB);
	d3d9::Release(m_pIB);
	d3d9::Release(m_pVD);
}

void SilhouetteEdge::Render()
{
	m_pDevice->SetVertexDeclaration(m_pVD);
	m_pDevice->SetStreamSource(0, m_pVB, 0, sizeof(EdgeVertex));
	m_pDevice->SetIndices(m_pIB);
	m_pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, m_dwVertexNum, 0, m_dwFaceNum);
}

bool SilhouetteEdge::CreateVertexDeclaration()
{
	D3DVERTEXELEMENT9 vertexelement[] =
	{
		{ 0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
		{ 0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0 },
		{ 0, 24, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 1 },
		{ 0, 36, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 2 },
		D3DDECL_END()
	};

	HRESULT hr = 0;
	hr = m_pDevice->CreateVertexDeclaration(vertexelement, &m_pVD);
	if (FAILED(hr))
	{
		MessageBox(0, "Failed to Create vertex declaration!", 0, 0);
		return false;
	}
	return true;
}

void SilhouetteEdge::GetFaceNormal(ID3DXMesh* pMesh, DWORD dwFaceIndex, D3DXVECTOR3* pFaceNormal)
{
	IDirect3DVertexBuffer9* pVB = NULL;
	IDirect3DIndexBuffer9* pIB = NULL;

	DWORD vertexnum = pMesh->GetNumVertices();
	DWORD facenum = pMesh->GetNumFaces();

	pMesh->GetVertexBuffer(&pVB);
	pMesh->GetIndexBuffer(&pIB);
	MeshVertex* pMeshVertices;
	WORD* pIndices;
	pVB->Lock(0, 0, (void**)&pMeshVertices, 0);
	pIB->Lock(0, 0, (void**)&pIndices, 0);

	D3DXVECTOR3 A;
	D3DXVECTOR3 B;
	D3DXVECTOR3 C;
	WORD indexA = pIndices[dwFaceIndex * 3];
	WORD indexB = pIndices[dwFaceIndex * 3 + 1];
	WORD indexC = pIndices[dwFaceIndex * 3 + 2];
	A = pMeshVertices[indexA].position;
	B = pMeshVertices[indexB].position;
	C = pMeshVertices[indexC].position;

	D3DXVECTOR3 v0 = B - A;
	D3DXVECTOR3 v1 = C - A;
	D3DXVec3Cross(pFaceNormal, &v0, &v1);
	D3DXVec3Normalize(pFaceNormal, pFaceNormal);

	pVB->Unlock();
	pIB->Unlock();
}

void SilhouetteEdge::GetFaceNormals(ID3DXMesh* pMesh, ID3DXBuffer* pAdjBuff, DWORD dwFaceIndex, D3DXVECTOR3* pCurFaceNormal, D3DXVECTOR3(*pAdjFaceNormal)[3])
{
	GetFaceNormal(pMesh, dwFaceIndex, pCurFaceNormal);
	DWORD size = pAdjBuff->GetBufferSize();
	int size1 = sizeof(DWORD);
	int size2 = sizeof(WORD);
	int size3 = sizeof(int);
	DWORD* pAdjIndex = (DWORD*)pAdjBuff->GetBufferPointer();
	WORD faceindex1 = pAdjIndex[dwFaceIndex * 3];
	WORD faceindex2 = pAdjIndex[dwFaceIndex * 3 + 1];
	WORD faceindex3 = pAdjIndex[dwFaceIndex * 3 + 2];

	if (faceindex1 != USHRT_MAX)
	{
		GetFaceNormal(pMesh, faceindex1, (*pAdjFaceNormal));
	}
	else
	{
		(*pAdjFaceNormal)[0] = -*pCurFaceNormal;
	}

	if (faceindex2 != USHRT_MAX)
	{
		GetFaceNormal(pMesh, faceindex2, (*pAdjFaceNormal) + 1);
	}
	else
	{
		(*pAdjFaceNormal)[1] = -*pCurFaceNormal;
	}

	if (faceindex3 != USHRT_MAX)
	{
		GetFaceNormal(pMesh, faceindex3, (*pAdjFaceNormal) + 2);
	}
	else
	{
		(*pAdjFaceNormal)[2] = -*pCurFaceNormal;
	}
}

void SilhouetteEdge::GenEdgeVertices(ID3DXMesh* pMesh, ID3DXBuffer* pAdjBuff)
{
	if (!pMesh || !pAdjBuff)
		return;
	DWORD dwEdgeNum = pMesh->GetNumFaces() * 3;
	m_dwVertexNum = dwEdgeNum * 4;
	IDirect3DVertexBuffer9* pVB = NULL;
	IDirect3DIndexBuffer9* pIB = NULL;
	pMesh->GetVertexBuffer(&pVB);
	pMesh->GetIndexBuffer(&pIB);
	if (!pVB || !pIB)
		return;
	HRESULT hr = 0;
	hr = m_pDevice->CreateVertexBuffer(
		m_dwVertexNum*sizeof(EdgeVertex),
		D3DUSAGE_WRITEONLY,
		0,
		D3DPOOL_MANAGED,
		&m_pVB,
		NULL
		);
	if (FAILED(hr))
	{
		MessageBox(0, "Failed to create vertex buffer!", 0, 0);
		return;
	}

	MeshVertex* pMeshVertices;
	WORD* pIndices;
	EdgeVertex* pEdgeVertices;
	pVB->Lock(0, 0, (void**)&pMeshVertices, 0);
	pIB->Lock(0, 0, (void**)&pIndices, 0);
	m_pVB->Lock(0, 0, (void**)&pEdgeVertices,0);

	for (int faceindex = 0; faceindex < pMesh->GetNumFaces(); faceindex++)
	{
		DWORD dwVertexNumPerFace = 3 * 4;//每个片面形成的四边形顶点总数
		D3DXVECTOR3 curfacenormal;
		D3DXVECTOR3 adjfacenormal[3];
		GetFaceNormals(pMesh, pAdjBuff, faceindex, &curfacenormal, &adjfacenormal);
		MeshVertex A;
		MeshVertex B;
		MeshVertex C;
		WORD indexA = pIndices[faceindex * 3];
		WORD indexB = pIndices[faceindex * 3 + 1];
		WORD indexC = pIndices[faceindex * 3 + 2];
		A = pMeshVertices[indexA];
		B = pMeshVertices[indexB];
		C = pMeshVertices[indexC];

		//A->B形成的退化四边形
		EdgeVertex A0, B0, C0, D0;
		A0.position = A.position;
		A0.normal = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		A0.facenormal1 = curfacenormal;
		A0.facenormal2 = adjfacenormal[0];

		B0.position = B.position;
		B0.normal = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		B0.facenormal1 = curfacenormal;
		B0.facenormal2 = adjfacenormal[0];

		C0 = A0;
		C0.normal = A.normal;

		D0 = B0;
		D0.normal = B.normal;

		*pEdgeVertices++ = A0;
		*pEdgeVertices++ = B0;
		*pEdgeVertices++ = C0;
		*pEdgeVertices++ = D0;


		//B->C形成的退化四边形
		EdgeVertex A1, B1, C1, D1;
		A1.position = B.position;
		A1.normal = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		A1.facenormal1 = curfacenormal;
		A1.facenormal2 = adjfacenormal[1];

		B1.position = C.position;
		B1.normal = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		B1.facenormal1 = curfacenormal;
		B1.facenormal2 = adjfacenormal[1];

		C1 = A1;
		C1.normal = B.normal;

		D1 = B1;
		D1.normal = C.normal;

		*pEdgeVertices++ = A1;
		*pEdgeVertices++ = B1;
		*pEdgeVertices++ = C1;
		*pEdgeVertices++ = D1;

		//C->A形成的退化四边形
		EdgeVertex A2, B2, C2, D2;
		A2.position = C.position;
		A2.normal = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		A2.facenormal1 = curfacenormal;
		A2.facenormal2 = adjfacenormal[1];

		B2.position = A.position;
		B2.normal = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		B2.facenormal1 = curfacenormal;
		B2.facenormal2 = adjfacenormal[1];

		C2 = A2;
		C2.normal = C.normal;

		D2 = B2;
		D2.normal = A.normal;

		*pEdgeVertices++ = A1;
		*pEdgeVertices++ = B1;
		*pEdgeVertices++ = C1;
		*pEdgeVertices++ = D1;

	}
	pVB->Unlock();
	pIB->Unlock();
	m_pVB->Unlock();

}

void SilhouetteEdge::GenEdgeIndices(ID3DXMesh* pMesh)
{
	DWORD dwEdgeNum = pMesh->GetNumFaces() * 3;
	m_dwFaceNum = dwEdgeNum * 2;
	m_dwIndexNum = m_dwFaceNum * 3;
	LRESULT hr = 0;
	hr = m_pDevice->CreateIndexBuffer(
		m_dwIndexNum*sizeof(DWORD),
		D3DUSAGE_WRITEONLY,
		D3DFMT_INDEX32,
		D3DPOOL_MANAGED,
		&m_pIB,
		NULL
		);
	if (FAILED(hr))
	{
		MessageBox(0, "Failed to create index buffer!", 0, 0);
		return;
	}
	DWORD* pIndices = NULL;
	m_pIB->Lock(0, 0, (void**)&pIndices, 0);

	for (int faceindex = 0; faceindex < pMesh->GetNumFaces(); faceindex++)
	{
		//第一条边的退化三角形顶点索引
		*pIndices++ = faceindex * 12 + 0;
		*pIndices++ = faceindex * 12 + 1;
		*pIndices++ = faceindex * 12 + 2;
		*pIndices++ = faceindex * 12 + 1;
		*pIndices++ = faceindex * 12 + 3;
		*pIndices++ = faceindex * 12 + 2;

		//第一条边的退化三角形顶点索引
		*pIndices++ = faceindex * 12 + 4 + 0;
		*pIndices++ = faceindex * 12 + 4 + 1;
		*pIndices++ = faceindex * 12 + 4 + 2;
		*pIndices++ = faceindex * 12 + 4 + 1;
		*pIndices++ = faceindex * 12 + 4 + 3;
		*pIndices++ = faceindex * 12 + 4 + 2;

		//第一条边的退化三角形顶点索引
		*pIndices++ = faceindex * 12 + 8 + 0;
		*pIndices++ = faceindex * 12 + 8 + 1;
		*pIndices++ = faceindex * 12 + 8 + 2;
		*pIndices++ = faceindex * 12 + 8 + 1;
		*pIndices++ = faceindex * 12 + 8 + 3;
		*pIndices++ = faceindex * 12 + 8 + 2;

	}
	m_pIB->Unlock();
}