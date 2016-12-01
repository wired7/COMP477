#include "Shader.h"

Shader::Shader(const char* vertexPath, const char* fragmentPath)
{
	program = LoadShaders(vertexPath, fragmentPath);
	viewID = glGetUniformLocation(program, "View");
	projectionID = glGetUniformLocation(program, "Projection");
};

GLuint Shader::LoadShaders(const char * vertex_file_path, const char * fragment_file_path) {
	// Create the shaders
	GLuint Vertex2ShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	// Read the Vertex2 Shader code from the file
	std::string Vertex2ShaderCode;
	std::ifstream Vertex2ShaderStream(vertex_file_path, std::ios::in);
	if (Vertex2ShaderStream.is_open()) {
		std::string Line = "";
		while (getline(Vertex2ShaderStream, Line))
			Vertex2ShaderCode += "\n" + Line;
		Vertex2ShaderStream.close();
	}
	else {
		printf("Impossible to open %s. Are you in the right directory ? Don't forget to read the FAQ !\n", vertex_file_path);
		getchar();
		return 0;
	}

	// Read the Fragment Shader code from the file
	std::string FragmentShaderCode;
	std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);
	if (FragmentShaderStream.is_open()) {
		std::string Line = "";
		while (getline(FragmentShaderStream, Line))
			FragmentShaderCode += "\n" + Line;
		FragmentShaderStream.close();
	}

	GLint Result = GL_FALSE;
	int InfoLogLength;


	// Compile Vertex2 Shader
	printf("Compiling shader : %s\n", vertex_file_path);
	char const * Vertex2SourcePointer = Vertex2ShaderCode.c_str();
	glShaderSource(Vertex2ShaderID, 1, &Vertex2SourcePointer, NULL);
	glCompileShader(Vertex2ShaderID);

	// Check Vertex2 Shader
	glGetShaderiv(Vertex2ShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(Vertex2ShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> Vertex2ShaderErrorMessage(InfoLogLength + 1);
		glGetShaderInfoLog(Vertex2ShaderID, InfoLogLength, NULL, &Vertex2ShaderErrorMessage[0]);
		printf("%s\n", &Vertex2ShaderErrorMessage[0]);
	}


	// Compile Fragment Shader
	printf("Compiling shader : %s\n", fragment_file_path);
	char const * FragmentSourcePointer = FragmentShaderCode.c_str();
	glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer, NULL);
	glCompileShader(FragmentShaderID);

	// Check Fragment Shader
	glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> FragmentShaderErrorMessage(InfoLogLength + 1);
		glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
		printf("%s\n", &FragmentShaderErrorMessage[0]);
	}


	// Link the program
	printf("Linking program\n");
	GLuint ProgramID = glCreateProgram();
	glAttachShader(ProgramID, Vertex2ShaderID);
	glAttachShader(ProgramID, FragmentShaderID);
	glLinkProgram(ProgramID);

	// Check the program
	glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
	glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> ProgramErrorMessage(InfoLogLength + 1);
		glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
		printf("%s\n", &ProgramErrorMessage[0]);
	}

	glDetachShader(ProgramID, Vertex2ShaderID);
	glDetachShader(ProgramID, FragmentShaderID);

	glDeleteShader(Vertex2ShaderID);
	glDeleteShader(FragmentShaderID);

	return ProgramID;
};

void Shader::Use()
{
	glUseProgram(program);
};

LitShader* LitShader::shader;

LitShader::LitShader(const char* vertexPath, const char* fragmentPath) : Shader(vertexPath, fragmentPath)
{
	modelID = glGetUniformLocation(program, "Model");

/*	lightSourceID = glGetUniformLocation(Program, "lightSource");
	lightSourceNID = glGetUniformLocation(Program, "nLightSource");
	lightColorID = glGetUniformLocation(Program, "lightColor");
	lightIntensityID = glGetUniformLocation(Program, "lightIntensity");

	specularIntensityID = glGetUniformLocation(Program, "specularIntensity");
	specularColorID = glGetUniformLocation(Program, "specularColor");
	shininessID = glGetUniformLocation(Program, "shininess");
	ambientID = glGetUniformLocation(Program, "ambient");*/
};

InstancedLitShader* InstancedLitShader::shader;

InstancedLitShader::InstancedLitShader(const char* vertexPath, const char* fragmentPath) : Shader(vertexPath, fragmentPath)
{
	scaleID = glGetUniformLocation(program, "Scale");
}

CubeMapShader* CubeMapShader::shader;

CubeMapShader::CubeMapShader(const char* vertexPath, const char* fragmentPath) : Shader(vertexPath, fragmentPath)
{
	cubeMap = glGetUniformLocation(program, "cubemap");
}

GUIShader* GUIShader::shader;

GUIShader::GUIShader(const char* vertexPath, const char* fragmentPath) : Shader(vertexPath, fragmentPath)
{
	modelID = glGetUniformLocation(program, "Model");
}

TextShader* TextShader::shader;

TextShader::TextShader(const char* vertexPath, const char* fragmentPath) : Shader(vertexPath, fragmentPath)
{
	colorID = glGetUniformLocation(program, "textColor");
}