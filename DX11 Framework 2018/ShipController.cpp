#include "ShipController.h"

ShipController::ShipController(GameObject* _ship) {
	ship = _ship;
}

void ShipController::Update() {
	XMFLOAT2 shipDirection = DegreesToVector(-ship->GetRotation()->y + 90);

	if (GetAsyncKeyState(0x57)) { //W
		curSpeedX = Interpolate(curSpeedX, maxSpeed, 0.001f) * shipDirection.x;
		curSpeedZ = Interpolate(curSpeedZ, maxSpeed, 0.001f) * shipDirection.y;

		ship->SetPosition(vector3(ship->GetPosition()->x + curSpeedX, ship->GetPosition()->y, ship->GetPosition()->z + curSpeedZ));
	} 
	if (GetAsyncKeyState(0x41)) { //A
		ship->SetRotation(vector3(ship->GetRotation()->z, ship->GetRotation()->y - rotationSpeed, ship->GetRotation()->z));
	}
	if (GetAsyncKeyState(0x44)) { //D
		ship->SetRotation(vector3(ship->GetRotation()->z, ship->GetRotation()->y + rotationSpeed, ship->GetRotation()->z));
	}
}

float ShipController::Interpolate(float current, float target, float increment) {
	if (current + increment >= target)
		return target;
	else
		return current + increment;
}

XMFLOAT2 ShipController::DegreesToVector(float rot) {
	return XMFLOAT2(cos(DegreesToRadians(rot)), sin(DegreesToRadians(rot)));
}

float ShipController::DegreesToRadians(float f) {
	return (f)* XM_PI / 180;
}
