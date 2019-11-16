#include "GameObject.h"

GameObject::GameObject(ID3D11Device* _device, std::string modelDir, std::wstring textureDir, vector3 initPos, vector3 initRot, vector3 initScale, Material _material, ShaderType _shaderType) {
	//Load model into MeshData
	if(modelDir != "")	//modelDir is set to "" when object is a plane, so we don't need to load a mesh
		mesh = OBJLoader::Load(const_cast<char*>(modelDir.c_str()), _device);	//Takes char* not const char* so did a const_cast

	//Load Texture
	CreateDDSTextureFromFile(_device, textureDir.c_str(), nullptr, &texture);

	//Set intitial positions
	position = initPos;
	rotation = initRot;
	scale = initScale;
	if (initScale.x == 0 && initScale.y == 0 && initScale.z == 0)
		scale = vector3(1, 1, 1);

	//Set material
	material = _material;

	//Set shader type
	shaderType = _shaderType;
}

GameObject::~GameObject() {
	delete texture;
	texture = nullptr;
}

MeshData* GameObject::GetMesh() {
	return &mesh;
}

vector3* GameObject::GetPosition() {
	return &position;
}

vector3* GameObject::GetRotation() {
	return &rotation;
}

vector3* GameObject::GetScale() {
	return &scale;
}

XMFLOAT4X4* GameObject::GetWorldMatrix() {
	//Create world matrix from Position, Rotation and Scale values
	XMStoreFloat4x4(&worldMatrix, XMMatrixScaling(scale.x, scale.y, scale.z) * 
									XMMatrixRotationX(rotation.x) * XMMatrixRotationY(rotation.y) * XMMatrixRotationZ(rotation.z) * 
									XMMatrixTranslation(position.x, position.y, position.z));

	return &worldMatrix;
}

Material* GameObject::GetMaterial() {
	return &material;
}

ID3D11ShaderResourceView** GameObject::GetTexture() {
	return &texture;
}

ShaderType GameObject::GetShaderType() {
	return shaderType;
}

void GameObject::SetPosition(vector3 newPos) {
	position = newPos;
}

void GameObject::SetRotation(vector3 newRot) {
	rotation = newRot;
}

void GameObject::SetScale(vector3 newScale) {
	scale = newScale;
}
