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

void ParticleSystem::addParticles(vector<Particle*> part)
{
	particles = part;

	for (int i = 0; i < particles.size(); i++)
	{
		particles[i]->index = i;
		particles[i]->gridCellCoord = particles[i]->position / grid.getCellSize();
		grid.data[(int)particles[i]->gridCellCoord.x][(int)particles[i]->gridCellCoord.y][(int)particles[i]->gridCellCoord.z].particles.push_back(i);
	}
}

void ParticleSystem::updateList() {
	for (int i = 0; i < particles.size(); ++i) {
		particles[i]->position = particles[i]->nextPosition;
		grid.update(*particles[i]);
	}	
}

void ParticleSystem::calcNeighbors(Particle* particle)
{
	particle->neighbors.clear();
	particle->neighbors = grid.getNeighbors(*particle);
}

// Instantiate vector to heap, the class which receives this function must take care of the deletion of the pointer
vector<glm::vec3>* ParticleSystem::getParticlePositions() {
	
	vector<glm::vec3>* particlePositions = new vector<glm::vec3>();

	for (int i = 0; i < particles.size(); ++i) {
		particlePositions->push_back(particles[i]->position);
	}

	return particlePositions;
}