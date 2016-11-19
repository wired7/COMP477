#include "StateSpace.h"


StateSpace* StateSpace::activeStateSpace = NULL;

StateSpace::StateSpace(GLFWwindow* window, Skybox* skybox)
{
	milliseconds ms = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
	time = ms.count();
	frameCount = 0;
	playModeOn = false;
	this->skybox = skybox;
//	terrain = new Terrain(1, 40, 40, 32, STATIC);
	int width, height;
	glfwGetWindowSize(window, &width, &height);
	Camera::activeCamera = new StateSpaceCamera(window, vec2(0, 0), vec2(1, 1), vec3(0, 0, -5), vec3(0, 0, 0), vec3(0, 1, 0), perspective(45.0f, (float)width / height, 0.1f, 100.0f), terrain);
	
	float radius = 0.1f;
	int blockSize = 10;
	
	vector<vec3> pos;
	for(int k = 0; k < blockSize; k++)
		for(int j = 0; j < blockSize; j++)
			for (int i = 0; i < blockSize; i++)
				pos.push_back(vec3(-(float)blockSize * radius / 2.0f + radius * (float)i,
									-(float)blockSize * radius / 2.0f + radius * (float)j,
									-(float)blockSize * radius / 2.0f + radius * (float)k));

	for (int j = 0; j < 1000; j++)
	{
		frames.push_back(vector<vec3>());
		for (int i = 0; i < pos.size(); i++)
			if (!j)
				frames[j].push_back(pos[i]);
			else
				frames[j].push_back(frames[j - 1][i] + vec3(0.01f, 0.0f, 0.0f));
	}

	models.push_back(new InstancedSpheres(radius, 8, vec4(0.5, 0.5, 1, 1), pos));
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
		glUniform1i(CubeMapShader::shader->cubeMap, 0);

		skybox->draw();

		LitShader::shader->Use();
		glUniformMatrix4fv(LitShader::shader->projectionID, 1, GL_FALSE, &(observers[i]->Projection[0][0]));
		glUniformMatrix4fv(LitShader::shader->viewID, 1, GL_FALSE, &(observers[i]->View[0][0]));

		if(terrain != nullptr)
			terrain->draw();

		InstancedLitShader::shader->Use();
		glUniformMatrix4fv(InstancedLitShader::shader->projectionID, 1, GL_FALSE, &(observers[i]->Projection[0][0]));
		glUniformMatrix4fv(InstancedLitShader::shader->viewID, 1, GL_FALSE, &(observers[i]->View[0][0]));

		for (int i = 0; i < models.size(); i++)
			models[i]->draw();

		milliseconds ms = duration_cast<milliseconds>(system_clock::now().time_since_epoch());

		if (ms.count() - time >= 16 && playModeOn)
		{
			if (frameCount < frames.size() - 1)
			{
				((InstancedSpheres*)models[0])->updateInstances(&(frames[++frameCount]));
			}

			time = ms.count();
		}
	}
}