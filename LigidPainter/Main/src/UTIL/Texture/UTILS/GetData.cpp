/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

(c) 2024 Mert Tetik. All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------
*/

#include <glad/glad.h>
#include "LigidGL/LigidGL.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <stb_image.h>

#include "GUI/GUI.hpp"

#include "UTIL/Texture/Texture.hpp"
#include "UTIL/GL/GL.hpp"

static void flipImageVertically(unsigned char* pixels, int width, int height, int channels) {
    int rowSize = width * channels; // Number of bytes per row
    unsigned char* tempRow = new unsigned char[rowSize]; // Temporary storage for swapping

    for (int i = 0; i < height / 2; ++i) {
        unsigned char* rowTop = pixels + i * rowSize; // Pointer to the top row
        unsigned char* rowBottom = pixels + (height - 1 - i) * rowSize; // Pointer to the bottom row

        // Swap the top row with the bottom row
        std::copy(rowTop, rowTop + rowSize, tempRow);
        std::copy(rowBottom, rowBottom + rowSize, rowTop);
        std::copy(tempRow, tempRow + rowSize, rowBottom);
    }

    delete[] tempRow; // Free temporary storage
}

unsigned char* Texture::getTextureDataViaPath(const char* aPath, int &aWidth, int &aHeight, int &aChannels, int desiredChannels, bool flip){
    unsigned char* data = stbi_load(aPath, &aWidth, &aHeight, &aChannels, desiredChannels);
    
    if(data != NULL){
        LGDLOG::start<< "Loaded " << aPath << LGDLOG::end;
        
        if(flip)
            flipImageVertically(data, aWidth, aHeight, aChannels);
        
        return data;
    }
    
    else{
        const char* reason = "[unknown reason]";
		if (stbi_failure_reason())
		{
			reason = stbi_failure_reason();
		}
		LGDLOG::start<< "Failed to load texture! " << aPath << " Reason : " << reason<< LGDLOG::end;
    
        stbi_image_free(data);
        
        //Allocate 4 unsigned char memory blocks
        unsigned char* aData = (unsigned char*) malloc(4 * sizeof(unsigned char));
        return aData;
    }

}

void Texture::getData(char* pixels){
    glActiveTexture(GL_TEXTURE0);
    int txtr_slot = GL::bindTexture_2D(this->ID, "Texture::getData");
	glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_BYTE, pixels);
    
    GL::releaseTextureFromSlot(txtr_slot, "Texture::getData");
}

void Texture::getData(unsigned char* pixels){
    glActiveTexture(GL_TEXTURE0);
    int txtr_slot = GL::bindTexture_2D(this->ID, "Texture::getData");
	glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
    
    GL::releaseTextureFromSlot(txtr_slot, "Texture::getData");
}

glm::ivec2 Texture::getResolution(){

    if(!this->ID || glIsTexture(this->ID) == GL_FALSE){
        return glm::ivec2(1024);
    }
    
    while (glGetError() != GL_NO_ERROR){}
    
    int w = 0, h = 0;

    GLint activeTextureUnit;
    glGetIntegerv(GL_ACTIVE_TEXTURE, &activeTextureUnit);

    // Check if the texture is bound
    GLint currentTexture;
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &currentTexture);
    
    int txtr_slot = -1; 
    
    if (currentTexture != ID) {
        // Bind the texture if it's not already bound
        txtr_slot = GL::bindTexture_2D(this->ID, "Texture::getResolution dest");
    }

    // Retrieve the texture width
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &w);

    // Check for OpenGL errors after glGetTexLevelParameteriv
    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        LGDLOG::start << "ERROR : Can't get the resolution of the texture : OpenGL Error " << error << LGDLOG::end;
        return glm::ivec2(1024);
        // Handle the error (print a message, log, etc.)
        // Optionally, you might want to return an "invalid" value here
    }

    // Retrieve the texture height
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &h);

    // Check for OpenGL errors after the second glGetTexLevelParameteriv
    error = glGetError();
    if (error != GL_NO_ERROR) {
        LGDLOG::start << "ERROR : Can't get the resolution of the texture : OpenGL Error " << error << LGDLOG::end;
        return glm::ivec2(1024);
        // Handle the error (print a message, log, etc.)
        // Optionally, you might want to return an "invalid" value here
    }

    // Restore the original texture binding (if it was changed)
    if (currentTexture != ID && txtr_slot != -1){
        GL::releaseTextureFromSlot(txtr_slot, "Texture::getResolution already bound");
    }

    return glm::ivec2(w, h);
}