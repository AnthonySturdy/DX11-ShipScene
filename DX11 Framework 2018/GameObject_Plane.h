#pragma once
#include "GameObject.h"
#include <d3d11_1.h>
#include <d3dcompiler.h>

using namespace DirectX;

class GameObject_Plane : public GameObject {
private:
	int width, depth;

	std::vector<SimpleVertex> vertices;
	std::vector<unsigned short> indices;
	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;


public:
	GameObject_Plane(ID3D11Device* _device, std::wstring textureDir, int planeWidth, int planeDepth, vector3 initPos = vector3(), vector3 initRot = vector3(), vector3 initScale = vector3(), Material _material = Material(), ShaderType _shaderType = ShaderType::DEFAULT);
	
	std::vector<SimpleVertex> CreatePlaneVertices();
	std::vector<unsigned short> CreatePlaneIndices();
	void CreateMesh(ID3D11Device* device);
};

