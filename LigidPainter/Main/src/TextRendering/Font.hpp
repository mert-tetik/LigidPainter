#ifndef LGDFONT_HPP
#define LGDFONT_HPP

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

#include <glm/gtc/type_ptr.hpp>

#include "freetype/freetype.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
#include <cstdlib>

struct character { //A single char
	unsigned int TextureID;
	glm::ivec2   Size;
	glm::ivec2   Bearing;
	unsigned int Advance;
};

class Font
{
private:
    /* data */
public:
	//Font class contains the characters of a font and the font file's name

    std::string name = "";
    std::string path = "";
	std::map<char, character> characters;

    Font(/* args */){}

    void loadFont(const char* path){
        
        //Refresh the characters
        characters.clear();
        
        this->path = path;

		FT_Library ft;
		FT_Init_FreeType(&ft);
		//glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // disable byte-alignment restriction
		FT_Face face;

		if (FT_New_Face(ft, path, 0, &face)) {
			std::cout << "ERROR::FREETYPE: Failed to load font " << path << std::endl;
		}
		else {
			glActiveTexture(GL_TEXTURE0);
			FT_Set_Pixel_Sizes(face, 0, 60); 
			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	
			// load first 128 characters of ASCII set
			for (unsigned char c = 0; c < 128; c++)
			{
				// Load character glyph 
				if (FT_Load_Char(face, c, FT_LOAD_RENDER))
				{
					std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
					continue;
				}
				
				// generate texture
				unsigned int texture;
				glGenTextures(1, &texture);
				glBindTexture(GL_TEXTURE_2D, texture);
				glTexImage2D(
					GL_TEXTURE_2D,
					0,
					GL_RED,
					face->glyph->bitmap.width,
					face->glyph->bitmap.rows,
					0,
					GL_RED,
					GL_UNSIGNED_BYTE,
					face->glyph->bitmap.buffer
				);
	
				// set texture options
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
				// now store character for later use
				character Character = {
					texture,
					glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
					glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
					static_cast<unsigned int>(face->glyph->advance.x)
				};
				characters.insert(std::pair<char, character>(c, Character));
			}
			glBindTexture(GL_TEXTURE_2D, 0);

			std::cout << "Loaded " << path << std::endl; 
			
			#if defined(_WIN32) || defined(_WIN64)
    			char folderDistinguisher = '\\';
			#else
				char folderDistinguisher = '/'; 
			#endif

            //Get the name of the font file
            Util util;
			name = util.getLastWordBySeparatingWithChar(path,folderDistinguisher);
		}

		// destroy FreeType once we're finished
		FT_Done_Face(face);
		FT_Done_FreeType(ft);
    }
};

#endif