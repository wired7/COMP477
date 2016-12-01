#pragma once
#include <vector>
#include "Particle.h"
#include "Rigidbody.h"
#include "Grid3D.h"

using namespace std;

struct SystemParameters {
	float particleRadius;
	float searchRadius;
	float viscocity;
	float stiffness;
	float restDensity;
	float gravity;
	float tStep;
	float maxTStep;
	float blockSize;
	float mass;
	float tensionCoefficient = 0.0072f;
	float pressureGamma = 7.0f;

	SystemParameters() {};
	SystemParameters(float pR, float sR, float v, float k, float rD, float g, float tS, float mS) : particleRadius(pR), searchRadius(sR), viscocity(v), stiffness(k), restDensity(rD), gravity(g), tStep(tS), maxTStep(mS)
	{
		mass = 3.0f * 3.1415f * pow(particleRadius, 3) * restDensity / 4.0f;
	}
};

class ParticleSystem 
{
public:
	~ParticleSystem();
	static ParticleSystem* getInstance();
	void goNuts(float, float, string);
	vector<glm::vec3>* getParticlePositions();
	SystemParameters sysParams;
	vector<Particle*> particles;
	vector<Rigidbody*> rigidbodies;
	void addParticles(vector<Particle*>);
	void addRigidbodies(vector<Rigidbody*>);
	void updateList();
	void calcNeighbors(Particle* particle);
	void setStiffnessOfParticleSystem(int blockSize);
	Grid3D grid;
	vector<MeshObject*> rayTrace(vector<glm::vec3>*, float);
private:
	static ParticleSystem* particleSystem;
	ParticleSystem();
};
