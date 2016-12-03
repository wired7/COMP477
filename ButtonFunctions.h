#pragma once
#include "SceneManager.h"

inline void backToMenu()
{
	delete SceneManager::getInstance()->getActiveState();
	Controller::setController(MenuController::getController());
	SceneManager::getInstance()->changeActiveScene(Scenes::menu);
}