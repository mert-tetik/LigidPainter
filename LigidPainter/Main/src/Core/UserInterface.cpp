#include<iostream>
#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include <string>
#include <cstring>
#include <fstream>
#include <sstream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include "LigidPainter.h"
#include "UserInterface.h"
#include "gl.h"
#include "Texture.h"
#include "Render.h"
#include <vector>
#include "stb_image.h"
#include "stb_image_write.h"
#include <ft2build.h>
#include <map>

#include FT_FREETYPE_H 

Programs uiPrograms;

ColorData colorD;

int uiMaxScreenWidth;
int uiMaxScreenHeight;

//--------------------INSTRUCTIONS--------------------\\

/*
	How UI works

	Visualization
		userinterface.cpp - provides neccessary informations
		gl.cpp - Rendering to the screen.
		callback.cpp - Check if button hover using isMouseOnButton function in userinterface.cpp. Return boolean values to the application.cpp for each button.
		UiActions.cpp - Check if button pressed
		application.cpp - Adjust values.

*/

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
	std::vector<float> panelHoldCoor{ //Holding line standing left side of the panel
		panelLoc - 1.0f , 1.0f,0,0,0,0,0,0,
		panelLoc - 1.0f ,-1.0f,0,0,0,0,0,0,
		panelLoc - 1.0f + 0.006f,1.0f,0,0,0,0,0,0,
		panelLoc - 1.0f + 0.006f,1.0f,0,0,0,0,0,0,
		panelLoc - 1.0f + 0.006f,-1.0f,0,0,0,0,0,0,
		panelLoc - 1.0f ,-1.0f,0,0,0,0,0,0
	};
	glm::vec3 blankVal = glm::vec3(0);
	glset.uiDataToShaders(colorD.panelHoldColor);
	glset.uniform3fv(uiPrograms.program, "uiTransitionColor", blankVal);
	glset.uniform1f(uiPrograms.program, "uiTransitionMixVal", 0);
	glset.drawArrays(panelHoldCoor, false);
	glset.uiDataToShaders(colorD.panelColor);
	glset.drawArrays(panelCoor, false);
}

void UserInterface::textureDemonstrator(float width,float height, float position_x,float position_y,float z){ 
	GlSet glset;
	std::vector<float> buttonCoorSq{
		// first triangle
		 width + position_x,  position_y, z,1,1,0,0,0,  // top right
		 width + position_x,  -height +position_y, z,1,0,0,0,0,  // bottom right
		 position_x,  position_y, z,0,1,0,0,0,  // top left 
		// second triangle						   
		 width + position_x,   -height +position_y, z,1,0,0,0,0,  // bottom right
		 position_x, -height +position_y, z,0,0,0,0,0,  // bottom left
		 position_x,  position_y, z,0,1,0,0,0  // top left
	};
	box(0.005f,0.035f,position_x+0.005f,position_y-0.01f,"",glm::vec3(0),0,0,0,1,10,glm::vec3(0),0);
	glset.uniform1i(3, "is2D", 0);
	glset.uniform1i(3, "isRenderTextureMode", 1);
	glset.drawArrays(buttonCoorSq,false);
	glset.uniform1i(3, "isRenderTextureMode", 0);
	glset.uniform1i(3, "is2D", 1);
}
void UserInterface::numericModifier(float position_x,float position_y,unsigned int leftArrow,unsigned int rightArrow,float z,int value,float mixValP,float mixValN){
	//box(0.005f,0.035f,position_x+0.005f,position_y-0.01f,"",glm::vec3(0),0,0,0,1,10,glm::vec3(0),0);

	iconBox(0.02f,0.04f,position_x - 0.05f,position_y,z,leftArrow,mixValN);
	renderText(3,std::to_string(value),position_x-0.01f,position_y-0.01f,0.00022f);
	iconBox(0.02f,0.04f,position_x+ 0.05f,position_y,z,rightArrow,mixValP);
}
void UserInterface::iconBox(float width, float height, float position_x, float position_y, float z,	unsigned int icon,float mixVal){
	std::vector<float> buttonCoorSq{
		// first triangle
		 width + position_x,  height + position_y, z,1,1,0,0,0,  // top right
		 width + position_x, -height + position_y, z,1,0,0,0,0,  // bottom right
		-width + position_x,  height + position_y, z,0,1,0,0,0,  // top left 
		// second triangle						   
		 width + position_x, -height + position_y, z,1,0,0,0,0,  // bottom right
		-width + position_x, -height + position_y, z,0,0,0,0,0,  // bottom left
		-width + position_x,  height + position_y, z,0,1,0,0,0  // top left
	};
	GlSet glset;
	ColorData clrData;

	glUseProgram(uiPrograms.iconsProgram);
	glset.uniform3fv(12,"iconColor",clrData.iconColor);
	glset.uniform3fv(12,"iconColorHover",clrData.iconColorHover);
	glset.uniform1f(12,"iconMixVal",mixVal);
	glset.activeTexture(GL_TEXTURE6);
	glset.bindTexture(icon);
	glset.drawArrays(buttonCoorSq,false);
	glUseProgram(uiPrograms.program);
}

void UserInterface::box(float width, float height, float position_x, float position_y,std::string text,glm::vec3 color, float textRatio,bool isTextBox,bool isMaskImageBox,float z,float buttonCurveReduce, glm::vec3 colorTransitionColor, float mixVal) {
	
	//buttonCurveReduce = 10 | normal  
	//buttonCurveReduce = >10 | smaller
	//buttonCurveReduce = <10 | bigger
	//buttonCurveReduce = 10000 to get a square

	//parameter z = position_z
	GlSet glset;

	//std::vector<float> buttonCoorSq{
	//	// first triangle
	//	 width + position_x,  height + position_y, z,1,1,0,0,0,  // top right
	//	 width + position_x, -height + position_y, z,1,0,0,0,0,  // bottom right
	//	-width + position_x,  height + position_y, z,0,1,0,0,0,  // top left 
	//	// second triangle						   
	//	 width + position_x, -height + position_y, z,1,0,0,0,0,  // bottom right
	//	-width + position_x, -height + position_y, z,0,0,0,0,0,  // bottom left
	//	-width + position_x,  height + position_y, z,0,1,0,0,0  // top left
	//};

	std::vector<float> buttonCoor{
		  position_x+ width + 0.08651f/ buttonCurveReduce,position_y + height / 1.29802f, z, 1, 0.874506, 0, 0, 1, //R / 2
		  position_x+ -width, position_y + height, z, 0, 1, 0, 0, 1,//L / 1
		  position_x+ -width - 0.08651f / buttonCurveReduce, position_y + height / 1.29802f, z, 0, 0.874506, 0, 0, 1,//L / 2
		  position_x+ width + 0.15074f / buttonCurveReduce,position_y + -height / 3.88978f, z, 1, 0.375, 0, 0, 1, //R / 6
		  position_x+ -width - 0.15800f / buttonCurveReduce,position_y + 0, z, 0, 0.5, 0, 0, 1,//L / M
		  position_x+ -width - 0.15074f / buttonCurveReduce,position_y + -height / 3.88978f, z, 0, 0.375, 0, 0, 1,//L / 6
		  position_x+ width + 0.15074f / buttonCurveReduce,position_y + height / 3.88978f, z, 0.99999, 0.624971, 0, 0, 1, //R / 4
		  position_x+ -width - 0.13064f / buttonCurveReduce, position_y + height / 2.00794f, z, 0, 0.749012, 0, 0, 1,//L / 3
		  position_x+ -width - 0.15074f / buttonCurveReduce,position_y + height / 3.88978f, z, 1e-05, 0.624971, 0, 0, 1,//L / 4
		  position_x+ width + 0.08651f / buttonCurveReduce, position_y + -height / 1.29802f, z, 1, 0.125, 0, 0, 1,//R / 8
		  position_x+ -width - 0.13064f / buttonCurveReduce, position_y + -height / 2.00794f, z, 0, 0.25, 0, 0, 1,//L / 7
		  position_x+ -width - 0.08651f / buttonCurveReduce, position_y + -height / 1.29802f, z, 0, 0.125, 0, 0, 1,//L / 8
		  position_x+ width,  position_y -height, z, 1, 0, 0, 0, 1,//R / 9
		  position_x+ -width - 0.08651f / buttonCurveReduce,position_y + -height / 1.29802f, z, 0, 0.125, 0, 0, 1,//L / 8
		  position_x+ -width, position_y -height, z, 0, 0, 0, 0, 1,//L / 9
		  position_x+ width + 0.13064f / buttonCurveReduce, position_y + -height / 2.00794f, z, 1, 0.25, 0, 0, 1,//R / 7
		  position_x+ -width - 0.15074f / buttonCurveReduce,position_y + -height / 3.88978f, z, 0, 0.375, 0, 0, 1,//L / 6
		  position_x+ -width - 0.13064f / buttonCurveReduce, position_y + -height / 2.00794f, z, 0, 0.25, 0, 0, 1,//L / 7
		  position_x+ width + 0.15800f / buttonCurveReduce,position_y + -0, z, 1, 0.5, 0, 0, 1,//R / M
		  position_x+ -width - 0.15074f / buttonCurveReduce,position_y + height / 3.88978f, z, 1e-05, 0.624971, 0, 0, 1,//L / 4
		  position_x+ -width - 0.15800f / buttonCurveReduce,position_y + 0, z, 0, 0.5, 0, 0, 1,//L / M
		  position_x+ width + 0.13064f / buttonCurveReduce, position_y + height / 2.00794f, z, 1, 0.749012, 0, 0, 1,//R / 3
		  position_x+ -width - 0.08651f / buttonCurveReduce,position_y + height / 1.29802f, z, 0, 0.874506, 0, 0, 1,//L / 2
		  position_x+ -width - 0.13064f / buttonCurveReduce, position_y + height / 2.00794f, z, 0, 0.749012, 0, 0, 1,//L / 3
		  position_x+ width + 0.08651f / buttonCurveReduce,position_y + height / 1.29802f, z, 1, 0.874506, 0, 0, 1,//R / 2
		  position_x+ width, position_y + height, z, 1, 1, 0, 0, 1,//R / 1
		  position_x+ -width, position_y + height,z, 0, 1, 0, 0, 1,//L / 1
		  position_x+ width + 0.15074f / buttonCurveReduce, position_y + -height / 3.88978f, z, 1, 0.375, 0, 0, 1,//R / 6
		  position_x+ width + 0.15800f / buttonCurveReduce,position_y + -0, z, 1, 0.5, 0, 0, 1,//R / M
		  position_x+ -width - 0.15800f / buttonCurveReduce,position_y + 0, z, 0, 0.5, 0, 0, 1,//L / M
		  position_x+ width + 0.15074f / buttonCurveReduce, position_y + height / 3.88978f, z, 0.99999, 0.624971, 0, 0, 1,//R / 4
		  position_x+ width + 0.13064f / buttonCurveReduce, position_y + height / 2.00794f, z, 1, 0.749012, 0, 0, 1,//R / 3
		  position_x+ -width - 0.13064f / buttonCurveReduce, position_y + height / 2.00794f, z, 0, 0.749012, 0, 0, 1,//L / 3
		  position_x+ width + 0.08651f / buttonCurveReduce, position_y + -height / 1.29802f, z, 1, 0.125, 0, 0, 1,//R / 8
		  position_x+ width + 0.13064f / buttonCurveReduce, position_y + -height / 2.00794f, z, 1, 0.25, 0, 0, 1,//R / 7
		  position_x+ -width - 0.13064f / buttonCurveReduce, position_y + -height / 2.00794f, z, 0, 0.25, 0, 0, 1,//L / 7
		  position_x+ width, position_y -height,z, z, 0, 0, 0, 1,//R / 9
		  position_x+ width + 0.08651f / buttonCurveReduce, position_y + -height / 1.29802f, z, 1, 0.125, 0, 0, 1,//R / 8
		  position_x+ -width - 0.08651f / buttonCurveReduce,position_y + -height / 1.29802f, z, 0, 0.125, 0, 0, 1,//L / 8
		  position_x+ width + 0.13064f / buttonCurveReduce, position_y + -height / 2.00794f, z, 1, 0.25, 0, 0, 1,//R / 7
		  position_x+ width + 0.15074f / buttonCurveReduce,position_y + -height / 3.88978f, z, 1, 0.375, 0, 0, 1,//R / 6
		  position_x+ -width - 0.15074f / buttonCurveReduce, position_y + -height / 3.88978f, z, 0, 0.375, 0, 0, 1,//L / 6
		  position_x+ width + 0.15800f / buttonCurveReduce, position_y + -0, z, 1, 0.5, 0, 0, 1,//R / M
		  position_x+ width + 0.15074f / buttonCurveReduce,position_y + height / 3.88978f, z, 0.99999, 0.624971, 0, 0, 1,//R / 4
		  position_x+ -width - 0.15074f / buttonCurveReduce, position_y + height / 3.88978f, z, 1e-05, 0.624971, 0, 0, 1,//L / 4
		  position_x+ width + 0.13064f / buttonCurveReduce, position_y + height / 2.00794f, z, 1, 0.749012, 0, 0, 1,//R / 3
		  position_x+ width + 0.08651f / buttonCurveReduce, position_y + height / 1.29802f, z, 1, 0.874506, 0, 0, 1,//R / 2
		  position_x + -width - 0.08651f / buttonCurveReduce,position_y + height / 1.29802f, z, 0, 0.874506, 0, 0, 1,//L / 2
	};

	glset.uniform1i(uiPrograms.program,"isUiTextureUsed",isMaskImageBox);

	glset.uniform3fv(uiPrograms.program,"uiColor",color);
	glset.uniform3fv(uiPrograms.program, "uiTransitionColor", colorTransitionColor);
	glset.uniform1f(uiPrograms.program, "uiTransitionMixVal", mixVal);

	//glset.uiDataToShaders(glm::vec3(color.x, color.y, color.z));
	glset.drawArrays(buttonCoor, false);

	if (!isTextBox) {
		renderText(uiPrograms.program, text, position_x -textRatio, position_y - 0.01, 0.00022f);
	}
	else {
		renderText(uiPrograms.program, text, -width + position_x, position_y - 0.01, 0.00022f);
	}
	glset.uniform1i(uiPrograms.program, "isUiTextureUsed", 0);
}
void UserInterface::colorBox(float position_x, float position_y,float valueX, float valueY) {
	ColorData colorData;
	glm::vec3 color = glm::vec3(1.0f,0.0f,0.0f);
	std::vector<float> boxCoor{
		// first triangle								    //Color - Normal Vectors Will Be Usen For Color Data Of Vertices
		 -0.10f + position_x,  0.20f + position_y, 0.9f,1.0f,1.0f	,1,1,1,  // top right
		 -0.10f + position_x, -0.20f + position_y, 0.9f,1.0f,0.0f	,0,0,0,  // bottom right
		 0.10f + position_x,  0.20f + position_y, 0.9f,0.0f,1.0f	,color.r,color.g+0.22f,color.b + 0.22f,  // top left 
		// second triangle
		 -0.10f + position_x, -0.20f + position_y, 0.9f,1.0f,0.0f	,0,0,0,  // bottom right
		 0.10f + position_x, -0.20f + position_y, 0.9f,0.0f,0.0f	,0,0,0,  // bottom left
		 0.10f + position_x,  0.20f + position_y, 0.9f,0.0f,1.0f	,color.r,color.g + 0.2f,color.b + 0.2f // top left
	};

	box(0.0f, 0.01f, position_x + valueX, position_y + valueY, "", colorData.colorBoxIndicatorColor, 0.045f, false, false, 0.9f, 18,glm::vec3(0),0);

	GlSet glset;
	glset.uniform1i(uiPrograms.program,"isColorBox",1);
	glset.drawArrays(boxCoor,false);
	glset.uniform1i(uiPrograms.program, "isColorBox", 0);

}
void UserInterface::colorRect(float position_x, float position_y,float value,unsigned int FBO,GLFWwindow* window) { //Changing colorBox value will be once the value changed
	std::vector<float> boxCoor{
		//Color - Normal Vectors Will Be Usen For Color Data Of Vertices
		 -0.01f + position_x,  0.04f + position_y + 0.16f, 0.9f ,1, (0.04f + 0.16f) *2.5 + 0.5f  ,1,0,0,  // top right
		 -0.01f + position_x, -0.04f + position_y + 0.16f, 0.9f ,1, (-0.04f + 0.16f)*2.5 + 0.5f  ,1,0,1,  // bottom right
		  0.01f + position_x,  0.04f + position_y + 0.16f, 0.9f ,0, (0.04f + 0.16f) *2.5 + 0.5f  ,1,0,0,  // top left 
		 -0.01f + position_x, -0.04f + position_y + 0.16f, 0.9f ,1, (-0.04f + 0.16f)*2.5 + 0.5f  ,1,0,1,  // bottom right
	 	  0.01f + position_x, -0.04f + position_y + 0.16f, 0.9f ,0, (-0.04f + 0.16f)*2.5 + 0.5f  ,1,0,1,  // bottom left
		  0.01f + position_x,  0.04f + position_y + 0.16f, 0.9f ,0, (0.04f + 0.16f) *2.5 + 0.5f  ,1,0,0,  // top left
																	 					   	     
		 -0.01f + position_x,  0.02f + position_y + 0.10f, 0.9f ,1, (0.02f + 0.10f) *2.5 + 0.5f  ,1,0,1,  // top right
		 -0.01f + position_x, -0.02f + position_y + 0.10f, 0.9f ,1, (-0.02f + 0.10f)*2.5 + 0.5f  ,0,0,1,  // bottom right
		  0.01f + position_x,  0.02f + position_y + 0.10f, 0.9f ,0, (0.02f + 0.10f) *2.5 + 0.5f  ,1,0,1,  // top left 
		 -0.01f + position_x, -0.02f + position_y + 0.10f, 0.9f ,1, (-0.02f + 0.10f)*2.5 + 0.5f  ,0,0,1,  // bottom right
		  0.01f + position_x, -0.02f + position_y + 0.10f, 0.9f ,0, (-0.02f + 0.10f)*2.5 + 0.5f  ,0,0,1,  // bottom left
		  0.01f + position_x,  0.02f + position_y + 0.10f, 0.9f ,0, (0.02f + 0.10f) *2.5 + 0.5f  ,1,0,1,  // top left
																	 					   	     
		 -0.01f + position_x,  0.02f + position_y + 0.06f, 0.9f ,1, (0.02f + 0.06f) *2.5 + 0.5f  ,0,0,1,  // top right
		 -0.01f + position_x, -0.02f + position_y + 0.06f, 0.9f ,1, (-0.02f + 0.06f)*2.5 + 0.5f  ,0,1,1,  // bottom right
		  0.01f + position_x,  0.02f + position_y + 0.06f, 0.9f ,0, (0.02f + 0.06f) *2.5 + 0.5f  ,0,0,1,  // top left 
		 -0.01f + position_x, -0.02f + position_y + 0.06f, 0.9f ,1, (-0.02f + 0.06f)*2.5 + 0.5f  ,0,1,1,  // bottom right
		  0.01f + position_x, -0.02f + position_y + 0.06f, 0.9f ,0, (-0.02f + 0.06f)*2.5 + 0.5f  ,0,1,1,  // bottom left
		  0.01f + position_x,  0.02f + position_y + 0.06f, 0.9f ,0, (0.02f + 0.06f) *2.5 + 0.5f  ,0,0,1,  // top left
																	 					   	     
		 -0.01f + position_x,  0.04f + position_y		 , 0.9f ,1, (0.04f + 0.0f)  *2.5 + 0.5f  ,0,1,1,  // top right
		 -0.01f + position_x, -0.04f + position_y		 , 0.9f ,1, (-0.04f + 0.0f) *2.5 + 0.5f  ,0,1,0,  // bottom right
		  0.01f + position_x,  0.04f + position_y		 , 0.9f ,0, (0.04f + 0.0f)  *2.5 + 0.5f  ,0,1,1,  // top left 
		 -0.01f + position_x, -0.04f + position_y		 , 0.9f ,1, (-0.04f + 0.0f) *2.5 + 0.5f  ,0,1,0,  // bottom right
		  0.01f + position_x, -0.04f + position_y	     , 0.9f ,0, (-0.04f + 0.0f) *2.5 + 0.5f  ,0,1,0,  // bottom left
		  0.01f + position_x,  0.04f + position_y		 , 0.9f ,0, (0.04f + 0.0f)  *2.5 + 0.5f  ,0,1,1,  // top left
																	 					   	     
		 -0.01f + position_x,  0.02f + position_y - 0.10f, 0.9f ,1, (0.02f - 0.10f) *2.5 + 0.5f  ,1,1,0,  // top right
		 -0.01f + position_x, -0.02f + position_y - 0.10f, 0.9f ,1, (-0.02f - 0.10f)*2.5 + 0.5f  ,1,1,0,  // bottom right
		  0.01f + position_x,  0.02f + position_y - 0.10f, 0.9f ,0, (0.02f - 0.10f) *2.5 + 0.5f  ,1,1,0,  // top left 
		 -0.01f + position_x, -0.02f + position_y - 0.10f, 0.9f ,1, (-0.02f - 0.10f)*2.5 + 0.5f  ,1,1,0,  // bottom right
		  0.01f + position_x, -0.02f + position_y - 0.10f, 0.9f ,0, (-0.02f - 0.10f)*2.5 + 0.5f  ,1,1,0,  // bottom left
		  0.01f + position_x,  0.02f + position_y - 0.10f, 0.9f ,0, (0.02f - 0.10f) *2.5 + 0.5f  ,1,1,0,  // top left
												  					 					   	     
		 -0.01f + position_x,  0.02f + position_y - 0.06f, 0.9f ,1, (0.02f - 0.06f) *2.5 + 0.5f  ,0,1,0,  // top right
		 -0.01f + position_x, -0.02f + position_y - 0.06f, 0.9f ,1, (-0.02f - 0.06f)*2.5 + 0.5f  ,1,1,0,  // bottom right
		  0.01f + position_x,  0.02f + position_y - 0.06f, 0.9f ,0, (0.02f - 0.06f) *2.5 + 0.5f  ,0,1,0,  // top left 
		 -0.01f + position_x, -0.02f + position_y - 0.06f, 0.9f ,1, (-0.02f - 0.06f)*2.5 + 0.5f  ,1,1,0,  // bottom right
		  0.01f + position_x, -0.02f + position_y - 0.06f, 0.9f ,0, (-0.02f - 0.06f)*2.5 + 0.5f  ,1,1,0,  // bottom left
		  0.01f + position_x,  0.02f + position_y - 0.06f, 0.9f ,0, (0.02f - 0.06f) *2.5 + 0.5f  ,0,1,0,  // top left
																	 					   	     
		 -0.01f + position_x,  0.04f + position_y - 0.16f, 0.9f ,1, (0.04f - 0.16f) *2.5 + 0.5f  ,1,1,0,  // top right
		 -0.01f + position_x, -0.04f + position_y - 0.16f, 0.9f ,1, (-0.04f - 0.16f)*2.5 + 0.5f  ,1,0,0,  // bottom right
		  0.01f + position_x,  0.04f + position_y - 0.16f, 0.9f ,0, (0.04f - 0.16f) *2.5 + 0.5f  ,1,1,0,  // top left 
		 -0.01f + position_x, -0.04f + position_y - 0.16f, 0.9f ,1, (-0.04f - 0.16f)*2.5 + 0.5f  ,1,0,0,  // bottom right
		  0.01f + position_x, -0.04f + position_y - 0.16f, 0.9f ,0, (-0.04f - 0.16f)*2.5 + 0.5f  ,1,0,0,  // bottom left
		  0.01f + position_x,  0.04f + position_y - 0.16f, 0.9f ,0, (0.04f - 0.16f) *2.5 + 0.5f  ,1,1,0,  // top left
	};

	GlSet glset;
	ColorData colorData;

	//Render color rectangle into the screen to get the value
	glset.viewport(1920, 1080);
	glset.uniform1i(uiPrograms.program, "isRenderTextureModeV", 1);
	glset.uniform1i(uiPrograms.program, "isRenderTextureMode", 1);
	glset.uniform1i(uiPrograms.program, "isTwoDimensional", 0);
	glset.bindFramebuffer(FBO);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	GLubyte* colorRectPixel = new GLubyte[1 * 1 * 3];//Color value
	glset.uniform1i(uiPrograms.program, "isColorBox", 1);
	glset.uniform1i(uiPrograms.program, "isRect", 1);
	glset.drawArrays(boxCoor, false); //Render Model
	glset.uniform1i(uiPrograms.program, "isRect", 1);
	glset.uniform1i(uiPrograms.program, "isColorBox", 0);
	glReadPixels(10, (value + 0.2f) * 2.5f * 1080, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, colorRectPixel);
	//Render color rectangle into the screen to get the value

	//Finish
	glset.uniform1i(uiPrograms.program, "isRenderTextureMode", 0);
	glset.uniform1i(uiPrograms.program, "isRenderTextureModeV", 0);
	glset.uniform1i(uiPrograms.program, "isTwoDimensional", 1);
	glset.bindFramebuffer(0);
	int screenSizeX;
	int screenSizeY;
	glfwGetWindowSize(window, &screenSizeX, &screenSizeY);
	glViewport(0, -(uiMaxScreenHeight - screenSizeY), uiMaxScreenWidth, uiMaxScreenHeight);

	glset.uniform3f(uiPrograms.program, "boxColor", colorRectPixel[0] / 255.0f, colorRectPixel[1] / 255.0f, colorRectPixel[2] / 255.0f);
	delete(colorRectPixel);
	//Finish

	box(0.01f, 0.005f, position_x, position_y + value, "", colorData.colorBoxIndicatorColor, 0.045f, false, false, 1.0f, 10000, glm::vec3(0), 0); //Value indicator

	glset.uniform1i(uiPrograms.program, "isColorBox", 1);
	glset.uniform1i(uiPrograms.program, "isRect", 1);
	glset.drawArrays(boxCoor, false); //Render color rectangle displayer
	glset.uniform1i(uiPrograms.program, "isColorBox", 0);
	glset.uniform1i(uiPrograms.program, "isRect", 0);
}
void UserInterface::rangeBar(float position_x, float position_y,float value) {
	ColorData colorData;
	GlSet gl;

	gl.uniform1f(3, "uiOpacity", 1.0f);
	box(0, 0.015f, position_x+value, position_y, "", colorData.rangeBarFront, 0.035f, false, false, 0.9f, 15, glm::vec3(0), 0);//Value Square
	box((value+0.11f) / 2, 0.015f, position_x+(value+0.11f) / 2 - 0.11f, position_y, "", colorData.rangeBarSlide, 0.035f, false, false, 0.9f, 15, glm::vec3(0), 0);//Range Rectangle
	box(0.11f, 0.015f, position_x, position_y, "", colorData.rangeBarBack, 0.035f, false, false, 0.9f, 15, glm::vec3(0), 0);//Range Rectangle
	gl.uniform1f(3, "uiOpacity", 0.5f);

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
void UserInterface::decorationSquare(float position_x, float position_y) {
	std::vector<float> buttonCoor{
		// first triangle
		 0.045f + position_x,  0.02f + position_y, 0.9f,0,0,0,0,0,  // top right
		 0.02f + position_x, -0.05f + position_y, 0.9f,0,0,0,0,0,  // bottom right
		-0.02f + position_x,  0.02f + position_y, 0.9f,0,0,0,0,0,  // top left 
		// second triangle						     
		 0.02f + position_x, -0.05f + position_y, 0.9f,0,0,0,0,0,  // bottom right
		-0.02f + position_x, -0.05f + position_y, 0.9f,0,0,0,0,0,  // bottom left
		-0.02f + position_x,  0.02f + position_y, 0.9f,0,0,0,0,0  // top left
	};
	GlSet glset;
	ColorData colorData;
	glset.uiDataToShaders(colorData.panelColorSnd);
	glset.drawArrays(buttonCoor, false);
}
bool UserInterface::isMouseOnPanelChangeButton(GLFWwindow* window, float position_x, float position_y, int mouseXpos, int mouseYpos) { //Button with different shape used in order to switch between panels
	std::vector<float> buttonCoor{
		// first triangle
		 0.00f + position_x,  0.02f + position_y, 0.9f,0,0,0,0,0,  // top right
		 0.00f + position_x, -0.05f + position_y, 0.9f,0,0,0,0,0,  // bottom right
		-0.025f + position_x,  0.02f + position_y, 0.9f,0,0,0,0,0,  // top left 
		// second triangle						     
		 0.00f + position_x, -0.05f + position_y, 0.9f,0,0,0,0,0,  // bottom right
		-0.025f + position_x, -0.02f + position_y, 0.9f,0,0,0,0,0,  // bottom left
		-0.025f + position_x,  0.02f + position_y, 0.9f,0,0,0,0,0  // top left
	};
	//glfwGetWindowSize();
	int screenSizeX;
	int screenSizeY;
	glfwGetWindowSize(window, &screenSizeX, &screenSizeY);

	float mouseFX = ((float)mouseXpos / (uiMaxScreenWidth/2)); //Screen Coord
	float mouseFY = (((float)mouseYpos / (uiMaxScreenHeight / 2)) - 1.0f) * -1.0f; //Screen Coord

	//Barycentric calculations
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
	//Barycentric calculations
}
void UserInterface::checkBox(float position_x, float position_y, std::string text, glm::vec3 color,bool mouseHover,bool checked) {
	ColorData colorData;
	if (!checked) {
		if (!mouseHover)
			box(0.0f, 0.02f, position_x, position_y, "", colorData.checkBoxColor, 0.00022f, false, false, 0.9f, 10, glm::vec3(0), 0);
		else
			box(0.0f, 0.02f, position_x, position_y, "", colorData.checkBoxCheckedColor, 0.00022f, false, false, 0.9f, 10, glm::vec3(0), 0);
	}
	else {
		box(0.0f, 0.02f, position_x, position_y, "", glm::vec3(1.0f) - colorData.checkBoxColor, 0.00022f, false, false, 0.9f, 10, glm::vec3(0), 0);

	}
	renderText(uiPrograms.program, text, position_x+0.02f, position_y - 0.01, 0.00022f);
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
	int screenSizeX;
	int screenSizeY;
	glfwGetWindowSize(window,&screenSizeX,&screenSizeY);
	float mouseFX;
	if (!isPanelMoving) {
		mouseFX = ((float)mouseXpos / (uiMaxScreenWidth / 2)) - 1.0f;//Screen Coord
	}
	else {
		mouseFX = ((float)mouseXpos / (uiMaxScreenWidth / 2));//Screen Coord

	}
	float mouseFY = (((float)mouseYpos / (uiMaxScreenHeight / 2))-1.0f)*-1.0f;//Screen Coord

	//Barycentric calculations
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
	//Barycentric calculations
}
bool UserInterface::isMouseOnCoords(GLFWwindow*window,int mouseXpos, int mouseYpos,std::vector<float> buttonCoor,bool isPanelMoving){ //Return true if mouse hover on the given coordinates
	int screenSizeX;
	int screenSizeY;
	glfwGetWindowSize(window,&screenSizeX,&screenSizeY);

	float mouseFX;
	if (!isPanelMoving) {
		mouseFX = ((float)mouseXpos / (uiMaxScreenWidth / 2)) - 1.0f;//Screen Coord
	}
	else {
		mouseFX = ((float)mouseXpos / (uiMaxScreenWidth / 2));//Screen Coord

	}
	float mouseFY = (((float)mouseYpos / (uiMaxScreenHeight / 2))-1.0f)*-1.0f;//Screen Coord

	//Barycentric calculations
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
	//Barycentric calculations
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
	if (FT_New_Face(ft, "LigidPainter/Resources/fonts/arial.ttf", 0, &face)) {
		std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
	}
	else {
		glset.activeTexture(GL_TEXTURE2);
		FT_Set_Pixel_Sizes(face, 0, 100);
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
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
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

void UserInterface::renderText(unsigned int program, std::string text, float x, float y, float scale) {
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
			 xpos,     ypos + h + 0.004f, 0.99f  ,0.0f, 0.0f,0,0,0,
			 xpos,     ypos,     0.99f  ,0.0f, 1.0f,0,0,0,
			 xpos + w, ypos,     0.99f  ,1.0f, 1.0f,0,0,0,
			
			 xpos,     ypos + h + 0.004f, 0.99f  ,0.0f, 0.0f,0,0,0,
			 xpos + w, ypos,     0.99f  ,1.0f, 1.0f,0,0,0,
			 xpos + w, ypos + h+ 0.004f, 0.99f  ,1.0f, 0.0f,0,0,0
		};
		glset.bindTexture(ch.TextureID);
		glset.drawArrays(vertices, false);
		x += (ch.Advance >> 6) * scale / 1.2; 
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
Icons UserInterface::loadIcons(){
	GlSet glset;
	Texture txtr;
	Icons icons;

	glset.activeTexture(GL_TEXTURE6);
	icons.dropperIcon = txtr.getTexture("LigidPainter/Resources/Icons/Dropper.png",0,0,false);
	icons.TDModel = txtr.getTexture("LigidPainter/Resources/Icons/3DModel.jpg",0,0,false);
	icons.BackfaceCulling = txtr.getTexture("LigidPainter/Resources/Icons/BackfaceCulling.jpg",0,0,false);
	icons.ColorPicker = txtr.getTexture("LigidPainter/Resources/Icons/ColorPicker.png",0,0,false);
	icons.Export = txtr.getTexture("LigidPainter/Resources/Icons/Export.jpg",0,0,false);
	icons.Folder = txtr.getTexture("LigidPainter/Resources/Icons/Folder.png",0,0,false);
	icons.ImportMask = txtr.getTexture("LigidPainter/Resources/Icons/ImportMask.png",0,0,false);
	icons.ImportModel = txtr.getTexture("LigidPainter/Resources/Icons/ImportModel.jpg",0,0,false);
	icons.ImportTexture = txtr.getTexture("LigidPainter/Resources/Icons/ImportTexture.jpg",0,0,false);
	icons.JpgFile = txtr.getTexture("LigidPainter/Resources/Icons/JpgFile.png",0,0,false);
	icons.MaskGausBlur = txtr.getTexture("LigidPainter/Resources/Icons/MaskGausBlur.png",0,0,false);
	icons.MaskOpacity = txtr.getTexture("LigidPainter/Resources/Icons/MaskOpacity.png",0,0,false);
	icons.MaskRotation = txtr.getTexture("LigidPainter/Resources/Icons/MaskRotation.png",0,0,false);
	icons.MaskScale = txtr.getTexture("LigidPainter/Resources/Icons/MaskScale.png",0,0,false);
	icons.MaskSpacing = txtr.getTexture("LigidPainter/Resources/Icons/MaskSpacing.png",0,0,false);
	icons.Mirror = txtr.getTexture("LigidPainter/Resources/Icons/Mirror.jpg",0,0,false);
	icons.Panel = txtr.getTexture("LigidPainter/Resources/Icons/Panel.png",0,0,false);
	icons.PngFile = txtr.getTexture("LigidPainter/Resources/Icons/PngFile.png",0,0,false);
	icons.Sphere = txtr.getTexture("LigidPainter/Resources/Icons/Sphere.png",0,0,false);
	icons.Triangulate = txtr.getTexture("LigidPainter/Resources/Icons/Triangulate.jpg",0,0,false);
	icons.ArrowRight = txtr.getTexture("LigidPainter/Resources/Icons/ArrowRight.jpg",0,0,false);
	icons.ArrowLeft = txtr.getTexture("LigidPainter/Resources/Icons/ArrowLeft.jpg",0,0,false);
	icons.Painting = txtr.getTexture("LigidPainter/Resources/Icons/Painting.jpg",0,0,false);

	return icons;
}

void UserInterface::sendProgramsToUserInterface(Programs appuiPrograms){
	uiPrograms = appuiPrograms;
}
void UserInterface::sendMaxWindowSize(int maxScreenWidth,int maxScreenHeight){
	uiMaxScreenHeight = maxScreenHeight;
	uiMaxScreenWidth = maxScreenWidth;
}