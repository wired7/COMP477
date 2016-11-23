 #include "Menu.h"



Menu::Menu(GLFWwindow* window)
{
	observer = new MenuCamera(window, vec2(0.8, 0), vec2(0.2, 1), vec3(0, 0, 0.8), vec3(0, 0, 0), vec3(0, 1, 0), perspective(45.0f, 1.0f, 0.1f, 100.0f));
//	bucket = new ImportedMesh("models\\bucket.3DS", vec3(0, 0.3, 0), vec3(0.5, 0.1, 0.1));
	rectangle = new Rectangle(-vec3(1, 0.5, 0), vec3(2, 1, 0), vec4(1, 1, 0, 0.5), true);
}


Menu::~Menu()
{
}

void Menu::draw()
{
	observer->setViewport();
	glUniformMatrix4fv(LitShader::shader->viewID, 1, GL_FALSE, &(observer->View[0][0]));

	rectangle->draw();
//	bucket->draw();
}