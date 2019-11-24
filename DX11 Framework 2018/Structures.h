#pragma once
#include <directxmath.h>

using namespace DirectX;

struct SimpleVertex {
	XMFLOAT3 Pos;
	XMFLOAT3 Normal;
	XMFLOAT2 TexC;

	bool operator<(const SimpleVertex other) const
	{
		return memcmp((void*)this, (void*)& other, sizeof(SimpleVertex)) > 0;
	};
};

struct Material {
	Material() {
		lightDirection = XMFLOAT3(0.25f, 0.5f, -1.0f);
		diffuseMaterial = XMFLOAT4(0.8f, 0.5f, 0.5f, 1.0f);
		diffuseLight = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		ambientMaterial = XMFLOAT4(0.2f, 0.2f, 0.2f, 0.2f);
		ambientLight = XMFLOAT4(0.2f, 0.2f, 0.2f, 0.2f);
		specularMaterial = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
		specularLight = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
		specularPower = 10.0f;
	}

	XMFLOAT3 lightDirection;	//Light direction from surface (x, y, z)
	XMFLOAT4 diffuseMaterial;	//Diffuse material properties (rgba)
	XMFLOAT4 diffuseLight;		//Diffuse light colour (rgba)
	XMFLOAT4 ambientMaterial;	//Ambient material properties (rgba)
	XMFLOAT4 ambientLight;		//Ambient light colour (rgba)
	XMFLOAT4 specularMaterial;	//Specular material properties (rgba)
	XMFLOAT4 specularLight;		//Specular light colour (rgba)
	float specularPower;		//Specular power
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

enum ShaderType {
	DEFAULT = 0,
	NO_LIGHTING = 1
};