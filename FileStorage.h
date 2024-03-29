#pragma once
#include "glm.hpp"
#include <vector>
#include <fstream>
#include "ParticleSystem.h"
#include "ProgramState.h"

using namespace std;

static class FileStorage
{
public:
	static int startPos;
	static void readFrames(char* file, int count, vector<vector<glm::vec3>>* frames, vector<MeshObject*>*);
	static int getFramesTotal(char* file);
	static fstream filePos;
	static bool hasOpen;
	static void resetReadFrames(char* file);
	static SystemParameters loadSysParams(string);
	static vector<MeshObject*> loadRigidbodies(string);
	static void serializeData(string file);
	static string serializeScene();
	static ProgramState deserializeData(string file);
	static string rigidBodyFile;
	static string getSerializedFile(string file);
private:
	static SystemParameters matchSysParams(vector<pair<string, float>> values);
	static vector<MeshObject*> matchRigidbodies(vector<pair<string, vector<float>>> values);
	static string serializeStrPS();
	static string serializeStrParticle(Particle particle);
	static void serializeRigidData(string outFile);
	static SystemParameters deserializeMatchPS(string values);
	static Particle* deserializeParticle(string values);
};

