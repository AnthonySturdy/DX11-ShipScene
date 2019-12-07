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
#include "ShipController.h"

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
	ID3D11BlendState*		_transparency;
	ID3D11RasterizerState*	_wireFrameRenderState;
	ID3D11RasterizerState*	_solidRenderState;
	D3D11_VIEWPORT			viewport;

	Camera* currentCamera = nullptr;
	std::vector<Camera*> cameras;
	std::vector<Shader*> shaders;
	std::vector<GameObject*> gameObjects;
	SceneGraph* hierarchy;
	ShipController* shipController = nullptr;

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

