#include "SceneGraphObject.h"


SceneGraphObject::SceneGraphObject(GameObject* _GameObject, SceneGraphObject* _Parent) {
	gameObject = _GameObject;
	parent = _Parent;
}

SceneGraphObject::~SceneGraphObject() {
	delete gameObject;
	for (int i = 0; i < children.size(); i++) {
		delete children[i];
	}
	children.clear();
}

void SceneGraphObject::UpdateTransformation(std::vector<GameObject*>* gameobjectList) {
	gameobjectList->push_back(gameObject);

	if (parent == nullptr) {
		//Set position of all parents combined
		totalParentPos = *(gameObject->GetPosition());
		totalParentRot = *(gameObject->GetRotation());
		totalParentScale = *(gameObject->GetScale());

		vector3* pos = gameObject->GetPosition();
		vector3* rot = gameObject->GetRotation();
		vector3* scale = gameObject->GetScale();
		XMFLOAT4X4 newMtx;
		XMStoreFloat4x4(&newMtx, XMMatrixScaling(scale->x, scale->y, scale->z) *		//Apply scale
								XMMatrixRotationRollPitchYaw(DegreesToRadians(rot->x), DegreesToRadians(rot->y), DegreesToRadians(rot->z)) *	//Apply rotation
								XMMatrixTranslation(pos->x, pos->y, pos->z));			//Apply position
		gameObject->SetWorldMatrix(newMtx);
	} else {
		vector3* pos = gameObject->GetPosition();
		vector3* parentPos = &parent->totalParentPos;
		vector3* rot = gameObject->GetRotation();
		vector3* parentRot = &parent->totalParentRot;
		vector3* scale = gameObject->GetScale();
		vector3* parentScale = &parent->totalParentScale;
		XMFLOAT4X4 newMtx;
		XMStoreFloat4x4(&newMtx, XMMatrixScaling(scale->x, scale->y, scale->z) *						//Apply scale
								XMMatrixScaling(parentScale->x, parentScale->y, parentScale->z) *		//Apply parent scale
								XMMatrixRotationRollPitchYaw(DegreesToRadians(rot->x), DegreesToRadians(rot->y), DegreesToRadians(rot->z)) *					//Apply rotation
								XMMatrixTranslation(pos->x, pos->y, pos->z) *							//Apply position
								XMMatrixRotationRollPitchYaw(DegreesToRadians(parentRot->x), DegreesToRadians(parentRot->y), DegreesToRadians(parentRot->z)) *	//Apply parent rotation
								XMMatrixTranslation(parentPos->x, parentPos->y, parentPos->z));			//Apply parent position
		gameObject->SetWorldMatrix(newMtx);

		//Set position of all parents combined
		totalParentPos = *(gameObject->GetPosition()) + parent->totalParentPos;
		totalParentRot = *(gameObject->GetRotation()) + parent->totalParentRot;
		totalParentScale = *(gameObject->GetScale()) + parent->totalParentScale;
	}

	for (int i = 0; i < children.size(); i++) {
		children[i]->UpdateTransformation(gameobjectList);
	}
}

float SceneGraphObject::DegreesToRadians(float f) {
	return (f) * XM_PI / 180;
}
