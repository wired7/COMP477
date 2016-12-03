#pragma once
#include <iostream>
#include "glew.h"
#include "glfw3.h"
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include "Shape.h"
#include "Terrain.h"

using namespace glm;
using namespace std;

class Camera
{
public:
	static Camera* activeCamera;
	static void setCamera(Camera*);
	GLFWwindow* window;
	vec2 relativePosition;
	vec2 relativeDimensions;
	mat4 Projection;
	mat4 OrthoProjection;
	mat4 View;
	vec3 lookAtVector;
	vec3 camPosVector;
	vec3 upVector;

	Camera(GLFWwindow* window, vec2 relativePosition, vec2 relativeDimensions, vec3 pos, vec3 lookAt, vec3 up, mat4 Projection);
	virtual ~Camera() {};
	virtual void update() = 0;
	void setViewport();

	int getScreenWidth();
	int getScreenHeight();
	GLFWwindow* getWindow();

private:
	int screenWidth;
	int screenHeight;
};

class SphericalCamera : public Camera
{
public:
	double camTheta = 0;
	double camPhi = 0;
	GLfloat distance;

	SphericalCamera(GLFWwindow* window, vec2 relativePosition, vec2 relativeDimensions, vec3 pos, vec3 lookAt, vec3 up, mat4 Projection);
	virtual void update();

private:
	double maxCamPhi;
};

class StateSpaceCamera : public Camera
{
public:
	double camTheta = 0;
	double camPhi = 0;
	Terrain* terrain;
	StateSpaceCamera(GLFWwindow* window, vec2 relativePosition, vec2 relativeDimensions, vec3 pos, vec3 lookAt, vec3 up, mat4 Projection, Terrain* terrain);
	StateSpaceCamera(GLFWwindow* window, vec2 relativePosition, vec2 relativeDimensions, vec3 pos, vec3 lookAt, vec3 up, mat4 Projection);
	void translate(vec2);
	void update();

private:
	double maxCamPhi;
};

class MenuCamera : public Camera
{
public:
	MenuCamera(GLFWwindow* window, vec2 relativePosition, vec2 relativeDimensions, vec3 pos, vec3 lookAt, vec3 up, mat4 Projection);
	void update();
};

