//--------------------------------------------------------------------------------------
// File: DX11 Framework.fx
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
// Constant Buffer Variables
//--------------------------------------------------------------------------------------
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

//--------------------------------------------------------------------------------------
struct VS_OUTPUT
{
    float4 Pos : SV_POSITION;
	float4 Color : COLOR0;
    float3 NormalL : NORMAL;
};

//--------------------------------------------------------------------------------------
// Vertex Shader - Gouraud shading and diffuse lighting
//--------------------------------------------------------------------------------------
VS_OUTPUT VS( float4 Pos : POSITION, float3 NormalL : NORMAL )
{
    VS_OUTPUT output = (VS_OUTPUT)0;

    output.Pos = mul( Pos, World );

	//Compute vector from the vertex to eye position
	//output.Pos is currently the position in world space
	float3 toEye = normalize(EyePosW - output.Pos.xyz);

    output.Pos = mul( output.Pos, View );
    output.Pos = mul( output.Pos, Projection );

	// Convert from local space to world space
	// W component of vector is 0 as vectors cannot be translated
	float3 normalW = mul(float4(NormalL, 0.0f), World).xyz;
	normalW = normalize(normalW);

	//Compute the reflection vector
	float3 r = reflect(-LightVecW, normalW);

	//Determine how much specular light makes it to eye
	float specularAmount = pow(max(dot(r, toEye), 0.0f), SpecularPower);

	//Compute colour using diffuse lighting only
	float diffuseAmount = max(dot(LightVecW, normalW), 0.0f);
	//Compute diffuse colour
	float3 diffuse = diffuseAmount * (DiffuseMtrl * DiffuseLight).rgb;
	//Compute ambient colour
	float3 ambient = AmbientMtrl * AmbientLight;
	//Compute Specular colour
	float3 specular = specularAmount * (SpecularMtrl * SpecularLight).rbg;

	output.Color.rgb = ambient + diffuse + specular;
	output.Color.a = DiffuseMtrl.a;

    return output;
}


//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS(VS_OUTPUT input) : SV_Target
{
	return input.Color;
}
