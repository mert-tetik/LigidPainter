#ifndef LGDMSHBOX_H
#define LGDMSHBOX_H

#include <GLFW/glfw3.h>

#include "Core/LigidPainter.h"
#include "Core/UI/UserInterface.h"

#include "glm/glm.hpp"

bool noButtonClickMsgBox = true;
bool clickTakenMsgBox = false;


int lgdMessageBox(GLFWwindow* window, double mouseXpos,double mouseYpos,GLFWcursor* defaultCursor,GLFWcursor* buttonHoverCursor,unsigned int icon,unsigned int uiProgram,const char * text, float textXpos,float textYpos,float backColor[3],float buttonColor[3],float renderMaxScreenWidth,float screenSizeX,unsigned int iconsProgram,Icons icons,Programs programs){
    UserInterface ui;
	ColorData colorData;
	int result = 2;

    glm::vec4 backColorVec;
    backColorVec.r = backColor[0];
    backColorVec.g = backColor[1];
    backColorVec.b = backColor[2];
	backColorVec.a = colorData.messageBoxPanelColor.a;

    glm::vec4 buttonColorVec;
    buttonColorVec.r = buttonColor[0];
    buttonColorVec.g = buttonColor[1];
    buttonColorVec.b = buttonColor[2];
	buttonColorVec.a = colorData.messageBoxButtonColor.a;


	float screenGapX = ((float)renderMaxScreenWidth - (float)screenSizeX)/(((float)renderMaxScreenWidth)/2.0f)/2.0f; 



	ui.container(+0.f, 0.f,0.899f,0.25f, 0.22f,backColorVec,programs,icons.Circle);
	glUseProgram(programs.uiProgram);
				
	//Buttons
	ui.box(0.04f, 0.04f, -0.1f, -0.15f, "Yes", buttonColorVec, 0.015f, false, false, 0.8999f, 10, buttonColorVec, 0);//Load model button
	ui.box(0.04f, 0.04f, +0.1f, -0.15f, "No", buttonColorVec, 0.012f, false, false, 0.8999f, 10, buttonColorVec, 0);//Load model button


	bool resultYesHover = ui.isMouseOnButton(window, 0.05f, 0.04f, -0.1f, -0.15f, mouseXpos, mouseYpos, false);//Yes button hover
	bool resultNoHover = ui.isMouseOnButton(window, 0.05f, 0.04f,  +0.1f, -0.15f, mouseXpos, mouseYpos, false);//No button hover

	//Make sure first click is done inside
	if (glfwGetMouseButton(window, 0) == GLFW_PRESS && !clickTakenMsgBox) {
		if (!resultYesHover && !resultNoHover) {
			noButtonClickMsgBox = true;
			}
		else{
				noButtonClickMsgBox = false;
			}
			clickTakenMsgBox = true;
	}
	if (glfwGetMouseButton(window, 0) == GLFW_RELEASE && clickTakenMsgBox) {
		clickTakenMsgBox = false;
	}


	if(resultYesHover || resultNoHover){
		glfwSetCursor(window,buttonHoverCursor);//Set pointer cursor once cursor enters any of the buttons
	}
	else{
		glfwSetCursor(window,defaultCursor);//Set pointer cursor once cursor enters any of the buttons
	}

	if(glfwGetMouseButton(window, 0) == GLFW_PRESS && resultYesHover && !noButtonClickMsgBox){
		result = 0; //Yes button pressed
	}
	else if(glfwGetMouseButton(window, 0) == GLFW_PRESS && resultNoHover && !noButtonClickMsgBox){
		result = 1; //No button pressed
	}

	//Text
	ui.renderText(uiProgram, text, textXpos, textYpos , 0.00022f,colorData.textColor,0.89999f,false);

	//LigidPainter icon
	glUseProgram(iconsProgram);
	ui.iconBox(0.04f,0.07f,0.0f,0.15f,1,icon,0,colorData.iconColor,colorData.iconColorHover);


	
	return result;	 //0 = Yes , 1 = No , 2 = skip/none
}

#endif // !LGDMSHBOX_H