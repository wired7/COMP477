#pragma once
#include "Particle.h"

Particle::Particle(glm::vec3 pos) : position(pos), params(ParticleParameters(0.1f, 0.5f * glm::normalize(glm::vec3((float)rand() / RAND_MAX, (float)rand() / RAND_MAX, (float)rand() / RAND_MAX))))
{
}

Particle::~Particle()
{
}

int Particle::getIndex()
{
	return index;
}

glm::vec3 Particle::getGridCellCoord()
{
	return gridCellCoord;
}

void Particle::setGridCellCoord(glm::vec3 gridCellCoord)
{
	this->gridCellCoord = gridCellCoord;
}
