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
#include "UTIL/Library/Library.hpp"
#include "UTIL/GL/GL.hpp"

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
        this->displayingTxtr = Texture((char*)nullptr, displayRes.x, displayRes.y, GL_LINEAR);
    else
        this->displayingTxtr.update((char*)nullptr, displayRes.x, displayRes.y, GL_LINEAR);

    //Create the framebuffer
    Framebuffer FBO = FBOPOOL::requestFBO(this->displayingTxtr, "Filter generateDisplayingTexture");

    //Clear the capture frame buffer(displaying texture) with color alpha zero
    glClearColor(0,0,0,0);
    glClear(GL_COLOR_BUFFER_BIT);
    
    this->shader.use();

    glm::vec2 scale = displayRes / glm::vec2(2);
    glm::vec3 pos = glm::vec3(displayRes / glm::vec2(2),1.f);
    glm::mat4 projection = glm::ortho(0.f, displayRes.x, displayRes.y, 0.f);
    this->shader.setVec2("scale", scale); 
    this->shader.setVec3("pos", pos); 
    this->shader.setMat4("projection", projection); 
    
    this->shader.setInt("txtr", GL::bindTexture_2D(sampleTxtr.ID, "Filter::generateDisplayingTexture"));
    this->shader.setVec2("txtrResolution", txtrRes); 
    this->shader.setFloat("strength", this->strength); 

    getBox()->draw("Filter::generatingDisplayingTexture");

    //Finish
    GL::releaseBoundTextures("Filter::generateDisplayingTexture");
    ShaderUTIL::release_bound_shader();
    FBOPOOL::releaseFBO(FBO);
}

void Filter::applyFilter(Texture txtr, Texture maskTexture, Texture maskTexture2){

    glm::vec2 txtrRes = txtr.getResolution();
    //Displaying resolution
    glm::vec2 displayRes = txtrRes;

    Texture duplicatedTxtr = txtr.get_temp_copy_txtr("applyFilter");  

    //Create the framebuffer
    Framebuffer FBO = FBOPOOL::requestFBO(txtr, "Filter generateDisplayingTexture");

    //Clear the capture frame buffer(displaying texture) with color alpha zero
    glClearColor(0,0,0,0);
    glClear(GL_COLOR_BUFFER_BIT);

    this->shader.use();

    this->shader.setVec2("scale", glm::vec2(0.5f)); //Cover the screen
    this->shader.setVec3("pos", glm::vec3(0.5f)); //Cover the screen
    this->shader.setMat4("projection", glm::ortho(0.f, 1.f, 1.f, 0.f)); //Cover the screen
    
    this->shader.setInt("txtr", GL::bindTexture_2D(duplicatedTxtr.ID, "Filter::applyFilter"));
    this->shader.setVec2("txtrResolution", txtrRes); 
    this->shader.setFloat("strength", this->strength); 

    getBox()->draw("Filter::applyFilter : applying filter");

    GL::releaseBoundTextures("Filter::applyFilter");
    FBOPOOL::releaseFBO(FBO);

    if(maskTexture.ID && glIsTexture(maskTexture.ID) == GL_TRUE){
        txtr.mix(duplicatedTxtr, maskTexture, false, false, true);
    }
    if(maskTexture2.ID && glIsTexture(maskTexture2.ID) == GL_TRUE){
        txtr.mix(duplicatedTxtr, maskTexture2, false, false, true);
    }

    //Finish
    ShaderUTIL::release_bound_shader();
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

bool Filter::readFilterData(std::ifstream& rf, bool dont_gen_buffers){

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

    bool lib_match = false;
    for (size_t i = 0; i < Library::getFilterArraySize(); i++)
    {
        if(Library::getFilter(i)->srcCode == this->srcCode){
            this->shader = Library::getFilter(i)->shader;
            lib_match = true;
            break;
        }
    }

    if(!lib_match){
        //if(!dont_gen_buffers)
        //    this->shader.loadShaderPS("LigidPainter/Resources/Shaders/aVert/2D_uniforms.vert", this->srcCode);
    }

    //if(!dont_gen_buffers)
    //    this->generateDisplayingTexture(glm::vec2(256));

    this->displayingTxtr.title = "ReadenFilterDisplayerTXTR";
    this->title = "ReadenFilter";

    return true;
}