#pragma once
#include "FileStorage.h"
#include <fstream>
#include "ParticleSystem.h"
#include <string>
#include <sstream>

using namespace std;

fstream FileStorage::filePos;
bool FileStorage::hasOpen = false;

FileStorage::FileStorage()
{
}


FileStorage::~FileStorage()
{
}

void FileStorage::write(char* file)
{
	ofstream outfile;
	outfile.open(file, std::ios::app);

	if (outfile.is_open())
	{
		ParticleSystem* sys = ParticleSystem::getInstance();
		for (int i = 0; i < sys->particles.size(); ++i)
		{
			string x = to_string(sys->particles[i]->position.x);
			string y = to_string(sys->particles[i]->position.y);
			string z = to_string(sys->particles[i]->position.z);
			outfile << x + " " + y + " " + z + " ";
		}
		outfile << "\n";
	}
	outfile.close();
}

void FileStorage::readFrames(char* file, int count, vector<vector<glm::vec3>>* frames)
{
	float x, y, z;
	string line;

	frames->clear();

	if (!hasOpen)
	{
		filePos.open(file, ios_base::in);
		hasOpen = true;
		getline(filePos, line); //skip first line
	}

	if (filePos.is_open())
	{
		for (int i = 0; i < count; ++i) {
			frames->push_back(vector<vec3>());
			getline(filePos, line);
			stringstream ss(line);
			while (ss >> x)
			{
				ss >> y;
				ss >> z;
				(*frames)[i].push_back(vec3(x, y, z));
			}
		}
	}
}

void FileStorage::resetReadFrames()
{
	filePos.close();
	hasOpen = false;
}

int FileStorage::getFramesTotal(char* file)
{
	std::ifstream f(file);
	int count = 0;
	if (f.is_open())
	{
		std::string line;
		for (count = 0; std::getline(f, line); ++count);
	}
	return count - 1; //we dont count the first line
}


