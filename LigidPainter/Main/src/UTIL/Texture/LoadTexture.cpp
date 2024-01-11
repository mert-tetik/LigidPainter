/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

(c) 2024 Mert Tetik. All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------
*/

#include<glad/glad.h>
#include "LigidGL/LigidGL.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <stb_image.h>

#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "stb_image_resize.h"

#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <filesystem>

#include "UTIL/Util.hpp"
#include "GUI/GUI.hpp"



bool Texture::load(const char* path){

    if(!std::filesystem::is_regular_file(path)){
        LGDLOG::start << "ERROR : Loading texture : " << path << " is not a regular file!" << LGDLOG::end;
        return false;
    }

    this->title = UTIL::getLastWordBySeparatingWithChar(path,UTIL::folderDistinguisher());
    this->title = UTIL::removeExtension(this->title);

    glActiveTexture(GL_TEXTURE0);
   
    // If the texture is not generated then generate it
    if(ID == 0)
        glGenTextures(1,&ID);
    
    glBindTexture(GL_TEXTURE_2D, ID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_MIRRORED_REPEAT);
    
    stbi_set_flip_vertically_on_load(true);
    
    int width, height, channels;
    unsigned char* data = stbi_load(path, &width, &height, &channels, 4);
	if (data != NULL)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
		LGDLOG::start<< "Loaded " << path << LGDLOG::end;
	}
	else
	{
		const char* reason = "[unknown reason]";
		if (stbi_failure_reason())
		{
			reason = stbi_failure_reason();
		}
		LGDLOG::start<< "Failed to load texture! " << path << " Reason : " << reason<< LGDLOG::end;
	}
    stbi_image_free(data);

    return true;
}

bool Texture::load(const char* path, glm::ivec2 textureResolution){
    if(!std::filesystem::is_regular_file(path)){
        LGDLOG::start << "ERROR : Loading texture : " << path << " is not a regular file!" << LGDLOG::end;
        return false;
    }
   
    this->title = UTIL::getLastWordBySeparatingWithChar(path,UTIL::folderDistinguisher());
    this->title = UTIL::removeExtension(this->title);

    glActiveTexture(GL_TEXTURE0);
   
    // If the texture is not generated then generate it
    if(ID == 0)
        glGenTextures(1,&ID);
    
    glBindTexture(GL_TEXTURE_2D, ID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_MIRRORED_REPEAT);
    
    stbi_set_flip_vertically_on_load(true);
    
    int width, height, channels;
    unsigned char* data = stbi_load(path, &width, &height, &channels, 4);
	if (data != NULL)
	{
        GLubyte* resizedPixelsX = new GLubyte[textureResolution.x * textureResolution.y * 4];
        stbir_resize_uint8(data, width, height, 0, resizedPixelsX, textureResolution.x, textureResolution.y, 0, 4);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, textureResolution.x, textureResolution.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, resizedPixelsX);
		glGenerateMipmap(GL_TEXTURE_2D);
		
        LGDLOG::start<< "Loaded " << path << LGDLOG::end;

        delete[] resizedPixelsX;
 	}
	else
	{
		const char* reason = "[unknown reason]";
		if (stbi_failure_reason())
		{
			reason = stbi_failure_reason();
		}
		LGDLOG::start<< "Failed to load texture! " << path << " Reason : " << reason<< LGDLOG::end;
	}
    stbi_image_free(data);

    return true;
}