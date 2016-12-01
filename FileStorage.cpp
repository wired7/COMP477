#pragma once
#include "FileStorage.h"
#include <fstream>
#include "ParticleSystem.h"
#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <algorithm>

using namespace std;

fstream FileStorage::filePos;
bool FileStorage::hasOpen = false;

SystemParameters FileStorage::loadSysParams(string filePath)
{
	vector<pair<string, float>> sysParams;

	ifstream myfile;
	myfile.open(filePath);

	string delimiter = ":";

	if (myfile.is_open())
	{
		string s = "";
		while (getline(myfile, s))
		{
			string name = s.substr(0, s.find(delimiter));
			transform(name.begin(), name.end(), name.begin(), ::tolower);

			float value = stof(s.substr(s.find(delimiter) + delimiter.length()));

			sysParams.push_back(pair<string, float>(name, value));
		}
	}
	myfile.close();

	return matchSysParams(sysParams);
}

SystemParameters FileStorage::matchSysParams(vector<pair<string, float>> values)
{
	float volume = 0;
	float radius = 0;
	float searchRadius = 0;	
	float viscocity = 0;
	float stiffness = 0;
	float density = 0;
	float gravity = 0;
	float timeStep = 0;
	float maxTimeStep = 0;

	for (int i = 0; i < values.size(); i++)
	{
		if (values[i].first == "volume")
			volume = values[i].second;
		else if (values[i].first == "particle radius")
			radius = values[i].second;
		else if (values[i].first == "search radius")
			searchRadius = values[i].second;
		else if (values[i].first == "viscocity")
			viscocity = values[i].second;
		else if (values[i].first == "stiffness")
			stiffness = values[i].second;
		else if (values[i].first == "rest density")
			density = values[i].second;
		else if (values[i].first == "gravity")
			gravity = values[i].second;
		else if (values[i].first == "time step")
			timeStep = values[i].second;
		else if (values[i].first == "max time step")
			maxTimeStep = values[i].second;
	}

	return SystemParameters(volume, radius, searchRadius, viscocity, stiffness, density, gravity, timeStep, maxTimeStep);
}

void FileStorage::readFrames(char* file, int count, vector<vector<glm::vec3>>* frames)
{
	float x, y, z;

	float volume = 0;
	float radius = 0;
	float searchRadius = 0;
	float viscocity = 0;
	float stiffness = 0;
	float density = 0;
	float gravity = 0;
	float timeStep = 0;
	float maxTimeStep = 0;

	string line;
	string delimiter = " ";
	frames->clear();

	if (!hasOpen)
	{
		filePos.open(file, ios_base::in);
		hasOpen = true;
		getline(filePos, line);
		
		stringstream ss(line);

		ss >> volume;
		ss >> radius;
		ss >> searchRadius;
		ss >> viscocity;
		ss >> stiffness;
		ss >> density;
		ss >> gravity;
		ss >> timeStep;
		ss >> maxTimeStep;

		ParticleSystem::getInstance()->sysParams = SystemParameters(volume, radius, searchRadius, viscocity, stiffness, density, gravity, timeStep, maxTimeStep);
	}

	if (filePos.is_open())
	{
		for (int i = 0; i < count; ++i) {
			frames->push_back(vector<vec3>());
			getline(filePos, line);
			stringstream ss(line);
			float tStep;
			ss >> tStep;

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


