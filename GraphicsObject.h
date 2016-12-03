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
#include <functional>
#include "ScreenState.h"
/*#include <Importer.hpp>
#include <scene.h>
#include <postprocess.h>

using namespace Assimp;*/
using namespace glm;
using namespace std;

struct Bounds {
	vec3 min;
	vec3 max;

	Bounds::Bounds() : max({ -INFINITY, -INFINITY, -INFINITY }), min({ INFINITY, INFINITY, INFINITY }) {};

	void join(Bounds b)
	{
		for (int i = 0; i < 3; i++)
		{
			if (b.min[i] < min[i])
				min[i] = b.min[i];
			if (b.max[i] > max[i])
				max[i] = b.max[i];
		}
	}
};

struct Vertex2 {
	glm::vec3 position;
	glm::vec4 color;
	glm::vec2 texCoord;
	glm::vec3 normal;
	GLuint id;

	Vertex2() {};
	Vertex2(vec3 pos, vec3 norm, vec4 col, vec2 tC) : position(pos), normal(norm), color(col), texCoord(tC) {};
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
	void applyTransform();
	Bounds getBounds();
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
	Polyhedron(int, vec3, vec3, vec4, bool);
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

class Cylinder : public MeshObject
{
public:
	Cylinder(vec3 topRadius, vec3 bottomRadius, vector<vec3> samplePoints, vec4 color, bool)
	{
		shader = LitShader::shader;

		loadTexture("textures\\blank.jpg");


	}
	void draw();
};

class GUIButton : public MeshObject
{
public:
	GUIButton() {};
	GUIButton(vec3 position, vec3 dimensions, vec4 color, char* text, vec4 textColor, char* texFilePath, bool isRendered, std::function<void()> clickEvent);
	void draw();
	virtual void checkMouseClick();
	void checkHover();
	glm::vec3 position;
	glm::vec3 dimensions;
	glm::vec4 hoverColor = glm::vec4(1.0f);
	glm::vec4 textColor;

	char* getText();

protected:
	bool isPointInRect(double x, double y);
	TextRenderer textRend;
	char* text;
private:
	std::function<void()> clickEvent;
};

class GUIBackground : public MeshObject
{
public:
	GUIBackground() {};
	GUIBackground(vec3 position, vec3 dimensions, vec4 color, char* texturePath, bool isRendered);
	glm::vec3 position;
	void draw();

};

class GUIButtonValued : public GUIButton
{
public:
	GUIButtonValued(float& Value) : valueToChange(Value) {};
	GUIButtonValued(vec3 position, vec3 dimensions, vec4 color, char* text, vec4 textColor, char* texFilePath, bool isRendered, std::function<void(float&)> clickEvent, float& ValueToChange);
	void checkMouseClick();

	float& valueToChange;

private:
	std::function<void(float&)> clickEvent;
};