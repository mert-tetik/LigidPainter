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

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <filesystem>

#include "UTIL/Util.hpp"
#include "ShaderSystem/Shader.hpp"

void Brush::updateDisplayTexture(){
    
    int frameCounter = 0;
    
    //Stroke properties
    double amplitude = 20.f;
    double lower = 45.f;

    //Wave array
    std::vector<double> wave;

    //Create the wave array
    for (int i = 10; i < 85; i++)
    {
        wave.push_back
            (
                (std::sin(glm::radians(float(i*9))) * amplitude * (i/50.f)) + lower
            );
    }
    
    //Create the framebuffer
    unsigned int captureFBO;
    glGenFramebuffers(1,&captureFBO);
    glBindFramebuffer(GL_FRAMEBUFFER,captureFBO);
    
    //Bind the displaying texture to the capture framebuffer
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, displayingTexture, 0);

    //Clear the capture frame buffer(displaying texture) with color alpha zero
    glClearColor(0,0,0,0);
    glClear(GL_COLOR_BUFFER_BIT);

    //Stroke resolution is 100
    glm::vec2 displayRes = glm::vec2(100);

    glViewport(0,0,displayRes.x,displayRes.y);
    
    ShaderSystem::twoDPainting().use();

    glm::vec2 scale = displayRes / glm::vec2(2);
    glm::vec3 pos = glm::vec3(displayRes / glm::vec2(2),1.f);
    glm::mat4 projection = glm::ortho(0.f,displayRes.x,0.f,displayRes.y);
    ShaderSystem::twoDPainting().setVec2("scale", scale); //Cover the screen
    ShaderSystem::twoDPainting().setVec3("pos", pos); //Cover the screen
    ShaderSystem::twoDPainting().setMat4("projection", projection); //Cover the screen

    //Set properties
    ShaderSystem::twoDPainting().setFloat("brush.hardness", 1.f);
    ShaderSystem::twoDPainting().setFloat("brush.sizeJitter", sizeJitter);
    ShaderSystem::twoDPainting().setFloat("brush.scatter", scatter);
    ShaderSystem::twoDPainting().setFloat("brush.fade", fade);
    ShaderSystem::twoDPainting().setFloat("brush.rotation", rotation);
    ShaderSystem::twoDPainting().setFloat("brush.rotationJitter", rotationJitter);
    ShaderSystem::twoDPainting().setFloat("brush.alphaJitter", alphaJitter);
    ShaderSystem::twoDPainting().setInt("brush.individualTexture", individualTexture);
    ShaderSystem::twoDPainting().setInt("brush.sinWavePattern", sinWavePattern);
    ShaderSystem::twoDPainting().setFloat("brush.txtr", 0);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D,texture.ID);

    ShaderSystem::twoDPainting().setVec2("videoScale", displayRes); 

    //Create the stroke
    for (size_t i = 0; i < wave.size(); i++)
    {
        ShaderSystem::twoDPainting().setInt("frame", frameCounter);
        
        ShaderSystem::twoDPainting().setFloat("brush.radius", frameCounter/7.5f);
        //Stroke positions
        std::vector<glm::vec2> holdLocations;
        holdLocations.push_back(glm::vec2(frameCounter + 10,wave[i]));
        ShaderSystem::twoDPainting().setInt("posCount",holdLocations.size());
        for (int i = 0; i < holdLocations.size(); i++)
        {
            std::string target = "positions[" + std::to_string(i) + "]";
            ShaderSystem::twoDPainting().setVec2(target,holdLocations[i]);
        }

        glDepthFunc(GL_ALWAYS);
        glBlendFunc(GL_ONE,GL_ONE);
        glBlendEquationSeparate(GL_FUNC_ADD,GL_FUNC_ADD);	
        //glDisable(GL_BLEND);

        glDrawArrays(GL_TRIANGLES,0,6);

        //glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glBlendEquationSeparate(GL_FUNC_ADD,GL_FUNC_ADD);
        glDepthFunc(GL_LESS);

        frameCounter++;
    }
    
    //Finish
    glBindFramebuffer(GL_FRAMEBUFFER,0);
    ShaderSystem::buttonShader().use();

    glDeleteFramebuffers(1,&captureFBO);
}