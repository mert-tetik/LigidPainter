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
#include "Core/Utilities.h"



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
bool settingsPanelButtonEnter;
bool generatorPanelButtonEnter;
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
	callbk.settingsPanelButtonEnter = settingsPanelButtonEnter;
	callbk.generatorPanelButtonEnter = generatorPanelButtonEnter;
	callbk.cameraPos = cameraPos;
	callbk.originPos = originPos;
	callbk.maskPanelSliderEnter = maskPanelSliderEnter;
	callbk.maskPanelEnter = maskPanelEnter;
	callbk.uiElementEnter = uiElementEnter;
	callbk.mainPanelBoundariesEnter = mainPanelBoundariesEnter;
	callbk.mainPanelEnter = mainPanelEnter;

	return callbk;
}
void Callback::key_callback(int key, int action,GLFWwindow* window){
	Utilities util;
	float originCameraDistance = glm::distance(originPos,cameraPos);

	if(util.shortCut(window,GLFW_KEY_LEFT_CONTROL,GLFW_KEY_3,0,0) && action == 1){
		// radius
		cameraPos.x = -originCameraDistance;
		cameraPos.y = 0;
		cameraPos.z = 0;
		
		radius = originCameraDistance;

		yaw = 180.f;
		pitch = 0.f;
	}
	if(util.shortCut(window,GLFW_KEY_LEFT_CONTROL,GLFW_KEY_4,0,0) && action == 1){
		cameraPos.x = originCameraDistance;
		cameraPos.y = 0;
		cameraPos.z = 0;

		radius = originCameraDistance;
		
		yaw = 0.f;
		pitch = 0.f;
	}
	if(util.shortCut(window,GLFW_KEY_LEFT_CONTROL,GLFW_KEY_1,0,0) && action == 1){
		cameraPos.x = 0;
		cameraPos.y = 0;
		cameraPos.z = -originCameraDistance;

		radius = originCameraDistance;

		yaw = -90.f;
		pitch = 0.f;
	}
	if(util.shortCut(window,GLFW_KEY_LEFT_CONTROL,GLFW_KEY_2,0,0) && action == 1){
		cameraPos.x = 0;
		cameraPos.y = 0;
		cameraPos.z = originCameraDistance;

		radius = originCameraDistance;

		yaw = 90.f;
		pitch = 0.f;
	}
}
CallbckData Callback::scroll_callback(GLFWwindow* window, double scroll, double scrollx) {
	float originCameraDistance = glm::distance(originPos,cameraPos)/10;
	CallbckData callbk;
	if (scrollx > 0 && radius > 1) {
		radius-=originCameraDistance;
	}
	else if (scrollx < 0) {
		radius+=originCameraDistance;
	}
	//Zoom in-out
	cameraPos.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch)) * radius + originPos.x;
	cameraPos.y = sin(glm::radians(pitch)) * -radius + originPos.y;
	cameraPos.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch)) * radius + originPos.z;

	callbk = preapareCallbackData();
	return callbk;
}
//+X yaw 0
//-X yaw 180

//-Z yaw -90
//+Z yaw 90


CallbckData Callback::mouse_callback(GLFWwindow* window, double xpos, double ypos, PanelData &panelData, float maskPanelSliderValue,bool brushMaskPanelMaskHover,
LigidCursors cursors,bool texturePanelButtonHover,std::vector<UIElement> &uiElements,float mainPanelLoc,ColorPicker &colorPicker,TextureDisplayer &textureDisplayer
, NodePanel &nodePanel, ContextMenu &addNodeContextMenu, SndPanel &sndPanel,ColoringPanel &coloringPanel,bool moveCamera,bool zoomInOutCamera)
{
	CallbckData callbk;
	
	//Get window size
	int screenSizeX;
	int screenSizeY;
	glfwGetFramebufferSize(window, &screenSizeX, &screenSizeY);
	
	buttonCheck(window, xpos, ypos, panelData,maskPanelSliderValue,brushMaskPanelMaskHover,cursors, texturePanelButtonHover,uiElements,mainPanelLoc,
	colorPicker,textureDisplayer,nodePanel,addNodeContextMenu,sndPanel,coloringPanel);	

	xoffset = xpos - lastX;
	yoffset = lastY - ypos;
	lastX = xpos;
	lastY = ypos;
	if ((glfwGetMouseButton(window, 1) == GLFW_PRESS || glfwGetMouseButton(window, 0) == GLFW_PRESS || moveCamera) && glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS && !zoomInOutCamera && glfwGetKey(window, GLFW_KEY_Z) == GLFW_RELEASE && glfwGetKey(window, GLFW_KEY_F3) == GLFW_RELEASE) {
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
	else if ((glfwGetMouseButton(window, 1) == GLFW_PRESS || moveCamera) && !zoomInOutCamera && glfwGetKey(window, GLFW_KEY_Z) == GLFW_RELEASE && glfwGetKey(window, GLFW_KEY_F3) == GLFW_RELEASE) {

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
	
	if (glfwGetMouseButton(window, 2) == GLFW_PRESS && nodePanel.panelHover){
		nodePanel.panelPositionX += xoffset/200.f;
		nodePanel.panelPositionY += yoffset/200.f;
	}
	std::cout << "yaw : " << yaw << " campos : "<< cameraPos.x << " , " << cameraPos.z << '\n';
	callbk = preapareCallbackData();
	return callbk;
}
bool panelChangeHover = false;
bool panelClickTaken = false;
bool noPanelClick = true;

void Callback::buttonCheck(GLFWwindow* window, double mouseXPos,double mouseYPos,PanelData &panelData,float maskPanelSliderValue,bool brushMaskPanelMaskHover,
LigidCursors cursors,bool texturePanelButtonHover,std::vector<UIElement> &uiElements,float mainPanelLoc,ColorPicker &colorPicker, TextureDisplayer &textureDisplayer
,NodePanel &nodePanel,ContextMenu &addNodeContextMenu,SndPanel &sndPanel,ColoringPanel &coloringPanel) {
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
	glfwGetFramebufferSize(window,&screenSizeX,&screenSizeY);

	float screenGapX = ((float)glfwGetVideoMode(glfwGetPrimaryMonitor())->width - screenSizeX)/(((float)glfwGetVideoMode(glfwGetPrimaryMonitor())->width)/2.0f); 

	if(!uiElements[UIfocusModeCheckBox].checkBox.checked){
		if(!colorPicker.dropperActive){
				for (size_t i = 0; i < uiElements.size(); i++)
				{
					uiElementEnter = false;

					std::string currentType = uiElements[i].type; 

					bool panelCompatibility;
					if(uiElements[i].panel == 1 && panelData.modelPanelActive || uiElements[i].panel == 2 && panelData.texturePanelActive || uiElements[i].panel == 3 && panelData.paintingPanelActive || uiElements[i].panel == 4 && panelData.exportPanelActive || uiElements[i].panel == 5 && panelData.settingsPanelActive || uiElements[i].panel == 6 && panelData.generatorPanelActive ||uiElements[i].panel == 0){
						panelCompatibility = true;
					}
					else{
						panelCompatibility = false;
					}
					float centerCoords = (mainPanelLoc + std::max(mainPanelLoc - 1.7f,0.0f)) / centerDivider + centerSum;

					if(!uiElements[i].attachedToMainPanel || uiElements[i].focusMode){
						centerCoords =  mainPanelLoc - 1.0f;
					}

					if(panelCompatibility){

						float slideVal = 0;
						if(panelData.paintingPanelActive && !uiElements[i].focusMode)
							slideVal = panelData.paintingPanelSlideVal;
						if(panelData.settingsPanelActive && !uiElements[i].focusMode)
							slideVal = panelData.settingsPanelSlideVal;

					if(currentType == "button"){
						uiElementEnter = ui.isMouseOnButton(window, uiElements[i].button.width + 0.02f, uiElements[i].button.height, centerCoords - screenGapX + uiElements[i].button.positionX, uiElements[i].button.positionY + slideVal, mouseXPos, mouseYPos, movePanel);
						uiElements[i].button.hover = uiElementEnter;
					}

					if(currentType == "text"){	

					}

					if(currentType == "rangeBar"){
						uiElementEnter = ui.isMouseOnButton(window, 0.02f, 0.02f, centerCoords - screenGapX + uiElements[i].rangeBar.positionX + (uiElements[i].rangeBar.value/uiElements[i].rangeBar.widthDivider)*(1.f - (float)uiElements[i].rangeBar.isConstant), uiElements[i].rangeBar.positionY + slideVal, mouseXPos, mouseYPos, movePanel);
						uiElements[i].rangeBar.hover = uiElementEnter;
					}

					if(currentType == "textBox"){
						uiElementEnter = ui.isMouseOnButton(window, uiElements[i].textBox.width, uiElements[i].textBox.height, centerCoords - screenGapX + uiElements[i].textBox.position_x, uiElements[i].textBox.position_y + slideVal, mouseXPos, mouseYPos, movePanel);
						uiElements[i].textBox.hover = uiElementEnter;
					}

					if(currentType == "checkBox"){
						uiElementEnter = ui.isMouseOnButton(window, 0.02f, 0.02f, centerCoords - screenGapX + uiElements[i].checkBox.positionX, uiElements[i].checkBox.positionY + slideVal, mouseXPos, mouseYPos, movePanel);
						uiElements[i].checkBox.mouseHover = uiElementEnter;
					}
						if(currentType == "icon"){
							if(uiElements[i].icon.clickable){
								uiElementEnter = ui.isMouseOnButton(window, uiElements[i].icon.width/1.5, uiElements[i].icon.height/1.5, centerCoords - screenGapX + uiElements[i].icon.positionX, uiElements[i].icon.positionY + slideVal, mouseXPos, mouseYPos, movePanel);
								uiElements[i].icon.hover = uiElementEnter;
							}
						}

					}
					if(uiElementEnter){
						break;
					}
				}

			if(panelData.paintingPanelActive){
				colorPicker.saturationValuePointerHover = ui.isMouseOnButton(window,0.015f, 0.03f, mainPanelLoc / centerDivider + centerSum - screenGapX - 0.02f + colorPicker.saturationValuePosX, -0.55f + colorPicker.saturationValuePosY + panelData.paintingPanelSlideVal, mouseXPos, mouseYPos, movePanel);
				colorPicker.hueValuePointerHover = ui.isMouseOnButton(window, 0.01f, 0.01f, mainPanelLoc / centerDivider + centerSum - screenGapX + 0.1f, -0.55f + colorPicker.hueValue+ panelData.paintingPanelSlideVal, mouseXPos, mouseYPos, movePanel);
					colorPicker.hexValTextBoxEnter =  ui.isMouseOnButton(window, 0.04f, 0.03f, mainPanelLoc / centerDivider + centerSum - screenGapX - 0.008f,-0.81f+ panelData.paintingPanelSlideVal, mouseXPos, mouseYPos, movePanel);
					colorPicker.dropperEnter = ui.isMouseOnButton(window, 0.015f,0.03f, mainPanelLoc / centerDivider + centerSum - screenGapX + 0.08f, -0.81f+ panelData.paintingPanelSlideVal, mouseXPos, mouseYPos,movePanel);

					colorPicker.saturationValueBoxHover = ui.isMouseOnButton(window, 0.1f, 0.2f, mainPanelLoc / centerDivider + centerSum - screenGapX - 0.02f, -0.55f+ panelData.paintingPanelSlideVal, mouseXPos, mouseYPos, movePanel);		
					colorPicker.hueValueBarHover = ui.isMouseOnButton(window, 0.01f, 0.18f, mainPanelLoc / centerDivider + centerSum - screenGapX + 0.1f, -0.55f+ panelData.paintingPanelSlideVal, mouseXPos, mouseYPos, movePanel);

					maskPanelEnter = ui.isMouseOnButton(window, 0.15f, 0.15f, mainPanelLoc / centerDivider + centerSum - screenGapX, 0.675f+ panelData.paintingPanelSlideVal, mouseXPos, mouseYPos, movePanel);
				}
				if(!panelData.paintingPanelActive){
					nodePanel.panelHover = ui.isMouseOnNodePanel(window,mainPanelLoc - screenGapX - 1.f, nodePanel.heigth,mouseXPos,mouseYPos,false,sndPanel.position);
					nodePanel.boundariesHover = ui.isMouseOnNodePanel(window,mainPanelLoc - screenGapX - 1.f, nodePanel.heigth,mouseXPos,mouseYPos,true,sndPanel.position);
				}

			//Add node context menu
			addNodeContextMenu.hover = ui.isMouseOnButton(window, addNodeContextMenu.width+0.02f, addNodeContextMenu.height+0.04f, addNodeContextMenu.positionX - screenGapX/2.f, addNodeContextMenu.positionY- addNodeContextMenu.height, mouseXPos, mouseYPos, false);

			//Snd Panel
			if(sndPanel.state == 0){
				if(sndPanel.activeFolderIndex != 10000)
					sndPanel.backSignHover = ui.isMouseOnButton(window, 0.01f,0.02f, sndPanel.position - 0.365f, 0.85f, mouseXPos, mouseYPos, false);
				else
						sndPanel.backSignHover = false;

					sndPanel.downSignHover = ui.isMouseOnButton(window, 0.01f,0.02f, sndPanel.position - 0.05f, 0.85f, mouseXPos, mouseYPos, false);
					sndPanel.folderSignHover = ui.isMouseOnButton(window, 0.01f,0.02f, sndPanel.position - 0.17f, 0.85f, mouseXPos, mouseYPos, false);
				}
				else{
					sndPanel.folderSignHover = ui.isMouseOnButton(window, 0.01f,0.02f, sndPanel.position - 0.17f, 0.85f, mouseXPos, mouseYPos, false);
					sndPanel.downSignHover = ui.isMouseOnButton(window, 0.01f,0.02f, sndPanel.position - 0.05f, 0.85f, mouseXPos, mouseYPos, false);
				}
				if(!coloringPanel.active){
					sndPanel.plusSignHover = ui.isMouseOnButton(window, 0.01f,0.02f, sndPanel.position - 0.08f, 0.85f, mouseXPos, mouseYPos, false);
					sndPanel.minusSignHover = ui.isMouseOnButton(window, 0.01f,0.02f, sndPanel.position - 0.11f, 0.85f, mouseXPos, mouseYPos, false);
					sndPanel.duplicateSignHover = ui.isMouseOnButton(window, 0.01f,0.02f, sndPanel.position - 0.14f, 0.85f, mouseXPos, mouseYPos, false);
					sndPanel.texturePanelButtonHover = ui.isMouseOnButton(window, 0.017f,0.034f, sndPanel.position + 0.015f, 0.8f, mouseXPos, mouseYPos, false);
					sndPanel.materialPanelButtonHover = ui.isMouseOnButton(window, 0.017f,0.034f, sndPanel.position + 0.015f, 0.72f, mouseXPos, mouseYPos, false);
				}

				sndPanel.boundariesHover = ui.isMouseOnButton(window, 0.02f,0.88,sndPanel.position,0.0f, mouseXPos, mouseYPos, 0);

				sndPanel.panelHover = ui.isMouseOnButton(window, 0.2f, 0.88f, sndPanel.position - 0.2f,0.0f, mouseXPos, mouseYPos, 0);


				mainPanelBoundariesEnter =  ui.isMouseOnButton(window, 0.02f, 0.88f, mainPanelLoc -1.0f + 0.02f - screenGapX, 0.0f, mouseXPos, mouseYPos, false);

				mainPanelEnter = ui.isMouseOnButton(window, 0.2f,0.9f,mainPanelLoc - 1.0f - screenGapX + 0.2f, 0.0f, mouseXPos, mouseYPos, 0);

				textureDisplayer.buttonHover = ui.isMouseOnButton(window, 0.02f,0.045f,(textureDisplayer.buttonPosX+0.005f)-1.0f,textureDisplayer.buttonPosY-0.01f, mouseXPos, mouseYPos, 0);		


			modelPanelButtonEnter = ui.isMouseOnButton(window, 0.02f,0.034f,mainPanelLoc - screenGapX , 0.8f, mouseXPos, mouseYPos,true);
			texturePanelButtonEnter = ui.isMouseOnButton(window, 0.02f,0.034f,mainPanelLoc - screenGapX , 0.72f, mouseXPos, mouseYPos,true);
			paintingPanelButtonEnter = ui.isMouseOnButton(window, 0.02f,0.034f,mainPanelLoc - screenGapX , 0.64f, mouseXPos, mouseYPos,true);
			exportPanelButtonEnter = ui.isMouseOnButton(window, 0.02f,0.034f,mainPanelLoc - screenGapX , 0.56f, mouseXPos, mouseYPos,true);
			settingsPanelButtonEnter = ui.isMouseOnButton(window, 0.02f,0.034f,mainPanelLoc - screenGapX , 0.48f, mouseXPos, mouseYPos,true);
			generatorPanelButtonEnter = ui.isMouseOnButton(window, 0.02f,0.034f,mainPanelLoc - screenGapX , 0.4f, mouseXPos, mouseYPos,true);
		}
	}
	else{
		for (size_t i = 0; i < uiElements.size(); i++)
		{
			uiElementEnter = false;
			std::string currentType = uiElements[i].type; 

			float centerCoords = (mainPanelLoc + std::max(mainPanelLoc - 1.7f,0.0f)) / centerDivider + centerSum;
			if(!uiElements[i].attachedToMainPanel || uiElements[i].focusMode){
				centerCoords =  mainPanelLoc - 1.0f;
			}
			if(uiElements[i].focusMode){
				float slideVal = 0;
				if(panelData.paintingPanelActive && !uiElements[i].focusMode)
					slideVal = panelData.paintingPanelSlideVal;
			if(currentType == "button"){
				uiElementEnter = ui.isMouseOnButton(window, uiElements[i].button.width + 0.02f, uiElements[i].button.height, centerCoords - screenGapX + uiElements[i].button.positionX, uiElements[i].button.positionY + slideVal, mouseXPos, mouseYPos, movePanel);
				uiElements[i].button.hover = uiElementEnter;
			}
			if(currentType == "text"){	
			}
			if(currentType == "rangeBar"){
				uiElementEnter = ui.isMouseOnButton(window, 0.02f, 0.02f, centerCoords - screenGapX + uiElements[i].rangeBar.positionX + uiElements[i].rangeBar.value/uiElements[i].rangeBar.widthDivider, uiElements[i].rangeBar.positionY + slideVal, mouseXPos, mouseYPos, movePanel);
				uiElements[i].rangeBar.hover = uiElementEnter;
			}
			if(currentType == "textBox"){
				uiElementEnter = ui.isMouseOnButton(window, uiElements[i].textBox.width, uiElements[i].textBox.height, centerCoords - screenGapX + uiElements[i].textBox.position_x, uiElements[i].textBox.position_y + slideVal, mouseXPos, mouseYPos, movePanel);
				uiElements[i].textBox.hover = uiElementEnter;
			}
			if(currentType == "checkBox"){
				uiElementEnter = ui.isMouseOnButton(window, 0.02f, 0.02f, centerCoords - screenGapX + uiElements[i].checkBox.positionX, uiElements[i].checkBox.positionY + slideVal, mouseXPos, mouseYPos, movePanel);
				uiElements[i].checkBox.mouseHover = uiElementEnter;
			}
				if(currentType == "icon"){
					if(uiElements[i].icon.clickable){
						uiElementEnter = ui.isMouseOnButton(window, uiElements[i].icon.width/1.5, uiElements[i].icon.height/1.5, centerCoords - screenGapX + uiElements[i].icon.positionX, uiElements[i].icon.positionY + slideVal, mouseXPos, mouseYPos, movePanel);
						uiElements[i].icon.hover = uiElementEnter;
					}
				}
			}
			if(uiElementEnter){
				break;
			}
		}
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
	else if(sndPanel.texturePanelButtonHover || sndPanel.materialPanelButtonHover){
		glfwSetCursor(window, cursors.pointerCursor);
	}
	else if(colorPicker.saturationValuePointerHover || colorPicker.hueValuePointerHover || colorPicker.hexValTextBoxEnter){
		glfwSetCursor(window, cursors.pointerCursor);
	}
	else if(sndPanel.plusSignHover || sndPanel.downSignHover || sndPanel.minusSignHover || sndPanel.folderSignHover || sndPanel.backSignHover || sndPanel.duplicateSignHover){
		glfwSetCursor(window, cursors.pointerCursor);
	}
	else if(modelPanelButtonEnter || texturePanelButtonEnter || paintingPanelButtonEnter || exportPanelButtonEnter || settingsPanelButtonEnter || generatorPanelButtonEnter){
		glfwSetCursor(window, cursors.pointerCursor);
	}
	else if(nodePanel.pointerCursor){
		glfwSetCursor(window, cursors.pointerCursor);
	}
	else if (!panelChangeHover){
		glfwSetCursor(window, cursors.defaultCursor);
	}
	
}
void Callback::framebuffer_size_callback(GLFWwindow* window, int width, int height) //Protect screen ratio
{
	// glfwSetWindowSize(window, width, height);
	LigidPainter lp;
	lp.setViewportToDefault();

	screenX = width;
	screenY = height;
}