#include "StateSpace.h"
#include "ParticleSystem.h"
#include "SPH.h"


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
	Camera::activeCamera = new StateSpaceCamera(window, vec2(0, 0), vec2(1, 1), vec3(0, 0, -1), vec3(0, 0, 0), vec3(0, 1, 0), perspective(45.0f, (float)width / height, 0.1f, 1000.0f), terrain);
	
	float radius = 0.1f;
	int blockSize = 10;
	
	vector<Particle*> pos;
	for(int k = 0; k < blockSize; k++)
		for(int j = 0; j < blockSize; j++)
			for (int i = 0; i < blockSize; i++)
				pos.push_back(new Particle(vec3(10.0f + radius * (float)i, 10.0f + radius * (float)j, 10.0f + radius * (float)k)));

	ParticleSystem::getInstance()->sysParams = SystemParameters(radius, 5, 0.01, 1, 1, 0, 0.01, 0.01);
	ParticleSystem::getInstance()->grid = Grid3D(30, 1);
	ParticleSystem::getInstance()->addParticles(pos);
	ParticleSystem::getInstance()->updateList();

	auto p = ParticleSystem::getInstance()->getParticlePositions();
	models.push_back(new InstancedSpheres(radius, 8, vec4(0.5, 0.5, 1, 1), *p));
	delete p;

	float t = 0;
	for (float simTime = 0; simTime < 20; simTime += t)
	{
		if (t >= 0.016f)
		{
			auto partPos = ParticleSystem::getInstance()->getParticlePositions();
			frames.push_back(*partPos);
			delete partPos;
			t = 0;
		}

		t += ParticleSystem::getInstance()->sysParams.tStep;
		SPH::calcSPH();
	}

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