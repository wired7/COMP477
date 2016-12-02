#pragma once
#include <glew.h>
#include <glfw3.h>
#include <vector>
#include <glm.hpp>

class InputState
{
public:
	static bool mouseDragged;
	static bool shiftPressed;
	static bool controlPressed;
	static bool altPressed;
	static bool wireframeModeOn;
	static bool surfaceModeOn;
	static bool mouseLeftButtonPrevState;
	static bool mouseButtonLeftPressed;
	static bool wPressed;
	static bool sPressed;
	static glm::vec2 mouseCoords;
	static glm::vec2 mouseGuiCoords;
	static double previousMousePosX;
	static double previousMousePosY;

};