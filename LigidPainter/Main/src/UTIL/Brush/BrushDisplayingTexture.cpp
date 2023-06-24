/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

Copyright (c) 2022-2023, LigidTools 

All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------

    Renders a curvy stroke to a texture using brush data

*/

#include<glad/glad.h>
#include<GLFW/glfw3.h>

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

void Brush::updateDisplayTexture(Shader paintingShader, Shader buttonShader){
    
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
    glm::vec2 videoScale = glm::vec2(100);

    glViewport(0,0,videoScale.x,videoScale.y);
    
    paintingShader.use();

    glm::vec2 scale = videoScale / glm::vec2(2);
    glm::vec3 pos = glm::vec3(videoScale / glm::vec2(2),1.f);
    glm::mat4 projection = glm::ortho(0.f,videoScale.x,0.f,videoScale.y);
    paintingShader.setVec2("scale", scale); //Cover the screen
    paintingShader.setVec3("pos", pos); //Cover the screen
    paintingShader.setMat4("projection", projection); //Cover the screen

    //Set properties
    paintingShader.setFloat("brush.hardness", 1.f);
    paintingShader.setFloat("brush.sizeJitter", sizeJitter);
    paintingShader.setFloat("brush.scatter", scatter);
    paintingShader.setFloat("brush.fade", fade);
    paintingShader.setFloat("brush.rotation", rotation);
    paintingShader.setFloat("brush.rotationJitter", rotationJitter);
    paintingShader.setFloat("brush.alphaJitter", alphaJitter);
    paintingShader.setInt("brush.individualTexture", individualTexture);
    paintingShader.setInt("brush.sinWavePattern", sinWavePattern);
    paintingShader.setFloat("brush.txtr", 0);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D,texture.ID);

    paintingShader.setVec2("videoScale", videoScale); 

    //Create the stroke
    for (size_t i = 0; i < wave.size(); i++)
    {
        paintingShader.setInt("frame", frameCounter);
        
        paintingShader.setFloat("brush.radius", frameCounter/7.5f);
        //Stroke positions
        std::vector<glm::vec2> holdLocations;
        holdLocations.push_back(glm::vec2(frameCounter + 10,wave[i]));
        paintingShader.setInt("posCount",holdLocations.size());
        for (int i = 0; i < holdLocations.size(); i++)
        {
            std::string target = "positions[" + std::to_string(i) + "]";
            paintingShader.setVec2(target,holdLocations[i]);
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
    buttonShader.use();

    glDeleteFramebuffers(1,&captureFBO);
}