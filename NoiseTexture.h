#pragma once
// Std. Includes
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include "glm.hpp"

enum noiseType{ STATIC , WOOD , RANDOM ,CLOUDS , MARBLE};
using namespace std;
class NoiseTexture{
public:
    float **noise;
    glm::vec3 **colors;
    

    NoiseTexture(int col , int row ){
        this->col = col;
        this->row = row;
        this->width = col;
        this->height = row;
        
        
        noise = new float*[row];
        colors = new glm::vec3*[row];
        for(int i = 0 ; i< row ; i++){
            noise[i] = new float[col];
            colors[i] = new glm::vec3[col];
        }
        
    }
    
    
    void createHeightMap(int layerCount , noiseType type){
        
        for (int i = 0; i < row ; i++){
            for (int j = 0; j < col; j++)
            {
                noise[i][j] = (rand() % 32768) / 32768.0;
            }
        }
        
        switch(type){
            case STATIC :
            {
                createStatic(layerCount);
                break;
            }
            case CLOUDS:
            {
                createClouds(layerCount);
                break;
            }
        
        }
        
    }
    
    void cleanUp(){
        // clean up
        for(int i = 0 ; i < row ; i++){
            delete [] noise[i];
            delete [] colors[i];
        }
        delete [] noise;
        delete [] colors;
    }
private:
    int width , height;
    int row , col;
    
    
    float layeringTexture(int x, int y, float zoomFactor)
    {
        float value = 0.0, initialZoom = zoomFactor;
        
        while(zoomFactor >= 1)
        {
            value += smoothMagnification(x / zoomFactor, y / zoomFactor) * zoomFactor;
            zoomFactor /= 2.0f;
        }
        
        return ((128 * value) / initialZoom);
    }
    
    double smoothMagnification(float x, float y)
    {
        float xFraction = x - int(x);
        float yFraction = y - int(y);
        // we get the indices of the noise array based on the zoom
        // to insure the indices wrap around, incase out of bound
        int x1 = (int(x)   + width)    % width;
        int y1 = (int(y)   + height)   % height;
        int x2 = (int(x)-1 + width)  % width;
        int y2 = (int(y)-1 + height) % height;
        
        //smooth the noise with bilinear interpolation
        float bilinearInterpolatedVal = 0.0f;
        
        bilinearInterpolatedVal += xFraction       * yFraction         * noise[y1][x1] ;
        bilinearInterpolatedVal += (1 - xFraction) * yFraction         * noise[y1][x2] ;
        bilinearInterpolatedVal += xFraction       * (1 - yFraction)   * noise[y2][x1] ;
        bilinearInterpolatedVal += (1 - xFraction) * (1 - yFraction)   * noise[y2][x2] ;
        
        return bilinearInterpolatedVal;
    }
    
    void createStatic(int layerCount){
        
        
        for (int i = 0; i < row ; i++){
            for (int j = 0; j < col; j++)
            {
                colors[i][j] = glm::vec3(layeringTexture(i, j, layerCount));
            }
        }
        
    }
    
    void createClouds(int layerCount){
        int colorZ;
        for(int i = 0 ; i < row ; i++){
            for(int j = 0; j<col ; j++){
                colorZ = (20 + int(layeringTexture(i,j,layerCount))+255)%255;
                colors[i][j] = glm::vec3(100,100,colorZ);
            }
        }
        
        
    }
    

    
};


