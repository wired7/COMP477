#pragma once

#include <glew.h>
#include <map>
#include <string>
#include <SOIL.h>
#include <fstream>
#include <cassert>

struct Texture {
	GLuint id;
	GLfloat shininess = 45.0f;
	std::string type;
	std::string path;
};

class TextureManager
{
public:
    virtual ~TextureManager();
    static TextureManager *instance();
    
    GLint addTexture(std::string, GLfloat = 1.0f);
    GLint addTexture(const char*, GLfloat = 1.0f);
    
private:
    TextureManager();
    static TextureManager *self;
    std::map<std::string, Texture> textures;
};
