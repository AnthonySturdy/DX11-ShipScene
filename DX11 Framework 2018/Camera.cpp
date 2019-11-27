#include "Camera.h"

Camera::Camera(XMFLOAT3 _position, XMFLOAT3 _at, XMFLOAT3 _up, float _windowWidth, float _windowHeight, float _nearDepth, float _farDepth) {
	eye = _position;
	at = _at;
	up = _up;
	rotation = XMFLOAT3(0, 0, 0);
	windowWidth = _windowWidth;
	windowHeight = _windowHeight;
	nearDepth = _nearDepth;
	farDepth = _farDepth;

	Update();
}

void Camera::Update() {
	XMVECTOR e = { eye.x, eye.y, eye.z };
	XMVECTOR a = { at.x, at.y, at.z };
	XMVECTOR u = { up.x, up.y, up.z };
	XMStoreFloat4x4(&view, XMMatrixLookAtLH(e, a, u));	//Set view matrix

	XMStoreFloat4x4(&projection, XMMatrixPerspectiveFovLH(XM_PIDIV2, windowWidth / (FLOAT)windowHeight, nearDepth, farDepth));	//Set projection matrix
}

XMFLOAT3 Camera::GetEye() {
	return eye;
}

XMFLOAT3 Camera::GetAt() {
	return at;
}

XMFLOAT3 Camera::GetUp() {
	return up;
}

void Camera::SetEye(XMFLOAT3 _position) {
	eye = _position;
}

void Camera::SetAt(XMFLOAT3 _at) {
	at = _at;
}

void Camera::SetUp(XMFLOAT3 _up) {
	up = _up;
}

XMMATRIX Camera::GetViewMatrix() {
	return XMLoadFloat4x4(&view);
}

XMMATRIX Camera::GetProjectionMatrix() {
	return XMLoadFloat4x4(&projection);
}

XMMATRIX Camera::GetViewProjectionMatrix() {
	XMMATRIX v = XMLoadFloat4x4(&view);
	XMMATRIX p = XMLoadFloat4x4(&projection);
	return XMMatrixMultiply(v, p);
}

void Camera::Reshape(float _windowWidth, float _windowHeight, float _nearDepth, float _farDepth) {
	windowWidth = _windowWidth;
	windowHeight = _windowHeight;
	nearDepth = _nearDepth;
	farDepth = _farDepth;
}
