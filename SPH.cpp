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
		float distance = glm::length(particle.position - currParticle->position);
		density += currParticle->params.mass * calcKernel(distance, sys->sysParams.searchRadius);
	}

	return density;
}

float SPH::calcPressure(const Particle& particle)
{
	ParticleSystem* sys = ParticleSystem::getInstance();
	return sys->sysParams.stiffness * (particle.params.density - sys->sysParams.restDensity);
}

float SPH::calcKernel(float distance, float h)
{
	//TODO: ASK TEACHER ABOUT THIS CONDITION
	if (distance < 0 || distance > h)
	{
		return 0.0f;
	}

	float first = 15.0 / ((2 * glm::pi<float>())*(pow(h, 3)));
	float second = pow(-distance, 3) / (2 * (pow(h, 3)));
	float third = pow(distance, 2) / pow(h, 2);
	float fourth = h / (2 * distance);
	float fifth = -1.0;

	return first * (second + third + fourth + fifth);
}

vec3 SPH::calcGradientW(vec3 distance, float h)
{
	float x = 0.0f;
	float y = 0.0f;
	float z = 0.0f;

	//TODO: ASK TEACHER ABOUT THIS CONDITION
	if (distance.x > 0 && distance.x < h)
	{
		x = pow((h*h) - (distance.x*distance.x), 2);
	}

	if (distance.y > 0 && distance.y < h)
	{
		y = pow((h*h) - (distance.y*distance.y), 2);
	}

	if (distance.z > 0 && distance.z < h)
	{
		z = pow((h*h) - (distance.z*distance.z), 2);
	}
	
	vec3 temp(x, y, z);

	return (float)(1 / ((32.0f*glm::pi<float>()*pow(h, 9)))) * ((315.0f * 3.0f * distance) * temp);
}

vec3 SPH::calcLaplacianW(vec3 distance, float h)
{
	float x = 0.0f;
	float y = 0.0f;
	float z = 0.0f;

	if (distance.x > 0 && distance.x < h)
	{
		x = (h*h) - (distance.x*distance.x);
	}

	if (distance.y > 0 && distance.y < h)
	{
		y = (h*h) - (distance.y*distance.y);
	}

	if (distance.z > 0 && distance.z < h)
	{
		z = (h*h) - (distance.z*distance.z);
	}
	
	vec3 temp(x, y, z);

	return (float)(1 / (8.0f*glm::pi<float>()*pow(h, 9))) * ((315.0f * 3.0f * distance * distance)  * (temp));
}

vec3 SPH::calcGradient(Particle particle)
{
	ParticleSystem* sys = ParticleSystem::getInstance();
	vec3 ret;
	for (int i = 0; i < particle.neighbors.size(); ++i)
	{
		int index = particle.neighbors[i];
		vec3 distance = particle.position - sys->particles[index]->position; //glm::length(particle.position - sys->particles[index]->position);
		float h = sys->sysParams.searchRadius;
		ret += (float)((sys->particles[index]->params.mass / sys->particles[index]->params.density) * (sys->particles[index]->params.pressure)) * (calcGradientW(distance, h));
	}

	return ret;
}

vec3 SPH::calcLaplacian(Particle particle)
{
	ParticleSystem* sys = ParticleSystem::getInstance();
	vec3 ret;
	for (int i = 0; i < particle.neighbors.size(); ++i)
	{
		int index = particle.neighbors[i];
		vec3 distance = particle.position - sys->particles[index]->position; //glm::length(particle.position - sys->particles[index]->position);
		float h = sys->sysParams.searchRadius;
		ret += (sys->particles[index]->params.mass / sys->particles[index]->params.density) * (sys->particles[index]->params.pressure) * (calcLaplacianW(distance, h));
	}

	return ret;
}

vec3 SPH::calcAcceleration(const Particle& particle)
{
	ParticleSystem* sys = ParticleSystem::getInstance();
	return ((-1.0f)*calcGradient(particle) + sys->sysParams.viscocity*calcLaplacian(particle) + particle.params.density * sys->sysParams.gravity) / particle.params.density;
}


