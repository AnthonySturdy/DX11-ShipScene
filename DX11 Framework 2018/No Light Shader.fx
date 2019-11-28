//--------------------------------------------------------------------------------------
// File: DX11 Framework.fx
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
// Constant Buffer Variables
//--------------------------------------------------------------------------------------
Texture2D txDiffuse : register(t0);
SamplerState samLinear : register(s0);

cbuffer ConstantBuffer : register( b0 )
{
	matrix World;
	matrix View;
	matrix Projection;
	float4 DiffuseMtrl;
	float4 DiffuseLight;
	float3 LightVecW;
	float gTime;
	float4 AmbientMtrl;
	float4 AmbientLight;
	float4 SpecularMtrl;
	float4 SpecularLight;
	float SpecularPower;
	float3 EyePosW;	//Cam position in world space
}

struct PS_INPUT
{
    float4 Pos : SV_POSITION;
	float2 Tex : TEXCOORD0;
};

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
PS_INPUT VS( float4 Pos : POSITION, float3 NormalL : NORMAL, float2 Tex : TEXCOORD0 )
{
    PS_INPUT output = (PS_INPUT)0;

	output.Tex = Tex;
    output.Pos = mul( Pos, World );
    output.Pos = mul( output.Pos, View );
    output.Pos = mul( output.Pos, Projection );


    return output;
}


//--------------------------------------------------------------------------------------
// Pixel Shader - Specular lighting
//--------------------------------------------------------------------------------------
float4 PS(PS_INPUT input) : SV_Target
{
	//Texture colour
	float4 texCol = txDiffuse.Sample(samLinear, input.Tex);

	clip(texCol.a - 0.15f);

	float4 outCol;
	outCol.rgb = texCol;
	outCol.a = 1;

	return outCol;
}
