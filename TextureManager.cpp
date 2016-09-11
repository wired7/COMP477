#include "TextureManager.h"
#include <SOIL.h>
#include <fstream>
#include <cassert>
	
#define NDEBUG

#ifndef NDEBUG
#include <iostream>
#endif

TextureManager *TextureManager::self = nullptr;

TextureManager::TextureManager()
{
}

TextureManager::~TextureManager()
{
}

TextureManager *TextureManager::instance() {
    if (TextureManager::self == nullptr)
        TextureManager::self = new TextureManager();
    
    return TextureManager::self;
}

GLint TextureManager::addTexture(std::string filePath, GLfloat shininess) {
    auto exists = this->textures.find(filePath);
    if (exists != this->textures.end()) {
        #ifndef NDEBUG
        std::cout << "Optimizing texture: " << filePath << std::endl;
        #endif
        return exists->second.id;
    }
    else {
        std::ifstream in(filePath, std::ios::in);
    
        #ifndef NDEBUG
        std::cout << "TextureManager::addTexture Opening texture file " << filePath << std::endl;
        #endif
        
        assert(in.is_open());
        
        // We are not opening it with fstream
        in.close();
        
        int width = 0;
        int height = 0;
        
        unsigned char* imageContents = SOIL_load_image(filePath.c_str(), &width, &height, 0, SOIL_LOAD_RGB);
        
        assert(sizeof(imageContents) > 0);
        assert(imageContents != NULL);
        assert(width > 0);
        assert(height > 0);
        
        GLuint texture;
        
        // Generate texture buffer
        glGenTextures(1, &texture);
        
        // Activate texture buffer
        glBindTexture(GL_TEXTURE_2D, texture);
        
        // Send data
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, imageContents);
        
        // Generate minimap
        glGenerateMipmap(GL_TEXTURE_2D);
        
        // Free image from memory
        SOIL_free_image_data(imageContents);
        
        // Unbind buffer
        glBindTexture(GL_TEXTURE_2D, 0);
        
        // Create texture record
        Texture textureRecord;
        textureRecord.id = texture;
        textureRecord.path = filePath;
        textureRecord.shininess = shininess;
        
        this->textures[filePath] = textureRecord;
        
        return texture;
    }
}

GLint TextureManager::addTexture(const char *filePath, GLfloat shininess) {
    return this->addTexture(std::string(filePath), shininess);
}