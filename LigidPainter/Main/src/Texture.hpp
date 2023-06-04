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

#ifndef LGDSTEXTURE_HPP
#define LGDSTEXTURE_HPP


#include<glad/glad.h>
#include<GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stb_image.h>

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

#include "Shader.hpp"

#include <glm/gtc/type_ptr.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "../../thirdparty/stb_image.h"

#define STBI_MSC_SECURE_CRT
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../../thirdparty/stb_image_write.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
#include <cstdlib>

class Texture
{
private:
    #if defined(_WIN32) || defined(_WIN64)
		char folderDistinguisher = '\\';
	#else
		char folderDistinguisher = '/'; 
	#endif
public:
    unsigned int ID = 0; 
    
    std::string path = "";

    std::string title = "";

    std::string tmpPath;
    bool isTexture = true; //Otherwise is a folder

    Texture(){}
    
    Texture(unsigned int ID){
        this->ID = ID;
    }

    void load(const char* path){
        Util util;
        this->title = util.getLastWordBySeparatingWithChar(path,folderDistinguisher);
        this->title = util.removeExtension(this->title);
        this->path = path;

        glActiveTexture(GL_TEXTURE0);
        if(ID == 0)
            glGenTextures(1,&ID);
	    glBindTexture(GL_TEXTURE_2D, ID);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);
	    
        stbi_set_flip_vertically_on_load(true);
        int width, height, channels;

        unsigned char* data = stbi_load(path, &width, &height, &channels, 4);

		if (data != NULL)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);

			std::cout << "Loaded " << path << std::endl;
		}
		else
		{
			const char* reason = "[unknown reason]";
			if (stbi_failure_reason())
			{
				reason = stbi_failure_reason();
			}
			std::cout << "Failed to load texture! " << path << " Reason : " << reason<< std::endl;
		}

        stbi_image_free(data);
    }

    //Returns the texture data from the given path
    //Is not related with the class object
    unsigned char* getTextureDataViaPath(const char* aPath,int &aWidth,int &aHeight,int &aChannels,int desiredChannels,bool flip){
        stbi_set_flip_vertically_on_load(flip);
        unsigned char* data = stbi_load(aPath, &aWidth, &aHeight, &aChannels, desiredChannels);

        if(data != NULL){
            std::cout << "Loaded " << aPath << std::endl;
            return data;
        }
        else{
            const char* reason = "[unknown reason]";
			if (stbi_failure_reason())
			{
				reason = stbi_failure_reason();
			}
			std::cout << "Failed to load texture! " << aPath << " Reason : " << reason<< std::endl;
        
            stbi_image_free(data);
            
            //Allocate 4 unsigned char memory blocks
            unsigned char* aData = (unsigned char*) malloc(4 * sizeof(unsigned char));

            return aData;
        }

    }

    void getData(char*& pixels){
	    glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D,ID);
		glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_BYTE, pixels);
    }

    glm::ivec2 getResolution(){
        int w,h;

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D,ID);
        glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &w);
        glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &h);
    
        return glm::ivec2(w,h);
    }
};

#endif