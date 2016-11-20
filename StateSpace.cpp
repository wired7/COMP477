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
	Camera::activeCamera = new StateSpaceCamera(window, vec2(0, 0), vec2(1, 1), vec3(0, 0, -5), vec3(0, 0, 0), vec3(0, 1, 0), perspective(45.0f, (float)width / height, 0.1f, 100.0f), terrain);
	
	float radius = 0.01f;
	int blockSize = 60;
	
	vector<Particle*> pos;
	for(int k = 0; k < blockSize; k++)
		for(int j = 0; j < blockSize; j++)
			for (int i = 0; i < blockSize; i++)
				pos.push_back(new Particle(vec3(1.0f + radius * (float)i, 1.0f + radius * (float)j, 1.0f + radius * (float)k)));

/*	for (int j = 0; j < 1000; j++)
	{
		frames.push_back(vector<vec3>());
		for (int i = 0; i < pos.size(); i++)
			if (!j)
				frames[j].push_back(pos[i]);
			else
 				frames[j].push_back(frames[j - 1][i] + vec3((float)rand() * 0.001f / RAND_MAX, (float)rand() * 0.001f / RAND_MAX, (float)rand() * 0.001f / RAND_MAX));
	}*/

	ParticleSystem::getInstance()->sysParams = SystemParameters(radius, 5, 0.01, 1, 1000, 0, 0.001, 0.001);
	ParticleSystem::getInstance()->particles = pos;
	ParticleSystem::getInstance()->grid = Grid3D(10, 1);
	ParticleSystem::getInstance()->updateList();

	auto positions = ParticleSystem::getInstance()->getParticlePositions();
	models.push_back(new InstancedSpheres(radius, 8, vec4(0.5, 0.5, 1, 1), *positions));
	delete positions;
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
//			if (frameCount < frames.size() - 1)
			{
//				((InstancedSpheres*)models[0])->updateInstances(&(frames[++frameCount]));
			}
			
			auto positions = ParticleSystem::getInstance()->getParticlePositions();
			((InstancedSpheres*)models[0])->updateInstances(positions);
			delete positions;
			time = ms.count();
		}

		SPH::calcSPH();
	}
}