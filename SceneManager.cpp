#include "SceneManager.h"

SceneManager* SceneManager::instance = nullptr;
Menu* Scenes::menu = nullptr;
StateSpace* Scenes::stateSpace = nullptr;
SceneEditor* Scenes::sceneEditor = nullptr;

SceneManager::SceneManager()
{
}

SceneManager::~SceneManager()
{
}

ScreenState* SceneManager::getActiveState()
{
	return activeState;
}

void SceneManager::changeActiveScene(ScreenState* newState)
{
	activeState = newState;
}

SceneManager* SceneManager::getInstance()
{
	if (instance == nullptr)
		instance = new SceneManager();
	return instance;
}
