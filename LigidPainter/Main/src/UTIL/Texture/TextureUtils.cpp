/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

Copyright (c) 2022-2023, Mert Tetik

All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------
*/

#include<glad/glad.h>
#include "LigidGL/LigidGL.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <stb_image.h>

#include <string>
#include <fstream>
#include <iostream>
#include <vector>

#include "UTIL/Util.hpp"

unsigned char* Texture::getTextureDataViaPath(const char* aPath,int &aWidth,int &aHeight,int &aChannels,int desiredChannels,bool flip){
    stbi_set_flip_vertically_on_load(flip);
    unsigned char* data = stbi_load(aPath, &aWidth, &aHeight, &aChannels, desiredChannels);
    
    if(data != NULL){
        std::cout << "Loaded " << aPath << std::endl;
        return data;
    }
    
    else{
        const char* reason = "[unknown reason]";
		if (stbi_failure_reason())
		{
			reason = stbi_failure_reason();
		}
		std::cout << "Failed to load texture! " << aPath << " Reason : " << reason<< std::endl;
    
        stbi_image_free(data);
        
        //Allocate 4 unsigned char memory blocks
        unsigned char* aData = (unsigned char*) malloc(4 * sizeof(unsigned char));
        return aData;
    }
}

void Texture::getData(char*& pixels){
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D,ID);
	glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_BYTE, pixels);
}

glm::ivec2 Texture::getResolution(){
    int w,h;
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D,ID);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &w);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &h);

    return glm::ivec2(w,h);
}

unsigned int Texture::duplicateTexture(){
    unsigned int newTexture;
    //Get the resolution data of the texture
    glActiveTexture(GL_TEXTURE0);
    glGenTextures(1, &newTexture);
    glBindTexture(GL_TEXTURE_2D, ID);
    
    GLint width, height, internalFormat;
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &height);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_INTERNAL_FORMAT, &internalFormat);
    //Create the duplicated texture
    glBindTexture(GL_TEXTURE_2D, newTexture);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);
    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    
    //Copy the texture
    unsigned int FBO;
    glGenFramebuffers(1,&FBO);
    glBindFramebuffer(GL_FRAMEBUFFER,FBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ID, 0);
    glCopyTexImage2D(GL_TEXTURE_2D, 0, internalFormat, 0, 0, width, height, 0);
    glBindFramebuffer(GL_FRAMEBUFFER,0);
    glDeleteFramebuffers(1,&FBO);
    return newTexture;
}

std::vector<glm::vec3> Texture::getMaterialIDPalette(){
    /*
        Valid colors : White, Red, Green, Blue, Pink, Yellow, Orange, Cyan, Black
    */

    //Get the resolution of the texture from the panel of the modifier
    int txtrWidth = getResolution().x;
    int txtrHeight = getResolution().y;

    //Get pixels of the texture
    unsigned char* pixels = new unsigned char[txtrWidth * txtrHeight * 4];
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D,ID);
	glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

    bool detectedWhite = false; 
    bool detectedRed = false; 
    bool detectedGreen = false; 
    bool detectedBlue = false; 
    bool detectedPink = false; 
    bool detectedYellow = false; 
    bool detectedOrange = false; 
    bool detectedCyan = false; 
    bool detectedBlack = false;

    const int precision = 20; // in pixel

    for (size_t i = 0; i < txtrWidth * txtrHeight / precision; i++)
    {
        Color clr;
        clr.loadRGB(glm::vec3(
                                pixels[i * precision * 4], 
                                pixels[i * precision * 4 + 1], 
                                pixels[i * precision * 4 + 2]
                            ));

        glm::vec3 HSV = clr.getHSV();

        float h = HSV.r;
        float s = HSV.g;
        float v = HSV.b;

        // Define ranges for each color
        const double hue_range = 30.0;
        const double sat_range = 0.2;
        const double val_range = 0.2;

        // HSV values for each color
        const double white_h = 0.0;
        const double red_h = 0.0;
        const double green_h = 120.0;
        const double blue_h = 240.0;
        const double pink_h = 300.0;
        const double yellow_h = 60.0;
        const double orange_h = 30.0;
        const double cyan_h = 180.0;

        // Check if the HSV values match any of the colors
        if (s < sat_range && v > (1.0 - val_range)) {
            if (std::abs(h - white_h) <= hue_range)
                 detectedWhite = true;
        } 
        else if (v > (1.0 - val_range)) {
            if (std::abs(h - red_h) <= hue_range)
                 detectedRed = true;
            else if (std::abs(h - green_h) <= hue_range)
                 detectedGreen = true;
            else if (std::abs(h - blue_h) <= hue_range)
                 detectedBlue = true;
        } 
        else if (std::abs(h - pink_h) <= hue_range)
             detectedPink = true;
        else if (std::abs(h - yellow_h) <= hue_range)
             detectedYellow = true;
        else if (std::abs(h - orange_h) <= hue_range)
             detectedOrange = true;
        else if (std::abs(h - cyan_h) <= hue_range)
             detectedCyan = true;

        // If no match black is detected
        detectedBlack = true;
    }

    delete[] pixels;

    std::vector<glm::vec3> res;

    if(detectedWhite)
        res.push_back(glm::vec3(1.f,1.f,1.f));
    if(detectedRed)
        res.push_back(glm::vec3(1.f,0.f,0.f));
    if(detectedGreen)
        res.push_back(glm::vec3(0.f,1.f,0.f));
    if(detectedBlue)
        res.push_back(glm::vec3(0.f,0.f,1.f));
    if(detectedPink)
        res.push_back(glm::vec3(1.f,0.f,1.f));
    if(detectedYellow)
        res.push_back(glm::vec3(1.f,1.f,0.f));
    if(detectedOrange)
        res.push_back(glm::vec3(1.f,0.5f,0.f));
    if(detectedCyan)
        res.push_back(glm::vec3(0.f,1.f,1.f));
    if(detectedBlack)
        res.push_back(glm::vec3(0.f,0.f,0.f));

    return res;
    
}