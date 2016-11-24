#pragma once
#include <glew.h>
#include <glfw3.h>
#include <vector>
#include <functional>
#include "InputState.h"
#include "Camera.h"
#include "StateSpace.h"

class Controller
{
public:
	static Controller* activeController;
	static void setController(Controller*);
	void(*key_callback)(GLFWwindow*, int, int, int, int) = NULL;
	void(*scroll_callback)(GLFWwindow*, double, double) = NULL;
	void(*mouse_callback)(GLFWwindow*, int, int, int) = NULL;
	void(*mousePos_callback)(GLFWwindow*, double, double) = NULL;
	void(*windowResize_callback)(GLFWwindow*, int, int) = NULL;
};

class StateSpaceController : public Controller
{
private:
	static StateSpaceController* controller;
	StateSpaceController();
	~StateSpaceController();
	static void kC(GLFWwindow*, int, int, int, int);
	static void sC(GLFWwindow*, double, double);
	static void mC(GLFWwindow*, int, int, int);
	static void mPC(GLFWwindow*, double, double);
	static void wRC(GLFWwindow*, int, int);
public:
	static Controller* getController();
};

class EditorController : public Controller
{
private:
	static EditorController* controller;
	EditorController();
	~EditorController();
	static void kC(GLFWwindow*, int, int, int, int);
	static void sC(GLFWwindow*, double, double);
	static void mC(GLFWwindow*, int, int, int);
	static void mPC(GLFWwindow*, double, double);
	static void wRC(GLFWwindow*, int, int);
public:
	static Controller* getController();
};