#include "GraphicsObject.h"

void GraphicsObject::loadTexture(char* filePath)
{
	texturePath = filePath;
	std::ifstream in(filePath, std::ios::in);
	assert(in.is_open());

	texture = TextureManager::instance()->addTexture(filePath);
}

void MeshObject::bindBuffers(void)
{
	GLuint VBO;

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(Vertex2), &vertices[0], GL_DYNAMIC_DRAW);

	if (indices.size())
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size()*sizeof(GLuint), &indices[0], GL_DYNAMIC_DRAW);
	}

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex2), (GLvoid*)0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex2), (GLvoid*)offsetof(Vertex2, color));
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex2), (GLvoid*)offsetof(Vertex2, normal));
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex2), (GLvoid*)offsetof(Vertex2, texCoord));

	glBindVertexArray(0);
}


void MeshObject::enableBuffers()
{
	glBindVertexArray(VAO);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);
}

void MeshObject::addVertex(vec3 pos, vec4 color, vec2 textCoord, vec3 normal)
{
	vertices.push_back(Vertex2());
	vertices[vertices.size() - 1].position = pos;
	vertices[vertices.size() - 1].color = color;
	vertices[vertices.size() - 1].texCoord = textCoord;
	vertices[vertices.size() - 1].normal = glm::normalize(normal);
}


void InstancedMeshObject::bindInstances(void)
{
	GLuint buffer;
	glBindVertexArray(VAO);
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, matrices.size() * sizeof(glm::mat4), &matrices[0], GL_DYNAMIC_DRAW);
	// Vertex Attributes
	GLsizei vec4Size = sizeof(glm::vec4);
	glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (GLvoid*)0);
	glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (GLvoid*)(vec4Size));
	glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (GLvoid*)(2 * vec4Size));
	glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (GLvoid*)(3 * vec4Size));

	glVertexAttribDivisor(4, 1);
	glVertexAttribDivisor(5, 1);
	glVertexAttribDivisor(6, 1);
	glVertexAttribDivisor(7, 1);

	glBindVertexArray(0);
}


void InstancedMeshObject::enableInstances(void)
{
	glEnableVertexAttribArray(4);
	glEnableVertexAttribArray(5);
	glEnableVertexAttribArray(6);
	glEnableVertexAttribArray(7);
}


Polyhedron::Polyhedron(int resolution, double radius, vec3 pos, vec4 color)
{
	shader = LitShader::shader;
	
	loadTexture("textures\\blank.jpg");
//	model = rotate(scale(translate(mat4(1.0f), pos), vec3(radius, radius, radius)), 3.1415f / 2, vec3(1, 0, 0));
	this->resolution = resolution;
	double angle = 2 * 3.1415 / resolution;
	vector<vector<vec3>> circles;
	vec3 start(0, -1, 0);
	for (int j = 0; j <= resolution; j++)
	{
		vec3 temp = rotate(start, (GLfloat)(angle * j), vec3(0, 0, 1));
		circles.push_back(vector<vec3>());
		for (int i = 0; i <= resolution; i++)
			circles[circles.size() - 1].push_back(rotate(temp, (GLfloat)(angle * i), vec3(0, 1, 0)));
	}

	for (int j = 0; j < circles.size() - 1; j++)
		for (int i = 0; i < circles[j].size(); i++)
			addVertex(circles[j][i], color, vec2(i, 1) / (GLfloat)resolution, normalize(circles[j][i]));

	int m = resolution + 1;
	int s = vertices.size() - m;

	for (int i = 0; i < s; i++)
	{
		indices.push_back((i % m) + m * (i / m));
		indices.push_back((i % m) + m * ((i / m) + 1));
	}

	bindBuffers();
}

void Polyhedron::draw(void)
{
	enableBuffers();
	glUniformMatrix4fv(((LitShader*)shader)->modelID, 1, GL_FALSE, &model[0][0]);
	glBindTexture(GL_TEXTURE_2D, texture);
	glDrawElements(GL_TRIANGLE_STRIP, indices.size(), GL_UNSIGNED_INT, 0);
}

void Polyhedron::draw(mat4 model)
{
	enableBuffers();
	glUniformMatrix4fv(((LitShader*)shader)->modelID, 1, GL_FALSE, &model[0][0]);
	glBindTexture(GL_TEXTURE_2D, texture);
	glDrawElements(GL_TRIANGLE_STRIP, indices.size(), GL_UNSIGNED_INT, 0);
}