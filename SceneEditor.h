#pragma once
#include "Skybox.h"
#include "Camera.h"

class GUIValueChanger
{
public:
	GUIButtonValued* inc;
	GUIButtonValued* dec;
	TextRenderer textRend;
	vec3 pos;
	float& value;
	std::string name;

	GUIValueChanger(vec3 position, float& ValueToChange, std::string Name);
	~GUIValueChanger();

	void draw();
	bool checkMouseClick();
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
	GUIButton* acceptButton;
	GUIButton* cancelButton;
	std::vector<MeshObject*> tempRigidbodies;
	std::vector<MeshObject*> rigidbodies;
	bool displayOptions;
	bool showRecentShape;

	vec3 pos;
	vec3 dimensions;
	vec4 color;

	TextRenderer textRend;

	void resetOptionsMenu();
	void updateCurrentSpawn();
	void showAllOptions();

private:
	std::vector<GUIButton*> buttons;
};