#include<iostream>
#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include<windows.h>
#include <string>
#include <fstream>
#include <sstream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include "MSHPApp.h"
#include <vector>
#include "stb_image.h"
#include "stb_image_write.h"

colorData colorD;
glSet glset;

void MSHP_userInterface::panel(float panelLoc) {
	commonData cmnd;
	std::vector<float> panelCoor{
		panelLoc-1.0f,1.0f,0,0,0,0,0,0,
		panelLoc -1.0f,-1.0f,0,0,0,0,0,0,
		1.0f,1.0f,0,0,0,0,0,0,
		1.0f,1.0f,0,0,0,0,0,0,
		1.0f,-1.0f,0,0,0,0,0,0,
		panelLoc -1.0f,-1.0f,0,0,0,0,0,0
	};

	int isAxisPointerLoc = glGetUniformLocation(cmnd.program ,"isAxisPointer");
	glUniform1i(isAxisPointerLoc,0);
	int isTwoDimensionalLoc = glGetUniformLocation(cmnd.program,"isTwoDimensional");
	glUniform1i(isTwoDimensionalLoc,1);
	int is2DLoc = glGetUniformLocation(cmnd.program, "is2D");
	glUniform1i(is2DLoc, 1);
	int uiColorLoc = glGetUniformLocation(cmnd.program, "uiColor");
	glUniform3f(uiColorLoc, colorD.panelColor.x, colorD.panelColor.y, colorD.panelColor.z);

	glset.drawArrays(panelCoor, false);

	is2DLoc = glGetUniformLocation(cmnd.program, "is2D");
	glUniform1i(is2DLoc, 0);
	isTwoDimensionalLoc = glGetUniformLocation(cmnd.program,"isTwoDimensional");
	glUniform1i(isTwoDimensionalLoc,0);
}
void MSHP_userInterface::button(float width ,float position_x, float position_y){
	commonData cmnd;
	std::vector<float> buttonCoor{
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
	};
	
	glset.drawArrays(buttonCoor,false);
}
int* MSHP_userInterface::buttonGetScreenCoor(float width, float position_x, float position_y){
	int squareScreenProjection[] = {
		1105 - width * 960 + position_x * 960, 500 - position_y*540,//Top Right
		1105 - width * 960 + position_x * 960, 540 - position_y*540,//Bottom Right
		 810 + width * 960 + position_x * 960, 500 - position_y*540,//Top Left
		 810 + width * 960 + position_x * 960, 540 - position_y*540 //Bottom Right
	};
	return squareScreenProjection;
}
void MSHP_userInterface::setViewportBgColor() {
	glClearColor(colorD.viewportBackColor.x, colorD.viewportBackColor.y, colorD.viewportBackColor.z, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
}