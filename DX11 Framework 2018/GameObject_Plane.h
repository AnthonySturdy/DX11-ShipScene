#pragma once
#include "GameObject.h"
#include <d3d11_1.h>
#include <d3dcompiler.h>

using namespace DirectX;

class GameObject_Plane : public GameObject {
private:
	int width, depth;

	std::vector<SimpleVertex> vertices;
	std::vector<short> indices;
	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;

	//ID3D11Device* device;

public:
	GameObject_Plane(ID3D11Device* _device, std::wstring textureDir, int planeWidth, int planeDepth, vector3 initPos = vector3(), vector3 initRot = vector3(), vector3 initScale = vector3(), Material _material = Material());
	
	std::vector<SimpleVertex> CreatePlaneVertices();
	std::vector<short> CreatePlaneIndices();
	void CreateMesh(ID3D11Device* device);
};

