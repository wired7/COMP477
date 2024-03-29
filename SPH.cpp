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

	vector<GridCube> vec;
	
	vec = pS->grid.getNeighborCubes(*currParticle);

//	vec3 initialGridCoord(currParticle->getGridCellCoord().x, currParticle->getGridCellCoord().y, currParticle->getGridCellCoord().z);

//	pS->grid.getNeighborCubes(initialGridCoord, initialGridCoord, normalize(currParticle->params.velocity), 3.1415f / 3.0f, &vec, 5);



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
			Triangle triangle(point1, point2, point3);
			vec3 origin = currParticle->nextPosition;

			vec3 direction = -sign(dot(currParticle->params.velocity, normal)) * normal;
			
			if (currParticle->collisionNormal != direction)
			{
				// get the negative velocity
				vec3 velDir = normalize(currParticle->params.velocity);

				if (distance < pS->sysParams.particleRadius)
				{
					if (triangle.intersects(origin, direction) && abs(plane.intersects(origin, velDir)))
					{
						// Update particle velocity with reflected, assuming some loss in energy in terms of heat
						// take particle back to position where it would have first collided with the plane given the current velocity
						
						float d1 = plane.intersection(origin, velDir);
						float backwardsDisplacement;

						// displace the particle right before it collides with the surface
						if (distance == 0.0f)
						{
							//sin(theta) = radius / backwardsDisplacement -> bD = radius / sin(theta)
							// theta = angle between velocity vector and plane -> dot(v, normal) = cos(phi) -> phi = arccos(dot(v, normal))
							// theta = 90 - phi -> bD = radius / sin(90 - arccos(dot(v, normal)))
							backwardsDisplacement = pS->sysParams.particleRadius / abs(dot(velDir, direction));
						}
						else if (d1 > 0)
						{
							backwardsDisplacement = d1 * pS->sysParams.particleRadius / distance - d1 + 0.000005f; // using law of sines
						}
						else 
						{
							d1 = abs(d1);
							backwardsDisplacement = d1 * (pS->sysParams.particleRadius) / distance - d1 + 0.000005f; // using law of sines
						}

						float dT = backwardsDisplacement / length(currParticle->params.velocity);

						currParticle->params.velocity = 0.85f * glm::reflect(currParticle->params.velocity, direction);
						currParticle->nextPosition -= (velDir * abs(backwardsDisplacement)); //currParticle->position + (currParticle->nextPosition - currParticle->position) * dT / pS->sysParams.maxTStep;

/*						if (pS->sysParams.maxTStep - dT < pS->sysParams.tStep)
						{
							pS->sysParams.tStep = pS->sysParams.maxTStep - dT;
						}

						if (dT > currParticle->deltaTime)
						{
							currParticle->deltaTime = dT;
							currParticle->collisionNormal = direction;
						}*/

						// time retrocession = backwardsDisplacement / currParticle->params.velocity. So tStep -= tRetrocession.
						// But this shouldn't be done here since we must obtain the minimum time step produced by these calculations beforehand
						// we may never have to do this. collisions look pretty good by now
						return;
					}
				}
				/* Never gets into this condition
				else
				{					
					vec3 difference = currParticle->nextPosition - currParticle->position;
					float d1 = plane.intersection(currParticle->position, normalize(difference));

					if (abs(d1) < length(difference) && d1 > 0)
					{
						cout << "i got in" << endl;
//						vec3 velDir = normalize(currParticle->params.velocity);
						float backwardsDisplacement = d1 * (pS->sysParams.particleRadius) / distance + 0.0001f; // using law of sines

//						float dT = pS->sysParams.maxTStep - backwardsDisplacement / length(currParticle->params.velocity);

						currParticle->params.velocity = 0.75f * glm::reflect(currParticle->params.velocity, direction);
						currParticle->nextPosition -= velDir * abs(backwardsDisplacement);//currParticle->position + (currParticle->nextPosition - currParticle->position) * dT / pS->sysParams.maxTStep;

						float dT = backwardsDisplacement / length(currParticle->params.velocity);

						if (pS->sysParams.maxTStep - dT < pS->sysParams.tStep)
						{
							pS->sysParams.tStep = pS->sysParams.maxTStep - dT;
						}

						if (dT > currParticle->deltaTime)
						{
							currParticle->deltaTime = dT;
							currParticle->collisionNormal = direction;
						}
						return;
					}
				}
				*/
			}		
		}
	}

	currParticle->collisionNormal = glm::vec3(0, 0, 0);
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
		sys->particles[i]->params.gradientSmoothColor = vec3(0, 0, 0);
		sys->particles[i]->params.laplacianSmoothColor = vec3(0, 0, 0);
		sys->particles[i]->params.tensionForce = vec3(0, 0, 0);
		sys->particles[i]->deltaTime = 0;
	}

	// Calculate the density of each particle
	#pragma omp parallel for schedule(dynamic, 2)	
	for (int i = 0; i < sys->particles.size(); ++i)
	{
		// This is done because it possible to reach a critical state in the following condition:

		// 1st Thread - The first particle is in the calcDensity function and has reached the fourth particle as its neighbor
		//            - It is about to modify the fourth particle's contents
		// 2nd Thread - The fourth particle has finished the calcDensity and it is about to add the density from the resulting calcDensity function

		// 1st Thread reaches the critical section in the calcDensity function and modifies the contents
		// 2nd Thread gets his turn and modifies the old density value and the 1st thread's modification is lost

		// Result: One of the threads modification will be overridden

		float holdDensityForCriticalSection = SPH::calcDensity(*sys->particles[i]);
		#pragma omp critical
		{
			sys->particles[i]->params.density += holdDensityForCriticalSection;
		}
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

	sys->sysParams.tStep = sys->sysParams.maxTStep;
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

	float density = sys->sysParams.mass * calcKernel(vec3(0.0f, 0.0f, 0.0f), sys->sysParams.searchRadius);
	
	for (int i = 0; i < particle.neighbors.size(); ++i)
	{
		int index = particle.neighbors[i];

		if (index > particle.getIndex())
		{
			Particle* currParticle = sys->particles[particle.neighbors.at(i)];
			vec3 distance = particle.position - currParticle->position;
			float kernel = calcKernel(distance, sys->sysParams.searchRadius);

			// particle - particle, both are being influenced by each other
			// update density of both particles
			density += sys->sysParams.mass * kernel;

			#pragma omp critical
			{
				sys->particles[index]->params.density += sys->sysParams.mass * kernel;
			}
		}
	}

	return density;
}

/* Ideal Gas Equation
float SPH::calcPressure(const Particle& particle)
{
ParticleSystem* sys = ParticleSystem::getInstance();
return sys->sysParams.stiffness * (particle.params.density - sys->sysParams.restDensity);
}
*/

float SPH::calcPressure(const Particle& particle)
{
	ParticleSystem* sys = ParticleSystem::getInstance();
	// using Tait's equation which is an equation of state --> describes the state of mmatter under a given set of physical conditions
	return sys->sysParams.stiffness * (pow(particle.params.density / sys->sysParams.restDensity, sys->sysParams.pressureGamma) - 1);
}

// Derived Kernel functions were found in this paper
// http://image.diku.dk/projects/media/kelager.06.pdf
// For reference go to page 16

// Using poly6 Kernel
float SPH::calcKernel(vec3 distance, float h)
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
vec3 SPH::calcGradientKernel(vec3 distance, float h)
{
	float magnitude = glm::length(distance);

	if (magnitude > h) {
		return vec3(0, 0, 0);
	}

	float coefficient = -45.0f / (glm::pi<float>()*pow(h, 6));
	vec3 derivedFirstValue = magnitude > 0.0f ? distance / magnitude : vec3(0.0f, 0.0f, 0.0f);
	float derivedSecondValue = pow((h - magnitude), 2);

	return coefficient * derivedFirstValue * derivedSecondValue;
}

// Using viscosity Kernel for calculating viscosity
// Calculates the divergence of the vector field
float SPH::calcLaplacianKernel(vec3 distance, float h)
{
	float magnitude = glm::length(distance);
	if (magnitude > h) {
		return 0.0f;
	}

	float coefficient = 45.0f / (glm::pi<float>()*pow(h, 6));
	float derivedFirstValue = h - magnitude;

	return coefficient * derivedFirstValue;
}

// calculate the pressure gradient to determine the forces
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

			vec3 kernel = calcGradientKernel(distance, h);

			ret -= (sys->sysParams.mass / sys->particles[index]->params.density) * symmetricPressure * kernel;

			#pragma omp critical
			{
				sys->particles[index]->params.gradientPressure += (sys->sysParams.mass / particle.params.density) * symmetricPressure * kernel;
			}
		}
	}

	return ret; // return the negated vector
}

// used for surface tension
vec3 SPH::calcGradientColor(Particle particle)
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

			vec3 kernel = calcGradientKernel(distance, h);

			// color field is always 1 on particle and 0 otherwise, hence we only need to do (p/d) * kernel
			ret += (sys->sysParams.mass / sys->particles[index]->params.density) * kernel;

			#pragma omp critical
			{
				sys->particles[index]->params.gradientSmoothColor += (sys->sysParams.mass / particle.params.density) * kernel;
			}
		}
	}

	return ret; // return the negated vector
}

// used for surface tension calculation
vec3 SPH::calcLaplacianColor(Particle particle) {
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
			float kernel = calcLaplacianKernel(distance, h);

			ret += (sys->sysParams.mass / sys->particles[index]->params.density) * kernel;

			#pragma omp critical
			{
				sys->particles[index]->params.laplacianSmoothColor += (sys->sysParams.mass / particle.params.density) * kernel;
			}
		}
	}

	return ret;
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

			float kernel = calcLaplacianKernel(distance, h);

			ret += (sys->sysParams.mass / sys->particles[index]->params.density) * symmetricVelocity * kernel;

			#pragma omp critical
			{
				sys->particles[index]->params.laplacianVelocity -= (sys->sysParams.mass / particle.params.density) * symmetricVelocity * kernel;
			}
		}
	}

	return ret;
}

// calculating the acceleration of the particle in order to determine the velocity
vec3 SPH::calcAcceleration(Particle particle)
{
	ParticleSystem* sys = ParticleSystem::getInstance();

	vec3 holdVec3ForCritical = calcGradientPressure(particle);

	#pragma omp critical
	{
		particle.params.gradientPressure += holdVec3ForCritical;
	}

	holdVec3ForCritical = calcLaplacianVelocity(particle);

	#pragma omp critical
	{
		particle.params.laplacianVelocity += holdVec3ForCritical;
	}

	// surface tension
	// http://citeseerx.ist.psu.edu/viewdoc/download?doi=10.1.1.2.7720&rep=rep1&type=pdf

	holdVec3ForCritical = calcGradientColor(particle);

	#pragma omp critical
	{
		particle.params.gradientSmoothColor += holdVec3ForCritical;
	}

	holdVec3ForCritical = calcLaplacianColor(particle);

	#pragma omp critical
	{
		particle.params.laplacianSmoothColor += holdVec3ForCritical;
	}

	glm::vec3 surfaceForce = glm::vec3(0, 0, 0);

	// needed this condition in order to avoid division by 0
	if (glm::length(particle.params.gradientSmoothColor) != 0) {
		surfaceForce = (-1.0f) * particle.params.laplacianSmoothColor * sys->sysParams.tensionCoefficient * (particle.params.gradientSmoothColor / glm::length(particle.params.gradientSmoothColor));
	}

	auto g = particle.params.density * vec3(0.0f, sys->sysParams.gravity, 0.0f);

	return ((particle.params.gradientPressure + sys->sysParams.viscocity * particle.params.laplacianVelocity + g + surfaceForce) / particle.params.density);
}