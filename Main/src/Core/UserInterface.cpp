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
#include "RigidPainter.h"
#include "UserInterface.h"
#include "gl.h"
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
void UserInterface::box(float width, float height, float position_x, float position_y,std::string text,glm::vec3 color, float textRatio,bool isTextBox,bool isMaskImageBox) {
	CommonData commonData;
	GlSet glset;

	std::vector<float> buttonCoor{
		// first triangle
		 width + position_x,  height + position_y, 0.9f,1,1,0,0,0,  // top right
		 width + position_x, -height + position_y, 0.9f,1,0,0,0,0,  // bottom right
		-width + position_x,  height + position_y, 0.9f,0,1,0,0,0,  // top left 
		// second triangle
		 width + position_x, -height + position_y, 0.9f,1,0,0,0,0,  // bottom right
		-width + position_x, -height + position_y, 0.9f,0,0,0,0,0,  // bottom left
		-width + position_x,  height + position_y, 0.9f,0,1,0,0,0  // top left
	};
	glset.uniform1i(commonData.program,"isUiTextureUsed",isMaskImageBox);

	glset.uiDataToShaders(glm::vec3(color.x, color.y, color.z));
	glset.drawArrays(buttonCoor, false);
	if (!isTextBox) {
		renderText(commonData.program, text, position_x -textRatio, position_y - 0.01, 0.0004f, glm::vec3(0.5, 0.8f, 0.2f));
	}
	else {
		renderText(commonData.program, text, -width + position_x, position_y - 0.01, 0.0004f, glm::vec3(0.5, 0.8f, 0.2f));
	}
	glset.uniform1i(commonData.program, "isUiTextureUsed", 0);

}
void UserInterface::colorBox(float position_x, float position_y) {
	glm::vec3 color = glm::vec3(1.0f,0.0f,0.0f);
	std::vector<float> boxCoor{
		// first triangle								    //Color - Normal Vectors Will Be Usen For Color Data Of Vertices
		 -0.10f + position_x,  0.20f + position_y, 0.9f,1,1	,1,1,1,  // top right
		 -0.10f + position_x, -0.20f + position_y, 0.9f,1,0	,0,0,0,  // bottom right
		 0.10f + position_x,  0.20f + position_y, 0.9f,0,1	,color.r,color.g+0.22f,color.b + 0.22f,  // top left 
		// second triangle
		 -0.10f + position_x, -0.20f + position_y, 0.9f,1,0	,0,0,0,  // bottom right
		 0.10f + position_x, -0.20f + position_y, 0.9f,0,0	,0,0,0,  // bottom left
		 0.10f + position_x,  0.20f + position_y, 0.9f,0,1	,color.r,color.g + 0.2f,color.b + 0.2f // top left
	};
	GlSet glset;
	CommonData commonData;
	glset.uniform1i(commonData.program,"isColorBox",1);
	glset.drawArrays(boxCoor,false);
	glset.uniform1i(commonData.program, "isColorBox", 0);

}
void UserInterface::colorRect(float position_x, float position_y) {
	std::vector<float> boxCoor{
		//Color - Normal Vectors Will Be Usen For Color Data Of Vertices

		 -0.01f + position_x,  0.04f + position_y + 0.16f, 0.9f ,1,1	,1,0,0,  // top right
		 -0.01f + position_x, -0.04f + position_y + 0.16f, 0.9f ,1,0	,1,0,1,  // bottom right
		  0.01f + position_x,  0.04f + position_y + 0.16f, 0.9f ,0,1	,1,0,0,  // top left 
		 -0.01f + position_x, -0.04f + position_y + 0.16f, 0.9f ,1,0	,1,0,1,  // bottom right
	 	  0.01f + position_x, -0.04f + position_y + 0.16f, 0.9f ,0,0	,1,0,1,  // bottom left
		  0.01f + position_x,  0.04f + position_y + 0.16f, 0.9f ,0,1	,1,0,0,	 // top left

		 -0.01f + position_x,  0.02f + position_y + 0.10f, 0.9f ,1,1	,1,0,1,  // top right
		 -0.01f + position_x, -0.02f + position_y + 0.10f, 0.9f ,1,0	,0,0,1,  // bottom right
		  0.01f + position_x,  0.02f + position_y + 0.10f, 0.9f ,0,1	,1,0,1,  // top left 
		 -0.01f + position_x, -0.02f + position_y + 0.10f, 0.9f ,1,0	,0,0,1,  // bottom right
		  0.01f + position_x, -0.02f + position_y + 0.10f, 0.9f ,0,0	,0,0,1,  // bottom left
		  0.01f + position_x,  0.02f + position_y + 0.10f, 0.9f ,0,1	,1,0,1,	 // top left

		 -0.01f + position_x,  0.02f + position_y + 0.06f, 0.9f ,1,1	,0,0,1,  // top right
		 -0.01f + position_x, -0.02f + position_y + 0.06f, 0.9f ,1,0	,0,1,1,  // bottom right
		  0.01f + position_x,  0.02f + position_y + 0.06f, 0.9f ,0,1	,0,0,1,  // top left 
		 -0.01f + position_x, -0.02f + position_y + 0.06f, 0.9f ,1,0	,0,1,1,  // bottom right
		  0.01f + position_x, -0.02f + position_y + 0.06f, 0.9f ,0,0	,0,1,1,  // bottom left
		  0.01f + position_x,  0.02f + position_y + 0.06f, 0.9f ,0,1	,0,0,1,	 // top left

		 -0.01f + position_x,  0.04f + position_y		 , 0.9f ,1,1	,0,1,1,  // top right
		 -0.01f + position_x, -0.04f + position_y		 , 0.9f ,1,0	,0,1,0,  // bottom right
		  0.01f + position_x,  0.04f + position_y		 , 0.9f ,0,1	,0,1,1,  // top left 
		 -0.01f + position_x, -0.04f + position_y		 , 0.9f ,1,0	,0,1,0,  // bottom right
		  0.01f + position_x, -0.04f + position_y	     , 0.9f ,0,0	,0,1,0,  // bottom left
		  0.01f + position_x,  0.04f + position_y		 , 0.9f ,0,1	,0,1,1,	 // top left

		 -0.01f + position_x,  0.02f + position_y - 0.10f, 0.9f ,1,1	,1,1,0,  // top right
		 -0.01f + position_x, -0.02f + position_y - 0.10f, 0.9f ,1,0	,1,1,0,  // bottom right
		  0.01f + position_x,  0.02f + position_y - 0.10f, 0.9f ,0,1	,1,1,0,  // top left 
		 -0.01f + position_x, -0.02f + position_y - 0.10f, 0.9f ,1,0	,1,1,0,  // bottom right
		  0.01f + position_x, -0.02f + position_y - 0.10f, 0.9f ,0,0	,1,1,0,  // bottom left
		  0.01f + position_x,  0.02f + position_y - 0.10f, 0.9f ,0,1	,1,1,0,	 // top left
												  
		 -0.01f + position_x,  0.02f + position_y - 0.06f, 0.9f ,1,1	,0,1,0,  // top right
		 -0.01f + position_x, -0.02f + position_y - 0.06f, 0.9f ,1,0	,1,1,0,  // bottom right
		  0.01f + position_x,  0.02f + position_y - 0.06f, 0.9f ,0,1	,0,1,0,  // top left 
		 -0.01f + position_x, -0.02f + position_y - 0.06f, 0.9f ,1,0	,1,1,0,  // bottom right
		  0.01f + position_x, -0.02f + position_y - 0.06f, 0.9f ,0,0	,1,1,0,  // bottom left
		  0.01f + position_x,  0.02f + position_y - 0.06f, 0.9f ,0,1	,0,1,0,	 // top left


		 -0.01f + position_x,  0.04f + position_y - 0.16f, 0.9f ,1,1	,1,1,0,  // top right
		 -0.01f + position_x, -0.04f + position_y - 0.16f, 0.9f ,1,0	,1,0,0,  // bottom right
		  0.01f + position_x,  0.04f + position_y - 0.16f, 0.9f ,0,1	,1,1,0,  // top left 
		 -0.01f + position_x, -0.04f + position_y - 0.16f, 0.9f ,1,0	,1,0,0,  // bottom right
		  0.01f + position_x, -0.04f + position_y - 0.16f, 0.9f ,0,0	,1,0,0,  // bottom left
		  0.01f + position_x,  0.04f + position_y - 0.16f, 0.9f ,0,1	,1,1,0,	 // top left
	};
	GlSet glset;
	CommonData commonData;
	glset.uniform1i(commonData.program, "isColorBox", 1);
	glset.drawArrays(boxCoor, false);
	glset.uniform1i(commonData.program, "isColorBox", 0);

}
void UserInterface::rangeBar(float position_x, float position_y,float value) {
	ColorData colorData;
	box(0.01f, 0.02f, position_x+value, 0.0f, "", colorData.rangeBarFront, 0.045f, false, false);
	box(0.11f, 0.02f, position_x, 0.0f, "", colorData.rangeBarBack, 0.045f, false, false);
}
void UserInterface::panelChangeButton(float position_x, float position_y) {
	std::vector<float> buttonCoor{
		// first triangle
		 0.00f + position_x,  0.02f + position_y, 0.0f,0,0,0,0,0,  // top right
		 0.00f + position_x, -0.05f + position_y, 0.0f,0,0,0,0,0,  // bottom right
		-0.025f + position_x,  0.02f + position_y, 0.0f,0,0,0,0,0,  // top left 
		// second triangle						     9
		 0.00f + position_x, -0.05f + position_y, 0.0f,0,0,0,0,0,  // bottom right
		-0.025f + position_x, -0.02f + position_y, 0.0f,0,0,0,0,0,  // bottom left
		-0.025f + position_x,  0.02f + position_y, 0.0f,0,0,0,0,0  // top left
	};
	GlSet glset;
	ColorData colorData;
	glset.uiDataToShaders(colorData.panelHoldColor);
	glset.drawArrays(buttonCoor, false);
}
bool UserInterface::isMouseOnPanelChangeButton(GLFWwindow* window, float position_x, float position_y, int mouseXpos, int mouseYpos) {
	std::vector<float> buttonCoor{
		// first triangle
		 0.00f + position_x,  0.02f + position_y, 0.9f,0,0,0,0,0,  // top right
		 0.00f + position_x, -0.05f + position_y, 0.9f,0,0,0,0,0,  // bottom right
		-0.025f + position_x,  0.02f + position_y, 0.9f,0,0,0,0,0,  // top left 
		// second triangle						     9
		 0.00f + position_x, -0.05f + position_y, 0.9f,0,0,0,0,0,  // bottom right
		-0.025f + position_x, -0.02f + position_y, 0.9f,0,0,0,0,0,  // bottom left
		-0.025f + position_x,  0.02f + position_y, 0.9f,0,0,0,0,0  // top left
	};
	GlSet glset;
	CommonData commonData;
	//glfwGetWindowSize();
	int screenSizeX;
	int screenSizeY;
	glfwGetWindowSize(window, &screenSizeX, &screenSizeY);

	float mouseFX = ((float)mouseXpos / (screenSizeX/2));
	float mouseFY = (((float)mouseYpos / (screenSizeY / 2)) - 1.0f) * -1.0f;

	for (size_t i = 0; i < 2; i++)
	{
		float ax = buttonCoor[0 + (24 * i)];
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
void UserInterface::checkBox(float position_x, float position_y, std::string text, glm::vec3 color,bool mouseHover,bool checked) {
	CommonData commonData;
	ColorData colorData;
	if (!checked) {
		if (!mouseHover)
			box(0.012f, 0.02f, position_x, position_y, "", colorData.checkBoxColor, 0.034f, false, false);
		else
			box(0.012f, 0.02f, position_x, position_y, "", colorData.checkBoxCheckedColor, 0.034f, false, false);
	}
	else {
		box(0.012f, 0.02f, position_x, position_y, "", glm::vec3(1.0f) - colorData.checkBoxColor, 0.034f, false, false);

	}
	renderText(commonData.program, text, position_x+0.04f, position_y - 0.01, 0.0004f, glm::vec3(0.5, 0.8f, 0.2f));
}
bool UserInterface::isMouseOnButton(GLFWwindow*window, float width, float height, float position_x, float position_y,int mouseXpos, int mouseYpos,bool isPanelMoving){ //Return true if mouse hover on the given coordinates
	std::vector<float> buttonCoor{
		// first triangle
		 width + position_x,  height + position_y, 1,1,0,0,0,0,  // top right
		 width + position_x, -height + position_y, 1,0,0,0,0,0,  // bottom right
		-width + position_x,  height + position_y, 0,1,0,0,0,0,  // top left 
		// second triangle
		 width + position_x, -height + position_y, 1,0,0,0,0,0,  // bottom right
		-width + position_x, -height + position_y, 0,0,0,0,0,0,  // bottom left
		-width + position_x,  height + position_y, 0,1,0,0,0,0  // top left
	};
	//glfwGetWindowSize();
	int screenSizeX;
	int screenSizeY;
	glfwGetWindowSize(window,&screenSizeX,&screenSizeY);
	float mouseFX;
	if (!isPanelMoving) {
		mouseFX = ((float)mouseXpos / (screenSizeX / 2)) - 1.0f;
	}
	else {
		mouseFX = ((float)mouseXpos / (screenSizeX / 2));

	}
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
		FT_Set_Pixel_Sizes(face, 0, 50);
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

	glset.uniform1i(program,"isText", 1);
	glset.uniform1i(program, "isTextF", 1);
	glset.uniform1i(program, "text", 2);

	
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
			
			 xpos,     ypos + h, 1.0f  ,0.0f, 0.0f,0,0,0,
			 xpos + w, ypos,     1.0f  ,1.0f, 1.0f,0,0,0,
			 xpos + w, ypos + h, 1.0f  ,1.0f, 0.0f,0,0,0
		};
		glset.bindTexture(ch.TextureID);
		glset.drawArrays(vertices, false);
		x += (ch.Advance >> 6) * scale; 
	}
	glset.uniform1i(program, "isTextF", 0);
	glset.uniform1i(program, "isText", 0);

}
void UserInterface::renderMenubar(GLFWwindow* window) {
	/*ColorData colorD;
	box(0.04f, 0.02f, -0.92f, 0.98f, "Layers", colorD.menuBarColor, 0.034f, false, false);
	box(0.04f, 0.02f, -0.82f, 0.98f, "Nodes", colorD.menuBarColor, 0.034f, false, false);
	box(0.08f, 0.02f, -0.68f, 0.98f, "Texture Paint", colorD.menuBarColor, 0.06f, false, false);
	box(0.08f, 0.02f, -0.50f, 0.98f, "Load Model", colorD.menuBarColor, 0.06f, false, false);
	box(0.04f, 0.02f, -0.36f, 0.98f, "Export", colorD.menuBarColor, 0.034f, false, false);
	box(0.08f, 0.02f, -0.22f, 0.98f, "Load Project", colorD.menuBarColor, 0.06f, false, false);
	box(0.06f, 0.02f, -0.06f, 0.98f, "Settings", colorD.menuBarColor, 0.04f, false, false);

	box(1.0f, 0.08f, 0.0f, 1.03f, "", colorD.menuBarColor, 0.00f, false, false);*/
}