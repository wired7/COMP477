#pragma once
#include "glm.hpp"
#include <unordered_map>
#include <vector>
#include "Particle.h"
#include <string>
#include "GraphicsObject.h"

using namespace std;

struct GridCube {
	vector<int> particles;
	vector<pair<int, MeshObject*>> rigidData;
};

class Grid3D {
public:
	vector<vector<vector<GridCube>>> data;
	Grid3D::Grid3D(int numCells, float cellSize);
	Grid3D::Grid3D(glm::vec3 dim, float cellSize);
	Grid3D();
	~Grid3D();
	float getCellSize();
	void update(Particle& particle);
	vector<int> getNeighbors(Particle particle);
	void getNeighborCubes(vec3 currCell, vec3 origin, vec3 direction, float angle, vector<GridCube>* neighborCubes, int depth);
	vector<GridCube> getNeighborCubes(Particle particle);
	glm::vec3 dim;
private:
	float cellSize;
	int numCells;
	bool inRadius(glm::vec3 pos1, glm::vec3 pos2);
};

