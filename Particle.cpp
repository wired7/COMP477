#pragma once
#include "Particle.h"
#include <iostream>


Particle::Particle(glm::vec3 pos) : position(pos), nextPosition(pos), params(ParticleParameters(glm::vec3(0,0,0)))
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

void Particle::setGridCellCoord(const glm::vec3& gridCellCoord)
{
	this->gridCellCoord = gridCellCoord;
}
