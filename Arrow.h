#pragma once
#include "GraphicsObject.h"

class ArrowMesh : public MeshObject
{
public:
	int resolution;
	ArrowMesh();
	void draw() {};
	void draw(mat4);
};

class Arrow
{
public:
	static ArrowMesh* mesh;
	vec4 color;
	mat4 model;
	Arrow(vec3, vec3);
	~Arrow();
	void draw();
	void update(vec3, vec3);
};

