/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

Copyright (c) 2022-2023, Mert Tetik

All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------

    Renders a curvy stroke to a texture using brush data

*/

#include<glad/glad.h>
#include "LigidGL/LigidGL.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <filesystem>

#include "UTIL/Util.hpp"
#include "ShaderSystem/Shader.hpp"
#include "SettingsSystem/Settings.hpp"

static std::vector<glm::vec2> getWaveVector(double amplitude, double lower){
    std::vector<glm::vec2> wave;
    
    for (int i = 10; i < 85; i++)
    {
        wave.push_back
            (
                glm::vec2(i, (std::sin(glm::radians(float(i*9))) * amplitude * (i/50.f)) + lower)
            );
    }

    return wave;
}

void Brush::updateDisplayTexture(float radius, float hardness){
    
    int frameCounter = 0;
    
    // Create the wave array
    double amplitude = 20.f;
    double lower = 45.f;
    std::vector<glm::vec2> wave = getWaveVector(amplitude, lower);

    glm::vec2 displayRes = displayingTexture.getResolution();

    // Create and bind the capturing framebuffer
    Framebuffer captureFBO = Framebuffer(displayingTexture, GL_TEXTURE_2D);
    captureFBO.bind();

    glClearColor(0,0,0,0);
    glClear(GL_COLOR_BUFFER_BIT);

    //Cover the whole monitor (since we are painting to the screen)
    glViewport(0, 0, displayRes.x, displayRes.y);
    
    glm::vec2 scale = glm::vec2(0.5f);
    glm::vec3 pos = glm::vec3(0.5f, 0.5f, 1.f);
    glm::mat4 projection = glm::ortho(0.f, 1.f, 0.f, 1.f);
    
    //Prepeare the 2D painting shader
    ShaderSystem::twoDPainting().use();
    ShaderSystem::twoDPainting().setVec2("scale", scale); //Cover the screen
    ShaderSystem::twoDPainting().setVec3("pos", pos); //Cover the screen
    ShaderSystem::twoDPainting().setMat4("projection", projection); //Cover the screen
    ShaderSystem::twoDPainting().setVec2("paintingRes", displayRes); 
    ShaderSystem::twoDPainting().setVec2("videoScale", displayRes); 
    ShaderSystem::twoDPainting().setInt("redChannelOnly", false);
    ShaderSystem::twoDPainting().setVec3("rgbClr", glm::vec3(1.f)); //Cover the screen

    //Set brush properties
    ShaderSystem::twoDPainting().setFloat("brush.radius", radius);
    ShaderSystem::twoDPainting().setFloat("brush.hardness", hardness);
    ShaderSystem::twoDPainting().setFloat("brush.sizeJitter", 1.f - sizeJitter);
    ShaderSystem::twoDPainting().setFloat("brush.scatter", 1.f - scatter);
    ShaderSystem::twoDPainting().setFloat("brush.fade", 1.f - fade);
    ShaderSystem::twoDPainting().setFloat("brush.rotation", rotation);
    ShaderSystem::twoDPainting().setFloat("brush.rotationJitter", 1.f - rotationJitter);
    ShaderSystem::twoDPainting().setFloat("brush.alphaJitter", 1.f - alphaJitter);
    ShaderSystem::twoDPainting().setInt("brush.individualTexture", individualTexture);
    ShaderSystem::twoDPainting().setInt("brush.sinWavePattern", sinWavePattern);

    const int strokeSize = 5;

    //Prepeare painting
    glDisable(GL_BLEND);
    glDepthFunc(GL_ALWAYS);
    glBlendFunc(GL_ONE,GL_ONE);
    glBlendEquationSeparate(GL_FUNC_ADD,GL_FUNC_ADD);	
    
    for (size_t i = 0; i < wave.size() / strokeSize; i++)
    {
        ShaderSystem::twoDPainting().setInt("frame", i * strokeSize);

        Texture bgTxtr = displayingTexture.duplicateTexture();
        captureFBO.bind();

        glClear(GL_COLOR_BUFFER_BIT);

        ShaderSystem::twoDPainting().setInt("bgTxtr", 1); 
        glActiveTexture(GL_TEXTURE1); 
        glBindTexture(GL_TEXTURE_2D, bgTxtr.ID);

        ShaderSystem::twoDPainting().setInt("brush.txtr", 0); glActiveTexture(GL_TEXTURE0); glBindTexture(GL_TEXTURE_2D, this->texture.ID);
        
        //Stroke positions
        std::vector<glm::vec2> strokes;
        strokes.assign(wave.begin() + i * strokeSize, wave.begin() + i * strokeSize + strokeSize);
        
        // Set the mouseOffset value
        if(strokes.size())
            ShaderSystem::twoDPainting().setVec2("mouseOffset", strokes[0] - strokes[strokes.size()-1]);
        
        // Set the stroke positions
        ShaderSystem::twoDPainting().setInt("posCount",strokes.size());
        for (int i = 0; i < strokes.size(); i++)
        {
            std::string target = "positions[" + std::to_string(i) + "]";
            ShaderSystem::twoDPainting().setVec2(target,strokes[i]);
        }
        
        //Painting
        glDrawArrays(GL_TRIANGLES,0,6);
        
    }

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBlendEquationSeparate(GL_FUNC_ADD,GL_FUNC_ADD);
    glDepthFunc(GL_LESS);
    
    //Finish
    Settings::defaultFramebuffer()->FBO.bind();
    Settings::defaultFramebuffer()->setViewport();
    ShaderSystem::buttonShader().use();

    captureFBO.deleteBuffers(false, false);
}