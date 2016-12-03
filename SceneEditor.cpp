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
	vec3 center(0.0f, 0.0f, 0.0f);
	vec3 dimensions(5.0f, 5.0f, 5.0f);
	vec4 color(0.0f, 0.0f, 0.0f, 1.0f);

	Scenes::sceneEditor->rigidbodies.push_back(new Cube(center, dimensions, color, true));

	Scenes::sceneEditor->displayOptions = false;
}

GUIValueChanger::GUIValueChanger(vec3 position, float& ValueToChange) : pos(position), value(ValueToChange)
{
	inc = new GUIButtonValued(vec3(pos.x + 50, pos.y, 0), vec3(32, 32, 1), vec4(1.0, 1.0, 1.0, 1.0), "+", vec4(0.0f, 0.0f, 0.0f, 1.0f), "textures\\button.png", true, increment, value);
	dec = new GUIButtonValued(vec3(pos.x - 50, pos.y, 0), vec3(32, 32, 1), vec4(1.0, 1.0, 1.0, 1.0), "-", vec4(0.0f, 0.0f, 0.0f, 1.0f), "textures\\button.png", true, decrement, value);
	done = new GUIButton(vec3(pos.x - 100, pos.y, 0), vec3(32, 32, 1), vec4(1.0, 1.0, 1.0, 1.0), "DONE", vec4(0.0f, 0.0f, 0.0f, 1.0f), "textures\\button.png", true, acceptChanges);
}

GUIValueChanger::~GUIValueChanger()
{

}

void GUIValueChanger::draw()
{
	inc->draw();
	dec->draw();
	done->draw();

	textRend.RenderText(std::to_string(value), pos.x, pos.y, 1.0f, glm::vec3(0.0f, 0.0f, 0.0f), Camera::activeCamera->getScreenWidth(), Camera::activeCamera->getScreenHeight());
}

void GUIValueChanger::checkHover()
{
	inc->checkHover();
	dec->checkHover();
	done->checkHover();
}

void GUIValueChanger::checkMouseClick()
{
	inc->checkMouseClick();
	dec->checkMouseClick();
	done->checkMouseClick();
}

void spawnCube()
{
	Scenes::sceneEditor->color = vec4(0.0f);
	Scenes::sceneEditor->pos = vec3(0.0f);
	Scenes::sceneEditor->dimensions = vec3(0.0f);

	for (int i = 0; i < Scenes::sceneEditor->valueChangers.size(); i++)
		delete Scenes::sceneEditor->valueChangers[i];
	Scenes::sceneEditor->valueChangers.clear();

	Scenes::sceneEditor->valueChangers.push_back(new GUIValueChanger(vec3(600, 600, 0), Scenes::sceneEditor->pos.x));

	Scenes::sceneEditor->displayOptions = true;	
}

void spawnSphere()
{
	vec3 center(0.0f, 10.0f, 0.0f);
	vec3 dimensions(5.0f, 5.0f, 5.0f);
	vec4 color(0.0f, 0.0f, 0.0f, 1.0f);

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
		for (int i = 0; i < valueChangers.size(); i++)
			valueChangers[i]->draw();
}

void SceneEditor::checkInput()
{
	//hover
	for (int i = 0; i < buttons.size(); i++)
	{
		buttons[i]->checkHover();
	}

	if (displayOptions)
		for (int i = 0; i < valueChangers.size(); i++)
			valueChangers[i]->checkHover();

	static int oldLeftClickState = GLFW_RELEASE;

	int leftClick = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
	if (leftClick == GLFW_PRESS && oldLeftClickState == GLFW_RELEASE)
	{
		oldLeftClickState = GLFW_PRESS;
		for (int i = 0; i < buttons.size(); i++)
			buttons[i]->checkMouseClick();

		if (displayOptions)
			for (int i = 0; i < valueChangers.size(); i++)
				valueChangers[i]->checkMouseClick();
	}

	oldLeftClickState = leftClick;
}

void SceneEditor::execute()
{
	draw();
	checkInput();
}