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

#include <glad/glad.h>
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
#include "3D/ThreeD.hpp"
#include "ShaderSystem/Shader.hpp"
#include "GUI/GUI.hpp"

unsigned char* Texture::getTextureDataViaPath(const char* aPath,int &aWidth,int &aHeight,int &aChannels,int desiredChannels,bool flip){
    stbi_set_flip_vertically_on_load(flip);
    unsigned char* data = stbi_load(aPath, &aWidth, &aHeight, &aChannels, desiredChannels);
    
    if(data != NULL){
        LGDLOG::start<< "Loaded " << aPath << LGDLOG::end;
        return data;
    }
    
    else{
        const char* reason = "[unknown reason]";
		if (stbi_failure_reason())
		{
			reason = stbi_failure_reason();
		}
		LGDLOG::start<< "Failed to load texture! " << aPath << " Reason : " << reason<< LGDLOG::end;
    
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
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_MIRRORED_REPEAT);
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

    int detectedWhite = false; 
    int detectedRed = false; 
    int detectedGreen = false; 
    int detectedBlue = false; 
    int detectedPink = false; 
    int detectedYellow = false; 
    int detectedOrange = false; 
    int detectedCyan = false; 
    int detectedBlack = false;

    const int precision = 1; // in pixel

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
        float s = HSV.g * 100.f;
        float v = HSV.b;

        // Define ranges for each color
        const double hue_range = 5.0;

        if(s < 5 && v > 90)
            detectedWhite++;
        else if(s < 5 && v < 10)
            detectedBlack++;
        else{
            if(h < 15 || h > 345)
                detectedRed++;
            else if(h > 265 && h <= 345)
                detectedPink++;
            else if(h > 205 && h <= 265)
                detectedBlue++;
            else if(h > 175 && h <= 205)
                detectedCyan++;
            else if(h > 70 && h <= 175)
                detectedGreen++;
            else if(h > 55 && h <= 70) 
                detectedYellow++;
            else 
                detectedOrange++;
        } 
    }

    delete[] pixels;

    std::vector<glm::vec3> res;

    // To be accepted the color has to occupy at least 0.5% of the texture

    if(detectedWhite > (txtrWidth * txtrHeight) / 100.f * 0.5f)
        res.push_back(glm::vec3(1.f,1.f,1.f));
    if(detectedRed > (txtrWidth * txtrHeight) / 100.f * 0.5f)
        res.push_back(glm::vec3(1.f,0.f,0.f));
    if(detectedGreen > (txtrWidth * txtrHeight) / 100.f * 0.5f)
        res.push_back(glm::vec3(0.f,1.f,0.f));
    if(detectedBlue > (txtrWidth * txtrHeight) / 100.f * 0.5f)
        res.push_back(glm::vec3(0.f,0.f,1.f));
    if(detectedPink > (txtrWidth * txtrHeight) / 100.f * 0.5f)
        res.push_back(glm::vec3(1.f,0.f,1.f));
    if(detectedYellow > (txtrWidth * txtrHeight) / 100.f * 0.5f)
        res.push_back(glm::vec3(1.f,1.f,0.f));
    if(detectedOrange > (txtrWidth * txtrHeight) / 100.f * 0.5f)
        res.push_back(glm::vec3(1.f,0.5f,0.f));
    if(detectedCyan > (txtrWidth * txtrHeight) / 100.f * 0.5f)
        res.push_back(glm::vec3(0.f,1.f,1.f));
    if(detectedBlack > (txtrWidth * txtrHeight) / 100.f * 0.5f)
        res.push_back(glm::vec3(0.f,0.f,0.f));

    return res;
    
}

void Texture::removeSeams(Mesh& mesh, int textureResolution){
    
    /*! Binds another framebuffer !*/
    Texture textureObject = Texture(this->ID);
    unsigned int textureCopy = textureObject.duplicateTexture();
    
    unsigned int FBO;
    glGenFramebuffers(1,&FBO);
    glBindFramebuffer(GL_FRAMEBUFFER,FBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->ID, 0);
    glViewport(0, 0, textureResolution, textureResolution);

    glClearColor(0,0,0,0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    Box box;
    box.init();
    box.bindBuffers();
    
    glm::mat4 projection = glm::ortho(0.f, (float)textureResolution, (float)textureResolution, 0.f); 
    ShaderSystem::boundaryExpandingShader().use();
    ShaderSystem::boundaryExpandingShader().setMat4("projection"  ,       projection);
    ShaderSystem::boundaryExpandingShader().setMat4("projectedPosProjection"  ,       projection);
    ShaderSystem::boundaryExpandingShader().setVec3("pos"         ,       glm::vec3((float)textureResolution / 2.f, (float)textureResolution / 2.f, 0.9f));
    ShaderSystem::boundaryExpandingShader().setVec2("scale"       ,       glm::vec2((float)textureResolution / 2.f));

    ShaderSystem::boundaryExpandingShader().setInt("whiteUVTexture", 0);
    ShaderSystem::boundaryExpandingShader().setInt("originalTexture", 1);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, mesh.uvMask);
    
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, textureCopy);

    glDrawArrays(GL_TRIANGLES, 0 , 6);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDeleteFramebuffers(1, &FBO);
    glDeleteTextures(1, &textureCopy);
}

void Texture::removeSeams(Mesh& mesh, glm::ivec2 textureResolution){
    
    /*! Binds another framebuffer !*/
    Texture textureObject = Texture(this->ID);
    unsigned int textureCopy = textureObject.duplicateTexture();
    
    unsigned int FBO;
    glGenFramebuffers(1,&FBO);
    glBindFramebuffer(GL_FRAMEBUFFER,FBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->ID, 0);
    glViewport(0, 0, textureResolution.x, textureResolution.y);

    glClearColor(0,0,0,0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    Box box;
    box.init();
    box.bindBuffers();
    
    glm::mat4 projection = glm::ortho(0.f, (float)textureResolution.x, (float)textureResolution.y, 0.f); 
    ShaderSystem::boundaryExpandingShader().use();
    ShaderSystem::boundaryExpandingShader().setMat4("projection"  ,       projection);
    ShaderSystem::boundaryExpandingShader().setMat4("projectedPosProjection"  ,       projection);
    ShaderSystem::boundaryExpandingShader().setVec3("pos"         ,       glm::vec3((float)textureResolution.x / 2.f, (float)textureResolution.y / 2.f, 0.9f));
    ShaderSystem::boundaryExpandingShader().setVec2("scale"       ,       glm::vec2((float)textureResolution.x / 2.f, (float)textureResolution.y / 2.f));

    ShaderSystem::boundaryExpandingShader().setInt("whiteUVTexture", 0);
    ShaderSystem::boundaryExpandingShader().setInt("originalTexture", 1);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, mesh.uvMask);
    
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, textureCopy);

    glDrawArrays(GL_TRIANGLES, 0 , 6);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDeleteFramebuffers(1, &FBO);
    glDeleteTextures(1, &textureCopy);
}

unsigned int Texture::generateProceduralTexture(Mesh &mesh, int textureRes){
    unsigned int proceduralTxtr;
    
    ShaderSystem::to2DProcedural().use();

    if(this->proceduralID == 1000)
        ShaderSystem::to2DProcedural().setInt("proceduralID", -1);
    else
        ShaderSystem::to2DProcedural().setInt("proceduralID", this->proceduralID);
    
    std::cout << "proceduralID : " << proceduralID << std::endl;

    ShaderSystem::to2DProcedural().setFloat("proceduralScale", this->proceduralScale);
    ShaderSystem::to2DProcedural().setInt("proceduralInverted", this->proceduralnverted);
    ShaderSystem::to2DProcedural().setInt("proceduralUseTexCoords", this->proceduralUseTexCoords);
    ShaderSystem::to2DProcedural().setVec4("smartProperties", this->smartProperties);
    
    ShaderSystem::to2DProcedural().setMat4("orthoProjection", glm::ortho(0.f,1.f,0.f,1.f));
    ShaderSystem::to2DProcedural().setMat4("perspectiveProjection", getScene()->projectionMatrix);
    ShaderSystem::to2DProcedural().setMat4("view", getScene()->viewMatrix);

    ShaderSystem::to2DProcedural().setInt("proceduralTexture", 1);
    glActiveTexture(GL_TEXTURE1);
    if(this->proceduralID == 1000)
        glBindTexture(GL_TEXTURE_2D, this->ID);
    else
        glBindTexture(GL_TEXTURE_2D, this->proceduralTextureID);

    glActiveTexture(GL_TEXTURE0);
    glGenTextures(1,&proceduralTxtr);
    glBindTexture(GL_TEXTURE_2D,proceduralTxtr);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_MIRRORED_REPEAT);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, textureRes, textureRes, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glGenerateMipmap(GL_TEXTURE_2D);

    unsigned int FBO;
    glGenFramebuffers(1,&FBO);
    glBindFramebuffer(GL_FRAMEBUFFER,FBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, proceduralTxtr, 0);
    glViewport(0, 0, textureRes, textureRes);
    
    glClearColor(0,0,0,0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    mesh.Draw();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDeleteFramebuffers(0, &FBO);

    if(this->proceduralNormalMap){
        Texture txtrObject = Texture(proceduralTxtr);

        unsigned int normalMapRes;

        glActiveTexture(GL_TEXTURE0);
        glGenTextures(1,&normalMapRes);
        glBindTexture(GL_TEXTURE_2D,normalMapRes);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_MIRRORED_REPEAT);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, textureRes, textureRes, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
        glGenerateMipmap(GL_TEXTURE_2D);


        txtrObject.generateNormalMap(normalMapRes, textureRes, this->proceduralNormalStrength, this->proceduralNormalGrayScale); 

        glDeleteTextures(1,&proceduralTxtr);
        proceduralTxtr = normalMapRes;
    }

    return proceduralTxtr;
}

void Texture::generateNormalMap(unsigned int& normalMap, int textureResolution, float proceduralNormalStrength, bool proceduralNormalGrayScale){
    unsigned int FBO;
    glGenFramebuffers(1,&FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, normalMap, 0);
    glViewport(0, 0, textureResolution, textureResolution);

    glClearColor(0,0,0,0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    Box box;
    box.init();
    box.bindBuffers();
    
    glm::mat4 projection = glm::ortho(0.f, (float)textureResolution, (float)textureResolution, 0.f); 
    ShaderSystem::heightToNormalMap().use();
    ShaderSystem::heightToNormalMap().setInt("heightMap", 0);
    ShaderSystem::heightToNormalMap().setInt("txtrRes", textureResolution);
    ShaderSystem::heightToNormalMap().setFloat("strength", proceduralNormalStrength);
    ShaderSystem::heightToNormalMap().setInt("grayScale", proceduralNormalGrayScale);
    ShaderSystem::heightToNormalMap().setMat4("projection"  ,       projection);
    ShaderSystem::heightToNormalMap().setMat4("projectedPosProjection"  ,       projection);
    ShaderSystem::heightToNormalMap().setVec3("pos"         ,       glm::vec3((float)textureResolution / 2.f, (float)textureResolution / 2.f, 0.9f));
    ShaderSystem::heightToNormalMap().setVec2("scale"       ,       glm::vec2((float)textureResolution / 2.f));

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, this->ID);

    glDrawArrays(GL_TRIANGLES, 0 , 6);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDeleteFramebuffers(1, &FBO);
}