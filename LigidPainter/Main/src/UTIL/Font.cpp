/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

(c) 2024 Mert Tetik. All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------

* 											BYTE-ALIGNMENT RESTRICTION

Byte-alignment restriction, also known as pixel alignment or row alignment, is a concept in computer graphics and image 
processing that specifies how pixel data should be organized in memory. It refers to the requirement that each row of 
pixel data in an image must start on a memory address that is a multiple of a certain value.

In OpenGL, the glPixelStorei function with the parameter GL_UNPACK_ALIGNMENT is used to control the byte-alignment 
restriction for unpacking pixel data. By default, the value is set to 4, meaning that each row of pixel data 
should start at a memory address that is a multiple of 4 bytes.

When the byte-alignment restriction is enabled, OpenGL expects pixel data to be packed in memory with each row aligned 
to the specified value. If the alignment requirement is not met, it can result in undefined behavior and potential 
performance penalties.

However, in some cases, such as when using pixel data that doesn't conform to the default alignment or when performing 
certain optimizations, you may want to disable the byte-alignment restriction. By setting the GL_UNPACK_ALIGNMENT 
parameter to 1, you inform OpenGL that the pixel data is not subject to any alignment requirements. 
This allows you to provide pixel data that is packed in memory without any padding or alignment between rows.

It's important to note that when you disable the byte-alignment restriction, you need to ensure that the pixel 
data you provide is correctly aligned and packed according to the requirements of the specific image format 
or texture format you are working with.


*/

#include <glad/glad.h>
#include "LigidGL/LigidGL.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <iostream>
#include <vector>
#include <map>

#include "UTIL/Util.hpp"
#include "GUI/GUI.hpp"

#include "freetype/freetype.h"

Font::Font(/* args */){}

void Font::loadFont(const char* path){
	
	//Refresh the characters before loading
	characters.clear();
	
	// Init the freeType
	FT_Library ft;
	FT_Init_FreeType(&ft);

	///Create a new FreeType face to load the font
	FT_Face face;
	
	///Load the font to the FreeType face
	if (FT_New_Face(ft, path, 0, &face)) {
		//If couldn't load
		LGDLOG::start<< "ERROR::FREETYPE: Failed to load font " << path << LGDLOG::end;
	}
	else {
		//FreeType indicate the resolution of a single char texture
		FT_Set_Pixel_Sizes(face, 0, 48); 
		
		//load first 128 characters of ASCII set
		for (unsigned char c = 0; c < 128; c++)
		{
			//Load character glyph 
			if (FT_Load_Char(face, c, FT_LOAD_RENDER))
			{
				LGDLOG::start<< "ERROR::FREETYTPE: Failed to load Glyph" << LGDLOG::end;
				continue;
			}
			
			Texture texture = Texture(nullptr, face->glyph->bitmap.buffer, true, face->glyph->bitmap.width, face->glyph->bitmap.rows, GL_LINEAR, GL_RED, GL_R8, GL_MIRRORED_REPEAT, GL_TEXTURE_2D);

			//Create character stucture using font char data received
			character Character = {
				texture.ID,
				glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
				glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
				static_cast<unsigned int>(face->glyph->advance.x)
			};
			
			// Now store character for later use
			characters.insert(std::pair<char, character>(c, Character));
		}

		LGDLOG::start<< "Loaded " << path << LGDLOG::end; 
		
		//Get the name of the font file
		name = UTIL::getLastWordBySeparatingWithChar(path,UTIL::folderDistinguisher());
	}

	//Destroy FreeType once we're finished
	FT_Done_Face(face);
	FT_Done_FreeType(ft);
}