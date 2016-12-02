#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <time.h> 
#include <glew.h>
#include <glfw3.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtx/rotate_vector.hpp>
#include <thread>
#include "FileStorage.h"
#include "OpenFileDialog.h"
#include "SaveFileDialog.h"
#include "GraphicsObject.h"
#include "Shader.h"
#include "StateSpace.h"
#include "ParticleSystem.h"
#include "Menu.h"
#include <windows.h>
#include <omp.h>

using namespace std;
using namespace glm;

GLFWwindow* window;
StateSpace* stateSpace;
Skybox* skybox;
Menu* menu;

//Nvidia GPU support
extern "C" {
	_declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
}

// GLEW and GLFW initialization. Projection and View matrix setup
int init() {
	// Initialise GLFW
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		getchar();
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Open a window and create its OpenGL context
	window = glfwCreateWindow(1200, 800, "COMP477", NULL, NULL);
	glfwHideWindow(window);
	if (window == NULL) {
		fprintf(stderr, "Failed to open GLFW window.\n");
		getchar();
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		getchar();
		glfwTerminate();
		return -1;
	}

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	// White background
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	glEnable(GL_BLEND);
	glEnable(GL_ALPHA_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glPointSize(3.0f);

	LitShader::shader = new LitShader("shaders\\litShader.VERTEXSHADER", "shaders\\InstancedFragmentShader.FRAGMENTSHADER");
	InstancedLitShader::shader = new InstancedLitShader("shaders\\InstancedVertexShader.VERTEXSHADER", "shaders\\InstancedFragmentShader.FRAGMENTSHADER");
	CubeMapShader::shader = new CubeMapShader("shaders\\CubeMap.VERTEXSHADER", "shaders\\CubeMap.FRAGMENTSHADER");

	srand(time(NULL));
}

int main()
{
	if (init() < 0)
		return -1;

menu:
//	system("CLS");
	cout << "What would you like to do?" << endl;
	cout << "1. Create new simulation" << endl;
	cout << "2. Edit existing simulation" << endl;
	cout << "3. Run Existing simulation" << endl;

	string s;
	do {
		s = "";

		getline(cin, s);

		if (stoi(s) < 1 || stoi(s) > 3)
			cout << "Invalid Input. Please try again." << endl;

	} while (stoi(s) < 0 || stoi(s) > 3);

	if (stoi(s) == 1)
	{
		OpenFileDialog dialog;

		string sysParamsFile = dialog.SelectFile();

		if (sysParamsFile == "")
			goto menu;
		
		auto sysParams = FileStorage::loadSysParams(sysParamsFile);

		string rigidBodiesFile = dialog.SelectFile();

		if (rigidBodiesFile == "")
			goto menu;

		auto rigidBodies = FileStorage::loadRigidbodies(rigidBodiesFile);

		string simParams;
		system("CLS");
		cout << "Enter number of particles: " << endl;
		getline(cin, simParams);
		int blockSize = pow(stof(simParams), 1.0f / 3.0f);

		cout << "Enter animation time" << endl;
		getline(cin, simParams);
		float animationTime = stof(simParams);

		cout << "Save animation as..." << endl;

		SaveFileDialog saveDialog;

		string animFile = saveDialog.SaveFile();

		if (animFile == "")
			goto menu;

		float separation = sysParams.particleRadius;

		Bounds b;

		for (int i = 0; i < rigidBodies.size(); i++)
		{
			b.join(rigidBodies[i]->getBounds());
		}

		vec3 center = vec3(1, 1, 1) - b.min;
		vec3 maximum = center + b.max + vec3(1, 1, 1);

		int gridSize = max(maximum.x, max(maximum.y, maximum.z));

		float particleSpan = separation * blockSize / 2.0f;

		vector<Particle*> pos;
		for (int k = 0; k < blockSize; k++)
			for (int j = 0; j < blockSize; j++)
				for (int i = 0; i < blockSize; i++)
					pos.push_back(new Particle(center + vec3(separation * (float)i - particleSpan, separation * (float)j - particleSpan, separation * (float)k - particleSpan)));

		vector<Rigidbody*> rigidbodies;

		for (int i = 0; i < rigidBodies.size(); i++)
		{
			rigidBodies[i]->applyTransform();
			rigidBodies[i]->model = translate(rigidBodies[i]->model, center);
			rigidbodies.push_back(new Rigidbody(rigidBodies[i]->vertices, rigidBodies[i]->indices, rigidBodies[i]->model, 1000, false));
		}

		ParticleSystem::getInstance()->sysParams = sysParams;		
		ParticleSystem::getInstance()->grid = Grid3D(gridSize / sysParams.searchRadius, sysParams.searchRadius);
		ParticleSystem::getInstance()->setStiffnessOfParticleSystem(blockSize); // calculating the stiffness of the system by using the blockSize * particleRadius to get the height of the water
		ParticleSystem::getInstance()->addParticles(pos);
		ParticleSystem::getInstance()->addRigidbodies(rigidbodies);

		ParticleSystem::getInstance()->goNuts(animationTime, 0.016f, animFile);

		goto menu;
	}
	else if(stoi(s) == 2)
	{ }
	else if (stoi(s) == 3)
	{
		skybox = new Skybox("skyboxes\\ame_majesty\\");
		stateSpace = new StateSpace(window, skybox);

		StateSpace::activeStateSpace = stateSpace;

		glfwShowWindow(window);

		do {
			// Clear the screen
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			stateSpace->draw();

			// Swap buffers
			glfwSwapBuffers(window);
			glfwPollEvents();
		} while (glfwWindowShouldClose(window) == 0);

		goto menu;
	}

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}