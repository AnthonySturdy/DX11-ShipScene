#pragma once

#include <windows.h>
#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <directxcolors.h>
#include <vector>
#include <time.h>
#include "resource.h"
#include "DDSTextureLoader.h"
#include "OBJLoader.h"
#include "Structures.h"
#include "GameObject.h"
#include "GameObject_Plane.h"
#include "Camera.h"
#include "Shader.h"
#include "SceneGraph.h"
#include "SceneGraphObject.h"

using namespace DirectX;

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
	ID3D11Buffer*           _pConstantBuffer;
	XMFLOAT4X4              _world;
	XMFLOAT4X4              _view;
	XMFLOAT4X4              _projection;
	ID3D11DepthStencilView* _depthStencilView;
	ID3D11Texture2D*		_depthStencilBuffer;
	ID3D11RasterizerState*	_wireFrameRenderState;
	ID3D11RasterizerState*	_solidRenderState;
	Camera* currentCamera = nullptr;
	std::vector<Camera*> cameras;
	std::vector<Shader*> shaders;
	std::vector<GameObject*> gameObjects;
	SceneGraph* hierarchy;

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

private:
	HRESULT InitWindow(HINSTANCE hInstance, int nCmdShow);
	HRESULT InitDevice();
	void Cleanup();

	UINT _WindowHeight;
	UINT _WindowWidth;

public:
	Application();
	~Application();

	HRESULT Initialise(HINSTANCE hInstance, int nCmdShow);

	void Update();
	void Draw();
};

