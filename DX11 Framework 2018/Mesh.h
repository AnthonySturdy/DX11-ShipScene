#pragma once

#include <windows.h>
#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <directxcolors.h>

#include "resource.h"
#include "Structs.h"

using namespace DirectX;

class Mesh {
public:
	void Update();
	void Draw();
};

