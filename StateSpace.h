#pragma once
#include "GraphicsObject.h"
#include "Skybox.h"
#include "Terrain.h"
#include "Controller.h"
#include <mutex>
#include "ScreenState.h"
#include <glfw3.h>

class StateSpace : public ScreenState
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
	char* fileName;
	vector<vector<vec3>>* framesFront;
	vector<vector<vec3>>* framesBack;
	vector<Camera*> observers;
	vector<InstancedMeshObject*> instancedModels;
	vector<MeshObject*> models;
	Skybox* skybox;
	Terrain* terrain;
	StateSpace(GLFWwindow*, Skybox*);
	~StateSpace();
	void draw();
	void updateFrames();
	void loadFramesInBack();
	void swap(vector<vector<vec3>>* p1, vector<vector<vec3>>* p2);
	void initializeFrameRead();
	void clearFrameRead();
	int framesBuffSize;
	int totalFrames = 0;
	int totalFramesLoaded;
	int currGlobalFrame;
	GLFWwindow* window;

	int loadAnimation();
	void execute();
	void checkInput();

private:
	std::vector<GUIButton*> buttons;
};

