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
	Grid3D::Grid3D(int numCells, int cellSize);
	Grid3D();
	~Grid3D();
	int getCellSize();
	void update(Particle particle);
	vector<int> getNeighbors(Particle particle);
	vector<GridCube> getNeighborCubes(Particle particle);
private:
	int cellSize;
	int numCells;
	bool inRadius(glm::vec3 pos1, glm::vec3 pos2);
};

