#pragma once
#include "ParticleSystem.h"
#include "SPH.h"
#include "glm.hpp"
#include <thread>
#include <chrono>
#include <omp.h>
#include <fstream>
#include "FileStorage.h"
#include "Utils.h"

using namespace std::chrono;

ParticleSystem* ParticleSystem::particleSystem;
bool ParticleSystem::SerializeData = false;

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

void ParticleSystem::watchCIN()
{
	char val = ' ';
	while (val != 'x')
	{
		cin >> val;
	}
	ParticleSystem::SerializeData = true;
}

void ParticleSystem::startSimulation(float playbackTime, float frameRate, string filePath, bool continueFlag)
{
	std::thread t1(&ParticleSystem::watchCIN, this);
	t1.detach();

	milliseconds ms;

	float t = 0;

	ofstream myfile;

	if (continueFlag)
	{
		myfile.open(filePath, std::ios_base::app);
	}
	else
	{
		myfile.open(filePath);
		
		vector<string> vec = Utils::split(filePath, '\\');
		string newFile = "Animations/Serialize/s_" + vec.at(vec.size() - 1);

		myfile << "Serialized:" << newFile << endl;
	}

	if (!continueFlag)
	{
		for (int i = 0; i < rigidbodies.size(); i++)
		{
			myfile << "Rigidbody:" << endl;

			for (int j = 0; j < rigidbodies[i]->vertices.size(); j++)
			{
				for (int k = 0; k < 3; k++)
					myfile << rigidbodies[i]->vertices[j].position[k] << " ";
				for (int k = 0; k < 3; k++)
					myfile << rigidbodies[i]->vertices[j].normal[k] << " ";
				for (int k = 0; k < 4; k++)
					myfile << rigidbodies[i]->vertices[j].color[k] << " ";
			}

			myfile << endl;

			for (int j = 0; j < rigidbodies[i]->indices.size(); j++)
			{
				myfile << rigidbodies[i]->indices[j] << " ";
			}

			myfile << endl;
		}

		myfile << "Particles:" << endl;

		myfile << sysParams.volume << " " << sysParams.particleRadius << " " << sysParams.searchRadius << " " << sysParams.viscocity << " " << sysParams.stiffness << " ";
		myfile << sysParams.restDensity << " " << sysParams.gravity << " " << sysParams.tStep << " " << sysParams.maxTStep << endl;
		myfile.close();
	}
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

			if (ParticleSystem::SerializeData)
			{
				FileStorage::serializeData(filePath);
				cout << "Data serialized. Press any key to exit ..." << endl;
				int temp;
				cin >> temp;
				std::exit(0);
			}
		}

		SPH::calcSPH();

		t += sysParams.tStep;
	}
}
// add particles to the system
void ParticleSystem::addParticles(vector<Particle*> part)
{
	particles = part;

	for (int i = 0; i < particles.size(); i++)
	{
		particles[i]->index = i;
		particles[i]->gridCellCoord = particles[i]->position / grid.getCellSize();

		int gridX = particles[i]->gridCellCoord.x;
		int gridY = particles[i]->gridCellCoord.y;
		int gridZ = particles[i]->gridCellCoord.z;

		bool offGrid = (gridX >= grid.dim.x || gridY >= grid.dim.y || gridZ >= grid.dim.z || gridX < 0 || gridY < 0 || gridZ < 0);

		if (!offGrid)
		{
			grid.data[(int)particles[i]->gridCellCoord.x][(int)particles[i]->gridCellCoord.y][(int)particles[i]->gridCellCoord.z].particles.push_back(i);
		}
	}
}
// add rigidbodies for collision interactions in the system
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
}
// update the position of each particle
void ParticleSystem::updateList() {
	float ratio = sysParams.tStep / sysParams.maxTStep;
	for (int i = 0; i < particles.size(); ++i) {
		particles[i]->position = particles[i]->nextPosition;
/*		particles[i]->position = particles[i]->position + (particles[i]->nextPosition - particles[i]->position) * ratio;
		particles[i]->nextPosition = particles[i]->position;

		// Collision Handling
		if (particles[i]->deltaTime > 0 && sysParams.maxTStep - particles[i]->deltaTime == sysParams.tStep)
		{
			vec3 normal = normalize(particles[i]->collisionNormal);
			vec3 direction = -sign(dot(particles[i]->params.velocity, normal)) * normal;
//			cout << particles[i]->params.velocity.x << " " << particles[i]->params.velocity.y << " " << particles[i]->params.velocity.z << endl;
			particles[i]->params.velocity = 0.8f * glm::reflect(particles[i]->params.velocity, direction);
			particles[i]->collisionNormal = vec3(0, 0, 0);
//			cout << particles[i]->params.velocity.x << " " << particles[i]->params.velocity.y << " " << particles[i]->params.velocity.z << endl;
//			system("PAUSE");
		}*/

		grid.update(*particles[i]);
	}	
}
// find all neighbors of particle
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

void ParticleSystem::setStiffnessOfParticleSystem() {

	// Getting the height of water by multiplying the block size 
	float heightOfWater = pow(particleSystem->sysParams.volume, 1.0f/3.0f);

	// Determining the max velocity by doing the square root of 2 * g * H
	float maxVelocitySquared = 2.0f * -particleSystem->sysParams.gravity * heightOfWater;

	// The ratio between the square of the velocity and the square of the speed of sound is proportional to the ratio between the variation of density between particles over the rest density
	// In the paper, they have concluded that n reaches ~0.01 which means that the ratio between the squared velocity is equal to 0.01
	float speedOfSoundSquared = maxVelocitySquared / 0.01f;

	// Stiffness can be calculated by multiplying the velocity with the restDensity then dividing it with Tait's constant, gamma
	particleSystem->sysParams.stiffness = (speedOfSoundSquared * particleSystem->sysParams.restDensity / particleSystem->sysParams.pressureGamma) / 1000; // Pa to kPa
}

void resolveMesh(NodeInfo** vertices, int size, Rigidbody* rigidbody, vec2 coordinates, float distanceCutOff);

Rigidbody* ParticleSystem::rayTrace(vector<vec3>* points, float radius, int resolution) {
	Rigidbody* rigidbody = new Rigidbody(vector<Vertex2>(), vector<GLuint>(), mat4(1.0f), 1, false);
	
	vector<Sphere*> spheres;
	vec3 max = {-INFINITY, -INFINITY, -INFINITY};
	vec3 min = {INFINITY, INFINITY, INFINITY};
	// Get bounding cube
	for (int i = 0; i < points->size(); i++)
	{
		for (int j = 0; j < 3; j++)
		{
			if (points->at(i)[j] > max[j])
				max[j] = points->at(i)[j];
			if (points->at(i)[j] < min[j])
				min[j] = points->at(i)[j];
		}

		spheres.push_back(new Sphere(points->at(i), radius));
	}

	vec3 boxCenter = (max + min) / 2.0f;

	vec3 faces[] = { min, min + vec3(max.x, 0, 0), min + vec3(max.x, max.y, 0), min + vec3(0, max.y, 0),
					min, min + vec3(0, 0, max.z), min + vec3(0, max.y, max.z), min + vec3(0, max.y, 0),
					min, min + vec3(0, 0, max.z), min + vec3(max.x, 0, max.z), min + vec3(max.x, 0, 0)};
	
	for (int side = 0; side < 1; side++)
	{
		vec3 pts[] = { faces[side * 4], faces[side * 4 + 1], faces[side * 4 + 2], faces[side * 4 + 3] };

		vec3 range = vec3(0, 0, 0);

		for (int i = 1; i < 4; i++)
		{
			range += pts[i] - pts[0];
			cout << "POINTS:" << endl;
			for (int j = 0; j < 3; j++)
			{
				cout << pts[i][j] << " ";
				if (abs(pts[i][j] - pts[0][j]) > range[j])
					range[j] = pts[i][j] - pts[0][j];
			}
			cout << endl;
		}

		range /= 4;

		vec3 midPoint = (pts[0] + range) / 2.0f;

		cout << "MIDPOINT: " << midPoint.x << " " << midPoint.y << " " << midPoint.z << endl;

		vec3 normal = midPoint - boxCenter;

		vec3 camDir = -normalize(normal);

		vec3 camOrigin = pts[0] + normal * 1.5f;

		pair<float, float> twoGreatest(-INFINITY, -INFINITY);

		for (int i = 0; i < 3; i++)
		{
			if (range[i] > twoGreatest.first)
				twoGreatest.first = range[i];
			else if (range[i] == twoGreatest.first || range[i] > twoGreatest.second)
				twoGreatest.second = range[i];
		}

		vec3 up = normalize(pts[3] - pts[0]);

		vec3 right = normalize(pts[1] - pts[0]);

		NodeInfo** vertices = new NodeInfo*[resolution];

		float stepX = twoGreatest.first / resolution;
		float stepY = twoGreatest.second / resolution;

		for (int i = 0; i < resolution; i++)
		{
			vertices[i] = new NodeInfo[resolution];
			for (int j = 0; j < resolution; j++)
			{
				vec3 o = camOrigin + right * (float)i * stepX + up * (float)j * stepY;

				float closestIntersection = INFINITY;
				Sphere* closestObject = NULL;
				for (int k = 0; k < spheres.size(); k++)
				{
					float t = spheres[k]->intersection(o, camDir);

					if (t > 0 && t < closestIntersection)
					{
						closestIntersection = t;
						closestObject = spheres[k];
					}
				}

				if (closestObject != NULL)
				{
					vertices[i][j].vertex = new Vertex2(o + camDir * closestIntersection, normalize(o + camDir * closestIntersection - closestObject->center), vec4(0.5f, 0.5, 0.8f, 1.0f), vec2());
				}
			}
		}

		for (int i = 0; i < resolution; i++)
			for (int j = 0; j < resolution; j++)
				if (vertices[i][j].vertex != NULL)
				{
					resolveMesh(vertices, resolution, rigidbody, vec2(i, j), radius);
				}
		
		for (int i = 0; i < resolution; i++)
			for (int j = 0; j < resolution; j++)
				if(vertices[i][j].vertex != NULL)
					delete vertices[i][j].vertex;
	}

	return rigidbody;
}

void resolveMesh(NodeInfo** vertices, int size, Rigidbody* rigidbody, vec2 coordinates, float distanceCutOff)
{
	if (vertices[(int)coordinates.x][(int)coordinates.y].visited)
		return;

	vertices[(int)coordinates.x][(int)coordinates.y].visited = true;

	if (vertices[(int)coordinates.x][(int)coordinates.y].index < 0)
	{
		rigidbody->vertices.push_back(*vertices[(int)coordinates.x][(int)coordinates.y].vertex);
		vertices[(int)coordinates.x][(int)coordinates.y].index = rigidbody->vertices.size() - 1;
	}

	vec2 dirs[] = { vec2(1, 0), vec2(1, 1), vec2(0, 1), vec2(-1, 1), vec2(-1, 0), vec2(-1, -1), vec2(0, -1), vec2(1, -1) };

	vector<vec2> visitNext;

	for (int i = 0; i < 8; i++)
	{
		vec2 dir = dirs[i];
		vec2 nextDir = dirs[(i + 1) % 8];

		vec2 newCoord = coordinates + dir;
		vec2 newNewCoord = coordinates + nextDir;

		if (newCoord.x > 0 && newCoord.x < size && newCoord.y > 0 && newCoord.y < size)
			if (vertices[(int)newCoord.x][(int)newCoord.y].vertex != NULL)
			{
				visitNext.push_back(newCoord);

				if (!vertices[(int)newCoord.x][(int)newCoord.y].visited)
					if (newNewCoord.x > 0 && newNewCoord.x < size && newNewCoord.y > 0 && newNewCoord.y < size)
						if (vertices[(int)newNewCoord.x][(int)newNewCoord.y].vertex != NULL)
							if (length(vertices[(int)newCoord.x][(int)newCoord.y].vertex->position - vertices[(int)coordinates.x][(int)coordinates.y].vertex->position) < distanceCutOff)
								if (length(vertices[(int)newNewCoord.x][(int)newNewCoord.y].vertex->position - vertices[(int)coordinates.x][(int)coordinates.y].vertex->position) < distanceCutOff)
									if (length(vertices[(int)newCoord.x][(int)newCoord.y].vertex->position - vertices[(int)newNewCoord.x][(int)newNewCoord.y].vertex->position) < distanceCutOff)
									{
										if (vertices[(int)newCoord.x][(int)newCoord.y].index < 0)
										{
											rigidbody->vertices.push_back(*vertices[(int)newCoord.x][(int)newCoord.y].vertex);
											vertices[(int)newCoord.x][(int)newCoord.y].index = rigidbody->vertices.size() - 1;
										}

										if (vertices[(int)newNewCoord.x][(int)newNewCoord.y].index < 0)
										{
											rigidbody->vertices.push_back(*vertices[(int)newNewCoord.x][(int)newNewCoord.y].vertex);
											vertices[(int)newNewCoord.x][(int)newNewCoord.y].index = rigidbody->vertices.size() - 1;
										}

										rigidbody->indices.push_back(vertices[(int)coordinates.x][(int)coordinates.y].index);
										rigidbody->indices.push_back(vertices[(int)newNewCoord.x][(int)newNewCoord.y].index);
										rigidbody->indices.push_back(vertices[(int)newCoord.x][(int)newCoord.y].index);
									}
			}
	}

	for (int i = 0; i < visitNext.size(); i++)
	{
		resolveMesh(vertices, size, rigidbody, visitNext[i], distanceCutOff);
	}
}


void ParticleSystem::calculateMassOfParticles(){
	// m = pV / n
	// where m is the mass, p is the rest density, V is the volume and n is the number of particles
	particleSystem->sysParams.mass = (particleSystem->sysParams.restDensity * particleSystem->sysParams.volume) / particleSystem->particles.size();

	cout << particleSystem->sysParams.mass << endl;
}