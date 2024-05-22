/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

(c) 2024 Mert Tetik. All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------
*/

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <fstream>
#include <iostream>
#include <vector>

#include "GUI/Dialogs.hpp"

#include "UTIL/Util.hpp"
#include "UTIL/Shader/Shader.hpp"
#include "UTIL/Settings/Settings.hpp"
#include "UTIL/GL/GL.hpp"
#include "UTIL/Framebuffers/Framebuffer_Pool.hpp"

static void get_color_data(const int precision, int txtrWidth, int txtrHeight, unsigned char* pixels, int* detectedWhite, int* detectedRed, int* detectedGreen, int* detectedBlue, int* detectedPink, int* detectedYellow, int* detectedOrange, int* detectedCyan, int* detectedBlack){
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
}

static std::vector<glm::vec3> get_colors(Texture* txtr){
    std::vector<glm::vec3> res;

    //Get the resolution of the texture from the panel of the modifier
    int txtrWidth = txtr->getResolution().x;
    int txtrHeight = txtr->getResolution().y;

    //Get pixels of the texture
    unsigned char* pixels = new unsigned char[txtrWidth * txtrHeight * 4];
    txtr->getData(pixels);

    int detectedWhite = false, detectedRed = false, detectedGreen = false, detectedBlue = false, detectedPink = false, detectedYellow = false, detectedOrange = false, detectedCyan = false, detectedBlack = false;

    get_color_data(1, txtrWidth, txtrHeight, pixels, &detectedWhite, &detectedRed, &detectedGreen, &detectedBlue, &detectedPink, &detectedYellow, &detectedOrange, &detectedCyan, &detectedBlack);

    delete[] pixels;

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

std::vector<MaterialIDColor> Texture::getMaterialIDPalette(){
    
    if(!this->ID){
        LGDLOG::start << "ERROR : Texture::getMaterialIDPalette : Texture is not valid" << LGDLOG::end;
        return {};
    }
    
    /*
        Valid colors : White, Red, Green, Blue, Pink, Yellow, Orange, Cyan, Black
    */
    std::vector<glm::vec3> res = get_colors(this);
    
    std::vector<MaterialIDColor> materialIDColors;

    // Get already bound shader program to set it back at the end
    Shader already_bound_shader = ShaderUTIL::get_bound_shader();

    // Generate the masking texture for the each color 
    for (size_t i = 0; i < res.size(); i++)
    {
        // Generate the material ID color object
        MaterialIDColor materialIDColor;
        materialIDColor.color = res[i];
        materialIDColor.grayScaleTxtr = Texture((char*)nullptr, 512, 512);

        // Get the FBO
        Framebuffer FBO = FBOPOOL::requestFBO(materialIDColor.grayScaleTxtr, "Texture::getMaterialIDPalette");

        // Clear the color buffer
        glClearColor(0,0,0,0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Set shader
        ShaderSystem::colorIDMaskingShader().use();
        ShaderSystem::colorIDMaskingShader().setMat4("projection", glm::ortho(0.f,1.f,1.f,0.f));
        ShaderSystem::colorIDMaskingShader().setMat4("projectedPosProjection", glm::ortho(0.f,1.f,1.f,0.f));
        ShaderSystem::colorIDMaskingShader().setVec3("pos", glm::vec3(0.5f));
        ShaderSystem::colorIDMaskingShader().setVec2("scale", glm::vec2(0.5f));
        ShaderSystem::colorIDMaskingShader().setInt("IDTexture", GL::bindTexture_2D(this->ID, "Texture::getMaterialIDPalette"));
        ShaderSystem::colorIDMaskingShader().setVec3("wbr", glm::vec3(res[i] == glm::vec3(1.f,1.f,1.f), res[i] == glm::vec3(0.f,0.f,0.f), res[i] == glm::vec3(1.f,0.f,0.f)));
        ShaderSystem::colorIDMaskingShader().setVec3("pbc", glm::vec3(res[i] == glm::vec3(1.f,0.f,1.f), res[i] == glm::vec3(0.f,0.f,1.f), res[i] == glm::vec3(0.f,1.f,1.f)));
        ShaderSystem::colorIDMaskingShader().setVec3("gyo", glm::vec3(res[i] == glm::vec3(0.f,1.f,0.f), res[i] == glm::vec3(1.f,1.f,0.f), res[i] == glm::vec3(1.f,0.5f,0.f)));

        // Render result
        getBox()->draw("Texture::getMaterialIDPalette : Generating gray scale texture");

        // Release bound textures
        GL::releaseBoundTextures("Texture::getMaterialIDPalette");
        // Release the FBO
        FBOPOOL::releaseFBO(FBO);

        // Push the generated material ID color to the result vector
        materialIDColors.push_back(materialIDColor);
    }

    // Set the default shader program back
    already_bound_shader.use();

    // Return the result
    return materialIDColors;
}