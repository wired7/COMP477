#pragma once
#include <vector>
#include <glm.hpp>

using namespace std;

struct ParticleParameters {
	float mass;
	glm::vec3 velocity;
	float density;
	float pressure;
	float kernel;
	glm::vec3 gradientPressure;
	glm::vec3 laplacianVelocity;

	ParticleParameters(float m, glm::vec3 v) : mass(m), velocity(v) {}
};

class Particle {
public:
	int index;
	glm::vec3 gridCellCoord;
	Particle(glm::vec3);
	~Particle();
	glm::vec3 position;
	glm::vec3 nextPosition;
	int getIndex();
	glm::vec3 getGridCellCoord();
	void setGridCellCoord(glm::vec3 gridCellCoord);
	vector<int> neighbors;
	ParticleParameters params;
	glm::vec3 collisionNormal;
};
