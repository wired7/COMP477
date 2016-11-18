#pragma once
#include "Particle.h"

class SPH
{
public:
	void calcSPH();
private:
	double calcDensity(Particle particle);
	double calcPressure(const Particle& particle);
	double calcKernel(double distance, double h);
	double calcGradientW(double distance, double h);
	double calcLaplacianW(double distance, double h);
	double calcGradient(Particle particle);
	double calcLaplacian(Particle particle);
	double calcAcceleration(const Particle& particle);
};