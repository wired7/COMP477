#pragma once
#include "GraphicsObject.h"
#include "Skybox.h"
#include "Terrain.h"
#include "Controller.h"

class StateSpace
{
public:
	static StateSpace* activeStateSpace;
	vector<Camera*> observers;
	vector<GraphicsObject*> models;
	Skybox* skybox;
	Terrain* terrain;
	StateSpace(GLFWwindow*, Skybox*);
	~StateSpace();
	void draw();
};

