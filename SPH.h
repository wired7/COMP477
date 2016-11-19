#pragma once
#include "Particle.h"

class SPH
{
public:
	void calcSPH();
private:
	float calcDensity(Particle particle);
	float calcPressure(const Particle& particle);
	float calcKernel(glm::vec3 distance, float h);
	glm::vec3 calcGradientW(glm::vec3 distance, float h);
	float calcLaplacianW(glm::vec3 distance, float h);
	glm::vec3 calcGradientPressure(Particle particle);
	glm::vec3 calcLaplacianVelocity(Particle particle);
	glm::vec3 calcAcceleration(const Particle& particle);
};