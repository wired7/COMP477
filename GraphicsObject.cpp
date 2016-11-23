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

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex2), (GLvoid*)offsetof(Vertex2, position));
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
	model = scale(translate(mat4(1.0f), pos), vec3(radius, radius, radius));
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

Polyhedron* InstancedSpheres::sphere;

InstancedSpheres::InstancedSpheres(float radius, int resolution, vec4 color, vector<vec3> positions)
{
	shader = InstancedLitShader::shader;
	sphere = new Polyhedron(resolution, radius, vec3(0, 0, 0), color);
	model = scale(mat4(1.0f), vec3(radius, radius, radius));

	vertices = sphere->vertices;
	indices = sphere->indices;

	this->positions = positions;

	if (this->positions.size())
	{
		InstancedMeshObject::bindBuffers();
		bindInstances();
	}
}

void InstancedSpheres::bindInstances(void)
{
	GLuint positionsVertexBuffer;
	glBindVertexArray(VAO);
	glGenBuffers(1, &positionsVertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, positionsVertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(vec3), &(positions[0]), GL_DYNAMIC_DRAW);
	// Vertex Attributes
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), (GLvoid*)0);
	glVertexAttribDivisor(4, 1);

	glBindVertexArray(0);

//	positionsVertexBuffer = VBO;
}

void InstancedSpheres::updateInstances()
{
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, 8);
	glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(vec3), &(positions[0]), GL_DYNAMIC_DRAW);

	glBindVertexArray(0);
}

void InstancedSpheres::updateInstances(vector<vec3>* positions)
{
	glBindVertexArray(VAO);
//	GLuint VBO = positionsVertexBuffer;
	glBindBuffer(GL_ARRAY_BUFFER, 8);
	glBufferData(GL_ARRAY_BUFFER, positions->size() * sizeof(vec3), &((*positions)[0]), GL_DYNAMIC_DRAW);

	glBindVertexArray(0);

//	positionsVertexBuffer = VBO;
}

void InstancedSpheres::enableInstances(void)
{
	glEnableVertexAttribArray(4);
}

void InstancedSpheres::draw()
{
	InstancedMeshObject::enableBuffers();
	enableInstances();
	glUniform3fv(((InstancedLitShader*)shader)->scaleID, 1, &(vec3(model[0][0], model[1][1], model[2][2])[0]));
	glBindTexture(GL_TEXTURE_2D, texture);
	glDrawElementsInstanced(GL_TRIANGLE_STRIP, indices.size(), GL_UNSIGNED_INT, 0, positions.size());
}

Rectangle::Rectangle(vec3 position, vec3 dimensions, vec4 color)
{
	shader = LitShader::shader;
	loadTexture("textures\\blank.jpg");

	addVertex(vec3(0, 0, 0), color, vec2(), vec3(0, 0, -1));
	addVertex(vec3(0, 1, 0), color, vec2(), vec3(0, 0, -1));
	addVertex(vec3(1, 0, 0), color, vec2(), vec3(0, 0, -1));
	addVertex(vec3(1, 1, 0), color, vec2(), vec3(0, 0, -1));

	model = scale(translate(mat4(), position), dimensions);

	bindBuffers();
}

void Rectangle::draw()
{
	enableBuffers();
	glUniformMatrix4fv(((LitShader*)shader)->modelID, 1, GL_FALSE, &model[0][0]);
	glBindTexture(GL_TEXTURE_2D, texture);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, vertices.size());
}

Cube::Cube(vec3 center, vec3 dimensions, vec4 color)
{
	shader = LitShader::shader;
	loadTexture("textures\\blank.jpg");

	for(int i = 0; i < 2; i++)
		for(int j = 0; j < 2; j++)
			for(int k = 0; k < 2; k++)
				addVertex(0.5f * vec3(i * 2 - 1, j * 2 - 1, k * 2 - 1), color, vec2(), normalize(0.5f * vec3(i * 2 - 1, j * 2 - 1, k * 2 - 1) - center));

	indices = {0, 1, 2, 1, 3, 2, 0, 2, 4, 2, 6, 4, 4, 6, 5, 6, 7, 5, 5, 7, 1, 7, 3, 1, 1, 0, 4, 1, 4, 5, 2, 3, 6, 3, 7, 6};

	model = translate(mat4(1.0f), center) * scale(mat4(1.0f), dimensions);

	bindBuffers();
}

void Cube::draw()
{
	enableBuffers();
	glUniformMatrix4fv(((LitShader*)shader)->modelID, 1, GL_FALSE, &model[0][0]);
	glBindTexture(GL_TEXTURE_2D, texture);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
}

/*ImportedMesh::ImportedMesh(char* s, vec3 position, vec3 dimensions)
{
	shader = LitShader::shader;
	loadTexture("textures\\blank.jpg");

	loadFile(s);
	bindBuffers();

	model = rotate(scale(mat4(), 0.07f * dimensions), -3.1415f / 2, vec3(1, 0, 0));
	model[3] = vec4(position, 1);
}

void ImportedMesh::loadFile(char* s)
{
	Importer importer;
	const aiScene* scene = importer.ReadFile(s, aiProcess_CalcTangentSpace | aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_SortByPType);

	if (!scene)
	{
		cout << "Couldn't load " << s << endl;
		return;
	}

	for(int i = 0; i < scene->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[i];
		int iMeshFaces = mesh->mNumFaces;
		for(int j = 0; j < iMeshFaces; j++)
		{
			const aiFace& face = mesh->mFaces[j];
			for(int k = 0; k < 3; k++)
			{
				aiVector3D pos = mesh->mVertices[face.mIndices[k]];
				aiVector3D uv = mesh->mTextureCoords[0][face.mIndices[k]];
				aiVector3D normal = mesh->HasNormals() ? mesh->mNormals[face.mIndices[k]] : aiVector3D(1.0f, 1.0f, 1.0f);
				addVertex(vec3(pos.x, pos.y, pos.z), vec4(0.5, 0.5, 0.5, 1), vec2(uv.x, uv.y), vec3(normal.x, normal.y, normal.z));
			}
		}
	}
}

void ImportedMesh::draw()
{
	glUniformMatrix4fv(((LitShader*)shader)->modelID, 1, GL_FALSE, &model[0][0]);
	enableBuffers();
	glBindTexture(GL_TEXTURE_2D, texture);
	glDrawArrays(GL_TRIANGLES, 0, vertices.size());
}*/