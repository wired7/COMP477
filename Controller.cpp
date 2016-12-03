#include "Controller.h"

Controller* Controller::activeController = NULL;

void Controller::setController(Controller* controller)
{
	activeController = controller;

	glfwSetKeyCallback(Camera::activeCamera->window, activeController->key_callback);
	glfwSetScrollCallback(Camera::activeCamera->window, activeController->scroll_callback);
	glfwSetMouseButtonCallback(Camera::activeCamera->window, activeController->mouse_callback);
	glfwSetCursorPosCallback(Camera::activeCamera->window, activeController->mousePos_callback);
	glfwSetWindowSizeCallback(Camera::activeCamera->window, activeController->windowResize_callback);
}

glm::vec2 Controller::mouseScreenToGUICoords(GLFWwindow* window, double x, double y)
{
	glm::vec2 coords;

	float guiScaleX;
	float guiScaleY;

	float GUI_RES_X = 1200;
	float GUI_RES_Y = 800;

	guiScaleX = Camera::activeCamera->getScreenWidth() / GUI_RES_X;
	guiScaleY = Camera::activeCamera->getScreenHeight() / GUI_RES_Y;

	coords = glm::vec2(x / guiScaleX, GUI_RES_Y - y / guiScaleY);

	return coords;
}

StateSpaceController* StateSpaceController::controller = NULL;

StateSpaceController::StateSpaceController()
{
	key_callback = kC;
	scroll_callback = sC;
	mouse_callback = mC;
	mousePos_callback = mPC;
	windowResize_callback = wRC;
}

StateSpaceController::~StateSpaceController()
{
}

void StateSpaceController::kC(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_P && action == GLFW_PRESS)
	{
		StateSpace::activeStateSpace->playModeOn ^= true;
	}

	if ((key == GLFW_KEY_W))
	{
		InputState::wPressed = true;
		StateSpaceCamera* activeCam = (StateSpaceCamera*)Camera::activeCamera;
		activeCam->translate(0.1f * vec2(1.0f));
		activeCam->update();
	}

	if ((key == GLFW_KEY_S))
	{
		InputState::sPressed = true;
		StateSpaceCamera* activeCam = (StateSpaceCamera*)Camera::activeCamera;
		activeCam->translate(-0.1f * vec2(1.0f));
		activeCam->update();
	}


	if ((key == GLFW_KEY_LEFT_SHIFT || key == GLFW_KEY_RIGHT_SHIFT) && action == GLFW_PRESS)
	{
		InputState::shiftPressed = true;
	}

	if ((key == GLFW_KEY_LEFT_CONTROL || key == GLFW_KEY_RIGHT_CONTROL) && action == GLFW_PRESS)
	{
		InputState::controlPressed = true;
	}

	if ((key == GLFW_KEY_LEFT_ALT || key == GLFW_KEY_RIGHT_ALT) && action == GLFW_PRESS)
	{
		InputState::altPressed = true;
	}

	if (key == GLFW_KEY_DELETE && action == GLFW_PRESS)
	{

	}

	if ((key == GLFW_KEY_LEFT_SHIFT || key == GLFW_KEY_RIGHT_SHIFT) && action == GLFW_RELEASE)
	{
		InputState::shiftPressed = false;
	}

	if ((key == GLFW_KEY_LEFT_CONTROL || key == GLFW_KEY_RIGHT_CONTROL) && action == GLFW_RELEASE)
	{
		InputState::controlPressed = false;
	}

	if ((key == GLFW_KEY_LEFT_ALT || key == GLFW_KEY_RIGHT_ALT) && action == GLFW_RELEASE)
	{
		InputState::altPressed = false;
	}
}

void StateSpaceController::sC(GLFWwindow* window, double xOffset, double yOffset)
{

	StateSpaceCamera* activeCam = (StateSpaceCamera*)Camera::activeCamera;
	activeCam->camTheta -= 0.1 * (GLfloat)xOffset;

	if (InputState::shiftPressed)
	{
		if (abs(activeCam->camPhi + 0.1 * (GLfloat)yOffset) < 3.1415 / 2)
			activeCam->camPhi += 0.1 * (GLfloat)yOffset;
	}
	else
		activeCam->translate(0.1f * vec2(yOffset, yOffset));

	activeCam->update();
}

void StateSpaceController::mC(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
	{
		InputState::mouseButtonLeftPressed = true;
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	}
	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
	{

	}
	if ((button == GLFW_MOUSE_BUTTON_LEFT || button == GLFW_MOUSE_BUTTON_RIGHT) && action == GLFW_RELEASE)
	{
		InputState::mouseButtonLeftPressed = false;
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}
}

void StateSpaceController::mPC(GLFWwindow* window, double xpos, double ypos)
{
	double deltaX = xpos - InputState::previousMousePosX;
	double deltaY = ypos - InputState::previousMousePosY;

	if (InputState::mouseButtonLeftPressed)
	{
		StateSpaceCamera* activeCam = (StateSpaceCamera*)Camera::activeCamera;
		activeCam->camTheta += 0.005 * (GLfloat)deltaX;
		activeCam->camPhi -= 0.005 * (GLfloat)deltaY;

		activeCam->update();
	}

	InputState::previousMousePosX = xpos;
	InputState::previousMousePosY = ypos;

	InputState::mouseCoords = glm::vec2(xpos, ypos);
	InputState::mouseGuiCoords = mouseScreenToGUICoords(window, xpos, ypos);
}

void StateSpaceController::wRC(GLFWwindow*, int, int)
{
	Camera::activeCamera->update();
}

Controller* StateSpaceController::getController()
{
	if (controller == NULL)
	{
		controller = new StateSpaceController();
	}

	return controller;
}

EditorController* EditorController::controller = NULL;

MenuController* MenuController::controller = NULL;

MenuController::MenuController()
{
	key_callback = kC;
	scroll_callback = sC;
	mouse_callback = mC;
	mousePos_callback = mPC;
	windowResize_callback = wRC;
}

MenuController::~MenuController()
{

}
void MenuController::kC(GLFWwindow*, int, int, int, int)
{

}
void MenuController::sC(GLFWwindow*, double, double)
{
}
void MenuController::mC(GLFWwindow* window , int button, int action, int mods)
{
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}
void MenuController::mPC(GLFWwindow* window, double xpos, double ypos)
{
	InputState::mouseCoords = glm::vec2(xpos, ypos);
	InputState::mouseGuiCoords = mouseScreenToGUICoords(window, xpos, ypos);
}
void MenuController::wRC(GLFWwindow*, int, int)
{
	Camera::activeCamera->update();
}

Controller* MenuController::getController()
{
	if (controller == NULL)
		controller = new MenuController();

	return controller;
}
