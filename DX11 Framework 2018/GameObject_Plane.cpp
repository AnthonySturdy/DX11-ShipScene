#include "GameObject_Plane.h"

GameObject_Plane::GameObject_Plane(ID3D11Device* _device, std::wstring textureDir, int planeWidth, int planeDepth, vector3 initPos, vector3 initRot, vector3 initScale, Material _material) : GameObject(_device, "", textureDir, initPos, initRot, initScale, _material) {
	//device = _device;
	
	width = planeWidth;
	depth = planeDepth;

	CreateMesh(_device);
}

std::vector<SimpleVertex> GameObject_Plane::CreatePlaneVertices() {
	std::vector<SimpleVertex> returnVec;

	for (int y = 0; y < depth; y++) {
		for (int x = 0; x < width; x++) {
			SimpleVertex vert = { XMFLOAT3(x, 0, y), XMFLOAT3(0, 1, 0), XMFLOAT2(x, y) };
			returnVec.push_back(vert);
		}
	}

	return returnVec;
}

std::vector<int> GameObject_Plane::CreatePlaneIndices() {
	std::vector<int> returnVec;

	for (int y = 0; y < depth - 1; y++) {
		for (int x = 0; x < width - 1; x++) {
			//Triangle 1
			returnVec.push_back((y)* width + (x));
			returnVec.push_back((y + 1) * width + (x));
			returnVec.push_back((y)* width + (x + 1));

			//Triangle 2
			returnVec.push_back((y)* width + (x + 1));
			returnVec.push_back((y + 1) * width + (x));
			returnVec.push_back((y + 1) * width + (x + 1));
		}
	}

	return returnVec;
}

void GameObject_Plane::CreateMesh(ID3D11Device* device) {
	//Generate Vertices and Indices
	std::vector<SimpleVertex> vertices = CreatePlaneVertices();	//Generate vertex positions
	std::vector<int> indices = CreatePlaneIndices();

	//Create mesh object
	mesh = MeshData();
	
	//Create Vertex Buffer then assign to mesh
	D3D11_BUFFER_DESC vbd;
	ZeroMemory(&vbd, sizeof(vbd));
	vbd.Usage = D3D11_USAGE_DEFAULT;
	vbd.ByteWidth = sizeof(SimpleVertex) * vertices.size();
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = &vertices[0];

	device->CreateBuffer(&vbd, &InitData, &vertexBuffer);
	mesh.VertexBuffer = vertexBuffer;

	//Create Index Buffer then assign to mesh
	D3D11_BUFFER_DESC ibd;
	ZeroMemory(&ibd, sizeof(ibd));

	ibd.Usage = D3D11_USAGE_DEFAULT;
	ibd.ByteWidth = sizeof(WORD) * indices.size();
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA InitData2;
	ZeroMemory(&InitData2, sizeof(InitData2));
	InitData2.pSysMem = &indices[0];
	device->CreateBuffer(&ibd, &InitData2, &indexBuffer);;
	mesh.IndexBuffer = indexBuffer;

	//Fill rest of mesh data
	mesh.IndexCount = indices.size();
	mesh.VBOffset = 0;
	mesh.VBStride = sizeof(SimpleVertex);
}


