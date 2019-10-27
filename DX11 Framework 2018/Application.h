#pragma once

#include <windows.h>
#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <directxcolors.h>
#include <vector>
#include <time.h>
#include "resource.h"

using namespace DirectX;

#define PLANE_WIDTH 20
#define PLANE_HEIGHT 70

struct SimpleVertex
{
    XMFLOAT3 Pos;
	XMFLOAT3 Normal;
	XMFLOAT2 TexC;
};

struct ConstantBuffer
{
	//Matrices
	XMMATRIX mWorld;
	XMMATRIX mView;
	XMMATRIX mProjection;
	//Lighting and materials
	XMFLOAT4 mDiffuseMaterial;
	XMFLOAT4 mDiffuseLight;
	XMFLOAT3 mLightDirection;
	float gTime;//This is put here because it pads the float3
	XMFLOAT4 mAmbientMaterial;
	XMFLOAT4 mAmbientLight;
	XMFLOAT4 mSpecularMaterial;
	XMFLOAT4 mSpecularLight;	
	float mSpecularPower;		
	XMFLOAT3 mEyePosW;
};

class Application
{
private:
	HINSTANCE               _hInst;
	HWND                    _hWnd;
	D3D_DRIVER_TYPE         _driverType;
	D3D_FEATURE_LEVEL       _featureLevel;
	ID3D11Device*           _pd3dDevice;
	ID3D11DeviceContext*    _pImmediateContext;
	IDXGISwapChain*         _pSwapChain;
	ID3D11RenderTargetView* _pRenderTargetView;
	ID3D11VertexShader*     _pVertexShader;
	ID3D11PixelShader*      _pPixelShader;
	ID3D11InputLayout*      _pVertexLayout;
	ID3D11Buffer*           _pCubeVertexBuffer;
	ID3D11Buffer*           _pCubeIndexBuffer;	
	ID3D11Buffer*           _pPyramidVertexBuffer;
	ID3D11Buffer*           _pPyramidIndexBuffer;
	ID3D11Buffer*           _pPlaneVertexBuffer;
	ID3D11Buffer*           _pPlaneIndexBuffer;
	ID3D11Buffer*           _pConstantBuffer;
	XMFLOAT4X4              _world, _world2, _world3, _world4;
	std::vector<XMFLOAT4X4>	_worldBelt;
	XMFLOAT4X4              _view;
	XMFLOAT4X4              _projection;
	ID3D11DepthStencilView* _depthStencilView;
	ID3D11Texture2D*		_depthStencilBuffer;
	ID3D11RasterizerState*	_wireFrameRenderState;
	ID3D11RasterizerState*	_solidRenderState;

	XMFLOAT3 eyePos = XMFLOAT3(10.0f, 7.0f, -6.0f);

	XMFLOAT3 lightDirection = XMFLOAT3(0.25f, 0.5f, -1.0f);			//Light direction from surface (x, y, z)
	XMFLOAT4 diffuseMaterial = XMFLOAT4(0.8f, 0.5f, 0.5f, 1.0f);	//Diffuse material properties (rgba)
	XMFLOAT4 diffuseLight = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);		//Diffuse light colour (rgba)
	XMFLOAT4 ambientMaterial = XMFLOAT4(0.2f, 0.2f, 0.2f, 0.2f);	//Ambient material properties (rgba)
	XMFLOAT4 ambientLight = XMFLOAT4(0.2f, 0.2f, 0.2f, 0.2f);		//Ambient light colour (rgba)
	XMFLOAT4 specularMaterial = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);	//Specular material properties (rgba)
	XMFLOAT4 specularLight = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);		//Specular light colour (rgba)
	float specularPower = 10.0f;									//Specular power

	float _time;
	bool isAllWireframe = false;
	std::vector<float> random;

private:
	HRESULT InitWindow(HINSTANCE hInstance, int nCmdShow);
	HRESULT InitDevice();
	void Cleanup();
	HRESULT CompileShaderFromFile(WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut);
	HRESULT InitShadersAndInputLayout();
	HRESULT InitVertexBuffer();
	HRESULT InitIndexBuffer();

	std::vector<XMFLOAT3> CreatePlaneVertices(int sizeX, int sizeY);
	std::vector<int> CreatePlaneIndices(int sizeX, int sizeY);
	void CalculateNormals(SimpleVertex* vertices, int numVertices);
	float RandomFloat(float a, float b) {
		float random = ((float)rand()) / (float)RAND_MAX;
		float diff = b - a;
		float r = random * diff;
		return a + r;
	}

	UINT _WindowHeight;
	UINT _WindowWidth;

public:
	Application();
	~Application();

	HRESULT Initialise(HINSTANCE hInstance, int nCmdShow);

	void Update();
	void Draw();
};

