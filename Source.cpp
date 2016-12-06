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
#include "GraphicsObject.h"
#include "Shader.h"
#include "StateSpace.h"
#include "ParticleSystem.h"
#include "Menu.h"
#include <windows.h>
#include <omp.h>
#include "SceneManager.h"
#include "OpenFileDialog.h"
#include "SaveFileDialog.h"

using namespace std;
using namespace glm;

GLFWwindow* window;
Skybox* skybox;

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

	//Check version of OpenGL and print
	std::printf("*** OpenGL Version: %s ***\n", glGetString(GL_VERSION));

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

	LitShader::shader = new LitShader("shaders\\litShader.VERTEXSHADER", "shaders\\litShader.FRAGMENTSHADER");
	InstancedLitShader::shader = new InstancedLitShader("shaders\\InstancedVertexShader.VERTEXSHADER", "shaders\\InstancedFragmentShader.FRAGMENTSHADER");
	UnlitShader::shader = new UnlitShader("shaders\\unlit2DShader.VERTEXSHADER", "shaders\\unlit2DShader.FRAGMENTSHADER");
	CubeMapShader::shader = new CubeMapShader("shaders\\CubeMap.VERTEXSHADER", "shaders\\CubeMap.FRAGMENTSHADER");
	GUIShader::shader = new GUIShader("shaders\\GUIShader.VERTEXSHADER", "shaders\\GUIShader.FRAGMENTSHADER");
	TextShader::shader = new TextShader("shaders\\TextShader.VERTEXSHADER", "shaders\\TextShader.FRAGMENTSHADER");

	Skybox* skybox = new Skybox("skyboxes\\ame_nebula\\");
	Scenes::stateSpace = new StateSpace(window, skybox);
	Scenes::sceneEditor = new SceneEditor(window, skybox);
	Scenes::menu = new Menu(window);
	StateSpace::activeStateSpace = Scenes::stateSpace;
	SceneManager::getInstance()->changeActiveScene(Scenes::menu);

	srand(time(NULL));

/*	string anotherChinchilla = OpenFileDialog::SelectFile();

	ifstream myFile(anotherChinchilla);
	vector<vec3> positions;
	
	if (myFile.is_open())
	{
		string yetAThirdChinchilla;
		for(int i = 0; i < 5; i++)
			getline(myFile, yetAThirdChinchilla);

		getline(myFile, yetAThirdChinchilla);

		stringstream ss(yetAThirdChinchilla);

		ss >> yetAThirdChinchilla;

		while (ss.rdbuf()->in_avail())
		{
			vec3 pos;

			for (int j = 0; j < 3; j++)
			{
				ss >> pos[j];
			}
			positions.push_back(pos);
		}
		myFile.close();
	}

	Rigidbody* chinchilla = ParticleSystem::rayTrace(&positions, 0.05f, 100);

	string c = SaveFileDialog::SaveFile();

	ofstream mS(c);

	if (mS.is_open())
	{
		for (int i = 0; i < chinchilla->vertices.size(); i++)
		{
			for (int j = 0; j < 3; j++)
			{
				mS << chinchilla->vertices[i].position[j] << " ";
			}

			for (int j = 0; j < 3; j++)
			{
				mS << chinchilla->vertices[i].normal[j] << " ";
			}

			for (int j = 0; j < 4; j++)
			{
				mS << chinchilla->vertices[i].color[j] << " ";
			}
		}

		mS << endl;

		for (int i = 0; i < chinchilla->indices.size(); i++)
		{
			mS << chinchilla->indices[i] << " ";
		}

		mS.close();
	}*/
}

int main()
{
	if (init() < 0)
		return -1;

	do {
		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		SceneManager::getInstance()->getActiveState()->execute();

		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();
	} while (glfwWindowShouldClose(window) == 0);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}