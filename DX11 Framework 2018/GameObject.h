#pragma once
#include "OBJLoader.h"
#include "Structures.h"
#include "DDSTextureLoader.h"
#include "vector3.h"
#include <directxmath.h>
#include <string>
#include <sstream>

class GameObject {
protected:
	MeshData mesh;
	vector3 position;
	vector3 rotation;
	vector3 scale;
	Material material;
	ID3D11ShaderResourceView* diffuseTex = nullptr;
	ID3D11ShaderResourceView* normalTex = nullptr;
	ID3D11ShaderResourceView* specularTex = nullptr;
	XMFLOAT4X4 worldMatrix;
	ShaderType shaderType;

	std::wstring AddSuffixBeforeExtension(std::wstring str, std::wstring suffix, std::wstring extension);

public:
	GameObject(ID3D11Device* _device, std::string modelDir, std::wstring textureDir, vector3 initPos = vector3(), vector3 initRot = vector3(), vector3 initScale = vector3(), Material _material = Material(), ShaderType _shaderType = ShaderType::DEFAULT);
	~GameObject();

	MeshData* GetMesh();
	vector3* GetPosition();
	vector3* GetRotation();
	vector3* GetScale();
	XMFLOAT4X4* GetWorldMatrix();
	Material* GetMaterial();
	ID3D11ShaderResourceView** GetDiffuseTexture();
	ID3D11ShaderResourceView** GetNormalTexture();
	ID3D11ShaderResourceView** GetSpecularTexture();
	ShaderType GetShaderType();

	void SetPosition(vector3 newPos);
	void SetRotation(vector3 newRot);
	void SetScale(vector3 newScale);
	void UpdateWorldMatrix();
	void SetWorldMatrix(XMFLOAT4X4 mtx);
};