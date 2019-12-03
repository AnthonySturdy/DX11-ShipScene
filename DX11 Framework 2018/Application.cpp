#include "Application.h"

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    PAINTSTRUCT ps;
    HDC hdc;

    switch (message)
    {
        case WM_PAINT:
            hdc = BeginPaint(hWnd, &ps);
            EndPaint(hWnd, &ps);
            break;

        case WM_DESTROY:
            PostQuitMessage(0);
            break;

        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
    }

    return 0;
}

Application::Application() {
	_hInst = nullptr;
	_hWnd = nullptr;
	_driverType = D3D_DRIVER_TYPE_NULL;
	_featureLevel = D3D_FEATURE_LEVEL_11_0;
	_pd3dDevice = nullptr;
	_pImmediateContext = nullptr;
	_pSwapChain = nullptr;
	_pRenderTargetView = nullptr;
	_pConstantBuffer = nullptr;
	_depthStencilView = nullptr;
	_depthStencilBuffer = nullptr;
}

Application::~Application() {
	Cleanup();
}

HRESULT Application::Initialise(HINSTANCE hInstance, int nCmdShow) {
    if (FAILED(InitWindow(hInstance, nCmdShow)))
	{
        return E_FAIL;
	}

    RECT rc;
    GetClientRect(_hWnd, &rc);
    _WindowWidth = rc.right - rc.left;
    _WindowHeight = rc.bottom - rc.top;

    if (FAILED(InitDevice()))
    {
        Cleanup();

        return E_FAIL;
    }

	srand(time(NULL));

	//Initialise cameras
	cameras.push_back(new Camera(XMFLOAT3(40.0f, 30.0f, 50.0f), XMFLOAT3(0.0f, 6.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), _WindowWidth, _WindowHeight, 0.1f, 300.0f));
	cameras.push_back(new Camera(XMFLOAT3(40.0f, 40.0f, 50.0f), XMFLOAT3(0.0f, 2.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), _WindowWidth, _WindowHeight, 0.1f, 300.0f));
	cameras.push_back(new Camera(XMFLOAT3(-30.0f, 20.0f, -40.0f), XMFLOAT3(0.0f, 2.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), _WindowWidth, _WindowHeight, 0.1f, 300.0f));
	currentCamera = cameras[0];

	//Initialise Shaders
	shaders.push_back(new Shader(_pd3dDevice, _pImmediateContext, L"Normal Shader.fx"));
	shaders.push_back(new Shader(_pd3dDevice, _pImmediateContext, L"Boat Shader.fx"));
	shaders.push_back(new Shader(_pd3dDevice, _pImmediateContext, L"Water Shader.fx"));
	shaders.push_back(new Shader(_pd3dDevice, _pImmediateContext, L"Land Under Water Shader.fx"));
	shaders.push_back(new Shader(_pd3dDevice, _pImmediateContext, L"No Light Shader.fx"));

	// Initialize the world matrix
	XMStoreFloat4x4(&_world, XMMatrixIdentity());

	//Initialise the view matrix
	XMStoreFloat4x4(&_view, XMMatrixLookAtLH(XMLoadFloat3(&currentCamera->GetEye()), XMLoadFloat3(&currentCamera->GetAt()), XMLoadFloat3(&currentCamera->GetUp())));

    // Initialize the projection matrix
	XMStoreFloat4x4(&_projection, currentCamera->GetProjectionMatrix());

	//Load scene from json file
	hierarchy = new SceneGraph("SCENE.json", _pd3dDevice);
	shipController = new ShipController(hierarchy->GetBase()->children[0]->gameObject);

	return S_OK;
}

HRESULT Application::InitWindow(HINSTANCE hInstance, int nCmdShow) {
    // Register class
    WNDCLASSEX wcex;
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, (LPCTSTR)IDI_TUTORIAL1);
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW );
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = nullptr;
    wcex.lpszClassName = L"TutorialWindowClass";
    wcex.hIconSm = LoadIcon(wcex.hInstance, (LPCTSTR)IDI_TUTORIAL1);
    if (!RegisterClassEx(&wcex))
        return E_FAIL;

    // Create window
    _hInst = hInstance;
    RECT rc = {0, 0, 1280, 720};
    AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
    _hWnd = CreateWindow(L"TutorialWindowClass", L"DX11 Scene // Anthony Sturdy", WS_OVERLAPPEDWINDOW,
                         CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, nullptr, nullptr, hInstance,
                         nullptr);
    if (!_hWnd)
		return E_FAIL;

    ShowWindow(_hWnd, nCmdShow);

    return S_OK;
}

HRESULT Application::InitDevice() {
    HRESULT hr = S_OK;

    UINT createDeviceFlags = 0;

#ifdef _DEBUG
    createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    D3D_DRIVER_TYPE driverTypes[] = {
        D3D_DRIVER_TYPE_HARDWARE,
        D3D_DRIVER_TYPE_WARP,
        D3D_DRIVER_TYPE_REFERENCE,
    };

    UINT numDriverTypes = ARRAYSIZE(driverTypes);

    D3D_FEATURE_LEVEL featureLevels[] = {
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
    };

	UINT numFeatureLevels = ARRAYSIZE(featureLevels);

    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory(&sd, sizeof(sd));
    sd.BufferCount = 1;
    sd.BufferDesc.Width = _WindowWidth;
    sd.BufferDesc.Height = _WindowHeight;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = _hWnd;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;

    for (UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++) {
        _driverType = driverTypes[driverTypeIndex];
        hr = D3D11CreateDeviceAndSwapChain(nullptr, _driverType, nullptr, createDeviceFlags, featureLevels, numFeatureLevels,
                                           D3D11_SDK_VERSION, &sd, &_pSwapChain, &_pd3dDevice, &_featureLevel, &_pImmediateContext);
        if (SUCCEEDED(hr))
            break;
    }

    if (FAILED(hr))
        return hr;

    // Create a render target view
    ID3D11Texture2D* pBackBuffer = nullptr;
    hr = _pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);

    if (FAILED(hr))
        return hr;

    hr = _pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &_pRenderTargetView);
    pBackBuffer->Release();

    if (FAILED(hr))
        return hr;


	//Set up depth/stencil buffer
	D3D11_TEXTURE2D_DESC depthStencilDesc;
	depthStencilDesc.Width = _WindowWidth;
	depthStencilDesc.Height = _WindowHeight;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilDesc.SampleDesc.Count = 1;
	depthStencilDesc.SampleDesc.Quality = 0;
	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags = 0;
	depthStencilDesc.MiscFlags = 0;

	_pd3dDevice->CreateTexture2D(&depthStencilDesc, nullptr, &_depthStencilBuffer);
	_pd3dDevice->CreateDepthStencilView(_depthStencilBuffer, nullptr, &_depthStencilView);

    _pImmediateContext->OMSetRenderTargets(1, &_pRenderTargetView, _depthStencilView);

    // Setup the viewport
    D3D11_VIEWPORT vp;
    vp.Width = (FLOAT)_WindowWidth;
    vp.Height = (FLOAT)_WindowHeight;
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;
    _pImmediateContext->RSSetViewports(1, &vp);

    // Set primitive topology
    _pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Create the constant buffer
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(ConstantBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
    hr = _pd3dDevice->CreateBuffer(&bd, nullptr, &_pConstantBuffer);

	//Create wireframe render state
	D3D11_RASTERIZER_DESC wfdesc;
	ZeroMemory(&wfdesc, sizeof(D3D11_RASTERIZER_DESC));
	wfdesc.FillMode = D3D11_FILL_WIREFRAME;
	wfdesc.CullMode = D3D11_CULL_NONE;
	hr = _pd3dDevice->CreateRasterizerState(&wfdesc, &_wireFrameRenderState);

	//Create solid render state
	D3D11_RASTERIZER_DESC wfdesc1;
	ZeroMemory(&wfdesc1, sizeof(D3D11_RASTERIZER_DESC));
	wfdesc1.FillMode = D3D11_FILL_SOLID;
	wfdesc1.CullMode = D3D11_CULL_BACK;
	hr = _pd3dDevice->CreateRasterizerState(&wfdesc1, &_solidRenderState);

	//Blend state
	D3D11_RENDER_TARGET_BLEND_DESC rtbd;
	ZeroMemory(&rtbd, sizeof(rtbd));

	D3D11_BLEND_DESC blendDesc;
	ZeroMemory(&blendDesc, sizeof(blendDesc));

	rtbd.BlendEnable = true;
	rtbd.SrcBlend = D3D11_BLEND_SRC_COLOR;
	rtbd.DestBlend = D3D11_BLEND_BLEND_FACTOR;
	rtbd.BlendOp = D3D11_BLEND_OP_ADD;
	rtbd.SrcBlendAlpha = D3D11_BLEND_ONE;
	rtbd.DestBlendAlpha = D3D11_BLEND_ZERO;
	rtbd.BlendOpAlpha = D3D11_BLEND_OP_ADD;
	rtbd.RenderTargetWriteMask = D3D10_COLOR_WRITE_ENABLE_ALL;

	blendDesc.AlphaToCoverageEnable = false;
	blendDesc.RenderTarget[0] = rtbd;

	_pd3dDevice->CreateBlendState(&blendDesc, &_transparency);

    if (FAILED(hr))
        return hr;

    return S_OK;
}

void Application::Cleanup() {
    if (_pImmediateContext) _pImmediateContext->ClearState();
    if (_pConstantBuffer) _pConstantBuffer->Release();
    if (_pRenderTargetView) _pRenderTargetView->Release();
    if (_pSwapChain) _pSwapChain->Release();
    if (_pImmediateContext) _pImmediateContext->Release();
    if (_pd3dDevice) _pd3dDevice->Release();
	if (_depthStencilView) _depthStencilView->Release();
	if (_depthStencilBuffer) _depthStencilBuffer->Release();
	if (_wireFrameRenderState) _wireFrameRenderState->Release();
	if (_transparency) _transparency->Release();
}

void Application::Update() {
    // Update our time
    static float t = 0.0f;

    if (_driverType == D3D_DRIVER_TYPE_REFERENCE)
    {
        t += (float) XM_PI * 0.0125f;
    }
    else
    {
        static DWORD dwTimeStart = 0;
        DWORD dwTimeCur = GetTickCount();

        if (dwTimeStart == 0)
            dwTimeStart = dwTimeCur;

        t = (dwTimeCur - dwTimeStart) / 1000.0f;
    }

	if (GetAsyncKeyState(VK_F1)) {
		isAllWireframe = !isAllWireframe;
	}

	if (GetAsyncKeyState(0x31)) {
		currentCamera = cameras[0];

		XMStoreFloat4x4(&_view, XMMatrixLookAtLH(XMLoadFloat3(&currentCamera->GetEye()), XMLoadFloat3(&currentCamera->GetAt()), XMLoadFloat3(&currentCamera->GetUp())));
		XMStoreFloat4x4(&_projection, currentCamera->GetProjectionMatrix());
	} 
	if (GetAsyncKeyState(0x32)) {
		currentCamera = cameras[1];

		XMStoreFloat4x4(&_view, XMMatrixLookAtLH(XMLoadFloat3(&currentCamera->GetEye()), XMLoadFloat3(&currentCamera->GetAt()), XMLoadFloat3(&currentCamera->GetUp())));
		XMStoreFloat4x4(&_projection, currentCamera->GetProjectionMatrix());
	} 
	if (GetAsyncKeyState(0x33)) {
		currentCamera = cameras[2];

		XMStoreFloat4x4(&_view, XMMatrixLookAtLH(XMLoadFloat3(&currentCamera->GetEye()), XMLoadFloat3(&currentCamera->GetAt()), XMLoadFloat3(&currentCamera->GetUp())));
		XMStoreFloat4x4(&_projection, currentCamera->GetProjectionMatrix());
	}

	//Update ship
	shipController->Update();

	//Update all SceneGraph positions and populate gameObjects list
	hierarchy->GetBase()->UpdateTransformation(&gameObjects);	

	//Camera control
	GameObject* camPosParentObject = hierarchy->GetBase()->children[0]->children[0]->gameObject;
	GameObject* camPosObject = hierarchy->GetBase()->children[0]->children[0]->children[0]->gameObject;		// Hierarchy base->Ship->Camera Parent (for rotation)->Child (camPos object)
	XMFLOAT4X4* camPosViewMatrix = camPosObject->GetWorldMatrix();	
	vector3 camPos(camPosViewMatrix->_41, camPosViewMatrix->_42, camPosViewMatrix->_43);	//Have to access matrix to get actual position after other transformations (such as rotation and scaling)
	vector3 camLookAt = *shipController->GetShip()->GetPosition();
	cameras[0]->SetEye(XMFLOAT3(camPos.x, camPos.y, camPos.z));
	cameras[0]->SetAt(XMFLOAT3(camLookAt.x, camLookAt.y, camLookAt.z));
	if (GetAsyncKeyState(VK_LEFT)) {
		camPosParentObject->SetRotation(vector3(camPosParentObject->GetRotation()->x, camPosParentObject->GetRotation()->y + 0.03f, camPosParentObject->GetRotation()->z));
	} 
	if (GetAsyncKeyState(VK_RIGHT)) {
		camPosParentObject->SetRotation(vector3(camPosParentObject->GetRotation()->x, camPosParentObject->GetRotation()->y - 0.03f, camPosParentObject->GetRotation()->z));
	}

	currentCamera->Update();
	_time = t;

	// Set Skybox Position
	hierarchy->GetBase()->children[5]->gameObject->SetPosition(vector3(
		currentCamera->GetEye().x,
		currentCamera->GetEye().y,
		currentCamera->GetEye().z));
}

void Application::Draw() {
<<<<<<< HEAD
=======
    // Clear the back buffer
    float ClearColor[4] = {0.0f, 0.125f, 0.3f, 1.0f};	// red,green,blue,alpha
    _pImmediateContext->ClearRenderTargetView(_pRenderTargetView, ClearColor);
	_pImmediateContext->ClearDepthStencilView(_depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

>>>>>>> parent of 1d242f9... Not much progress tbh
	XMMATRIX view = currentCamera->GetViewMatrix();
	XMMATRIX projection = currentCamera->GetProjectionMatrix();

    // Set Render state
	_pImmediateContext->RSSetState((isAllWireframe ? _wireFrameRenderState : _solidRenderState));

	_pImmediateContext->OMSetRenderTargets(1, &_pRenderTargetView, _shadowMapStencil);
	_pImmediateContext->ClearDepthStencilView(_depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	for (int i = 0; i < gameObjects.size(); i++) {
		//Set position
		XMMATRIX world = XMLoadFloat4x4(gameObjects[i]->GetWorldMatrix());		//Convert XMFloat4x4 to XMMATRIX object

		//Set constant buffer
		ConstantBuffer cb;

		cb.mWorld = XMMatrixTranspose(world);
		cb.mView = XMMatrixTranspose(view);
		cb.mProjection = XMMatrixTranspose(projection);

		cb.gTime = _time;
		cb.mEyePosW = currentCamera->GetEye();

		Material* m = gameObjects[i]->GetMaterial();
		cb.mLightDirection = m->lightDirection;
		cb.mDiffuseMaterial = m->diffuseMaterial;
		cb.mDiffuseLight = m->diffuseLight;
		cb.mAmbientMaterial = m->ambientMaterial;
		cb.mAmbientLight = m->ambientLight;
		cb.mSpecularMaterial = m->specularMaterial;
		cb.mSpecularLight = m->specularLight;
		cb.mSpecularPower = m->specularPower;

		_pImmediateContext->UpdateSubresource(_pConstantBuffer, 0, nullptr, &cb, 0, 0);

		//Set blend state
		float blendFactor[] = { 0.5f, 0.5f, 0.5f, 1.0f };
		if (gameObjects[i]->GetShaderType() == ShaderType::WATER)
			_pImmediateContext->OMSetBlendState(_transparency, blendFactor, 0xffffffff);
		else
			_pImmediateContext->OMSetBlendState(0, 0, 0xffffffff);

		//Set shader
		Shader* s = shaders[gameObjects[i]->GetShaderType()];
		_pImmediateContext->VSSetShader(s->GetVertexShader(), nullptr, 0);
		_pImmediateContext->VSSetConstantBuffers(0, 1, &_pConstantBuffer);
		_pImmediateContext->PSSetConstantBuffers(0, 1, &_pConstantBuffer);
		_pImmediateContext->PSSetShader(s->GetPixelShader(), nullptr, 0);

		//Set texture
		ID3D11SamplerState* samp = s->GetSampler();
		_pImmediateContext->PSSetSamplers(0, 1, &samp);
		_pImmediateContext->PSSetShaderResources(0, 1, gameObjects[i]->GetDiffuseTexture());
		_pImmediateContext->PSSetShaderResources(1, 1, gameObjects[i]->GetNormalTexture());
		_pImmediateContext->PSSetShaderResources(2, 1, gameObjects[i]->GetSpecularTexture());

		//Set vertex and index buffer
		_pImmediateContext->IASetVertexBuffers(0, 1, &gameObjects[i]->GetMesh()->VertexBuffer, &gameObjects[i]->GetMesh()->VBStride, &gameObjects[i]->GetMesh()->VBOffset);
		_pImmediateContext->IASetIndexBuffer(gameObjects[i]->GetMesh()->IndexBuffer, DXGI_FORMAT_R16_UINT, 0);

		//Draw
		_pImmediateContext->DrawIndexed(gameObjects[i]->GetMesh()->IndexCount, 0, 0);
	}

	_pImmediateContext->OMSetRenderTargets(1, &_pRenderTargetView, _depthStencilView);
	float ClearColor[4] = { 0.0f, 0.125f, 0.3f, 1.0f };	// red,green,blue,alpha
	_pImmediateContext->ClearRenderTargetView(_pRenderTargetView, ClearColor);
	_pImmediateContext->ClearDepthStencilView(_depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	//Render GameObjects
	for (int i = 0; i < gameObjects.size(); i++) {
		//Set position
		XMMATRIX world = XMLoadFloat4x4(gameObjects[i]->GetWorldMatrix());		//Convert XMFloat4x4 to XMMATRIX object

		//Set constant buffer
		ConstantBuffer cb;

		cb.mWorld = XMMatrixTranspose(world);
		cb.mView = XMMatrixTranspose(view);
		cb.mProjection = XMMatrixTranspose(projection);

		cb.gTime = _time;
		cb.mEyePosW = currentCamera->GetEye();

		Material* m = gameObjects[i]->GetMaterial();
		cb.mLightDirection = m->lightDirection;
		cb.mDiffuseMaterial = m->diffuseMaterial;
		cb.mDiffuseLight = m->diffuseLight;
		cb.mAmbientMaterial = m->ambientMaterial;
		cb.mAmbientLight = m->ambientLight;
		cb.mSpecularMaterial = m->specularMaterial;
		cb.mSpecularLight = m->specularLight;
		cb.mSpecularPower = m->specularPower;

		_pImmediateContext->UpdateSubresource(_pConstantBuffer, 0, nullptr, &cb, 0, 0);

		//Set blend state
		float blendFactor[] = { 0.5f, 0.5f, 0.5f, 1.0f };
		if (gameObjects[i]->GetShaderType() == ShaderType::WATER)
			_pImmediateContext->OMSetBlendState(_transparency, blendFactor, 0xffffffff);
		else
			_pImmediateContext->OMSetBlendState(0, 0, 0xffffffff);

		//Set shader
		Shader* s = shaders[gameObjects[i]->GetShaderType()];
		_pImmediateContext->VSSetShader(s->GetVertexShader(), nullptr, 0);
		_pImmediateContext->VSSetConstantBuffers(0, 1, &_pConstantBuffer);
		_pImmediateContext->PSSetConstantBuffers(0, 1, &_pConstantBuffer);
		_pImmediateContext->PSSetShader(s->GetPixelShader(), nullptr, 0);

		//Set texture
		ID3D11SamplerState* samp = s->GetSampler();
		_pImmediateContext->PSSetSamplers(0, 1, &samp);
		_pImmediateContext->PSSetShaderResources(0, 1, &_shadowMapRV);
		_pImmediateContext->PSSetShaderResources(1, 1, gameObjects[i]->GetNormalTexture());
		_pImmediateContext->PSSetShaderResources(2, 1, gameObjects[i]->GetSpecularTexture());

		//Set vertex and index buffer
		_pImmediateContext->IASetVertexBuffers(0, 1, &gameObjects[i]->GetMesh()->VertexBuffer, &gameObjects[i]->GetMesh()->VBStride, &gameObjects[i]->GetMesh()->VBOffset);
		_pImmediateContext->IASetIndexBuffer(gameObjects[i]->GetMesh()->IndexBuffer, DXGI_FORMAT_R16_UINT, 0);

		//Draw
		_pImmediateContext->DrawIndexed(gameObjects[i]->GetMesh()->IndexCount, 0, 0);
	}

	gameObjects.clear();	//Clear gameobjects list so it can be populated next frame

    // Present our back buffer to our front buffer
    _pSwapChain->Present(0, 0);
}