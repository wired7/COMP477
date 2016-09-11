#pragma once
#include "GraphicsObject.h"
#include "NoiseTexture.h"
#include "Shader.h"

#ifndef DBL_EPSILON
#define DBL_EPSILON     2.2204460492503131e-016
#endif


#ifndef UPPER_RANGE 
#define UPPER_RANGE     40
#endif

class Terrain : public MeshObject, public LitObject
{
public:
	float **heights; // to store the heights of each node using perlin noise
	Terrain(int, int, int, int, noiseType);
	~Terrain();
	void draw();
	void setUpMesh(void);
	void setHeights(int, int, NoiseTexture);
	void createMesh(vec2, int, int, int, int, noiseType);
	void addToVertices(Vertex2, GLuint&, vec3);
	bool search_vertices(vector<Vertex2>, vec3, int&);
	bool AreSame(GLfloat, GLfloat);
	float getRandomNumber(float, float);
	float map(float, float, float, float, float);
};

