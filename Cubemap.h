#pragma once
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <SOIL.h>
#include <glew.h>
#include <glm.hpp>
#include "Shader.h"

using namespace std;
using namespace glm;

enum CubemapFaces {
  CUBEMAP_RIGHT = GL_TEXTURE_CUBE_MAP_POSITIVE_X,
  CUBEMAP_LEFT = GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
  CUBEMAP_TOP = GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
  CUBEMAP_BOTTOM = GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
  CUBEMAP_BACK = GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
  CUBEMAP_FRONT = GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
};

class Cubemap
{
public:
    /*! */
    Cubemap(string);
    
    /*! */
    virtual ~Cubemap();
    
    /*! */
    void addFace(CubemapFaces, string);
    
    /*! */
    void replaceFace(CubemapFaces, string);
    
    /*! */
    void bind();
    
    /*! */
    virtual void draw();
    
protected:
    std::vector<GLfloat> points;
    GLuint VAO;
    GLuint VBO;

private:
    
    /**< */
    string path;
    
    /**< */
    map<CubemapFaces, string> faces;
    
    /**< */
    GLuint textureId;

};
