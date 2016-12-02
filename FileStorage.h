#pragma once
#include "glm.hpp"
#include <vector>
#include <fstream>
#include "ParticleSystem.h"

using namespace std;

static class FileStorage
{
public:
	static long startPos;
	static void readFrames(char* file, int count, vector<vector<glm::vec3>>* frames, vector<MeshObject*>*);
	static int getFramesTotal(char* file);
	static fstream filePos;
	static bool hasOpen;
	static void resetReadFrames();
	static SystemParameters loadSysParams(string);
	static vector<MeshObject*> loadRigidbodies(string);
private:
	static SystemParameters matchSysParams(vector<pair<string, float>> values);
	static vector<MeshObject*> matchRigidbodies(vector<pair<string, vector<float>>> values);
};

