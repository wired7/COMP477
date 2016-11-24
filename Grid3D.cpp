#pragma once
#include "Grid3D.h"
#include "ParticleSystem.h"

Grid3D::Grid3D(int numCells, float cellSize): numCells(numCells), cellSize(cellSize)
{
	//initializes our 3D vector to be (numCell X numCells X numCells)
	data = vector<vector<vector<GridCube>>>();

	for (int i = 0; i < numCells; i++)
	{
		data.push_back(vector<vector<GridCube>>());
		for (int j = 0; j < numCells; j++)
		{
			data[i].push_back(vector<GridCube>());
			for (int k = 0; k < numCells; k++)
				data[i][j].push_back(GridCube());
		}
	}
}

Grid3D::Grid3D() {

}

Grid3D::~Grid3D() {

}

float Grid3D::getCellSize()
{
	return cellSize;
}

//updates a particle to the correct
void Grid3D::update(Particle particle)
{
	//TODO: particle remove itself from previous cell it was in?

	int gridX = particle.position.x / cellSize;
	int gridY = particle.position.y / cellSize;
	int gridZ = particle.position.z / cellSize;

	bool offGrid = (gridX >= numCells || gridY >= numCells || gridZ >= numCells || gridX < 0 || gridY < 0 || gridZ < 0);
	
	glm::vec3 currCell = particle.getGridCellCoord();
	bool remove = (gridX != (int)currCell.x || gridY != (int)currCell.y || gridZ != (int)currCell.z);

	if (remove)
	{
		GridCube* cubeptr = &data[(int)currCell.x][(int)currCell.y][(int)currCell.z];
		// This can probably be optimized using the object's name
		for (int i = 0; i < cubeptr->particles.size(); ++i)
		{
			if (cubeptr->particles.at(i) == particle.getIndex())
			{
				cubeptr->particles.erase(cubeptr->particles.begin() + i);
			}
		}
	}

	if (!offGrid && remove)
	{
		data[gridX][gridY][gridZ].particles.push_back(particle.getIndex());
		particle.setGridCellCoord(glm::vec3(gridX, gridY, gridZ));
	}
	else
	{
		//TODO
	}

}

vector<GridCube> Grid3D::getNeighborCubes(Particle particle)
{
	//+00, 0+0, 00+
	//++-, +-+, -++
	//+--, --+, -+-
	//-00, 0-0, 00-
	//0+-, -0+, +-0, -+0, 0-+, +0-
	//0++, +0+, ++0
	//0--, -0-, --0
	//+++
	//---

	glm::vec3 currCell = particle.getGridCellCoord();
	int x = currCell.x;
	int y = currCell.y;
	int z = currCell.z;

	vector<GridCube> neighborCubes;
	glm::vec3 incr(0, -1, 1);
	for (int i = 0; i < 3; ++i)
	{
		if (x + incr[i] < 0 || x + incr[i] >= data.size()) {
			continue;
		}
		for (int j = 0; j < 3; ++j)
		{
			if (y + incr[j] < 0 || y + incr[j] >= data[x + incr[i]].size()) {
				continue;
			}
			for (int k = 0; k < 3; ++k)
			{
				if (z + incr[k] < 0 || z + incr[k] >= data[x + incr[i]][y + incr[j]].size()) {
					continue;
				}
				GridCube temp = data[x + incr[i]][y + incr[j]][z + incr[k]];
				neighborCubes.push_back(temp);
			}
		}
	}
	return neighborCubes;
}

vector<int> Grid3D::getNeighbors(Particle particle)
{
	vector<GridCube> neighborCubes = getNeighborCubes(particle);
	
	vector<int> ret;
	
	ParticleSystem* ps = ParticleSystem::getInstance();
	for (int i = 0; i < neighborCubes.size(); ++i)
	{
		int particlesSize = neighborCubes[i].particles.size();

		GridCube* currCube = &neighborCubes[i];
		for (int j = 0; j < particlesSize; j++)
		{
			int particleIndex = currCube->particles.at(j);

			if (particleIndex > particle.getIndex() && inRadius(particle.position, ps->particles[particleIndex]->position))
			{
				ret.push_back(particleIndex);
			}
		}
	}

	return ret;
}

bool Grid3D::inRadius(glm::vec3 pos1, glm::vec3 pos2)
{
	return (glm::length(pos1 - pos2) <= ParticleSystem::getInstance()->sysParams.searchRadius);
}
