#pragma once
#include "SPH.h"
#include "ParticleSystem.h"
#include "Shape.h"
#include <math.h>

void SPH::calcSPH()
{
	ParticleSystem* sys = ParticleSystem::getInstance();
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
						
						// TODO: Reflection
						



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
		density += currParticle->params.mass * calcKernel(distance, sys->sysParams.searchRadius);
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

// Using poly5 Kernel
float SPH::calcKernel(vec3 distance, float h)
{
	float magnitude = glm::length(distance);
	if (magnitude > h)
	{
		return 0.0f;
	}

	float first = 15.0 / ((2 * glm::pi<float>())*(pow(h, 3)));
	float second = pow(-magnitude, 3) / (2 * (pow(h, 3)));
	float third = pow(magnitude, 2) / pow(h, 2);
	float fourth = h / (2 * magnitude);
	float fifth = -1.0;

	return first * (second + third + fourth + fifth);
}

// Calculates the vector field
vec3 SPH::calcGradientW(vec3 distance, float h)
{	
	float magnitude = glm::length(distance);

	float coefficient = -945.0f / (32.0f*glm::pi<float>()*pow(h, 9));
	float derivedValue = pow((h*h) - (magnitude*magnitude), 2);	

	return coefficient * distance * derivedValue;
}

// Calculates the divergence of the vector field
float SPH::calcLaplacianW(vec3 distance, float h)
{
	float magnitude = glm::length(distance);

	float coefficient = -945.0f / (32.0f*glm::pi<float>()*pow(h, 9));
	float derivedFirstValue = ((h*h) - (magnitude*magnitude));
	float derivedSecondValue = (3 * (h*h) - 7 * (magnitude*magnitude));

	return coefficient * derivedFirstValue * derivedSecondValue;
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
		ret += (sys->particles[index]->params.mass / sys->particles[index]->params.density) * ((sys->particles[index]->params.pressure + particle.params.pressure) / 2.0f) * (calcGradientW(distance, h));
	}

	return -ret; // return the negated vector --> -pressureGradient
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

		// forces between two particles should be equal & opposite
		// viscocity force is non-symmetric, finding the difference in velocities 
		ret += (sys->particles[index]->params.mass / sys->particles[index]->params.density) * (sys->particles[index]->params.velocity - particle.params.velocity) * (calcLaplacianW(distance, h));
	}

	return ret;
}

vec3 SPH::calcAcceleration(const Particle& particle)
{
	ParticleSystem* sys = ParticleSystem::getInstance();
	return (calcGradientPressure(particle) + sys->sysParams.viscocity*calcLaplacianVelocity(particle) + particle.params.density * sys->sysParams.gravity) / particle.params.density;
}


