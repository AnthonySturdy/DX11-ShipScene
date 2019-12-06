//--------------------------------------------------------------------------------------
// File: DX11 Framework.fx
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
// Constant Buffer Variables
//--------------------------------------------------------------------------------------
Texture2D txDiffuse : register(t0);
Texture2D txNormal : register(t1);
Texture2D txSpecular : register(t2);
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
    float3 Norm : NORMAL0;
	float3 eyePos : POSITION0;
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

	//Compute vector from the vertex to eye position
	//output.Pos is currently the position in world space
	float3 toEye = normalize(EyePosW - output.Pos.xyz);
	output.eyePos = toEye;

    output.Pos = mul( output.Pos, View );
    output.Pos = mul( output.Pos, Projection );

	// Convert from local space to world space
	// W component of vector is 0 as vectors cannot be translated
	float3 normalW = mul(float4(NormalL, 0.0f), World).xyz;
	output.Norm = normalW;

    return output;
}


//--------------------------------------------------------------------------------------
// Pixel Shader - Specular lighting
//--------------------------------------------------------------------------------------
float4 PS(PS_INPUT input) : SV_Target
{
	float3 texNormal = txNormal.Sample(samLinear, input.Tex).rgb;	//Get normal map values
	//txtNormal = normalize(texNormal * 2.0 - 1.0);	//Convert to -1
	float3 normalW = normalize(input.Norm) + texNormal;

	//Compute the reflection vector
	float3 r = reflect(-LightVecW, normalW);
	//Determine how much specular light makes it to eye
	float specularAmount = pow(max(dot(r, input.eyePos), 0.0f), SpecularPower);
	//Compute colour using diffuse lighting only
	float diffuseAmount = max(dot(LightVecW, normalW), 0.0f);
	//Compute diffuse colour
	float3 diffuse = diffuseAmount * (DiffuseMtrl * DiffuseLight).rgb;
	//Compute ambient colour
	float3 ambient = AmbientMtrl * AmbientLight;
	//Compute Specular colour
	float3 specular = specularAmount * (SpecularMtrl * SpecularLight).rbg * txSpecular.Sample(samLinear, input.Tex);

	//Texture colour
	float4 texCol = txDiffuse.Sample(samLinear, input.Tex);

	clip(texCol.a-0.15f);

	float dist = input.Pos.z / input.Pos.w;		//Distance to pixel being rendered
	float4 fogCol = float4(0.8f, 0.8f, 0.8f, 1.0f);		//Colour of fog

	float4 outCol;
	outCol.rgb = texCol + ambient + diffuse + specular;
	outCol.a = DiffuseMtrl.a;

	outCol = lerp(fogCol, outCol, saturate(dist * 25));	//Interpolate from output colour to fog colour based on distance

	return saturate(outCol);
}
