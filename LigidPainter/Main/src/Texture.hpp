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
    /* data */
public:
    unsigned int ID = 0; 
    
    int width;
    int height;
    int channels;
    
    std::string path;

    Texture(){}

    void load(const char* path){
        this->path = path;

        glActiveTexture(GL_TEXTURE0);
        if(ID == 0)
            glGenTextures(1,&ID);
	    glBindTexture(GL_TEXTURE_2D, ID);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	    
        stbi_set_flip_vertically_on_load(true);

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
};

#endif