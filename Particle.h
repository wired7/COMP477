#pragma once
#include <vector>
#include <glm.hpp>
#include "Shape.h"

using namespace std;

struct ParticleParameters {
	glm::vec3 velocity = glm::vec3(0,0,0);
	glm::vec3 acceleration = glm::vec3(0, 0, 0);
	float density;
	float pressure;
	float kernel;
	glm::vec3 gradientPressure;
	glm::vec3 laplacianVelocity;
	glm::vec3 tensionForce;
	glm::vec3 gradientSmoothColor;
	glm::vec3 laplacianSmoothColor;
	ParticleParameters(glm::vec3 v) : velocity(v) {}
};

class Particle {
public:
	int index;
	glm::vec3 gridCellCoord;
	Particle(glm::vec3);
	~Particle();
	glm::vec3 position = glm::vec3(0,0,0);
	glm::vec3 nextPosition = glm::vec3(0, 0, 0);
	int getIndex();
	glm::vec3 getGridCellCoord();
	void setGridCellCoord(const glm::vec3& gridCellCoord);
	vector<int> neighbors;
	ParticleParameters params;
	vec3 collisionNormal;
	float deltaTime;
};
