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
	Rigidbody(); 
	~Rigidbody();
};

