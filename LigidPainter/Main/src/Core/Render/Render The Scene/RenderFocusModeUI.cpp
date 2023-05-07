#include<iostream>

#include<glad/glad.h>
#include<GLFW/glfw3.h>

#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <filesystem>


#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

#include "Core/Model/model.h"

#include "Core/Render/Render.h"
#include "Core/LigidPainter.h"
#include "Core/UI/UserInterface.h"
#include "Core/Utilities.h"
#include "Core/gl.h"
#include "Core/Texture/Texture.h"
#include "Core/Load.hpp"

#include "tinyfiledialogs.h"

double lastMouseXFocused = 0;
double lastMouseYFocused = 0;
void Render::renderFocusModeUI(Programs programs,RenderData &renderData,std::vector<UIElement> &UIElements,Icons icons,ColoringPanel &coloringPanel,SaturationValShaderData saturationValShaderData,double mouseXpos,double mouseYpos,bool firstClick,unsigned int FBOScreen,ColorPicker &colorPicker,glm::vec3 screenHoverPixel,glm::vec3 &drawColor,OutShaderData &outShaderData) {

	ColorData colorData;
	glm::mat4 projection;
	UserInterface ui;
	Utilities util;
    GlSet gl; 
	RenderOutData uiOut;
	Texture txtr;
	uiOut.maskPanelMaskClicked = false;
	uiOut.maskPanelMaskHover = false;
	uiOut.texturePanelButtonClicked = false;
	uiOut.texturePanelButtonHover = false;

	float centerDivider;
	float centerSum;
	centerDivider = 2.0f;
	centerSum = 0;
	projection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f);
	glUseProgram(programs.rampProgram);
	gl.uniformMatrix4fv(programs.rampProgram, "TextProjection", projection);
	glUseProgram(programs.iconsProgram);
	gl.uniformMatrix4fv(programs.iconsProgram, "Projection", projection);
	glUseProgram(programs.renderTheTextureProgram);
	gl.uniformMatrix4fv(programs.renderTheTextureProgram, "TextProjection", projection);
	glUseProgram(programs.curveProgram);
	gl.uniformMatrix4fv(programs.curveProgram, "TextProjection", projection);
	glUseProgram(programs.uiProgram);
	gl.uniformMatrix4fv(programs.uiProgram, "TextProjection", projection);
	float centerCoords = (renderData.panelLoc + max(renderData.panelLoc - 1.7f,0.0f)) / centerDivider + centerSum;

	int screenSizeX;
	int screenSizeY;
	glfwGetFramebufferSize(renderData.window,&screenSizeX,&screenSizeY);

	float screenGapX = ((float)glfwGetVideoMode(glfwGetPrimaryMonitor())->width - screenSizeX)/(((float)glfwGetVideoMode(glfwGetPrimaryMonitor())->width)/2.0f)/2.0f; 

	for (size_t i = 0; i < UIElements.size(); i++)
	{
		std::string currentType = UIElements[i].type;
				
		if(!UIElements[i].attachedToMainPanel || UIElements[i].focusMode){
			centerCoords =  renderData.panelLoc - 1.0f;
		}
		else{
			centerCoords = (renderData.panelLoc + max(renderData.panelLoc - 1.7f,0.0f)) / centerDivider + centerSum;
		}


		bool panelCompatibility;
		if(UIElements[i].panel == 3){
			panelCompatibility = true;
		}
		else{
			panelCompatibility = false;
		}
		if(renderData.panelLoc > 1.98f)
			panelCompatibility = false;

		if(panelCompatibility && UIElements[i].focusMode){
			float slideVal = 0.0f;
			const int uiIconStartingIndex = 30; 
			glUseProgram(programs.uiProgram);

			if(currentType == "button"){
				ui.box(UIElements[i].button.width, UIElements[i].button.height, centerCoords - screenGapX + UIElements[i].button.positionX, UIElements[i].button.positionY+slideVal, UIElements[i].button.text, UIElements[i].button.color, UIElements[i].button.textRatio, false, false, UIElements[i].button.positionZ, UIElements[i].button.buttonCurveReduce, UIElements[i].button.colorHover, UIElements[i].button.transitionMixVal); //Add mask texture button
			}
			if(currentType == "text"){	
				ui.renderText(programs.uiProgram,UIElements[i].text.text, centerCoords - screenGapX + UIElements[i].text.positionX, UIElements[i].text.positionY+slideVal, UIElements[i].text.scale,colorData.textColor,0.99999f,false);
			}
			if(currentType == "rangeBar"){
				if(!UIElements[i].rangeBar.isConstant)
					ui.rangeBar(centerCoords - screenGapX + UIElements[i].rangeBar.positionX, UIElements[i].rangeBar.positionY+slideVal, UIElements[i].rangeBar.value,UIElements[i].rangeBar.widthDivider);
				else{
					if(!UIElements[i].rangeBar.isRich)
						ui.constRangeBar(centerCoords - screenGapX + UIElements[i].rangeBar.positionX, UIElements[i].rangeBar.positionY+slideVal, UIElements[i].rangeBar.value,icons,UIElements[i].rangeBar.mixVal,UIElements[i].rangeBar.lastVal,UIElements[i].rangeBar.increase);
					else
						ui.richConstRangeBar(centerCoords - screenGapX + UIElements[i].rangeBar.positionX, UIElements[i].rangeBar.positionY+slideVal, UIElements[i].rangeBar.value,icons,UIElements[i].rangeBar.mixVal,UIElements[i].rangeBar.lastVal,UIElements[i].rangeBar.increase,UIElements[i].rangeBar.hover,UIElements[i].rangeBar.name);
				}
			}
			if(currentType == "textBox"){
				ui.box(UIElements[i].textBox.width, UIElements[i].textBox.height,centerCoords - screenGapX + UIElements[i].textBox.position_x, UIElements[i].textBox.position_y+slideVal,UIElements[i].textBox.text , colorData.textBoxColor, 0 , true, false, UIElements[i].textBox.position_z, 10 , colorData.textBoxColorClicked, UIElements[i].textBox.transitionMixVal); //Add mask texture button
			}
			if(currentType == "checkBox"){
				ui.checkBox(centerCoords - screenGapX + UIElements[i].checkBox.positionX, UIElements[i].checkBox.positionY+slideVal, UIElements[i].checkBox.text,  UIElements[i].checkBox.mouseHover,  UIElements[i].checkBox.checked,icons.Circle); //jpg checkbox
			}
			if(currentType == "icon"){
				glUseProgram(programs.iconsProgram);
				ui.iconBox(UIElements[i].icon.width,UIElements[i].icon.height,centerCoords - screenGapX + UIElements[i].icon.positionX ,UIElements[i].icon.positionY+slideVal,UIElements[i].icon.positionZ,UIElements[i].icon.icon, UIElements[i].icon.mixVal , UIElements[i].icon.color , UIElements[i].icon.colorHover);
			}
		}
	}	
	if(glfwGetMouseButton(renderData.window,2) == GLFW_PRESS){
		coloringPanel.active = true;
		coloringPanel.panelPosX = mouseXpos/(glfwGetVideoMode(glfwGetPrimaryMonitor())->width/2.f) - 1.0f + screenGapX ;
		coloringPanel.panelPosY = -mouseYpos/glfwGetVideoMode(glfwGetPrimaryMonitor())->height*2.f + 1.0f;
	}
	
	double xOffset = mouseXpos - lastMouseXFocused;
	double yOffset = mouseYpos - lastMouseYFocused;	

	lastMouseXFocused = mouseXpos;
	lastMouseYFocused = mouseYpos;

	if(coloringPanel.active){
		glUseProgram(programs.uiProgram);
		bool brushChanged = ui.coloringPanel(coloringPanel,programs,icons,renderData.window,saturationValShaderData,projection,mouseXpos,mouseYpos,firstClick,xOffset,yOffset,FBOScreen,colorPicker,screenGapX,screenHoverPixel,UIElements,true);
		drawColor = coloringPanel.result/glm::vec3(255.f); 
		colorPicker.pickerValue = coloringPanel.result;
		
		float brushBlurVal = ((UIElements[UIbrushBlurRangeBar].rangeBar.value + 0.11f) * 545.454545455f) + 1.0f; //Max 120
		if(brushChanged){
			txtr.updateMaskTexture(FBOScreen,screenSizeX,screenSizeY, UIElements[UIbrushRotationRangeBar].rangeBar.value,false,UIElements[UIbrushBordersRangeBar].rangeBar.value,brushBlurVal,outShaderData,programs,glfwGetVideoMode(glfwGetPrimaryMonitor())->width,glfwGetVideoMode(glfwGetPrimaryMonitor())->height);
		}
	}

}
//--------------------RENDER UI --------------------\\