#pragma once
#include <vector>
#include <glm.hpp>

using namespace std;

class Particle {
public:
	Particle();
	~Particle();
	glm::vec3 position;
	glm::vec3 nextPosition;
	int getIndex();
	glm::vec3 getGridCellCoord();
	void setGridCellCoord(glm::vec3 gridCellCoord);
	const double g = -9.81;
	vector<int> neighbors;
	ParticleParameters params;
private:
	int index;
	glm::vec3 gridCellCoord;
};

struct ParticleParameters{
	double mass;
	double velocity;
	double density;
	double pressure;
	double kernel;
	double gradientPressure = 0;
	double laplacianVelocity = 0;
};