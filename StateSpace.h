#pragma once
#include "GraphicsObject.h"
#include "Terrain.h"
#include "Controller.h"

class StateSpace
{
public:
	static StateSpace* activeStateSpace;
	vector<Camera*> observers;
	vector<GraphicsObject*> models;
	Terrain* terrain;
	StateSpace(GLFWwindow*);
	~StateSpace();
	void draw();
};

