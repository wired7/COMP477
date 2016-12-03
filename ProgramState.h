#pragma once
#include "ParticleSystem.h"

class ProgramState
{
public:
	SystemParameters sysParams;
	vector<Particle*> particlesVec;
	vector<MeshObject*> vecMeshes;
};