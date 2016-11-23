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

	SystemParameters() {};
	SystemParameters(float pR, float sR, float v, float k, float rD, float g, float tS, float mS) : particleRadius(pR), searchRadius(sR), viscocity(v), stiffness(k), restDensity(rD), gravity(g), tStep(tS), maxTStep(mS) {}
};

class ParticleSystem 
{
public:
	~ParticleSystem();
	static ParticleSystem* getInstance();
	vector<glm::vec3>* getParticlePositions();
	SystemParameters sysParams;
	vector<Particle*> particles;
	vector<Rigidbody*> rigidbodies;
	void addParticles(vector<Particle*>);
	void addRigidbodies(vector<Rigidbody*>);
	void updateList();
	void calcNeighbors(Particle* particle);
	Grid3D grid;
private:
	static ParticleSystem* particleSystem;
	ParticleSystem();
};
