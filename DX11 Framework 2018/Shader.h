#pragma once
#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <directxcolors.h>

using namespace DirectX;

class Shader {
private:
	ID3D11Device* device;
	ID3D11DeviceContext* context;

	ID3D11VertexShader* vertexShader;
	ID3D11PixelShader* pixelShader;
	ID3D11InputLayout* vertexLayout;
	ID3D11SamplerState* samplerLinear;

public:
	Shader(ID3D11Device* _device, ID3D11DeviceContext* _context, WCHAR* fileDir);
	~Shader();

	HRESULT InitShadersAndInputLayout(WCHAR* fileDir);
	HRESULT CompileShaderFromFile(WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut);

	ID3D11VertexShader* GetVertexShader();
	ID3D11PixelShader* GetPixelShader();
	ID3D11SamplerState* GetSampler();
};