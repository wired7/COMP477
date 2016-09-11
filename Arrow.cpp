#include "Arrow.h"

ArrowMesh::ArrowMesh()
{
	this->shader = LitShader::shader;
	
	resolution = 10;
	GLfloat angle = 2 * 3.1415 / resolution;

	addVertex(vec3(2, 0, 0), vec4(0, 0, 1, 1), vec2(), vec3(1, 0, 0));

	for (int i = 0; i <= resolution; i++)
	{
		vec3 pt = rotate(vec3(1, 0.5, 0), angle * i, vec3(1, 0, 0));
		addVertex(pt, vec4(1, 1, 1, 1), vec2(), normalize(pt));
	}

	for (int i = 0; i <= resolution; i++)
	{
		vec3 pt = rotate(vec3(1, 0.2, 0), angle * i, vec3(1, 0, 0));
		addVertex(pt, vec4(1, 1, 1, 1), vec2(), normalize(pt));

		pt = rotate(vec3(0, 0.2, 0), angle * i, vec3(1, 0, 0));
		addVertex(pt, vec4(1, 1, 1, 1), vec2(), normalize(pt));
	}

	bindBuffers();
}

void ArrowMesh::draw(mat4 model)
{
	enableBuffers();
	glDrawArrays(GL_TRIANGLE_FAN, 0, resolution + 2);
	glDrawArrays(GL_TRIANGLE_STRIP, resolution + 2, (resolution + 1) * 2);
}

ArrowMesh* Arrow::mesh = NULL;

Arrow::Arrow(vec3 pos1, vec3 pos2)
{
	if (!mesh)
		mesh = new ArrowMesh();

	update(pos1, pos2);
}


Arrow::~Arrow()
{
}


void Arrow::draw()
{	
	mesh->draw(model);
}

void Arrow::update(vec3 pos1, vec3 pos2)
{
	GLfloat l = length(pos1 - pos2) / 2.0f;
	GLfloat angle = acos(dot(normalize(pos2 - pos1), vec3(1, 0, 0)));
	vec3 c = cross(vec3(1, 0, 0), normalize(pos2 - pos1));
	if (length(c) > 0)
		model = scale(rotate(translate(mat4(1.0f), pos1), angle, c), vec3(l, l, l));
	else
		if (angle == 0)
			model = scale(translate(mat4(1.0f), pos1), vec3(l, l, l));
		else
			model = scale(rotate(translate(mat4(1.0f), pos1), 3.1415f, vec3(0, 1, 0)), vec3(l, l, l));
}