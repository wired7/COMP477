#include "Camera.h"

Camera* Camera::activeCamera = NULL;

Camera::Camera(GLFWwindow* window, vec2 relativePosition, vec2 relativeDimensions, vec3 pos, vec3 lookAt, vec3 up, mat4 Projection)
{
	this->window = window;
	this->relativePosition = relativePosition;
	this->relativeDimensions = relativeDimensions;
	glfwGetWindowSize(window, &screenWidth, &screenHeight);

	mat4 ratioMatrix(1);
	ratioMatrix[1][1] *= relativeDimensions.x;
	Projection = Projection * ratioMatrix;
	OrthoProjection = glm::ortho(0.0f, (float)1200, 0.0f, (float)800);
	this->Projection = Projection;

	View = glm::lookAt(pos, lookAt, up);
	camPosVector = pos;
	lookAtVector = lookAt;
	upVector = up;
}

void Camera::setViewport()
{
	glfwGetWindowSize(window, &screenWidth, &screenHeight);
	glViewport(relativePosition.x * screenWidth, relativePosition.y * screenHeight, relativeDimensions.x * screenWidth, relativeDimensions.y * screenHeight);
}

void Camera::setCamera(Camera* cam)
{
	activeCamera = cam;
	activeCamera->setViewport();
}

int Camera::getScreenWidth()
{
	int width;
	int height;
	glfwGetWindowSize(window, &width, &height);
	return width;
}

int Camera::getScreenHeight()
{
	int width;
	int height;
	glfwGetWindowSize(window, &width, &height);
	return height;
}

GLFWwindow* Camera::getWindow()
{
	return window;
}

SphericalCamera::SphericalCamera(GLFWwindow* window, vec2 relativePosition, vec2 relativeDimensions, vec3 pos, vec3 lookAt, vec3 up, mat4 Projection) :
	Camera(window, relativePosition, relativeDimensions, pos, lookAt, up, Projection)
{
	distance = length(camPosVector - lookAtVector);
	maxCamPhi = 0.7;

	//camTheta = atan2(pos.z - lookAt.z, pos.x - lookAt.x);
	update();
}

void SphericalCamera::update()
{
	if (camPhi > maxCamPhi)
		camPhi = maxCamPhi;
	else if (camPhi < maxCamPhi * -1)
		camPhi = maxCamPhi * -1;

	camPosVector = distance * vec3(cos(camTheta) * cos(camPhi), sin(camPhi), sin(camTheta) * cos(camPhi)) + lookAtVector;
	upVector = vec3(-cos(camTheta) * sin(camPhi), cos(camPhi), -sin(camTheta) * sin(camPhi));
	View = lookAt(camPosVector, lookAtVector, upVector);
}

StateSpaceCamera::StateSpaceCamera(GLFWwindow* window, vec2 relativePosition, vec2 relativeDimensions, vec3 pos, vec3 lookAt, vec3 up, mat4 Projection, Terrain* terrain) :
	Camera(window, relativePosition, relativeDimensions, pos, lookAt, up, Projection)
{
	this->terrain = terrain;
	maxCamPhi = 0.7;

	//camTheta = atan2(lookAt.z - pos.z, lookAt.x - pos.x);

	update();
}

StateSpaceCamera::StateSpaceCamera(GLFWwindow* window, vec2 relativePosition, vec2 relativeDimensions, vec3 pos, vec3 lookAt, vec3 up, mat4 Projection) :
	Camera(window, relativePosition, relativeDimensions, pos, lookAt, up, Projection)
{
	maxCamPhi = 0.7;

	camTheta = atan2(lookAt.z - pos.z, lookAt.x - pos.x);

	update();
}

void StateSpaceCamera::translate(vec2 offset)
{
	vec3 diff(cos(camTheta), 0, sin(camTheta));

	if (length(camPosVector + diff * vec3(offset.x, 0, offset.y)) < 15)
		camPosVector += diff * vec3(offset.x, 0, offset.y);
}

// Method to call if the camera needs to update its matrix
void StateSpaceCamera::update()
{
	if (camPhi > maxCamPhi)
		camPhi = maxCamPhi;
	else if (camPhi < maxCamPhi * -1)
		camPhi = maxCamPhi * -1;

	vector<double> minDiff = { INFINITY, INFINITY, INFINITY };
	vector<int> minIndex = { -1, -1, -1 };

	if (terrain != nullptr)
	{
		for (int i = 0; i < terrain->vertices.size(); i++)
		{
			vec3 temp(terrain->vertices[i].position);
			temp = vec3(terrain->model * vec4(temp, 1));
			vec2 diff2D(camPosVector.x - temp.x, camPosVector.z - temp.z);
			if (length(diff2D) < minDiff[0])
			{
				minDiff[2] = minDiff[1];
				minDiff[1] = minDiff[0];
				minDiff[0] = length(diff2D);
				minIndex[2] = minIndex[1];
				minIndex[1] = minIndex[0];
				minIndex[0] = i;
			}
			else if (length(diff2D) < minDiff[1])
			{
				minDiff[2] = minDiff[1];
				minDiff[1] = length(diff2D);
				minIndex[2] = minIndex[1];
				minIndex[1] = i;
			}
			else if (length(diff2D) < minDiff[2])
			{
				minDiff[2] = length(diff2D);
				minIndex[2] = i;
			}
		}

		Plane plane(vec3(terrain->model * vec4(terrain->vertices[minIndex[0]].position, 1)), vec3(terrain->model * vec4(terrain->vertices[minIndex[1]].position, 1)), vec3(terrain->model * vec4(terrain->vertices[minIndex[2]].position, 1)));

		camPosVector.y = plane.intersection(vec3(camPosVector.x, 0, camPosVector.z), vec3(0, 1, 0)) + 2;
	}
	lookAtVector = camPosVector + vec3(cos(camTheta) * cos(camPhi), sin(camPhi), sin(camTheta) * cos(camPhi));

	View = lookAt(camPosVector, lookAtVector, upVector);

	int width, height;
	glfwGetWindowSize(window, &width, &height);
	Projection = perspective(45.0f, (float)width / height, 0.1f, 100.0f);
	//OrthoProjection = glm::ortho(0.0f, (float)width, 0.0f, (float)height);
}

MenuCamera::MenuCamera(GLFWwindow* window, vec2 relativePosition, vec2 relativeDimensions, vec3 pos, vec3 lookAt, vec3 up, mat4 Projection) :
	Camera(window, relativePosition, relativeDimensions, pos, lookAt, up, Projection)
{

}

void MenuCamera::update()
{
	/*
	int width;
	int height;
	glfwGetWindowSize(window, &width, &height);
	OrthoProjection = glm::ortho(0.0f, (float)width, 0.0f, (float)height);
	*/
	
}