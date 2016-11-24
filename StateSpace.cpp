#include "StateSpace.h"
#include "ParticleSystem.h"
#include "SPH.h"
#include <chrono>
#include <fstream>
#include "FileStorage.h"
#include <thread>

using namespace std::chrono;

std::mutex _mutex;

StateSpace* StateSpace::activeStateSpace = NULL;

StateSpace::StateSpace(GLFWwindow* window, Skybox* skybox)
{
	framesFront = new vector<vector<vec3>>();
	framesBack = new vector<vector<vec3>>();
	framesBuffSize = 60;
	currGlobalFrame = 0;

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

float viscocity = 0.01f;
float stiffness = 0.01f;
float timeStep = 0.01f;
float density = 1000.0f;
float radius = 0.1f;//1.0f / density;
float searchRadius = 1.0f;
float gravity = -9.81f;
float mass = 1.0f;

int blockSize = 10;

vector<Particle*> pos;
for (int k = 0; k < blockSize; k++)
	for (int j = 0; j < blockSize; j++)
		for (int i = 0; i < blockSize; i++)
			pos.push_back(new Particle(vec3(5.0f + (float)i / density, 5.0f + (float)j / density, 5.0f + (float)k / density)));

Cube cube(vec3(5.0f, 5.0f, 5.0f), vec3(1.0f, 1.0f, 1.0f), vec4(1.0f, 0.0f, 0.0f, 0.5f), false);
Rectangle rect(vec3(5, 4, 5), vec3(1, 2, 1), vec4(1, 1, 0, 1), false);
rect.model = rect.model * rotate(mat4(1.0f), 1.5f, vec3(1, 1, 1));

Rigidbody* rB = new Rigidbody(cube.vertices, cube.indices, cube.model, 1000, false);
Rigidbody* rB1 = new Rigidbody(rect.vertices, rect.indices, rect.model, 1000, false);
vector<Rigidbody*> rigidbodies;
rigidbodies.push_back(rB);
rigidbodies.push_back(rB1);

ParticleSystem::getInstance()->sysParams = SystemParameters(radius, searchRadius, viscocity, stiffness, density, gravity, timeStep, timeStep, mass);
ParticleSystem::getInstance()->grid = Grid3D(30, searchRadius);
ParticleSystem::getInstance()->addParticles(pos);
ParticleSystem::getInstance()->addRigidbodies(rigidbodies);

auto p = ParticleSystem::getInstance()->getParticlePositions();
instancedModels.push_back(new InstancedSpheres(radius, 32, vec4(0.5, 0.5, 1, 1.0f), *p));
framesFront->push_back(*p);
delete p;
fileName = "Animations\\test.anim";
ParticleSystem::getInstance()->goNuts(3, 0.016f, fileName);

for (int i = 0; i < ParticleSystem::getInstance()->rigidbodies.size(); i++)
	models.push_back(new Rigidbody(ParticleSystem::getInstance()->rigidbodies[i]->vertices, ParticleSystem::getInstance()->rigidbodies[i]->indices, ParticleSystem::getInstance()->rigidbodies[i]->model, 0, true));

initializeFrameRead();
}


StateSpace::~StateSpace()
{
}


void StateSpace::draw()
{
	updateFrames();
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

		if (terrain != nullptr)
			terrain->draw();

		for (int i = 0; i < models.size(); i++)
			models[i]->draw();

		milliseconds ms = duration_cast<milliseconds>(system_clock::now().time_since_epoch());

		if (ms.count() - time >= 16 && playModeOn)
		{
			bool thisIsAHackImLazy = false;
			frameCount = (frameCount + 1) % framesFront->size();

			if ((*framesFront)[frameCount].size() == 0)
			{
				thisIsAHackImLazy = true;
				currGlobalFrame = totalFrames;
			}
			if (!thisIsAHackImLazy)
			{
				((InstancedSpheres*)instancedModels[0])->updateInstances(&((*framesFront)[frameCount]));
			}
			time = ms.count();
			currGlobalFrame++;
		}

	}
}

void StateSpace::loadFramesInBack()
{	
	_mutex.lock();
	framesBack->clear();
	FileStorage::readFrames(fileName, framesBuffSize, framesBack);
	_mutex.unlock();
	return;
}

void StateSpace::updateFrames() 
{
	if (frameCount == framesFront->size() - 1)
	{
		currGlobalFrame++;
	
		//wait for buffer to be ready for swap
		_mutex.lock();
		_mutex.unlock();
		
		swap(framesFront, framesBack);
		frameCount = 0;

		std::thread t1(&StateSpace::loadFramesInBack, this);
		t1.detach();
	}
	if (currGlobalFrame > totalFrames - 1)
	{
		FileStorage::resetReadFrames();
		initializeFrameRead();
	}
	return;
}

void StateSpace::swap(vector<vector<vec3>>* p1, vector<vector<vec3>>* p2)
{
	vector<vector<vec3>> temp = *p1;
	*p1 = *p2;
	*p2 = temp;
	return;
}

void StateSpace::initializeFrameRead()
{
	currGlobalFrame = 0;
	framesFront->clear();
	framesBack->clear();
	totalFrames = FileStorage::getFramesTotal(fileName);
	FileStorage::readFrames(fileName, framesBuffSize, framesFront);
	std::thread t1(&StateSpace::loadFramesInBack, this);
	t1.join();
}