#pragma once
#include "ScreenState.h"
#include "Menu.h"
#include "StateSpace.h"
#include "SceneEditor.h"

static struct Scenes
{
	static Menu* menu;
	static StateSpace* stateSpace;
	static SceneEditor* sceneEditor;
};

static class SceneManager
{
public:
	SceneManager();
	~SceneManager();

	GLFWwindow* window;

	void changeActiveScene(ScreenState* newState);

	static SceneManager* getInstance();
	ScreenState* getActiveState();

private:
	static SceneManager* instance;
	ScreenState* activeState;
};

