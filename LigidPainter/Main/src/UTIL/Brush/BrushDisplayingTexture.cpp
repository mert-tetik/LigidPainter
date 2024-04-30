/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

(c) 2024 Mert Tetik. All rights reserved.

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
#include "UTIL/Shader/Shader.hpp"
#include "UTIL/Settings/Settings.hpp"
#include "UTIL/GL/GL.hpp"


// Function to interpolate between two glm::vec2 points
std::vector<glm::vec2> interpolate(const glm::vec2& p1, const glm::vec2& p2, int numPoints) {
    std::vector<glm::vec2> interpolatedPoints;
    
    for (int i = 0; i <= numPoints; ++i) {
        float t = static_cast<float>(i) / static_cast<float>(numPoints);
        glm::vec2 interpolatedPoint = p1 + t * (p2 - p1);
        interpolatedPoints.push_back(interpolatedPoint);
    }
    
    return interpolatedPoints;
}

static std::vector<glm::vec2> getWaveVector(double amplitude, double lower, int numInterpolationPoints){
    std::vector<glm::vec2> wave;
    
    for (int i = 10; i < 85; i++) {
        glm::vec2 p1(i, (std::sin(glm::radians(static_cast<float>(i) * 9)) * amplitude * (static_cast<float>(i) / 50.f)) + lower);
        
        if(numInterpolationPoints < 0){
            if(i % std::abs(numInterpolationPoints) == 0){
                wave.push_back(p1);
            }
        }
        else
            wave.push_back(p1);
        
        if (i < 84) {
            glm::vec2 p2(i + 1, (std::sin(glm::radians(static_cast<float>(i + 1) * 9)) * amplitude * (static_cast<float>(i + 1) / 50.f)) + lower);
            if(numInterpolationPoints >= 0){
                std::vector<glm::vec2> interpolatedPoints = interpolate(p1, p2, numInterpolationPoints);
                wave.insert(wave.end(), interpolatedPoints.begin(), interpolatedPoints.end());
            }
        }
    }

    return wave;
}

void Brush::updateDisplayTexture(float radius){
    
    int frameCounter = 0;
    
    // Create the wave array
    double amplitude = 20.f;
    double lower = 45.f;
    int numInterpolationPoints = ((0.1f - radius) * 50.f);
    if(this->properties.spacing > 2.f){
        numInterpolationPoints = -(this->properties.spacing / 10.f);
    }

    std::vector<glm::vec2> wave = getWaveVector(amplitude, lower, numInterpolationPoints);

    glm::vec2 displayRes = displayingTexture.getResolution();

    // Create and bind the capturing framebuffer
    Framebuffer captureFBO = Framebuffer(displayingTexture, GL_TEXTURE_2D, "Brush::updateDisplayTexture");
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
    ShaderSystem::twoDPainting().setFloat("brush.hardness", this->properties.hardness);
    ShaderSystem::twoDPainting().setFloat("brush.sizeJitter", 1.f - this->properties.sizeJitter);
    ShaderSystem::twoDPainting().setFloat("brush.scatter", 1.f - this->properties.scatter);
    ShaderSystem::twoDPainting().setFloat("brush.fade", this->properties.fade);
    ShaderSystem::twoDPainting().setFloat("brush.rotation", this->properties.rotation);
    ShaderSystem::twoDPainting().setFloat("brush.rotationJitter", 1.f - this->properties.rotationJitter);
    ShaderSystem::twoDPainting().setFloat("brush.alphaJitter", 1.f - this->properties.alphaJitter);
    ShaderSystem::twoDPainting().setInt("brush.individualTexture", this->properties.individualTexture);
    ShaderSystem::twoDPainting().setInt("brush.sinWavePattern", this->properties.sinWavePattern);

    const int strokeSize = 5;

    //Prepeare painting
    glDisable(GL_BLEND);
    glDepthFunc(GL_ALWAYS);
    glBlendFunc(GL_ONE,GL_ONE);
    glBlendEquationSeparate(GL_FUNC_ADD,GL_FUNC_ADD);	

    captureFBO.bind();

    for (int i = 0; i < wave.size() / strokeSize; i++)
    {

        ShaderSystem::twoDPainting().setInt("frame", i);

        Texture bgTxtr = displayingTexture.duplicateTexture();
        captureFBO.bind();

        glClear(GL_COLOR_BUFFER_BIT);

        ShaderSystem::twoDPainting().setInt("brush.txtr", 0); 

        GL::bindTexture_2D((this->properties.brushTexture.ID) ? this->properties.brushTexture.ID : appTextures.white.ID, 0, "Brush::updateDisplayingTexture");
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);
        GLfloat borderColor[] = { 0.f, 0.f, 0.f, 1.f };  // Replace r, g, b, a with the desired color values
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
        
        ShaderSystem::twoDPainting().setInt("bgTxtr", 1); GL::bindTexture_2D(bgTxtr.ID, 1, "Brush::updateDisplayingTexture");
        
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
        LigidGL::makeDrawCall(GL_TRIANGLES, 0, 6, "Brush::updateDisplayingTexture : Painting the stroke");

        getBox()->bindBuffers();

        glDeleteTextures(1, &bgTxtr.ID);
    }

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBlendEquationSeparate(GL_FUNC_ADD,GL_FUNC_ADD);
    glDepthFunc(GL_LEQUAL);
    
    //Finish
    Settings::defaultFramebuffer()->FBO.bind();
    Settings::defaultFramebuffer()->setViewport();
    ShaderSystem::buttonShader().use();

    captureFBO.deleteBuffers(false, false);
}