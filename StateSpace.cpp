#include "StateSpace.h"
#include "ParticleSystem.h"
#include "SPH.h"
#include <chrono>

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
	
	float viscocity = 0.01f;
	float stiffness = 1.0f;
	float timeStep = 0.01f;
	float density = 1000.0f;
	float radius = 0.1f;//1.0f / density;

	int blockSize = 5;
	
	vector<Particle*> pos;
	for(int k = 0; k < blockSize; k++)
		for(int j = 0; j < blockSize; j++)
			for (int i = 0; i < blockSize; i++)
				pos.push_back(new Particle(vec3(5.0f + (float)i / density, 5.0f + (float)j / density, 5.0f + (float)k / density)));

	Cube cube(vec3(5.0f, 5.0f, 5.0f), vec3(1.0f, 1.0f, 1.0f), vec4(1.0f, 0.0f, 0.0f, 0.5f));
	Rigidbody* rB = new Rigidbody(cube.vertices, cube.indices, cube.model, 1000, false);
	vector<Rigidbody*> rigidbodies;
	rigidbodies.push_back(rB);

	ParticleSystem::getInstance()->sysParams = SystemParameters(radius, 0.5f, viscocity, stiffness, density, -9.81f, timeStep, timeStep);
	ParticleSystem::getInstance()->grid = Grid3D(30, 0.5);
	ParticleSystem::getInstance()->addParticles(pos);
	ParticleSystem::getInstance()->addRigidbodies(rigidbodies);
	ParticleSystem::getInstance()->updateList();

	auto p = ParticleSystem::getInstance()->getParticlePositions();
	instancedModels.push_back(new InstancedSpheres(radius, 8, vec4(0.5, 0.5, 1, 1.0f), *p));
	frames.push_back(*p);
	delete p;

	float t = 0;
	float playbackTime = 3;
	float currentTimeStep = 0;

	for (float simTime = 0; simTime < playbackTime; simTime += currentTimeStep)
	{
		ms = duration_cast<milliseconds>(system_clock::now().time_since_epoch());

		if (t >= 0.016f)
		{
			auto partPos = ParticleSystem::getInstance()->getParticlePositions();
			frames.push_back(*partPos);
			delete partPos;
			t = 0;

			
			system("CLS");

			float deltaTime = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count() - ms.count();
			cout << (simTime / playbackTime) * 100 << "%" << endl;
			cout << "ETA: " << deltaTime * (playbackTime - simTime) / 1000 / 60 << " mins" << endl;
			
		}

		SPH::calcSPH();

		currentTimeStep = ParticleSystem::getInstance()->sysParams.tStep;

		t += currentTimeStep;
	}
	
	for (int i = 0; i < ParticleSystem::getInstance()->rigidbodies.size(); i++)
		models.push_back(new Rigidbody(ParticleSystem::getInstance()->rigidbodies[i]->vertices, ParticleSystem::getInstance()->rigidbodies[i]->indices, ParticleSystem::getInstance()->rigidbodies[i]->model, 0, true));
	
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