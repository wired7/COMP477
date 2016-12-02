#pragma once
#include "GraphicsObject.h"
#include "Camera.h"
#include "ScreenState.h"

class Menu : public ScreenState
{
public:
	Camera* observer;
	//	ImportedMesh* bucket;
	Menu(GLFWwindow* window);
	~Menu();
	void draw();
	void checkInput();
	GLFWwindow* window;

	void execute();

private:
	ScreenState* activeState;
	std::vector<GUIButton*> menuButtons;
	GUIBackground* background;
};

