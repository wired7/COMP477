#include "StateSpace.h"
#include "ParticleSystem.h"
#include "SPH.h"
#include <chrono>
#include <fstream>
#include "FileStorage.h"
#include "OpenFileDialog.h"
#include <thread>

using namespace std::chrono;

std::mutex _mutex;

StateSpace* StateSpace::activeStateSpace = nullptr;

StateSpace::StateSpace(GLFWwindow* window, Skybox* skybox)
{
	framesFront = new vector<vector<vec3>>();
	framesBack = new vector<vector<vec3>>();
	framesBuffSize = 60;
	currGlobalFrame = 0;

	frameCount = 0;
	playModeOn = false;
	this->skybox = skybox;
	this->window = window;
//	terrain = new Terrain(1, 40, 40, 32, STATIC);

	//loadAnimation();
}

void StateSpace::loadAnimation()
{
	milliseconds ms = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
	time = ms.count();
	int width, height;
	glfwGetWindowSize(window, &width, &height);
	Camera::activeCamera = new StateSpaceCamera(window, vec2(0, 0), vec2(1, 1), vec3(5, 5, 0), vec3(4, 4, 4), vec3(0, 1, 0), perspective(45.0f, (float)width / height, 0.1f, 1000.0f), terrain);
	observers.push_back(Camera::activeCamera);

	Controller::setController(StateSpaceController::getController());

	fileName = _strdup(OpenFileDialog().SelectFile().c_str());
	initializeFrameRead();

	//	cout << framesFront->at(0)[0].x << " " << framesFront->at(0)[0].y << " " << framesFront->at(0)[0].z << endl;

	instancedModels.push_back(new InstancedSpheres(ParticleSystem::getInstance()->sysParams.particleRadius, 32, vec4(0.5, 0.5, 1, 1.0f), framesFront->at(0)));

	for (int i = 0; i < ParticleSystem::getInstance()->rigidbodies.size(); i++)
		models.push_back(new Rigidbody(ParticleSystem::getInstance()->rigidbodies[i]->vertices, ParticleSystem::getInstance()->rigidbodies[i]->indices, ParticleSystem::getInstance()->rigidbodies[i]->model, 0, true));
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
			frameCount = (frameCount + 1) % framesFront->size();
			if ((*framesFront)[frameCount].size() == 0) {
				FileStorage::resetReadFrames();
				initializeFrameRead();
			}
			else {
				((InstancedSpheres*)instancedModels[0])->updateInstances(&((*framesFront)[frameCount]));
				time = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
				currGlobalFrame++;
			}
		}
	}
}

void StateSpace::loadFramesInBack()
{	
	_mutex.lock();
	framesBack->clear();

	int framesLeft = totalFrames - totalFramesLoaded;

	if (framesLeft > 0) {
		int loadSize = (framesLeft > framesBuffSize)? framesBuffSize : framesLeft;
		totalFramesLoaded += loadSize;
		FileStorage::readFrames(fileName, framesBuffSize, framesBack);
	}

	_mutex.unlock();
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
	frameCount = 0;
	currGlobalFrame = 0;
	framesFront->clear();
	framesBack->clear();
	if (totalFrames == 0) 
	{
		totalFrames = FileStorage::getFramesTotal(fileName);
	}
	totalFramesLoaded = framesBuffSize % totalFrames;
	FileStorage::readFrames(fileName, totalFramesLoaded, framesFront);
	std::thread t1(&StateSpace::loadFramesInBack, this);
	t1.detach();
}

void StateSpace::execute()
{
	draw();
}