#include "StateSpace.h"
#include "ParticleSystem.h"
#include "SPH.h"
#include <chrono>
#include <fstream>

using namespace std::chrono;

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
	Camera::activeCamera = new StateSpaceCamera(window, vec2(0, 0), vec2(1, 1), vec3(5, 5, 0), vec3(5, 5, 5), vec3(0, 1, 0), perspective(45.0f, (float)width / height, 0.1f, 1000.0f), terrain);
	observers.push_back(Camera::activeCamera);
	Controller::setController(StateSpaceController::getController());

	auto p = ParticleSystem::getInstance()->getParticlePositions();
	instancedModels.push_back(new InstancedSpheres(ParticleSystem::getInstance()->sysParams.particleRadius, 8, vec4(0.5, 0.5, 1, 1.0f), *p));
	frames.push_back(*p);
	delete p;
	
	for (int i = 0; i < ParticleSystem::getInstance()->rigidbodies.size(); i++)
		models.push_back(new Rigidbody(ParticleSystem::getInstance()->rigidbodies[i]->vertices, ParticleSystem::getInstance()->rigidbodies[i]->indices, ParticleSystem::getInstance()->rigidbodies[i]->model, 0, true));
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

		InstancedLitShader::shader->Use();
		glUniformMatrix4fv(InstancedLitShader::shader->projectionID, 1, GL_FALSE, &(observers[i]->Projection[0][0]));
		glUniformMatrix4fv(InstancedLitShader::shader->viewID, 1, GL_FALSE, &(observers[i]->View[0][0]));

		for (int i = 0; i < instancedModels.size(); i++)
			instancedModels[i]->draw();

		LitShader::shader->Use();
		glUniformMatrix4fv(LitShader::shader->projectionID, 1, GL_FALSE, &(observers[i]->Projection[0][0]));
		glUniformMatrix4fv(LitShader::shader->viewID, 1, GL_FALSE, &(observers[i]->View[0][0]));

		if(terrain != nullptr)
			terrain->draw();

		for (int i = 0; i < models.size(); i++)
			models[i]->draw();

		milliseconds ms = duration_cast<milliseconds>(system_clock::now().time_since_epoch());

		if (ms.count() - time >= 16 && playModeOn)
		{
			frameCount = (frameCount + 1) % frames.size();
			((InstancedSpheres*)instancedModels[0])->updateInstances(&(frames[frameCount]));
			time = ms.count();
		}

	}
}