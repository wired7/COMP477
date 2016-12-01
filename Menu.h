#pragma once
#include "GraphicsObject.h"
#include "Camera.h"

class Menu
{
public:
	Camera* observer;
	//	ImportedMesh* bucket;
	GUIButton* button;
	Menu(GLFWwindow*);
	~Menu();
	void draw();
};

