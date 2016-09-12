#pragma once
#include "Shader.h"
#include "Shape.h"
#include "TextureManager.h"
#include <vector>
#include <glew.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtx/rotate_vector.hpp>

using namespace glm;
using namespace std;

struct Vertex2 {
	glm::vec3 position;
	glm::vec4 color;
	glm::vec2 texCoord;
	glm::vec3 normal;
	GLuint id;
};

class GraphicsObject
{
public:
	mat4 model;
	Shader* shader;
	string texturePath;	
	GLuint texture;
	GraphicsObject() {};
	~GraphicsObject() {};
	void loadTexture(char*);
	virtual void draw() = 0;
};

class MeshObject : public GraphicsObject
{
public:
	vector<Vertex2> vertices;
	vector<GLuint> indices;
	GLuint VAO;
	GLuint EBO;
	MeshObject() {};
	~MeshObject() {};
	void bindBuffers(void);
	void enableBuffers(void);
	void addVertex(vec3, vec4, vec2, vec3);
};

class InstancedMeshObject : public MeshObject
{
public:
	vector<mat4> matrices;
	InstancedMeshObject() {};
	~InstancedMeshObject() {};
	void bindInstances(void);
	void enableInstances(void);
};

class LitObject
{
public:
	GLfloat specularIntensity;
	GLfloat shininess;
	GLfloat ambient;
	vec4 specularColor;
};

class Polyhedron : public MeshObject
{
public:
	int resolution;
	Polyhedron() {};
	Polyhedron(int, double, vec3, vec4);
	void draw();
};