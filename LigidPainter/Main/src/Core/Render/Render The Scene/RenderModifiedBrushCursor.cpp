#include<iostream>

#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

#include "Core/Render/Render.h"
#include "Core/LigidPainter.h"
#include "Core/Gl.h"

void Render::renderModifiedBrushCursor(float distanceX,int screenWidth,int screenHeight,double mouseXpos,double mouseYpos,glm::vec3 color,int maxScreenWidth,int maxScreenHeight,Programs programs) {

	float sizeX = distanceX; //Match the size of the window
	float screenGapX = ((float)maxScreenWidth - (float)screenWidth)/(((float)maxScreenWidth)/2.0f)/2.0f; 
	
	GlSet glset;
	glset.uniform1i(programs.uiProgram, "drawBrushIndicator", 1);
	std::vector<float> paintingSquare{
		// first triangle
		( sizeX / maxScreenWidth / 1.0f + (float)mouseXpos / maxScreenWidth / 0.5f - 1.0f)+screenGapX,  sizeX / maxScreenHeight / 1.0f - (float)mouseYpos / maxScreenHeight / 0.5f + 1.0f , 0.0f,1,1,0,0,0,  // top right
		( sizeX / maxScreenWidth / 1.0f + (float)mouseXpos / maxScreenWidth / 0.5f - 1.0f)+screenGapX, -sizeX / maxScreenHeight / 1.0f - (float)mouseYpos / maxScreenHeight / 0.5f + 1.0f , 0.0f,1,0,0,0,0,  // bottom right
		(-sizeX / maxScreenWidth / 1.0f + (float)mouseXpos / maxScreenWidth / 0.5f - 1.0f)+screenGapX,  sizeX / maxScreenHeight / 1.0f - (float)mouseYpos / maxScreenHeight / 0.5f + 1.0f , 0.0f,0,1,0,0,0,  // top left 
		( sizeX / maxScreenWidth / 1.0f + (float)mouseXpos / maxScreenWidth / 0.5f - 1.0f)+screenGapX, -sizeX / maxScreenHeight / 1.0f - (float)mouseYpos / maxScreenHeight / 0.5f + 1.0f , 0.0f,1,0,0,0,0,  // bottom right
		(-sizeX / maxScreenWidth / 1.0f + (float)mouseXpos / maxScreenWidth / 0.5f - 1.0f)+screenGapX, -sizeX / maxScreenHeight / 1.0f - (float)mouseYpos / maxScreenHeight / 0.5f + 1.0f , 0.0f,0,0,0,0,0,  // bottom left
		(-sizeX / maxScreenWidth / 1.0f + (float)mouseXpos / maxScreenWidth / 0.5f - 1.0f)+screenGapX,  sizeX / maxScreenHeight / 1.0f - (float)mouseYpos / maxScreenHeight / 0.5f + 1.0f , 0.0f,0,1,0,0,0  // top left
	};

	glm::vec4 result = glm::vec4(color / glm::vec3(255.0f),0.2f);
	glset.uniform4fv(programs.uiProgram, "uiColor", result);
	glset.uniform1i(programs.uiProgram, "currentTexture", 0);

	glset.drawArrays(paintingSquare, false);
	

	glset.uniform1i(programs.uiProgram, "drawBrushIndicator", 0);
}