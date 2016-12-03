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
	Cylinder(float topRadius, float bottomRadius, vec2 resolution, vector<vec3> samplePoints, vec4 color, bool render)
	{
		shader = LitShader::shader;

		loadTexture("textures\\blank.jpg");

		float radialStep = 2 * 3.1415f / resolution.x;
		vec3 diff;
		for (int i = 0; i < samplePoints.size(); i++)
		{
			float currentRadius = topRadius * (1 - (float)i / (float)samplePoints.size()) + bottomRadius * (float)i / (float)samplePoints.size();
			
			if(i < samplePoints.size() - 1)
				diff = normalize(samplePoints[i + 1] - samplePoints[i]);

			vec3 pt1 = normalize(vec3(1.0f, diff.y / diff.x, 0));

			cout << diff.y / diff.x << endl;

			system("PAUSE");

			for (int j = 0; j < resolution.x; j++)
			{
				vec3 pos = vec3(translate(mat4(1.0f), -samplePoints[i]) * rotate(mat4(1.0f), radialStep * j, diff) * translate(mat4(1.0f), samplePoints[i]) * vec4(pt1, 1.0f));
				vec3 normal = normalize(pos - samplePoints[i]);
				addVertex(pos, color, vec2(), normal);

				if (j > 0 && i > 0)
				{
					indices.push_back(vertices.size() - 2 - resolution.x);
					indices.push_back(vertices.size() - 2);
					indices.push_back(vertices.size() - 1 - resolution.x);

					indices.push_back(vertices.size() - 2);
					indices.push_back(vertices.size() - 1);
					indices.push_back(vertices.size() - 1 - resolution.x);
				}
			}
		}

		if (render)
			bindBuffers();
	}
	void draw() {
		enableBuffers();
		glUniformMatrix4fv(((LitShader*)shader)->modelID, 1, GL_FALSE, &model[0][0]);
		glBindTexture(GL_TEXTURE_2D, texture);
		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	};
};

class GUIButton : public MeshObject
{
public:
	GUIButton() {};
	GUIButton(vec3 position, vec3 dimensions, vec4 color, char* text, vec4 textColor, char* texFilePath, bool isRendered, std::function<void()> clickEvent);
	void draw();
	void checkMouseClick();
	void checkHover();
	glm::vec3 position;
	glm::vec3 dimensions;
	glm::vec4 hoverColor = glm::vec4(1.0f);
	glm::vec4 textColor;

	char* getText();

private:
	bool isPointInRect(double x, double y);

	std::function<void()> clickEvent;
	TextRenderer textRend;
	char* text;
};

class GUIBackground : public MeshObject
{
public:
	GUIBackground() {};
	GUIBackground(vec3 position, vec3 dimensions, vec4 color, char* texturePath, bool isRendered);
	glm::vec3 position;
	void draw();

};