#include "Menu.h"

Menu::Menu(GLFWwindow* window)
{
	observer = new MenuCamera(window, vec2(0, 0), vec2(1, 1), vec3(0, 0, 0), vec3(0, 0, -1), vec3(0, 1, 0), perspective(45.0f, 1.0f, 0.1f, 100.0f));
	Camera::activeCamera = observer;
	//	bucket = new ImportedMesh("models\\bucket.3DS", vec3(0, 0.3, 0), vec3(0.5, 0.1, 0.1));
	//rectangle = new Rectangle(vec3(100, 1, 1), vec3(2, 1, 1), vec4(0, 0, 0, 1.0), true);
	button = new GUIButton(vec3(0, 0.3, 0), vec3(0.3, 0.17, 1), vec4(0.3, 0.3, 0.3, 1.0), "This is a Button", true);
}


Menu::~Menu()
{
}

void Menu::draw()
{
	observer->setViewport();

	glUniformMatrix4fv(GUIShader::shader->viewID, 1, GL_FALSE, &(observer->View[0][0]));

	button->draw();

	//	bucket->draw();
}