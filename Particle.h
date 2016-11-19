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
};

class Particle {
public:
	Particle();
	~Particle();
	glm::vec3 position;
	glm::vec3 nextPosition;
	int getIndex();
	glm::vec3 getGridCellCoord();
	void setGridCellCoord(glm::vec3 gridCellCoord);
	const float g = -9.81f;
	vector<int> neighbors;
	ParticleParameters params;
	glm::vec3 collisionNormal;
private:
	int index;
	glm::vec3 gridCellCoord;
};
