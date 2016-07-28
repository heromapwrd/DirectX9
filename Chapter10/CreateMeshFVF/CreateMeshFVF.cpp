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
ID3DXMesh* pMesh =NULL;
const DWORD NumSubsets = 3;
IDirect3DTexture9* Textures[NumSubsets] = {NULL,NULL,NULL};
ofstream OutFile;

void dumpVertices(ofstream& OutFile, ID3DXMesh* pMesh);
void dumpIndex(ofstream& OutFile, ID3DXMesh* pMesh);
void dumpAttributeBuffer(ofstream& OutFile, ID3DXMesh* pMesh);
void dumpAdjacencyBuffer(ofstream& OutFile, ID3DXMesh* pMesh);
void dumpAttributeTable(ofstream& OutFile, ID3DXMesh* pMesh);


// 退出循环之后是清理工作
void CleanUp()
{
	for (int i = 0; i < NumSubsets;i++)
	{
		d3d9::Release<IDirect3DTexture9*>(Textures[i]);
	}
	d3d9::Release<ID3DXMesh*>(pMesh);
}

// 进入循环之前的设置
bool SetUp()
{
	HRESULT hr = 0;
	hr = D3DXCreateMeshFVF(
		12,
		24,
		D3DXMESH_MANAGED,
		Vertex::FVF,
		g_pDevice,
		&pMesh);

	if (FAILED(hr))
	{
		MessageBox(0, "Create Mesh Failed!", 0, 0);
		return false;
	}

	Vertex* v = 0;
	pMesh->LockVertexBuffer(0, (void**)&v);

	// fill in the front face vertex data
	v[0] = Vertex(-1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f);
	v[1] = Vertex(-1.0f, 1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
	v[2] = Vertex(1.0f, 1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f);
	v[3] = Vertex(1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);

	// fill in the back face vertex data
	v[4] = Vertex(-1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f);
	v[5] = Vertex(1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f);
	v[6] = Vertex(1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f);
	v[7] = Vertex(-1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f);

	// fill in the top face vertex data
	v[8] = Vertex(-1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f);
	v[9] = Vertex(-1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f);
	v[10] = Vertex(1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f);
	v[11] = Vertex(1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f);

	// fill in the bottom face vertex data
	v[12] = Vertex(-1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f);
	v[13] = Vertex(1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f);
	v[14] = Vertex(1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f);
	v[15] = Vertex(-1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f);

	// fill in the left face vertex data
	v[16] = Vertex(-1.0f, -1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	v[17] = Vertex(-1.0f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
	v[18] = Vertex(-1.0f, 1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f);
	v[19] = Vertex(-1.0f, -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f);

	// fill in the right face vertex data
	v[20] = Vertex(1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	v[21] = Vertex(1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
	v[22] = Vertex(1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f);
	v[23] = Vertex(1.0f, -1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f);

	pMesh->UnlockVertexBuffer();

	//
	// Define the triangles of the box
	//
	WORD* i = 0;
	pMesh->LockIndexBuffer(0, (void**)&i);

	// fill in the front face index data
	i[0] = 0; i[1] = 1; i[2] = 2;
	i[3] = 0; i[4] = 2; i[5] = 3;

	// fill in the back face index data
	i[6] = 4; i[7] = 5; i[8] = 6;
	i[9] = 4; i[10] = 6; i[11] = 7;

	// fill in the top face index data
	i[12] = 8; i[13] = 9; i[14] = 10;
	i[15] = 8; i[16] = 10; i[17] = 11;

	// fill in the bottom face index data
	i[18] = 12; i[19] = 13; i[20] = 14;
	i[21] = 12; i[22] = 14; i[23] = 15;

	// fill in the left face index data
	i[24] = 16; i[25] = 17; i[26] = 18;
	i[27] = 16; i[28] = 18; i[29] = 19;

	// fill in the right face index data
	i[30] = 20; i[31] = 21; i[32] = 22;
	i[33] = 20; i[34] = 22; i[35] = 23;

	pMesh->UnlockIndexBuffer();

	//
	// Specify the subset each triangle belongs to, in this example
	// we will use three subsets, the first two faces of the cube specified
	// will be in subset 0, the next two faces will be in subset 1 and
	// the the last two faces will be in subset 2.
	//
	DWORD* attributeBuffer = 0;
	pMesh->LockAttributeBuffer(0, &attributeBuffer);

	for (int a = 0; a < 4; a++)
		attributeBuffer[a] = 0;

	for (int b = 4; b < 8; b++)
		attributeBuffer[b] = 1;

	for (int c = 8; c < 12; c++)
		attributeBuffer[c] = 2;

	pMesh->UnlockAttributeBuffer();

	vector<DWORD> adjancencyBuffer(pMesh->GetNumFaces() * 3);
	pMesh->GenerateAdjacency(0.0f, &adjancencyBuffer[0]);

	hr = pMesh->OptimizeInplace(
		D3DXMESHOPT_ATTRSORT | D3DXMESHOPT_COMPACT | D3DXMESHOPT_VERTEXCACHE,
		&adjancencyBuffer[0],
		0,
		0,
		0);
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

	D3DXCreateTextureFromFile(
		g_pDevice,
		"brick0.jpg",
		&Textures[0]);
	D3DXCreateTextureFromFile(
		g_pDevice,
		"brick1.jpg",
		&Textures[1]);
	D3DXCreateTextureFromFile(
		g_pDevice,
		"checker.jpg",
		&Textures[2]);

	// 纹理采样设置
	g_pDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	g_pDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	g_pDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

	g_pDevice->SetRenderState(D3DRS_LIGHTING, false);

	// 初始化视觉矩阵,投影矩阵
	// 相机位置,视点位置,竖直向上向量
	D3DXVECTOR3 eye(-5.0f, 3.0f, -5.0f);
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
		for (int i = 0; i < NumSubsets; i++)
		{
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