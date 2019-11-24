#include "SceneGraph.h"

SceneGraph::SceneGraph(std::string sceneDir, ID3D11Device* _device) {
	//baseObject = new SceneGraphObject(base, nullptr);
	device = _device;
	LoadFromFile(sceneDir);
}

SceneGraph::~SceneGraph() {
	delete baseObject;
}

void SceneGraph::LoadFromFile(std::string fileDir) {
	std::ifstream inFile;
	inFile.open(fileDir);
	if (!inFile.is_open())
		return;

	std::stringstream ss;
	ss << inFile.rdbuf();		//https://stackoverflow.com/questions/2602013/read-whole-ascii-file-into-c-stdstring
	
	json j = json::parse(ss.str());
	CreateSceneGraphObject(nullptr, j["0"]);
	int i = 0;
}

void SceneGraph::CreateSceneGraphObject(SceneGraphObject* parent, json j) {
	GameObject* go;
	json obj = j;	//Create a copy to use for GameObject creation. Checking if elements exist creates the element which we don't want

	if (obj["Type"] == "GameObject") {
		go = new GameObject(
			device, 
			obj["ModelDirectory"], 
			StringToWstring(obj["TextureDirectory"]),
			(obj["Position"] == nullptr ? vector3() : vector3(obj["Position"]["x"], obj["Position"]["y"], obj["Position"]["z"])),
			(obj["Rotation"] == nullptr ? vector3() : vector3(obj["Rotation"]["x"], obj["Rotation"]["y"], obj["Rotation"]["z"])),
			(obj["Scale"] == nullptr ? vector3() : vector3(obj["Scale"]["x"], obj["Scale"]["y"], obj["Scale"]["z"])),
			Material(),	//TODO: Might get back to this (going to be a headache)
			(obj["ShaderType"] == nullptr ? (ShaderType)0 : (ShaderType)obj["ShaderType"]));	//0 = ShaderType.DEFAULT
	} else {	//GameObject_Plane
		go = new GameObject_Plane(
			device,
			StringToWstring(obj["TextureDirectory"]),
			obj["PlaneWidth"],
			obj["PlaneHeight"],
			(obj["Position"] == nullptr ? vector3() : vector3(obj["Position"]["x"], obj["Position"]["y"], obj["Position"]["z"])),
			(obj["Rotation"] == nullptr ? vector3() : vector3(obj["Rotation"]["x"], obj["Rotation"]["y"], obj["Rotation"]["z"])),
			(obj["Scale"] == nullptr ? vector3() : vector3(obj["Scale"]["x"], obj["Scale"]["y"], obj["Scale"]["z"])),
			Material(),	//TODO: Might get back to this (going to be a headache)
			(obj["ShaderType"] == nullptr ? (ShaderType)0 : (ShaderType)obj["ShaderType"]));	//0 = ShaderType.DEFAULT
	}

	SceneGraphObject* graphObj = new SceneGraphObject(go, parent);
	if (baseObject == nullptr)
		baseObject = graphObj;

	if (parent != nullptr)
		parent->children.push_back(graphObj);

	for (auto& x : j.items()) {
		std::string key = x.key();
		if (key != "Type" && key != "TextureDirectory" && key != "ModelDirectory" && key != "PlaneWidth" && key != "PlaneHeight" && key != "Position" && key != "Rotation" && key != "Scale" && key != "ShaderType") {	//If is not GameObject property
			CreateSceneGraphObject(graphObj, j[key]);
		}
	}
}

std::wstring SceneGraph::StringToWstring(std::string s) {
	return std::wstring(s.begin(), s.end());
}


SceneGraphObject* SceneGraph::GetBase() {
	return baseObject;
}
