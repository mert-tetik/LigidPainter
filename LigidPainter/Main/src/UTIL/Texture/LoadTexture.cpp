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
#include "UTIL/Texture/Texture.hpp"
#include "GUI/GUI.hpp"

bool Texture::load(const char* path){

    if(!std::filesystem::is_regular_file(path)){
        LGDLOG::start << "ERROR : Loading texture : " << path << " is not a regular file!" << LGDLOG::end;
        return false;
    }

    this->title = UTIL::getLastWordBySeparatingWithChar(path,UTIL::folderDistinguisher());
    this->title = UTIL::removeExtension(this->title);

    int width, height, channels;
    unsigned char* data = stbi_load(path, &width, &height, &channels, 4);
	if (data != NULL)
	{
        if(!this->ID){
            *this = Texture(data, width, height);
        }
        else{
            this->update(data, width, height);
        }

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

    if(ShaderSystem::textureRenderingShader().ID){
        this->flipTexture(false, true);
    }

    return true;
}