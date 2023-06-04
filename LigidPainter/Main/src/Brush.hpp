/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

Copyright (c) 2022-2023, LigidTools 

All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------

    Description header
    64bit 64bit 64bit
    0x4B4B9AAA
    0xABAB9ACC
    0x334A9FFF
        |
    sizeJitter(sizeof(float))
        |   
    scatter(sizeof(float))
        |    
    fade(sizeof(float))
        |    
    rotation(sizeof(float))
        |    
    rotationJitter(sizeof(float))
        |    
    alphaJitter(sizeof(float))
        |
    individualTexture(sizeof(bool))
        |
    sinWavePattern(sizeof(bool))
        |
    readTexture(sizeof(bool)) //If that bit is set to 1 prep to read raw texture data otherwise ignore the rest
        |(if set to 1)
    textureWidth(64bit)
        |
    textureHeight(64bit) (Texture always has 4 channels for now)
        |
    textureData(8bit * textureWidth * textureHeight * 4)

*/

#ifndef LGDBRUSH_HPP
#define LGDBRUSH_HPP

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

class Brush
{
private:
    #if defined(_WIN32) || defined(_WIN64)
		    char folderDistinguisher = '\\';
	#else
			char folderDistinguisher = '/'; 
	#endif
public:
    float sizeJitter;
    float scatter;
    float fade;
    float rotation;
    float rotationJitter;
    float alphaJitter;
    bool individualTexture;
    bool sinWavePattern;

    std::string title;

    unsigned int displayingTexture;

    Texture texture; //Brush texture

    void initDisplayingTexture(){
        glGenTextures(1,&displayingTexture);
        glActiveTexture(GL_TEXTURE0);

        glBindTexture(GL_TEXTURE_2D,displayingTexture);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 100, 100, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    Brush(){
        initDisplayingTexture();
    }
    Brush(
            float sizeJitter,
            float scatter,
            float fade,
            float rotation,
            float rotationJitter,
            float alphaJitter,
            bool individualTexture,
            bool sinWavePattern,
            std::string title,
            Texture texture
        )
    {
        this->sizeJitter = sizeJitter;
        this->scatter = scatter;
        this->fade = fade;
        this->rotation = rotation;
        this->rotationJitter = rotationJitter;
        this->alphaJitter = alphaJitter;
        this->individualTexture = individualTexture;
        this->sinWavePattern = sinWavePattern;
        this->title = title;
        this->texture = texture;

        initDisplayingTexture();
    }

    void saveFile(){
        //Saves the brush file
        std::string path = tinyfd_selectFolderDialog("Select a folder to export the brush file", "");
        if(path.size()){

            std::ofstream wf(path + folderDistinguisher + title + ".lgdbrush", std::ios::out | std::ios::binary);

		    if(!wf) {
                std::cout << "ERROR WHILE WRITING BRUSH FILE! Cannot open file : " << path << std::endl;
                return;
            }
            
            //!HEADER
            
            //!Description
            uint64_t h1 = 0x4B4B9AAA; 
            uint64_t h2 = 0xABAB9ACC; 
            uint64_t h3 = 0x334A9FFF; 

            wf.write(reinterpret_cast<char*>(   &h1    ),sizeof(uint64_t));
            wf.write(reinterpret_cast<char*>(   &h2    ),sizeof(uint64_t));
            wf.write(reinterpret_cast<char*>(   &h3    ),sizeof(uint64_t));
        
            //!Brush Data
            wf.write(reinterpret_cast<char*>(   &sizeJitter         ),sizeof(float));
            wf.write(reinterpret_cast<char*>(   &scatter            ),sizeof(float));
            wf.write(reinterpret_cast<char*>(   &fade               ),sizeof(float));
            wf.write(reinterpret_cast<char*>(   &rotation           ),sizeof(float));
            wf.write(reinterpret_cast<char*>(   &rotationJitter     ),sizeof(float));
            wf.write(reinterpret_cast<char*>(   &alphaJitter        ),sizeof(float));
            wf.write(reinterpret_cast<char*>(   &individualTexture  ),sizeof(bool));
            wf.write(reinterpret_cast<char*>(   &sinWavePattern     ),sizeof(bool));

            bool haveTexture = texture.ID; 
            
            wf.write(reinterpret_cast<char*>(   &haveTexture     ),sizeof(bool));

            if(haveTexture){
                //Write texture resolution
                uint64_t txtrWidth = texture.getResolution().x;
                uint64_t txtrHeight = texture.getResolution().y;
                wf.write(reinterpret_cast<char*>(   &txtrWidth     ),sizeof(uint64_t));
                wf.write(reinterpret_cast<char*>(   &txtrHeight     ),sizeof(uint64_t));

                //Get pixels of the texture
                char* pixels = new char[txtrWidth * txtrHeight * 4];
                texture.getData(pixels);
                
                //Write texture data
                wf.write(pixels , txtrWidth * txtrHeight * 4 * sizeof(char));

                delete[] pixels;
            }
        }
    }

    void readFile(std::string path){
        std::ifstream rf(path, std::ios::in | std::ios::binary);

        //Get the title(name) of the brush
        Util util;
        title = util.getLastWordBySeparatingWithChar(path,folderDistinguisher);
        title = util.removeExtension(title);

		if(!rf) {
            std::cout << "ERROR WHILE WRITING BRUSH FILE! Cannot open file : " << path << std::endl;
            return;
        }
            
        //!HEADER
            
        //!Description
        uint64_t h1; 
        uint64_t h2; 
        uint64_t h3; 

        rf.read(reinterpret_cast<char*>(   &h1    ),sizeof(uint64_t));
        rf.read(reinterpret_cast<char*>(   &h2    ),sizeof(uint64_t));
        rf.read(reinterpret_cast<char*>(   &h3    ),sizeof(uint64_t));

        if(
                h1 == 0x4B4B9AAA &&
                h2 == 0xABAB9ACC &&
                h3 == 0x334A9FFF 
            )
        {
            //!Brush Data
            rf.read(reinterpret_cast<char*>(   &sizeJitter         ),sizeof(float));
            rf.read(reinterpret_cast<char*>(   &scatter            ),sizeof(float));
            rf.read(reinterpret_cast<char*>(   &fade               ),sizeof(float));
            rf.read(reinterpret_cast<char*>(   &rotation           ),sizeof(float));
            rf.read(reinterpret_cast<char*>(   &rotationJitter     ),sizeof(float));
            rf.read(reinterpret_cast<char*>(   &alphaJitter        ),sizeof(float));
            rf.read(reinterpret_cast<char*>(   &individualTexture  ),sizeof(bool));
            rf.read(reinterpret_cast<char*>(   &sinWavePattern     ),sizeof(bool));

            bool haveTexture = false; 

            rf.read(reinterpret_cast<char*>(   &haveTexture     ),sizeof(bool));

            if(haveTexture){
                uint64_t w = 0,h = 0;
                //read texture resolution
                rf.read(reinterpret_cast<char*>(   &w     ),sizeof(uint64_t));
                rf.read(reinterpret_cast<char*>(   &h     ),sizeof(uint64_t));

                //Get pixels of the texture
                char* pixels = new char[w * h * 4];

                //read texture data
                rf.read(pixels , w * h * 4 * sizeof(char));

                glGenTextures(1,&texture.ID);
                glActiveTexture(GL_TEXTURE0);

                glBindTexture(GL_TEXTURE_2D,texture.ID);

                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);

		        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, w, h, 0, GL_RGBA, GL_BYTE, pixels);
                glGenerateMipmap(GL_TEXTURE_2D);

                delete[] pixels;
            }
        }
        else{
            std::cout << "ERROR WHILE WRITING BRUSH FILE! File is not recognised : " << path << std::endl;
            return;
        }
    }

    void updateDisplayTexture(Shader paintingShader, Shader buttonShader){
        int frameCounter = 0;
        
        double amplitude = 20.f;
        double lower = 45.f;

        std::vector<double> wave;

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
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, displayingTexture, 0);

        glClearColor(0,0,0,0);
        glClear(GL_COLOR_BUFFER_BIT);

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
};



#endif