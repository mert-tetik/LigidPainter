#include<iostream>
#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include <string>
#include <fstream>
#include <sstream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include "LigidPainter.h"
#include "UserInterface.h"
#include "Callback.h"
#include "gl.h"
#include "Utilities.h"
#include <vector>
#include "stb_image.h"
#include "stb_image_write.h"
#include "UiActions.h"

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
bool addImageButtonEnter;
bool addMaskTextureButtonEnter;
bool brushSizeRangeBarEnter;
bool brushBlurRangeBarEnter;
bool brushRotationRangeBarEnter;
bool brushOpacityRangeBarEnter;
bool brushSpacingRangeBarEnter;
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
bool paintingFillNumericModifierPEnter;
bool paintingFillNumericModifierNEnter;
bool colorBoxEnter;
bool maskPanelSliderEnter;
bool maskPanelEnter;
bool hexValueTextboxEnter;
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
	callbk.addImageButtonEnter = addImageButtonEnter;
	callbk.modelPanelButtonEnter = modelPanelButtonEnter;
	callbk.texturePanelButtonEnter = texturePanelButtonEnter;
	callbk.paintingPanelButtonEnter = paintingPanelButtonEnter;
	callbk.addMaskTextureButtonEnter = addMaskTextureButtonEnter;
	callbk.brushSizeRangeBarEnter = brushSizeRangeBarEnter;
	callbk.brushBlurRangeBarEnter = brushBlurRangeBarEnter;
	callbk.brushRotationRangeBarEnter = brushRotationRangeBarEnter;
	callbk.brushOpacityRangeBarEnter = brushOpacityRangeBarEnter;
	callbk.brushSpacingRangeBarEnter = brushSpacingRangeBarEnter;
	callbk.colorBoxColorRangeBarEnter = colorBoxColorRangeBarEnter;
	callbk.colorBoxPickerEnter = colorBoxPickerEnter;
	callbk.exportPanelButtonEnter = exportPanelButtonEnter;

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

	callbk.paintingFillNumericModifierPEnter = paintingFillNumericModifierPEnter;
	callbk.paintingFillNumericModifierNEnter = paintingFillNumericModifierNEnter;

	callbk.colorBoxEnter = colorBoxEnter; 

	callbk.maskPanelSliderEnter = maskPanelSliderEnter;
	callbk.maskPanelEnter = maskPanelEnter;

	callbk.hexValueTextboxEnter = hexValueTextboxEnter;

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
CallbckData Callback::mouse_callback(GLFWwindow* window, double xpos, double ypos, PanelData panelData,float brushSizeRangeBarValue,float colorBoxPickerValue_x, float colorBoxPickerValue_y,float colorBoxColorRangeBarValue, float brushBlurRangeBarValue, bool enablePanelMovement, float brushRotationRangeBarValue, float brushOpacityRangeBarValue, float brushSpacingRangeBarValue, float textureDemonstratorButtonPosX,float textureDemonstratorButtonPosY,float maskPanelSliderValue,bool brushMaskPanelMaskHover,GLFWcursor* pointerCursor,GLFWcursor* defaultCursor)
{
	CallbckData callbk;
	
	//Get window size
	int screenSizeX;
	int screenSizeY;
	glfwGetWindowSize(window, &screenSizeX, &screenSizeY);

	panelCheck(window,xpos,screenSizeX,enablePanelMovement);
	
	buttonCheck(window, xpos, ypos, panelData ,brushSizeRangeBarValue, colorBoxPickerValue_x, colorBoxPickerValue_y, colorBoxColorRangeBarValue, brushBlurRangeBarValue, brushRotationRangeBarValue, brushOpacityRangeBarValue, brushSpacingRangeBarValue,textureDemonstratorButtonPosX,textureDemonstratorButtonPosY,maskPanelSliderValue,brushMaskPanelMaskHover,pointerCursor,defaultCursor);
	

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
void Callback::panelCheck(GLFWwindow* window, int mouseXpos, int screenSizeX, bool enablePanelMovement) {
	panelOffset = mouseXpos - holdXPos;
	holdXPos = mouseXpos;
	float screenGapX = (1920.0 - screenSizeX); 

	if (enablePanelMovement) {
		if (mouseXpos > (1920 / 2 * panelLoc) - 10 - screenGapX && mouseXpos < (1920 / 2 * panelLoc) + 10 - screenGapX) {
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
void Callback::buttonCheck(GLFWwindow* window, int mouseXPos,int mouseYPos,PanelData panelData, float brushSizeRangeBarValue, float colorBoxPickerValue_x, float colorBoxPickerValue_y,float colorBoxColorRangeBarValue,float brushBlurRangeBarValue,float brushRotationRangeBarValue, float brushOpacityRangeBarValue, float brushSpacingRangeBarValue,float textureDemonstratorButtonPosX,float textureDemonstratorButtonPosY,float maskPanelSliderValue,bool brushMaskPanelMaskHover,GLFWcursor* pointerCursor,GLFWcursor* defaultCursor) {
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



	//GLFWcursor* pointerCursor = glfwCreateStandardCursor(GLFW_HAND_CURSOR);
	if (panelData.modelPanelActive) {
		loadModelButtonEnter = ui.isMouseOnButton(window, 0.1f, 0.04f, panelLoc / centerDivider + centerSum - screenGapX, 0.4f, mouseXPos, mouseYPos,movePanel);
		modelFilePathTextBoxEnter = ui.isMouseOnButton(window, 0.12f, 0.03f, panelLoc / centerDivider + centerSum - screenGapX, 0.6f, mouseXPos, mouseYPos, movePanel);
		autoTriangulateCheckBoxEnter = ui.isMouseOnButton(window, 0.012f, 0.02f, panelLoc / centerDivider + centerSum - screenGapX - 0.08f, 0.3f, mouseXPos, mouseYPos, movePanel);
		backfaceCullingCheckBoxEnter = ui.isMouseOnButton(window, 0.012f, 0.02f, panelLoc / centerDivider + centerSum - screenGapX - 0.08f, 0.2f, mouseXPos, mouseYPos, movePanel);
		addPlaneButtonEnter = ui.isMouseOnButton(window, 0.03f, 0.04f, panelLoc / centerDivider + centerSum - screenGapX, 0.0f, mouseXPos, mouseYPos, movePanel);
		addSphereButtonEnter = ui.isMouseOnButton(window, 0.03f, 0.04f, panelLoc / centerDivider + centerSum - screenGapX, -0.1f, mouseXPos, mouseYPos, movePanel);
	}
	else {
		loadModelButtonEnter = false;
		modelFilePathTextBoxEnter = false;
		autoTriangulateCheckBoxEnter = false;
		backfaceCullingCheckBoxEnter = false;
		addPlaneButtonEnter = false;
		addSphereButtonEnter = false;
	}
	if (panelData.texturePanelActive) {
		addImageButtonEnter = ui.isMouseOnButton(window, 0.12f, 0.04f, panelLoc / centerDivider + centerSum - screenGapX, 0.8f, mouseXPos, mouseYPos, movePanel);
	}
	else {
		addImageButtonEnter = false;
	}
	if (panelData.paintingPanelActive) {
		addMaskTextureButtonEnter = ui.isMouseOnButton(window, 0.12f, 0.04f, panelLoc / centerDivider + centerSum - screenGapX, 0.9f, mouseXPos, mouseYPos, movePanel);
		useNegativeForDrawingCheckboxEnter = ui.isMouseOnButton(window, 0.012f, 0.02f, panelLoc /+
		
		
		 centerDivider + centerSum - screenGapX - 0.03f, 0.45f, mouseXPos, mouseYPos, movePanel);
		brushSizeRangeBarEnter = ui.isMouseOnButton(window, 0.02f, 0.02f, panelLoc / centerDivider + centerSum - screenGapX+ brushSizeRangeBarValue,  0.22f+0.02f, mouseXPos, mouseYPos, movePanel);
		brushBlurRangeBarEnter = ui.isMouseOnButton(window, 0.02f, 0.02f, panelLoc / centerDivider + centerSum - screenGapX+ brushBlurRangeBarValue, 0.09f+0.04f, mouseXPos, mouseYPos, movePanel);
		brushRotationRangeBarEnter = ui.isMouseOnButton(window, 0.02f, 0.02f, panelLoc / centerDivider + centerSum - screenGapX+ brushRotationRangeBarValue, -0.04f+0.06f, mouseXPos, mouseYPos, movePanel);
		brushOpacityRangeBarEnter = ui.isMouseOnButton(window, 0.02f, 0.02f, panelLoc / centerDivider + centerSum - screenGapX+ brushOpacityRangeBarValue,  -0.17f+0.08f, mouseXPos, mouseYPos, movePanel);
		brushSpacingRangeBarEnter = ui.isMouseOnButton(window, 0.02f, 0.02f, panelLoc / centerDivider + centerSum - screenGapX+ brushSpacingRangeBarValue,-0.30f+0.1f, mouseXPos, mouseYPos, movePanel);

		paintingFillNumericModifierPEnter = ui.isMouseOnButton(window, 0.02f, 0.04f, panelLoc / centerDivider + centerSum - screenGapX + 0.05f,-0.3f, mouseXPos, mouseYPos, movePanel);
		paintingFillNumericModifierNEnter = ui.isMouseOnButton(window, 0.02f, 0.04f, panelLoc / centerDivider + centerSum - screenGapX - 0.05f,-0.3f, mouseXPos, mouseYPos, movePanel);

		colorBoxPickerEnter = ui.isMouseOnButton(window,0.01f, 0.02f, panelLoc / centerDivider + centerSum - screenGapX - 0.02f + colorBoxPickerValue_x, -0.6f + colorBoxPickerValue_y, mouseXPos, mouseYPos, movePanel);
		colorBoxColorRangeBarEnter = ui.isMouseOnButton(window, 0.01f, 0.01f, panelLoc / centerDivider + centerSum - screenGapX + 0.1f, -0.6f + colorBoxColorRangeBarValue, mouseXPos, mouseYPos, movePanel);

		paintingDropperEnter = ui.isMouseOnButton(window, 0.015f,0.03f, panelLoc / centerDivider + centerSum - screenGapX + 0.08f, -0.86f, mouseXPos, mouseYPos,movePanel);

		mirrorXCheckBoxEnter = ui.isMouseOnPanelChangeButton(window, panelLoc- 0.15f - screenGapX, 0.91f, mouseXPos, mouseYPos); //isMouseOnPanelChangeButton used for projection
		mirrorYCheckBoxEnter = ui.isMouseOnPanelChangeButton(window, panelLoc- 0.09f - screenGapX, 0.91f, mouseXPos, mouseYPos); //isMouseOnPanelChangeButton used for projection
		mirrorZCheckBoxEnter = ui.isMouseOnPanelChangeButton(window, panelLoc- 0.03f - screenGapX, 0.91f, mouseXPos, mouseYPos); //isMouseOnPanelChangeButton used for projection

		colorBoxEnter = ui.isMouseOnButton(window, 0.1f, 0.2f, panelLoc / centerDivider + centerSum - screenGapX - 0.02f, -0.6f, mouseXPos, mouseYPos, movePanel);

		maskPanelSliderEnter = ui.isMouseOnButton(window, 0.01f, 0.015f, panelLoc / centerDivider + centerSum - screenGapX + 0.13f, 0.8f + maskPanelSliderValue, mouseXPos, mouseYPos, movePanel);
		maskPanelEnter = ui.isMouseOnButton(window, 0.15f, 0.15f, panelLoc / centerDivider + centerSum - screenGapX, 0.675f, mouseXPos, mouseYPos, movePanel);

		hexValueTextboxEnter =  ui.isMouseOnButton(window, 0.04f, 0.03f, panelLoc / centerDivider + centerSum - screenGapX,-0.86f, mouseXPos, mouseYPos, movePanel);
	}
	else {
		addMaskTextureButtonEnter = false;
		useNegativeForDrawingCheckboxEnter = false;
		brushSizeRangeBarEnter = false;
		brushBlurRangeBarEnter = false;
		brushRotationRangeBarEnter = false;
		brushOpacityRangeBarEnter = false;
		brushSpacingRangeBarEnter = false;

		paintingFillNumericModifierPEnter = false;
		paintingFillNumericModifierNEnter = false;


		colorBoxColorRangeBarEnter = false;
		colorBoxPickerEnter = false;
		paintingDropperEnter = false;


		mirrorXCheckBoxEnter = false;
		mirrorYCheckBoxEnter = false;
		mirrorZCheckBoxEnter = false;

		colorBoxEnter = false;

		maskPanelSliderEnter = false;
		maskPanelEnter = false;

		hexValueTextboxEnter = false;
	}
	if (panelData.exportPanelActive) {
		exportPathTextBoxEnter = ui.isMouseOnButton(window, 0.12f, 0.03f, panelLoc / centerDivider + centerSum - screenGapX, 0.6f, mouseXPos, mouseYPos, movePanel);
		exportFileNameTextBoxEnter = ui.isMouseOnButton(window, 0.12f, 0.03f, panelLoc / centerDivider + centerSum - screenGapX, 0.5f, mouseXPos, mouseYPos, movePanel);
		exportExtJPGCheckBoxEnter = ui.isMouseOnButton(window, 0.014f, 0.02f, panelLoc / centerDivider + centerSum - screenGapX - 0.11f, 0.4f, mouseXPos, mouseYPos, movePanel);
		exportExtPNGCheckBoxEnter = ui.isMouseOnButton(window, 0.014f, 0.02f, panelLoc / centerDivider + centerSum - screenGapX + 0.05f, 0.4f + colorBoxPickerValue_y, mouseXPos, mouseYPos, movePanel);
		exportDownloadButtonEnter = ui.isMouseOnButton(window, 0.12f, 0.04f, panelLoc / centerDivider + centerSum - screenGapX, 0.2f, mouseXPos, mouseYPos, movePanel);
		
	}
	else {
		exportPathTextBoxEnter = false;
		exportFileNameTextBoxEnter = false;
		exportExtJPGCheckBoxEnter = false;
		exportExtPNGCheckBoxEnter = false;
		exportDownloadButtonEnter = false;
	}

	textureDemonstratorButtonEnter = ui.isMouseOnButton(window, 0.02f,0.045f,(textureDemonstratorButtonPosX+0.005f)-1.0f,textureDemonstratorButtonPosY-0.01f, mouseXPos, mouseYPos, 0);

	modelPanelButtonEnter = ui.isMouseOnPanelChangeButton(window, panelLoc- screenGapX, 0.8f, mouseXPos, mouseYPos);
	texturePanelButtonEnter = ui.isMouseOnPanelChangeButton(window, panelLoc- screenGapX, 0.72f, mouseXPos, mouseYPos);
	paintingPanelButtonEnter = ui.isMouseOnPanelChangeButton(window, panelLoc- screenGapX, 0.64f, mouseXPos, mouseYPos);
	exportPanelButtonEnter = ui.isMouseOnPanelChangeButton(window, panelLoc- screenGapX, 0.56f, mouseXPos, mouseYPos);

	if (modelFilePathTextBoxEnter) {
		glfwSetCursor(window, pointerCursor);
	}
	else if (loadModelButtonEnter) {
		glfwSetCursor(window, pointerCursor);
	}
	else if (autoTriangulateCheckBoxEnter) {
		glfwSetCursor(window, pointerCursor);
	}
	else if (backfaceCullingCheckBoxEnter) {
		glfwSetCursor(window, pointerCursor);
	}
	else if (addPlaneButtonEnter) {
		glfwSetCursor(window, pointerCursor);
	}
	else if (addSphereButtonEnter) {
		glfwSetCursor(window, pointerCursor);
	}
	else if (addImageButtonEnter) {
		glfwSetCursor(window, pointerCursor);
	}
	else if (modelPanelButtonEnter) {
		glfwSetCursor(window, pointerCursor);
	}
	else if (texturePanelButtonEnter) {
		glfwSetCursor(window, pointerCursor);
	}
	else if (paintingPanelButtonEnter) {
		glfwSetCursor(window, pointerCursor);
	}
	else if (exportPanelButtonEnter) {
		glfwSetCursor(window, pointerCursor);
	}
	else if(addMaskTextureButtonEnter){
		glfwSetCursor(window, pointerCursor);
	}
	else if (brushSizeRangeBarEnter || brushBlurRangeBarEnter || brushRotationRangeBarEnter || brushOpacityRangeBarEnter || brushSpacingRangeBarEnter) {
		glfwSetCursor(window, pointerCursor);
	}
	else if (colorBoxPickerEnter) {
		glfwSetCursor(window, pointerCursor);
	}
	else if (colorBoxColorRangeBarEnter) {
		glfwSetCursor(window,pointerCursor);
	}
	else if (exportDownloadButtonEnter || exportExtJPGCheckBoxEnter || exportExtPNGCheckBoxEnter || exportPathTextBoxEnter || exportFileNameTextBoxEnter) {
		glfwSetCursor(window, pointerCursor);
	}
	else if (textureDemonstratorButtonEnter){
		glfwSetCursor(window, pointerCursor);
	}
	else if (mirrorXCheckBoxEnter || mirrorYCheckBoxEnter || mirrorZCheckBoxEnter) {
		glfwSetCursor(window, pointerCursor);
	}
	else if (useNegativeForDrawingCheckboxEnter) {
		glfwSetCursor(window, pointerCursor);
	}
	else if (paintingDropperEnter){
		glfwSetCursor(window, pointerCursor);
	}
	else if (paintingFillNumericModifierPEnter || paintingFillNumericModifierNEnter){
		glfwSetCursor(window, pointerCursor);
	}
	else if(maskPanelSliderEnter){
		glfwSetCursor(window, pointerCursor);
	}
	else if(brushMaskPanelMaskHover){
		glfwSetCursor(window, pointerCursor);
	}
	else if(hexValueTextboxEnter){
		glfwSetCursor(window, pointerCursor);
	}
	else if (!panelChangeHover){
		glfwSetCursor(window, defaultCursor);
	}
	
}
void Callback::framebuffer_size_callback(GLFWwindow* window, int width, int height) //Protect screen ratio
{
	int intrpHeight;
	GlSet glset;
	Utilities util;
	glfwSetWindowSize(window, width, height);
	glViewport(0, -(callbackMaxScreenHeight - height), callbackMaxScreenWidth, callbackMaxScreenHeight);


	screenX = width;
	screenY = height;
}

void Callback::sendMaxWindowSize(int maxScreenWidth,int maxScreenHeight){
	callbackMaxScreenHeight = maxScreenHeight;
	callbackMaxScreenWidth = maxScreenWidth;
}