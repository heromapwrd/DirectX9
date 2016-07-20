/*______________________________________________________________

Desc: Demonstrates mirrors with stencils.  Use the arrow keys
and the 'A' and 'S' key to navigate the scene and translate the teapot.
*/

#include "d3dUtility.h"
IDirect3DDevice9* g_pDevice = 0;

const int Width = 640;
const int Height = 480;

IDirect3DVertexBuffer9* VB = 0;

IDirect3DTexture9* FloorTex = 0;
IDirect3DTexture9* WallTex = 0;
IDirect3DTexture9* MirrorTex = 0;

D3DMATERIAL9 FloorMtrl;
D3DMATERIAL9 WallMtrl;
D3DMATERIAL9 MirrorMtrl;

ID3DXMesh* Teapot = 0;
D3DXVECTOR3 TeapotPosition;
D3DMATERIAL9 TeapotMtrl;

void RenderScene();
void RenderMirror();
void RenderShadow();

struct Vertex
{
	Vertex(){}
	Vertex(float _x, float _y, float _z, float _nx, float _ny, float _nz, float _u, float _v)
	{
		x = _x;
		y = _y;
		z = _z;
		nx = _nz;
		ny = _ny;
		nz = _nz;
		u = _u;
		v = _v;
	}
	float x, y, z;
	float nx, ny, nz;
	float u, v;

	static const DWORD FVF;
};

const DWORD Vertex::FVF = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1;

// 退出循环之后是清理工作
void CleanUp()
{
	d3d9::Release<IDirect3DVertexBuffer9*>(VB);
	d3d9::Release<IDirect3DTexture9*>(FloorTex);
	d3d9::Release<IDirect3DTexture9*>(WallTex);
	d3d9::Release<IDirect3DTexture9*>(MirrorTex);
	d3d9::Release<ID3DXMesh*>(Teapot);
}

// 进入循环之前的设置
bool SetUp()
{
	// 初始化茶壶位置
	TeapotPosition = D3DXVECTOR3(0.0f, 3.0f, -7.5f);

	// 初始化材质
	FloorMtrl = d3d9::WHITE_MTRL;
	WallMtrl = d3d9::WHITE_MTRL;
	MirrorMtrl = d3d9::WHITE_MTRL;
	TeapotMtrl = d3d9::YELLOW_MTRL;

	WallMtrl.Specular = d3d9::WHITE_COLOR*0.2f;

	// 初始化纹理
	D3DXCreateTextureFromFile(g_pDevice, "checker.jpg", &FloorTex);
	D3DXCreateTextureFromFile(g_pDevice, "brick0.jpg", &WallTex);
	D3DXCreateTextureFromFile(g_pDevice, "ice.bmp", &MirrorTex);

	// 创建茶壶网格缓存
	D3DXCreateTeapot(g_pDevice, &Teapot, 0);

	// 创建背景顶点缓存
	g_pDevice->CreateVertexBuffer(
		24 * sizeof(Vertex),
		D3DUSAGE_WRITEONLY,
		Vertex::FVF,
		D3DPOOL_MANAGED,
		&VB,
		0
		);

	// 初始化背景顶点缓存
	Vertex* v = 0;
	VB->Lock(0, 0, (void**)&v, 0);

	// floor
	v[0] = Vertex(-7.5f, 0.0f, -10.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f);
	v[1] = Vertex(-7.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f);
	v[2] = Vertex(7.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f);

	v[3] = Vertex(-7.5f, 0.0f, -10.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f);
	v[4] = Vertex(7.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f);
	v[5] = Vertex(7.5f, 0.0f, -10.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f);

	// wall
	v[6] = Vertex(-7.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
	v[7] = Vertex(-7.5f, 5.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f);
	v[8] = Vertex(-2.5f, 5.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);

	v[9] = Vertex(-7.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
	v[10] = Vertex(-2.5f, 5.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);
	v[11] = Vertex(-2.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f);

	// Note: We leave gap in middle of walls for mirror

	v[12] = Vertex(2.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
	v[13] = Vertex(2.5f, 5.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f);
	v[14] = Vertex(7.5f, 5.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);

	v[15] = Vertex(2.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
	v[16] = Vertex(7.5f, 5.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);
	v[17] = Vertex(7.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f);

	// mirror
	v[18] = Vertex(-2.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
	v[19] = Vertex(-2.5f, 5.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f);
	v[20] = Vertex(2.5f, 5.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);

	v[21] = Vertex(-2.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
	v[22] = Vertex(2.5f, 5.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);
	v[23] = Vertex(2.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f);

	VB->Unlock();

	// 纹理采样设置
	g_pDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	g_pDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	g_pDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

	// 设置融合模式(此处设置为透明模式)
	g_pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	g_pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	// 设置透明模式中Alpha通道来源
	g_pDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE);
	g_pDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);

	// 创建光源
	D3DXVECTOR3 lightDir(0.707f, -0.707f, 0.707f);
	D3DXCOLOR color(1.0f, 1.0f, 1.0f, 1.0f);
	D3DLIGHT9 light = d3d9::InitDirLight(&lightDir, &color);

	// 注册光源
	g_pDevice->SetLight(0, &light);
	g_pDevice->LightEnable(0, true);
	g_pDevice->SetRenderState(D3DRS_LIGHTING, true);
	g_pDevice->SetRenderState(D3DRS_NORMALIZENORMALS, true);
	g_pDevice->SetRenderState(D3DRS_SPECULARENABLE, true);

	// 初始化视觉矩阵,投影矩阵
	// 相机位置,视点位置,竖直向上向量
	D3DXVECTOR3 eye(-10.0f, 3.0f, -15.0f);
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
		static float radius = 20.0f;
		static float angle = (3.0f * D3DX_PI) / 2.0f;

		if (::GetAsyncKeyState(VK_LEFT) & 0x8000f)
			angle -= 0.5f * ftimeDleta;

		if (::GetAsyncKeyState(VK_RIGHT) & 0x8000f)
			angle += 0.5f * ftimeDleta;

		if (::GetAsyncKeyState(VK_UP) & 0x8000f)
			radius -= 2.0f * ftimeDleta;

		if (::GetAsyncKeyState(VK_DOWN) & 0x8000f)
			radius += 2.0f * ftimeDleta;

		if (::GetAsyncKeyState('A') & 0x8000f)
			TeapotPosition.x -= 3.0f * ftimeDleta;

		if (::GetAsyncKeyState('S') & 0x8000f)
			TeapotPosition.x += 3.0f * ftimeDleta;


		D3DXVECTOR3 position(cosf(angle) * radius, 3.0f, sinf(angle) * radius);
		D3DXVECTOR3 target(0.0f, 0.0f, 0.0f);
		D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);
		D3DXMATRIX V;
		D3DXMatrixLookAtLH(&V, &position, &target, &up);
		g_pDevice->SetTransform(D3DTS_VIEW, &V);

		//
		// Draw the scene:
		//
		g_pDevice->Clear(0, 0,
			D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL,
			0xff000000, 1.0f, 0L);

		g_pDevice->BeginScene();

		RenderScene();

		RenderShadow();

		RenderMirror();

		g_pDevice->EndScene();
		g_pDevice->Present(0, 0, 0, 0);
	}
	return true;
}


void RenderScene()
{
	// 绘制茶壶
	g_pDevice->SetMaterial(&TeapotMtrl);
	g_pDevice->SetTexture(0, NULL);
	D3DXMATRIX t;
	D3DXMatrixTranslation(&t, TeapotPosition.x, TeapotPosition.y, TeapotPosition.z);
	g_pDevice->SetTransform(D3DTS_WORLD, &t);
	Teapot->DrawSubset(0);
	// 设置一下三个物体绘制时的公共属性
	D3DXMATRIX w;
	D3DXMatrixIdentity(&w);
	g_pDevice->SetTransform(D3DTS_WORLD, &w);
	g_pDevice->SetStreamSource(0, VB, 0, sizeof(Vertex));
	g_pDevice->SetFVF(Vertex::FVF);
	// 绘制地面
	g_pDevice->SetMaterial(&FloorMtrl);
	g_pDevice->SetTexture(0, FloorTex);
	g_pDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 2);
	// 绘制墙壁
	g_pDevice->SetMaterial(&WallMtrl);
	g_pDevice->SetTexture(0, WallTex);
	g_pDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 6, 4);
	// 绘制镜子
	g_pDevice->SetMaterial(&MirrorMtrl);
	g_pDevice->SetTexture(0, MirrorTex);
	g_pDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 18, 2);

}

void RenderMirror()
{
	// 设置模板属性
	g_pDevice->SetRenderState(D3DRS_STENCILENABLE, true);
	g_pDevice->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_ALWAYS);
	g_pDevice->SetRenderState(D3DRS_STENCILREF, 0x1);
	g_pDevice->SetRenderState(D3DRS_STENCILMASK, 0xffffffff);
	g_pDevice->SetRenderState(D3DRS_STENCILWRITEMASK, 0xffffffff);
	g_pDevice->SetRenderState(D3DRS_STENCILFAIL, D3DSTENCILOP_KEEP);
	g_pDevice->SetRenderState(D3DRS_STENCILZFAIL, D3DSTENCILOP_KEEP);
	g_pDevice->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_REPLACE);

	// 禁止写入深度缓存
	g_pDevice->SetRenderState(D3DRS_ZWRITEENABLE, false);
	// 打开透明效果
	g_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
	g_pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ZERO);
	g_pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

	// 初始化模板中镜面区域
	g_pDevice->SetMaterial(&MirrorMtrl);
	g_pDevice->SetTexture(0, MirrorTex);
	g_pDevice->SetFVF(Vertex::FVF);
	g_pDevice->SetStreamSource(0, VB, 0, sizeof(Vertex));

	D3DXMATRIX world;
	D3DXMatrixIdentity(&world);
	g_pDevice->SetTransform(D3DTS_WORLD, &world);
	g_pDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 18, 2);

	// 恢复深度缓存区写入功能
	g_pDevice->SetRenderState(D3DRS_ZWRITEENABLE, true);
	g_pDevice->Clear(0, 0, D3DCLEAR_ZBUFFER, 0, 1.0f, 0);

	// 重新设置模板属性
	g_pDevice->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_EQUAL);
	g_pDevice->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_KEEP);

	// 设置融合参数
	g_pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_DESTCOLOR);
	g_pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ZERO);
	// 世界矩阵
	D3DXMATRIX W, T, R;
	D3DXPLANE plan(0.0f, 0.0f, 1.0f, 0.0f);
	D3DXMatrixReflect(&R, &plan);
	D3DXMatrixTranslation(&T, TeapotPosition.x, TeapotPosition.y, TeapotPosition.z);
	W = T*R;

	// 画镜像
	g_pDevice->SetMaterial(&TeapotMtrl);
	g_pDevice->SetTexture(0, 0);
	g_pDevice->SetTransform(D3DTS_WORLD, &W);
	g_pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);// 设置背面消除模式
	Teapot->DrawSubset(0);

	// 还原背面消除模式
	g_pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	// 取消融合及末班功能
	g_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
	g_pDevice->SetRenderState(D3DRS_STENCILENABLE, false);
}

void RenderShadow()
{
	g_pDevice->SetRenderState(D3DRS_STENCILENABLE, true);
	g_pDevice->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_EQUAL);
	g_pDevice->SetRenderState(D3DRS_STENCILREF, 0x0);
	g_pDevice->SetRenderState(D3DRS_STENCILMASK, 0xffffffff);
	g_pDevice->SetRenderState(D3DRS_STENCILWRITEMASK, 0xffffffff);
	g_pDevice->SetRenderState(D3DRS_STENCILZFAIL, D3DSTENCILOP_KEEP);
	g_pDevice->SetRenderState(D3DRS_STENCILFAIL, D3DSTENCILOP_KEEP);
	g_pDevice->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_INCR); // increment to 1

	// position shadow
	D3DXVECTOR4 lightDirection(0.707f, -0.707f, 0.707f, 0.0f);
	D3DXPLANE groundPlane(0.0f, -1.0f, 0.0f, 0.0f);

	D3DXMATRIX S;
	D3DXMatrixShadow(
		&S,
		&lightDirection,
		&groundPlane);

	D3DXMATRIX T;
	D3DXMatrixTranslation(
		&T,
		TeapotPosition.x,
		TeapotPosition.y,
		TeapotPosition.z);

	D3DXMATRIX W = T * S;
	g_pDevice->SetTransform(D3DTS_WORLD, &W);

	// alpha blend the shadow
	g_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
	g_pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	g_pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	D3DMATERIAL9 mtrl = d3d9::InitMaterial(d3d9::BLACK_COLOR, d3d9::BLACK_COLOR, d3d9::BLACK_COLOR, d3d9::BLACK_COLOR, 0.0f);
	mtrl.Diffuse.a = 0.5f; // 50% transparency.

	// Disable depth buffer so that z-fighting doesn't occur when we
	// render the shadow on top of the floor.
	g_pDevice->SetRenderState(D3DRS_ZENABLE, false);

	g_pDevice->SetMaterial(&mtrl);
	g_pDevice->SetTexture(0, 0);
	Teapot->DrawSubset(0);

	g_pDevice->SetRenderState(D3DRS_ZENABLE, true);
	g_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
	g_pDevice->SetRenderState(D3DRS_STENCILENABLE, false);

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