#pragma once
#include "Particle.h"

class SPH
{
public:
	void calcSPH();
private:
	float calcDensity(Particle particle);
	float calcPressure(const Particle& particle);
	float calcKernel(float distance, float h);
	glm::vec3 calcGradientW(glm::vec3 distance, float h);
	glm::vec3 calcLaplacianW(glm::vec3 distance, float h);
	glm::vec3 calcGradient(Particle particle);
	glm::vec3 calcLaplacian(Particle particle);
	glm::vec3 calcAcceleration(const Particle& particle);
};