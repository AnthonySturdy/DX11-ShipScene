#pragma once
#include <vector>
#include "SceneGraphObject.h"
#include "Structures.h"
#include "GameObject.h"
#include "vector3.h"

class SceneGraph {
private:
	SceneGraphObject* baseObject;

public:
	SceneGraph(GameObject* base);
	~SceneGraph();

	SceneGraphObject* GetBase();
};