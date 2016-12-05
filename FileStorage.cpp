#pragma once
#include "FileStorage.h"
#include <fstream>
#include "ParticleSystem.h"
#include <string>
#include <sstream>
#include <iostream>
#include <algorithm>
#include "SceneManager.h"

using namespace std;

int FileStorage::startPos;
fstream FileStorage::filePos;
bool FileStorage::hasOpen = false;

string FileStorage::rigidBodyFile;

vector<string> split(const string &s, char delim) {
	stringstream ss(s);
	string item;
	vector<string> tokens;
	while (getline(ss, item, delim)) {
		tokens.push_back(item);
	}
	return tokens;
}
// open the system params file
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
// instantiate a particle system by using the values imported from the file
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
// get the params of the rigidbodies from a particular file path
vector<MeshObject*> FileStorage::loadRigidbodies(string filePath)
{
	rigidBodyFile = filePath;
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
// instantiate the rigidbodies in the scene from a specific file
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
		else if (values[i].first == "cylinder")
		{
			float startingRadius = values[i].second[0];
			bool openTop = values[i].second[1];
			float endingRadius = values[i].second[2];
			bool openBottom = values[i].second[3];
			float resolution = values[i].second[4];
			vec4 color(values[i].second[5], values[i].second[6], values[i].second[7], values[i].second[8]);
			vector<vec3> points;

			for (int j = 9; j < values[i].second.size(); j += 3)
			{
				vec3 temp;
				for (int k = 0; k < 3; k++)
					temp[k] = values[i].second[j + k];
				points.push_back(temp);
			}

			rigidbodies.push_back(new Cylinder(startingRadius, openTop, endingRadius, openBottom, vec2(resolution, 0), points, color, false));
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
// function to read frames from a file and output it in the scene player
void FileStorage::readFrames(char* file, int count, vector<vector<glm::vec3>>* frames, vector<MeshObject*>* rigidbodies)
{
	float x, y, z;

	frames->clear();

	string line;

	const int size = 8192;
	char buffer[size];

		if (!hasOpen)
		{
			filePos.rdbuf()->pubsetbuf(buffer, size);
			filePos.open(file, ios_base::in);

			if (filePos.is_open())
			{
				hasOpen = true;

				float volume = 0;
				float radius = 0;
				float searchRadius = 0;
				float viscocity = 0;
				float stiffness = 0;
				float density = 0;
				float gravity = 0;
				float timeStep = 0;
				float maxTimeStep = 0;

				string delimiter = " ";

				while (true)
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
					else if (name == "serialized")
					{

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
			}
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
// function to enable looping of a scene
void FileStorage::resetReadFrames(char* file)
{
	filePos.close();

	filePos.open(file, ios_base::in);
	string line;
	for (int i = 0; i < startPos; ++i) 
	{
		std::getline(filePos, line);
	}

}
// get the size of the file
int FileStorage::getFramesTotal(char* file)
{
	std::ifstream f(file);
	int count = 0;
	if (f.is_open())
	{
		std::string line;
		std::string s = "Particles:";

		startPos = 0;
		std::getline(f, line);

		while (line != s) {    
			startPos++;
			std::getline(f, line);
		}
		std::getline(f, line);
		startPos+=3;

		for (count = 0; std::getline(f, line); ++count);
	}
	f.close();
	return count; //we dont count the first line
}
// serialize the data on keyboard press or after the final simulation in order to enable further continuation of the simulation if required
void FileStorage::serializeData(string file)
{
	/*
	vector<string> vec = split(file, '\\');

	string newFile = "Animations/Serialize/s_" + vec.at(vec.size() - 1);

	std::fstream f(file, std::ios_base::app);
	if (f.is_open())
	{
		f << "Serialized:" << newFile << endl;
	}
	else
	{
		cout << "SERIALIZATION FAILED" << endl;
	}
	f.close();
	*/

	vector<string> vec = split(file, '\\');
	string newFile = "Animations/Serialize/s_" + vec.at(vec.size() - 1);

	cout << "Serialized file path: " << newFile << endl;
	std::fstream f2(newFile, ios::out);
	if (f2.is_open())
	{

		f2 << "Particle System: " << endl;
		f2 << serializeStrPS() << endl;

		f2 << "Particles: " << endl;
		for (int i = 0; i < ParticleSystem::getInstance()->particles.size(); ++i)
		{
			f2 << serializeStrParticle(*ParticleSystem::getInstance()->particles[i]) << endl;
		}
		f2.close();
		serializeRigidData(newFile);
	}
	else {
		cout << "SERIALIZATION FAILED" << endl;
	}
}
// deserialize the data in order to instantiate the objects
ProgramState FileStorage::deserializeData(string file)
{
	fstream fs(file);

	ProgramState programState;
	SystemParameters sysParams;
	vector<Particle*> particlesVec;
	vector<MeshObject*> vecMeshes;

	if (fs.is_open())
	{
		string line;
		while (getline(fs, line))
		{
			startLoop:
			vector<string> vec = split(line, ':');

			if (vec.size() == 1 || vec.size() == 2) {
				if (vec[0] == "Particle System")
				{
					getline(fs, line);
					sysParams = deserializeMatchPS(line);
				}
				else if (vec[0] == "Particles")
				{
					while (getline(fs, line))
					{
						vec = split(line, ':');
						if (vec.size() == 1 || vec.size() == 2)
						{
							break;
						}
						Particle* particle = deserializeParticle(line);
						particlesVec.push_back(particle);
					}
				}
				
				if (vec[0] == "Rigid")
				{
					ofstream out("tempfile");
					string str;

					while (getline(fs, str))
					{
						out << str;
					}
					out.close();
					vecMeshes = loadRigidbodies("tempfile");
				}
			}			
		}
	}
	fs.close();
	programState.particlesVec = particlesVec;
	programState.sysParams = sysParams;
	programState.vecMeshes = vecMeshes;

	return programState;
}

// serialize the particle system data of the scene into a file
using namespace std;
string FileStorage::serializeStrPS()
{
	SystemParameters params = ParticleSystem::getInstance()->sysParams;
	string ret = "";

	ret += "volume: " + to_string(params.volume) + ":";
	ret += "particleRadius: " + to_string(params.particleRadius) + ":";
	ret += "searchRadius: " + to_string(params.searchRadius) + ":";
	ret += "viscocity: " + to_string(params.viscocity) + ":";
	ret += "stiffness: " + to_string(params.stiffness) + ":";
	ret += "restDensity: " + to_string(params.restDensity) + ":";
	ret += "gravity: " + to_string(params.gravity) + ":";
	ret += "tStep: " + to_string(params.tStep) + ":";
	ret += "maxTStep: " + to_string(params.maxTStep) + ":";
	ret += "blockSize: " + to_string(params.blockSize) + ":";
	ret += "mass: " + to_string(params.mass) + ":";
	ret += "tensionCoefficient: " + to_string(params.tensionCoefficient) + ":";
	ret += "pressureGamma: " + to_string(params.pressureGamma) + ":";

	return ret;
}
// serialize the particle data of the scene into a file
string FileStorage::serializeStrParticle(Particle particle)
{
	ParticleParameters params = particle.params;
	string ret = "";
	ret += "velocity: " + to_string(params.velocity.x) + " " + to_string(params.velocity.y) + " " + to_string(params.velocity.z) + ":";
	ret += "acceleration: " + to_string(params.acceleration.x) + " " + to_string(params.acceleration.y) + " " + to_string(params.acceleration.z) + ":";
	ret += "gradientPressure: " + to_string(params.gradientPressure.x) + " " + to_string(params.gradientPressure.y) + " " + to_string(params.gradientPressure.z) + ":";
	ret += "laplacianVelocity: " + to_string(params.laplacianVelocity.x) + " " + to_string(params.laplacianVelocity.y) + " " + to_string(params.laplacianVelocity.z) + ":";
	ret += "tensionForce: " + to_string(params.tensionForce.x) + " " + to_string(params.tensionForce.y) + " " + to_string(params.tensionForce.z) + ":";
	ret += "gradientSmoothColor: " + to_string(params.gradientSmoothColor.x) + " " + to_string(params.gradientSmoothColor.y) + " " + to_string(params.gradientSmoothColor.z) + ":";
	ret += "laplacianSmoothColor: " + to_string(params.laplacianSmoothColor.x) + " " + to_string(params.laplacianSmoothColor.y) + " " + to_string(params.laplacianSmoothColor.z) + ":";
	ret += "density: " + to_string(params.density) + ":";
	ret += "pressure: " + to_string(params.pressure) + ":";
	ret += "kernel: " + to_string(params.kernel) + ":";
	ret += "position: " + to_string(particle.position.x) + " " + to_string(particle.position.y) + " " + to_string(particle.position.z) + ":";

	return ret;
}
// serialize all objects within a scene
string FileStorage::serializeScene()
{

	std::vector<MeshObject*> sceneObjects = Scenes::sceneEditor->rigidbodies;
	string ret = "";

	for (int i = 0; i < Scenes::sceneEditor->rigidbodies.size(); i++)
	{
		ret += Scenes::sceneEditor->rigidbodies[i]->shapeType + ": ";
		ret += std::to_string(Scenes::sceneEditor->rigidbodies[i]->pos.x) + " ";
		ret += std::to_string(Scenes::sceneEditor->rigidbodies[i]->pos.y) + " ";
		ret += std::to_string(Scenes::sceneEditor->rigidbodies[i]->pos.z) + " ";
		ret += std::to_string(Scenes::sceneEditor->rigidbodies[i]->dimensions.x) + " ";
		ret += std::to_string(Scenes::sceneEditor->rigidbodies[i]->dimensions.y) + " ";
		ret += std::to_string(Scenes::sceneEditor->rigidbodies[i]->dimensions.z) + " ";
		ret += std::to_string(Scenes::sceneEditor->rigidbodies[i]->color.r) + " ";
		ret += std::to_string(Scenes::sceneEditor->rigidbodies[i]->color.g) + " ";
		ret += std::to_string(Scenes::sceneEditor->rigidbodies[i]->color.b) + " ";
		ret += std::to_string(Scenes::sceneEditor->rigidbodies[i]->color.a) + " ";
		ret += "\n";
	}

	return ret;
}

void FileStorage::serializeRigidData(string outFile)
{
	cout << "rigidBodyFile: " << rigidBodyFile << endl;
	ifstream in(rigidBodyFile);
	ofstream out(outFile, std::ios_base::app);
	string str;
	out << "Rigid: " << endl;
	while (getline(in, str))
	{
		cout << str << endl;
		out << str;
	}
	in.close(); 
	out.close(); 
}

SystemParameters FileStorage::deserializeMatchPS(string values)
{
	vector<string> vec = split(values, ':');
	SystemParameters sysParams;

	float volume;
	float particleRadius;
	float searchRadius;
	float viscocity;
	float stiffness;
	float restDensity;
	float gravity;
	float tStep;
	float maxTStep;
	float blockSize;
	float mass;
	float tensionCoefficient;
	float pressureGamma;

	for (int i = 0; i < vec.size();)
	{
		if (vec[i] == "volume")
		{
			i++;
			stringstream ss(vec[i++]);
			ss >> volume;
			sysParams.volume = volume;
		}
		else if (vec[i] == "particleRadius")
		{
			i++;
			stringstream ss(vec[i++]);
			ss >> particleRadius;
			sysParams.particleRadius = particleRadius;
		}
		else if (vec[i] == "searchRadius")
		{
			i++;
			stringstream ss(vec[i++]);
			ss >> searchRadius;
			sysParams.searchRadius = searchRadius;
		}
		else if (vec[i] == "viscocity")
		{
			i++;
			stringstream ss(vec[i++]);
			ss >> viscocity;
			sysParams.viscocity = viscocity;
		}
		else if (vec[i] == "stiffness")
		{
			i++;
			stringstream ss(vec[i++]);
			ss >> stiffness;
			sysParams.stiffness = stiffness;
		}
		else if (vec[i] == "restDensity")
		{
			i++;
			stringstream ss(vec[i++]);
			ss >> restDensity;
			sysParams.restDensity = restDensity;
		}
		else if (vec[i] == "gravity")
		{
			i++;
			stringstream ss(vec[i++]);
			ss >> gravity;
			sysParams.gravity = gravity;
		}
		else if (vec[i] == "tStep")
		{
			i++;
			stringstream ss(vec[i++]);
			ss >> tStep;
			sysParams.tStep = tStep;
		}
		else if (vec[i] == "maxTStep")
		{
			i++;
			stringstream ss(vec[i++]);
			ss >> maxTStep;
			sysParams.maxTStep = maxTStep;
		}
		else if (vec[i] == "blockSize")
		{
			i++;
			stringstream ss(vec[i++]);
			ss >> blockSize;
			sysParams.blockSize = blockSize;
		}
		else if (vec[i] == "mass")
		{
			i++;
			stringstream ss(vec[i++]);
			ss >> mass;
			sysParams.mass = mass;
		}
		else if (vec[i] == "tensionCoefficient")
		{
			i++;
			stringstream ss(vec[i++]);
			ss >> tensionCoefficient;
			sysParams.tensionCoefficient = tensionCoefficient;
		}
		else if (vec[i] == "pressureGamma")
		{
			i++;
			stringstream ss(vec[i++]);
			ss >> pressureGamma;
			sysParams.pressureGamma = pressureGamma;
		}
	}
	return sysParams;
}

Particle* FileStorage::deserializeParticle(string values)
{
	vector<string> vec = split(values, ':');
	ParticleSystem* sys = ParticleSystem::getInstance();

	glm::vec3 velocity;
	glm::vec3 acceleration;
	glm::vec3 gradientPressure;
	glm::vec3 laplacianVelocity;
	glm::vec3 tensionForce;
	glm::vec3 gradientSmoothColor;
	glm::vec3 laplacianSmoothColor;
	glm::vec3 position;
	float density;
	float pressure;
	float kernel;

	Particle* particle = new Particle(glm::vec3());

	for (int i = 0; i < vec.size();)
	{
		if (vec[i] == "velocity")
		{
			i++;
			stringstream ss(vec[i++]);
			ss >> velocity.x;
			ss >> velocity.y;
			ss >> velocity.z;
			particle->params.velocity = velocity;
		}
		else if (vec[i] == "acceleration")
		{
			i++;
			stringstream ss(vec[i++]);
			ss >> acceleration.x;
			ss >> acceleration.y;
			ss >> acceleration.z;
			particle->params.acceleration = acceleration;
		}
		else if (vec[i] == "gradientPressure")
		{
			i++;
			stringstream ss(vec[i++]);
			ss >> gradientPressure.x;
			ss >> gradientPressure.y;
			ss >> gradientPressure.z;
			particle->params.gradientPressure = gradientPressure;
		}
		else if (vec[i] == "laplacianVelocity")
		{
			i++;
			stringstream ss(vec[i++]);
			ss >> laplacianVelocity.x;
			ss >> laplacianVelocity.y;
			ss >> laplacianVelocity.z;
			particle->params.laplacianVelocity = laplacianVelocity;
		}
		else if (vec[i] == "tensionForce")
		{
			i++;
			stringstream ss(vec[i++]);
			ss >> tensionForce.x;
			ss >> tensionForce.y;
			ss >> tensionForce.z;
			particle->params.tensionForce = tensionForce;
		}
		else if (vec[i] == "gradientSmoothColor")
		{
			i++;
			stringstream ss(vec[i++]);
			ss >> gradientSmoothColor.x;
			ss >> gradientSmoothColor.y;
			ss >> gradientSmoothColor.z;
			particle->params.gradientSmoothColor = gradientSmoothColor;
		}
		else if (vec[i] == "laplacianSmoothColor")
		{
			i++;
			stringstream ss(vec[i++]);
			ss >> laplacianSmoothColor.x;
			ss >> laplacianSmoothColor.y;
			ss >> laplacianSmoothColor.z;
			particle->params.laplacianSmoothColor = laplacianSmoothColor;
		}
		else if (vec[i] == "position")
		{
			i++;
			stringstream ss(vec[i++]);
			ss >> position.x;
			ss >> position.y;
			ss >> position.z;
			particle->position = position;
		}
		else if (vec[i] == "density")
		{
			i++;
			stringstream ss(vec[i++]);
			ss >> density;
			particle->params.density = density;
		}
		else if (vec[i] == "pressure")
		{
			i++;
			stringstream ss(vec[i++]);
			ss >> pressure;
			particle->params.pressure = pressure;
		}
		else if (vec[i] == "kernel")
		{
			i++;
			stringstream ss(vec[i++]);
			ss >> kernel;
			particle->params.kernel = kernel;
		}
	}
	return particle;
}

string FileStorage::getSerializedFile(string file)
{
	fstream fs(file);
	vector<string> vec;

	if (fs.is_open())
	{
		string line;
		while (getline(fs, line))
		{
			vec = split(line, ':');
			if (vec[0] == "Serialized")
			{
				return vec[1];
			}
		}
	}

	return "";
}
