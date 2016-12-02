#pragma once
#include <vector>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtx/rotate_vector.hpp>
#include "GraphicsObject.h"

class Rigidbody : public MeshObject
{
public:
	float mass;
	mat3 inertiaTensor;
	bool dynamic;
	Rigidbody(vector<Vertex2>, vector<GLuint>, mat4, float, bool rendered, bool dynamic = false); 
	~Rigidbody();
	void draw();
private:
	void calculateInertiaTensor(void);
};

