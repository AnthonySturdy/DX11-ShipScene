#include "Application.h"

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
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

Application::Application()
{
	_hInst = nullptr;
	_hWnd = nullptr;
	_driverType = D3D_DRIVER_TYPE_NULL;
	_featureLevel = D3D_FEATURE_LEVEL_11_0;
	_pd3dDevice = nullptr;
	_pImmediateContext = nullptr;
	_pSwapChain = nullptr;
	_pRenderTargetView = nullptr;
	_pVertexShader = nullptr;
	_pPixelShader = nullptr;
	_pVertexLayout = nullptr;
	_pCubeVertexBuffer = nullptr;
	_pCubeIndexBuffer = nullptr;
	_pConstantBuffer = nullptr;
	_depthStencilView = nullptr;
	_depthStencilBuffer = nullptr;
}

Application::~Application()
{
	Cleanup();
}

HRESULT Application::Initialise(HINSTANCE hInstance, int nCmdShow)
{
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

	// Initialize the world matrix
	XMStoreFloat4x4(&_world, XMMatrixIdentity());

    // Initialize the view matrix
	XMVECTOR Eye = XMVectorSet(eyePos.x, eyePos.y, eyePos.z, 0.0f);
	XMVECTOR At = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	XMVECTOR Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	XMStoreFloat4x4(&_view, XMMatrixLookAtLH(Eye, At, Up));

    // Initialize the projection matrix
	XMStoreFloat4x4(&_projection, XMMatrixPerspectiveFovLH(XM_PIDIV2, _WindowWidth / (FLOAT) _WindowHeight, 0.01f, 100.0f));

	//Reserve belt world matrices
	for (int i = 0; i < 100; i++) {
		_worldBelt.push_back(XMFLOAT4X4());
		random.push_back(RandomFloat(-1.0f, 1.0f));
	}

	//Create gameobjects
	testGO.push_back(new GameObject(_pd3dDevice, "G06_hotdog.obj", L"hotdog.dds", vector3(4.0f, 0.0f, -2.0f), vector3(), vector3(2.0f, 2.0f, 2.0f)));
	testGO.push_back(new GameObject_Plane(_pd3dDevice, L"Crate_COLOR.dds", 100, 100));

	return S_OK;
}

HRESULT Application::InitShadersAndInputLayout()
{
	HRESULT hr;

    // Compile the vertex shader
    ID3DBlob* pVSBlob = nullptr;
    hr = CompileShaderFromFile(L"DX11 Framework.fx", "VS", "vs_4_0", &pVSBlob);

    if (FAILED(hr))
    {
        MessageBox(nullptr,
                   L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
        return hr;
    }

	// Create the vertex shader
	hr = _pd3dDevice->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), nullptr, &_pVertexShader);

	if (FAILED(hr))
	{	
		pVSBlob->Release();
        return hr;
	}

	// Compile the pixel shader
	ID3DBlob* pPSBlob = nullptr;
    hr = CompileShaderFromFile(L"DX11 Framework.fx", "PS", "ps_4_0", &pPSBlob);

    if (FAILED(hr))
    {
        MessageBox(nullptr,
                   L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
        return hr;
    }

	// Create the pixel shader
	hr = _pd3dDevice->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), nullptr, &_pPixelShader);
	pPSBlob->Release();

    if (FAILED(hr))
        return hr;

    // Define the input layout
    D3D11_INPUT_ELEMENT_DESC layout[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	UINT numElements = ARRAYSIZE(layout);

	//Load Texture
	CreateDDSTextureFromFile(_pd3dDevice, L"Crate_COLOR.dds", nullptr, &_pTextureRV);
	_pImmediateContext->PSSetShaderResources(0, 1, &_pTextureRV);

	//Define sampler
	ID3D11SamplerState* _pSamplerLinear = nullptr;

	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory(&sampDesc, sizeof(sampDesc));
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

	_pd3dDevice->CreateSamplerState(&sampDesc, &_pSamplerLinear);

	_pImmediateContext->PSSetSamplers(0, 1, &_pSamplerLinear);

    // Create the input layout
	hr = _pd3dDevice->CreateInputLayout(layout, numElements, pVSBlob->GetBufferPointer(),
                                        pVSBlob->GetBufferSize(), &_pVertexLayout);
	pVSBlob->Release();

	if (FAILED(hr))
        return hr;

    // Set the input layout
    _pImmediateContext->IASetInputLayout(_pVertexLayout);

	return hr;
}

HRESULT Application::InitVertexBuffer()
{
	HRESULT hr;

    // Create vertex buffers
    SimpleVertex cubeVertices[] =
    {
		{ XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT2(0.0f, 0.0f), },
		{ XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT2(1.0f, 1.0f), },
		{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT2(1.0f, 1.0f), },
		{ XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT2(0.0f, 1.0f), },
		{ XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT2(1.0f, 0.0f), },
		{ XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT2(1.0f, 1.0f), },
		{ XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT2(0.0f, 0.0f), },
		{ XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT2(0.0f, 1.0f), },
    };

	SimpleVertex pyramidVertices[] =
	{
		{ XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT2(1.0f, 1.0f),},
		{ XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT2(1.0f, 0.0f),},
		{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT2(0.0f, 0.0f),},
		{ XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT2(0.0f, 1.0f),},
		{ XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(0.5f, 0.5f)},
	};

	std::vector<SimpleVertex> plane = CreatePlaneVertices(PLANE_WIDTH, PLANE_HEIGHT);	//Generate vertex positions
	SimpleVertex planeVertices[PLANE_WIDTH * PLANE_HEIGHT];	//Create vertex buffer
	for (int i = 0; i < PLANE_WIDTH * PLANE_HEIGHT; i++) {	//Fill vertex buffer
		planeVertices[i] = plane[i];
	}

	//Create cube vertex buffer
    D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(SimpleVertex) * 8;
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;

    D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
    InitData.pSysMem = cubeVertices;

	hr = _pd3dDevice->CreateBuffer(&bd, &InitData, &_pCubeVertexBuffer);

	//Create pyramid vertex buffer
	D3D11_BUFFER_DESC bd1;
	ZeroMemory(&bd1, sizeof(bd1));
	bd1.Usage = D3D11_USAGE_DEFAULT;
	bd1.ByteWidth = sizeof(SimpleVertex) * 5;
	bd1.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd1.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA InitData1;
	ZeroMemory(&InitData1, sizeof(InitData1));
	InitData1.pSysMem = pyramidVertices;

    hr = _pd3dDevice->CreateBuffer(&bd1, &InitData1, &_pPyramidVertexBuffer);

	//Create plane vertex buffer
	D3D11_BUFFER_DESC bd2;
	ZeroMemory(&bd2, sizeof(bd2));
	bd2.Usage = D3D11_USAGE_DEFAULT;
	bd2.ByteWidth = sizeof(SimpleVertex) * plane.size();
	bd2.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd2.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA InitData2;
	ZeroMemory(&InitData2, sizeof(InitData2));
	InitData2.pSysMem = planeVertices;

	hr = _pd3dDevice->CreateBuffer(&bd2, &InitData2, &_pPlaneVertexBuffer);

    if (FAILED(hr))
        return hr;

	return S_OK;
}

HRESULT Application::InitIndexBuffer()
{
	HRESULT hr;

    // Create index buffers
    WORD cubeIndices[] =
    {
		0, 1, 2, 2, 1, 3,	//1
		4, 0, 6, 6, 0, 2,	//2
		7, 5, 6, 6, 5, 4,	//3
		3, 1, 7, 7, 1, 5,	//4
		4, 5, 0, 0, 5, 1,	//5
		3, 7, 2, 2, 7, 6,	//6
    };

	WORD pyramidIndices[] =
	{
		2, 4, 1,
		1, 4, 0,
		0, 4, 3,
		3, 4, 2,
		3, 2, 1,
		1, 0, 3
	};

	std::vector<int> plane = CreatePlaneIndices(PLANE_WIDTH, PLANE_HEIGHT);
	WORD planeIndices[(PLANE_WIDTH - 1) * (PLANE_HEIGHT - 1) * 6];	//Create vertex buffer
	for (int i = 0; i < (PLANE_WIDTH - 1) * (PLANE_HEIGHT - 1) * 6; i++) {	//Loop (sizex-1 * sizey-1 * amountOfIndicesPerSquareWhichIsSix) times
		planeIndices[i] = plane[i];
	}

	//Create cube index buffer
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));

    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(WORD) * 36;     
    bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
    InitData.pSysMem = cubeIndices;
    hr = _pd3dDevice->CreateBuffer(&bd, &InitData, &_pCubeIndexBuffer);

	//Create pyramid index buffer
	D3D11_BUFFER_DESC bd1;
	ZeroMemory(&bd1, sizeof(bd1));

    bd1.Usage = D3D11_USAGE_DEFAULT;
    bd1.ByteWidth = sizeof(WORD) * 18;     
    bd1.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd1.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA InitData1;
	ZeroMemory(&InitData1, sizeof(InitData1));
    InitData1.pSysMem = pyramidIndices;
    hr = _pd3dDevice->CreateBuffer(&bd1, &InitData1, &_pPyramidIndexBuffer);

	//Create plane index buffer
	D3D11_BUFFER_DESC bd2;
	ZeroMemory(&bd2, sizeof(bd2));

	bd2.Usage = D3D11_USAGE_DEFAULT;
	bd2.ByteWidth = sizeof(WORD) * plane.size();
	bd2.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd2.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA InitData2;
	ZeroMemory(&InitData2, sizeof(InitData2));
	InitData2.pSysMem = planeIndices;
	hr = _pd3dDevice->CreateBuffer(&bd2, &InitData2, &_pPlaneIndexBuffer);

    if (FAILED(hr))
        return hr;

	return S_OK;
}

std::vector<SimpleVertex> Application::CreatePlaneVertices(int sizeX, int sizeY) {
	std::vector<SimpleVertex> returnVec;

	for (int y = 0; y < sizeY; y++) {
		for (int x = 0; x < sizeX; x++) {
			SimpleVertex vert = {XMFLOAT3(x, 0, y), XMFLOAT3(0, 1, 0), XMFLOAT2(x, y)};
			returnVec.push_back(vert);
		}
	}

	return returnVec;
}

std::vector<int> Application::CreatePlaneIndices(int sizeX, int sizeY) {
	std::vector<int> returnVec;

	for (int y = 0; y < sizeY - 1; y++) {
		for (int x = 0; x < sizeX - 1; x++) {
			//Triangle 1
			returnVec.push_back((y)* sizeX + (x));
			returnVec.push_back((y + 1) * sizeX + (x));
			returnVec.push_back((y) * sizeX + (x + 1));

			//Triangle 2
			returnVec.push_back((y) * sizeX + (x + 1));
			returnVec.push_back((y + 1) * sizeX + (x));
			returnVec.push_back((y + 1) * sizeX + (x+1));
		}
	}

	return returnVec;
}

HRESULT Application::InitWindow(HINSTANCE hInstance, int nCmdShow)
{
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
    _hWnd = CreateWindow(L"TutorialWindowClass", L"DX11 Framework", WS_OVERLAPPEDWINDOW,
                         CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, nullptr, nullptr, hInstance,
                         nullptr);
    if (!_hWnd)
		return E_FAIL;

    ShowWindow(_hWnd, nCmdShow);

    return S_OK;
}

HRESULT Application::CompileShaderFromFile(WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut)
{
    HRESULT hr = S_OK;

    DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined(DEBUG) || defined(_DEBUG)
    // Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
    // Setting this flag improves the shader debugging experience, but still allows 
    // the shaders to be optimized and to run exactly the way they will run in 
    // the release configuration of this program.
    dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif

    ID3DBlob* pErrorBlob;
    hr = D3DCompileFromFile(szFileName, nullptr, nullptr, szEntryPoint, szShaderModel, 
        dwShaderFlags, 0, ppBlobOut, &pErrorBlob);

    if (FAILED(hr))
    {
        if (pErrorBlob != nullptr)
            OutputDebugStringA((char*)pErrorBlob->GetBufferPointer());

        if (pErrorBlob) pErrorBlob->Release();

        return hr;
    }

    if (pErrorBlob) pErrorBlob->Release();

    return S_OK;
}

HRESULT Application::InitDevice()
{
    HRESULT hr = S_OK;

    UINT createDeviceFlags = 0;

#ifdef _DEBUG
    createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    D3D_DRIVER_TYPE driverTypes[] =
    {
        D3D_DRIVER_TYPE_HARDWARE,
        D3D_DRIVER_TYPE_WARP,
        D3D_DRIVER_TYPE_REFERENCE,
    };

    UINT numDriverTypes = ARRAYSIZE(driverTypes);

    D3D_FEATURE_LEVEL featureLevels[] =
    {
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

    for (UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++)
    {
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

	InitShadersAndInputLayout();

	InitVertexBuffer();

    // Set vertex buffer
    UINT stride = sizeof(SimpleVertex);
    UINT offset = 0;
    _pImmediateContext->IASetVertexBuffers(0, 1, &_pCubeVertexBuffer, &stride, &offset);

	InitIndexBuffer();

    // Set index buffer
    _pImmediateContext->IASetIndexBuffer(_pCubeIndexBuffer, DXGI_FORMAT_R16_UINT, 0);

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

    if (FAILED(hr))
        return hr;

    return S_OK;
}

void Application::Cleanup()
{
    if (_pImmediateContext) _pImmediateContext->ClearState();
    if (_pConstantBuffer) _pConstantBuffer->Release();
    if (_pCubeVertexBuffer) _pCubeVertexBuffer->Release();
    if (_pCubeIndexBuffer) _pCubeIndexBuffer->Release();
    if (_pVertexLayout) _pVertexLayout->Release();
    if (_pVertexShader) _pVertexShader->Release();
    if (_pPixelShader) _pPixelShader->Release();
    if (_pRenderTargetView) _pRenderTargetView->Release();
    if (_pSwapChain) _pSwapChain->Release();
    if (_pImmediateContext) _pImmediateContext->Release();
    if (_pd3dDevice) _pd3dDevice->Release();
	if (_depthStencilView) _depthStencilView->Release();
	if (_depthStencilBuffer) _depthStencilBuffer->Release();
	if (_wireFrameRenderState) _wireFrameRenderState->Release();
}

void Application::Update()
{
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

	if (GetAsyncKeyState(VK_UP)) {
		isAllWireframe = !isAllWireframe;
	}

	_time = t;

    //
    // Animate the cube
    //
	testGO[0]->SetRotation(vector3(0, testGO[0]->GetRotation()->x + (t/2), 0));

	XMStoreFloat4x4(&_world, XMMatrixRotationY(t));
	XMStoreFloat4x4(&_world2, XMMatrixScaling(0.4f, 0.4f, 0.4f) * XMMatrixRotationY(-t * 5.0f) * XMMatrixTranslation(4.0f, 0.0f, 0.0f) * XMMatrixRotationY(-t * 0.4f));	//Apply transform to second world matrix
	XMStoreFloat4x4(&_world3, XMMatrixScaling(0.6f, 0.6f, 0.6f) * XMMatrixRotationY(-t * 3.0f) * XMMatrixTranslation(12.0f, 0.0f, 0.0f) * XMMatrixRotationY(t * 1.5f));	//Apply transform to third world matrix
	for (int i = 0; i < 100; i++) {
		XMStoreFloat4x4(&_worldBelt[i], XMMatrixScaling(0.1f, 0.1f, 0.1f) * XMMatrixRotationY(-t * 3.0f) * XMMatrixTranslation((i/10.0f * random[i]) + 3.0f, 0.0f, 0.0f) * XMMatrixRotationY(t * i/30.0f * random[i]) * XMMatrixTranslation(12.0f, 0.0f, 0.0f) * XMMatrixRotationY(t * 1.5f));	//Apply transform to third world matrix
	}
	XMStoreFloat4x4(&_world4, XMMatrixTranslation(-10, -5, -35));
}

void Application::Draw()
{
    //
    // Clear the back buffer
    //
    float ClearColor[4] = {0.0f, 0.125f, 0.3f, 1.0f}; // red,green,blue,alpha
    _pImmediateContext->ClearRenderTargetView(_pRenderTargetView, ClearColor);

	_pImmediateContext->ClearDepthStencilView(_depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	XMMATRIX world = XMLoadFloat4x4(&_world);
	XMMATRIX view = XMLoadFloat4x4(&_view);
	XMMATRIX projection = XMLoadFloat4x4(&_projection);
    //
    // Update variables
    //
    ConstantBuffer cb;
	//Matrices
	cb.mWorld = XMMatrixTranspose(world);
	cb.mView = XMMatrixTranspose(view);
	cb.mProjection = XMMatrixTranspose(projection);
	//Globals
	cb.gTime = _time;
	cb.mEyePosW = eyePos;
	//Lighting / Materials
	cb.mLightDirection = lightDirection;
	cb.mDiffuseMaterial = diffuseMaterial;
	cb.mDiffuseLight = diffuseLight;
	cb.mAmbientMaterial = ambientMaterial;
	cb.mAmbientLight = ambientLight;
	cb.mSpecularMaterial = specularMaterial;
	cb.mSpecularLight = specularLight;
	cb.mSpecularPower = specularPower;

	_pImmediateContext->UpdateSubresource(_pConstantBuffer, 0, nullptr, &cb, 0, 0);

    //
    // Renders a triangle
    //
	_pImmediateContext->RSSetState((isAllWireframe ? _wireFrameRenderState : _solidRenderState));

	_pImmediateContext->VSSetShader(_pVertexShader, nullptr, 0);
	_pImmediateContext->VSSetConstantBuffers(0, 1, &_pConstantBuffer);
    _pImmediateContext->PSSetConstantBuffers(0, 1, &_pConstantBuffer);
	_pImmediateContext->PSSetShader(_pPixelShader, nullptr, 0);

	//Render GameObject
	for (int i = 0; i < testGO.size(); i++) {
		world = XMLoadFloat4x4(testGO[i]->GetWorldMatrix());		//Convert XMFloat4x4 to XMMATRIX object
		cb.mWorld = XMMatrixTranspose(world);					//Transpose matrix (Swap rows and columns) and store it in constant buffer struct
		_pImmediateContext->UpdateSubresource(_pConstantBuffer, 0, nullptr, &cb, 0, 0);		//Copies constant buffer struct to Constant buffer on GPU

		_pImmediateContext->PSSetShaderResources(0, 1, testGO[i]->GetTexture());

		_pImmediateContext->IASetVertexBuffers(0, 1, &testGO[i]->GetMesh()->VertexBuffer, &testGO[i]->GetMesh()->VBStride, &testGO[i]->GetMesh()->VBOffset);
		_pImmediateContext->IASetIndexBuffer(testGO[i]->GetMesh()->IndexBuffer, DXGI_FORMAT_R16_UINT, 0);

		_pImmediateContext->DrawIndexed(testGO[i]->GetMesh()->IndexCount, 0, 0);
	}

	// Set vertex buffer
	_pImmediateContext->PSSetShaderResources(0, 1, &_pTextureRV);

	UINT stride1 = sizeof(SimpleVertex);
	UINT offset1 = 0;
	_pImmediateContext->IASetVertexBuffers(0, 1, &_pCubeVertexBuffer, &stride1, &offset1);
	_pImmediateContext->IASetIndexBuffer(_pCubeIndexBuffer, DXGI_FORMAT_R16_UINT, 0);

	world = XMLoadFloat4x4(&_world2);		//Convert XMFloat4x4 to XMMATRIX object
	cb.mWorld = XMMatrixTranspose(world);	//Transpose matrix (Swap rows and columns) and store it in constant buffer struct
	_pImmediateContext->UpdateSubresource(_pConstantBuffer, 0, nullptr, &cb, 0, 0);		//Copies constant buffer struct to Constant buffer on GPU
	_pImmediateContext->DrawIndexed(36, 0, 0);	//Draw object

	world = XMLoadFloat4x4(&_world3);		//Convert XMFloat4x4 to XMMATRIX object
	cb.mWorld = XMMatrixTranspose(world);	//Transpose matrix (Swap rows and columns) and store it in constant buffer struct
	_pImmediateContext->UpdateSubresource(_pConstantBuffer, 0, nullptr, &cb, 0, 0);		//Copies constant buffer struct to Constant buffer on GPU
	_pImmediateContext->DrawIndexed(36, 0, 0);	//Draw object
	
	for (int i = 0; i < 100; i++) {
		world = XMLoadFloat4x4(&_worldBelt[i]);		//Convert XMFloat4x4 to XMMATRIX object
		cb.mWorld = XMMatrixTranspose(world);	//Transpose matrix (Swap rows and columns) and store it in constant buffer struct
		_pImmediateContext->UpdateSubresource(_pConstantBuffer, 0, nullptr, &cb, 0, 0);		//Copies constant buffer struct to Constant buffer on GPU
		_pImmediateContext->DrawIndexed(36, 0, 0);	//Draw object
	}

	/*

	UINT stride2 = sizeof(SimpleVertex);
	UINT offset2 = 0;
	_pImmediateContext->IASetVertexBuffers(0, 1, &_pPlaneVertexBuffer, &stride2, &offset2);
	_pImmediateContext->IASetIndexBuffer(_pPlaneIndexBuffer, DXGI_FORMAT_R16_UINT, 0);

	world = XMLoadFloat4x4(&_world4);		//Convert XMFloat4x4 to XMMATRIX object
	cb.mWorld = XMMatrixTranspose(world);	//Transpose matrix (Swap rows and columns) and store it in constant buffer struct
	_pImmediateContext->UpdateSubresource(_pConstantBuffer, 0, nullptr, &cb, 0, 0);		//Copies constant buffer struct to Constant buffer on GPU
	_pImmediateContext->DrawIndexed((PLANE_WIDTH - 1) * (PLANE_HEIGHT - 1) * 6, 0, 0);	//Draw object
	*/
    //
    // Present our back buffer to our front buffer
    //
    _pSwapChain->Present(0, 0);

}