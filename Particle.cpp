#pragma once
#include "Particle.h"

Particle::Particle()
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
