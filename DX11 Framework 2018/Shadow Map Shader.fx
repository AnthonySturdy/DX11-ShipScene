//--------------------------------------------------------------------------------------
// File: DX11 Framework.fx
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------

//Constant buffer
cbuffer ConstantBuffer : register( b0 )
{
	matrix World;
	matrix View;
	matrix Projection;
	float4 LightPos;
}

struct PS_INPUT
{
    float4 Pos : SV_POSITION;
};

//Vertex Shader
PS_INPUT VS( float4 Pos : POSITION, float3 NormalL : NORMAL, float2 Tex : TEXCOORD0 )
{
    PS_INPUT output = (PS_INPUT)0;

	// Transform the vertex position into projected space.
	output.Pos = mul(Pos, World);
	output.Pos = mul(output.Pos, View);
	output.Pos = mul(output.Pos, Projection);

    return output;
}


//Pixel Shader - not used
float4 PS(PS_INPUT input) : SV_Target
{
	return float4(input.Pos.w, input.Pos.w, input.Pos.w, input.Pos.w);
}