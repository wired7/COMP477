#pragma once
#include "SceneEditor.h"
#include "Controller.h"
#include "SceneManager.h"
#include "ButtonFunctions.h"
#include "FileStorage.h"
#include "OpenFileDialog.h"
#include "SaveFileDialog.h"

void increment(float& value)
{
	value++;
	Scenes::sceneEditor->updateCurrentSpawn();
}

void decrement(float& value)
{
	value--;
	Scenes::sceneEditor->updateCurrentSpawn();
}

void acceptChanges()
{
	float maxValue = 8.0f;

	vec3 center(Scenes::sceneEditor->pos.x, Scenes::sceneEditor->pos.y, Scenes::sceneEditor->pos.z);
	vec4 color(Scenes::sceneEditor->color.r / maxValue, Scenes::sceneEditor->color.g / maxValue, Scenes::sceneEditor->color.b / maxValue, Scenes::sceneEditor->color.a / maxValue);
	vec3 dimensions(Scenes::sceneEditor->dimensions.x, Scenes::sceneEditor->dimensions.y, Scenes::sceneEditor->dimensions.z);

	Scenes::sceneEditor->rigidbodies.push_back(new Cube(center, dimensions, color, true, "Cube"));

	Scenes::sceneEditor->showRecentShape = false;
	Scenes::sceneEditor->displayOptions = false;
}

void cancelChanges()
{
	Scenes::sceneEditor->showRecentShape = false;
	Scenes::sceneEditor->displayOptions = false;
}

GUIValueChanger::GUIValueChanger(vec3 position, float& ValueToChange, std::string Name) : pos(position), value(ValueToChange), name(Name)
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

	vec3 white = vec3(1.0f, 1.0f, 1.0f);

	textRend.RenderText(name, pos.x - 40.0f, pos.y - 3.0f, 1.0f, white, Camera::activeCamera->getScreenWidth(), Camera::activeCamera->getScreenHeight(), 14);
	textRend.RenderText(std::to_string((int)value), pos.x - 5.0f, pos.y - 3.0f, 1.0f, white, Camera::activeCamera->getScreenWidth(), Camera::activeCamera->getScreenHeight(), 14);
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
	Scenes::sceneEditor->showAllOptions();

	float maxValue = 8.0f;

	vec3 center(Scenes::sceneEditor->pos.x, Scenes::sceneEditor->pos.y, Scenes::sceneEditor->pos.z);
	vec4 color(Scenes::sceneEditor->color.r / maxValue, Scenes::sceneEditor->color.g / maxValue, Scenes::sceneEditor->color.b / maxValue, Scenes::sceneEditor->color.a / maxValue);
	vec3 dimensions(Scenes::sceneEditor->dimensions.x, Scenes::sceneEditor->dimensions.y, Scenes::sceneEditor->dimensions.z);

	Scenes::sceneEditor->tempRigidbodies.push_back(new Cube(center, dimensions, color, true, "Cube"));
	Scenes::sceneEditor->displayOptions = true;	
	Scenes::sceneEditor->showRecentShape = true;
}

void spawnSphere()
{
	Scenes::sceneEditor->showAllOptions();

	float maxValue = 8.0f;

	vec3 center(Scenes::sceneEditor->pos.x, Scenes::sceneEditor->pos.y, Scenes::sceneEditor->pos.z);
	vec4 color(Scenes::sceneEditor->color.r / maxValue, Scenes::sceneEditor->color.g / maxValue, Scenes::sceneEditor->color.b / maxValue, Scenes::sceneEditor->color.a / maxValue);
	vec3 dimensions(Scenes::sceneEditor->dimensions.x, Scenes::sceneEditor->dimensions.y, Scenes::sceneEditor->dimensions.z);

	Scenes::sceneEditor->tempRigidbodies.push_back(new Polyhedron(64, center, dimensions, color, true, "Sphere"));
	Scenes::sceneEditor->displayOptions = true;
	Scenes::sceneEditor->showRecentShape = true;
}

void sweep()
{

}

void saveScene()
{
	SaveFileDialog saveDialog;

	string sceneFile = saveDialog.SaveFile();

	if (sceneFile == "")
		return;

	std::fstream f1(sceneFile, ios::out);

	if (f1.is_open())
	{
		f1 << FileStorage::serializeScene();
	}
}

void undo()
{
	// undo button only works when u aren't creating a shape
	if (!Scenes::sceneEditor->displayOptions)
	{
		if (Scenes::sceneEditor->tempRigidbodies.size() > 0)
		{
			delete Scenes::sceneEditor->tempRigidbodies.back();
			Scenes::sceneEditor->tempRigidbodies.pop_back();
		}


		if (Scenes::sceneEditor->rigidbodies.size() > 0)
		{
			delete Scenes::sceneEditor->rigidbodies.back();
			Scenes::sceneEditor->rigidbodies.pop_back();
		}
	}
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

	buttons.push_back(new GUIButton(vec3(1050, 100, 0.0f), vec3(150, 40, 0), vec4(1.0f, 1.0f, 1.0f, 1.0f), " Spawn Cube", vec4(0.0f), "textures\\button.png", true, spawnCube));
	buttons.push_back(new GUIButton(vec3(900, 100, 0.0f), vec3(150, 40, 0), vec4(1.0f, 1.0f, 1.0f, 1.0f), " Spawn Sphere", vec4(0.0f), "textures\\button.png", true, spawnSphere, 20));
	buttons.push_back(new GUIButton(vec3(750, 100, 0.0f), vec3(150, 40, 0), vec4(1.0f, 1.0f, 1.0f, 1.0f), "     Sweep", vec4(0.0f), "textures\\button.png", true, sweep));
	buttons.push_back(new GUIButton(vec3(150, 100, 0.0f), vec3(180, 64, 0), vec4(1.0f, 1.0f, 1.0f, 1.0f), "  Back To Menu", vec4(0.0f), "textures\\button.png", true, backToMenu, 24));
	buttons.push_back(new GUIButton(vec3(150, 180, 0.0f), vec3(180, 64, 0), vec4(1.0f, 1.0f, 1.0f, 1.0f), "   Save Scene", vec4(0.0f), "textures\\button.png", true, saveScene, 24));
	buttons.push_back(new GUIButton(vec3(650, 100, 0.0f), vec3(40, 40, 0), vec4(1.0f, 1.0f, 1.0f, 1.0f), "Undo", vec4(0.0f), "textures\\button.png", true, undo, 16));
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

	skybox->draw();

	LitShader::shader->Use();
	glUniformMatrix4fv(LitShader::shader->projectionID, 1, GL_FALSE, &(observer->Projection[0][0]));
	glUniformMatrix4fv(LitShader::shader->viewID, 1, GL_FALSE, &(observer->View[0][0]));

	for (int i = 0; i < rigidbodies.size(); i++)
	{
		rigidbodies[i]->draw();
	}

	if (tempRigidbodies.size() > 0 && showRecentShape)
	{
		tempRigidbodies.back()->draw();
	}

	for (int i = 0; i < buttons.size(); i++)
		buttons[i]->draw();

	if (displayOptions)
	{
		acceptButton->draw();
		for (int i = 0; i < valueChangers.size(); i++)
			valueChangers[i]->draw();

		cancelButton->draw();
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
		cancelButton->checkHover();
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
			cancelButton->checkMouseClick();
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
	delete cancelButton;

	Scenes::sceneEditor->tempRigidbodies.clear();
}

void SceneEditor::updateCurrentSpawn()
{
	/*
	float maxValue = 8.0f;
	vec3 center(pos.x, pos.y, pos.z);
	vec4 color(color.r / maxValue, color.g / maxValue, color.b / maxValue, color.a / maxValue);
	vec3 dimensions(dimensions.x, dimensions.y, dimensions.z);

	std::cout << pos.x << std::endl;

	tempRigidbodies.back()->pos = center;
	tempRigidbodies.back()->dimensions = dimensions;
	tempRigidbodies.back()->color = color;
	*/

	float maxValue = 8.0f;
	vec3 center(pos.x, pos.y, pos.z);
	vec4 color(color.r / maxValue, color.g / maxValue, color.b / maxValue, color.a / maxValue);
	vec3 dimensions(dimensions.x, dimensions.y, dimensions.z);

	delete tempRigidbodies.back();
	tempRigidbodies.pop_back();
	tempRigidbodies.push_back(new Cube(pos, dimensions, color, true, "Cube"));
}

void SceneEditor::showAllOptions()
{
	Scenes::sceneEditor->resetOptionsMenu();

	//Color pickers
	Scenes::sceneEditor->valueChangers.push_back(new GUIValueChanger(vec3(50, 750, 0), Scenes::sceneEditor->color.r, "r"));
	Scenes::sceneEditor->valueChangers.push_back(new GUIValueChanger(vec3(50, 700, 0), Scenes::sceneEditor->color.g, "g"));
	Scenes::sceneEditor->valueChangers.push_back(new GUIValueChanger(vec3(50, 650, 0), Scenes::sceneEditor->color.b, "b"));
	Scenes::sceneEditor->valueChangers.push_back(new GUIValueChanger(vec3(50, 600, 0), Scenes::sceneEditor->color.a, "a"));

	//Pos pickers
	Scenes::sceneEditor->valueChangers.push_back(new GUIValueChanger(vec3(150, 750, 0), Scenes::sceneEditor->pos.x, "x"));
	Scenes::sceneEditor->valueChangers.push_back(new GUIValueChanger(vec3(150, 700, 0), Scenes::sceneEditor->pos.y, "y"));
	Scenes::sceneEditor->valueChangers.push_back(new GUIValueChanger(vec3(150, 650, 0), Scenes::sceneEditor->pos.z, "z"));

	//Dimension pickers
	Scenes::sceneEditor->valueChangers.push_back(new GUIValueChanger(vec3(250, 750, 0), Scenes::sceneEditor->dimensions.x, "x"));
	Scenes::sceneEditor->valueChangers.push_back(new GUIValueChanger(vec3(250, 700, 0), Scenes::sceneEditor->dimensions.y, "y"));
	Scenes::sceneEditor->valueChangers.push_back(new GUIValueChanger(vec3(250, 650, 0), Scenes::sceneEditor->dimensions.z, "z"));

	Scenes::sceneEditor->dimensions.x = 1.0f;
	Scenes::sceneEditor->dimensions.y = 1.0f;
	Scenes::sceneEditor->dimensions.z = 1.0f;
	Scenes::sceneEditor->color.a = 8.0f;

	Scenes::sceneEditor->acceptButton = new GUIButton(vec3(50, 550, 0), vec3(40, 32, 1), vec4(1.0, 1.0, 1.0, 1.0), "Done", vec4(0.0f, 0.0f, 0.0f, 1.0f), "textures\\button.png", true, acceptChanges, 14);
	Scenes::sceneEditor->cancelButton = new GUIButton(vec3(150, 550, 0), vec3(40, 32, 1), vec4(1.0, 1.0, 1.0, 1.0), "Cancel", vec4(0.0f, 0.0f, 0.0f, 1.0f), "textures\\button.png", true, cancelChanges, 12);

}