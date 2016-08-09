#include "Terrain.h"
#include "Vertex.h"
#include<fstream>
#include<cmath>
using std::ifstream;

Terrain::Terrain(
	IDirect3DDevice9* pDevice,
	string sFileName,
	int iVertsPerRow,
	int iVertsPerCol,
	int iCellSpacing,
	float fScale
	)
{
	m_pDevice = pDevice;
	m_sHeightMapFileName = sFileName;
	m_nVertsPerRow = iVertsPerRow;
	m_nVertsPerCol = iVertsPerCol;
	m_nCellSpacing = iCellSpacing;
	m_fHeightScale = fScale;

	m_nCellPerRow = m_nVertsPerRow - 1;
	m_nCellPerCol = m_nVertsPerCol - 1;
	m_nWitdh = m_nCellPerRow*m_nCellSpacing;
	m_nHeight = m_nCellPerCol*m_nCellSpacing;
	m_nNumVertices = m_nVertsPerRow*m_nVertsPerCol;
	m_nNumTriangles = m_nCellPerRow*m_nCellPerCol * 2;

	if (!ReadHeightMapFile(m_sHeightMapFileName))
	{
		MessageBox(0, "Read height map failed!", 0, 0);
		PostQuitMessage(0);
		return;
	}
	for (int i = 0; i < m_vHeightMap.size(); i++)
	{
		m_vHeightMap[i] *= m_fHeightScale;
	}

	if (!ComputeVertices())
	{
		MessageBox(0, "Failed to Compute Vertices!", 0, 0);
		PostQuitMessage(0);
		return;
	}
	if (!ComputeIndices())
	{
		MessageBox(0, "Failed to Compute Indices!", 0, 0);
		PostQuitMessage(0);
		return;
	}
}

Terrain::~Terrain()
{
	d3d9::Release(m_pVB);
	d3d9::Release(m_pIB);
	d3d9::Release(m_pTex);
}

int Terrain::GetVertexHeight(int row, int col)
{
	int index = row*m_nVertsPerRow + col;
	if (index<0)
	{
		return m_vHeightMap[0];
	}
	else if (index >= m_vHeightMap.size())
	{
		return m_vHeightMap[m_vHeightMap.size() - 1];
	}
	else
	{
		return m_vHeightMap[index];
	}
}

void Terrain::SetVertexHeight(int row, int col, int value)
{
	int index = row*m_nVertsPerRow + col;
	if (index < 0 || index >= m_vHeightMap.size())
		return;
	m_vHeightMap[index] = value;
}

float Terrain::GetHeight(float x, float z)
{
	x = (float)m_nWitdh / 2.0f + x;
	z = (float)m_nHeight / 2.0f - z;

	x /= m_nCellSpacing;
	z /= m_nCellSpacing;

	int row = floor(z);
	int col = floor(x);

	float A = GetVertexHeight(row, col);
	float B = GetVertexHeight(row, col + 1);
	float C = GetVertexHeight(row + 1, col);
	float D = GetVertexHeight(row + 1, col + 1);

	float dx = x - col;
	float dz = z - row;
	float height = 0.0f;
	if (dx + dz > 1.0f)
	{
		float uy = C - D;
		float vy = B - D;
		D + d3d9::Lerp(0, uy, 1 - dx) + d3d9::Lerp(0, vy, 1 - dz);
	}
	else
	{
		float uy = B - A;
		float vy = C - A;
		height = A + d3d9::Lerp(0, uy, dx) + d3d9::Lerp(0, vy, dz);
	}
	return height;
}

bool Terrain::LoadTexture(string filename)
{
	HRESULT hr = 0;
	hr = D3DXCreateTextureFromFile(m_pDevice, filename.c_str(), &m_pTex);
	if (FAILED(hr))
	{
		MessageBox(0, "Failed to load texture!", 0, 0);
		return false;
	}
	return true;
}

bool Terrain::GenerateTexture(D3DXVECTOR3* pDirToLight)
{
	LRESULT hr = 0;
	int iTexWidth = m_nCellPerRow;
	int iTexHeight = m_nCellPerCol;
	hr = D3DXCreateTexture(
		m_pDevice,
		iTexWidth,
		iTexHeight,
		0,
		0,
		D3DFMT_X8R8G8B8,
		D3DPOOL_MANAGED,
		&m_pTex
		);
	if (FAILED(hr))
	{
		MessageBox(0, "Generate Texture failed!", 0, 0);
		return false;
	}

	D3DSURFACE_DESC teturedesc;
	m_pTex->GetLevelDesc(0, &teturedesc);
	if (teturedesc.Format != D3DFMT_X8R8G8B8)
		return false;

	DWORD* pTex = NULL;
	D3DLOCKED_RECT lockRecked;
	m_pTex->LockRect(0, &lockRecked, 0, 0);
	pTex = (DWORD*)lockRecked.pBits;
	for (int row = 0; row < iTexHeight; row++)
	{
		for (int col = 0; col < iTexWidth; col++)
		{
			D3DXCOLOR color;
			int index = row*m_nVertsPerRow + col;
			float height = m_vHeightMap[index];
			height /= m_fHeightScale;

			if (height < 42.5f)				color = d3d9::BEACH_SAND;
			else if (height < 85.0f)		color = d3d9::LIGHT_YELLOW_GREEN;
			else if (height < 127.5f)		color = d3d9::PUREGREEN;
			else if (height < 170.0f)		color = d3d9::DARK_YELLOW_GREEN;
			else if (height < 212.5f)		color = d3d9::DARK_BROWN;
			else                            color = d3d9::WHITE_COLOR;
			
			pTex[row*lockRecked.Pitch / sizeof(DWORD)+col] = (D3DCOLOR)color;
		}
	}

	m_pTex->UnlockRect(0);

	if (!LightTerrain(pDirToLight))
	{
		MessageBox(0, "Failed to light terrain!", 0, 0);
		return false;
	}

	hr = D3DXFilterTexture(m_pTex, NULL, 0, D3DX_DEFAULT);
	if (FAILED(hr))
	{
		MessageBox(0, "Failed to filter texture!", 0, 0);
		return false;
	}
	return true;
}

bool Terrain::Draw(D3DXMATRIX* pWorld, bool bDrawTris /*= false*/)
{
	if (m_pDevice)
	{
		HRESULT hr = 0;
		m_pDevice->SetTransform(D3DTS_WORLD, pWorld);
		m_pDevice->SetStreamSource(0, m_pVB, 0, sizeof(Vertex));
		m_pDevice->SetFVF(Vertex::FVF);
		m_pDevice->SetIndices(m_pIB);
		m_pDevice->SetTexture(0, m_pTex);
		m_pDevice->SetRenderState(D3DRS_LIGHTING, false);

		hr = m_pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, m_nNumVertices, 0, m_nNumTriangles);
		if (FAILED(hr))
		{
			MessageBox(0, "Draw scene failed!", 0, 0);
			return false;
		}

		if (bDrawTris)
		{
			m_pDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
			hr = m_pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, m_nNumVertices, 0, m_nNumTriangles);
			m_pDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
			if (FAILED(hr))
			{
				MessageBox(0, "Failed to draw triangles!", 0, 0);
				return false;
			}
		}
	}
	return true;
}
bool Terrain::ReadHeightMapFile(string filename)
{
	vector<BYTE> heightmap(m_nNumVertices);
	ifstream inFile;
	inFile.open(filename.c_str(), std::ios_base::binary);
	if (!inFile.is_open())
		return false;
	inFile.read((char*)(&heightmap[0]), heightmap.size());
	inFile.close();
	m_vHeightMap.resize(m_nNumVertices);
	for (int i = 0; i < heightmap.size(); i++)
	{
		m_vHeightMap[i] = heightmap[i];
	}
	return true;
}

bool Terrain::ComputeVertices()
{
	HRESULT hr = 0;
	hr = m_pDevice->CreateVertexBuffer(
		m_nNumVertices*sizeof(Vertex),
		D3DUSAGE_WRITEONLY,
		Vertex::FVF,
		D3DPOOL_MANAGED,
		&m_pVB,
		NULL
		);
	if (FAILED(hr))
	{
		MessageBox(0, "Failed to create vertex buff!", 0, 0);
		return false;
	}

	int StartX = -m_nWitdh / 2;
	int StartZ = m_nHeight / 2;
	int EndX = m_nWitdh / 2;
	int EndZ = -m_nHeight / 2;
	float uIncrement = 1.0f / m_nCellPerRow;
	float vIncrement = 1.0f / m_nCellPerCol;
	
	Vertex *pVB;
	m_pVB->Lock(0, 0, (void**)(&pVB), 0);
	int row = 0;
	for (int z = StartZ; z >= EndZ; z -= m_nCellSpacing)
	{
		int col = 0;
		for (int x = StartX; x <= EndX; x += m_nCellSpacing)
		{
			int index = row*m_nVertsPerRow + col;
			pVB[index].x = x;
			pVB[index].y = m_vHeightMap[index];
			pVB[index].z = z;
			pVB[index].u = col*uIncrement;
			pVB[index].v = row*vIncrement;
			col++;
		}
		row++;
	}
	m_pVB->Unlock();
	return true;
}

bool Terrain::ComputeIndices()
{
	HRESULT hr = 0;
	hr = m_pDevice->CreateIndexBuffer(
		m_nNumTriangles * 3 * sizeof(DWORD),
		D3DUSAGE_WRITEONLY,
		D3DFMT_INDEX32,
		D3DPOOL_MANAGED,
		&m_pIB,
		NULL
		);
	if (FAILED(hr))
	{
		MessageBox(0, "Failed to create index buff!", 0, 0);
		return false;
	}

	DWORD* pIB = NULL;
	m_pIB->Lock(0, 0, (void**)(&pIB), 0);
	int baseindex = 0;
	for (int row = 0; row < m_nCellPerCol; row++)
	{
		for (int col = 0; col < m_nCellPerRow;col++)
		{
			pIB[baseindex] = row*m_nVertsPerRow + col;
			pIB[baseindex + 1] = row*m_nVertsPerRow + col + 1;
			pIB[baseindex + 2] = (row+1)*m_nVertsPerRow + col;

			pIB[baseindex + 3] = (row + 1)*m_nVertsPerRow + col;
			pIB[baseindex + 4] = row*m_nVertsPerRow + col + 1;
			pIB[baseindex + 5] = (row + 1)*m_nVertsPerRow + col + 1;
			baseindex += 6;
		}
	}
	m_pIB->Unlock();
	return true;
}

float Terrain::ComputeShade(int cellrow, int cellcol, D3DXVECTOR3* pDirToLight)
{
	float heightA = GetVertexHeight(cellrow, cellcol);
	float heightB = GetVertexHeight(cellrow, cellcol + 1);
	float heightC = GetVertexHeight(cellrow + 1, cellcol);

	D3DXVECTOR3 u = D3DXVECTOR3(m_nCellSpacing, heightB - heightA, 0);
	D3DXVECTOR3 v = D3DXVECTOR3(0, heightC - heightA, -m_nCellSpacing);
	D3DXVECTOR3 n;
	D3DXVec3Cross(&n, &u, &v);
	D3DXVec3Normalize(&n, &n);
	float cosin = D3DXVec3Dot(pDirToLight, &n);
	if (cosin< 0.0f)
	{
		cosin = 0.0f;
	}
	return cosin;
}

bool Terrain::LightTerrain(D3DXVECTOR3* pDirToLight)
{
	D3DSURFACE_DESC desc;
	m_pTex->GetLevelDesc(0, &desc);
	if (desc.Format != D3DFMT_X8R8G8B8)
		return false;
	D3DLOCKED_RECT lockRecked;
	m_pTex->LockRect(0, &lockRecked, 0, 0);
	DWORD* pTex = NULL;
	pTex = (DWORD*)lockRecked.pBits;
	for (int row = 0; row < desc.Height; row++)
	{
		for (int col = 0; col < desc.Width; col++)
		{
			D3DXCOLOR color;
			color = (D3DXCOLOR)pTex[row*lockRecked.Pitch / sizeof(DWORD)+col];
			float shade = ComputeShade(row, col, pDirToLight);
			color *= shade;
			pTex[row*lockRecked.Pitch / sizeof(DWORD)+col] = (D3DCOLOR)color;
		}
	}

	m_pTex->UnlockRect(0);
	return true;
}