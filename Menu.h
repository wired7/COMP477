#pragma once
#include "GraphicsObject.h"
#include "Camera.h"

class Menu
{
public:
	Camera* observer;
	ImportedMesh* bucket;
	Rectangle* rectangle;
	Menu(GLFWwindow*);
	~Menu();
	void draw();
};

