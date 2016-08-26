#include "D3DApp.h"
#include "D3DCommon.h"
#include "FPS.h"


//-----------------------------------------------------------------
//							全局变量
//-----------------------------------------------------------------
D3DFPS* g_pFPS = NULL;
int g_WndWidth = 640;
int g_WndHeight = 480;
IDirect3DDevice9* g_pDevice = NULL;
D3DDEVTYPE g_DevType = D3DDEVTYPE_HAL;
HWND g_hWnd = NULL;

IDirect3DVertexBuffer9* VB = NULL;
IDirect3DIndexBuffer9 * IB = NULL;


//------------------------------------------------------------------
//							 实现
//------------------------------------------------------------------

HRESULT SetUp()
{
	//// 初始化视觉矩阵
	//D3DXVECTOR3 eye(0.0f, 0.0f, -5.0f);					// 相机位置
	//D3DXVECTOR3 lookat(0.0f, 0.0f, 0.0f);				// 被视点位置
	//D3DXVECTOR3 Up(0.0f, 1.0f, 0.0f);					// 竖直向上向量
	//D3DXMATRIX view;
	//D3DXMatrixLookAtLH(&view, &eye, &lookat, &Up);
	//g_pDevice->SetTransform(D3DTS_VIEW, &view);


	//// 设置投影矩阵
	//D3DVIEWPORT9 viewport;
	//HRESULT hr = g_pDevice->GetViewport(&viewport);
	//if (FAILED(hr))
	//	return hr;

	//float Aspect = (float)viewport.Width / (float)viewport.Height;
	//D3DXMATRIX matProjection;
	//D3DXMatrixPerspectiveFovLH(&matProjection, D3DX_PI / 4.0f, Aspect, 1.0f, 1000.0f);
	//g_pDevice->SetTransform(D3DTS_PROJECTION, &matProjection);

	//// 设置默认渲染状态
	//g_pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
	//g_pDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
	//g_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	//g_pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

	//// 设置默认纹理融合态
	//g_pDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	//g_pDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
	//g_pDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);

	//// 设置默认纹理采样器
	//g_pDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	//g_pDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	//g_pDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);


	// 创建顶点缓存
	g_pDevice->CreateVertexBuffer(
		8 * sizeof(Vertex),
		D3DUSAGE_WRITEONLY,
		Vertex::FVF,
		D3DPOOL_MANAGED,
		&VB,
		0
		);
	// 创建索引缓存
	g_pDevice->CreateIndexBuffer(
		36 * sizeof(DWORD),
		D3DUSAGE_WRITEONLY,
		D3DFMT_INDEX32,
		D3DPOOL_MANAGED,
		&IB,
		0
		);

	// 初始化顶点缓存
	Vertex* pVertex;
	VB->Lock(0, 0, (void**)&pVertex, 0);
	pVertex[0] = Vertex(-1.0f, -1.0f, -1.0f, WHITE_COLOR);
	pVertex[1] = Vertex(-1.0f, 1.0f, -1.0f, RED_COLOR);
	pVertex[2] = Vertex(1.0f, 1.0f, -1.0f, GREEN_COLOR);
	pVertex[3] = Vertex(1.0f, -1.0f, -1.0f, BLUE_COLOR);
	pVertex[4] = Vertex(-1.0f, -1.0f, 1.0f, YELLOW_COLOR);
	pVertex[5] = Vertex(-1.0f, 1.0f, 1.0f, CYAN_COLOR);
	pVertex[6] = Vertex(1.0f, 1.0f, 1.0f, BLACK_COLOR);
	pVertex[7] = Vertex(1.0f, -1.0f, 1.0f, MAGENTA_COLOR);
	VB->Unlock();

	// 初始化索引缓存
	DWORD* pIndex;
	IB->Lock(0, 0, (void**)&pIndex, 0);
	// 前面
	pIndex[0] = 0; pIndex[1] = 1; pIndex[2] = 2;
	pIndex[3] = 0; pIndex[4] = 2; pIndex[5] = 3;

	// 后面
	pIndex[6] = 4; pIndex[7] = 6; pIndex[8] = 5;
	pIndex[9] = 4; pIndex[10] = 7; pIndex[11] = 6;

	// 左面
	pIndex[12] = 4; pIndex[13] = 5; pIndex[14] = 1;
	pIndex[15] = 4; pIndex[16] = 1; pIndex[17] = 0;

	// 右面
	pIndex[18] = 3; pIndex[19] = 2; pIndex[20] = 6;
	pIndex[21] = 3; pIndex[22] = 6; pIndex[23] = 7;

	// 上面
	pIndex[24] = 1; pIndex[25] = 5; pIndex[26] = 6;
	pIndex[27] = 1; pIndex[28] = 6; pIndex[29] = 2;

	// 下面
	pIndex[30] = 4; pIndex[31] = 0; pIndex[32] = 3;
	pIndex[33] = 4; pIndex[34] = 3; pIndex[35] = 7;

	IB->Unlock();

	// 初始化视觉矩阵,投影矩阵
	// 相机位置,视点位置,竖直向上向量
	D3DXVECTOR3 eye(0.0f, 0.0f, -5.0f);
	D3DXVECTOR3 lookat(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 Up(0.0f, 1.0f, 0.0f);
	D3DXMATRIX view;
	D3DXMatrixLookAtLH(&view, &eye, &lookat, &Up);
	g_pDevice->SetTransform(D3DTS_VIEW, &view);

	// 竖直视角,宽/高比,近裁剪面,远裁剪面
	D3DXMATRIX proj;
	D3DXMatrixPerspectiveFovLH(&proj, D3DX_PI*0.5f, (float)g_WndWidth / (float)g_WndHeight, 1.0f, 1000.0f);
	g_pDevice->SetTransform(D3DTS_PROJECTION, &proj);

	// 设置图元绘制方式
	//g_pDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	g_pDevice->SetRenderState(D3DRS_LIGHTING, false);
	g_pFPS = new D3DFPS(g_pDevice);
	if (!g_pFPS)
	{
		MessageBox(0, "Failed to create FPS Counter!", 0, 0);
		return E_FAIL;
	}

	return S_OK;
}

void CleanUp()
{
	Delete<D3DFPS*>(g_pFPS);
	Release<IDirect3DDevice9*>(g_pDevice);
	Release<IDirect3DVertexBuffer9*>(VB);
	Release<IDirect3DIndexBuffer9*>(IB);
}

HRESULT LoadContent()
{
	return S_OK;
}

void UnLoadContent()
{

}

bool Tick(DWORD deltatimes)
{
	return true;
}

bool Render(DWORD deltatimes)
{
	if (!g_pDevice)
		return false;

	D3DXMATRIX Rx, Ry;
	D3DXMatrixRotationX(&Rx, D3DX_PI / 4.0f);

	static float y = 0.0f;
	D3DXMatrixRotationY(&Ry, y);
	float delta = (float)deltatimes / 1000.0f;
	y += delta;
	if (y > 2 * D3DX_PI)
		y = 0.0f;

	D3DXMATRIX Translation;
	D3DXMatrixTranslation(&Translation, 0.0f, 0.0f, 5.0f);
	D3DXMATRIX world = /*Rx**/Ry*Translation;
	g_pDevice->SetTransform(D3DTS_WORLD, &world);

	/*D3DXMATRIX I;
	D3DXMatrixIdentity(&I);
	g_pDevice->SetTransform(D3DTS_WORLD, &I);*/


	// Render Begin
	g_pDevice->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xff000000, 1.0f, 0);
	g_pDevice->BeginScene();

	// Render Cube
	g_pDevice->SetStreamSource(0, VB, 0, sizeof(Vertex));
	g_pDevice->SetIndices(IB);
	g_pDevice->SetFVF(Vertex::FVF);
	g_pDevice->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_GOURAUD);
	g_pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 8, 0, 12);

	// Render FPS
	g_pFPS->Render(0xffffff00);

	// Render End
	g_pDevice->EndScene();
	g_pDevice->Present(0, 0, 0, 0);
	return true;
}


int EnterMsgLoop()
{
	MSG msg;
	memset(&msg, 0, sizeof(msg));
	g_pFPS->Start();
	while (true)
	{
		while (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
				break;
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if (msg.message == WM_QUIT)
			break;
		
		DWORD deltatimes;
		g_pFPS->Tick(deltatimes);
		if (!Tick(deltatimes))
			break;
		if (!Render(deltatimes))
			break;
	}

	return msg.wParam;
}

HRESULT InitInstance(HINSTANCE hInstance, int Width, int Height, bool bWindowed, bool bMultiThread, D3DDEVTYPE type, IDirect3DDevice9** ppDevice)
{
	// 创建窗口类
	WNDCLASS wc;
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = (WNDPROC)WndProc;
	wc.hInstance = hInstance;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hIcon = LoadIcon(0, IDI_APPLICATION);
	wc.hCursor = LoadCursor(0, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName = 0;
	wc.lpszClassName = "D3DApp";

	// 注册窗口类
	if (!RegisterClass(&wc))
	{
		::MessageBox(NULL, "Register Class Faild!", 0, 0);
		return E_FAIL;
	}

	// 创建窗口
	HWND hWnd = NULL;
	hWnd = CreateWindow(
		"D3DApp",
		"MiniEngine",
		WS_OVERLAPPEDWINDOW,
		20, 20, g_WndWidth, g_WndHeight,
		NULL,
		NULL,
		hInstance,
		NULL
		);
	if (!hWnd)
	{
		MessageBox(NULL, "Create Window Failed!", 0, 0);
		return E_FAIL;
	}
	
	//HACCEL hAccelTable;
	//// 初始化全局字符串
	//LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	//LoadString(hInstance, IDC_KEN3DGAME, szWindowClass, MAX_LOADSTRING);
	//MyRegisterClass(hInstance);
	//hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_KEN3DGAME));

	// 初始化D3D
	LRESULT hr = 0;
	hr = InitD3D(hWnd, g_WndWidth, g_WndHeight, bWindowed, bMultiThread, type, ppDevice);
	if (FAILED(hr))
	{
		Release<IDirect3DDevice9*>(*ppDevice);
		MessageBox(0, "Failed to init d3d!", 0, 0);
		return E_FAIL;
	}

	// 显示和更新窗口
	ShowWindow(hWnd, SW_SHOW);
	UpdateWindow(hWnd);

	return S_OK;
}


LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_KEYDOWN:
	{
					if (wParam == VK_ESCAPE)
						DestroyWindow(hWnd);
					break;
	}
	default:
		break;
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}


int __stdcall WinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPSTR lpCmdLine,
	_In_ int nShowCmd)
{
	// 初始化Instance,创建窗口,初始化d3d工作
	HRESULT hr = 0;
	hr = InitInstance(hInstance, g_WndWidth, g_WndHeight, true, false, g_DevType, &g_pDevice);
	if (FAILED(hr))
	{
		MessageBox(0, "Faile to init instance!", 0, 0);
		return 0 ;
	}

	// 保存窗口句柄
	g_hWnd = GetActiveWindow();
	if (!g_hWnd)
	{
		MessageBox(0, "Failed to get windows handle!", 0, 0);
		return 0;
	}

	// 游戏设置
	hr = SetUp();
	if (FAILED(hr))
		goto Clean;
	// 加载游戏内容
	hr = LoadContent();
	if (FAILED(hr))
		goto UnLoad;
	// 游戏循环
	EnterMsgLoop();
	// 卸载游戏内容
UnLoad:	UnLoadContent();
	// 清除
Clean:	CleanUp();
	return 0;
}