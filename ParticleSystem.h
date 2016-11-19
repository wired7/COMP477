#pragma once
#include <vector>
#include "Particle.h"
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
};

class ParticleSystem 
{
public:
	~ParticleSystem();
	static ParticleSystem* getInstance();
	SystemParameters sysParams;
	vector<Particle*> particles;
	void updateList();
	void calcNeighbors(Particle* particle);
	Grid3D grid;
private:
	static ParticleSystem* particleSystem;
	ParticleSystem();
};
