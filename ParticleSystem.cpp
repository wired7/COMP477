#pragma once
#include "ParticleSystem.h"
#include "glm.hpp"

ParticleSystem* ParticleSystem::particleSystem;

ParticleSystem* ParticleSystem::getInstance()
{
	if (particleSystem == nullptr)
	{
		particleSystem = new ParticleSystem();
	}
	return particleSystem;
}

ParticleSystem::ParticleSystem() 
{

}
ParticleSystem::~ParticleSystem()
{

}

void ParticleSystem::updateList() {
	
}

void ParticleSystem::calcNeighbors(Particle* particle)
{
	particle->neighbors.clear();
	particle->neighbors = grid.getNeighbors(*particle);
}
