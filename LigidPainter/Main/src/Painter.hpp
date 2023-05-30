/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

Copyright (c) 2022-2023, LigidTools 

All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------
*/

#ifndef LGDPAINTER_HPP
#define LGDPAINTER_HPP

#include<glad/glad.h>
#include<GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

#include "Shader.hpp"
#include "Box.hpp"
#include "Renderer.h"
#include "Util.hpp"
#include "Timer.hpp"
#include "Texture.hpp"
#include "Color.hpp"

#include "Mouse.hpp"

#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
#include <cstdlib>
#include <filesystem>

#include "tinyfiledialogs.h"

class Painter
{
public:
    //3 selected colors to paint
    //These colors will be switched between & at the end the color1 will used to paint. 
    Color color1; 
    Color color2; 
    Color color3; 

    unsigned int RBO;
    unsigned int FBO;
    unsigned int paintingTexture;

    glm::vec2 videoScale;

    Shader paintingShader;
    Shader buttonShader;

    glm::mat4 projection;

    bool threeDimensionalMode = true;

    Painter(){
        
    }

    void initPainter(glm::vec2 videoScale, Shader paintingShader, Shader buttonShader){
        this->videoScale = videoScale;
        this->paintingShader = paintingShader;
        this->buttonShader = buttonShader;
        
        //Create the texture
        glActiveTexture(GL_TEXTURE0);
        glGenTextures(1,&paintingTexture);
        glBindTexture(GL_TEXTURE_2D,paintingTexture);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, videoScale.x, videoScale.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
        glGenerateMipmap(GL_TEXTURE_2D);

        //Create the framebuffer
        glGenFramebuffers(1,&FBO);
        glBindFramebuffer(GL_FRAMEBUFFER,FBO);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, paintingTexture, 0);
        
        //Create the renderbuffer
		glGenRenderbuffers(1,&RBO);
		glBindRenderbuffer(GL_RENDERBUFFER,RBO);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, videoScale.x, videoScale.y);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, RBO);

        glBindFramebuffer(GL_FRAMEBUFFER,0);
    }

    void loadColor1( ){
        changeColor(color1);
    }
    void loadColor2( ){
        changeColor(color2);
    }
    void loadColor3( ){
        changeColor(color3);
    }

    glm::vec2 lastCursorPos = glm::vec2(0); 
    void doPaint(Mouse mouse){
        glViewport(0,0,videoScale.x,videoScale.y);
        
        glBindFramebuffer(GL_FRAMEBUFFER,FBO);
        
        paintingShader.use();

        glm::vec2 scale = videoScale / glm::vec2(2);
        glm::vec3 pos = glm::vec3(videoScale / glm::vec2(2),1.f);
        projection = glm::ortho(0.f,videoScale.x,0.f,videoScale.y);
        paintingShader.setVec2("scale", scale); //Cover the screen
        paintingShader.setVec3("pos", pos); //Cover the screen
        paintingShader.setMat4("projection", projection); //Cover the screen

        std::vector<glm::vec2> holdLocations;
        
        glm::vec2 fstrokeLocation = glm::vec2(mouse.cursorPos);
		holdLocations.push_back(fstrokeLocation);
		
		//----------------------PAINTING----------------------\\

		int differenceBetweenMousePoints = glm::distance(mouse.cursorPos, lastCursorPos);

		//if(spacingRangeBarVal > -0.1)
		//	differenceBetweenMousePoints = 1; 

		float xposDif = (mouse.cursorPos.x - lastCursorPos.x) / differenceBetweenMousePoints;
		float yposDif = (mouse.cursorPos.y - lastCursorPos.y) / differenceBetweenMousePoints;

		for (size_t i = 0; i < differenceBetweenMousePoints; i++)
		{
			////differenceBetweenMousePoints > 10
			if (true) {
				mouse.cursorPos.x -= xposDif;
				mouse.cursorPos.y -= yposDif;
			}
			if(true){
				glm::vec2 strokeLocation = mouse.cursorPos;
				holdLocations.push_back(strokeLocation);
			}
		}

		paintingShader.setInt("posCount",holdLocations.size());

        for (int i = 0; i < holdLocations.size(); i++)
		{
			std::string target = "positions[" + std::to_string(i) + "]";
            paintingShader.setVec2(target,holdLocations[i]);
		}
        glDepthFunc(GL_ALWAYS);
        glBlendFunc(GL_ONE, GL_ONE);
		glBlendEquationSeparate(GL_MAX,GL_MAX);	
        
        glDrawArrays(GL_TRIANGLES,0,6);
        
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glBlendEquationSeparate(GL_FUNC_ADD,GL_FUNC_ADD);
        glDepthFunc(GL_LESS);

        //glClearColor(0,1,1,1);
        //glClear(GL_COLOR_BUFFER_BIT);

        //Finish
        glBindFramebuffer(GL_FRAMEBUFFER,0);
        lastCursorPos = mouse.cursorPos;
        buttonShader.use();

        //TODO SET BUTTON SHADER
    }
    
private:
    void changeColor(Color &color){
        unsigned char defRGB[3];
        const char* check = tinyfd_colorChooser("Select a color",NULL,defRGB,defRGB);
        if(check)
            color.loadHex(check);
    }
};


#endif