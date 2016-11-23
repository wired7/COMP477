#include "Rigidbody.h"



Rigidbody::Rigidbody(vector<Vertex2> vertices, vector<GLuint> indices, mat4 model, float mass, bool rendered, bool dynamic)
{
	shader = LitShader::shader;
	loadTexture("textures\\blank.jpg");

	this->vertices = vertices;
	this->indices = indices;
	this->model = model;
	this->mass = mass;
	this->dynamic = dynamic;

	calculateInertiaTensor();

	if(rendered)
		bindBuffers();
}


Rigidbody::~Rigidbody()
{
}

void Rigidbody::draw()
{
	enableBuffers();
	glUniformMatrix4fv(((LitShader*)shader)->modelID, 1, GL_FALSE, &model[0][0]);
	glBindTexture(GL_TEXTURE_2D, texture);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
}

void Rigidbody::calculateInertiaTensor()
{

}

void Rigidbody::applyTransform()
{
	//convert each vertex of the rigidbody to world coordinates based on latest model matrix
	if (model != mat4(1.0f))
	{
		mat4 transposeInverse = transpose(inverse(model));

		for (int j = 0; j < vertices.size(); ++j)
		{
			vertices[j].position = vec3(model * vec4(vertices[j].position, 1));
			vertices[j].normal = vec3(transposeInverse * vec4(vertices[j].position, 1));
		}

		model = mat4(1.0f);
	}
}