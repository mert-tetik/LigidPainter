#include<iostream>

#include<glad/glad.h>
#include<GLFW/glfw3.h>

#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <filesystem>


#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

#include "Core/Model/model.h"

#include "Core/Render/Render.h"
#include "Core/LigidPainter.h"
#include "Core/UserInterface.h"
#include "Core/Utilities.h"
#include "Core/gl.h"
#include "Core/Texture/Texture.h"

#include "stb_image.h"
#include "stb_image_write.h"

#include "tinyfiledialogs.h"

void Render::renderModifiedBrushCursor(float distanceX,int screenWidth,int screenHeight,double mouseXpos,double mouseYpos,glm::vec3 color,int maxScreenWidth,int maxScreenHeight,Programs programs) {

	float sizeX = distanceX; //Match the size of the window
	float screenGapX = ((float)maxScreenWidth - (float)screenWidth)/(((float)maxScreenWidth)/2.0f)/2.0f; 
	
	GlSet glset;
	glset.uniform1i(programs.uiProgram, "drawBrushIndicator", 1); //TODO : Create shader for brush indicator
	std::vector<float> paintingSquare{
		// first triangle
		( sizeX / maxScreenWidth / 1.0f + (float)mouseXpos / maxScreenWidth / 0.5f - 1.0f)+screenGapX,  sizeX / maxScreenHeight / 1.0f - (float)mouseYpos / maxScreenHeight / 0.5f + 1.0f , 1.0f,1,1,0,0,0,  // top right
		( sizeX / maxScreenWidth / 1.0f + (float)mouseXpos / maxScreenWidth / 0.5f - 1.0f)+screenGapX, -sizeX / maxScreenHeight / 1.0f - (float)mouseYpos / maxScreenHeight / 0.5f + 1.0f , 1.0f,1,0,0,0,0,  // bottom right
		(-sizeX / maxScreenWidth / 1.0f + (float)mouseXpos / maxScreenWidth / 0.5f - 1.0f)+screenGapX,  sizeX / maxScreenHeight / 1.0f - (float)mouseYpos / maxScreenHeight / 0.5f + 1.0f , 1.0f,0,1,0,0,0,  // top left 
		( sizeX / maxScreenWidth / 1.0f + (float)mouseXpos / maxScreenWidth / 0.5f - 1.0f)+screenGapX, -sizeX / maxScreenHeight / 1.0f - (float)mouseYpos / maxScreenHeight / 0.5f + 1.0f , 1.0f,1,0,0,0,0,  // bottom right
		(-sizeX / maxScreenWidth / 1.0f + (float)mouseXpos / maxScreenWidth / 0.5f - 1.0f)+screenGapX, -sizeX / maxScreenHeight / 1.0f - (float)mouseYpos / maxScreenHeight / 0.5f + 1.0f , 1.0f,0,0,0,0,0,  // bottom left
		(-sizeX / maxScreenWidth / 1.0f + (float)mouseXpos / maxScreenWidth / 0.5f - 1.0f)+screenGapX,  sizeX / maxScreenHeight / 1.0f - (float)mouseYpos / maxScreenHeight / 0.5f + 1.0f , 1.0f,0,1,0,0,0  // top left
	};
	glset.uiDataToShaders(color/glm::vec3(255.0f));
	glset.uniform1f(programs.uiProgram, "uiOpacity", 0.2f);
	glset.drawArrays(paintingSquare, false);
	glset.uniform1f(programs.uiProgram, "uiOpacity", 0.5f);

	glset.uniform1i(programs.uiProgram, "drawBrushIndicator", 0);
}