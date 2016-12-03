#pragma once
#include "Skybox.h"
#include "Camera.h"

struct GUIValueChanger
{
	GUIButton* inc;
	GUIButton* dec;
	GUIButton* done;
	float value;
	TextRenderer textRend;
	vec3 pos;

	GUIValueChanger(vec3 position) : pos(position)
	{
		//inc = new GUIButton(vec3(pos.x + 50, pos.y, 0), vec3(32, 32, 1), vec4(1.0, 1.0, 1.0, 1.0), "+", vec4(0.0f, 0.0f, 0.0f, 1.0f), "textures\\button.png", true, increment);
		//dec = new GUIButton(vec3(pos.x - 50, pos.y, 0), vec3(32, 32, 1), vec4(1.0, 1.0, 1.0, 1.0), "-", vec4(0.0f, 0.0f, 0.0f, 1.0f), "textures\\button.png", true, decrement);
		//done = new GUIButton(vec3(pos.x - 100, pos.y, 0), vec3(32, 32, 1), vec4(1.0, 1.0, 1.0, 1.0), "DONE", vec4(0.0f, 0.0f, 0.0f, 1.0f), "textures\\button.png", true, acceptChanges);
	};

	void draw()
	{
		inc->draw();
		dec->draw();
		done->draw();
		textRend.RenderText("0", pos.x, pos.y, 1.0f, glm::vec3(0.0f, 0.0f, 0.0f), Camera::activeCamera->getScreenWidth(), Camera::activeCamera->getScreenHeight());
	}

	void checkHover()
	{
		inc->checkHover();
		dec->checkHover();
		done->checkHover();
	}

	void checkMouseClick()
	{
		inc->checkMouseClick();
		dec->checkMouseClick();
		done->checkMouseClick();
	}
};

class SceneEditor : public ScreenState
{
public:
	SceneEditor(GLFWwindow* window, Skybox* skybox);
	~SceneEditor();

	Camera* observer;

	Skybox* skybox;

	GLFWwindow* window;
	void draw();
	void execute();
	void setup();
	void checkInput();

	std::vector<GUIValueChanger*> valueChangers;
	std::vector<MeshObject*> rigidbodies;
	bool displayOptions;

	vec3 pos;
	vec3 dimensions;
	vec4 color;

	TextRenderer textRend;

private:
	std::vector<GUIButton*> buttons;
};