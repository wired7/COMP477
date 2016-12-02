#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <sstream>
#include <glew.h>
#include <glm.hpp>

using namespace std;
using namespace glm;

class Shader
{
public:
	GLuint program;
	GLuint viewID;
	GLuint projectionID;
	Shader() {};
	Shader(const char* vertexPath, const char* fragmentPath);
	virtual GLuint LoadShaders(const char * vertex_file_path, const char * fragment_file_path);
	virtual void Use();
};

class LitShader : public Shader
{
public:
	static LitShader* shader;
//	GLuint lightSourceID, lightSourceNID, lightColorID, lightIntensityID;
//	GLuint specularIntensityID, specularColorID, shininessID, ambientID;
	GLuint modelID;
	LitShader() {};
	LitShader(const char* vertexPath, const char* fragmentPath);
};

class InstancedLitShader : public Shader
{
public:
	static InstancedLitShader* shader;
	GLuint scaleID;
	InstancedLitShader() {};
	InstancedLitShader(const char* vertexPath, const char* fragmentPath);
};

class CubeMapShader : public Shader
{
public:
	static CubeMapShader* shader;
	GLuint cubeMap;
	CubeMapShader() {};
	CubeMapShader(const char* vertexPath, const char* fragmentPath);
};

class GUIShader : public Shader
{
public:
	static GUIShader* shader;
	GLuint modelID;
	GLuint texID;
	GLuint hoverID;
	GUIShader() {};
	GUIShader(const char* vertexPath, const char* fragmentPath);
};

class TextShader : public Shader
{
public:
	static TextShader* shader;
	GLuint colorID; //uniform reprensenting the text color
	GLuint text;
	TextShader() {};
	TextShader(const char* vertexPath, const char* fragmentPath);
};

class UnlitShader : public Shader
{
public:
	GLuint texID;
	GLuint modelID;
	static UnlitShader* shader;
	GLuint colorID; //uniform reprensenting the text color
	UnlitShader() {};
	UnlitShader(const char* vertexPath, const char* fragmentPath);
};