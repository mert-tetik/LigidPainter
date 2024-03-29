/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

(c) 2024 Mert Tetik. All rights reserved.

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
#include "UTIL/Shader/Shader.hpp"
#include "UTIL/Library/Library.hpp"
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

    if(!this->ID || glIsTexture(this->ID) == GL_FALSE){
        std::cout << "ERROR : Can't get the resolution of the texture : Invalid ID : "  << std::endl;
        return glm::ivec2(1024);
    }
    
    while (glGetError() != GL_NO_ERROR){}
    
    int w = 0, h = 0;

    // Check if the texture is bound
    GLint currentTexture;
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &currentTexture);
    
    if (currentTexture != ID) {
        // Bind the texture if it's not already bound
        glBindTexture(GL_TEXTURE_2D, ID);
    }

    // Retrieve the texture width
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &w);

    // Check for OpenGL errors after glGetTexLevelParameteriv
    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        LGDLOG::start << "ERROR : Can't get the resolution of the texture : OpenGL Error " << error << LGDLOG::end;
        return glm::ivec2(1024);
        // Handle the error (print a message, log, etc.)
        // Optionally, you might want to return an "invalid" value here
    }

    // Retrieve the texture height
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &h);

    // Check for OpenGL errors after the second glGetTexLevelParameteriv
    error = glGetError();
    if (error != GL_NO_ERROR) {
        LGDLOG::start << "ERROR : Can't get the resolution of the texture : OpenGL Error " << error << LGDLOG::end;
        return glm::ivec2(1024);
        // Handle the error (print a message, log, etc.)
        // Optionally, you might want to return an "invalid" value here
    }

    // Restore the original texture binding (if it was changed)
    if (currentTexture != ID) {
        glBindTexture(GL_TEXTURE_2D, currentTexture);
    }

    return glm::ivec2(w, h);
}

unsigned int Texture::duplicateTexture(){

    // Return a 1x1 rgba 0 texture if the requested texture is not valid
    if(this->ID == 0 || glIsTexture(this->ID) == GL_FALSE){
        return Texture(nullptr, 1, 1).ID;
    }

    unsigned int newTexture;
    //Get the resolution data of the texture
    glActiveTexture(GL_TEXTURE0);
    glGenTextures(1, &newTexture);
    glBindTexture(GL_TEXTURE_2D, ID);

    GLint wrapParam_S;
    GLint wrapParam_T;
    GLint wrapParam_R;
    glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, &wrapParam_S);
    glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, &wrapParam_T);
    glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, &wrapParam_R);
    
    GLint width, height, internalFormat;
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &height);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_INTERNAL_FORMAT, &internalFormat);
    //Create the duplicated texture
    glBindTexture(GL_TEXTURE_2D, newTexture);
    

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapParam_S);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapParam_T);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, wrapParam_R);
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
    
    GLint wrapParam_S;
    GLint wrapParam_T;
    GLint wrapParam_R;
    glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, &wrapParam_S);
    glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, &wrapParam_T);
    glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, &wrapParam_R);
    
    GLint width, height, internalFormat;
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &height);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_INTERNAL_FORMAT, &internalFormat);
    
    //Create the duplicated texture
    glBindTexture(GL_TEXTURE_2D, txtr.ID);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapParam_S);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapParam_T);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, wrapParam_R);
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

void Texture::duplicateTextureSub(Texture& txtr){
    
    //Get the resolution data of the texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, ID);
    
    GLint wrapParam_S;
    GLint wrapParam_T;
    GLint wrapParam_R;
    glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, &wrapParam_S);
    glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, &wrapParam_T);
    glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, &wrapParam_R);
    
    GLint width, height, internalFormat;
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &height);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_INTERNAL_FORMAT, &internalFormat);
    
    //Create the duplicated texture
    glBindTexture(GL_TEXTURE_2D, txtr.ID);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapParam_S);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapParam_T);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, wrapParam_R);
    
    //Copy the texture
    unsigned int FBO;
    glGenFramebuffers(1,&FBO);
    glBindFramebuffer(GL_FRAMEBUFFER,FBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ID, 0);
    glCopyTexImage2D(GL_TEXTURE_2D, 0, internalFormat, 0, 0, width, height, 0);
    
    Settings::defaultFramebuffer()->FBO.bind();
    glDeleteFramebuffers(1,&FBO);

    txtr.proceduralProps = this->proceduralProps;
}


std::vector<MaterialIDColor> Texture::getMaterialIDPalette(){
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

    std::vector<MaterialIDColor> materialIDColors;

    for (size_t i = 0; i < res.size(); i++)
    {
        MaterialIDColor materialIDColor;
        materialIDColor.color = res[i];
        materialIDColor.grayScaleTxtr = Texture(nullptr, 512, 512);

        Framebuffer FBO = Framebuffer(materialIDColor.grayScaleTxtr, GL_TEXTURE_2D, "Texture::getMaterialIDPalette");
        FBO.bind();
        getBox()->bindBuffers();

        glViewport(0,0,512,512);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        ShaderSystem::colorIDMaskingShader().use();
        
        ShaderSystem::colorIDMaskingShader().setMat4("projection", glm::ortho(0.f,1.f,1.f,0.f));
        ShaderSystem::colorIDMaskingShader().setMat4("projectedPosProjection", glm::ortho(0.f,1.f,1.f,0.f));
        ShaderSystem::colorIDMaskingShader().setVec3("pos", glm::vec3(0.5f));
        ShaderSystem::colorIDMaskingShader().setVec2("scale", glm::vec2(0.5f));

        ShaderSystem::colorIDMaskingShader().setInt("IDTexture", 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, this->ID);

        ShaderSystem::colorIDMaskingShader().setVec3("wbr", glm::vec3(res[i] == glm::vec3(1.f,1.f,1.f), res[i] == glm::vec3(0.f,0.f,0.f), res[i] == glm::vec3(1.f,0.f,0.f)));
        ShaderSystem::colorIDMaskingShader().setVec3("pbc", glm::vec3(res[i] == glm::vec3(1.f,0.f,1.f), res[i] == glm::vec3(0.f,0.f,1.f), res[i] == glm::vec3(0.f,1.f,1.f)));
        ShaderSystem::colorIDMaskingShader().setVec3("gyo", glm::vec3(res[i] == glm::vec3(0.f,1.f,0.f), res[i] == glm::vec3(1.f,1.f,0.f), res[i] == glm::vec3(1.f,0.5f,0.f)));

        LigidGL::makeDrawCall(GL_TRIANGLES, 0, 6, "Texture::getMaterialIDPalette : Generating gray scale texture");

        Settings::defaultFramebuffer()->FBO.bind();
        Settings::defaultFramebuffer()->setViewport();
        FBO.deleteBuffers(false, false);
        ShaderSystem::buttonShader().use();

        materialIDColors.push_back(materialIDColor);
    }

    return materialIDColors;
}

void Texture::removeSeams(Mesh& mesh, int textureResolution){
    this->removeSeams(mesh, glm::ivec2(textureResolution));
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

    LigidGL::makeDrawCall(GL_TRIANGLES, 0 , 6, "Texture::removeSeams : Rendering result");

    Settings::defaultFramebuffer()->FBO.bind();
    glDeleteFramebuffers(1, &FBO);
    glDeleteTextures(1, &textureCopy);
}

static Texture normalMapTxtr;
static Texture normalMapTxtrBlurred;
static Texture noiseTxtr;
static Texture destTxtrCopy;
static Texture procTxtr;
static Texture normalMapRes;

Texture Texture::generateProceduralTexture(Mesh &mesh, int textureRes){
    if(!procTxtr.ID){
        glGenTextures(1, &procTxtr.ID);
    }
    
    procTxtr.update(nullptr, textureRes, textureRes);
    
    generateProceduralTexture(mesh, procTxtr, textureRes);
    
    return procTxtr;
}

void Texture::generateProceduralTexture(Mesh &mesh, Texture& destTxtr, int textureRes){
    
    glm::ivec2 destTxtrRes = destTxtr.getResolution();

    // ------- Edge Wear ------- 
    if(this->proceduralProps.proceduralID == 121){
    
        if(!normalMapTxtr.ID){
            normalMapTxtr = Texture(nullptr, 1024, 1024);
            normalMapTxtrBlurred = Texture(nullptr, 1024, 1024);
            noiseTxtr = Texture(nullptr, 1024, 1024);
            destTxtrCopy = Texture(nullptr, textureRes, textureRes);
        }
        
        noiseTxtr.proceduralProps.proceduralID = 74;
        noiseTxtr.generateProceduralTexture(mesh, noiseTxtr, 1024);

        unsigned int FBO;
        glGenFramebuffers(1,&FBO);
        glBindFramebuffer(GL_FRAMEBUFFER,FBO);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, normalMapTxtr.ID, 0);
        glViewport(0, 0, 1024, 1024);
        
        glClearColor(0,0,0,0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        ShaderSystem::normalVectorShader().use();
        ShaderSystem::normalVectorShader().setMat4("orthoProjection", glm::ortho(0.f,1.f,0.f,1.f));
        ShaderSystem::normalVectorShader().setMat4("perspectiveProjection", getScene()->projectionMatrix);
        ShaderSystem::normalVectorShader().setMat4("view", getScene()->camera.viewMatrix);
    
        mesh.Draw(false);

        // Blured normal vector txtr
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, normalMapTxtrBlurred.ID, 0);
        glClearColor(0,0,0,0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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
        ShaderSystem::bluringShader().setFloat("blurVal"     ,     this->proceduralProps.smartProperties.x * 35.f);
        
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, normalMapTxtr.ID);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, mesh.uvMask.ID);

        LigidGL::makeDrawCall(GL_TRIANGLES, 0, 6, "Texture::generateProceduralTexture : Edge wear : Blurred normal vec");

        //
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, destTxtr.ID, 0);
        glClearColor(0,0,0,0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glViewport(0, 0, textureRes, textureRes);
        
        ShaderSystem::edgeWearShader().use();
        ShaderSystem::edgeWearShader().setInt("normalVectorTxtr", 0);
        ShaderSystem::edgeWearShader().setInt("normalVectorTxtrBlurred", 1);
        ShaderSystem::edgeWearShader().setInt("noiseTexture", 2);
        ShaderSystem::edgeWearShader().setInt("invert", this->proceduralProps.proceduralnverted);
        ShaderSystem::edgeWearShader().setFloat("brightness", this->proceduralProps.proceduralBrightness);
        ShaderSystem::edgeWearShader().setFloat("softness", this->proceduralProps.smartProperties.y);
        ShaderSystem::edgeWearShader().setFloat("noiseStrength", this->proceduralProps.smartProperties.w);
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
        
        LigidGL::makeDrawCall(GL_TRIANGLES, 0, 6, "Texture::generateProceduralTexture : Edge wear : Result");

        //Bluring the result        
        ShaderSystem::bluringShader().use();
        ShaderSystem::bluringShader().setInt("txtr", 0);
        ShaderSystem::bluringShader().setInt("uvMask", 1);
        ShaderSystem::bluringShader().setVec2("txtrRes", glm::vec2(textureRes));
        ShaderSystem::bluringShader().setMat4("projection"  ,       projection);
        ShaderSystem::bluringShader().setMat4("projectedPosProjection"  ,       projection);
        ShaderSystem::bluringShader().setVec3("pos"         ,       glm::vec3((float)textureRes / 2.f, (float)textureRes / 2.f, 0.9f));
        ShaderSystem::bluringShader().setVec2("scale"       ,       glm::vec2((float)textureRes / 2.f));
        ShaderSystem::bluringShader().setFloat("blurVal"     ,     this->proceduralProps.smartProperties.z * 35.f);
        
        destTxtr.duplicateTexture(destTxtrCopy);
        glBindFramebuffer(GL_FRAMEBUFFER, FBO);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, destTxtrCopy.ID);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, mesh.uvMask.ID);

        LigidGL::makeDrawCall(GL_TRIANGLES, 0, 6, "Texture::generateProceduralTexture : Edge wear : Blurring result");

        Settings::defaultFramebuffer()->FBO.bind();
        glDeleteFramebuffers(1, &FBO);
    }

    // ------- Regular Procedural Texture -------
    else{
        ShaderSystem::to2DProcedural().use();

        if(this->proceduralProps.proceduralID == 1000)
            ShaderSystem::to2DProcedural().setInt("proceduralID", -1);
        else
            ShaderSystem::to2DProcedural().setInt("proceduralID", this->proceduralProps.proceduralID);
        
        ShaderSystem::to2DProcedural().setFloat("proceduralScale", this->proceduralProps.proceduralScale);
        ShaderSystem::to2DProcedural().setFloat("proceduralStretch", this->proceduralProps.proceduralStretch);
        ShaderSystem::to2DProcedural().setFloat("proceduralScaleModelPos", this->proceduralProps.proceduralScaleModelPos);
        ShaderSystem::to2DProcedural().setInt("proceduralInverted", this->proceduralProps.proceduralnverted);
        ShaderSystem::to2DProcedural().setInt("proceduralGrayScale", this->proceduralProps.proceduralGrayScale);
        ShaderSystem::to2DProcedural().setFloat("proceduralBrightness", this->proceduralProps.proceduralBrightness);
        ShaderSystem::to2DProcedural().setInt("proceduralUseTexCoords", this->proceduralProps.proceduralUseTexCoords);
        ShaderSystem::to2DProcedural().setVec4("smartProperties", this->proceduralProps.smartProperties);
        ShaderSystem::to2DProcedural().setVec2("txtrRes", this->getResolution());
        
        ShaderSystem::to2DProcedural().setMat4("orthoProjection", glm::ortho(0.f,1.f,0.f,1.f));
        ShaderSystem::to2DProcedural().setMat4("perspectiveProjection", getScene()->projectionMatrix);
        ShaderSystem::to2DProcedural().setMat4("view", getScene()->camera.viewMatrix);

        ShaderSystem::to2DProcedural().setInt("proceduralTexture", 1);
        glActiveTexture(GL_TEXTURE1);
        if(this->proceduralProps.proceduralID == 1000)
            glBindTexture(GL_TEXTURE_2D, this->ID);
        else
            glBindTexture(GL_TEXTURE_2D, this->proceduralProps.proceduralTextureID);

        unsigned int wrapParam = GL_REPEAT; 

        if(this->proceduralProps.proceduralMirroredRepeat)
            wrapParam = GL_MIRRORED_REPEAT; 

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapParam);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapParam);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, wrapParam);

        unsigned int FBO;
        glGenFramebuffers(1,&FBO);
        glBindFramebuffer(GL_FRAMEBUFFER,FBO);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, destTxtr.ID, 0);
        glViewport(0, 0, textureRes, textureRes);
        
        glClearColor(0,0,0,0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        mesh.Draw(false);

        Settings::defaultFramebuffer()->FBO.bind();
        glDeleteFramebuffers(1, &FBO);
    }

    // ------- Generating Normal Map -------
    if(this->proceduralProps.proceduralNormalMap){
        if(!normalMapRes.ID){
            normalMapRes = Texture(nullptr, textureRes, textureRes, GL_LINEAR);
        }
        else{
            normalMapRes.update(nullptr, textureRes, textureRes, GL_LINEAR);
        }

        destTxtr.generateNormalMap(normalMapRes.ID, textureRes, this->proceduralProps.proceduralNormalStrength, this->proceduralProps.proceduralNormalGrayScale); 

        char* normalMapData = new char[textureRes * textureRes * 4]; 
        normalMapRes.getData(normalMapData);
        destTxtr.update(normalMapData, textureRes, textureRes);
    }

    destTxtr.removeSeams(mesh, textureRes);
}

void Texture::generateNormalMap(unsigned int& normalMap, int textureResolution, float proceduralNormalStrength, bool proceduralNormalGrayScale){
    this->generateNormalMap(normalMap, glm::ivec2(textureResolution), proceduralNormalStrength, proceduralNormalGrayScale, false);
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

    LigidGL::makeDrawCall(GL_TRIANGLES, 0 , 6, "Texture::generateNormalMap : Render result");

    Settings::defaultFramebuffer()->FBO.bind();
    glDeleteFramebuffers(1, &FBO);
}


void Texture::applyNormalMap(float proceduralNormalStrength, bool proceduralNormalGrayScale, bool alphaMode){
    
    Texture tx = this->duplicateTexture();

    glm::ivec2 resolution = this->getResolution();
    
    Framebuffer FBO = Framebuffer(this->ID, GL_TEXTURE_2D, "Texture::applyNormalMap");
    FBO.bind();

    glViewport(0, 0, resolution.x, resolution.y);
    glClearColor(0,0,0,0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    getBox()->bindBuffers();
    
    ShaderSystem::heightToNormalMap().use();
    ShaderSystem::heightToNormalMap().setInt("heightMap", 0); glActiveTexture(GL_TEXTURE0); glBindTexture(GL_TEXTURE_2D, tx.ID);
    ShaderSystem::heightToNormalMap().setInt("alphaMode", alphaMode);
    ShaderSystem::heightToNormalMap().setInt("txtrRes", resolution.x);
    ShaderSystem::heightToNormalMap().setFloat("strength", proceduralNormalStrength);
    ShaderSystem::heightToNormalMap().setInt("grayScale", proceduralNormalGrayScale);
    ShaderSystem::heightToNormalMap().setMat4("projection", glm::ortho(0.f, 1.f, 1.f, 0.f));
    ShaderSystem::heightToNormalMap().setMat4("projectedPosProjection", glm::ortho(0.f, 1.f, 1.f, 0.f));
    ShaderSystem::heightToNormalMap().setVec3("pos", glm::vec3(0.5f, 0.5f, 0.9f));
    ShaderSystem::heightToNormalMap().setVec2("scale", glm::vec2(0.5f));

    // Draw
    LigidGL::makeDrawCall(GL_TRIANGLES, 0 , 6, "Texture::applyNormalMap : Rendering the result");

    // Finish
    Settings::defaultFramebuffer()->FBO.bind();
    FBO.deleteBuffers(false, false);
    glDeleteTextures(1, &tx.ID);
}

void Texture::generateProceduralDisplayingTexture(int displayingTextureRes, int displayMode){
    this->generateProceduralDisplayingTexture(displayingTextureRes, displayMode, 4.f, 272.f);
}

void Texture::generateProceduralDisplayingTexture(int displayingTextureRes, int displayMode, float radius, float rotation){
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

    //!LEAK
    this->update(nullptr, displayRes, displayRes);
    
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
    if(displayMode == 1 || displayMode == 2){
        Camera matCam;
        matCam.cameraPos = glm::vec3(radius * cos(glm::radians(rotation)), 0.0f, radius * sin(glm::radians(rotation)));;
        matCam.radius = radius;


        //Move the camera to the side
        glm::mat4 view = glm::lookAt(matCam.cameraPos,
                                     glm::vec3(0),
                                     glm::vec3(0.0, 1.0, 0.0));

        //The perspective projection matrix    
        glm::mat4 projectionMatrix = glm::perspective(
                                                        glm::radians(35.f), //Fov  
                                                        -1.f,  //Ratio (is 1 since the width & the height is equal to displayRes)
                                                        0.1f,    //Near
                                                        100.f  //Far (the material is pretty close to the camera actually  ) 
                                                    );

        //Use the 3D model rendering shader
        ShaderSystem::tdModelShader().use();

        //Throw the camera data to the shader
        ShaderSystem::tdModelShader().setInt("displayingMode", 0);
        ShaderSystem::tdModelShader().setVec3("viewPos",matCam.cameraPos);
        ShaderSystem::tdModelShader().setMat4("view",view);
        ShaderSystem::tdModelShader().setMat4("projection",projectionMatrix);
        ShaderSystem::tdModelShader().setInt("usingMeshSelection", false);
        ShaderSystem::tdModelShader().setInt("meshSelectionEditing", false);
        ShaderSystem::tdModelShader().setInt("hideUnselected", false);

        Model model;
        if(displayMode == 1)
            model = *getMaterialDisplayerModel();
        else if(displayMode == 2)
            model = *getScene()->model;

        for (size_t i = 0; i < model.meshes.size(); i++)
        {
            Texture proc = this->generateProceduralTexture(model.meshes[i], 416);
            
            glViewport(0, 0, displayRes, displayRes);
            glBindFramebuffer(GL_FRAMEBUFFER, FBO);
            ShaderSystem::tdModelShader().use();

            //Bind the channels of the material
            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D, proc.ID);
            glActiveTexture(GL_TEXTURE3);
            glBindTexture(GL_TEXTURE_2D, proc.ID);
            glActiveTexture(GL_TEXTURE4);
            glBindTexture(GL_TEXTURE_2D, proc.ID);
            glActiveTexture(GL_TEXTURE5);
            glBindTexture(GL_TEXTURE_2D, proc.ID);
            glActiveTexture(GL_TEXTURE6);
            glBindTexture(GL_TEXTURE_2D, proc.ID);
            glActiveTexture(GL_TEXTURE7);        
            glBindTexture(GL_TEXTURE_2D, proc.ID);

            model.meshes[i].Draw(false);
        }

        //Use the button shader (Is necessary since that process is done in the middle of GUI rendering) 
        ShaderSystem::buttonShader().use();

        getBox()->bindBuffers();
        //glDeleteTextures(1, &proc);
    }

    // Generate regular procedural textures
    if(this->proceduralProps.textureSelectionDialog_selectedMode != 3 && displayMode == 0){

        /* Displaying texture */
        ShaderSystem::proceduralDisplayerShader().use();
        glm::mat4 projection = glm::ortho(0.f, (float)displayRes, (float)displayRes, 0.f);
        ShaderSystem::proceduralDisplayerShader().setMat4("projection"  ,       projection);
        ShaderSystem::proceduralDisplayerShader().setVec3("pos"         ,       glm::vec3((float)displayRes / 2.f, (float)displayRes / 2.f,0.9f));
        ShaderSystem::proceduralDisplayerShader().setVec2("scale"       ,       glm::vec2((float)displayRes / 2.f));
        
        ShaderSystem::proceduralDisplayerShader().setInt("proceduralID", this->proceduralProps.proceduralID);                
        ShaderSystem::proceduralDisplayerShader().setFloat("proceduralScale", this->proceduralProps.proceduralScale);
        ShaderSystem::proceduralDisplayerShader().setFloat("proceduralStretch", this->proceduralProps.proceduralStretch);
        ShaderSystem::proceduralDisplayerShader().setFloat("proceduralScaleModelPos", this->proceduralProps.proceduralScaleModelPos);
        ShaderSystem::proceduralDisplayerShader().setInt("proceduralInverted", this->proceduralProps.proceduralnverted);
        ShaderSystem::proceduralDisplayerShader().setInt("proceduralGrayScale", this->proceduralProps.proceduralGrayScale);
        ShaderSystem::proceduralDisplayerShader().setFloat("proceduralBrightness", this->proceduralProps.proceduralBrightness);
        ShaderSystem::proceduralDisplayerShader().setInt("proceduralGrayScale", this->proceduralProps.proceduralGrayScale);
        ShaderSystem::proceduralDisplayerShader().setFloat("displayOpacity", 1.f);
        ShaderSystem::proceduralDisplayerShader().setInt("proceduralTexture", 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, this->proceduralProps.proceduralTextureID);

        unsigned int wrapParam = GL_REPEAT; 

        if(this->proceduralProps.proceduralMirroredRepeat)
            wrapParam = GL_MIRRORED_REPEAT; 

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapParam);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapParam);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, wrapParam);
        
        LigidGL::makeDrawCall(GL_TRIANGLES, 0, 6, "Texture::generateProceduralDisplayingTexture : Rendering procedural texture");
    }

    // Generate normal map
    if(this->proceduralProps.proceduralNormalMap && displayMode == 0){
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
    LGDTEXTURE_WRITEBITS(this->proceduralProps.proceduralMirroredRepeat, bool, "Property texture - proceduralMirroredRepeat");
    LGDTEXTURE_WRITEBITS(this->proceduralProps.proceduralStretch, float, "Property texture - proceduralStretch");
    LGDTEXTURE_WRITEBITS(this->proceduralProps.proceduralScaleModelPos, float, "Property texture - proceduralScaleModelPos");

    // -------- Procedural Texture Data --------
    std::string txtrTitle = "";
    for (size_t i = 0; i < Library::getTextureArraySize(); i++)
    {
        if(i < Library::getTextureArraySize()){
            if(Library::getTexture(i)->ID == this->proceduralProps.proceduralTextureID){
                if(i < Library::getTextureArraySize())
                    txtrTitle = Library::getTexture(i)->title;
            }
        }
    }
    
    int txtrTitleSize = txtrTitle.size();
    LGDTEXTURE_WRITEBITS(txtrTitleSize, int, "Property texture - Texture title size");
    for (size_t i = 0; i < txtrTitleSize; i++)
    {
        char c = txtrTitle[i];
        LGDTEXTURE_WRITEBITS(c, char, "Property texture - Texture title character");
    }
    
    return true;
}

#define LGDMATERIAL_READBITS(var, type, loc) if(!rf.read(reinterpret_cast<char*>(   &var     ), sizeof(type))){ \
                                LGDLOG::start<< "ERROR : Reading lgdmaterial file. Failed to read at : " << loc << LGDLOG::end;\
                                return false; \
                            }

bool Texture::readTextureData(std::ifstream& rf, bool threeDMode, unsigned int versionCode){

    // --------- Read procedural data ---------

    int proceduralID;
    float proceduralScale;
    int proceduralnverted;
    bool proceduralNormalMap;
    bool proceduralNormalGrayScale;
    float proceduralNormalStrength;
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
    bool proceduralMirroredRepeat;
    float proceduralStretch;
    float proceduralScaleModelPos;
    
    LGDMATERIAL_READBITS(proceduralID, int, "Property texture - procedural ID");
    LGDMATERIAL_READBITS(proceduralScale, float, "Property texture - procedural Scale");
    LGDMATERIAL_READBITS(proceduralnverted, int, "Property texture - procedural nverted");
    LGDMATERIAL_READBITS(proceduralNormalMap, bool, "Property texture - procedural NormalMap");
    LGDMATERIAL_READBITS(proceduralNormalGrayScale, bool, "Property texture - procedural NormalGrayScale");
    LGDMATERIAL_READBITS(proceduralNormalStrength, float, "Property texture - procedural NormalStrength");
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
    
    if(versionCode == 1){
        LGDMATERIAL_READBITS(proceduralMirroredRepeat, bool, "Property texture - proceduralMirroredRepeat");
        LGDMATERIAL_READBITS(proceduralStretch, float, "Property texture - proceduralStretch");
        proceduralScaleModelPos = 1.f;
    }
    else if(versionCode == 2){
        LGDMATERIAL_READBITS(proceduralMirroredRepeat, bool, "Property texture - proceduralMirroredRepeat");
        LGDMATERIAL_READBITS(proceduralStretch, float, "Property texture - proceduralStretch");
        LGDMATERIAL_READBITS(proceduralScaleModelPos, float, "Property texture - proceduralScaleModelPos");
    }
    else{
        proceduralMirroredRepeat = true;
        proceduralStretch = 1.f;
        proceduralScaleModelPos = 1.f;
    }

    // --------- Recalculate procedural ID ---------
    int MAX_PROCEDURAL_PATTERN_TEXTURE_SIZE = 66;
    int MAX_PROCEDURAL_NOISE_TEXTURE_SIZE = 50;
    int MAX_PROCEDURAL_SMART_TEXTURE_SIZE = 7;

    if(textureSelectionDialog_selectedMode == 1)
        proceduralID = textureSelectionDialog_selectedTextureIndex;
    else if(textureSelectionDialog_selectedMode == 2)
        proceduralID = textureSelectionDialog_selectedTextureIndex + MAX_PROCEDURAL_PATTERN_TEXTURE_SIZE;
    else if(textureSelectionDialog_selectedMode == 4)
        proceduralID = textureSelectionDialog_selectedTextureIndex + MAX_PROCEDURAL_PATTERN_TEXTURE_SIZE + MAX_PROCEDURAL_NOISE_TEXTURE_SIZE;

    // --------- Read texture data ---------
    std::string txtrTitle = "";
    int txtrTitleSize;
    LGDMATERIAL_READBITS(txtrTitleSize, int, "Property texture - Texture title size");
    for (size_t i = 0; i < txtrTitleSize; i++)
    {
        char c;
        LGDMATERIAL_READBITS(c, char, "Property texture - Texture title character");
        txtrTitle.push_back(c);
    }

    unsigned int proceduralTextureID = 0;
    for (size_t i = 0; i < Library::getTextureArraySize(); i++)
    {
        if(i < Library::getTextureArraySize()){
            if(Library::getTexture(i)->title == txtrTitle){
                if(i < Library::getTextureArraySize())
                    proceduralTextureID = Library::getTexture(i)->ID;
            }
        }
    }

    if(textureSelectionDialog_selectedMode == 5){
        proceduralID = -1;
        if(textureSelectionDialog_selectedTextureIndex < Library::getgetSrcLibTxtrsArraySize()){
            proceduralTextureID = Library::getSrcLibTxtr(textureSelectionDialog_selectedTextureIndex).getTexture().ID;
        }
    }

    // --------- Create the texture ---------

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
    this->proceduralProps.proceduralMirroredRepeat = proceduralMirroredRepeat;
    this->proceduralProps.proceduralStretch = proceduralStretch;
    this->proceduralProps.proceduralScaleModelPos = proceduralScaleModelPos;

    glGenTextures(1, &this->ID);

    this->generateProceduralDisplayingTexture(512, threeDMode);

    return true;
}

void Texture::flipTexture(bool horizontal, bool vertical){
    
    Texture copiedTxtr = this->duplicateTexture();
    
    glm::ivec2 txtrRes = this->getResolution();

    Framebuffer captureFBO = Framebuffer(*this, GL_TEXTURE_2D, "Texture::flipTexture");
    
    glClearColor(0,0,0,0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0, 0, txtrRes.x, txtrRes.y);

    glm::mat4 projection = glm::ortho(0.f, 1.f, 1.f, 0.f);
    if(horizontal && vertical)
        projection = glm::ortho(1.f, 0.f, 0.f, 1.f);
    else if(horizontal)
        projection = glm::ortho(1.f, 0.f, 1.f, 0.f);
    else if(vertical)
        projection = glm::ortho(0.f, 1.f, 0.f, 1.f);

    glm::vec3 pos = glm::vec3(0.5f, 0.5f, 0.9f);
    glm::vec2 scale = glm::vec2(0.5f, 0.5f);

    ShaderSystem::textureRenderingShader().use();
    ShaderSystem::textureRenderingShader().setMat4("projection", projection);
    ShaderSystem::textureRenderingShader().setVec3("pos", pos);
    ShaderSystem::textureRenderingShader().setVec2("scale", scale);
    ShaderSystem::textureRenderingShader().setFloat("rotation", 0.f);
    ShaderSystem::textureRenderingShader().setFloat("opacity", 1.f);
    ShaderSystem::textureRenderingShader().setInt("txtr", 0);
    ShaderSystem::textureRenderingShader().setFloat("depthToleranceValue", 0);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, copiedTxtr.ID);

    getBox()->bindBuffers();

    LigidGL::makeDrawCall(GL_TRIANGLES, 0, 6, "Texture::flipTexture : Rendering result");

    // Finish
    glDeleteTextures(1, &copiedTxtr.ID);
    captureFBO.deleteBuffers(false, false);
    Settings::defaultFramebuffer()->setViewport();
}

void Texture::resize(const glm::ivec2 newResolution){
    
    Texture copiedTxtr = this->duplicateTexture();

    this->update(nullptr, newResolution.x, newResolution.y);
    
    Framebuffer captureFBO = Framebuffer(*this, GL_TEXTURE_2D, "Texture::flipTexture");
    
    glClearColor(0,0,0,0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0, 0, newResolution.x, newResolution.y);

    glm::mat4 projection = glm::ortho(0.f, 1.f, 1.f, 0.f);
    glm::vec3 pos = glm::vec3(0.5f, 0.5f, 0.9f);
    glm::vec2 scale = glm::vec2(0.5f, 0.5f);

    ShaderSystem::textureRenderingShader().use();
    ShaderSystem::textureRenderingShader().setMat4("projection", projection);
    ShaderSystem::textureRenderingShader().setVec3("pos", pos);
    ShaderSystem::textureRenderingShader().setVec2("scale", scale);
    ShaderSystem::textureRenderingShader().setFloat("rotation", 0.f);
    ShaderSystem::textureRenderingShader().setFloat("opacity", 1.f);
    ShaderSystem::textureRenderingShader().setInt("txtr", 0);
    ShaderSystem::textureRenderingShader().setFloat("depthToleranceValue", 0);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, copiedTxtr.ID);

    getBox()->bindBuffers();

    LigidGL::makeDrawCall(GL_TRIANGLES, 0, 6, "Texture::resize : Rendering result");

    // Finish
    glDeleteTextures(1, &copiedTxtr.ID);
    captureFBO.deleteBuffers(false, false);
    Settings::defaultFramebuffer()->setViewport();
}

void Texture::copyDataToTheCopyContext(){
    glm::ivec2 res = this->getResolution();
    char* pxs = new char[res.x * res.y * 4]; 
    this->getData(pxs);

    if(getCopyContext()->window.makeContextCurrent()){
        mainThreadUsingCopyContext = true;

        glActiveTexture(GL_TEXTURE0);
        if(this->copyContextID == 0)
            glGenTextures(1,&this->copyContextID);

        //!LEAK
        Texture copyContextIDOBJ = Texture(copyContextID);
        copyContextIDOBJ.update(pxs, res.x, res.y);
        
        mainThreadUsingCopyContext = false;
        delete[] pxs;
    }
    
    getContext()->window.makeContextCurrent();
    
}

void Texture::mix(Texture txtr2, Texture mask, bool maskAlpha, bool normalMapMode, bool invert){
    
    glm::vec2 resolution = this->getResolution();
    
    Framebuffer FBO = Framebuffer(*this, GL_TEXTURE_2D, "Texture::mix");
    FBO.bind();
    glViewport(0,0,resolution.x,resolution.y);
    
    glm::mat4 projection = glm::ortho(0.f, 1.f, 1.f, 0.f);
    glm::vec3 pos = glm::vec3(1.f / 2.f, 1.f / 2.f, 0.9f);
    glm::vec2 scale = glm::vec2(1.f / 2.f, 1.f / 2.f);

    ShaderSystem::grayScaleIDMaskingShader().use();
    ShaderSystem::grayScaleIDMaskingShader().setMat4("projection", projection);
    ShaderSystem::grayScaleIDMaskingShader().setVec3("pos", pos);
    ShaderSystem::grayScaleIDMaskingShader().setVec2("scale", scale);
    
    ShaderSystem::grayScaleIDMaskingShader().setInt("maskTexture", 0);
    ShaderSystem::grayScaleIDMaskingShader().setInt("texture_black", 1);
    ShaderSystem::grayScaleIDMaskingShader().setInt("texture_white", 2);
    
    ShaderSystem::grayScaleIDMaskingShader().setInt("maskAlpha", maskAlpha);
    ShaderSystem::grayScaleIDMaskingShader().setInt("normalMapMode", normalMapMode);
    ShaderSystem::grayScaleIDMaskingShader().setInt("invert", invert);
    ShaderSystem::grayScaleIDMaskingShader().setFloat("offset", 0.5f);
    
    Texture copiedTxtr = this->duplicateTexture();
    FBO.bind();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, mask.ID);
    
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, copiedTxtr.ID);
    
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, txtr2.ID);
    
    LigidGL::makeDrawCall(GL_TRIANGLES, 0, 6, "Texture::mix : Rendering result");
    
    //Finish
    Settings::defaultFramebuffer()->FBO.bind();
    ShaderSystem::buttonShader().use();
    FBO.deleteBuffers(false, false);
    glDeleteTextures(1, &copiedTxtr.ID);
    Settings::defaultFramebuffer()->setViewport();
}