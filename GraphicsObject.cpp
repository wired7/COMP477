#include "GraphicsObject.h"
#include "Camera.h"
#include "InputState.h"
#include "Splines.h"

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

void MeshObject::applyTransform()
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

Bounds MeshObject::getBounds()
{
	Bounds b;

	for (int j = 0; j < vertices.size(); ++j)
	{
		vec3 pos(model * vec4(vertices[j].position, 1));

		for (int i = 0; i < 3; i++)
		{
			if (pos[i] > b.max[i])
			{
				b.max[i] = pos[i];
			}
			if (pos[i] < b.min[i])
			{
				b.min[i] = pos[i];
			}
		}
	}

	return b;
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


Polyhedron::Polyhedron(int resolution, vec3 pos, vec3 radii, vec4 color, bool isRendered)
{
	shader = LitShader::shader;
	
	loadTexture("textures\\blank.jpg");
	model = scale(translate(mat4(1.0f), pos), radii);
	this->resolution = resolution;
	double theta = 2 * 3.1415 / resolution;
	double phi = 3.1415 / resolution;
	vector<vector<vec3>> circles;
	vec3 start(0, -0.5f, 0);
	for (int j = 1; j < resolution; j++)
	{
		vec3 temp = rotate(start, (GLfloat)(phi * j), vec3(0, 0, 1));
		circles.push_back(vector<vec3>());
		for (int i = 0; i < resolution; i++)
			circles[circles.size() - 1].push_back(rotate(temp, (GLfloat)(theta * i), vec3(0, 1, 0)));
	}

	for (int j = 0; j < circles.size(); j++)
	{
		for (int i = 0; i < circles[j].size(); i++)
		{
			addVertex(circles[j][i], color, vec2(i, 1) / (GLfloat)resolution, normalize(circles[j][i]));

			if (j > 0 && i > 0)
			{
				indices.push_back(vertices.size() - 2 - circles[j].size());
				indices.push_back(vertices.size() - 2);
				indices.push_back(vertices.size() - 1 - circles[j].size());

				indices.push_back(vertices.size() - 2);
				indices.push_back(vertices.size() - 1);
				indices.push_back(vertices.size() - 1 - circles[j].size());
			}
		}

		if (j > 0)
		{
			indices.push_back(vertices.size() - 1 - circles[j].size());
			indices.push_back(vertices.size() - 1);
			indices.push_back(vertices.size() - 2 * circles[j].size());

			indices.push_back(vertices.size() - 1);
			indices.push_back(vertices.size() - 2 * circles[j].size());
			indices.push_back(vertices.size() - circles[j].size());
		}
	}

	addVertex(start, color, vec2(0, 1) / (GLfloat)resolution, start);
	addVertex(-start, color, vec2(0, 1) / (GLfloat)resolution, -start);

	for (int i = 0; i < resolution; i++)
	{
		indices.push_back(i);
		indices.push_back((i + 1) % resolution);
		indices.push_back(vertices.size() - 2);

		indices.push_back(vertices.size() - 2 - resolution + i);
		indices.push_back(vertices.size() - resolution + (i + 1) % resolution - 2);
		indices.push_back(vertices.size() - 1);
	}

	if(isRendered)
		bindBuffers();
}

void Polyhedron::draw(void)
{
	enableBuffers();
	glUniformMatrix4fv(((LitShader*)shader)->modelID, 1, GL_FALSE, &model[0][0]);
	glBindTexture(GL_TEXTURE_2D, texture);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
}

Polyhedron* InstancedSpheres::sphere;

InstancedSpheres::InstancedSpheres(float radius, int resolution, vec4 color, vector<vec3> positions)
{
	shader = InstancedLitShader::shader;
	sphere = new Polyhedron(resolution, vec3(radius, radius, radius), vec3(0, 0, 0), color, true);
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
	glBindVertexArray(VAO);
	glGenBuffers(1, &positionsVertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, positionsVertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(vec3), &(positions[0]), GL_DYNAMIC_DRAW);
	// Vertex Attributes
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), (GLvoid*)0);
	glVertexAttribDivisor(4, 1);

	glBindVertexArray(0);

}

void InstancedSpheres::updateInstances()
{
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, positionsVertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(vec3), &(positions[0]), GL_DYNAMIC_DRAW);

	glBindVertexArray(0);
}

void InstancedSpheres::updateInstances(vector<vec3>* positions)
{
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, positionsVertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, positions->size() * sizeof(vec3), &((*positions)[0]), GL_DYNAMIC_DRAW);

	glBindVertexArray(0);

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

float NewtonsMethod(float y, vec3 u, vec3 r, vec3 v, int depth)
{
	if (depth == 0)
		return y;
	
	float x = NewtonsMethod(y, u, r, v, depth - 1);

	return x - sqrt(-v.z + r.z * cos(x) + u.z * sin(x) + pow(-v.x + r.x * cos(x) + u.x * sin(x), 2) + 
					pow(-v.y + r.y * cos(x) + u.y * sin(x), 2)) / (u.z * cos(x) - r.z * sin(x) + 2 * (u.x * cos(x) - r.x * sin(x)) *
					(-v.x + r.x * cos(x) + u.x * sin(x)) + 2 * (u.y * cos(x) - r.y * sin(x)) * (-v.y + r.y* cos(x) + u.y * sin(x)));
}

Cylinder::Cylinder(float topRadius, bool openTop, float bottomRadius, bool openBottom, vec2 resolution, vector<vec3> samplePoints, vec4 color, bool render)
{
	shader = LitShader::shader;
	loadTexture("textures\\blank.jpg");

	vector<vec3> params;
	vector<vec3> points;

	if (samplePoints.size() > 2)
	{
		for (int i = 0; i < samplePoints.size() - 1; i++)
		{
			params.clear();
			params.push_back(samplePoints[i]);
			params.push_back(samplePoints[i + 1]);

			if (i < samplePoints.size() - 2)
			{
				if (i > 0)
				{
					params.push_back((samplePoints[i + 1] - samplePoints[i - 1]) / 2.0f);
					params.push_back((samplePoints[i + 2] - samplePoints[i]) / 2.0f);
				}
				else
				{
					params.push_back(samplePoints[i + 1] - samplePoints[i]);
					params.push_back(samplePoints[i + 2] - samplePoints[i + 1]);
				}
			}
			else if (i < samplePoints.size() - 1)
			{
				params.push_back((samplePoints[i + 1] - samplePoints[i - 1]) / 2.0f);
				params.push_back((samplePoints[i + 1] - samplePoints[i - 1]) / 2.0f);
			}
			else
			{
				params.push_back(samplePoints[i] - samplePoints[i - 1]);
				params.push_back(samplePoints[i] - samplePoints[i - 1]);
			}

			points.push_back(params[0]);

			Splines::subdivide(0, 1, 0.05f, &params, &points);
		}

		points.push_back(params[1]);
	}
	else
		points = samplePoints;

	float radialStep = 2 * 3.1415f / resolution.x;

	vec3 v;
	vec3 diff;
	for (int i = 0; i < points.size(); i++)
	{
		float currentRadius = topRadius + (bottomRadius - topRadius) * (float)i / (float)(points.size() - 1);

		if (i < points.size() - 1)
		{
			if (i > 0)
				diff = normalize(normalize(points[i + 1] - points[i]) + normalize(points[i] - points[i - 1]));
			else
				diff = normalize(points[i + 1] - points[i]);
		}

		vec3 vec[] = { { (-diff.y - diff.z) / diff.x, 1, 1 },{ 1, (-diff.x - diff.z) / diff.y, 1 },{ 1, 1, (-diff.y - diff.x) / diff.z } };

		vec3 pt1;

			if (diff.x != 0)
				pt1 = normalize(vec[0]);
			else if (diff.y != 0)
				pt1 = normalize(vec[1]);
			else if (diff.z != 0)
				pt1 = normalize(vec[2]);
			else
				pt1 = vec3(0, 0, 0);
		

		v = pt1;

		for (int j = 0; j < resolution.x; j++)
		{
			vec3 pos = vec3(scale(mat4(1.0f), currentRadius * vec3(1.0f, 1.0f, 1.0f)) * rotate(mat4(1.0f), radialStep * j, diff) * vec4(pt1, 1.0f)) + points[i];
			vec3 normal = normalize(pos - points[i]);
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

		if (i > 0)
		{
			indices.push_back(vertices.size() - 1 - resolution.x);
			indices.push_back(vertices.size() - 1);
			indices.push_back(vertices.size() - 2 * resolution.x);

			indices.push_back(vertices.size() - 1);
			indices.push_back(vertices.size() - 2 * resolution.x);
			indices.push_back(vertices.size() - resolution.x);
		}
	}

	int offset = 0;

	if (openTop)
	{
		addVertex(samplePoints[0], color, vec2(), normalize(samplePoints[1] - samplePoints[0]));

		for (int i = 0; i < resolution.x; i++)
		{
			indices.push_back(vertices.size() - 1);
			indices.push_back(i);
			indices.push_back((i + 1) % (int)resolution.x);
		}

		offset = 1;
	}

	if (openBottom)
	{
		addVertex(samplePoints[samplePoints.size() - 1], color, vec2(), normalize(samplePoints[samplePoints.size() - 1] - samplePoints[samplePoints.size() - 2]));

		for (int i = 0; i < resolution.x; i++)
		{
			indices.push_back(vertices.size() - 1);
			indices.push_back(vertices.size() - 2 - offset - (i + 1) % (int)resolution.x);
			indices.push_back(vertices.size() - 2 - offset - i);
		}
	}

	if (render)
		bindBuffers();
}

void Cylinder::draw() {
	enableBuffers();
	glUniformMatrix4fv(((LitShader*)shader)->modelID, 1, GL_FALSE, &model[0][0]);
	glBindTexture(GL_TEXTURE_2D, texture);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
};

Rectangle::Rectangle(vec3 position, vec3 dimensions, vec4 color, bool isRendered)
{
	shader = LitShader::shader;
	loadTexture("textures\\blank.jpg");

	addVertex(vec3(0, 0, 0), color, vec2(), vec3(0, 0, -1));
	addVertex(vec3(0, 1, 0), color, vec2(), vec3(0, 0, -1));
	addVertex(vec3(1, 0, 0), color, vec2(), vec3(0, 0, -1));
	addVertex(vec3(1, 1, 0), color, vec2(), vec3(0, 0, -1));

	indices = {0, 1, 2, 1, 2, 3};

	model = translate(mat4(1.0f), position) * scale(mat4(1.0f), dimensions);

	if(isRendered)
		bindBuffers();
}

void Rectangle::draw()
{
	enableBuffers();
	glUniformMatrix4fv(((LitShader*)shader)->modelID, 1, GL_FALSE, &model[0][0]);
	glBindTexture(GL_TEXTURE_2D, texture);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
}

Cube::Cube(vec3 center, vec3 dimensions, vec4 color, bool isRendered)
{
	shader = LitShader::shader;
	loadTexture("textures\\blank.jpg");

	for(int i = 0; i < 2; i++)
		for(int j = 0; j < 2; j++)
			for(int k = 0; k < 2; k++)
				addVertex(0.5f * vec3(i * 2 - 1, j * 2 - 1, k * 2 - 1), color, vec2(), normalize(0.5f * vec3(i * 2 - 1, j * 2 - 1, k * 2 - 1) - center));

	indices = {0, 1, 2, 1, 3, 2, 0, 2, 4, 2, 6, 4, 4, 6, 5, 6, 7, 5, 5, 7, 1, 7, 3, 1, 1, 0, 4, 1, 4, 5, 2, 3, 6, 3, 7, 6};

	model = translate(mat4(1.0f), center) * scale(mat4(1.0f), dimensions);

	if(isRendered)
		bindBuffers();
}

void Cube::draw()
{
	enableBuffers();
	glUniformMatrix4fv(((LitShader*)shader)->modelID, 1, GL_FALSE, &model[0][0]);
	glBindTexture(GL_TEXTURE_2D, texture);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
}

GUIButton::GUIButton(vec3 position, vec3 dimensions, vec4 color, char* text, vec4 textColor, char* filePath, bool isRendered, std::function<void()> clickEvent, int FontSize)
{
	shader = GUIShader::shader;
	loadTexture(filePath);

	addVertex(vec3(0, 0, 0), color, vec2(0, 0), vec3(0, 0, -1));
	addVertex(vec3(0, 1, 0), color, vec2(0, 1), vec3(0, 0, -1));
	addVertex(vec3(1, 0, 0), color, vec2(1, 0), vec3(0, 0, -1));
	addVertex(vec3(1, 1, 0), color, vec2(1, 1), vec3(0, 0, -1));

	indices = { 0, 1, 2, 1, 2, 3 };

	//center button
	position -= (dimensions / 2.0f);

	this->position = position;
	this->dimensions = dimensions;
	this->clickEvent = clickEvent;
	this->text = text;
	this->textColor = textColor;
	this->fontSize = FontSize;

	model = translate(mat4(1.0f), position) * scale(mat4(1.0f), dimensions);

	if (isRendered)
		bindBuffers();
}

void GUIButton::draw()
{
	GUIShader::shader->Use();

	glUniformMatrix4fv(((GUIShader*)shader)->modelID, 1, GL_FALSE, &model[0][0]);
	glUniformMatrix4fv(GUIShader::shader->projectionID, 1, GL_FALSE, &Camera::activeCamera->OrthoProjection[0][0]);
	glUniform1i(GUIShader::shader->texID, 0);
	glUniform4f(GUIShader::shader->hoverID, hoverColor.x, hoverColor.y, hoverColor.z, hoverColor.w);

	//Draw Rectangle
	enableBuffers();
	glBindTexture(GL_TEXTURE_2D, texture);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

	//Draw text
	textRend.RenderText(text, position.x, position.y + dimensions.y / 2.0f, 1.0f, glm::vec3(textColor.x, textColor.y, textColor.z) , Camera::activeCamera->getScreenWidth(), Camera::activeCamera->getScreenHeight(), fontSize);
}

glm::vec2 GUIButton::calcScale()
{
	int ORIGINAL_RES_X = 1200;
	int ORIGINAL_RES_Y = 800;

	int scaleX = Camera::activeCamera->getScreenWidth() / ORIGINAL_RES_X;
	int scaleY = Camera::activeCamera->getScreenHeight() / ORIGINAL_RES_Y;

	vec2 scale = vec2(scaleX, scaleY);

	return scale;

}

bool GUIButton::checkMouseClick()
{
	if (isPointInRect(InputState::mouseGuiCoords.x, InputState::mouseGuiCoords.y))
	{
		clickEvent();
		return true;
	}

	return false;
}

void GUIButton::checkHover() {
	if (isPointInRect(InputState::mouseGuiCoords.x, InputState::mouseGuiCoords.y))
	{
		hoverColor = glm::vec4(0.7f, 0.7f, 0.7f, 1.0f);
	}
	else
		hoverColor = glm::vec4(1.0f);
}

bool GUIButton::isPointInRect(double x, double y)
{
	if (x > position.x && x < position.x + dimensions.x && y > position.y && y < position.y + dimensions.y)
		return true;
	else
		return false;
}

char* GUIButton::getText()
{
	return text;
}

GUIButtonValued::GUIButtonValued(vec3 position, vec3 dimensions, vec4 color, char* text, vec4 textColor, char* filePath, bool isRendered, std::function<void(float&)> clickEvent, float& ValueToChange, int FontSize) : valueToChange(ValueToChange)
{
	shader = GUIShader::shader;
	loadTexture(filePath);

	addVertex(vec3(0, 0, 0), color, vec2(0, 0), vec3(0, 0, -1));
	addVertex(vec3(0, 1, 0), color, vec2(0, 1), vec3(0, 0, -1));
	addVertex(vec3(1, 0, 0), color, vec2(1, 0), vec3(0, 0, -1));
	addVertex(vec3(1, 1, 0), color, vec2(1, 1), vec3(0, 0, -1));

	indices = { 0, 1, 2, 1, 2, 3 };

	//center button
	position -= (dimensions / 2.0f);

	this->position = position;
	this->dimensions = dimensions;
	this->clickEvent = clickEvent;
	this->text = text;
	this->textColor = textColor;
	this->fontSize = FontSize;

	model = translate(mat4(1.0f), position) * scale(mat4(1.0f), dimensions);

	if (isRendered)
		bindBuffers();
}
bool GUIButtonValued::checkMouseClick()
{
	if (isPointInRect(InputState::mouseGuiCoords.x, InputState::mouseGuiCoords.y))
	{
		clickEvent(valueToChange);
		return true;
	}

	else
		return false;
}

GUIBackground::GUIBackground(vec3 position, vec3 dimensions, vec4 color, char* texturePath, bool isRendered)
{
	shader = UnlitShader::shader;
	loadTexture(texturePath);

	this->position = position;

	addVertex(vec3(0, 0, 0), color, vec2(0,0), vec3(0, 0, -1));
	addVertex(vec3(0, 1, 0), color, vec2(0,1), vec3(0, 0, -1));
	addVertex(vec3(1, 0, 0), color, vec2(1,0), vec3(0, 0, -1));
	addVertex(vec3(1, 1, 0), color, vec2(1,1), vec3(0, 0, -1));

	model = translate(mat4(1.0f), position) * scale(mat4(1.0f), dimensions);

	indices = { 0, 1, 2, 1, 2, 3 };

	if (isRendered)
		bindBuffers();

}
void GUIBackground::draw()
{
	UnlitShader::shader->Use();

	glUniformMatrix4fv(((UnlitShader*)shader)->modelID, 1, GL_FALSE, &model[0][0]);
	glUniformMatrix4fv(UnlitShader::shader->projectionID, 1, GL_FALSE, &Camera::activeCamera->OrthoProjection[0][0]);
	glUniform1i(UnlitShader::shader->texID, 0);

	//Draw Rectangle
	enableBuffers();
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