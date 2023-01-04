#include<iostream>

#include <string>
#include <vector>
#include <map>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

#include "Core/LigidPainter.h"
#include "Core/UI/UserInterface.h"
#include "Core/gl.h"
#include "Core/Load.hpp"


Programs uiPrograms;

ColorData colorD;

int uiMaxScreenWidth;
int uiMaxScreenHeight;

void UserInterface::box(float width, float height, float position_x, float position_y,std::string text,glm::vec4 color, float textRatio,bool isTextBox,bool isMaskImageBox,float z,float buttonCurveReduce, glm::vec4 colorTransitionColor, float mixVal) {
	
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
		  position_x +  width + 0.08651f/ buttonCurveReduce 	, position_y + height / 1.29802f , z , 1 , 0.874506f 	, 0, 0, 1, 	//R / 2
		  position_x + -width							    	, position_y + height			 , z , 0 , 1.0f	     	, 0, 0, 1,	//L / 1
		  position_x + -width - 0.08651f / buttonCurveReduce	, position_y + height / 1.29802f , z , 0 , 0.874506f 	, 0, 0, 1,	//L / 2
		  position_x +  width + 0.15074f / buttonCurveReduce	, position_y + -height / 3.88978f, z , 1 , 0.375f	 	, 0, 0, 1, 	//R / 6
		  position_x + -width - 0.15800f / buttonCurveReduce	, position_y + 0				 , z , 0 , 0.5f		 	, 0, 0, 1,	//L / M
		  position_x + -width - 0.15074f / buttonCurveReduce	, position_y + -height / 3.88978f, z , 0 , 0.375f	 	, 0, 0, 1,	//L / 6
		  position_x +  width + 0.15074f / buttonCurveReduce	, position_y + height / 3.88978f , z , 1 , 0.624971f	, 0, 0, 1, 	//R / 4
		  position_x + -width - 0.13064f / buttonCurveReduce	, position_y + height / 2.00794f , z , 0 , 0.749012f	, 0, 0, 1,	//L / 3
		  position_x + -width - 0.15074f / buttonCurveReduce	, position_y + height / 3.88978f , z , 0 , 0.624971f	, 0, 0, 1,	//L / 4
		  position_x +  width + 0.08651f / buttonCurveReduce	, position_y + -height / 1.29802f, z , 1 , 0.125f		, 0, 0, 1,	//R / 8
		  position_x + -width - 0.13064f / buttonCurveReduce	, position_y + -height / 2.00794f, z , 0 , 0.25f		, 0, 0, 1,	//L / 7
		  position_x + -width - 0.08651f / buttonCurveReduce	, position_y + -height / 1.29802f, z , 0 , 0.125f		, 0, 0, 1,	//L / 8
		  position_x +  width									, position_y -height			 , z , 1 , 0.0f			, 0, 0, 1,	//R / 9
		  position_x + -width - 0.08651f / buttonCurveReduce	, position_y + -height / 1.29802f, z , 0 , 0.125f		, 0, 0, 1,	//L / 8
		  position_x + -width									, position_y -height			 , z , 0 , 0.0f			, 0, 0, 1,	//L / 9
		  position_x +  width + 0.13064f / buttonCurveReduce	, position_y + -height / 2.00794f, z , 1 , 0.25f		, 0, 0, 1,	//R / 7
		  position_x + -width - 0.15074f / buttonCurveReduce	, position_y + -height / 3.88978f, z , 0 , 0.375f		, 0, 0, 1,	//L / 6
		  position_x + -width - 0.13064f / buttonCurveReduce	, position_y + -height / 2.00794f, z , 0 , 0.25f		, 0, 0, 1,	//L / 7
		  position_x +  width + 0.15800f / buttonCurveReduce	, position_y + -0				 , z , 1 , 0.5f			, 0, 0, 1,	//R / M
		  position_x + -width - 0.15074f / buttonCurveReduce	, position_y + height / 3.88978f , z , 0 , 0.624971f	, 0, 0, 1,	//L / 4
		  position_x + -width - 0.15800f / buttonCurveReduce	, position_y + 0				 , z , 0 , 0.5f			, 0, 0, 1,	//L / M
		  position_x +  width + 0.13064f / buttonCurveReduce	, position_y + height / 2.00794f , z , 1 , 0.749012f	, 0, 0, 1,	//R / 3
		  position_x + -width - 0.08651f / buttonCurveReduce	, position_y + height / 1.29802f , z , 0 , 0.874506f	, 0, 0, 1,	//L / 2
		  position_x + -width - 0.13064f / buttonCurveReduce	, position_y + height / 2.00794f , z , 0 , 0.749012f	, 0, 0, 1,	//L / 3
		  position_x +  width + 0.08651f / buttonCurveReduce	, position_y + height / 1.29802f , z , 1 , 0.874506f	, 0, 0, 1,	//R / 2
		  position_x +  width									, position_y + height			 , z , 1 , 1.0f			, 0, 0, 1,	//R / 1
		  position_x + -width									, position_y + height			 , z , 0 , 1.0f			, 0, 0, 1,	//L / 1
		  position_x +  width + 0.15074f / buttonCurveReduce	, position_y + -height / 3.88978f, z , 1 , 0.375f		, 0, 0, 1,	//R / 6
		  position_x +  width + 0.15800f / buttonCurveReduce	, position_y + -0				 , z , 1 , 0.5f			, 0, 0, 1,	//R / M
		  position_x + -width - 0.15800f / buttonCurveReduce	, position_y + 0				 , z , 0 , 0.5f			, 0, 0, 1,	//L / M
		  position_x +  width + 0.15074f / buttonCurveReduce	, position_y + height / 3.88978f , z , 1 , 0.624971f	, 0, 0, 1,	//R / 4
		  position_x +  width + 0.13064f / buttonCurveReduce	, position_y + height / 2.00794f , z , 1 , 0.749012f	, 0, 0, 1,	//R / 3
		  position_x + -width - 0.13064f / buttonCurveReduce	, position_y + height / 2.00794f , z , 0 , 0.749012f	, 0, 0, 1,	//L / 3
		  position_x +  width + 0.08651f / buttonCurveReduce	, position_y + -height / 1.29802f, z , 1 , 0.125f		, 0, 0, 1,	//R / 8
		  position_x +  width + 0.13064f / buttonCurveReduce	, position_y + -height / 2.00794f, z , 1 , 0.25f		, 0, 0, 1,	//R / 7
		  position_x + -width - 0.13064f / buttonCurveReduce	, position_y + -height / 2.00794f, z , 0 , 0.25f		, 0, 0, 1,	//L / 7
		  position_x +  width									, position_y -height		 	 , z , 1 , 0.0f			, 0, 0, 1,	//R / 9
		  position_x +  width + 0.08651f / buttonCurveReduce	, position_y + -height / 1.29802f, z , 1 , 0.125f		, 0, 0, 1,	//R / 8
		  position_x + -width - 0.08651f / buttonCurveReduce	, position_y + -height / 1.29802f, z , 0 , 0.125f		, 0, 0, 1,	//L / 8
		  position_x +  width + 0.13064f / buttonCurveReduce	, position_y + -height / 2.00794f, z , 1 , 0.25f		, 0, 0, 1,	//R / 7
		  position_x +  width + 0.15074f / buttonCurveReduce	, position_y + -height / 3.88978f, z , 1 , 0.375f		, 0, 0, 1,	//R / 6
		  position_x + -width - 0.15074f / buttonCurveReduce	, position_y + -height / 3.88978f, z , 0 , 0.375f		, 0, 0, 1,	//L / 6
		  position_x +  width + 0.15800f / buttonCurveReduce	, position_y + -0				 , z , 1 , 0.5f			, 0, 0, 1,	//R / M
		  position_x +  width + 0.15074f / buttonCurveReduce	, position_y + height / 3.88978f , z , 1 , 0.624971f	, 0, 0, 1,	//R / 4
		  position_x + -width - 0.15074f / buttonCurveReduce	, position_y + height / 3.88978f , z , 0 , 0.624971f	, 0, 0, 1,	//L / 4
		  position_x +  width + 0.13064f / buttonCurveReduce	, position_y + height / 2.00794f , z , 1 , 0.749012f	, 0, 0, 1,	//R / 3
		  position_x +  width + 0.08651f / buttonCurveReduce	, position_y + height / 1.29802f , z , 1 , 0.874506f	, 0, 0, 1,	//R / 2
		  position_x + -width - 0.08651f / buttonCurveReduce	, position_y + height / 1.29802f , z , 0 , 0.874506f	, 0, 0, 1,	//L / 2
	}; 	

	glset.uniform1i(uiPrograms.uiProgram,"isUiTextureUsed",isMaskImageBox);

	glset.uniform4fv(uiPrograms.uiProgram,"uiColor",color);
	glset.uniform4fv(uiPrograms.uiProgram, "uiTransitionColor", colorTransitionColor);

	glset.uniform1f(uiPrograms.uiProgram, "uiTransitionMixVal", mixVal);

	glset.drawArrays(buttonCoor, false);

	if (!isTextBox) {
		renderText(uiPrograms.uiProgram, text, position_x -textRatio, position_y - 0.01f, 0.00022f);
	}
	else {
		renderText(uiPrograms.uiProgram, text, -width + position_x, position_y - 0.01f, 0.00022f);
	}
	glset.uniform1i(uiPrograms.uiProgram, "isUiTextureUsed", 0);
}

void UserInterface::panel(float panelLoc, float) {
	GlSet glset;

	box(0.00f, 0.04f, panelLoc + 0.022f, 0.88f, "", colorD.panelColor, 0.022f, false, false, 0.1f, 7, colorD.panelColor, 0);//Load model button
	box(0.00f, 0.04f, panelLoc + 0.022f, -0.88f, "", colorD.panelColor, 0.022f, false, false, 0.1f, 7, colorD.panelColor, 0);//Load model button

	const float panelWidth = 0.2f;
	const float panelHeigth = 0.88f;
	
	box(0.02f, panelHeigth, panelLoc + 0.02f, 0.0f, "", colorD.panelColor, 0.022f, false, false, 0.1f, 10000, colorD.panelColor, 0);//Load model button
	box(panelWidth, 0.04f, panelLoc + panelWidth + 0.02f, 0.88f, "", colorD.panelColor, 0.022f, false, false, 0.1f, 10000, colorD.panelColor, 0);//Load model button
	box(panelWidth, 0.04f, panelLoc + panelWidth + 0.02f, -0.88f, "", colorD.panelColor, 0.022f, false, false, 0.1f, 10000, colorD.panelColor, 0);//Load model button
	box(panelWidth, panelHeigth - 0.02f, panelLoc + panelWidth + 0.02f, 0.0f, "", colorD.panelColor, 0.022f, false, false, 0.1f, 10000, colorD.panelColor, 0);//Load model button

}

void UserInterface::textureDisplayer(float width,float height, float position_x,float position_y,float z){ 
	GlSet glset;
	ColorData clrData;
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
	box(0.005f,0.035f,position_x+0.005f,position_y-0.01f,"", clrData.textureDisplayerButtonColor,0,0,0,1,10,glm::vec4(0,0,0,1),0);

	glset.uniform1i(uiPrograms.uiProgram,"drawTxtrDemonstrator",1);
	glset.drawArrays(buttonCoorSq,false);
	glset.uniform1i(uiPrograms.uiProgram,"drawTxtrDemonstrator",0);
}

void UserInterface::verticalRangeBar(float positionX,float positionY,float height,float value){
	ColorData colorData;

	//Pointer 
	box(0.0f, 0.015f, positionX, positionY - value, "",colorData.maskPanelSliderColor, 0.095f, false, false, 0.9f, 20, glm::vec4(0), 0); //Mask panel slider
		
	//Background
	box(0.008f, height, positionX, positionY - height, "",colorData.maskPanelSliderBackgroundColor, 0.095f, false, false, 0.9f, 10000, glm::vec4(0), 0); 
	box(0.0f, 0.015f, positionX, positionY , "", colorData.maskPanelSliderBackgroundColor, 0.095f, false, false, 0.9f, 20, glm::vec4(0), 0); 
	box(0.0f, 0.015f, positionX, positionY - height*2.f, "", colorData.maskPanelSliderBackgroundColor, 0.095f, false, false, 0.9f, 20, glm::vec4(0), 0); 
}


void UserInterface::numericModifier(float position_x,float position_y,unsigned int leftArrow,unsigned int rightArrow,float z,int value,float mixValP,float mixValN){
	//box(0.005f,0.035f,position_x+0.005f,position_y-0.01f,"",glm::vec3(0),0,0,0,1,10,glm::vec3(0),0);
	ColorData clrData;
	
	glUseProgram(uiPrograms.iconsProgram);
	iconBox(0.02f,0.04f,position_x - 0.05f,position_y,z,leftArrow,mixValN,clrData.numericModifierArrowColor,clrData.numericModifierArrowHoverColor);
	iconBox(0.02f,0.04f,position_x + 0.05f,position_y,z,rightArrow,mixValP,clrData.numericModifierArrowColor,clrData.numericModifierArrowHoverColor);

	glUseProgram(uiPrograms.uiProgram);
	renderText(uiPrograms.uiProgram,std::to_string(value),position_x-0.01f,position_y-0.01f,0.00022f);
}

void UserInterface::iconBox(float width, float height, float position_x, float position_y, float z,	unsigned int icon,float mixVal,glm::vec3 color,glm::vec3 colorHover){
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

	glset.uniform3fv(uiPrograms.iconsProgram,"iconColor",color);
	glset.uniform3fv(uiPrograms.iconsProgram,"iconColorHover",colorHover);
	
	glset.uniform1f(uiPrograms.iconsProgram,"iconMixVal",mixVal);
	glset.activeTexture(GL_TEXTURE6);
	glset.bindTexture(icon);
	glset.drawArrays(buttonCoorSq,false);
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
	

	GlSet glset;
	glset.drawArrays(boxCoor,false);

	glUseProgram(uiPrograms.uiProgram);
	box(0.0f, 0.01f, position_x + valueX, position_y + valueY, "", colorData.colorBoxIndicatorColor, 0.045f, false, false, 1.0f, 22,glm::vec4(0),0);
}

glm::vec3 hueValue;
glm::vec3 UserInterface::hueBar(float position_x, float position_y,float value,unsigned int FBO,GLFWwindow* window, glm::mat4 projection,bool updateHueVal) { //Changing colorBox value will be once the value changed
	std::vector<float> boxCoor{
		//Color - Normal Vectors Will Be Usen For Color Data Of Vertices

		//Red to pink
		 -0.01f + position_x,  0.03f + position_y + 0.15f, 0.9f ,1, (0.03f + 0.15f) *2.77777777778f + 0.5f  ,1,0,0,  // top right
		 -0.01f + position_x, -0.03f + position_y + 0.15f, 0.9f ,1, (-0.03f + 0.15f)*2.77777777778f + 0.5f  ,1,0,1,  // bottom right
		  0.01f + position_x,  0.03f + position_y + 0.15f, 0.9f ,0, (0.03f + 0.15f) *2.77777777778f + 0.5f  ,1,0,0,  // top left 
		 -0.01f + position_x, -0.03f + position_y + 0.15f, 0.9f ,1, (-0.03f + 0.15f)*2.77777777778f + 0.5f  ,1,0,1,  // bottom right
	 	  0.01f + position_x, -0.03f + position_y + 0.15f, 0.9f ,0, (-0.03f + 0.15f)*2.77777777778f + 0.5f  ,1,0,1,  // bottom left
		  0.01f + position_x,  0.03f + position_y + 0.15f, 0.9f ,0, (0.03f + 0.15f) *2.77777777778f + 0.5f  ,1,0,0,  // top left
		
		//Pink to blue
		 -0.01f + position_x,  0.03f + position_y + 0.09f, 0.9f ,1, (0.03f + 0.09f) *2.77777777778f + 0.5f  ,1,0,1,  // top right
		 -0.01f + position_x, -0.03f + position_y + 0.09f, 0.9f ,1, (-0.03f + 0.09f)*2.77777777778f + 0.5f  ,0,0,1,  // bottom right
		  0.01f + position_x,  0.03f + position_y + 0.09f, 0.9f ,0, (0.03f + 0.09f) *2.77777777778f + 0.5f  ,1,0,1,  // top left 
		 -0.01f + position_x, -0.03f + position_y + 0.09f, 0.9f ,1, (-0.03f + 0.09f)*2.77777777778f + 0.5f  ,0,0,1,  // bottom right
		  0.01f + position_x, -0.03f + position_y + 0.09f, 0.9f ,0, (-0.03f + 0.09f)*2.77777777778f + 0.5f  ,0,0,1,  // bottom left
		  0.01f + position_x,  0.03f + position_y + 0.09f, 0.9f ,0, (0.03f + 0.09f) *2.77777777778f + 0.5f  ,1,0,1,  // top left

		//blue to cyan			3				 					   	     
		 -0.01f + position_x,  0.03f + position_y + 0.03f, 0.9f ,1, (0.03f + 0.03f) *2.77777777778f + 0.5f  ,0,0,1,  // top right
		 -0.01f + position_x, -0.03f + position_y + 0.03f, 0.9f ,1, (-0.03f + 0.03f)*2.77777777778f + 0.5f  ,0,1,1,  // bottom right
		  0.01f + position_x,  0.03f + position_y + 0.03f, 0.9f ,0, (0.03f + 0.03f) *2.77777777778f + 0.5f  ,0,0,1,  // top left 
		 -0.01f + position_x, -0.03f + position_y + 0.03f, 0.9f ,1, (-0.03f + 0.03f)*2.77777777778f + 0.5f  ,0,1,1,  // bottom right
		  0.01f + position_x, -0.03f + position_y + 0.03f, 0.9f ,0, (-0.03f + 0.03f)*2.77777777778f + 0.5f  ,0,1,1,  // bottom left
		  0.01f + position_x,  0.03f + position_y + 0.03f, 0.9f ,0, (0.03f + 0.03f) *2.77777777778f + 0.5f  ,0,0,1,  // top left

		//cyan to green			3							 					   	     
		 -0.01f + position_x,  0.03f + position_y	- 0.03f	 , 0.9f ,1, (0.03f - 0.03f)  *2.77777777778f + 0.5f  ,0,1,1,  // top right
		 -0.01f + position_x, -0.03f + position_y	- 0.03f	 , 0.9f ,1, (-0.03f - 0.03f) *2.77777777778f + 0.5f  ,0,1,0,  // bottom right
		  0.01f + position_x,  0.03f + position_y	- 0.03f	 , 0.9f ,0, (0.03f - 0.03f)  *2.77777777778f + 0.5f  ,0,1,1,  // top left 
		 -0.01f + position_x, -0.03f + position_y	- 0.03f	 , 0.9f ,1, (-0.03f - 0.03f) *2.77777777778f + 0.5f  ,0,1,0,  // bottom right
		  0.01f + position_x, -0.03f + position_y	- 0.03f  , 0.9f ,0, (-0.03f - 0.03f) *2.77777777778f + 0.5f  ,0,1,0,  // bottom left
		  0.01f + position_x,  0.03f + position_y	- 0.03f	 , 0.9f ,0, (0.03f - 0.03f)  *2.77777777778f + 0.5f  ,0,1,1,  // top left

		//Green to yellow		3				  					 					   	     
		 -0.01f + position_x,  0.03f + position_y - 0.09f, 0.9f ,1, (0.03f - 0.09f) *2.77777777778f + 0.5f  ,0,1,0,  // top right
		 -0.01f + position_x, -0.03f + position_y - 0.09f, 0.9f ,1, (-0.03f - 0.09f)*2.77777777778f + 0.5f  ,1,1,0,  // bottom right
		  0.01f + position_x,  0.03f + position_y - 0.09f, 0.9f ,0, (0.03f - 0.09f) *2.77777777778f + 0.5f  ,0,1,0,  // top left 
		 -0.01f + position_x, -0.03f + position_y - 0.09f, 0.9f ,1, (-0.03f - 0.09f)*2.77777777778f + 0.5f  ,1,1,0,  // bottom right
		  0.01f + position_x, -0.03f + position_y - 0.09f, 0.9f ,0, (-0.03f - 0.09f)*2.77777777778f + 0.5f  ,1,1,0,  // bottom left
		  0.01f + position_x,  0.03f + position_y - 0.09f, 0.9f ,0, (0.03f - 0.09f) *2.77777777778f + 0.5f  ,0,1,0,  // top left

		//Yellow to red											 					   	     
		 -0.01f + position_x,  0.03f + position_y - 0.15f, 0.9f ,1, (0.03f - 0.15f) *2.77777777778f + 0.5f  ,1,1,0,  // top right
		 -0.01f + position_x, -0.03f + position_y - 0.15f, 0.9f ,1, (-0.03f - 0.15f)*2.77777777778f + 0.5f  ,1,0,0,  // bottom right
		  0.01f + position_x,  0.03f + position_y - 0.15f, 0.9f ,0, (0.03f - 0.15f) *2.77777777778f + 0.5f  ,1,1,0,  // top left 
		 -0.01f + position_x, -0.03f + position_y - 0.15f, 0.9f ,1, (-0.03f - 0.15f)*2.77777777778f + 0.5f  ,1,0,0,  // bottom right
		  0.01f + position_x, -0.03f + position_y - 0.15f, 0.9f ,0, (-0.03f - 0.15f)*2.77777777778f + 0.5f  ,1,0,0,  // bottom left
		  0.01f + position_x,  0.03f + position_y - 0.15f, 0.9f ,0, (0.03f - 0.15f) *2.77777777778f + 0.5f  ,1,1,0,  // top left
	};

	GlSet glset;
	ColorData colorData;
	HueShaderData hueShaderData;

	if(updateHueVal){

		glm::mat4 screenprojection = glm::ortho(0.0f, 1.77777777778f, 0.0f, 1.0f);
		hueShaderData.renderTextureProjection = screenprojection;
		hueShaderData.useTexCoords = 1;

		//Render color rectangle into the screen to get the value
		glset.useHueShader(uiPrograms.hueProgram,hueShaderData);

		glset.viewport(1920, 1081);
		glset.bindFramebuffer(FBO);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		GLubyte* hueBarPixel = new GLubyte[1 * 1 * 3];//Color value
		glset.drawArrays(boxCoor, false); //Render Model

		if( (value + 0.18f) * 2.77777777778f * 1080.0f != 1080.0f)
			glReadPixels(10, (value + 0.18f) * 2.77777777778f * 1080.0f, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, hueBarPixel);
		else{
			glReadPixels(10, 1079, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, hueBarPixel);
		}
		//Finish
		hueValue.r = hueBarPixel[0];
		hueValue.g = hueBarPixel[1];
		hueValue.b = hueBarPixel[2];

		delete(hueBarPixel);

		glset.bindFramebuffer(0);
		int screenSizeX;
		int screenSizeY;
		glfwGetWindowSize(window, &screenSizeX, &screenSizeY);
		glViewport(-(uiMaxScreenWidth - screenSizeX)/2, -(uiMaxScreenHeight - screenSizeY), uiMaxScreenWidth, uiMaxScreenHeight);
	}


	hueShaderData.useTexCoords = 0;
	hueShaderData.renderTextureProjection = projection;
	glset.useHueShader(uiPrograms.hueProgram,hueShaderData);

	//Finish
	glset.drawArrays(boxCoor, false); //Render color rectangle displayer
	glUseProgram(uiPrograms.uiProgram);
	box(0.01f, 0.005f, position_x, position_y + value, "", colorData.colorBoxIndicatorColor, 0.045f, false, false, 1.0f, 10000, glm::vec4(0), 0); //Value indicator


	return hueValue;
}
void UserInterface::rangeBar(float position_x, float position_y,float value) {
	ColorData colorData;
	GlSet gl;

	box(0, 0.015f, position_x+value, position_y, "", colorData.rangeBarFront, 0.035f, false, false, 0.9f, 15, glm::vec4(0), 0);//Value Square
	box((value+0.11f) / 2, 0.0075f, position_x+(value+0.11f) / 2 - 0.11f, position_y, "", colorData.rangeBarSlide, 0.035f, false, false, 0.9f, 15, glm::vec4(0), 0);//Range Rectangle
	box(0.11f, 0.0075f, position_x, position_y, "", colorData.rangeBarBack, 0.035f, false, false, 0.9f, 15, glm::vec4(0), 0);//Range Rectangle

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
	
	glset.uniform4fv(uiPrograms.uiProgram, "uiColor", colorData.panelHoldColor);
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
	
	glset.uniform4fv(uiPrograms.uiProgram, "uiColor", colorData.panelColorSnd);

	glset.drawArrays(buttonCoor, false);
}

void UserInterface::checkBox(float position_x, float position_y, std::string text, bool mouseHover,bool checked) {
	ColorData colorData;

	glm::vec4 color;

	if (!checked) {
		if (!mouseHover)
			color = colorData.checkBoxColor;
		else
			color =  colorData.checkBoxHoverColor;
	}
	else {
		color = colorData.checkBoxCheckedColor;
	}
	box(0.002f, 0.02f, position_x, position_y, "", color, 0.00022f, false, false, 0.9f, 13.0f, glm::vec4(0), 0);
	
	renderText(uiPrograms.uiProgram, text, position_x+0.02f, position_y - 0.01f, 0.00022f);
}


void UserInterface::setViewportBgColor() {
	glClearColor(colorD.viewportBackColor.x, colorD.viewportBackColor.y, colorD.viewportBackColor.z, 1.0f);
}

std::map<char, character> characters;
void UserInterface::renderText(unsigned int program, std::string text, float x, float y, float scale) {
	GlSet glset;
	glset.activeTexture(GL_TEXTURE2);

	glset.uniform1i(program,"isText", 1);
	glset.uniform1i(program, "isTextF", 1);
	
	std::string::const_iterator c;
	for (c = text.begin(); c != text.end(); c++)
	{
		character ch = characters[*c];

		float xpos = x + ch.Bearing.x * scale;
		float ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

		float w = ch.Size.x * scale * 0.8f;
		float h = ch.Size.y * scale;

		std::vector <float> vertices = {
			 xpos,     ypos + h + 0.004f, 	  0.99999f  ,0.0f, 0.0f,	0,0,0,
			 xpos,     ypos,      			  0.99999f  ,0.0f, 1.0f,	0,0,0,
			 xpos + w, ypos,      			  0.99999f  ,1.0f, 1.0f,	0,0,0,
			
			 xpos,     ypos + h + 0.004f, 	  0.99999f  ,0.0f, 0.0f,	0,0,0,
			 xpos + w, ypos,      			  0.99999f  ,1.0f, 1.0f,	0,0,0,
			 xpos + w, ypos + h+  0.004f, 	  0.99999f  ,1.0f, 0.0f,	0,0,0
		};

		glset.bindTexture(ch.TextureID);
		glset.drawArrays(vertices, false);
		x += (ch.Advance >> 6) * scale / 1.2f; 
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

void Load::sendCharsToUI(std::map<char, character> theseCharacters){
	characters = theseCharacters;
}
void UserInterface::sendProgramsToUserInterface(Programs appuiPrograms){
	uiPrograms = appuiPrograms;
}
void UserInterface::sendMaxWindowSize(int maxScreenWidth,int maxScreenHeight){
	uiMaxScreenHeight = maxScreenHeight;
	uiMaxScreenWidth = maxScreenWidth;
	sendMaxWindowSizeToCalculationsAndMore(maxScreenWidth,maxScreenHeight);
}