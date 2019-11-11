#pragma once
#include <d3d11_1.h>
#include <directxmath.h>

using namespace DirectX;

class Camera {
private:
	XMFLOAT3 eye;
	XMFLOAT3 at;
	XMFLOAT3 up;

	float windowWidth;
	float windowHeight;
	float nearDepth;
	float farDepth;

	XMFLOAT4X4 view;
	XMFLOAT4X4 projection;

public:
	Camera(XMFLOAT3 _position, XMFLOAT3 _at, XMFLOAT3 _up, float _windowWidth, float _windowHeight, float _nearDepth, float _farDepth);
	~Camera();

	void Update();	//Make the current view and projection matrices

	XMFLOAT3 GetEye();
	XMFLOAT3 GetAt();
	XMFLOAT3 GetUp();
	void SetEye(XMFLOAT3 _position);
	void SetAt(XMFLOAT3 _at);
	void SetUp(XMFLOAT3 _up);
	
	XMMATRIX GetViewMatrix();
	XMMATRIX GetProjectionMatrix();
	XMMATRIX GetViewProjectionMatrix();

	void Reshape(float _windowWidth, float _windowHeight, float _nearDepth, float _farDepth);
};