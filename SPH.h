#pragma once
#include "Particle.h"

static class SPH
{
public:
	static void calcSPH();

	static float calcDensity(Particle particle);
	static float calcPressure(const Particle& particle);
	static float calcDensityKernel(glm::vec3 distance, float h);
	static glm::vec3 calcGradientPressureKernel(glm::vec3 distance, float h);
	static float calcLaplacianViscosityKernel(glm::vec3 distance, float h);
	static glm::vec3 calcGradientPressure(Particle particle);
	static glm::vec3 calcLaplacianVelocity(Particle particle);
	static glm::vec3 calcAcceleration(const Particle& particle);
};