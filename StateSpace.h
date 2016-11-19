#pragma once
#include "GraphicsObject.h"
#include "Skybox.h"
#include "Terrain.h"
#include "Controller.h"
#include <chrono>

using namespace std::chrono;

class StateSpace
{
public:
	static StateSpace* activeStateSpace;
	bool wireframeModeOn;
	bool surfaceModeOn;
	bool reflectionsOn;
	bool refractionOn;
	bool playModeOn;
	int frameCount;
	double time;
	vector<vector<vec3>> frames;
	vector<Camera*> observers;
	vector<GraphicsObject*> models;
	Skybox* skybox;
	Terrain* terrain;
	StateSpace(GLFWwindow*, Skybox*);
	~StateSpace();
	void draw();
};

