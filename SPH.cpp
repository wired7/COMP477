#pragma once
#include "SPH.h"
#include "ParticleSystem.h"
#include "Shape.h"
#include <math.h>

void SPH::calcSPH()
{
	ParticleSystem* sys = ParticleSystem::getInstance();

	// UPDATE GRID-RIGIDBODY INTERSECTIONS. ONLY DO THIS HERE IF YOU WANT RIGIDBODY PHYSICS. DO ONCE OUTSIDE OTHERWISE. O(GRIDSIZE * MESH_TRIANGLE_COUNT * 12)
/*	auto vec = sys->grid.data;
	for (int j = 0; j < vec.size(); ++j)
	{

	}*/

	for (int i = 0; i < sys->particles.size(); ++i)
	{
		sys->calcNeighbors(sys->particles[i]);
		//if debug draw lines
	}

	for (int i = 0; i < sys->particles.size(); ++i)
	{
		sys->particles[i]->params.density = calcDensity(*sys->particles[i]);
		sys->particles[i]->params.pressure = calcPressure(*sys->particles[i]);
	}

	for (int i = 0; i < sys->particles.size(); ++i)
	{
		//calc right side equation
		vec3 acceleration = calcAcceleration(*sys->particles[i]);
		// float h; //TODO: =;
		sys->particles[i]->params.velocity += sys->sysParams.tStep * acceleration;
		sys->particles[i]->nextPosition = sys->particles[i]->position;
		sys->particles[i]->nextPosition += sys->sysParams.tStep * sys->particles[i]->params.velocity;
	}

	for (int i = 0; i < sys->particles.size(); ++i)
	{
		Particle* currParticle = sys->particles[i];
		currParticle->position = currParticle->nextPosition;
		sys->grid.update(*currParticle);
		
		sys->sysParams.tStep = sys->sysParams.maxTStep;
		vector<GridCube> vec = sys->grid.getNeighborCubes(*currParticle);
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

				//This should later become a triangle
				Plane plane(point1, point2, point3);
				float distance = -dot(plane.normal, (currParticle->position - plane.point)) / pow(length(plane.normal), 2);

				if (distance <= sys->sysParams.particleRadius)
				{
					if (!(currParticle->collisionNormal == plane.normal))
					{
						currParticle->collisionNormal = plane.normal;
						float distanceToPlaneAtCollision = plane.intersection(currParticle->position, -normalize(plane.normal)) - sys->sysParams.particleRadius;
						//theres a collision. Update velocity based on conservation of momentum.
						float t = distanceToPlaneAtCollision / length(currParticle->params.velocity);
						if (t < sys->sysParams.tStep)
						{
							sys->sysParams.tStep = t;
						}
						
						// Update particle velocity with reflected, assuming no loss in energy in terms of heat
						currParticle->params.velocity = glm::reflect(currParticle->params.velocity, plane.normal);
					}
				}				
				else {
					currParticle->collisionNormal = vec3(0,0,0);
				}
			}
		}
	}
}

float SPH::calcDensity(Particle particle)
{
	float density = 0.0;
	ParticleSystem* sys = ParticleSystem::getInstance();
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
		float symmetricPressure = (sys->particles[index]->params.pressure + particle.params.pressure) / 2;

		ret += (sys->particles[index]->params.mass / sys->particles[index]->params.density) * symmetricPressure * (calcGradientPressureKernel(distance, h));
	}

	return -ret; // return the negated vector
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
	return (calcGradientPressure(particle) + sys->sysParams.viscocity*calcLaplacianVelocity(particle) + particle.params.density * sys->sysParams.gravity) / particle.params.density;
}


