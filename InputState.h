#pragma once
#include <glew.h>
#include <glfw3.h>
#include <vector>

class InputState
{
public:
	static bool mouseDragged;
	static bool shiftPressed;
	static bool controlPressed;
	static bool altPressed;
	static bool wireframeModeOn;
	static bool surfaceModeOn;
	static bool mouseButtonLeftPressed;
	static bool wPressed;
	static bool sPressed;
	static double previousMousePosX;
	static double previousMousePosY;

};