#include "SceneGraph.h"

SceneGraph::SceneGraph(GameObject* base) {
	baseObject = new SceneGraphObject(base, nullptr);
}

SceneGraph::~SceneGraph() {
	delete baseObject;
}

SceneGraphObject* SceneGraph::GetBase() {
	return baseObject;
}
