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
float panelLoc = 1.6f;
float panelOffset;
bool movePanel;
//Panel

//Ui enter
bool loadModelButtonEnter;
bool modelFilePathTextBoxEnter;
bool autoTriangulateCheckBoxEnter;
bool backfaceCullingCheckBoxEnter;
bool addPlaneButtonEnter;
bool addSphereButtonEnter;
bool addMaskTextureButtonEnter;
bool brushSizeRangeBarEnter;
bool brushBlurRangeBarEnter;
bool brushRotationRangeBarEnter;
bool brushOpacityRangeBarEnter;
bool brushSpacingRangeBarEnter;
bool brushBordersRangeBarEnter;
bool colorBoxColorRangeBarEnter;
bool colorBoxPickerEnter;
bool modelPanelButtonEnter;
bool texturePanelButtonEnter;
bool exportPanelButtonEnter;
bool paintingPanelButtonEnter;
bool exportPathTextBoxEnter;
bool exportFileNameTextBoxEnter;
bool exportExtJPGCheckBoxEnter;
bool exportExtPNGCheckBoxEnter;
bool mirrorXCheckBoxEnter;
bool mirrorYCheckBoxEnter;
bool mirrorZCheckBoxEnter;
bool exportDownloadButtonEnter;
bool textureDemonstratorButtonEnter;
bool useNegativeForDrawingCheckboxEnter;
bool paintingDropperEnter;
bool colorBoxEnter;
bool maskPanelSliderEnter;
bool maskPanelEnter;
bool hexValueTextboxEnter;
bool loadCustomModelEnter;
bool hueBarEnter;


bool uiElementEnter;
//Ui enter


bool panelChangeLoc = false;

int screenX;
int screenY;

CallbckData preapareCallbackData() {
	CallbckData callbk;

	callbk.loadModelButtonEnter = loadModelButtonEnter;
	callbk.modelFilePathTextBoxEnter = modelFilePathTextBoxEnter;
	callbk.autoTriangulateCheckBoxEnter = autoTriangulateCheckBoxEnter;
	callbk.backfaceCullingCheckBoxEnter = backfaceCullingCheckBoxEnter;
	callbk.addPlaneButtonEnter = addPlaneButtonEnter;
	callbk.addSphereButtonEnter = addSphereButtonEnter;
	callbk.movePanel = movePanel;

	callbk.modelPanelButtonEnter = modelPanelButtonEnter;
	callbk.texturePanelButtonEnter = texturePanelButtonEnter;
	callbk.paintingPanelButtonEnter = paintingPanelButtonEnter;
	callbk.exportPanelButtonEnter = exportPanelButtonEnter;

	callbk.addMaskTextureButtonEnter = addMaskTextureButtonEnter;
	callbk.brushSizeRangeBarEnter = brushSizeRangeBarEnter;
	callbk.brushBlurRangeBarEnter = brushBlurRangeBarEnter;
	callbk.brushRotationRangeBarEnter = brushRotationRangeBarEnter;
	callbk.brushOpacityRangeBarEnter = brushOpacityRangeBarEnter;
	callbk.brushSpacingRangeBarEnter = brushSpacingRangeBarEnter;
	callbk.brushBordersRangeBarEnter = brushBordersRangeBarEnter;
	callbk.colorBoxColorRangeBarEnter = colorBoxColorRangeBarEnter;
	callbk.colorBoxPickerEnter = colorBoxPickerEnter;

	callbk.exportPathTextBoxEnter = exportPathTextBoxEnter;
	callbk.exportFileNameTextBoxEnter = exportFileNameTextBoxEnter;
	callbk.exportExtJPGCheckBoxEnter = exportExtJPGCheckBoxEnter;
	callbk.exportExtPNGCheckBoxEnter = exportExtPNGCheckBoxEnter;
	callbk.exportDownloadButtonEnter = exportDownloadButtonEnter;

	callbk.mirrorXCheckBoxEnter = mirrorXCheckBoxEnter;
	callbk.mirrorYCheckBoxEnter = mirrorYCheckBoxEnter;
	callbk.mirrorZCheckBoxEnter = mirrorZCheckBoxEnter;

	callbk.cameraPos = cameraPos;
	callbk.originPos = originPos;
	callbk.panelLoc = panelLoc;
	callbk.panelChangeLoc = panelChangeLoc; 

	callbk.textureDemonstratorButtonEnter = textureDemonstratorButtonEnter;

	callbk.useNegativeForDrawingCheckboxEnter = useNegativeForDrawingCheckboxEnter;

	callbk.paintingDropperEnter = paintingDropperEnter;


	callbk.colorBoxEnter = colorBoxEnter; 

	callbk.maskPanelSliderEnter = maskPanelSliderEnter;
	callbk.maskPanelEnter = maskPanelEnter;

	callbk.hexValueTextboxEnter = hexValueTextboxEnter;

	callbk.loadCustomModelEnter = loadCustomModelEnter;

	callbk.hueBarEnter = hueBarEnter;

	callbk.uiElementEnter = uiElementEnter;

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

CallbckData Callback::mouse_callback(GLFWwindow* window, double xpos, double ypos, PanelData panelData,float brushSizeRangeBarValue,float colorBoxPickerValue_x, 
float colorBoxPickerValue_y,float colorBoxColorRangeBarValue, float brushBlurRangeBarValue, bool enablePanelMovement, float brushRotationRangeBarValue, 
float brushOpacityRangeBarValue, float brushSpacingRangeBarValue, float textureDemonstratorButtonPosX,float textureDemonstratorButtonPosY,float maskPanelSliderValue,
bool brushMaskPanelMaskHover,LigidCursors cursors,bool paintingDropperPressed,float brushBorderRangeBarValue,bool texturePanelButtonHover,std::vector<UIElement> &uiElements)
{
	CallbckData callbk;
	
	//Get window size
	int screenSizeX;
	int screenSizeY;
	glfwGetWindowSize(window, &screenSizeX, &screenSizeY);

	panelCheck(window,xpos,screenSizeX,enablePanelMovement);
	
	buttonCheck(window, xpos, ypos, panelData ,brushSizeRangeBarValue, colorBoxPickerValue_x, colorBoxPickerValue_y, colorBoxColorRangeBarValue, brushBlurRangeBarValue, brushRotationRangeBarValue, brushOpacityRangeBarValue, brushSpacingRangeBarValue,textureDemonstratorButtonPosX,textureDemonstratorButtonPosY,maskPanelSliderValue,brushMaskPanelMaskHover,cursors,paintingDropperPressed,brushBorderRangeBarValue, texturePanelButtonHover,uiElements);
	

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

void Callback::panelCheck(GLFWwindow* window, double mouseXpos, int screenSizeX, bool enablePanelMovement) {
	panelOffset = mouseXpos - holdXPos;
	holdXPos = mouseXpos;
	float screenGapX = (callbackMaxScreenWidth - screenSizeX); 

	if (enablePanelMovement) {
		if (mouseXpos > (callbackMaxScreenWidth / 2 * panelLoc) - 10 - screenGapX && mouseXpos < (callbackMaxScreenWidth / 2 * panelLoc) + 10 - screenGapX) {
			panelChangeHover = true;
			if (glfwGetMouseButton(window, 0) == GLFW_PRESS) {
				panelChangeLoc = true;
			}
		}
		else{
			panelChangeHover = false;
		}


		if (glfwGetMouseButton(window, 0) == GLFW_PRESS && !panelClickTaken) {
	 		if (panelChangeHover) {
	 			noPanelClick = true;
	 		}
	 		else {
	 			noPanelClick = false;
	 		}
	 		panelClickTaken = true;
	    }
	    if (glfwGetMouseButton(window, 0) == GLFW_RELEASE && panelClickTaken) {
	     	panelClickTaken = false;
	    }
		if (panelChangeLoc && noPanelClick) {
			panelLoc += panelOffset / 1000;
			if (panelLoc > 1.7f) {
				movePanel = true;
			}
			else {
				movePanel = false;
			}
			if (panelLoc < 1.6f)
				panelLoc = 1.6f;
			if (panelLoc > 1.987f)
				panelLoc = 1.987f;
		}
		if (glfwGetMouseButton(window, 0) == GLFW_RELEASE) {
			panelChangeLoc = false;
		}
	}
}
void Callback::buttonCheck(GLFWwindow* window, double mouseXPos,double mouseYPos,PanelData panelData, float brushSizeRangeBarValue, float colorBoxPickerValue_x, float colorBoxPickerValue_y,float colorBoxColorRangeBarValue,float brushBlurRangeBarValue,float brushRotationRangeBarValue, float brushOpacityRangeBarValue, float brushSpacingRangeBarValue,float textureDemonstratorButtonPosX,float textureDemonstratorButtonPosY,float maskPanelSliderValue,bool brushMaskPanelMaskHover,LigidCursors cursors,bool paintingDropperPressed,float brushBorderRangeBarValue,bool texturePanelButtonHover,std::vector<UIElement> &uiElements) {
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


	if(!paintingDropperPressed){
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
			float centerCoords = (panelLoc + std::max(panelLoc - 1.7f,0.0f)) / centerDivider + centerSum;
		
			if(uiElements[i].attachedToMainPanel == false){
				centerCoords =  panelLoc - 1.0f;
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
			colorBoxPickerEnter = ui.isMouseOnButton(window,0.015f, 0.03f, panelLoc / centerDivider + centerSum - screenGapX - 0.02f + colorBoxPickerValue_x, -0.55f + colorBoxPickerValue_y, mouseXPos, mouseYPos, movePanel);
			colorBoxColorRangeBarEnter = ui.isMouseOnButton(window, 0.01f, 0.01f, panelLoc / centerDivider + centerSum - screenGapX + 0.1f, -0.55f + colorBoxColorRangeBarValue, mouseXPos, mouseYPos, movePanel);
			hexValueTextboxEnter =  ui.isMouseOnButton(window, 0.04f, 0.03f, panelLoc / centerDivider + centerSum - screenGapX - 0.008f,-0.81f, mouseXPos, mouseYPos, movePanel);
			paintingDropperEnter = ui.isMouseOnButton(window, 0.015f,0.03f, panelLoc / centerDivider + centerSum - screenGapX + 0.08f, -0.81f, mouseXPos, mouseYPos,movePanel);
			
			colorBoxEnter = ui.isMouseOnButton(window, 0.1f, 0.2f, panelLoc / centerDivider + centerSum - screenGapX - 0.02f, -0.55f, mouseXPos, mouseYPos, movePanel);		
			hueBarEnter = ui.isMouseOnButton(window, 0.01f, 0.18f, panelLoc / centerDivider + centerSum - screenGapX + 0.1f, -0.55f, mouseXPos, mouseYPos, movePanel);
		
			maskPanelEnter = ui.isMouseOnButton(window, 0.15f, 0.15f, panelLoc / centerDivider + centerSum - screenGapX, 0.675f, mouseXPos, mouseYPos, movePanel);
		}


		textureDemonstratorButtonEnter = ui.isMouseOnButton(window, 0.02f,0.045f,(textureDemonstratorButtonPosX+0.005f)-1.0f,textureDemonstratorButtonPosY-0.01f, mouseXPos, mouseYPos, 0);	modelPanelButtonEnter = ui.isMouseOnPanelChangeButton(window, panelLoc- screenGapX, 0.8f, mouseXPos, mouseYPos);
		modelPanelButtonEnter = ui.isMouseOnPanelChangeButton(window, panelLoc- screenGapX, 0.8f, mouseXPos, mouseYPos);
		texturePanelButtonEnter = ui.isMouseOnPanelChangeButton(window, panelLoc- screenGapX, 0.72f, mouseXPos, mouseYPos);
		paintingPanelButtonEnter = ui.isMouseOnPanelChangeButton(window, panelLoc- screenGapX, 0.64f, mouseXPos, mouseYPos);
		exportPanelButtonEnter = ui.isMouseOnPanelChangeButton(window, panelLoc- screenGapX, 0.56f, mouseXPos, mouseYPos);
	}

	if(paintingDropperPressed){
		glfwSetCursor(window, cursors.dropperCursor);
	}
	else if(uiElementEnter){
		glfwSetCursor(window, cursors.pointerCursor);
	}
	else if(paintingDropperEnter){
		glfwSetCursor(window, cursors.pointerCursor);
	}
	else if(textureDemonstratorButtonEnter){
		glfwSetCursor(window, cursors.pointerCursor);
	}
	else if(colorBoxPickerEnter || colorBoxColorRangeBarEnter || hexValueTextboxEnter){
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