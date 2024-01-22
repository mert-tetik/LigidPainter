/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

(c) 2024 Mert Tetik. All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------
*/

#include<glad/glad.h>
#include "LigidGL/LigidGL.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <filesystem>

#include "UTIL/Util.hpp"
#include "GUI/GUI.hpp"

Filter::Filter(){

}

int Filter::load(std::string path){
    if(!std::filesystem::is_regular_file(path)){
        
        return 0;
    }

    std::ifstream rf(path);

    if (!rf.is_open()) {
        LGDLOG::start << "ERROR : Reading filter file : Can't open the file : " << path << LGDLOG::end;
        return 0;
    }

    std::string firstLine;
    if (!std::getline(rf, firstLine)) {
        LGDLOG::start << "ERROR : Reading filter file : Can't read the header : " << path << LGDLOG::end;
        return 0;
    }
    
    if(firstLine != "#ligidFilter"){
        LGDLOG::start << "ERROR : Reading filter file : Can't locate the header : " << path << LGDLOG::end;
        return 0;
    }

    this->srcCode = "";
    this->srcCode += "#version 400 core\n";
    
    std::string line;
    while (std::getline(rf, line))
    {
        this->srcCode += line + "\n";
    }
    
    this->shader.loadShaderPS("LigidPainter/Resources/Shaders/aVert/2D_uniforms.vert", this->srcCode);

    this->generateDisplayingTexture(glm::vec2(256));

    this->title = UTIL::removeExtension(UTIL::getLastWordBySeparatingWithChar(path, UTIL::folderDistinguisher()));

    return 1;
}

void Filter::generateDisplayingTexture(glm::vec2 displayResolution){
    
    // This texture will be displayed
    Texture sampleTxtr = appTextures.filterDisplayerImage;
    // Resolution of the sample texture
    glm::vec2 txtrRes = sampleTxtr.getResolution();

    //Displaying resolution
    glm::vec2 displayRes = displayResolution;

    if(this->displayingTxtr.ID == 0)
        this->displayingTxtr = Texture(nullptr, displayRes.x, displayRes.y, GL_LINEAR);
    else
        this->displayingTxtr.update(nullptr, displayRes.x, displayRes.y, GL_LINEAR);

    //Create the framebuffer
    Framebuffer captureFBO = Framebuffer(this->displayingTxtr, GL_TEXTURE_2D, "Filter generateDisplayingTexture");

    //Clear the capture frame buffer(displaying texture) with color alpha zero
    glClearColor(0,0,0,0);
    glClear(GL_COLOR_BUFFER_BIT);

    glViewport(0, 0, displayRes.x, displayRes.y);
    
    this->shader.use();

    glm::vec2 scale = displayRes / glm::vec2(2);
    glm::vec3 pos = glm::vec3(displayRes / glm::vec2(2),1.f);
    glm::mat4 projection = glm::ortho(0.f, displayRes.x, displayRes.y, 0.f);
    this->shader.setVec2("scale", scale); 
    this->shader.setVec3("pos", pos); 
    this->shader.setMat4("projection", projection); 
    
    this->shader.setInt("txtr", 0); 
    this->shader.setVec2("txtrResolution", txtrRes); 
    this->shader.setFloat("strength", this->strength); 

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, sampleTxtr.ID);

    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    //Finish
    Settings::defaultFramebuffer()->FBO.bind();
    ShaderSystem::buttonShader().use();

    captureFBO.deleteBuffers(false, false);
}

void Filter::applyFilter(unsigned int txtr, Texture maskTexture, Texture maskTexture2){

    Texture txtrObject = txtr;
    glm::vec2 txtrRes = txtrObject.getResolution();

    Texture duplicatedTxtr = txtrObject.duplicateTexture();  

    //Displaying resolution
    glm::vec2 displayRes = txtrRes;
     
    //Create the framebuffer
    Framebuffer captureFBO = Framebuffer(txtr, GL_TEXTURE_2D, "Filter apply filter");
    captureFBO.bind();

    //Clear the capture frame buffer(displaying texture) with color alpha zero
    glClearColor(0,0,0,0);
    glClear(GL_COLOR_BUFFER_BIT);

    glViewport(0, 0, displayRes.x, displayRes.y);
    
    this->shader.use();

    glm::vec2 scale = displayRes / glm::vec2(2);
    glm::vec3 pos = glm::vec3(displayRes / glm::vec2(2),1.f);
    glm::mat4 projection = glm::ortho(0.f, displayRes.x, displayRes.y, 0.f);
    this->shader.setVec2("scale", scale); //Cover the screen
    this->shader.setVec3("pos", pos); //Cover the screen
    this->shader.setMat4("projection", projection); //Cover the screen
    
    this->shader.setInt("txtr", 0); //Cover the screen
    this->shader.setVec2("txtrResolution", txtrRes); //Cover the screen
    this->shader.setFloat("strength", this->strength); 

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, duplicatedTxtr.ID);

    glDrawArrays(GL_TRIANGLES, 0, 6);

    if(maskTexture.ID != 0 && glIsTexture(maskTexture.ID) == GL_TRUE){
        ShaderSystem::grayScaleIDMaskingShader().use();
        ShaderSystem::grayScaleIDMaskingShader().setMat4("projection", projection);
        ShaderSystem::grayScaleIDMaskingShader().setVec3("pos", pos);
        ShaderSystem::grayScaleIDMaskingShader().setVec2("scale", scale);
        ShaderSystem::grayScaleIDMaskingShader().setInt("maskTexture", 0);
        ShaderSystem::grayScaleIDMaskingShader().setInt("texture_black", 1);
        ShaderSystem::grayScaleIDMaskingShader().setInt("texture_white", 2);
        ShaderSystem::grayScaleIDMaskingShader().setFloat("offset", 0.5f); 
        ShaderSystem::grayScaleIDMaskingShader().setInt("maskAlpha", 0);
        ShaderSystem::grayScaleIDMaskingShader().setInt("normalMapMode", 0);
        ShaderSystem::grayScaleIDMaskingShader().setInt("invert", 0);
        
        Texture destTxtrObj = txtr;
        Texture copiedDestTxtr = destTxtrObj.duplicateTexture();
        
        captureFBO.bind();
        
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, maskTexture.ID);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, duplicatedTxtr.ID);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, copiedDestTxtr.ID);

        glDrawArrays(GL_TRIANGLES, 0, 6);
    }

    if(maskTexture2.ID != 0 && glIsTexture(maskTexture2.ID) == GL_TRUE){
        ShaderSystem::grayScaleIDMaskingShader().use();
        ShaderSystem::grayScaleIDMaskingShader().setMat4("projection", projection);
        ShaderSystem::grayScaleIDMaskingShader().setVec3("pos", pos);
        ShaderSystem::grayScaleIDMaskingShader().setVec2("scale", scale);
        ShaderSystem::grayScaleIDMaskingShader().setInt("maskTexture", 0);
        ShaderSystem::grayScaleIDMaskingShader().setInt("texture_black", 1);
        ShaderSystem::grayScaleIDMaskingShader().setInt("texture_white", 2);
        ShaderSystem::grayScaleIDMaskingShader().setFloat("offset", 0.5f); 
        ShaderSystem::grayScaleIDMaskingShader().setInt("maskAlpha", 0);
        
        Texture destTxtrObj = txtr;
        Texture copiedDestTxtr = destTxtrObj.duplicateTexture();
        
        captureFBO.bind();
        
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, maskTexture2.ID);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, duplicatedTxtr.ID);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, copiedDestTxtr.ID);

        glDrawArrays(GL_TRIANGLES, 0, 6);
    }
    

    //Finish
    Settings::defaultFramebuffer()->FBO.bind();
    ShaderSystem::buttonShader().use();

    glDeleteTextures(1, &duplicatedTxtr.ID);
    
    captureFBO.deleteBuffers(false, false);
}

#define LGDFILTER_WRITEBITS(var, type, loc) if(!wf.write(reinterpret_cast<char*>(   &var     ), sizeof(type))){ \
                                    LGDLOG::start << "ERROR : Writing texture data. Failed to write at : " << loc << LGDLOG::end;\
                                    return false; \
                                }

bool Filter::writeFilterData(std::ofstream& wf){
    int32_t srcCodeCharSize = this->srcCode.size();
    LGDFILTER_WRITEBITS(srcCodeCharSize, int32_t, "Filter source code character size");

    for (size_t i = 0; i < srcCodeCharSize; i++)
    {
        char c = this->srcCode[i];
        LGDFILTER_WRITEBITS(c, char, "Filter source code character");
    }

    LGDFILTER_WRITEBITS(this->strength, float, "Filter strength")

    return true;
}

#define LGDFILTER_READBITS(var, type, loc) if(!rf.read(reinterpret_cast<char*>(   &var     ), sizeof(type))){ \
                                LGDLOG::start << "ERROR : Reading lgdmaterial file. Failed to read at : " << loc << LGDLOG::end;\
                                return false; \
                            }

bool Filter::readFilterData(std::ifstream& rf){

    int32_t srcCodeCharSize;
    LGDFILTER_READBITS(srcCodeCharSize, int32_t, "Filter source code character size");

    this->srcCode = "";
    for (size_t i = 0; i < srcCodeCharSize; i++)
    {
        char c;
        LGDFILTER_READBITS(c, char, "Filter source code character");
        this->srcCode.push_back(c);
    }
    LGDFILTER_READBITS(this->strength, float, "Filter strength")

    this->shader.loadShaderPS("LigidPainter/Resources/Shaders/aVert/2D_uniforms.vert", this->srcCode);

    this->generateDisplayingTexture(glm::vec2(256));

    this->displayingTxtr.title = "ReadenFilterDisplayerTXTR";
    this->title = "ReadenFilter";

    return true;
}