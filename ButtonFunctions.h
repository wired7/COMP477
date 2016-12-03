#pragma once
#include "SceneManager.h"

inline void backToMenu()
{
	delete SceneManager::getInstance()->getActiveState();
	SceneManager::getInstance()->changeActiveScene(Scenes::menu);
}