#pragma once
#include "SPH.h"
#include "ParticleSystem.h"
#include "Shape.h"
#include <math.h>
#include <chrono>
#include <thread>

#define THREADSIZE 200

using namespace std::chrono;

void neighbors(ParticleSystem* pS, int i, int* count)
{
	for (int j = i; (j < i + THREADSIZE) && (j < pS->particles.size()); j++, *count++)
	{
		pS->calcNeighbors(pS->particles[j]);
	}
}

void densitiesPressures(ParticleSystem* pS, int i, int* count)
{
	for (int j = i; j < i + THREADSIZE && j < pS->particles.size(); j++, *count++)
	{
		pS->particles[j]->params.density = SPH::calcDensity(*pS->particles[j]);
		pS->particles[j]->params.pressure = SPH::calcPressure(*pS->particles[j]);
	}
}

void eulerTimeIntegrations(ParticleSystem* pS, int i, int* count)
{
	for (int j = i; j < i + THREADSIZE && j < pS->particles.size(); j++, *count++)
	{
		//calc right side equation
		vec3 acceleration = SPH::calcAcceleration(*pS->particles[j]);
		pS->particles[j]->params.velocity += pS->sysParams.tStep * acceleration;
		pS->particles[j]->nextPosition = pS->particles[j]->position;
		pS->particles[j]->nextPosition += pS->sysParams.tStep * pS->particles[j]->params.velocity;
	}
}

void collisionsSubFunction(ParticleSystem*, int);

void collisions(ParticleSystem* pS, int i, int* count)
{
	for (int n = i; n < i + THREADSIZE && n < pS->particles.size(); n++, *count++)
	{
		collisionsSubFunction(pS, n);
	}
}

void collisionsSubFunction(ParticleSystem* pS, int n)
{
	Particle* currParticle = pS->particles[n];

	pS->sysParams.tStep = pS->sysParams.maxTStep;
	vector<GridCube> vec = pS->grid.getNeighborCubes(*currParticle);

	for (int j = 0; j < vec.size(); ++j)
	{
		vector<pair<int, MeshObject*>> rigidData = vec[j].rigidData;
		for (int k = 0; k < rigidData.size(); ++k)
		{
			int indexInMesh = rigidData[k].first;
			int vertexInMesh1 = rigidData[k].second->indices[indexInMesh];
			int vertexInMesh2 = rigidData[k].second->indices[indexInMesh + 1];
			int vertexInMesh3 = rigidData[k].second->indices[indexInMesh + 2];

			vec3 point1 = rigidData[k].second->vertices[vertexInMesh1].position;
			vec3 point2 = rigidData[k].second->vertices[vertexInMesh2].position;
			vec3 point3 = rigidData[k].second->vertices[vertexInMesh3].position;

			Plane plane(point1, point2, point3);
			vec3 normal = normalize(plane.normal);
			float distance = abs(dot(normal, currParticle->nextPosition) - dot(normal, plane.point));

			if (distance <= pS->sysParams.particleRadius)
			{
				if (currParticle->collisionNormal != normal && currParticle->collisionNormal != -normal)
				{
					vec3 origin = currParticle->nextPosition;
					vec3 direction = normal;
					Triangle triangle(point1, point2, point3);

					if (triangle.intersects(origin, direction))
					{
						float distanceToPlaneAtCollision = plane.intersection(origin, direction);

						direction = -sign(distanceToPlaneAtCollision) * direction;
						currParticle->collisionNormal = direction;
						//theres a collision. Update velocity based on conservation of momentum.
						//						float t = distanceToPlaneAtCollision / length(currParticle->params.velocity);
						/*						if (t < sys->sysParams.tStep)
						{
						sys->sysParams.tStep = t;
						}*/

						// Update particle velocity with reflected, assuming no loss in energy in terms of heat
						currParticle->params.velocity = glm::reflect(currParticle->params.velocity, direction);
						currParticle->nextPosition += pS->sysParams.tStep * currParticle->params.velocity;
						return;
					}
				}
			}
			else {
				currParticle->collisionNormal = glm::vec3(0, 0, 0);
			}
		}
	}
}

void SPH::calcSPH()
{
	ParticleSystem* sys = ParticleSystem::getInstance();
	
	int threadCount = sys->particles.size() / THREADSIZE + 1;
	std::thread* threads = new thread[threadCount];
	
	for (int i = 0, count = 0, step = 0; step < threadCount; i += THREADSIZE, ++step)
	{
		threads[step] = std::thread(neighbors, sys, i, &count);
		//if debug draw lines
	}

	for (int i = 0; i < threadCount; ++i)
	{
		threads[i].join();
	}


	for (int i = 0, count = 0, step = 0; step < threadCount; i += THREADSIZE, ++step)
	{
		threads[step] = std::thread(densitiesPressures, sys, i, &count);
	}

	for (int i = 0; i < threadCount; ++i)
	{
		threads[i].join();
	}


	for (int i = 0, count = 0, step = 0; step < threadCount; i += THREADSIZE, ++step)
	{
		threads[step] = std::thread(eulerTimeIntegrations, sys, i, &count);
	}

	for (int i = 0; i < threadCount; ++i)
	{
		threads[i].join();
	}


	for (int i = 0, count = 0, step = 0; step < threadCount; i += THREADSIZE, ++step)
	{
		threads[step] = std::thread(collisions, sys, i, &count);
	}

	for (int i = 0; i < threadCount; ++i)
	{
		threads[i].join();
	}

	delete[] threads;

	// update list of particles
	sys->updateList();
}

float SPH::calcDensity(Particle particle)
{
	ParticleSystem* sys = ParticleSystem::getInstance();
	float density = 0.0f;

	for (int i = 0; i < particle.neighbors.size(); ++i)
	{
		Particle* currParticle = sys->particles[particle.neighbors.at(i)];
		vec3 distance = particle.position - currParticle->position;
		density += currParticle->params.mass * calcDensityKernel(distance, sys->sysParams.searchRadius);
	}

	return density;
}

float SPH::calcPressure(const Particle& particle)
{
	ParticleSystem* sys = ParticleSystem::getInstance();
	return sys->sysParams.stiffness * (particle.params.density - sys->sysParams.restDensity);
}

// Derived Kernel functions were found in this paper
// http://image.diku.dk/projects/media/kelager.06.pdf
// For reference go to page 16

// Using poly6 Kernel
float SPH::calcDensityKernel(vec3 distance, float h)
{
	float magnitude = glm::length(distance);
	if (magnitude > h)
	{
		return 0.0f;
	}
	
	float first = 315.0f / (64.0f * glm::pi<float>() * pow(h, 9));
	float second = pow((h * h) - (magnitude * magnitude), 3);

	return first * second;
}

// Using Spiky Kernel for calculating pressure
// Calculates the vector field
vec3 SPH::calcGradientPressureKernel(vec3 distance, float h)
{	
	float magnitude = glm::length(distance);

	if (magnitude > h) {
		return vec3(0,0,0);
	}

	float coefficient = -45.0f / (glm::pi<float>()*pow(h, 6));
	vec3 derivedFirstValue = distance / magnitude;
	float derivedSecondValue = pow((h - magnitude), 2);	

	return coefficient * derivedFirstValue * derivedSecondValue;
}

// Using viscosity Kernel for calculating viscosity
// Calculates the divergence of the vector field
float SPH::calcLaplacianViscosityKernel(vec3 distance, float h)
{
	float magnitude = glm::length(distance);
	if (magnitude > h) {
		return 0.0f;
	}

	float coefficient = -45.0f / (glm::pi<float>()*pow(h, 6));
	float derivedFirstValue = ((h - magnitude));

	return coefficient * derivedFirstValue;
}

vec3 SPH::calcGradientPressure(Particle particle)
{
	ParticleSystem* sys = ParticleSystem::getInstance();
	vec3 ret;
	for (int i = 0; i < particle.neighbors.size(); ++i)
	{
		int index = particle.neighbors[i];
		vec3 distance = particle.position - sys->particles[index]->position;

		float h = sys->sysParams.searchRadius;

		// forces between two particles should be equal & opposite
		// pressure has to be symmetric, to guarantee symmetry we take the average of the two pressures
		float symmetricPressure = (sys->particles[index]->params.pressure + particle.params.pressure) / 2.0f;

		ret += (sys->particles[index]->params.mass / sys->particles[index]->params.density) * symmetricPressure * (calcGradientPressureKernel(distance, h));
	}

	return (-1.0f)*ret; // return the negated vector
}

vec3 SPH::calcLaplacianVelocity(Particle particle)
{
	ParticleSystem* sys = ParticleSystem::getInstance();
	vec3 ret;
	for (int i = 0; i < particle.neighbors.size(); ++i)
	{
		int index = particle.neighbors[i];
		vec3 distance = particle.position - sys->particles[index]->position;
		float h = sys->sysParams.searchRadius;

		// forces between two particles should be equal & opposite, symmetrize the velocity fields
		vec3 symmetricVelocity = sys->particles[index]->params.velocity - particle.params.velocity;
		
		ret += (sys->particles[index]->params.mass / sys->particles[index]->params.density) * symmetricVelocity * (calcLaplacianViscosityKernel(distance, h));
	}

	return ret;
}

vec3 SPH::calcAcceleration(const Particle& particle)
{
	ParticleSystem* sys = ParticleSystem::getInstance();
	auto gP = calcGradientPressure(particle);
	auto lV = sys->sysParams.viscocity*calcLaplacianVelocity(particle);
	auto g = particle.params.density * vec3(0.0f, sys->sysParams.gravity, 0.0f);

	return (gP + lV + g) / particle.params.density;
}


