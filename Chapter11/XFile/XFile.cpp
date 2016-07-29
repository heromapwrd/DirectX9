#include "d3dUtility.h"
#include "Vertex.h"
#include<fstream>
#include<vector>
using namespace std;
using std::vector;
using std::ofstream;

IDirect3DDevice9* g_pDevice = NULL;
int Width = 640;
int Height = 480;
ID3DXMesh* pMesh = NULL;
vector<D3DMATERIAL9> Mtrls(0);
vector<IDirect3DTexture9*> Textures(0);
ofstream OutFile;

void dumpVertices(ofstream& OutFile, ID3DXMesh* pMesh);
void dumpIndex(ofstream& OutFile, ID3DXMesh* pMesh);
void dumpAttributeBuffer(ofstream& OutFile, ID3DXMesh* pMesh);
void dumpAdjacencyBuffer(ofstream& OutFile, ID3DXMesh* pMesh);
void dumpAttributeTable(ofstream& OutFile, ID3DXMesh* pMesh);


// 退出循环之后是清理工作
void CleanUp()
{
	for (int i = 0; i < Textures.size(); i++)
	{
		d3d9::Release<IDirect3DTexture9*>(Textures[i]);
	}
	d3d9::Release<ID3DXMesh*>(pMesh);
}

// 进入循环之前的设置
bool SetUp()
{
	HRESULT hr = 0;

	ID3DXBuffer* pAdjBuffer = NULL;
	ID3DXBuffer* pMtrlBuffer = NULL;
	DWORD dMtrlNum = 0;

	hr = D3DXLoadMeshFromX(
		"bigship1.x", 
		D3DXMESH_MANAGED, 
		g_pDevice, 
		&pAdjBuffer, 
		&pMtrlBuffer, 
		NULL, 
		&dMtrlNum, 
		&pMesh
		);

	if (FAILED(hr))
	{
		MessageBox(0, "Create Mesh From XFile Failed!", 0, 0);
		return false;
	}

	if (NULL!=pMtrlBuffer && 0 != dMtrlNum)
	{
		D3DXMATERIAL* pMtrl = (D3DXMATERIAL*)pMtrlBuffer->GetBufferPointer();
		for (int i = 0; i < dMtrlNum; i++)
		{
			pMtrl[i].MatD3D.Ambient = pMtrl[i].MatD3D.Diffuse;
			Mtrls.push_back(pMtrl[i].MatD3D);

			if (pMtrl[i].pTextureFilename != NULL)
			{
				IDirect3DTexture9* pTex = NULL;
				hr = D3DXCreateTextureFromFile(
					g_pDevice, 
					pMtrl[i].pTextureFilename, 
					&pTex
					);
				if (FAILED(hr))
				{
					pMtrlBuffer->Release();
					MessageBox(0, "Failed to create texture from file!", 0, 0);
					return false;
				}

				Textures.push_back(pTex);
			}
			else
			{
				Textures.push_back(NULL);
			}
			
		}
	}
	pMtrlBuffer->Release();

	hr = pMesh->OptimizeInplace(
		D3DXMESHOPT_ATTRSORT | D3DXMESHOPT_COMPACT | D3DXMESHOPT_VERTEXCACHE,
		(DWORD*)pAdjBuffer->GetBufferPointer(),
		0,
		0,
		0);
	pAdjBuffer->Release();
	if (FAILED(hr))
	{
		MessageBox(0, "Optimize Mesh Failed!", 0, 0);
		return false;
	}

	OutFile.open("Mesh_Dump.txt");

	dumpVertices(OutFile, pMesh);
	dumpIndex(OutFile, pMesh);
	dumpAttributeBuffer(OutFile, pMesh);
	dumpAdjacencyBuffer(OutFile, pMesh);
	dumpAttributeTable(OutFile, pMesh);

	OutFile.close();

	// 纹理采样设置
	g_pDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	g_pDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	g_pDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_POINT);

	g_pDevice->SetRenderState(D3DRS_LIGHTING, true);

	D3DXVECTOR3 dir(1.0f, -1.0f, 1.0f);
	D3DLIGHT9 Light = d3d9::InitDirLight(&dir, &d3d9::WHITE_COLOR);
	g_pDevice->SetLight(0, &Light);
	g_pDevice->LightEnable(0, true);
	g_pDevice->SetRenderState(D3DRS_NORMALIZENORMALS, true);
	g_pDevice->SetRenderState(D3DRS_SPECULARENABLE, true);

	// 初始化视觉矩阵,投影矩阵
	// 相机位置,视点位置,竖直向上向量
	D3DXVECTOR3 eye(-25.0f, 3.0f, -25.0f);
	D3DXVECTOR3 lookat(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 Up(0.0f, 1.0f, 0.0f);
	D3DXMATRIX view;
	D3DXMatrixLookAtLH(&view, &eye, &lookat, &Up);
	g_pDevice->SetTransform(D3DTS_VIEW, &view);

	// 竖直视角,宽/高比,近裁剪面,远裁剪面
	D3DXMATRIX proj;
	D3DXMatrixPerspectiveFovLH(&proj, D3DX_PI*0.5f, (float)Width / (float)Height, 1.0f, 1000.0f);
	g_pDevice->SetTransform(D3DTS_PROJECTION, &proj);

	return true;
}

bool DisPlay(DWORD timeDelta)
{
	float ftimeDleta = (float)timeDelta / 1000.0f;
	if (g_pDevice)
	{
		D3DXMATRIX Rx, Ry;
		D3DXMatrixRotationX(&Rx, D3DX_PI / 4.0f);

		static float y = 0.0f;
		D3DXMatrixRotationY(&Ry, y);
		float delta = (float)timeDelta / 1000.0f;
		y += delta;
		if (y > 2 * D3DX_PI)
			y = 0.0f;
		D3DXMATRIX world = Rx*Ry;
		g_pDevice->SetTransform(D3DTS_WORLD, &world);

		g_pDevice->Clear(0, 0,
			D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL,
			0xff000000, 1.0f, 0);

		g_pDevice->BeginScene();
		for (int i = 0; i < Textures.size(); i++)
		{
			g_pDevice->SetMaterial(&Mtrls[i]);
			g_pDevice->SetTexture(0, Textures[i]);
			pMesh->DrawSubset(i);
		}

		g_pDevice->EndScene();
		g_pDevice->Present(0, 0, 0, 0);
	}
	return true;
}

LRESULT CALLBACK d3d9::WndProc(
	HWND hWnd,
	UINT msg,
	WPARAM wParam,
	LPARAM lParam)
{
	switch (msg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE)
			DestroyWindow(hWnd);
		break;
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

int WINAPI WinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPSTR lpCmdLine,
	_In_ int nShowCmd)
{
	if (!d3d9::Init(hInstance, 640, 480, true, D3DDEVTYPE_HAL, &g_pDevice))
	{
		MessageBox(0, "Init Failed!", 0, 0);
		return 0;
	}
	if (!SetUp())
	{
		MessageBox(0, "SetUp Failed!", 0, 0);
		return 0;
	}
	d3d9::EnterMsgLoop(DisPlay);
	CleanUp();
	g_pDevice->Release();
	return 0;
}

void dumpVertices(ofstream& OutFile, ID3DXMesh* pMesh)
{
	OutFile << "Vertex:" << endl;
	OutFile << "-------" << endl << endl;
	Vertex* v;
	pMesh->LockVertexBuffer(0, (void**)&v);
	for (int i = 0; i < pMesh->GetNumVertices(); i++)
	{
		OutFile << "Vertex" << i << ": (";
		OutFile << v[i].x << "," << v[i].y << "," << v[i].z << ",";
		OutFile << v[i].nx << "," << v[i].ny << "," << v[i].nz << ",";
		OutFile << v[i].u << "," << v[i].v << ")" << endl;
	}
	pMesh->UnlockVertexBuffer();
	OutFile << endl << endl;
}

void dumpIndex(ofstream& OutFile, ID3DXMesh* pMesh)
{
	OutFile << "Indices:" << std::endl;
	OutFile << "--------" << std::endl << std::endl;

	WORD* indices = 0;
	pMesh->LockIndexBuffer(0, (void**)&indices);

	for (int i = 0; i < pMesh->GetNumFaces(); i++)
	{
		OutFile << "Triangle " << i << ": ";
		OutFile << indices[i * 3] << " ";
		OutFile << indices[i * 3 + 1] << " ";
		OutFile << indices[i * 3 + 2] << std::endl;
	}
	pMesh->UnlockIndexBuffer();

	OutFile << std::endl << std::endl;
}

void dumpAttributeBuffer(ofstream& OutFile, ID3DXMesh* pMesh)
{
	OutFile << "Attribute Buffer:" << std::endl;
	OutFile << "-----------------" << std::endl << std::endl;

	DWORD* attributeBuffer = 0;
	pMesh->LockAttributeBuffer(0, &attributeBuffer);

	// an attribute for each face
	for (int i = 0; i < pMesh->GetNumFaces(); i++)
	{
		OutFile << "Triangle lives in subset " << i << ": ";
		OutFile << attributeBuffer[i] << std::endl;
	}
	pMesh->UnlockAttributeBuffer();

	OutFile << std::endl << std::endl;
}

void dumpAdjacencyBuffer(ofstream& OutFile, ID3DXMesh* pMesh)
{
	OutFile << "Adjacency Buffer:" << std::endl;
	OutFile << "-----------------" << std::endl << std::endl;

	// three enttries per face
	std::vector<DWORD> adjacencyBuffer(pMesh->GetNumFaces() * 3);

	pMesh->GenerateAdjacency(0.0f, &adjacencyBuffer[0]);

	for (int i = 0; i < pMesh->GetNumFaces(); i++)
	{
		OutFile << "Triangle's adjacent to triangle " << i << ": ";
		OutFile << adjacencyBuffer[i * 3] << " ";
		OutFile << adjacencyBuffer[i * 3 + 1] << " ";
		OutFile << adjacencyBuffer[i * 3 + 2] << std::endl;
	}

	OutFile << std::endl << std::endl;
}

void dumpAttributeTable(ofstream& OutFile, ID3DXMesh* pMesh)
{
	OutFile << "Attribute Table:" << std::endl;
	OutFile << "----------------" << std::endl << std::endl;

	// number of entries in the attribute table
	DWORD numEntries = 0;

	pMesh->GetAttributeTable(0, &numEntries);

	if (numEntries <= 0)
		return;
	std::vector<D3DXATTRIBUTERANGE> table(numEntries);

	pMesh->GetAttributeTable(&table[0], &numEntries);

	for (int i = 0; i < numEntries; i++)
	{
		OutFile << "Entry " << i << std::endl;
		OutFile << "-----------" << std::endl;

		OutFile << "Subset ID:    " << table[i].AttribId << std::endl;
		OutFile << "Face Start:   " << table[i].FaceStart << std::endl;
		OutFile << "Face Count:   " << table[i].FaceCount << std::endl;
		OutFile << "Vertex Start: " << table[i].VertexStart << std::endl;
		OutFile << "Vertex Count: " << table[i].VertexCount << std::endl;
		OutFile << std::endl;
	}

	OutFile << std::endl << std::endl;
}