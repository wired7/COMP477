#pragma once
#include "SPH.h"
#include "ParticleSystem.h"
#include "Shape.h"
#include <math.h>
#include <chrono>
#include <thread>
#include <omp.h>

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
				vec3 origin = currParticle->nextPosition;
				vec3 direction = normal;
				Triangle triangle(point1, point2, point3);

				float distanceToPlaneAtCollision = plane.intersection(origin, direction);

				direction = -sign(distanceToPlaneAtCollision) * direction;
				
				if (currParticle->collisionNormal != direction)
				{
					if (triangle.intersects(origin, direction))
					{
						currParticle->collisionNormal = direction;
						//theres a collision. Update velocity based on conservation of momentum.
						//						float t = distanceToPlaneAtCollision / length(currParticle->params.velocity);
						/*						if (t < sys->sysParams.tStep)
						{
						sys->sysParams.tStep = t;
						}*/

						// Update particle velocity with reflected, assuming some loss in energy in terms of heat
						// take particle back to position where it would have first collided with the plane given the current velocity
						vec3 velDir = normalize(currParticle->params.velocity);
						float d1 = plane.intersection(origin, velDir);
						float backwardsDisplacement = (pS->sysParams.particleRadius * d1 / distanceToPlaneAtCollision) - d1; // using law of sines
						currParticle->params.velocity = 0.8f * glm::reflect(currParticle->params.velocity, direction);
						currParticle->nextPosition -= velDir * backwardsDisplacement;

//						currParticle->nextPosition += (pS->sysParams.particleRadius - abs(distanceToPlaneAtCollision)) * direction;

						// time retrocession = backwardsDisplacement / currParticle->params.velocity. So tStep -= tRetrocession.
						// But this shouldn't be done here since we must obtain the minimum time step produced by these calculations beforehand
						// we may never have to do this. collisions look pretty good by now
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
	
	// Get the neighbors of each particle and set up its attributes for pressure and density calculation
	#pragma omp parallel for schedule(dynamic, 2)
	for (int i = 0; i < sys->particles.size(); ++i)
	{
		sys->calcNeighbors(sys->particles[i]);
		sys->particles[i]->params.density = 0;
		sys->particles[i]->params.pressure = 0;
		sys->particles[i]->params.gradientPressure = vec3(0, 0, 0);
		sys->particles[i]->params.laplacianVelocity = vec3(0, 0, 0);
	}	

	// Calculate the density of each particle
	#pragma omp parallel for schedule(dynamic, 2)	
	for (int i = 0; i < sys->particles.size(); ++i)
	{
		sys->particles[i]->params.density += SPH::calcDensity(*sys->particles[i]);
	}

	// Calculate the pressure of each particle
	#pragma omp parallel for schedule(dynamic, 2)	
	for (int i = 0; i < sys->particles.size(); ++i)
	{
		sys->particles[i]->params.pressure += SPH::calcPressure(*sys->particles[i]);
	}

	// Using the pressure and the density, calculate the acceleration of the particle
	#pragma omp parallel for schedule(dynamic, 2)	
	for (int i = 0; i < sys->particles.size(); ++i)
	{
		sys->particles[i]->params.acceleration = SPH::calcAcceleration(*sys->particles[i]);
	}

	// Using the acceleration, use explicit euler integration to find the next position of the particle
	#pragma omp parallel for schedule(dynamic, 2)	
	for (int i = 0; i < sys->particles.size(); ++i)
	{
		sys->particles[i]->params.velocity += sys->sysParams.tStep * sys->particles[i]->params.acceleration;
		sys->particles[i]->nextPosition = sys->particles[i]->position;
		sys->particles[i]->nextPosition += sys->sysParams.tStep * sys->particles[i]->params.velocity;
	}

	// Find external forces (collisions with rigidbodies) and apply them to the particle
	#pragma omp parallel for schedule(dynamic, 2)
	for (int i = 0; i < sys->particles.size(); ++i)
	{
		collisionsSubFunction(sys, i);
	}
	
	// update list of particles
	sys->updateList();
}

float SPH::calcDensity(Particle particle)
{
	ParticleSystem* sys = ParticleSystem::getInstance();

	float density = sys->sysParams.mass * calcDensityKernel(vec3(0.0f, 0.0f, 0.0f), sys->sysParams.searchRadius);

	for (int i = 0; i < particle.neighbors.size(); ++i)
	{
		int index = particle.neighbors[i];

		if (index > particle.getIndex())
		{
			Particle* currParticle = sys->particles[particle.neighbors.at(i)];
			vec3 distance = particle.position - currParticle->position;
			float kernel = calcDensityKernel(distance, sys->sysParams.searchRadius);
			density += sys->sysParams.mass * kernel;
			sys->particles[index]->params.density += sys->sysParams.mass * kernel;
		}
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

	float coefficient = 45.0f / (glm::pi<float>()*pow(h, 6));
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

		if (index > particle.getIndex())
		{
			vec3 distance = particle.position - sys->particles[index]->position;

			float h = sys->sysParams.searchRadius;

			// forces between two particles should be equal & opposite
			// pressure has to be symmetric, to guarantee symmetry we take the average of the two pressures
			float symmetricPressure = (sys->particles[index]->params.pressure + particle.params.pressure) / 2.0f;

			vec3 kernel = calcGradientPressureKernel(distance, h);

			ret -= (sys->sysParams.mass / sys->particles[index]->params.density) * symmetricPressure * kernel;

			sys->particles[index]->params.gradientPressure += (sys->sysParams.mass / particle.params.density) * symmetricPressure * kernel;
		}
	}

	return ret; // return the negated vector
}

vec3 SPH::calcLaplacianVelocity(Particle particle)
{
	ParticleSystem* sys = ParticleSystem::getInstance();
	vec3 ret;
	for (int i = 0; i < particle.neighbors.size(); ++i)
	{
		int index = particle.neighbors[i];

		if (index > particle.getIndex())
		{
			vec3 distance = particle.position - sys->particles[index]->position;
			float h = sys->sysParams.searchRadius;

			// forces between two particles should be equal & opposite, symmetrize the velocity fields
			vec3 symmetricVelocity = sys->particles[index]->params.velocity - particle.params.velocity;

			float kernel = calcLaplacianViscosityKernel(distance, h);

			ret += (sys->sysParams.mass / sys->particles[index]->params.density) * symmetricVelocity * kernel;

			sys->particles[index]->params.laplacianVelocity -= (sys->sysParams.mass / particle.params.density) * symmetricVelocity * kernel;
		}
	}

	return ret;
}

vec3 SPH::calcAcceleration(Particle particle)
{
	ParticleSystem* sys = ParticleSystem::getInstance();
	particle.params.gradientPressure += calcGradientPressure(particle);
	particle.params.laplacianVelocity += calcLaplacianVelocity(particle);

	auto g = particle.params.density * vec3(0.0f, sys->sysParams.gravity, 0.0f);

	return (particle.params.gradientPressure + sys->sysParams.viscocity * particle.params.laplacianVelocity + g) / particle.params.density;
}


