#pragma once
#include "SPH.h"
#include "ParticleSystem.h"
#include <math.h>

void SPH::calcSPH()
{
	ParticleSystem* sys = ParticleSystem::getInstance();
	for (int i = 0; sys->particles.size(); ++i)
	{
		sys->calcNeighbors(sys->particles[i]);
		//if debug draw lines
	}

	for (int i = 0; sys->particles.size(); ++i)
	{
		sys->particles[i]->params.density = calcDensity(*sys->particles[i]);
		sys->particles[i]->params.pressure = calcPressure(*sys->particles[i]);
	}

	for (int i = 0; sys->particles.size(); ++i)
	{
		//calc right side equation
		double acceleration = calcAcceleration(*sys->particles[i]);
		double h; //TODO: = System.TimeStep;
		sys->particles[i]->params.velocity += h * acceleration;
		sys->particles[i]->nextPosition = sys->particles[i]->position;
		sys->particles[i]->nextPosition += h * sys->particles[i]->params.velocity;		
	}

	for (int i = 0; sys->particles.size(); ++i)
	{
		sys->particles[i]->position = sys->particles[i]->nextPosition;
		sys->grid.update(*sys->particles[i]);
		//TODO: RIGIDODY COLLISION
	}

	//Update grid
}

double SPH::calcDensity(Particle particle)
{
	double density = 0.0;
	ParticleSystem* sys = ParticleSystem::getInstance();
	for (int i = 0; i < particle.neighbors.size(); ++i)
	{
		Particle* currParticle = sys->particles[particle.neighbors.at(i)];
		double distance = glm::length(particle.position - currParticle->position);
		density += currParticle->params.mass * calcKernel(distance, sys->sysParams.searchRadius);
	}

	return density;
}

double SPH::calcPressure(const Particle& particle)
{
	ParticleSystem* sys = ParticleSystem::getInstance();
	return sys->sysParams.stiffness * (particle.params.density - sys->sysParams.restDensity);
}

double SPH::calcKernel(double distance, double h)
{
	if (distance < 0 || distance > h)
	{
		return;
	}

	double first = 15.0 / ((2 * glm::pi<double>())*(pow(h, 3)));
	double second = pow(-distance, 3) / (2 * (pow(h, 3)));
	double third = pow(distance, 2) / pow(h, 2);
	double fourth = h / (2 * distance);
	double fifth = -1.0;

	return first * (second + third + fourth + fifth);
}

double SPH::calcGradientW(double distance, double h)
{
	if (distance < 0 || distance > h)
	{
		return;
	}
	return ((315.0 * 3.0 * distance) / (32.0*glm::pi<double>()*pow(h, 9))) * pow((h*h) - (distance*distance), 2);
}

double SPH::calcLaplacianW(double distance, double h)
{
	if (distance < 0 || distance > h)
	{
		return;
	}

	return ((315.0 * 3.0 * distance * distance) / (8.0*glm::pi<double>()*pow(h, 9))) * ((h*h) - (distance*distance));
}

double SPH::calcGradient(Particle particle)
{
	ParticleSystem* sys = ParticleSystem::getInstance();
	double ret = 0.0;
	for (int i = 0; i < particle.neighbors.size(); ++i)
	{
		int index = particle.neighbors[i];
		double distance = glm::length(particle.position - sys->particles[index]->position);
		double h = sys->sysParams.searchRadius;
		ret += (sys->particles[index]->params.mass / sys->particles[index]->params.density) * (sys->particles[index]->params.pressure) * (calcGradientW(distance, h));
	}

	return ret;
}
double SPH::calcLaplacian(Particle particle)
{
	ParticleSystem* sys = ParticleSystem::getInstance();
	double ret = 0.0;
	for (int i = 0; i < particle.neighbors.size(); ++i)
	{
		int index = particle.neighbors[i];
		double distance = glm::length(particle.position - sys->particles[index]->position);
		double h = sys->sysParams.searchRadius;
		ret += (sys->particles[index]->params.mass / sys->particles[index]->params.density) * (sys->particles[index]->params.pressure) * (calcLaplacianW(distance, h));
	}

	return ret;
}

double SPH::calcAcceleration(const Particle& particle)
{
	ParticleSystem* sys = ParticleSystem::getInstance();
	return ((-1)*calcGradient(particle) + sys->sysParams.viscocity*calcLaplacian(particle) + particle.params.density * sys->sysParams.gravity) / particle.params.density;
}


