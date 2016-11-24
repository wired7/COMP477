#pragma once
#include "FileStorage.h"
#include <fstream>
#include <fstream>
#include <iostream>
#include <string>
#include <algorithm>

using namespace std;

void FileStorage::write(char* file)
{

}

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
	float radius = 0;
	float searchRadius = 0;
	float viscocity = 0;
	float stiffness = 0;
	float density = 0;
	float gravity = 0;
	float timeStep = 0;
	float maxTimeStep = 0;
	float mass = 0;

	for (int i = 0; i < values.size(); i++)
	{
		if (values[i].first == "radius")
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
		else if (values[i].first == "mass")
			mass = values[i].second;
	}

	return SystemParameters(radius, searchRadius, viscocity, stiffness, density, gravity, timeStep, maxTimeStep, mass);
}


