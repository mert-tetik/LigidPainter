#include<iostream>

#include<glad/glad.h>
#include<GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

#include "Core/LigidPainter.h"
#include "Core/UI/UserInterface.h"
#include "Core/Callback.h"
#include "Core/gl.h"
#include "Core/UI/UserInterface.h"


int callbackMaxScreenWidth;
int callbackMaxScreenHeight;

//Camera 
float yaw = -90.0f;
float lastX = 400, lastY = 300;
float pitch = 0.0f;
float xoffset;
float yoffset;
float sensitivity = 0.2f;
float radius = 10.0;
glm::vec3 originPos = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 cameraPos = glm::vec3(0.034906f, 0.000000f, -9.999939f);
//Camera 

//Panel
int holdXPos = 1536;
float panelOffset;
bool movePanel;
//Panel

//Ui enter
bool modelPanelButtonEnter;
bool texturePanelButtonEnter;
bool exportPanelButtonEnter;
bool paintingPanelButtonEnter;
bool maskPanelSliderEnter;
bool maskPanelEnter;
bool mainPanelBoundariesEnter;
bool mainPanelEnter;
bool uiElementEnter;
//Ui enter

int screenX;
int screenY;

CallbckData preapareCallbackData() {
	CallbckData callbk;

	callbk.movePanel = movePanel;
	callbk.modelPanelButtonEnter = modelPanelButtonEnter;
	callbk.texturePanelButtonEnter = texturePanelButtonEnter;
	callbk.paintingPanelButtonEnter = paintingPanelButtonEnter;
	callbk.exportPanelButtonEnter = exportPanelButtonEnter;
	callbk.cameraPos = cameraPos;
	callbk.originPos = originPos;
	callbk.maskPanelSliderEnter = maskPanelSliderEnter;
	callbk.maskPanelEnter = maskPanelEnter;
	callbk.uiElementEnter = uiElementEnter;
	callbk.mainPanelBoundariesEnter = mainPanelBoundariesEnter;
	callbk.mainPanelEnter = mainPanelEnter;

	return callbk;
}

CallbckData Callback::scroll_callback(GLFWwindow* window, double scroll, double scrollx) {
	float originCameraDistance = glm::distance(originPos,cameraPos)/10;
	CallbckData callbk;
	if (scrollx == 1 && radius > 1) {
		radius-=originCameraDistance;
	}
	else if (scrollx == -1) {
		radius+=originCameraDistance;
	}
	//Zoom in-out
	cameraPos.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch)) * radius + originPos.x;
	cameraPos.y = sin(glm::radians(pitch)) * -radius + originPos.y;
	cameraPos.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch)) * radius + originPos.z;

	callbk = preapareCallbackData();
	return callbk;
}

CallbckData Callback::mouse_callback(GLFWwindow* window, double xpos, double ypos, PanelData panelData, float maskPanelSliderValue,bool brushMaskPanelMaskHover,
LigidCursors cursors,bool texturePanelButtonHover,std::vector<UIElement> &uiElements,float mainPanelLoc,ColorPicker &colorPicker,TextureDisplayer &textureDisplayer
, NodePanel &nodePanel, ContextMenu &addNodeContextMenu, SndPanel &sndPanel)
{
	CallbckData callbk;
	
	//Get window size
	int screenSizeX;
	int screenSizeY;
	glfwGetWindowSize(window, &screenSizeX, &screenSizeY);
	
	buttonCheck(window, xpos, ypos, panelData,maskPanelSliderValue,brushMaskPanelMaskHover,cursors, texturePanelButtonHover,uiElements,mainPanelLoc,
	colorPicker,textureDisplayer,nodePanel,addNodeContextMenu,sndPanel);	

	xoffset = xpos - lastX;
	yoffset = lastY - ypos;
	lastX = xpos;
	lastY = ypos;
	if (glfwGetMouseButton(window, 1) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
		xoffset *= sensitivity;
		yoffset *= sensitivity;

		//Straight Movement
		cameraPos.x -= sin(glm::radians(yaw)) * xoffset * (sensitivity / 2) + cos(glm::radians(yaw)) * (sin(glm::radians(pitch)) * yoffset) / 4;
		originPos.x -= sin(glm::radians(yaw)) * xoffset * (sensitivity / 2) + cos(glm::radians(yaw)) * (sin(glm::radians(pitch)) * yoffset) / 4;

		cameraPos.z += cos(glm::radians(yaw)) * xoffset * (sensitivity / 2) - sin(glm::radians(yaw)) * (sin(glm::radians(pitch)) * yoffset) / 4;
		originPos.z += cos(glm::radians(yaw)) * xoffset * (sensitivity / 2) - sin(glm::radians(yaw)) * (sin(glm::radians(pitch)) * yoffset) / 4;

		cameraPos.y -= cos(glm::radians(pitch)) * yoffset * (sensitivity / 2);
		originPos.y -= cos(glm::radians(pitch)) * yoffset * (sensitivity / 2);
	}
	else if (glfwGetMouseButton(window, 1) == GLFW_PRESS) {

		xoffset *= sensitivity;
		yoffset *= sensitivity;

		yaw += xoffset;
		pitch += yoffset;

		//Disable 90+ degrees rotations in y axis
		if (pitch > 89.0f)
			pitch = 89.0f;
		if (pitch < -89.0f)
			pitch = -89.0f;

		//Helical Movement
		cameraPos.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch)) * radius + originPos.x;
		cameraPos.y = sin(glm::radians(pitch)) * -radius + originPos.y;
		cameraPos.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch)) * radius + originPos.z;
	}
	callbk = preapareCallbackData();
	return callbk;
}
bool panelChangeHover = false;
bool panelClickTaken = false;
bool noPanelClick = true;

void Callback::buttonCheck(GLFWwindow* window, double mouseXPos,double mouseYPos,PanelData panelData,float maskPanelSliderValue,bool brushMaskPanelMaskHover,
LigidCursors cursors,bool texturePanelButtonHover,std::vector<UIElement> &uiElements,float mainPanelLoc,ColorPicker &colorPicker, TextureDisplayer &textureDisplayer
,NodePanel &nodePanel,ContextMenu &addNodeContextMenu,SndPanel &sndPanel) {
	UserInterface ui;

	float centerDivider;
	float centerSum;
	if (!movePanel) {
		centerDivider = 2.0f;
		centerSum = 0;
	}
	else {
		centerDivider = 1.0f;
		centerSum = 0.15f;
	}


	int screenSizeX;
	int screenSizeY;
	glfwGetWindowSize(window,&screenSizeX,&screenSizeY);

	float screenGapX = ((float)callbackMaxScreenWidth - screenSizeX)/(((float)callbackMaxScreenWidth)/2.0f); 


	if(!colorPicker.dropperActive){
		for (size_t i = 0; i < uiElements.size(); i++)
		{
			uiElementEnter = false;

			std::string currentType = uiElements[i].type; 

			bool panelCompatibility;
			if(uiElements[i].panel == 1 && panelData.modelPanelActive || uiElements[i].panel == 2 && panelData.texturePanelActive || uiElements[i].panel == 3 && panelData.paintingPanelActive || uiElements[i].panel == 4 && panelData.exportPanelActive || uiElements[i].panel == 0){
				panelCompatibility = true;
			}
			else{
				panelCompatibility = false;
			}
			float centerCoords = (mainPanelLoc + std::max(mainPanelLoc - 1.7f,0.0f)) / centerDivider + centerSum;
		
			if(uiElements[i].attachedToMainPanel == false){
				centerCoords =  mainPanelLoc - 1.0f;
			}

			if(panelCompatibility){
				if(currentType == "button"){
					uiElementEnter = ui.isMouseOnButton(window, uiElements[i].button.width + 0.02f, uiElements[i].button.height, centerCoords - screenGapX + uiElements[i].button.positionX, uiElements[i].button.positionY, mouseXPos, mouseYPos, movePanel);
					uiElements[i].button.hover = uiElementEnter;
				}
			
				if(currentType == "text"){	
				
				}
	
				if(currentType == "rangeBar"){
					uiElementEnter = ui.isMouseOnButton(window, 0.02f, 0.02f, centerCoords - screenGapX + uiElements[i].rangeBar.positionX + uiElements[i].rangeBar.value, uiElements[i].rangeBar.positionY, mouseXPos, mouseYPos, movePanel);
					uiElements[i].rangeBar.hover = uiElementEnter;
				}
	
				if(currentType == "textBox"){
					uiElementEnter = ui.isMouseOnButton(window, uiElements[i].textBox.width, uiElements[i].textBox.height, centerCoords - screenGapX + uiElements[i].textBox.position_x, uiElements[i].textBox.position_y, mouseXPos, mouseYPos, movePanel);
					uiElements[i].textBox.hover = uiElementEnter;
				}
	
				if(currentType == "checkBox"){
					uiElementEnter = ui.isMouseOnButton(window, 0.02f, 0.02f, centerCoords - screenGapX + uiElements[i].checkBox.positionX, uiElements[i].checkBox.positionY, mouseXPos, mouseYPos, movePanel);
					uiElements[i].checkBox.mouseHover = uiElementEnter;
				}
				if(currentType == "icon"){
				}
				
			}
			if(uiElementEnter){
				break;
			}
		}

		if(panelData.paintingPanelActive){
			colorPicker.saturationValuePointerHover = ui.isMouseOnButton(window,0.015f, 0.03f, mainPanelLoc / centerDivider + centerSum - screenGapX - 0.02f + colorPicker.saturationValuePosX, -0.55f + colorPicker.saturationValuePosY, mouseXPos, mouseYPos, movePanel);
			colorPicker.hueValuePointerHover = ui.isMouseOnButton(window, 0.01f, 0.01f, mainPanelLoc / centerDivider + centerSum - screenGapX + 0.1f, -0.55f + colorPicker.hueValue, mouseXPos, mouseYPos, movePanel);
			colorPicker.hexValTextBoxEnter =  ui.isMouseOnButton(window, 0.04f, 0.03f, mainPanelLoc / centerDivider + centerSum - screenGapX - 0.008f,-0.81f, mouseXPos, mouseYPos, movePanel);
			colorPicker.dropperEnter = ui.isMouseOnButton(window, 0.015f,0.03f, mainPanelLoc / centerDivider + centerSum - screenGapX + 0.08f, -0.81f, mouseXPos, mouseYPos,movePanel);
			
			colorPicker.saturationValueBoxHover = ui.isMouseOnButton(window, 0.1f, 0.2f, mainPanelLoc / centerDivider + centerSum - screenGapX - 0.02f, -0.55f, mouseXPos, mouseYPos, movePanel);		
			colorPicker.hueValueBarHover = ui.isMouseOnButton(window, 0.01f, 0.18f, mainPanelLoc / centerDivider + centerSum - screenGapX + 0.1f, -0.55f, mouseXPos, mouseYPos, movePanel);
		
			maskPanelEnter = ui.isMouseOnButton(window, 0.15f, 0.15f, mainPanelLoc / centerDivider + centerSum - screenGapX, 0.675f, mouseXPos, mouseYPos, movePanel);
		}

		nodePanel.panelHover = ui.isMouseOnNodePanel(window,mainPanelLoc - screenGapX, nodePanel.heigth,mouseXPos,mouseYPos,false);
		nodePanel.boundariesHover = ui.isMouseOnNodePanel(window,mainPanelLoc - screenGapX, nodePanel.heigth,mouseXPos,mouseYPos,true);

		//Add node context menu
		addNodeContextMenu.hover = ui.isMouseOnButton(window, addNodeContextMenu.width+0.02f, addNodeContextMenu.height+0.04f, addNodeContextMenu.positionX - screenGapX/2.f, addNodeContextMenu.positionY, mouseXPos, mouseYPos, false);
		for (size_t i = 0; i < addNodeContextMenu.buttons.size(); i++)
		{
			addNodeContextMenu.buttons[i].hover = ui.isMouseOnButton(window, addNodeContextMenu.buttons[i].width, addNodeContextMenu.buttons[i].height, addNodeContextMenu.positionX - screenGapX/2.f, addNodeContextMenu.buttons[i].positionY + addNodeContextMenu.positionY, mouseXPos, mouseYPos, false);
		}
		


		mainPanelBoundariesEnter =  ui.isMouseOnButton(window, 0.02f, 0.88f, mainPanelLoc -1.0f + 0.02f - screenGapX, 0.0f, mouseXPos, mouseYPos, false);
		
		mainPanelEnter = ui.isMouseOnButton(window, 0.2f,0.9f,mainPanelLoc - 1.0f - screenGapX + 0.2f, 0.0f, mouseXPos, mouseYPos, 0);
		
		textureDisplayer.buttonHover = ui.isMouseOnButton(window, 0.02f,0.045f,(textureDisplayer.buttonPosX+0.005f)-1.0f,textureDisplayer.buttonPosY-0.01f, mouseXPos, mouseYPos, 0);		

		sndPanel.boundariesHover = ui.isMouseOnButton(window, 0.02f,0.88,sndPanel.position,0.0f, mouseXPos, mouseYPos, 0);

		modelPanelButtonEnter = ui.isMouseOnPanelChangeButton(window, mainPanelLoc- screenGapX, 0.8f, mouseXPos, mouseYPos);
		texturePanelButtonEnter = ui.isMouseOnPanelChangeButton(window, mainPanelLoc- screenGapX, 0.72f, mouseXPos, mouseYPos);
		paintingPanelButtonEnter = ui.isMouseOnPanelChangeButton(window, mainPanelLoc- screenGapX, 0.64f, mouseXPos, mouseYPos);
		exportPanelButtonEnter = ui.isMouseOnPanelChangeButton(window, mainPanelLoc- screenGapX, 0.56f, mouseXPos, mouseYPos);
	}

	if(colorPicker.dropperActive){
		glfwSetCursor(window, cursors.dropperCursor);
	}
	else if(mainPanelBoundariesEnter){
		glfwSetCursor(window, cursors.pointerCursor);
	}
	else if(uiElementEnter){
		glfwSetCursor(window, cursors.pointerCursor);
	}
	else if(colorPicker.dropperEnter){
		glfwSetCursor(window, cursors.pointerCursor);
	}
	else if(textureDisplayer.buttonHover){
		glfwSetCursor(window, cursors.pointerCursor);
	}
	else if(colorPicker.saturationValuePointerHover || colorPicker.hueValuePointerHover || colorPicker.hexValTextBoxEnter){
		glfwSetCursor(window, cursors.pointerCursor);
	}
	else if(modelPanelButtonEnter || texturePanelButtonEnter || paintingPanelButtonEnter || exportPanelButtonEnter){
		glfwSetCursor(window, cursors.pointerCursor);
	}
	else if (!panelChangeHover){
		glfwSetCursor(window, cursors.defaultCursor);
	}
	
}
void Callback::framebuffer_size_callback(GLFWwindow* window, int width, int height) //Protect screen ratio
{
	glfwSetWindowSize(window, width, height);
	glViewport(-(callbackMaxScreenWidth - width)/2, -(callbackMaxScreenHeight - height), callbackMaxScreenWidth, callbackMaxScreenHeight);

	screenX = width;
	screenY = height;
}

void Callback::sendMaxWindowSize(int maxScreenWidth,int maxScreenHeight){
	callbackMaxScreenHeight = maxScreenHeight;
	callbackMaxScreenWidth = maxScreenWidth;
}