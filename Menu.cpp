#include "Menu.h"
#include <glfw3.h>
#include "OpenFileDialog.h"
#include "SaveFileDialog.h"
#include "Skybox.h"
#include "StateSpace.h"
#include "SceneManager.h"

GLFWwindow* windowHandle;

void newSimulation()
{
	
}

void runSimulation()
{
	StateSpace::activeStateSpace->loadAnimation();
	SceneManager::getInstance()->changeActiveScene(Scenes::stateSpace);
}

void exitSim()
{
	glfwSetWindowShouldClose(windowHandle, 1);
}

Menu::Menu(GLFWwindow* window)
{
	this->window = window;
	windowHandle = window;
	observer = new MenuCamera(window, vec2(0, 0), vec2(1, 1), vec3(0, 0, 0), vec3(0, 0, -1), vec3(0, 1, 0), perspective(45.0f, 1.0f, 0.1f, 100.0f));
	Camera::activeCamera = observer;
	Controller::setController(MenuController::getController());
	//	bucket = new ImportedMesh("models\\bucket.3DS", vec3(0, 0.3, 0), vec3(0.5, 0.1, 0.1));
	//rectangle = new Rectangle(vec3(100, 1, 1), vec3(2, 1, 1), vec4(0, 0, 0, 1.0), true);
	menuButtons.push_back(new GUIButton(vec3(600, 600, 0), vec3(180, 70, 1), vec4(1.0, 1.0, 1.0, 1.0), "Create Simulation", "textures\\button.png", true, newSimulation));
	menuButtons.push_back(new GUIButton(vec3(600, 450, 0), vec3(180, 70, 1), vec4(1.0, 1.0, 1.0, 1.0), "Run Simulation", "textures\\button.png", true, runSimulation));
	menuButtons.push_back(new GUIButton(vec3(600, 300, 0), vec3(180, 70, 1), vec4(1.0, 1.0, 1.0, 1.0), "Exit Program", "textures\\button.png", true, exitSim));
	background = new GUIBackground(vec3(0.0f, 0.0f, 0.0f), 
								   vec3(1200, 
								   800, 1.0f), 
								   vec4(1.0f, 1.0f, 1.0f, 1.0f), "textures\\menuBack.jpg", true);
}


Menu::~Menu()
{
}

void Menu::execute()
{
	this->activeState = activeState;
	draw();
	checkInput();
}

void Menu::draw()
{
	glDisable(GL_DEPTH_TEST);

	observer->setViewport();

	background->draw();

	for (int i = 0; i < menuButtons.size(); i++)
		menuButtons[i]->draw();

	//	bucket->draw();
}

void Menu::checkInput()
{
	//hover
	for (int i = 0; i < menuButtons.size(); i++)
	{
		menuButtons[i]->checkHover();
	}

	static int oldLeftClickState = GLFW_RELEASE;

	int leftClick = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
	if (leftClick == GLFW_PRESS && oldLeftClickState == GLFW_RELEASE)
	{
		oldLeftClickState = GLFW_PRESS;
		for (int i = 0; i < menuButtons.size(); i++)
			menuButtons[i]->checkMouseClick();

	}

	oldLeftClickState = leftClick;
}

