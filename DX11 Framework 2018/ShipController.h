#pragma once
#include "GameObject.h"
#include <d3d11_1.h>
#include <d3dcompiler.h>

using namespace DirectX;

class ShipController {
private:
	GameObject* ship = nullptr;

	float maxSpeed = 0.5f;
	float curSpeedX = 0.0f, curSpeedZ = 0.0f;
	float rotationSpeed = 0.004f;

public:
	ShipController(GameObject* _ship);

	void Update();
	float Interpolate(float current, float target, float increment);
	XMFLOAT2 DegreesToVector(float rot);
	float DegreesToRadians(float f);
};