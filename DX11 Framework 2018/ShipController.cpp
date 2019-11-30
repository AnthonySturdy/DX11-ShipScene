#include "ShipController.h"

ShipController::ShipController(GameObject* _ship) {
	ship = _ship;
}

void ShipController::Update() {
	XMFLOAT2 shipDirection = DegreesToVector(-ship->GetRotation()->y + 90);

	if (GetAsyncKeyState(0x57)) { //W
		curSpeedX = 0.001f * shipDirection.x;
		curSpeedZ = 0.001f * shipDirection.y;

		ship->SetPosition(vector3(ship->GetPosition()->x + curSpeedX, ship->GetPosition()->y, ship->GetPosition()->z + curSpeedZ));

		//Can only turn when moving
		if (GetAsyncKeyState(0x41)) { //A
			ship->SetRotation(vector3(ship->GetRotation()->z, ship->GetRotation()->y - rotationSpeed, ship->GetRotation()->z));
		}
		if (GetAsyncKeyState(0x44)) { //D
			ship->SetRotation(vector3(ship->GetRotation()->z, ship->GetRotation()->y + rotationSpeed, ship->GetRotation()->z));
		}
	} else {
		curSpeedX = curSpeedZ = 0;
	}
}

XMFLOAT2 ShipController::DegreesToVector(float rot) {
	return XMFLOAT2(cos(DegreesToRadians(rot)), sin(DegreesToRadians(rot)));
}

float ShipController::DegreesToRadians(float f) {
	return (f) * XM_PI / 180;
}

GameObject* ShipController::GetShip() {
	return ship;
}
