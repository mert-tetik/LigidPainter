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
#include "LibrarySystem/Library.hpp"
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
    Settings::defaultFramebuffer()->FBO.bind();
    glDeleteFramebuffers(1,&FBO);
    return newTexture;
}

void Texture::duplicateTexture(Texture& txtr){
    //Get the resolution data of the texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, ID);
    
    GLint width, height, internalFormat;
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &height);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_INTERNAL_FORMAT, &internalFormat);
    
    //Create the duplicated texture
    glBindTexture(GL_TEXTURE_2D, txtr.ID);
    
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
    Settings::defaultFramebuffer()->FBO.bind();
    glDeleteFramebuffers(1,&FBO);
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

    getBox()->bindBuffers();
    
    glm::mat4 projection = glm::ortho(0.f, (float)textureResolution, (float)textureResolution, 0.f); 
    ShaderSystem::boundaryExpandingShader().use();
    ShaderSystem::boundaryExpandingShader().setMat4("projection"  ,       projection);
    ShaderSystem::boundaryExpandingShader().setMat4("projectedPosProjection"  ,       projection);
    ShaderSystem::boundaryExpandingShader().setVec3("pos"         ,       glm::vec3((float)textureResolution / 2.f, (float)textureResolution / 2.f, 0.9f));
    ShaderSystem::boundaryExpandingShader().setVec2("scale"       ,       glm::vec2((float)textureResolution / 2.f));

    ShaderSystem::boundaryExpandingShader().setInt("whiteUVTexture", 0);
    ShaderSystem::boundaryExpandingShader().setInt("originalTexture", 1);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, mesh.uvMask.ID);
    
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, textureCopy);

    glDrawArrays(GL_TRIANGLES, 0 , 6);

    Settings::defaultFramebuffer()->FBO.bind();
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

    getBox()->bindBuffers();
    
    glm::mat4 projection = glm::ortho(0.f, (float)textureResolution.x, (float)textureResolution.y, 0.f); 
    ShaderSystem::boundaryExpandingShader().use();
    ShaderSystem::boundaryExpandingShader().setMat4("projection"  ,       projection);
    ShaderSystem::boundaryExpandingShader().setMat4("projectedPosProjection"  ,       projection);
    ShaderSystem::boundaryExpandingShader().setVec3("pos"         ,       glm::vec3((float)textureResolution.x / 2.f, (float)textureResolution.y / 2.f, 0.9f));
    ShaderSystem::boundaryExpandingShader().setVec2("scale"       ,       glm::vec2((float)textureResolution.x / 2.f, (float)textureResolution.y / 2.f));

    ShaderSystem::boundaryExpandingShader().setInt("whiteUVTexture", 0);
    ShaderSystem::boundaryExpandingShader().setInt("originalTexture", 1);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, mesh.uvMask.ID);
    
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, textureCopy);

    glDrawArrays(GL_TRIANGLES, 0 , 6);

    Settings::defaultFramebuffer()->FBO.bind();
    glDeleteFramebuffers(1, &FBO);
    glDeleteTextures(1, &textureCopy);
}

unsigned int Texture::generateProceduralTexture(Mesh &mesh, int textureRes){
    // The result texture
    unsigned int proceduralTxtr;
    
    // ------- Edge Wear ------- 
    if(this->proceduralProps.proceduralID == 111){
        Texture normalMapTxtr = Texture(nullptr, textureRes, textureRes);
        Texture normalMapTxtrBlurred = Texture(nullptr, textureRes, textureRes);
        Texture noiseTxtr;
        noiseTxtr.proceduralProps.proceduralID = 74;
        noiseTxtr = noiseTxtr.generateProceduralTexture(mesh, textureRes);

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
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, normalMapTxtr.ID, 0);
        glViewport(0, 0, textureRes, textureRes);
        
        glClearColor(0,0,0,0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        ShaderSystem::normalVectorShader().use();
        ShaderSystem::normalVectorShader().setMat4("orthoProjection", glm::ortho(0.f,1.f,0.f,1.f));
        ShaderSystem::normalVectorShader().setMat4("perspectiveProjection", getScene()->projectionMatrix);
        ShaderSystem::normalVectorShader().setMat4("view", getScene()->viewMatrix);
    
        mesh.Draw();

        // Blured normal vector txtr
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, normalMapTxtrBlurred.ID, 0);

        //TODO : Remove the box
        getBox()->bindBuffers();
        
        glm::mat4 projection = glm::ortho(0.f, (float)textureRes, (float)textureRes, 0.f); 
        ShaderSystem::bluringShader().use();
        ShaderSystem::bluringShader().setInt("txtr", 0);
        ShaderSystem::bluringShader().setInt("uvMask", 1);
        ShaderSystem::bluringShader().setVec2("txtrRes", glm::vec2(textureRes));
        ShaderSystem::bluringShader().setMat4("projection"  ,       projection);
        ShaderSystem::bluringShader().setMat4("projectedPosProjection"  ,       projection);
        ShaderSystem::bluringShader().setVec3("pos"         ,       glm::vec3((float)textureRes / 2.f, (float)textureRes / 2.f, 0.9f));
        ShaderSystem::bluringShader().setVec2("scale"       ,       glm::vec2((float)textureRes / 2.f));
        ShaderSystem::bluringShader().setFloat("blurVal"     ,     this->proceduralProps.smartProperties.x);
        
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, normalMapTxtr.ID);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, mesh.uvMask.ID);

        glDrawArrays(GL_TRIANGLES, 0, 6);

        //
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, proceduralTxtr, 0);
        
        ShaderSystem::edgeWearShader().use();
        ShaderSystem::edgeWearShader().setInt("normalVectorTxtr", 0);
        ShaderSystem::edgeWearShader().setInt("normalVectorTxtrBlurred", 1);
        ShaderSystem::edgeWearShader().setInt("noiseTexture", 2);
        ShaderSystem::edgeWearShader().setInt("invert", this->proceduralProps.proceduralnverted);
        ShaderSystem::edgeWearShader().setFloat("brightness", this->proceduralProps.proceduralBrightness);
        ShaderSystem::edgeWearShader().setFloat("softness", this->proceduralProps.smartProperties.y);
        ShaderSystem::edgeWearShader().setFloat("noiseStrength", this->proceduralProps.smartProperties.w);
        ShaderSystem::edgeWearShader().setVec2("txtrRes", glm::vec2(textureRes));
        ShaderSystem::edgeWearShader().setMat4("projection", projection);
        ShaderSystem::edgeWearShader().setMat4("projectedPosProjection", projection);
        ShaderSystem::edgeWearShader().setVec3("pos", glm::vec3((float)textureRes / 2.f, (float)textureRes / 2.f, 0.9f));
        ShaderSystem::edgeWearShader().setVec2("scale", glm::vec2((float)textureRes / 2.f));
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, normalMapTxtr.ID);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, normalMapTxtrBlurred.ID);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, noiseTxtr.ID);
        
        glDrawArrays(GL_TRIANGLES, 0, 6);


        //Bluring the result        
        ShaderSystem::bluringShader().use();
        ShaderSystem::bluringShader().setInt("txtr", 0);
        ShaderSystem::bluringShader().setInt("uvMask", 1);
        ShaderSystem::bluringShader().setVec2("txtrRes", glm::vec2(textureRes));
        ShaderSystem::bluringShader().setMat4("projection"  ,       projection);
        ShaderSystem::bluringShader().setMat4("projectedPosProjection"  ,       projection);
        ShaderSystem::bluringShader().setVec3("pos"         ,       glm::vec3((float)textureRes / 2.f, (float)textureRes / 2.f, 0.9f));
        ShaderSystem::bluringShader().setVec2("scale"       ,       glm::vec2((float)textureRes / 2.f));
        ShaderSystem::bluringShader().setFloat("blurVal"     ,     this->proceduralProps.smartProperties.z);
        
        Texture procObject = proceduralTxtr;
        Texture procCopy = procObject.duplicateTexture();
        glBindFramebuffer(GL_FRAMEBUFFER, FBO);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, procCopy.ID);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, mesh.uvMask.ID);

        glDrawArrays(GL_TRIANGLES, 0, 6);

        Settings::defaultFramebuffer()->FBO.bind();
        glDeleteFramebuffers(1, &FBO);
        glDeleteTextures(1, &normalMapTxtr.ID);
        glDeleteTextures(1, &normalMapTxtrBlurred.ID);
        glDeleteTextures(1, &procCopy.ID);
        glDeleteTextures(1, &noiseTxtr.ID);
    }

    // ------- Regular Procedural Texture -------
    else{
        ShaderSystem::to2DProcedural().use();

        if(this->proceduralProps.proceduralID == 1000)
            ShaderSystem::to2DProcedural().setInt("proceduralID", -1);
        else
            ShaderSystem::to2DProcedural().setInt("proceduralID", this->proceduralProps.proceduralID);
        
        ShaderSystem::to2DProcedural().setFloat("proceduralScale", this->proceduralProps.proceduralScale);
        ShaderSystem::to2DProcedural().setInt("proceduralInverted", this->proceduralProps.proceduralnverted);
        ShaderSystem::to2DProcedural().setInt("proceduralGrayScale", this->proceduralProps.proceduralGrayScale);
        ShaderSystem::to2DProcedural().setFloat("proceduralBrightness", this->proceduralProps.proceduralBrightness);
        ShaderSystem::to2DProcedural().setInt("proceduralUseTexCoords", this->proceduralProps.proceduralUseTexCoords);
        ShaderSystem::to2DProcedural().setVec4("smartProperties", this->proceduralProps.smartProperties);
        ShaderSystem::to2DProcedural().setVec2("txtrRes", this->getResolution());
        
        ShaderSystem::to2DProcedural().setMat4("orthoProjection", glm::ortho(0.f,1.f,0.f,1.f));
        ShaderSystem::to2DProcedural().setMat4("perspectiveProjection", getScene()->projectionMatrix);
        ShaderSystem::to2DProcedural().setMat4("view", getScene()->viewMatrix);

        ShaderSystem::to2DProcedural().setInt("proceduralTexture", 1);
        glActiveTexture(GL_TEXTURE1);
        if(this->proceduralProps.proceduralID == 1000)
            glBindTexture(GL_TEXTURE_2D, this->ID);
        else
            glBindTexture(GL_TEXTURE_2D, this->proceduralProps.proceduralTextureID);

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

        Settings::defaultFramebuffer()->FBO.bind();
        glDeleteFramebuffers(1, &FBO);
    }

    // ------- Generating Normal Map -------
    if(this->proceduralProps.proceduralNormalMap){
        Texture txtrObject = Texture(proceduralTxtr);

        Texture normalMapRes = Texture(nullptr, textureRes, textureRes, GL_LINEAR);

        txtrObject.generateNormalMap(normalMapRes.ID, textureRes, this->proceduralProps.proceduralNormalStrength, this->proceduralProps.proceduralNormalGrayScale); 

        glDeleteTextures(1,&proceduralTxtr);
        proceduralTxtr = normalMapRes.ID;
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

    getBox()->bindBuffers();
    
    glm::mat4 projection = glm::ortho(0.f, (float)textureResolution, (float)textureResolution, 0.f); 
    ShaderSystem::heightToNormalMap().use();
    ShaderSystem::heightToNormalMap().setInt("heightMap", 0);
    ShaderSystem::heightToNormalMap().setInt("alphaMode", 0);
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

    Settings::defaultFramebuffer()->FBO.bind();
    glDeleteFramebuffers(1, &FBO);
}

void Texture::generateNormalMap(unsigned int& normalMap, glm::ivec2 textureResolution, float proceduralNormalStrength, bool proceduralNormalGrayScale, bool alphaMode){
    unsigned int FBO;
    glGenFramebuffers(1,&FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, normalMap, 0);
    glViewport(0, 0, textureResolution.x, textureResolution.y);

    glClearColor(0,0,0,0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    getBox()->bindBuffers();
    
    glm::mat4 projection = glm::ortho(0.f, (float)textureResolution.x, (float)textureResolution.y, 0.f); 
    ShaderSystem::heightToNormalMap().use();
    ShaderSystem::heightToNormalMap().setInt("heightMap", 0);
    ShaderSystem::heightToNormalMap().setInt("alphaMode", alphaMode);
    ShaderSystem::heightToNormalMap().setInt("txtrRes", textureResolution.x);
    ShaderSystem::heightToNormalMap().setFloat("strength", proceduralNormalStrength);
    ShaderSystem::heightToNormalMap().setInt("grayScale", proceduralNormalGrayScale);
    ShaderSystem::heightToNormalMap().setMat4("projection"  ,       projection);
    ShaderSystem::heightToNormalMap().setMat4("projectedPosProjection"  ,       projection);
    ShaderSystem::heightToNormalMap().setVec3("pos"         ,       glm::vec3((float)textureResolution.x / 2.f, (float)textureResolution.y / 2.f, 0.9f));
    ShaderSystem::heightToNormalMap().setVec2("scale"       ,       glm::vec2((glm::vec2)textureResolution / 2.f));

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, this->ID);

    glDrawArrays(GL_TRIANGLES, 0 , 6);

    Settings::defaultFramebuffer()->FBO.bind();
    glDeleteFramebuffers(1, &FBO);
}


void Texture::applyNormalMap(glm::ivec2 textureResolution, float proceduralNormalStrength, bool proceduralNormalGrayScale){
    
    Texture tx = this->duplicateTexture();
    
    unsigned int FBO;
    glGenFramebuffers(1,&FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->ID, 0);
    glViewport(0, 0, textureResolution.x, textureResolution.y);

    glClearColor(0,0,0,0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    getBox()->bindBuffers();
    
    glm::mat4 projection = glm::ortho(0.f, (float)textureResolution.x, (float)textureResolution.y, 0.f); 
    ShaderSystem::heightToNormalMap().use();
    ShaderSystem::heightToNormalMap().setInt("heightMap", 0);
    ShaderSystem::heightToNormalMap().setInt("alphaMode", 0);
    ShaderSystem::heightToNormalMap().setInt("txtrRes", textureResolution.x);
    ShaderSystem::heightToNormalMap().setFloat("strength", proceduralNormalStrength);
    ShaderSystem::heightToNormalMap().setInt("grayScale", proceduralNormalGrayScale);
    ShaderSystem::heightToNormalMap().setMat4("projection"  ,       projection);
    ShaderSystem::heightToNormalMap().setMat4("projectedPosProjection"  ,       projection);
    ShaderSystem::heightToNormalMap().setVec3("pos"         ,       glm::vec3((float)textureResolution.x / 2.f, (float)textureResolution.y / 2.f, 0.9f));
    ShaderSystem::heightToNormalMap().setVec2("scale"       ,       glm::vec2(glm::vec2(textureResolution) / 2.f));


    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tx.ID);

    glDrawArrays(GL_TRIANGLES, 0 , 6);

    Settings::defaultFramebuffer()->FBO.bind();
    glDeleteFramebuffers(1, &FBO);
    glDeleteTextures(1, &tx.ID);
}

void Texture::generateProceduralDisplayingTexture(int displayingTextureRes){
    
    // ------------- Set Up -------------

    GLint viewport[4]; 
    glGetIntegerv(GL_VIEWPORT, viewport);

    glm::ivec2 viewportResolution = glm::ivec2(viewport[2], viewport[3]);
    
    //TODO Set the scale & invert 
    // The texture is not generated
    if(this->ID == 0 || glIsTexture(this->ID) == GL_FALSE){
        glGenTextures(1, &this->ID);
    }

    const int displayRes = displayingTextureRes;

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, this->ID);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_MIRRORED_REPEAT);
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, displayRes, displayRes, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glGenerateMipmap(GL_TEXTURE_2D);
    
    /* Capturing FBO */
    unsigned int FBO; 
    glGenFramebuffers(1,&FBO);
    glBindFramebuffer(GL_FRAMEBUFFER,FBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->ID, 0);
    glClearColor(0,0,0,0);

    unsigned int RBO;
	glGenRenderbuffers(1,&RBO);
	glBindRenderbuffer(GL_RENDERBUFFER,RBO);
	
    //Set the renderbuffer to store depth
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, displayRes, displayRes);
	
    //Give the renderbuffer to the framebuffer
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, RBO);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glDepthFunc(GL_LEQUAL);




    // Generate the texture pack texture
    if(this->proceduralProps.textureSelectionDialog_selectedMode == 3){
        if(this->proceduralProps.textureSelectionDialog_selectedTextureIndex < Library::getTexturePackArraySize()){
            Library::getTexturePack(this->proceduralProps.textureSelectionDialog_selectedTextureIndex)->apply(
                                                                                                                *this,
                                                                                                                this->proceduralProps.txtrPackScale,
                                                                                                                this->proceduralProps.txtrPackCount,
                                                                                                                this->proceduralProps.txtrPackRotation_Jitter,
                                                                                                                this->proceduralProps.txtrPackSize_Jitter,
                                                                                                                this->proceduralProps.txtrPackOpacity_Jitter,
                                                                                                                this->proceduralProps.txtrPackScatter
                                                                                                            );
            ShaderSystem::buttonShader().use();
        }
        
        glBindFramebuffer(GL_FRAMEBUFFER,FBO);
    }

    glViewport(0, 0, displayRes, displayRes);

    // Generate the smart texture displaying texture
    if(this->proceduralProps.textureSelectionDialog_selectedMode == 4){
        Camera matCam;
        matCam.cameraPos = glm::vec3(0,0,-8.f);
        matCam.radius = 8.f;


        //Move the camera to the side
        glm::mat4 view = glm::lookAt(matCam.cameraPos, 
                                     glm::vec3(0), 
                                     glm::vec3(0.0, 1.0, 0.0));

        //The perspective projection matrix    
        glm::mat4 projectionMatrix = glm::perspective(
                                                        glm::radians(35.f), //Fov  
                                                        -1.f,  //Ratio (is 1 since the width & the height is equal to displayRes)
                                                        100.f,  //Near (the material is pretty close to the camera actually  ) 
                                                        0.1f    //Far
                                                    );

        //Use the 3D model rendering shader
        ShaderSystem::tdModelShader().use();

        //Throw the camera data to the shader
        ShaderSystem::tdModelShader().setInt("displayingMode", 0);
        ShaderSystem::tdModelShader().setVec3("viewPos",matCam.cameraPos);
        ShaderSystem::tdModelShader().setMat4("view",view);
        ShaderSystem::tdModelShader().setMat4("projection",projectionMatrix);

        unsigned int proc = this->generateProceduralTexture(getMaterialDisplayerModel()->meshes[0], 512);
        glViewport(0, 0, displayRes, displayRes);
        glBindFramebuffer(GL_FRAMEBUFFER, FBO);
        ShaderSystem::tdModelShader().use();

        //Bind the channels of the material
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, proc);
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, proc);
        glActiveTexture(GL_TEXTURE4);
        glBindTexture(GL_TEXTURE_2D, proc);
        glActiveTexture(GL_TEXTURE5);
        glBindTexture(GL_TEXTURE_2D, proc);
        glActiveTexture(GL_TEXTURE6);
        glBindTexture(GL_TEXTURE_2D, proc);
        glActiveTexture(GL_TEXTURE7);        
        glBindTexture(GL_TEXTURE_2D, proc);

        getMaterialDisplayerModel()->Draw();

        //Use the button shader (Is necessary since that process is done in the middle of GUI rendering) 
        ShaderSystem::buttonShader().use();

        getBox()->bindBuffers();
        //glDeleteTextures(1, &proc);
    }

    // Generate regular procedural textures
    if(this->proceduralProps.textureSelectionDialog_selectedMode != 3 && this->proceduralProps.textureSelectionDialog_selectedMode != 4){

        /* Displaying texture */
        ShaderSystem::proceduralDisplayerShader().use();
        glm::mat4 projection = glm::ortho(0.f, (float)displayRes, (float)displayRes, 0.f);
        ShaderSystem::proceduralDisplayerShader().setMat4("projection"  ,       projection);
        ShaderSystem::proceduralDisplayerShader().setVec3("pos"         ,       glm::vec3((float)displayRes / 2.f, (float)displayRes / 2.f,0.9f));
        ShaderSystem::proceduralDisplayerShader().setVec2("scale"       ,       glm::vec2((float)displayRes / 2.f));
        
        ShaderSystem::proceduralDisplayerShader().setInt("proceduralID", this->proceduralProps.proceduralID);                
        ShaderSystem::proceduralDisplayerShader().setFloat("proceduralScale", this->proceduralProps.proceduralScale);
        ShaderSystem::proceduralDisplayerShader().setInt("proceduralInverted", this->proceduralProps.proceduralnverted);
        ShaderSystem::proceduralDisplayerShader().setInt("proceduralGrayScale", this->proceduralProps.proceduralGrayScale);
        ShaderSystem::proceduralDisplayerShader().setFloat("proceduralBrightness", this->proceduralProps.proceduralBrightness);
        ShaderSystem::proceduralDisplayerShader().setInt("proceduralGrayScale", this->proceduralProps.proceduralGrayScale);
        ShaderSystem::proceduralDisplayerShader().setInt("proceduralTexture", 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, this->proceduralProps.proceduralTextureID);
        
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }

    // Generate normal map
    if(this->proceduralProps.proceduralNormalMap && this->proceduralProps.textureSelectionDialog_selectedMode != 4){
        Texture txtrObject = Texture(this->ID);
        Texture normalMapRes = Texture(nullptr, displayRes, displayRes, GL_LINEAR);

        txtrObject.generateNormalMap(normalMapRes.ID, displayRes, this->proceduralProps.proceduralNormalStrength, this->proceduralProps.proceduralGrayScale); 

        glDeleteTextures(1,&this->ID);
        this->ID = normalMapRes.ID;
    }

    // Finish
    ShaderSystem::buttonShader().use();
    Settings::defaultFramebuffer()->FBO.bind();
    glDeleteFramebuffers(1, &FBO);
    glDeleteRenderbuffers(1, &RBO);
    glViewport(0, 0, viewportResolution.x, viewportResolution.y);
}


#define LGDTEXTURE_WRITEBITS(var, type, loc) if(!wf.write(reinterpret_cast<char*>(   &var     ), sizeof(type))){ \
                                    LGDLOG::start<< "ERROR : Writing texture data. Failed to write at : " << loc << LGDLOG::end;\
                                    return false; \
                                }

bool Texture::writeTextureData(std::ofstream& wf){
    
    // -------- Procedural Data -------- 
    
    LGDTEXTURE_WRITEBITS(this->proceduralProps.proceduralID, int, "Property texture - procedural ID");
    LGDTEXTURE_WRITEBITS(this->proceduralProps.proceduralScale, float, "Property texture - procedural Scale");
    LGDTEXTURE_WRITEBITS(this->proceduralProps.proceduralnverted, int, "Property texture - procedural nverted");
    LGDTEXTURE_WRITEBITS(this->proceduralProps.proceduralNormalMap, bool, "Property texture - procedural NormalMap");
    LGDTEXTURE_WRITEBITS(this->proceduralProps.proceduralNormalGrayScale, bool, "Property texture - procedural NormalGrayScale");
    LGDTEXTURE_WRITEBITS(this->proceduralProps.proceduralNormalStrength, float, "Property texture - procedural NormalStrength");
    LGDTEXTURE_WRITEBITS(this->proceduralProps.proceduralTextureID, unsigned int, "Property texture - procedural TextureID");
    LGDTEXTURE_WRITEBITS(this->proceduralProps.proceduralUseTexCoords, bool, "Property texture - procedural UseTexCoords");
    LGDTEXTURE_WRITEBITS(this->proceduralProps.proceduralGrayScale, bool, "Property texture - procedural GrayScale");
    LGDTEXTURE_WRITEBITS(this->proceduralProps.proceduralBrightness, float, "Property texture - procedural Brightness");
    LGDTEXTURE_WRITEBITS(this->proceduralProps.smartProperties.x, float, "Property texture - Smart Properties");
    LGDTEXTURE_WRITEBITS(this->proceduralProps.smartProperties.y, float, "Property texture - Smart Properties");
    LGDTEXTURE_WRITEBITS(this->proceduralProps.smartProperties.z, float, "Property texture - Smart Properties");
    LGDTEXTURE_WRITEBITS(this->proceduralProps.smartProperties.w, float, "Property texture - Smart Properties");
    LGDTEXTURE_WRITEBITS(this->proceduralProps.txtrPackScale, float, "Property texture - txtrPackScale");
    LGDTEXTURE_WRITEBITS(this->proceduralProps.txtrPackCount, float, "Property texture - txtrPackCount");
    LGDTEXTURE_WRITEBITS(this->proceduralProps.txtrPackRotation_Jitter, float, "Property texture - txtrPackRotation_Jitter");
    LGDTEXTURE_WRITEBITS(this->proceduralProps.txtrPackSize_Jitter, float, "Property texture - txtrPackSize_Jitter");
    LGDTEXTURE_WRITEBITS(this->proceduralProps.txtrPackOpacity_Jitter, float, "Property texture - txtrPackOpacity_Jitter");
    LGDTEXTURE_WRITEBITS(this->proceduralProps.txtrPackScatter, float, "Property texture - txtrPackScatter");
    LGDTEXTURE_WRITEBITS(this->proceduralProps.textureSelectionDialog_selectedTextureIndex, int, "Property texture - textureSelectionDialog_selectedTextureIndex");
    LGDTEXTURE_WRITEBITS(this->proceduralProps.textureSelectionDialog_selectedMode, int, "Property texture - textureSelectionDialog_selectedMode");


    // -------- Texture Data --------

    int32_t textureWidth = this->getResolution().x;
    LGDTEXTURE_WRITEBITS(textureWidth, int32_t, "Property texture - texture width");

    int32_t textureHeight = this->getResolution().y;
    LGDTEXTURE_WRITEBITS(textureHeight, int32_t, "Property texture - texture height");

    char* pixels = new char[textureWidth * textureHeight * 4];
    this->getData(pixels);
    
    if(!wf.write(pixels, textureWidth * textureHeight * 4 * sizeof(char))){
        LGDLOG::start<< "ERROR : Writing texture data. Failed to write at : " << "Property texture - texture pixels" << LGDLOG::end;
        return false; 
    }

    delete[] pixels;
    
    return true;
}

#define LGDMATERIAL_READBITS(var, type, loc) if(!rf.read(reinterpret_cast<char*>(   &var     ), sizeof(type))){ \
                                LGDLOG::start<< "ERROR : Reading lgdmaterial file. Failed to read at : " << loc << LGDLOG::end;\
                                return false; \
                            }

bool Texture::readTextureData(std::ifstream& rf){

    // --------- Read procedural data ---------

    int proceduralID;
    float proceduralScale;
    int proceduralnverted;
    bool proceduralNormalMap;
    bool proceduralNormalGrayScale;
    float proceduralNormalStrength;
    unsigned int proceduralTextureID;
    bool proceduralUseTexCoords;
    bool proceduralGrayScale;
    float proceduralBrightness;
    float smartProperties_x;
    float smartProperties_y;
    float smartProperties_z;
    float smartProperties_w;
    float txtrPackScale;
    float txtrPackCount;
    float txtrPackRotation_Jitter;
    float txtrPackSize_Jitter;
    float txtrPackOpacity_Jitter;
    float txtrPackScatter;
    int textureSelectionDialog_selectedTextureIndex;
    int textureSelectionDialog_selectedMode;
    
    LGDMATERIAL_READBITS(proceduralID, int, "Property texture - procedural ID");
    LGDMATERIAL_READBITS(proceduralScale, float, "Property texture - procedural Scale");
    LGDMATERIAL_READBITS(proceduralnverted, int, "Property texture - procedural nverted");
    LGDMATERIAL_READBITS(proceduralNormalMap, bool, "Property texture - procedural NormalMap");
    LGDMATERIAL_READBITS(proceduralNormalGrayScale, bool, "Property texture - procedural NormalGrayScale");
    LGDMATERIAL_READBITS(proceduralNormalStrength, float, "Property texture - procedural NormalStrength");
    LGDMATERIAL_READBITS(proceduralTextureID, unsigned int, "Property texture - procedural TextureID");
    LGDMATERIAL_READBITS(proceduralUseTexCoords, bool, "Property texture - procedural UseTexCoords");
    LGDMATERIAL_READBITS(proceduralGrayScale, bool, "Property texture - procedural GrayScale");
    LGDMATERIAL_READBITS(proceduralBrightness, float, "Property texture - procedural Brightness");
    LGDMATERIAL_READBITS(smartProperties_x, float, "Property texture - Smart Properties");
    LGDMATERIAL_READBITS(smartProperties_y, float, "Property texture - Smart Properties");
    LGDMATERIAL_READBITS(smartProperties_z, float, "Property texture - Smart Properties");
    LGDMATERIAL_READBITS(smartProperties_w, float, "Property texture - Smart Properties");
    LGDMATERIAL_READBITS(txtrPackScale, float, "Property texture - txtrPackScale");
    LGDMATERIAL_READBITS(txtrPackCount, float, "Property texture - txtrPackCount");
    LGDMATERIAL_READBITS(txtrPackRotation_Jitter, float, "Property texture - txtrPackRotation_Jitter");
    LGDMATERIAL_READBITS(txtrPackSize_Jitter, float, "Property texture - txtrPackSize_Jitter");
    LGDMATERIAL_READBITS(txtrPackOpacity_Jitter, float, "Property texture - txtrPackOpacity_Jitter");
    LGDMATERIAL_READBITS(txtrPackScatter, float, "Property texture - txtrPackScatter");
    LGDMATERIAL_READBITS(textureSelectionDialog_selectedTextureIndex, int, "Property texture - textureSelectionDialog_selectedTextureIndex");
    LGDMATERIAL_READBITS(textureSelectionDialog_selectedMode, int, "Property texture - textureSelectionDialog_selectedMode");

    // --------- Read texture data ---------
     
    int32_t textureWidth = (*this).getResolution().x;
    LGDMATERIAL_READBITS(textureWidth, int32_t, "Modifier's property - texture data - texture width");

    int32_t textureHeight = (*this).getResolution().y;
    LGDMATERIAL_READBITS(textureHeight, int32_t, "Modifier's property - texture data - texture height");

    char* pixels = new char[textureWidth * textureHeight * 4];
    if(!rf.read(pixels, textureWidth * textureHeight * 4 * sizeof(char))){
        LGDLOG::start<< "ERROR : Reading lgdmaterial file. Failed to read at : Modifier's property - texture data - texture pixels" << LGDLOG::end;
        return false;   
    }

    // --------- Create the texture ---------

    (*this) = Texture(pixels, textureWidth, textureHeight, GL_NEAREST);

    this->proceduralProps.proceduralID = proceduralID;
    this->proceduralProps.proceduralScale = proceduralScale;
    this->proceduralProps.proceduralnverted = proceduralnverted;
    this->proceduralProps.proceduralNormalMap = proceduralNormalMap;
    this->proceduralProps.proceduralNormalGrayScale = proceduralNormalGrayScale;
    this->proceduralProps.proceduralNormalStrength = proceduralNormalStrength;
    this->proceduralProps.proceduralTextureID = proceduralTextureID;
    this->proceduralProps.proceduralUseTexCoords = proceduralUseTexCoords;
    this->proceduralProps.proceduralGrayScale = proceduralGrayScale;
    this->proceduralProps.proceduralBrightness = proceduralBrightness;
    this->proceduralProps.smartProperties.x = smartProperties_x;
    this->proceduralProps.smartProperties.y = smartProperties_y;
    this->proceduralProps.smartProperties.z = smartProperties_z;
    this->proceduralProps.smartProperties.w = smartProperties_w;
    this->proceduralProps.txtrPackScale = txtrPackScale;
    this->proceduralProps.txtrPackCount = txtrPackCount;
    this->proceduralProps.txtrPackRotation_Jitter = txtrPackRotation_Jitter;
    this->proceduralProps.txtrPackSize_Jitter = txtrPackSize_Jitter;
    this->proceduralProps.txtrPackOpacity_Jitter = txtrPackOpacity_Jitter;
    this->proceduralProps.txtrPackScatter = txtrPackScatter;
    this->proceduralProps.textureSelectionDialog_selectedTextureIndex = textureSelectionDialog_selectedTextureIndex;
    this->proceduralProps.textureSelectionDialog_selectedMode = textureSelectionDialog_selectedMode;

    return true;
}