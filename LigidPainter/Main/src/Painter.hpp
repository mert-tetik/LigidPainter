/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

Copyright (c) 2022-2023, LigidTools 

All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------

Brush Properties - 
    Hardness if default circle
    Radius

    Size Jitter
        Control : Off & Fade
        Min Diameter
    Rotation Jitter

    Scatter
    Count
    Count Jitter

    Texture

    Alpha Channel Jitter

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
    int selectedColorIndex = 0;
    Texture brushTexture;


    unsigned int RBO;
    unsigned int FBO;
    unsigned int paintingTexture; //Is paintingTexture8 or paintingTexture16f 
    unsigned int paintingTexture8;
    unsigned int paintingTexture16f;//Used for smear brush


    unsigned int depthTexture;
    bool updateTheDepthTexture = false;

    int selectedTextureIndex = 0;

    int selectedPaintingModeIndex = 0; 

    glm::vec2 videoScale;

    Shader paintingShader;
    Shader buttonShader;
    Shader tdModelShader;
    Shader depth3DShader;

    bool refreshable = true; //To avoid refreshing every frame in UI.hpp

    glm::mat4 projection;

    bool threeDimensionalMode = true;
    glm::mat4 windowProjection;
    glm::vec3 pos2D;
    glm::vec2 scale2D;

    Painter(){
        
    }

    void initPainter(glm::vec2 videoScale, Shader paintingShader, Shader buttonShader, Shader tdModelShader,Shader depth3DShader){
        this->videoScale = videoScale;
        this->paintingShader = paintingShader;
        this->buttonShader = buttonShader;
        this->tdModelShader = tdModelShader;
        this->depth3DShader = depth3DShader;
        
        //Create the texture
        glActiveTexture(GL_TEXTURE0);
        glGenTextures(1,&paintingTexture8);
        glBindTexture(GL_TEXTURE_2D,paintingTexture8);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, videoScale.x, videoScale.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
        glGenerateMipmap(GL_TEXTURE_2D);

        glGenTextures(1,&paintingTexture16f);
        glBindTexture(GL_TEXTURE_2D,paintingTexture16f);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, videoScale.x, videoScale.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
        glGenerateMipmap(GL_TEXTURE_2D);

        glGenTextures(1,&depthTexture);
        glBindTexture(GL_TEXTURE_2D,depthTexture);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, videoScale.x, videoScale.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
        glGenerateMipmap(GL_TEXTURE_2D);

        //Create the framebuffer
        glGenFramebuffers(1,&FBO);
        glBindFramebuffer(GL_FRAMEBUFFER,FBO);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, paintingTexture8, 0);
        
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

    glm::vec2 startCursorPos = glm::vec2(0); 
    glm::vec2 lastCursorPos = glm::vec2(0); 

    int frameCounter = 0;
    void doPaint(Mouse mouse,float radius,float hardness,float opacity,float spacing,float sizeJitter,float scatter,float fade,float rotation,
                 float rotationJitter,float alphaJitter,bool individualTexture,bool sinWavePattern,std::vector<Texture> textures){
        glm::vec2 firstCursorPos = mouse.cursorPos;
        if(mouse.LClick){
            startCursorPos = mouse.cursorPos;
            lastCursorPos = mouse.cursorPos;
            frameCounter = 0;
        }

        glViewport(0,0,videoScale.x,videoScale.y);
        
        glBindFramebuffer(GL_FRAMEBUFFER,FBO);
        if(selectedPaintingModeIndex == 2){
            paintingTexture = paintingTexture16f;
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, paintingTexture16f, 0);
        }
        else{
            paintingTexture = paintingTexture8;
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, paintingTexture8, 0);
        }
        
        paintingShader.use();

        glm::vec2 scale = videoScale / glm::vec2(2);
        glm::vec3 pos = glm::vec3(videoScale / glm::vec2(2),1.f);
        projection = glm::ortho(0.f,videoScale.x,0.f,videoScale.y);
        
        paintingShader.setVec2("scale", scale); //Cover the screen
        paintingShader.setVec3("pos", pos); //Cover the screen
        paintingShader.setMat4("projection", projection); //Cover the screen

        //Set properties
        paintingShader.setFloat("brush.radius", radius);
        paintingShader.setFloat("brush.hardness", hardness);
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
        glBindTexture(GL_TEXTURE_2D,brushTexture.ID);

        paintingShader.setInt("frame", frameCounter);

        //3D Model shader side of the painting        
        tdModelShader.use();
        tdModelShader.setInt("brushModeState", selectedPaintingModeIndex);
        if(selectedColorIndex == 0)
            tdModelShader.setVec3("paintingColor", color1.RGB / glm::vec3(255.f));
        if(selectedColorIndex == 1)
            tdModelShader.setVec3("paintingColor", color2.RGB / glm::vec3(255.f));
        if(selectedColorIndex == 2)
            tdModelShader.setVec3("paintingColor", color3.RGB / glm::vec3(255.f));
        tdModelShader.setFloat("paintingOpacity", opacity);
        
        paintingShader.use();
        paintingShader.setVec2("videoScale", videoScale); 
        paintingShader.setVec2("mouseOffset", mouse.mouseOffset);


        //Stroke positions
        std::vector<glm::vec2> holdLocations = getCursorSubstitution(mouse,spacing);
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

        if(glm::distance(startCursorPos,mouse.cursorPos) > spacing){
            startCursorPos = mouse.cursorPos;            
            glDrawArrays(GL_TRIANGLES,0,6);
        }
        
        //glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glBlendEquationSeparate(GL_FUNC_ADD,GL_FUNC_ADD);
        glDepthFunc(GL_LESS);

        
        //Finish
        glBindFramebuffer(GL_FRAMEBUFFER,0);
        lastCursorPos = firstCursorPos;
        buttonShader.use();
        
        refreshable = true;
        
        frameCounter++;

        //TODO SET BUTTON SHADER
    }

    void refreshPainting(){
        glBindFramebuffer(GL_FRAMEBUFFER,FBO);
        glClearColor(0,0,0,0);
        glClear(GL_COLOR_BUFFER_BIT);
        glBindFramebuffer(GL_FRAMEBUFFER,0);
    }
    
    void updateTexture(std::vector<Texture> &textures, Model &model,int textureRes){
        unsigned int captureFBO;
        unsigned int captureTexture;
        
        //Create the texture
        glActiveTexture(GL_TEXTURE0);
        glGenTextures(1,&captureTexture);
        glBindTexture(GL_TEXTURE_2D,captureTexture);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, textureRes, textureRes, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
        glGenerateMipmap(GL_TEXTURE_2D);

        //Create the framebuffer
        glGenFramebuffers(1,&captureFBO);
        glBindFramebuffer(GL_FRAMEBUFFER,captureFBO);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, captureTexture, 0);
        
        glActiveTexture(GL_TEXTURE2);
        if(textures.size())
            glBindTexture(GL_TEXTURE_2D, textures[selectedTextureIndex].ID);

        glViewport(0,0,textureRes,textureRes);

        tdModelShader.use();
        
        glm::mat4 orthoProjection = glm::ortho(0.f,1.f,0.f,1.f);
        tdModelShader.setMat4("oneZeroProjection",orthoProjection);
        
        if(threeDimensionalMode){
            tdModelShader.setInt("useTransformUniforms",0);
            tdModelShader.setInt("returnSingleTxtr",1);
            tdModelShader.setInt("render2D",1);
            model.meshes[0].Draw(); //TODO SELECT THE MESH
            tdModelShader.setInt("render2D",0);
            tdModelShader.setInt("returnSingleTxtr",0);
        }
        else{
            tdModelShader.setInt("useTransformUniforms",1);
            tdModelShader.setInt("returnSingleTxtr",1);
            tdModelShader.setVec2("scale",scale2D);
            tdModelShader.setVec3("pos",pos2D);
            tdModelShader.setInt("render2D",1);
            glDrawArrays(GL_TRIANGLES,0,6);
            tdModelShader.setInt("render2D",0);
            tdModelShader.setInt("useTransformUniforms",0);
            tdModelShader.setInt("returnSingleTxtr",0);
        }

        //Finish
        glDeleteFramebuffers(1,&captureFBO);
        
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D,captureTexture);
        char* pixels = new char[textureRes * textureRes * 4];
		glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_BYTE, pixels);
        if(textures.size())
            glBindTexture(GL_TEXTURE_2D,textures[selectedTextureIndex].ID);
        
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, textureRes, textureRes, 0, GL_RGBA, GL_BYTE, pixels);
        glGenerateMipmap(GL_TEXTURE_2D);
        
        delete[] pixels;
    }

    void updateDepthTexture( Model &model){
        unsigned int captureFBO;
        glGenFramebuffers(1,&captureFBO);
        glBindFramebuffer(GL_FRAMEBUFFER,captureFBO);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, depthTexture, 0);

        glClearColor(0,0,0,0);
        glClear(GL_COLOR_BUFFER_BIT);
        
        depth3DShader.use();
        depth3DShader.setInt("useTransformUniforms",0);

        glm::mat4 modelMatrix = glm::mat4(1);
        depth3DShader.setMat4("modelMatrix",modelMatrix);

        model.meshes[0].Draw();//TODO SELECT THE MESH

        buttonShader.use();

        glDeleteFramebuffers(1,&captureFBO);
    }
private:
    std::vector<glm::vec2> getCursorSubstitution(Mouse &mouse,float spacing){
        std::vector<glm::vec2> holdLocations;
        
        glm::vec2 fstrokeLocation = glm::vec2(mouse.cursorPos);
		holdLocations.push_back(fstrokeLocation);
		
		//----------------------PAINTING----------------------\\

		int differenceBetweenMousePoints = glm::distance(mouse.cursorPos, lastCursorPos);
        if(spacing > 5)
		    differenceBetweenMousePoints = 1; 

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
        return holdLocations;
    }


    void changeColor(Color &color){
        unsigned char defRGB[3];
        const char* check = tinyfd_colorChooser("Select a color",color.hex.c_str(),defRGB,defRGB);
        if(check)
            color.loadHex(check);
    }
};


#endif