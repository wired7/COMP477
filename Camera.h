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
	mat4 View;
	vec3 lookAtVector;
	vec3 camPosVector;
	vec3 upVector;

	Camera(GLFWwindow*, vec2, vec2, vec3, vec3, vec3, mat4);
	virtual ~Camera() {};
	virtual void update() = 0;
	void setViewport();

	int getScreenWidth();
	int getScreenHeight();

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

	SphericalCamera(GLFWwindow*, vec2, vec2, vec3, vec3, vec3, mat4);
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
	StateSpaceCamera(GLFWwindow*, vec2, vec2, vec3, vec3, vec3, mat4, Terrain*);
	void translate(vec2);
	void update();

private:
	double maxCamPhi;
};

class MenuCamera : public Camera
{
public:
	MenuCamera(GLFWwindow*, vec2, vec2, vec3, vec3, vec3, mat4);
	void update();
};

