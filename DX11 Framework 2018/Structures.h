#pragma once
#include <directxmath.h>

using namespace DirectX;

struct SimpleVertex
{
	XMFLOAT3 Pos;
	XMFLOAT3 Normal;
	XMFLOAT2 TexC;

	bool operator<(const SimpleVertex other) const
	{
		return memcmp((void*)this, (void*)& other, sizeof(SimpleVertex)) > 0;
	};
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