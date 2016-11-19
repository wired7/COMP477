#include "Cubemap.h"

/**
 * @brief 
 * @param path
 * @return 
 */
Cubemap::Cubemap(string path) {
    this->path = path;
    
    glGenVertexArrays(1, &this->VAO);
    glGenBuffers(1, &this->VBO);
    
    glGenTextures(1, &this->textureId);
}

/**
 * @brief 
 * @return 
 */
Cubemap::~Cubemap() {
    glDeleteTextures(1, &this->textureId);
    
    glDeleteVertexArrays(1, &this->VAO);
    glDeleteBuffers(1, &this->VBO);
}




/**
 * @brief 
 * @param face
 * @param filename
 */
void Cubemap::addFace(CubemapFaces face, std::string filename) {
    this->faces[face] = filename;
}

/**
 * @brief 
 * @param face
 * @param filename
 */
void Cubemap::replaceFace(CubemapFaces face, std::string filename) {
    
    // Make sure we are not using this function to load a fresh cubemap.
    assert (this->faces.size() == 6);
    
    // Check the file exists
    std::string filePath = this->path + filename;
    
    std::ifstream in(filePath, std::ios::in);
    assert(in.is_open());
    in.close();
    
    // Load the texture
    glBindTexture(GL_TEXTURE_CUBE_MAP, this->textureId);
    
    // Image size
    int width = 0, height = 0;
    
    #ifndef NDEBUG
    std::cout << "Loading cubemap texture " << filename << " to replace face " << face << std::endl;
    #endif
    
    // Load image with SOIL
    unsigned char* imageData = SOIL_load_image(filePath.c_str(), &width, &height, 0, SOIL_LOAD_RGB);
    
    // Make sure it loaded
    assert(imageData != NULL);
    assert (width > 0);
    assert (height > 0);
    
    // Buffer to GPU
    glTexImage2D(face, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, imageData);
    
    // Free image data from memory
    SOIL_free_image_data(imageData);
    
    // Unbind texture
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}


/**
 * @brief 
 */
void Cubemap::bind() {
    
    /*
     * Load points
     */
    assert(this->points.size() >= 6);
    
    glBindVertexArray(this->VAO);
    glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
    
    glBufferData(GL_ARRAY_BUFFER, sizeof(this->points[0]) * this->points.size(), &this->points[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(this->points[0]) * 3, (GLvoid*)0);
    glEnableVertexAttribArray(0);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    
    // Always have to have 6 faces
    assert(this->faces.size() == 6);
    
    /*
     * Load textures
     */
    glBindTexture(GL_TEXTURE_CUBE_MAP, this->textureId);
    
    for (auto face : this->faces) {
        
        #ifndef NDEBUG
        std::cout << "Loading cubemap texture: " << face.second << " for face: " << face.first << std::endl;
        #endif
        
        int width = 0;
        int height = 0;
        
        std::cout << "Path: " << this->path << std::endl << "File: " << face.second << std::endl;
        std::string filePath = this->path + face.second;
        const char* filePath_cstr = filePath.c_str();
        
        std::cout << "Trying to open path " << filePath << std::endl;
        
        /*
         * Make sure the texture file exists
         */
        std::ifstream in(filePath, std::ios::in);
        
        assert(in.is_open());
        
        // We are not reading the data using fstream
        in.close();
        
        // Load the data using SOIL
        unsigned char* imageData = SOIL_load_image(filePath_cstr, &width, &height, 0, SOIL_LOAD_RGB);
        
        assert(imageData != NULL);
        assert(width > 0);
        assert(height > 0);
        
        glTexImage2D(face.first, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, imageData);
        
        SOIL_free_image_data(imageData);
    }
    
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

void Cubemap::draw() {
    glDepthMask(GL_FALSE);
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, this->textureId);
        
    glBindVertexArray(this->VAO);
    glDrawArrays(GL_TRIANGLES, 0, this->points.size() / 3);
    glBindVertexArray(0);
    
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    glDepthMask(GL_TRUE);
}

