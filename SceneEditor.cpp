#pragma once
#include "SceneEditor.h"
#include "Controller.h"
#include "SceneManager.h"
#include "ButtonFunctions.h"

void increment(float& value)
{
	value++;
}

void decrement(float& value)
{
	value--;
}

void acceptChanges()
{
	float maxValue = 8.0f;

	vec3 center(0.0f, 0.0f, 0.0f);
	vec3 dimensions(5.0f, 5.0f, 5.0f);

	vec4 color(Scenes::sceneEditor->color.r / maxValue, Scenes::sceneEditor->color.g / maxValue, Scenes::sceneEditor->color.b / maxValue, Scenes::sceneEditor->color.a / maxValue);

	Scenes::sceneEditor->rigidbodies.push_back(new Cube(center, dimensions, color, true));

	Scenes::sceneEditor->displayOptions = false;
}

GUIValueChanger::GUIValueChanger(vec3 position, float& ValueToChange) : pos(position), value(ValueToChange)
{
	inc = new GUIButtonValued(vec3(pos.x + 20, pos.y, 0), vec3(16, 16, 1), vec4(1.0, 1.0, 1.0, 1.0), " +", vec4(0.0f, 0.0f, 0.0f, 1.0f), "textures\\button.png", true, increment, value, 12);
	dec = new GUIButtonValued(vec3(pos.x - 20, pos.y, 0), vec3(16, 16, 1), vec4(1.0, 1.0, 1.0, 1.0), " -", vec4(0.0f, 0.0f, 0.0f, 1.0f), "textures\\button.png", true, decrement, value, 12);
}

GUIValueChanger::~GUIValueChanger()
{

}

void GUIValueChanger::draw()
{
	inc->draw();
	dec->draw();

	textRend.RenderText(std::to_string((int)value), pos.x - 5.0f, pos.y - 3.0f, 1.0f, glm::vec3(0.0f, 0.0f, 0.0f), Camera::activeCamera->getScreenWidth(), Camera::activeCamera->getScreenHeight(), 14);
}

void GUIValueChanger::checkHover()
{
	inc->checkHover();
	dec->checkHover();
}

bool GUIValueChanger::checkMouseClick()
{
	if (inc->checkMouseClick() || dec->checkMouseClick())
		return true;
	else
		return false;
}

void spawnCube()
{
	Scenes::sceneEditor->resetOptionsMenu();

	//Color pickers
	Scenes::sceneEditor->valueChangers.push_back(new GUIValueChanger(vec3(600, 600, 0), Scenes::sceneEditor->color.r));
	Scenes::sceneEditor->valueChangers.push_back(new GUIValueChanger(vec3(600, 550, 0), Scenes::sceneEditor->color.g));
	Scenes::sceneEditor->valueChangers.push_back(new GUIValueChanger(vec3(600, 500, 0), Scenes::sceneEditor->color.b));
	Scenes::sceneEditor->valueChangers.push_back(new GUIValueChanger(vec3(600, 450, 0), Scenes::sceneEditor->color.a));

	Scenes::sceneEditor->acceptButton = new GUIButton(vec3(500, 600, 0), vec3(32, 32, 1), vec4(1.0, 1.0, 1.0, 1.0), "Done", vec4(0.0f, 0.0f, 0.0f, 1.0f), "textures\\button.png", true, acceptChanges, 14);

	Scenes::sceneEditor->displayOptions = true;	
}

void spawnSphere()
{
	vec3 center(0.0f, 10.0f, 0.0f);
	vec3 dimensions(5.0f, 5.0f, 5.0f);
	std::cout << Scenes::sceneEditor->color.r << std::endl;
	vec4 color(Scenes::sceneEditor->color.r, Scenes::sceneEditor->color.g, Scenes::sceneEditor->color.b, Scenes::sceneEditor->color.a);

	Scenes::sceneEditor->rigidbodies.push_back(new Polyhedron(64, center, dimensions, color, true));
}

void sweep()
{

}

SceneEditor::SceneEditor(GLFWwindow* window, Skybox* skybox)
{
	this->skybox = skybox;
	this->window = window;

	int width, height;
	glfwGetWindowSize(window, &width, &height);

	Camera::activeCamera = new StateSpaceCamera(window, vec2(0, 0), vec2(1, 1), vec3(-10, 0, 0), vec3(0,0,0), vec3(0, 1, 0), perspective(45.0f, (float)width / height, 0.1f, 1000.0f));
	observer = Camera::activeCamera;

	Controller::setController(StateSpaceController::getController());

	buttons.push_back(new GUIButton(vec3(1050, 100, 0.0f), vec3(150, 40, 0), vec4(1.0f, 1.0f, 1.0f, 1.0f), "Spawn Cube", vec4(0.0f), "textures\\button.png", true, spawnCube));
	buttons.push_back(new GUIButton(vec3(900, 100, 0.0f), vec3(150, 40, 0), vec4(1.0f, 1.0f, 1.0f, 1.0f), "Spawn Sphere", vec4(0.0f), "textures\\button.png", true, spawnSphere));
	buttons.push_back(new GUIButton(vec3(750, 100, 0.0f), vec3(150, 40, 0), vec4(1.0f, 1.0f, 1.0f, 1.0f), "Sweep", vec4(0.0f), "textures\\button.png", true, sweep));
	buttons.push_back(new GUIButton(vec3(150, 100, 0.0f), vec3(180, 64, 0), vec4(1.0f, 1.0f, 1.0f, 1.0f), "Back To Menu", vec4(0.0f), "textures\\button.png", true, backToMenu));
}

SceneEditor::~SceneEditor()
{
}

void SceneEditor::setup()
{
	Controller::setController(StateSpaceController::getController());
	Camera::activeCamera = observer;
}

void SceneEditor::draw()
{
	observer->setViewport();

	CubeMapShader::shader->Use();
	glUniformMatrix4fv(CubeMapShader::shader->projectionID, 1, GL_FALSE, &(observer->Projection[0][0]));
	glUniformMatrix4fv(CubeMapShader::shader->viewID, 1, GL_FALSE, &(observer->View[0][0]));
	glUniform1i(CubeMapShader::shader->cubeMap, 0);

	//skybox->draw();

	LitShader::shader->Use();
	glUniformMatrix4fv(LitShader::shader->projectionID, 1, GL_FALSE, &(observer->Projection[0][0]));
	glUniformMatrix4fv(LitShader::shader->viewID, 1, GL_FALSE, &(observer->View[0][0]));

	for (int i = 0; i < rigidbodies.size(); i++)
		rigidbodies[i]->draw();

	for (int i = 0; i < buttons.size(); i++)
		buttons[i]->draw();

	if (displayOptions)
	{
		acceptButton->draw();
		for (int i = 0; i < valueChangers.size(); i++)
			valueChangers[i]->draw();
	}

}

void SceneEditor::checkInput()
{
	//hover
	for (int i = 0; i < buttons.size(); i++)
	{
		buttons[i]->checkHover();
	}

	if (displayOptions)
	{
		acceptButton->checkHover();
		for (int i = 0; i < valueChangers.size(); i++)
			valueChangers[i]->checkHover();
	}


	static int oldLeftClickState = GLFW_RELEASE;

	int leftClick = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
	if (leftClick == GLFW_PRESS && oldLeftClickState == GLFW_RELEASE)
	{
		oldLeftClickState = GLFW_PRESS;
		for (int i = 0; i < buttons.size(); i++)
			if (buttons[i]->checkMouseClick())
				break;

		if (displayOptions)
		{
			acceptButton->checkMouseClick();
			for (int i = 0; i < valueChangers.size(); i++)
				if (valueChangers[i]->checkMouseClick())
					break;
		}
	}

	oldLeftClickState = leftClick;
}

void SceneEditor::execute()
{
	draw();
	checkInput();
}

void SceneEditor::resetOptionsMenu()
{
	color = vec4(0.0f);
	pos = vec3(0.0f);
	dimensions = vec3(0.0f);

	for (int i = 0; i < Scenes::sceneEditor->valueChangers.size(); i++)
		delete valueChangers[i];
	valueChangers.clear();
	delete acceptButton;
}