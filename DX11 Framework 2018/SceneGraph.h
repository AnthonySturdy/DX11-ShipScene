#pragma once
#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include "SceneGraphObject.h"
#include "Structures.h"
#include "GameObject.h"
#include "GameObject_Plane.h"
#include "vector3.h"
#include "json.hpp"	//https://github.com/nlohmann/json

#include <iostream> //DELETE THIS

using json = nlohmann::json;

class SceneGraph {
private:
	SceneGraphObject* baseObject = nullptr;

	ID3D11Device* device;

public:
	SceneGraph(std::string sceneDir, ID3D11Device* _device);
	~SceneGraph();

	void LoadFromFile(std::string fileDir);
	void CreateSceneGraphObject(SceneGraphObject* parent, json j);
	std::wstring StringToWstring(std::string s);

	SceneGraphObject* GetBase();
};