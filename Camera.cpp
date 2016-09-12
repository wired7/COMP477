#include "Camera.h"

Camera* Camera::activeCamera = NULL;

Camera::Camera(GLFWwindow* window, vec2 relativePosition, vec2 relativeDimensions, vec3 pos, vec3 lookAt, vec3 up, mat4 Projection)
{
	this->window = window;
	this->relativePosition = relativePosition;
	this->relativeDimensions = relativeDimensions;

	mat4 ratioMatrix(1);
	ratioMatrix[1][1] *= relativeDimensions.x;
	Projection = Projection * ratioMatrix;
	this->Projection = Projection;

	View = glm::lookAt(pos, lookAt, up);
	camPosVector = pos;
	lookAtVector = lookAt;
	upVector = up;
}

void Camera::setViewport()
{
	int width;
	int height;
	glfwGetWindowSize(window, &width, &height);
	glViewport(relativePosition.x * width, relativePosition.y * height, relativeDimensions.x * width, relativeDimensions.y * height);
}

void Camera::setCamera(Camera* cam)
{
	activeCamera = cam;
	activeCamera->setViewport();
}



SphericalCamera::SphericalCamera(GLFWwindow* window, vec2 relativePosition, vec2 relativeDimensions, vec3 pos, vec3 lookAt, vec3 up, mat4 Projection) :
Camera(window, relativePosition, relativeDimensions, pos, lookAt, up, Projection)
{
	distance = length(camPosVector - lookAtVector);
	update();
}

void SphericalCamera::update()
{
	camPosVector = distance * vec3(cos(camTheta) * cos(camPhi), sin(camPhi), sin(camTheta) * cos(camPhi)) + lookAtVector;
	upVector = vec3(-cos(camTheta) * sin(camPhi), cos(camPhi), -sin(camTheta) * sin(camPhi));
	View = lookAt(camPosVector, lookAtVector, upVector);
}

StateSpaceCamera::StateSpaceCamera(GLFWwindow* window, vec2 relativePosition, vec2 relativeDimensions, vec3 pos, vec3 lookAt, vec3 up, mat4 Projection, Terrain* terrain) :
	Camera(window, relativePosition, relativeDimensions, pos, lookAt, up, Projection)
{
	this->terrain = terrain;

	update();
}

void StateSpaceCamera::translate(vec2 offset)
{
	vec3 diff(cos(camTheta), 0, sin(camTheta));

	if(length(camPosVector + diff * vec3(offset.x, 0, offset.y)) < 15)
		camPosVector += diff * vec3(offset.x, 0, offset.y);
}

void StateSpaceCamera::update()
{
	vector<double> minDiff = {INFINITY, INFINITY, INFINITY};
	vector<int> minIndex = {-1, -1, -1};
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
	lookAtVector = camPosVector + vec3(cos(camTheta) * cos(camPhi), sin(camPhi), sin(camTheta) * cos(camPhi));

	View = lookAt(camPosVector, lookAtVector, upVector);

	int width, height;
	glfwGetWindowSize(window, &width, &height);
	Projection = perspective(45.0f, (float)width / height, 0.1f, 100.0f);
}