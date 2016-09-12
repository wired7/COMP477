#include "Skybox.h"
#include <iostream>

/**
 * @brief Sets up the skybpx
 * @param shaders POINTER TO THE SHADERS CLASS. NOTE THE NAMESPACE Lev::Shaders ..... Lev:: is an accesso for the namespace.
 * @param path PATH WHERE THE SKYBOX IMAGES ARE LOCATED. DON'T FORGET THE TRAILING SLASH: 
 *        \\path\\where\\they\\are\\   <--- trailing slash = slash at the end of the path
 */
Skybox::Skybox(string path) : Cubemap(path) {
    GLfloat skyboxVertices[] = {
        // Positions          
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
  
        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,
  
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
   
        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,
  
        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,
  
        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };
    
    
    // TO CHANGE THE SIZE OF THE SKYBOX, LOOK IN BELOW
    
    
    // CHANGE THE NAMES OF THE FILES TO MATCH YOUR TEXTURE FILES OR RENAME YOUR TEXTURE FILES TO MATCH THESE NAMES
    this->addFace(CUBEMAP_RIGHT, "right.jpg");
    this->addFace(CUBEMAP_LEFT, "left.jpg");
    this->addFace(CUBEMAP_TOP, "top.jpg");
    this->addFace(CUBEMAP_BOTTOM, "bottom.jpg");
    this->addFace(CUBEMAP_BACK, "back.jpg");
    this->addFace(CUBEMAP_FRONT, "front.jpg");
    
    /**
     * 
     * THIS IS WHERE YOU CAN ADJUST HOW BIG THE SKYBOX IS. MULTIPLY THE VERTEX COORDINATES BY WHATEVER VALUE YOU WANT
     * IT'LL BECOME THAT VALUE (1.0f * x = x)
     * 
     */
    
    // HERE IS THE SKYBOX SIZE. CHANGE IT HERE.
    const GLuint SKYBOX_SIZE = 10;
    
    
    for (unsigned int i = 0; i < sizeof(skyboxVertices) / sizeof(GLfloat); i++) {
        this->points.push_back(skyboxVertices[i] * SKYBOX_SIZE);
    }
}

Skybox::~Skybox() {
}

