#pragma once
#include "Particle.h"

static class SPH
{
public:
	static void calcSPH();

	static float calcDensity(Particle particle);
	static float calcPressure(const Particle& particle);

	static float calcKernel(glm::vec3 distance, float h);
	static glm::vec3 calcGradientKernel(glm::vec3 distance, float h);
	static float calcLaplacianKernel(glm::vec3 distance, float h);

	static glm::vec3 calcGradientPressure(Particle particle);
	static glm::vec3 calcLaplacianVelocity(Particle particle);
	static glm::vec3 calcSurfaceTension(Particle particle);
	static glm::vec3 calcAcceleration(Particle particle);

	// TEST
	static float calcSmoothedColor(Particle particle);
	static glm::vec3 calcGradientColor(Particle particle);
	static glm::vec3 calcLaplacianColor(Particle particle);

};