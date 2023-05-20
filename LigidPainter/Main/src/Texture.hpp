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

#include "Mesh.hpp"
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
    unsigned int ID;
    
    int width;
    int height;
    int channels;
    
    std::string path;

    Texture();

    void load(const char* path){
        this->path = path;

        glActiveTexture(GL_TEXTURE0);
	    glBindTexture(GL_TEXTURE_CUBE_MAP, ID);

        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	    
        stbi_set_flip_vertically_on_load(true);

        unsigned char* data = stbi_load(path, &width, &height, &channels, 4);

		if (data != NULL)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, channels, GL_UNSIGNED_BYTE, data);
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
    }

    //Returns the texture data from the given path
    //Is not related with the class object
    unsigned char* getTextureDataViaPath(const char* path,int aWidth,int aHeight,int aChannels){
        stbi_set_flip_vertically_on_load(true);
        return stbi_load(path, &aWidth, &aHeight, &aChannels, 4);
    }
};

#endif