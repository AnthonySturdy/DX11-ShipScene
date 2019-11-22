#pragma once
#include <vector>
#include "GameObject.h"

class SceneGraphObject {
public:
	GameObject* gameObject;
	vector3 totalParentPos, totalParentRot, totalParentScale;
	SceneGraphObject* parent;
	std::vector<SceneGraphObject*> children;

	SceneGraphObject(GameObject* _GameObject, SceneGraphObject* _Parent);
	~SceneGraphObject();

	void UpdateTransformation(std::vector<GameObject*>* gameobjectList);
};