#pragma once
#include "ParticleSystem.h"

using namespace std;

static class FileStorage
{
public:
	static void write(char* file);
	static SystemParameters loadSysParams(string);
private:
	static SystemParameters matchSysParams(vector<pair<string, float>>);
};

