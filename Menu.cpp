#include "Menu.h"
#include <glfw3.h>
#include "OpenFileDialog.h"
#include "SaveFileDialog.h"
#include "Skybox.h"
#include "StateSpace.h"
#include "SceneManager.h"
#include "FileStorage.h"

GLFWwindow* windowHandle;

void newSimulation()
{
	OpenFileDialog dialog;

	string sysParamsFile = dialog.SelectFile();

	auto sysParams = FileStorage::loadSysParams(sysParamsFile);

	string rigidBodiesFile = dialog.SelectFile();

	auto rigidBodies = FileStorage::loadRigidbodies(rigidBodiesFile);

	string simParams;
	system("CLS");
	cout << "Enter number of particles: " << endl;
	getline(cin, simParams);

	/*
	int blockSize = pow(stof(simParams), 1.0f/3.0f);
	*/

	/// Dexter's way of placing particles
	// New Way of initializing position of water
	int numOfParticles = stoi(simParams);

	// setting up the particles in the scene
	int numOfParticlesPerMeterCube = ceil(numOfParticles / sysParams.volume);
	int particlesPerMeter = ceil(pow(numOfParticlesPerMeterCube, 1.0f / 3.0f));
	float distanceOwnedByParticle = 1.0f / particlesPerMeter;
	float offsetFromBoundary = distanceOwnedByParticle / 2.0f;

	cout << numOfParticlesPerMeterCube << " " << particlesPerMeter << " " << distanceOwnedByParticle << " " << offsetFromBoundary << endl;

	// get height, length, width of the body of water
	// Assuming it's a cube for now
	float heightWater;
	float lengthWater;
	float widthWater;

	heightWater = lengthWater = widthWater = pow(sysParams.volume, 1.0f / 3.0f);

	cout << "Dimension h x l x w " << heightWater << " " << lengthWater << " " << widthWater << endl;

	cout << "Enter animation time" << endl;
	getline(cin, simParams);
	float animationTime = stof(simParams);

	cout << "Save animation as..." << endl;

	SaveFileDialog saveDialog;

	string animFile = saveDialog.SaveFile();

	Bounds b;

	for (int i = 0; i < rigidBodies.size(); i++)
	{
		b.join(rigidBodies[i]->getBounds());
	}

	vec3 center = vec3(1, 1, 1) - b.min;
	vec3 maximum = center + b.max + vec3(1, 1, 1);

	int gridSize = max(maximum.x, max(maximum.y, maximum.z));

	int floorHeight = floor(heightWater * particlesPerMeter);
	int floorWidth = floor(widthWater * particlesPerMeter);
	int floorLength = floor(lengthWater * particlesPerMeter);

	cout << "NumberOfParticles/meter: h x l x w " << floorHeight << " " << floorWidth << " " << floorLength << endl;

	vector<Particle*> pos;
	for (int h = 0; h < floorHeight; ++h) {
		for (int w = 0; w < floorWidth; ++w) {
			for (int l = 0; l < floorLength; ++l) {
				pos.push_back(new Particle(center + vec3(h * distanceOwnedByParticle - offsetFromBoundary, w * distanceOwnedByParticle - offsetFromBoundary, l * distanceOwnedByParticle - offsetFromBoundary)));
			}
		}
	}

	vector<Rigidbody*> rigidbodies;

	for (int i = 0; i < rigidBodies.size(); i++)
	{
		rigidBodies[i]->applyTransform();
		rigidBodies[i]->model = translate(rigidBodies[i]->model, center);
		rigidbodies.push_back(new Rigidbody(rigidBodies[i]->vertices, rigidBodies[i]->indices, rigidBodies[i]->model, 1000, false));
	}


	ParticleSystem::getInstance()->sysParams = sysParams;
	ParticleSystem::getInstance()->grid = Grid3D(gridSize / sysParams.searchRadius, sysParams.searchRadius);
	ParticleSystem::getInstance()->addParticles(pos);
	ParticleSystem::getInstance()->addRigidbodies(rigidbodies);

	// set up mass and stiffness of system
	ParticleSystem::getInstance()->setStiffnessOfParticleSystem(); // calculating the stiffness of the system by using the height of water * particleRadius to get the height of the water
	ParticleSystem::getInstance()->calculateMassOfParticles();

	ParticleSystem::getInstance()->goNuts(animationTime, 0.016f, animFile);

	cout << "Animation complete!" << std::endl;
}

void runSimulation()
{
	Skybox* skybox = new Skybox("skyboxes\\ame_nebula\\");
	Scenes::stateSpace = new StateSpace(windowHandle, skybox);
	StateSpace::activeStateSpace = Scenes::stateSpace;
	if (StateSpace::activeStateSpace->loadAnimation() == 1)
		SceneManager::getInstance()->changeActiveScene(Scenes::stateSpace);
}

void exitSim()
{
	glfwSetWindowShouldClose(windowHandle, 1);
}

void enterEditor()
{
	Skybox* skybox = new Skybox("skyboxes\\ame_nebula\\");
	Scenes::sceneEditor = new SceneEditor(Camera::activeCamera->getWindow(), skybox);
	Scenes::sceneEditor->setup();
	SceneManager::getInstance()->changeActiveScene(Scenes::sceneEditor);
}


Menu::Menu(GLFWwindow* window)
{
	this->window = window;
	windowHandle = window;
	observer = new MenuCamera(window, vec2(0, 0), vec2(1, 1), vec3(0, 0, 0), vec3(0, 0, -1), vec3(0, 1, 0), perspective(45.0f, 1.0f, 0.1f, 100.0f));
	Camera::activeCamera = observer;
	Controller::setController(MenuController::getController());
	menuButtons.push_back(new GUIButton(vec3(600, 600, 0), vec3(180, 70, 1), vec4(1.0, 1.0, 1.0, 1.0), "Create Simulation", vec4(0.0f, 0.0f, 0.0f, 1.0f), "textures\\button.png", true, newSimulation));
	menuButtons.push_back(new GUIButton(vec3(600, 450, 0), vec3(180, 70, 1), vec4(1.0, 1.0, 1.0, 1.0), "Run Simulation", vec4(0.0f, 0.0f, 0.0f, 1.0f), "textures\\button.png", true, runSimulation));
	menuButtons.push_back(new GUIButton(vec3(600, 300, 0), vec3(180, 70, 1), vec4(1.0, 1.0, 1.0, 1.0), "Exit Program", vec4(0.0f, 0.0f, 0.0f, 1.0f), "textures\\button.png", true, exitSim));
	menuButtons.push_back(new GUIButton(vec3(600, 150, 0), vec3(180, 70, 1), vec4(1.0, 1.0, 1.0, 1.0), "Enter Editor", vec4(0.0f, 0.0f, 0.0f, 1.0f), "textures\\button.png", true, enterEditor));
	background = new GUIBackground(vec3(0.0f, 0.0f, 0.0f), 
								   vec3(1200, 
								   800, 1.0f), 
								   vec4(1.0f, 1.0f, 1.0f, 1.0f), "textures\\menuBack.jpg", true);
}


Menu::~Menu()
{
}

void Menu::execute()
{
	this->activeState = activeState;
	draw();
	checkInput();
}

void Menu::draw()
{
	glDisable(GL_DEPTH_TEST);

	observer->setViewport();

	background->draw();

	for (int i = 0; i < menuButtons.size(); i++)
		menuButtons[i]->draw();

	//	bucket->draw();
}

void Menu::checkInput()
{
	//hover
	for (int i = 0; i < menuButtons.size(); i++)
	{
		menuButtons[i]->checkHover();
	}

	static int oldLeftClickState = GLFW_RELEASE;

	int leftClick = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
	if (leftClick == GLFW_PRESS && oldLeftClickState == GLFW_RELEASE)
	{
		oldLeftClickState = GLFW_PRESS;
		for (int i = 0; i < menuButtons.size(); i++)
			menuButtons[i]->checkMouseClick();

	}

	oldLeftClickState = leftClick;
}

