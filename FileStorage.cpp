#pragma once
#include "FileStorage.h"
#include <fstream>
#include "ParticleSystem.h"

using namespace std;

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
