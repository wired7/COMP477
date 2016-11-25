#pragma once
#include "ParticleSystem.h"
#include "SPH.h"
#include "glm.hpp"
#include <thread>
#include <chrono>
#include <omp.h>
#include <fstream>
#

using namespace std::chrono;

ParticleSystem* ParticleSystem::particleSystem;

void rigidbodyIntersections(int i)
{
	int indices[] = { 0, 1, 2, 1, 3, 2, 0, 2, 4, 2, 6, 4, 4, 6, 5, 6, 7, 5, 5, 7, 1, 7, 3, 1, 1, 0, 4, 1, 4, 5, 2, 3, 6, 3, 7, 6 };
	ParticleSystem* pS = ParticleSystem::getInstance();

	for (int j = 0; j < pS->grid.data[i].size(); ++j)
		for (int k = 0; k < pS->grid.data[i][j].size(); ++k)
		{
			vec3 points[8];
			for (int l = 0; l < 2; ++l)
				for (int m = 0; m < 2; ++m)
					for (int n = 0; n < 2; ++n)
						points[l * 4 + m * 2 + n] = pS->grid.getCellSize() * (vec3(i, j, k) + vec3(l, m, n));

			vector<Triangle> gridCubeTriangles;

			for (int l = 0; l < 36; l += 3)
				gridCubeTriangles.push_back(Triangle(points[indices[l]], points[indices[l + 1]], points[indices[l + 2]]));

			vec3 min = points[0];
			vec3 max = points[7];

			for (int l = 0; l < pS->rigidbodies.size(); ++l)
			{
				for (int m = 0; m < pS->rigidbodies[l]->indices.size(); m += 3)
				{
					bool intersection = false;

					for (int n = m; n < m + 3; ++n)
					{
						vec3 pos = pS->rigidbodies[l]->vertices[pS->rigidbodies[l]->indices[n]].position;

						if (pos.x >= min.x && pos.x <= max.x)
							if (pos.y >= min.y && pos.y <= max.y)
								if (pos.z >= min.z && pos.z <= max.z)
								{
									pS->grid.data[i][j][k].rigidData.push_back(pair<int, MeshObject*>(m, pS->rigidbodies[l]));
									intersection = true;
									break;
								}
					}

					if (!intersection)
					{
						vec3 rigidbodyPositions[3];
						for (int n = 0; n < 3; ++n)
							rigidbodyPositions[n] = pS->rigidbodies[l]->vertices[pS->rigidbodies[l]->indices[m + n]].position;

						Triangle rigidbodyTriangle(rigidbodyPositions[0], rigidbodyPositions[1], rigidbodyPositions[2]);

						for (int n = 0; n < gridCubeTriangles.size(); ++n)
						{
							if (rigidbodyTriangle.intersects(gridCubeTriangles[n]))
							{
								pS->grid.data[i][j][k].rigidData.push_back(pair<int, MeshObject*>(m, pS->rigidbodies[l]));
								break;
							}
						}
					}
				}
			}
		}
}

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

void ParticleSystem::goNuts(float playbackTime, float frameRate, string filePath)
{
	milliseconds ms;

	float t = 0;

	ofstream myfile;
	myfile.open(filePath);
	myfile << sysParams.particleRadius << " " << sysParams.searchRadius << " " << sysParams.viscocity << " " << sysParams.stiffness << " ";
	myfile << sysParams.restDensity << " " << sysParams.gravity << " " << sysParams.tStep << " " << sysParams.maxTStep << " " << sysParams.mass << " " << endl;
	myfile.close();

	for (float simTime = 0; simTime < playbackTime; simTime += sysParams.tStep)
	{
		ms = duration_cast<milliseconds>(system_clock::now().time_since_epoch());

		if (t >= frameRate)
		{
			myfile.open(filePath, std::ios_base::app);

			myfile << sysParams.tStep;

			for (int i = 0; i < particles.size(); i++)
				myfile << " " << particles[i]->position.x << " " << particles[i]->position.y << " " << particles[i]->position.z;

			myfile << endl;

			myfile.close();

			t = 0;

			system("CLS");

			float deltaTime = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count() - ms.count();
			cout << (simTime / playbackTime) * 100 << "%" << endl;
			cout << "ETA: " << deltaTime * (playbackTime - simTime) / 1000 / 60 << " mins" << endl;
		}

		SPH::calcSPH();

		t += sysParams.tStep;
	}
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

void ParticleSystem::addRigidbodies(vector<Rigidbody*> rigid)
{
	rigidbodies = rigid;

	#pragma omp parallel for
	for (int i = 0; i < rigidbodies.size(); i++)
	{
		rigidbodies[i]->applyTransform();
	}

	milliseconds ms = duration_cast<milliseconds>(system_clock::now().time_since_epoch());

	#pragma omp parallel for
	for(int i = 0; i < grid.data.size(); ++i)
		rigidbodyIntersections(i);

//	cout << duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count() - ms.count() << endl;
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