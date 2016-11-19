#include "Terrain.h"



Terrain::Terrain(int cellSize, int width, int height, int noiseLayerCount, noiseType type)
{
	this->shader = LitShader::shader;
	createMesh(vec2(0,0), cellSize, width, height, noiseLayerCount, type);
	this->setUpMesh();
	bindBuffers();
	model = translate(mat4(), -vec3(width / 2, 0, height / 2));
}


Terrain::~Terrain()
{
}


void Terrain::draw() {
	// then we need to set the value of the uniforms in the shader
	enableBuffers();
	glBindTexture(GL_TEXTURE_2D, texture);

	glUniformMatrix4fv(LitShader::shader->modelID, 1, GL_FALSE, &(model[0][0]));

	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}


/*   Functions   */
/* this function is for setting up the mesh buffers */
/* ------------------------------------------------ */
void Terrain::setUpMesh() {
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture); // All upcoming GL_TEXTURE_2D operations now have effect on this texture object
										   // Set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// Set texture wrapping to GL_REPEAT (usually basic wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// Set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// Load image, create texture and generate mipmaps
	int width, height;
	unsigned char* image = SOIL_load_image("textures\\blank.jpg", &width, &height, 0, SOIL_LOAD_RGB);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);
	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0); // Unbind texture when done, so we won't accidentily mess up our texture.
}
void Terrain::setHeights(int row, int col, NoiseTexture noiseText) {

	auto temp = new float*[row];
	heights = new float*[row];
	for (int i = 0; i< row; i++) {
		temp[i] = new float[col];
		heights[i] = new float[col];
	}

	for (int i = 0; i < row; i++) {
		for (int j = 0; j < col; j++) {
			float z = map(noiseText.colors[j][i].z, 0, 255, 0, UPPER_RANGE);
			//heights[i][j] = noiseText.noise[i][j];
//			heights[i][j] = 0;
			temp[i][j] = z;
		}
	}

	int s = 4;

	GLfloat t;
	for (int i = 0; i < row; i++)
		for (int j = 0; j < col; j++)
		{	
			heights[i][j] = 0;		
			vec2 p(i - row / 2, j - col / 2);
			int s2 = pow(pow(p.x, 10) + pow(p.y, 10), 0.5) / 64000;
			int count = 0;
			for (int k = glm::max(0, i - s2); k <= glm::min(row - 1, i + s2); k++)
				for (int n = glm::max(0, j - s2); n <= glm::min(col - 1, j + s2); n++, count++)
					heights[i][j] += temp[k][n];

			heights[i][j] /= count;
			if (!i && !j)
				t = heights[0][0];
			heights[i][j] -= t;
		}

	for (int i = 0; i < row; i++)
	{
		delete temp[i];
	}
}
/* this is a recursive function that populates the vertices and the indices vectors */
/* -------------------------------------------------------------------------------- */
void Terrain::createMesh(glm::vec2 cornerPosition, int cellSize, int width, int height, int noiseLayerCount, noiseType type) {
	// to color the terrain
	int col = width / cellSize;
	int row = height / cellSize;
	NoiseTexture noiseText(col, row);
	noiseText.createHeightMap(noiseLayerCount, type);
	glm::vec2 textureCorner = glm::vec2(0.0, 0.0);

	float textureCellSize = 1 / (float)col;
	setHeights(row, col, noiseText);
	glm::vec2 cornerPositionCol;
	glm::vec2 cornerPositionRow;

	glm::vec2 cornerTextureRow;
	glm::vec2 cornerTexturecol;

	Vertex2 vertex_0;
	Vertex2 vertex_1;
	Vertex2 vertex_2;
	Vertex2 vertex_3;

	GLuint indicesCounter = 0;

	cornerPositionRow = glm::vec2(cornerPosition.x, cornerPosition.y);
	cornerTextureRow = glm::vec2(textureCorner.x, textureCorner.y);
	for (int i = 0; i < row - 1; i++) {
		cornerPositionCol = glm::vec2(cornerPositionRow.x, cornerPositionRow.y);
		cornerTexturecol = glm::vec2(cornerTextureRow.x, cornerTextureRow.y);
		for (int j = 0; j < col - 1; j++) {
			vertex_0.position = glm::vec3(cornerPositionCol.x, heights[i][j], cornerPositionCol.y);
			vertex_0.texCoord = glm::vec2(cornerTexturecol.x, cornerTexturecol.y);

			vertex_1.position = glm::vec3(cornerPositionCol.x + cellSize, heights[i][j + 1], cornerPositionCol.y);
			vertex_1.texCoord = glm::vec2(cornerTexturecol.x + textureCellSize, cornerTexturecol.y);

			vertex_2.position = glm::vec3(cornerPositionCol.x, heights[i + 1][j], cornerPositionCol.y + cellSize);
			vertex_2.texCoord = glm::vec2(cornerTexturecol.x, cornerTexturecol.y + textureCellSize);

			vertex_3.position = glm::vec3(cornerPositionCol.x + cellSize, heights[i + 1][j + 1], cornerPositionCol.y + cellSize);
			vertex_3.texCoord = glm::vec2(cornerTexturecol.x + textureCellSize, cornerTexturecol.y + textureCellSize);

			addToVertices(vertex_0, indicesCounter, vec3((GLfloat)rand() * 0.5 / RAND_MAX, 1, (GLfloat)rand() * 0.3 / RAND_MAX));
			addToVertices(vertex_1, indicesCounter, vec3((GLfloat)rand() * 0.5 / RAND_MAX, 1, (GLfloat)rand() * 0.3 / RAND_MAX));
			addToVertices(vertex_2, indicesCounter, vec3((GLfloat)rand() * 0.5 / RAND_MAX, 1, (GLfloat)rand() * 0.3 / RAND_MAX));
			addToVertices(vertex_1, indicesCounter, vec3((GLfloat)rand() * 0.5 / RAND_MAX, 1, (GLfloat)rand() * 0.3 / RAND_MAX));
			addToVertices(vertex_2, indicesCounter, vec3((GLfloat)rand() * 0.5 / RAND_MAX, 1, (GLfloat)rand() * 0.3 / RAND_MAX));
			addToVertices(vertex_3, indicesCounter, vec3((GLfloat)rand() * 0.5 / RAND_MAX, 1, (GLfloat)rand() * 0.3 / RAND_MAX));


			cornerPositionCol = glm::vec2(cornerPositionCol.x + cellSize, cornerPositionCol.y);
			cornerTexturecol = glm::vec2(cornerTexturecol.x + textureCellSize, cornerTexturecol.y);
		}
		cornerPositionRow = glm::vec2(cornerPositionRow.x, cornerPositionRow.y + cellSize);
		cornerTextureRow = glm::vec2(cornerTextureRow.x, cornerTextureRow.y + textureCellSize);
	}
	// clean up
	for (int i = 0; i < row; i++) {
		delete[] heights[i];
	}
	delete[] heights;
	noiseText.cleanUp();

}


void Terrain::addToVertices(Vertex2 newVertex, GLuint &indicesCount, vec3 color) {
	int index; // to keep track of the ids in the vertices vector
	if (!search_vertices(vertices, newVertex.position, index)) {
		newVertex.id = indicesCount;
		newVertex.color = vec4(color, 1);
		index = newVertex.id;
		this->vertices.push_back(newVertex);
		indicesCount++;
	}
	this->indices.push_back(index);

}


/* this function returns true if the vertex exist */
/* ---------------------------------------------- */
bool Terrain::search_vertices(std::vector<Vertex2> vertices, glm::vec3 position, int &index) {
	bool flag = false;
	int size = (int)vertices.size();
	for (int i = 0; i < size; i++) {
		if (AreSame(position.x, this->vertices[i].position.x) &&
			AreSame(position.z, this->vertices[i].position.z)) {
			index = i;
			flag = true;
			return flag;
		}
	}
	return flag;
}


/* for comparing to float values */
/* ----------------------------- */
bool Terrain::AreSame(GLfloat a, GLfloat b) {
	return (fabs(a - b) < DBL_EPSILON);
}

/* this function is for creating a random value between -1,1*/
float Terrain::getRandomNumber(float min, float max) {
	return min + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (max - min)));
}

float Terrain::map(float val, float prevMin, float prevMax, float min, float max) {
	float prevLength = prevMax - prevMin;
	float length = max - min;
	if (val > prevMin && val < prevMax) {
		float prevDistance = val - prevMin;
		float distance = (prevDistance * length) / prevLength;
		return min + distance;
	}
	else {
		std::cout << "wrong value" << std::endl;
		return 0.0f;
	}

}