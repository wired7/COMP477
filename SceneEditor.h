#pragma once
#include "Skybox.h"
#include "Camera.h"

class GUIValueChanger
{
public:
	GUIButton* inc;
	GUIButton* dec;
	GUIButton* done;
	float value;
	TextRenderer textRend;
	vec3 pos;

	GUIValueChanger(vec3 position);
	~GUIValueChanger();

	void draw();
	void checkMouseClick();
	void checkHover();

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