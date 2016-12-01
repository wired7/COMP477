#pragma once
#include "Shader.h"
#include "Shape.h"
#include "TextRenderer.h"
#include "TextureManager.h"
#include <vector>
#include <glew.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtx/rotate_vector.hpp>
/*#include <Importer.hpp>
#include <scene.h>
#include <postprocess.h>

using namespace Assimp;*/
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
	virtual void bindBuffers(void);
	virtual void enableBuffers(void);
	void addVertex(vec3, vec4, vec2, vec3);
};

class InstancedMeshObject : public MeshObject
{
public:
	vector<mat4> matrices;
	InstancedMeshObject() {};
	~InstancedMeshObject() {};
	virtual void bindInstances(void);
	virtual void enableInstances(void);
};

class LitObject
{
public:
	GLfloat specularIntensity;
	GLfloat shininess;
	GLfloat ambient;
	vec4 specularColor;
};

/*class ImportedMesh : public MeshObject
{
public:
	ImportedMesh(char*, vec3, vec3);
	void draw();
private:
	void loadFile(char*);
};*/

class Polyhedron : public MeshObject
{
public:
	int resolution;
	Polyhedron() {};
	Polyhedron(int, double, vec3, vec4);
	void draw();
};

class InstancedSpheres : public InstancedMeshObject
{
public:
	static Polyhedron* sphere;
	GLuint positionsVertexBuffer;
	vector<vec3> positions;
	InstancedSpheres(float, int, vec4, vector<vec3> positions = {});
	InstancedSpheres(Polyhedron*) {};
	~InstancedSpheres() {};
	virtual void bindInstances(void);
	virtual void updateInstances(void);
	virtual void updateInstances(vector<vec3>*);
	virtual void enableInstances(void);
	virtual void draw();
};

class Rectangle : public MeshObject
{
public:
	Rectangle() {};
	Rectangle(vec3, vec3, vec4, bool);
	void draw();
};

class Cube : public MeshObject
{
public:
	Cube(vec3, vec3, vec4, bool);
	void draw();
};

class GUIButton : public MeshObject
{
public:
	GUIButton() {};
	GUIButton(vec3, vec3, vec4, char* text, bool);
	void draw();
	glm::vec3 position;

	char* getText();

private:
	TextRenderer textRend;
	char* text;
};