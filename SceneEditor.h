#pragma once
#include "Skybox.h"
#include "Camera.h"

class GUIValueChanger
{
public:
	GUIButtonValued* inc;
	GUIButtonValued* dec;
	GUIButton* done;
	TextRenderer textRend;
	vec3 pos;
	float& value;

	GUIValueChanger(vec3 position, float& ValueToChange);
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