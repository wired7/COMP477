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

long FileStorage::startPos;
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

vector<MeshObject*> FileStorage::loadRigidbodies(string filePath)
{
	vector<pair<string, vector<float>>> rigidbodies;

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

			string values = s.substr(s.find(delimiter) + delimiter.length() + 1);

			vector<float> v;

			while (values.length() != 0)
			{
				size_t found = values.find(" ");
				if (found >= values.length())
				{
					v.push_back(stof(values));
					break;
				}
				else
				{
					v.push_back(stof(values.substr(0, found)));
					values = values.substr(found + 1);
				}

			}

			rigidbodies.push_back(pair<string, vector<float>>(name, v));
		}
	}
	myfile.close();

	return matchRigidbodies(rigidbodies);
}

vector<MeshObject*> FileStorage::matchRigidbodies(vector<pair<string, vector<float>>> values)
{
	vector<MeshObject*> rigidbodies;

	for (int i = 0; i < values.size(); i++)
	{
		if (values[i].first == "cube")
		{
			vec3 center(values[i].second[0], values[i].second[1], values[i].second[2]);
			vec3 dimensions(values[i].second[3], values[i].second[4], values[i].second[5]);
			vec4 color(values[i].second[6], values[i].second[7], values[i].second[8], values[i].second[9]);

			rigidbodies.push_back(new Cube(center, dimensions, color, false));
		}
		else if (values[i].first == "sphere")
		{
			vec3 center(values[i].second[0], values[i].second[1], values[i].second[2]);
			vec3 dimensions(values[i].second[3], values[i].second[4], values[i].second[5]);
			vec4 color(values[i].second[6], values[i].second[7], values[i].second[8], values[i].second[9]);
			float res = values[i].second[10];

			rigidbodies.push_back(new Polyhedron(res, center, dimensions, color, false));
		}
/*		else if (values[i].first == "triangle")
		{
			vec3 pt1(values[i].second[0], values[i].second[1], values[i].second[2]);
			vec3 pt2(values[i].second[3], values[i].second[4], values[i].second[5]);
			vec3 pt3(values[i].second[6], values[i].second[7], values[i].second[8]);
			vec4 color(values[i].second[9], values[i].second[10], values[i].second[11], values[i].second[12]);
		}*/
	}

	return rigidbodies;
}

void FileStorage::readFrames(char* file, int count, vector<vector<glm::vec3>>* frames, vector<MeshObject*>* rigidbodies)
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

		while(true)
		{
			getline(filePos, line);
			stringstream ss(line);

			string name = line.substr(0, line.find(":"));
			transform(name.begin(), name.end(), name.begin(), ::tolower);

			if (name == "rigidbody")
			{
				getline(filePos, line);

				ss = stringstream(line);

				int stringLen = line.length();

				vector<Vertex2> vertices;

				while (stringLen > 0) {
					vec3 pos;

					for (int i = 0; i < 3; i++)
						ss >> pos[i];

					vec3 normal;

					for (int i = 0; i < 3; i++)
						ss >> normal[i];
					
					vec4 color;

					for (int i = 0; i < 4; i++)
						ss >> color[i];

					vertices.push_back(Vertex2(pos, normal, color, vec2(0, 0)));

					stringLen -= 10;
				}

				getline(filePos, line);

				ss = stringstream(line);

				stringLen = line.length();

				vector<GLuint> indices;

				int val;
				while (ss >> val)
				{
					indices.push_back(val);
				}

				rigidbodies->push_back(new Rigidbody(vertices, indices, mat4(1.0f), 1000, false));
			}
			else
				break;

		}

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

		startPos = filePos.tellg();
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
	filePos.seekg(startPos);
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


