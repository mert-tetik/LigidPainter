#include<iostream>
#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include <string>
#include <fstream>
#include <sstream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include "Fadenode.h"
#include <vector>
#include "stb_image.h"
#include "stb_image_write.h"
#include <ft2build.h>
#include <map>

#include FT_FREETYPE_H 

ColorData colorD;

void UserInterface::panel(float panelLoc, float) {
	GlSet glset;
	std::vector<float> panelCoor{
		panelLoc-1.0f,1.0f,0,0,0,0,0,0,
		panelLoc -1.0f  ,-1.0f,0,0,0,0,0,0,
		1.0f ,1.0f,0,0,0,0,0,0,
		1.0f ,1.0f,0,0,0,0,0,0,
		1.0f ,-1.0f,0,0,0,0,0,0,
		panelLoc -1.0f ,-1.0f,0,0,0,0,0,0,
	};
	std::vector<float> panelHoldCoor{
		panelLoc - 1.0f , 1.0f,0,0,0,0,0,0,
		panelLoc - 1.0f ,-1.0f,0,0,0,0,0,0,
		panelLoc - 1.0f + 0.006f,1.0f,0,0,0,0,0,0,
		panelLoc - 1.0f + 0.006f,1.0f,0,0,0,0,0,0,
		panelLoc - 1.0f + 0.006f,-1.0f,0,0,0,0,0,0,
		panelLoc - 1.0f ,-1.0f,0,0,0,0,0,0
	};

	glset.uiDataToShaders(colorD.panelHoldColor);
	glset.drawArrays(panelHoldCoor, false);
	glset.uiDataToShaders(colorD.panelColor);
	glset.drawArrays(panelCoor, false);
}
void UserInterface::box(float width, float height, float position_x, float position_y,std::string text,glm::vec3 color, float textRatio,bool isTextBox) {
	CommonData commonData;
	GlSet glset;

	/*std::vector<float> buttonCoor{
	   -1.474451f/10 + width + position_x, 0.348604f/10 + position_y  ,0,0,0,0,0,0,
	   -1.379259f/10 + width + position_x, 0.385555f/10 + position_y  ,0,0,0,0,0,0,
	   -1.379259f/10 + width + position_x, 0.253546f/10 + position_y  ,0,0,0,0,0,0,
	   -1.474451f/10 + width + position_x, 0.348604f/10 + position_y  ,0,0,0,0,0,0,
	   -1.537795f/10 + width + position_x, 0.253546f/10 + position_y  ,0,0,0,0,0,0,
	   -1.379259f/10 + width + position_x, 0.253546f/10 + position_y  ,0,0,0,0,0,0,
	   -1.537795f/10 + width + position_x, 0.253546f/10 + position_y  ,0,0,0,0,0,0,
	   -1.379259f/10 + width + position_x, 0.253546f/10 + position_y  ,0,0,0,0,0,0,
	   -1.379259f/10 + width + position_x,-0.253546f/10 + position_y  ,0,0,0,0,0,0,
	   -1.537795f/10 + width + position_x,-0.253546f/10 + position_y  ,0,0,0,0,0,0,
	   -1.537795f/10 + width + position_x, 0.253546f/10 + position_y  ,0,0,0,0,0,0,
	   -1.379259f/10 + width + position_x,-0.253546f/10 + position_y  ,0,0,0,0,0,0,
	   -1.487647f/10 + width + position_x,-0.338047f/10 + position_y  ,0,0,0,0,0,0,
	   -1.537795f/10 + width + position_x,-0.253546f/10 + position_y  ,0,0,0,0,0,0,
	   -1.379259f/10 + width + position_x,-0.253546f/10 + position_y  ,0,0,0,0,0,0,
	   -1.487647f/10 + width + position_x,-0.338047f/10 + position_y  ,0,0,0,0,0,0,
	   -1.379259f/10 + width + position_x,-0.385555f/10 + position_y  ,0,0,0,0,0,0,
	   -1.379259f/10 + width + position_x,-0.253546f/10 + position_y  ,0,0,0,0,0,0,
	   -0.000000f/10         + position_x,-0.385555f/10 + position_y  ,0,0,0,0,0,0,
	   -1.379259f/10 + width + position_x,-0.385555f/10 + position_y  ,0,0,0,0,0,0,
	   -1.379259f/10 + width + position_x,-0.253546f/10 + position_y  ,0,0,0,0,0,0,
	   -0.000000f/10         + position_x,-0.385555f/10 + position_y  ,0,0,0,0,0,0,
	   -0.000000f/10         + position_x,-0.253546f/10 + position_y  ,0,0,0,0,0,0,
	   -1.379259f/10 + width + position_x,-0.253546f/10 + position_y  ,0,0,0,0,0,0,
	   -0.000000f/10         + position_x,-0.253546f/10 + position_y  ,0,0,0,0,0,0,
	   -1.379259f/10 + width + position_x, 0.253546f/10 + position_y  ,0,0,0,0,0,0,
	   -1.379259f/10 + width + position_x,-0.253546f/10 + position_y  ,0,0,0,0,0,0,
		0.000000f/10         + position_x, 0.253546f/10 + position_y  ,0,0,0,0,0,0,
	   -0.000000f/10         + position_x,-0.253546f/10 + position_y  ,0,0,0,0,0,0,
	   -1.379259f/10 + width + position_x, 0.253546f/10 + position_y  ,0,0,0,0,0,0,
	   -1.379259f/10 + width + position_x, 0.385555f/10 + position_y  ,0,0,0,0,0,0,
		0.000000f/10         + position_x, 0.253546f/10 + position_y  ,0,0,0,0,0,0,
       -1.379259f/10 + width + position_x, 0.253546f/10 + position_y  ,0,0,0,0,0,0,
		0.000000f/10         + position_x, 0.385555f/10 + position_y  ,0,0,0,0,0,0,
	   -1.379259f/10 + width + position_x, 0.385555f/10 + position_y  ,0,0,0,0,0,0,
		0.000000f/10         + position_x, 0.253546f/10 + position_y  ,0,0,0,0,0,0,
		0.000000f/10         + position_x, 0.385555f/10 + position_y  ,0,0,0,0,0,0,
		0.000000f/10         + position_x, 0.253546f/10 + position_y  ,0,0,0,0,0,0,
		1.379259f/10 - width + position_x, 0.385555f/10 + position_y  ,0,0,0,0,0,0,
		0.000000f/10         + position_x, 0.253546f/10 + position_y  ,0,0,0,0,0,0,
		1.379259f/10 - width + position_x, 0.385555f/10 + position_y  ,0,0,0,0,0,0,
		1.379259f/10 - width + position_x, 0.253546f/10 + position_y  ,0,0,0,0,0,0,
		0.000000f/10         + position_x, 0.253546f/10 + position_y  ,0,0,0,0,0,0,
	   -0.000000f/10         + position_x,-0.253546f/10 + position_y  ,0,0,0,0,0,0,
		1.379259f/10 - width + position_x, 0.253546f/10 + position_y  ,0,0,0,0,0,0,
	   -0.000000f/10         + position_x,-0.253546f/10 + position_y  ,0,0,0,0,0,0,
		1.379259f/10 - width + position_x, 0.253546f/10 + position_y  ,0,0,0,0,0,0,
		1.379259f/10 - width + position_x,-0.253546f/10 + position_y  ,0,0,0,0,0,0,
	   -0.000000f/10         + position_x,-0.385555f/10 + position_y  ,0,0,0,0,0,0,
       -0.000000f/10         + position_x,-0.253546f/10 + position_y  ,0,0,0,0,0,0,
		1.379259f/10 - width + position_x,-0.253546f/10 + position_y  ,0,0,0,0,0,0,
	   -0.000000f/10         + position_x,-0.385555f/10 + position_y  ,0,0,0,0,0,0,
		1.379259f/10 - width + position_x,-0.385555f/10 + position_y  ,0,0,0,0,0,0,
		1.379259f/10 - width + position_x,-0.253546f/10 + position_y  ,0,0,0,0,0,0,
		1.487647f/10 - width + position_x,-0.338047f/10 + position_y  ,0,0,0,0,0,0,
		1.379259f/10 - width + position_x,-0.385555f/10 + position_y  ,0,0,0,0,0,0,
		1.379259f/10 - width + position_x,-0.253546f/10 + position_y  ,0,0,0,0,0,0,
		1.487647f/10 - width + position_x,-0.338047f/10 + position_y  ,0,0,0,0,0,0,
		1.537795f/10 - width + position_x,-0.253546f/10 + position_y  ,0,0,0,0,0,0,
		1.379259f/10 - width + position_x,-0.253546f/10 + position_y  ,0,0,0,0,0,0,
		1.537795f/10 - width + position_x,-0.253546f/10 + position_y  ,0,0,0,0,0,0,
		1.537795f/10 - width + position_x, 0.253546f/10 + position_y  ,0,0,0,0,0,0,
		1.379259f/10 - width + position_x,-0.253546f/10 + position_y  ,0,0,0,0,0,0,
		1.537795f/10 - width + position_x, 0.253546f/10 + position_y  ,0,0,0,0,0,0,
		1.379259f/10 - width + position_x, 0.253546f/10 + position_y  ,0,0,0,0,0,0,
		1.379259f/10 - width + position_x,-0.253546f/10 + position_y  ,0,0,0,0,0,0,
		1.474451f/10 - width + position_x, 0.348604f/10 + position_y  ,0,0,0,0,0,0,
		1.537795f/10 - width + position_x, 0.253546f/10 + position_y  ,0,0,0,0,0,0,
		1.379259f/10 - width + position_x, 0.253546f/10 + position_y  ,0,0,0,0,0,0,
		1.474451f/10 - width + position_x, 0.348604f/10 + position_y  ,0,0,0,0,0,0,
		1.379259f/10 - width + position_x, 0.385555f/10 + position_y  ,0,0,0,0,0,0,
		1.379259f/10 - width + position_x, 0.253546f/10 + position_y  ,0,0,0,0,0,0
	};*/
	std::vector<float> buttonCoor{
		// first triangle
		 width + position_x,  height + position_y, 0.9f,0,0,0,0,0,  // top right
		 width + position_x, -height + position_y, 0.9f,0,0,0,0,0,  // bottom right
		-width + position_x,  height + position_y, 0.9f,0,0,0,0,0,  // top left 
		// second triangle						     9
		 width + position_x, -height + position_y, 0.9f,0,0,0,0,0,  // bottom right
		-width + position_x, -height + position_y, 0.9f,0,0,0,0,0,  // bottom left
		-width + position_x,  height + position_y, 0.9f,0,0,0,0,0  // top left
	};

	glset.uniform3f(commonData.program, "textBg", color.x, color.y, color.z);//Set text background color to what color it's used in.

	glset.uiDataToShaders(glm::vec3(color.x, color.y, color.z));
	glset.drawArrays(buttonCoor, false);
	if (!isTextBox) {
		renderText(commonData.program, text, position_x -textRatio, position_y - 0.01, 0.0004f, glm::vec3(0.5, 0.8f, 0.2f));
	}
	else {
		renderText(commonData.program, text, -width + position_x, position_y - 0.01, 0.0004f, glm::vec3(0.5, 0.8f, 0.2f));
	}
	
	

}
bool UserInterface::isMouseOnButton(GLFWwindow*window, float width, float height, float position_x, float position_y,int mouseXpos, int mouseYpos){
	std::vector<float> buttonCoor{
		// first triangle
		 width + position_x,  height + position_y, 0,0,0,0,0,0,  // top right
		 width + position_x, -height + position_y, 0,0,0,0,0,0,  // bottom right
		-width + position_x,  height + position_y, 0,0,0,0,0,0,  // top left 
		// second triangle
		 width + position_x, -height + position_y, 0,0,0,0,0,0,  // bottom right
		-width + position_x, -height + position_y, 0,0,0,0,0,0,  // bottom left
		-width + position_x,  height + position_y, 0,0,0,0,0,0  // top left
	};
	//glfwGetWindowSize();
	int screenSizeX;
	int screenSizeY;
	glfwGetWindowSize(window,&screenSizeX,&screenSizeY);

	float mouseFX = ((float)mouseXpos / (screenSizeX/2))-1.0f;
	float mouseFY = (((float)mouseYpos / (screenSizeY / 2))-1.0f)*-1.0f;


	for (size_t i = 0; i < 2; i++)
	{
		float ax = buttonCoor[0 + (24*i)];
		float ay = buttonCoor[1 + (24 * i)];
		float bx = buttonCoor[8 + (24 * i)];
		float by = buttonCoor[9 + (24 * i)];
		float cx = buttonCoor[16 + (24 * i)];
		float cy = buttonCoor[17 + (24 * i)];

		if (cy - ay == 0) {
			cy += 0.0001f;
		}

		float w1 = (ax * (cy - ay) + (mouseFY - ay) * (cx - ax) - mouseFX * (cy - ay)) / ((by - ay) * (cx - ax) - (bx - ax) * (cy - ay));
		float w2 = (mouseFY - ay - w1 * (by - ay)) / (cy - ay);
		if (w1 >= 0 && w2 >= 0 && (w1 + w2) <= 1) {
			return true;
		}
		else if (i == 1) {
			return false;
		}
	}
	
}
void UserInterface::setViewportBgColor() {
	glClearColor(colorD.viewportBackColor.x, colorD.viewportBackColor.y, colorD.viewportBackColor.z, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
}

struct character {
	unsigned int TextureID;
	glm::ivec2   Size;
	glm::ivec2   Bearing;
	unsigned int Advance;
};
std::map<char, character> characters;

void UserInterface::uploadChars() {
	GlSet glset;
	FT_Library ft;
	FT_Init_FreeType(&ft);
	//glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // disable byte-alignment restriction
	FT_Face face;
	if (FT_New_Face(ft, "fonts/arial.ttf", 0, &face)) {
		std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
	}
	else {
		glset.activeTexture(GL_TEXTURE2);
		// set size to load glyphs as
		FT_Set_Pixel_Sizes(face, 0, 50);

		// disable byte-alignment restriction
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
	}
	// destroy FreeType once we're finished
	FT_Done_Face(face);
	FT_Done_FreeType(ft);
}

void UserInterface::renderText(unsigned int program, std::string text, float x, float y, float scale, glm::vec3 color) {
	GlSet glset;
	glset.activeTexture(GL_TEXTURE2);
	//glBindVertexArray(VAO);
	int isTextLoc = glGetUniformLocation(program, "isText");
	glUniform1i(isTextLoc, 1);
	int isTextFLoc = glGetUniformLocation(program, "isTextF");
	glUniform1i(isTextFLoc, 1);
	int CharLoc = glGetUniformLocation(program, "text");
	glUniform1i(CharLoc, 2);
	


	// iterate through all characters
	std::string::const_iterator c;
	for (c = text.begin(); c != text.end(); c++)
	{
		character ch = characters[*c];

		float xpos = x + ch.Bearing.x * scale;
		float ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

		float w = ch.Size.x * scale;
		float h = ch.Size.y * scale;
		std::vector <float> vertices = {
			 xpos,     ypos + h, 1.0f  ,0.0f, 0.0f,0,0,0,
			 xpos,     ypos,     1.0f  ,0.0f, 1.0f,0,0,0,
			 xpos + w, ypos,     1.0f  ,1.0f, 1.0f,0,0,0,
			//					 1
			 xpos,     ypos + h, 1.0f  ,0.0f, 0.0f,0,0,0,
			 xpos + w, ypos,     1.0f  ,1.0f, 1.0f,0,0,0,
			 xpos + w, ypos + h, 1.0f  ,1.0f, 0.0f,0,0,0
		};
		glset.bindTexture(ch.TextureID);
		glset.drawArrays(vertices, false);
		x += (ch.Advance >> 6) * scale; 
	}
	glBindTexture(GL_TEXTURE_2D, 2);
	glUniform1i(isTextLoc, 0);
	glUniform1i(isTextFLoc, 0);
}
void UserInterface::renderMenubar(GLFWwindow* window) {
	ColorData colorD;
	box(0.04f, 0.02f, -0.92f, 0.98f, "Layers", colorD.menuBarColor, 0.034f, false);
	box(0.04f, 0.02f, -0.82f, 0.98f, "Nodes", colorD.menuBarColor, 0.034f, false);
	box(0.08f, 0.02f, -0.68f, 0.98f, "Texture Paint", colorD.menuBarColor, 0.06f, false);
	box(0.08f, 0.02f, -0.50f, 0.98f, "Load Model", colorD.menuBarColor, 0.06f, false);
	box(0.04f, 0.02f, -0.36f, 0.98f, "Export", colorD.menuBarColor, 0.034f, false);
	box(0.08f, 0.02f, -0.22f, 0.98f, "Load Project", colorD.menuBarColor, 0.06f, false);
	box(0.06f, 0.02f, -0.06f, 0.98f, "Settings", colorD.menuBarColor, 0.04f, false);

	box(1.0f, 0.08f, 0.0f, 1.03f, "", colorD.menuBarColor, 0.00f, false);
}