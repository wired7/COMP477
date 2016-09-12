#include "StateSpace.h"


StateSpace* StateSpace::activeStateSpace = NULL;

StateSpace::StateSpace(GLFWwindow* window, Skybox* skybox)
{
	this->skybox = skybox;
	terrain = new Terrain(1, 40, 40, 16, STATIC);
	int width, height;
	glfwGetWindowSize(window, &width, &height);
	Camera::activeCamera = new StateSpaceCamera(window, vec2(0, 0), vec2(1, 1), vec3(0, 0, 5), vec3(0, 0, 0), vec3(0, 1, 0), perspective(45.0f, (float)width / height, 0.1f, 100.0f), terrain);
	models.push_back(new Polyhedron(100, 2, vec3(0, 0, 0), vec4(0.5, 0.5, 1, 1)));
	observers.push_back(Camera::activeCamera);
}


StateSpace::~StateSpace()
{
}


void StateSpace::draw()
{
	for (int i = 0; i < observers.size(); i++)
	{
		observers[i]->setViewport();

		CubeMapShader::shader->Use();
		glUniformMatrix4fv(CubeMapShader::shader->projectionID, 1, GL_FALSE, &(observers[i]->Projection[0][0]));
		glUniformMatrix4fv(CubeMapShader::shader->viewID, 1, GL_FALSE, &(observers[i]->View[0][0]));

		skybox->draw();

		LitShader::shader->Use();
		glUniformMatrix4fv(LitShader::shader->projectionID, 1, GL_FALSE, &(observers[i]->Projection[0][0]));
		glUniformMatrix4fv(LitShader::shader->viewID, 1, GL_FALSE, &(observers[i]->View[0][0]));

		terrain->draw();

		for (int i = 0; i < models.size(); i++)
			models[i]->draw();
	}
}