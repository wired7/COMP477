#pragma once
#include "glm.hpp"
#include <vector>
#include <fstream>

using namespace std;

class FileStorage
{
public:
	FileStorage();
	~FileStorage();
	static void write(char* file);
	static void readFrames(char* file, int count, vector<vector<glm::vec3>>* frames);
	static int getFramesTotal(char* file);
	static fstream filePos;
	static bool hasOpen;
	static void resetReadFrames();
};

