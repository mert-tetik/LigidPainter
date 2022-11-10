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
#include "RigidPainter.h"
#include "UserInterface.h"
#include "Callback.h"
#include "gl.h"
#include <vector>
#include "stb_image.h"
#include "stb_image_write.h"

float yaw = -90.0f;
float lastX = 400, lastY = 300;
float pitch = 0.0f;

float xoffset;
float yoffset;
float sensitivity = 0.2f;
int radius = 10;

glm::vec3 cameraRight = glm::vec3(0.0f, 0.0f, 1.0f);
glm::vec3 cameraForward = glm::vec3(1.0f, 0.0f, 0.0f);

//Panel
int holdXPos = 1536;
float panelLoc = 1.6f;
float panelOffset;
bool changeLoc = false;

bool buttonEnter = true;
//Button
int* buttonScreenCoor;
//Cursors
GLFWcursor* cursorHResize = glfwCreateStandardCursor(GLFW_HRESIZE_CURSOR);


bool loadModelButtonEnter;
bool modelFilePathTextBoxEnter;
bool autoTriangulateCheckBoxEnter;
bool backfaceCullingCheckBoxEnter;
bool addPlaneButtonEnter;
bool addSphereButtonEnter;
bool addImageButtonEnter;
bool addMaskTextureButtonEnter;
bool brushSizeRangeBarEnter;
bool colorBoxColorRangeBarEnter;
bool colorBoxPickerEnter;
bool modelPanelButtonEnter;
bool texturePanelButtonEnter;
bool exportPanelButtonEnter;
bool paintingPanelButtonEnter;

bool exportPathTextBoxEnter;
bool exportExtJPGButtonEnter;
bool exportExtPNGButtonEnter;
bool exportDownloadButtonEnter;

bool movePanel;

glm::vec3 originPos = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 cameraPos = glm::vec3(0.034906f, 0.000000f, -9.999939f);

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
	callbk.colorBoxColorRangeBarEnter = colorBoxColorRangeBarEnter;
	callbk.colorBoxPickerEnter = colorBoxPickerEnter;
	callbk.exportPanelButtonEnter = exportPanelButtonEnter;

	callbk.exportPathTextBoxEnter = exportPathTextBoxEnter;
	callbk.exportExtJPGButtonEnter = exportExtJPGButtonEnter;
	callbk.exportExtPNGButtonEnter = exportExtPNGButtonEnter;
	callbk.exportDownloadButtonEnter = exportDownloadButtonEnter;

	callbk.cameraPos = cameraPos;
	callbk.originPos = originPos;
	callbk.panelLoc = panelLoc;

	return callbk;
}

CallbckData Callback::scroll_callback(GLFWwindow* window, double scroll, double scrollx) {
	CallbckData callbk;

	if (scrollx == 1 && radius != 1) {
		radius--;
	}
	else if (scrollx == -1) {
		radius++;
	}
	cameraPos.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch)) * radius + originPos.x;
	cameraPos.y = sin(glm::radians(pitch)) * -radius + originPos.y;
	cameraPos.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch)) * radius + originPos.z;

	callbk = preapareCallbackData();
	return callbk;
}
CallbckData Callback::mouse_callback(GLFWwindow* window, double xpos, double ypos, PanelData panelData,float brushSizeRangeBarValue,float colorBoxPickerValue_x, float colorBoxPickerValue_y,float colorBoxColorRangeBarValue)
{
	CallbckData callbk;
	
	//Get window size
	int screenSizeX;
	int screenSizeY;
	glfwGetWindowSize(window, &screenSizeX, &screenSizeY);

	panelCheck(window,xpos,screenSizeX);
	
	buttonCheck(window, xpos, ypos, panelData ,brushSizeRangeBarValue, colorBoxPickerValue_x, colorBoxPickerValue_y, colorBoxColorRangeBarValue);
	

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
void Callback::panelCheck(GLFWwindow* window, int mouseXpos, int screenSizeX) {
	panelOffset = mouseXpos - holdXPos;
	holdXPos = mouseXpos;
	if (changeLoc) {
		panelLoc += panelOffset / 1000;
		if (panelLoc > 1.7f ) {
			movePanel = true;
		}
		else {
			movePanel = false;
		}
		if (panelLoc < 1.6f)
			panelLoc = 1.6f;
		if (panelLoc > 1.987f)
			panelLoc = 1.987f;
		if (glfwGetMouseButton(window, 0) == GLFW_RELEASE) {
			changeLoc = false;
		}
	}

	if (mouseXpos > (screenSizeX / 2 * panelLoc) - 10 && mouseXpos < (screenSizeX / 2 * panelLoc) + 10) {
		if (glfwGetMouseButton(window, 0) == GLFW_PRESS) {
			changeLoc = true;
		}
		//glfwSetCursor(window, cursorHResize);
	}
	else {
		//Return Once
		//glfwSetCursor(window, arrowCursor);
	}
}
void Callback::buttonCheck(GLFWwindow* window, int mouseXPos,int mouseYPos,PanelData panelData, float brushSizeRangeBarValue, float colorBoxPickerValue_x, float colorBoxPickerValue_y,float colorBoxColorRangeBarValue) {
	UserInterface ui;
	GlSet glset;
	CommonData commonData;

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

	GLFWcursor* pointerCursor = glfwCreateStandardCursor(GLFW_HAND_CURSOR);
	if (panelData.modelPanelActive) {
		loadModelButtonEnter = ui.isMouseOnButton(window, 0.08f, 0.04f, panelLoc / centerDivider + centerSum, 0.4f, mouseXPos, mouseYPos,movePanel);
		modelFilePathTextBoxEnter = ui.isMouseOnButton(window, 0.12f, 0.03f, panelLoc / centerDivider + centerSum, 0.6f, mouseXPos, mouseYPos, movePanel);
		autoTriangulateCheckBoxEnter = ui.isMouseOnButton(window, 0.012f, 0.02f, panelLoc / centerDivider + centerSum - 0.08f, 0.3f, mouseXPos, mouseYPos, movePanel);
		backfaceCullingCheckBoxEnter = ui.isMouseOnButton(window, 0.012f, 0.02f, panelLoc / centerDivider + centerSum - 0.08f, 0.2f, mouseXPos, mouseYPos, movePanel);
		addPlaneButtonEnter = ui.isMouseOnButton(window, 0.08f, 0.04f, panelLoc / centerDivider + centerSum, 0.0f, mouseXPos, mouseYPos, movePanel);
		addSphereButtonEnter = ui.isMouseOnButton(window, 0.08f, 0.04f, panelLoc / centerDivider + centerSum, -0.1f, mouseXPos, mouseYPos, movePanel);
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
		addImageButtonEnter = ui.isMouseOnButton(window, 0.1f, 0.04f, panelLoc / centerDivider + centerSum, 0.8f, mouseXPos, mouseYPos, movePanel);
	}
	else {
		addImageButtonEnter = false;
	}
	if (panelData.paintingPanelActive) {
		addMaskTextureButtonEnter = ui.isMouseOnButton(window, 0.1f, 0.04f, panelLoc / centerDivider + centerSum, 0.8f, mouseXPos, mouseYPos, movePanel);
		brushSizeRangeBarEnter = ui.isMouseOnButton(window, 0.01f, 0.02f, panelLoc / centerDivider + centerSum+ brushSizeRangeBarValue, 0.05f, mouseXPos, mouseYPos, movePanel);
		colorBoxPickerEnter = ui.isMouseOnButton(window,0.01f, 0.02f, panelLoc / centerDivider + centerSum - 0.02f + colorBoxPickerValue_x, -0.3f + colorBoxPickerValue_y, mouseXPos, mouseYPos, movePanel);
		colorBoxColorRangeBarEnter = ui.isMouseOnButton(window, 0.01f, 0.01f, panelLoc / centerDivider + centerSum + 0.1f, -0.3f + colorBoxColorRangeBarValue, mouseXPos, mouseYPos, movePanel);
	}
	else {
		addMaskTextureButtonEnter = false;
		brushSizeRangeBarEnter = false;
		colorBoxColorRangeBarEnter = false;
		colorBoxPickerEnter = false;
	}
	if (panelData.exportPanelActive) {
		exportPathTextBoxEnter = ui.isMouseOnButton(window, 0.12f, 0.03f, panelLoc / centerDivider + centerSum, 0.6f, mouseXPos, mouseYPos, movePanel);
		exportExtJPGButtonEnter = ui.isMouseOnButton(window, 0.012f, 0.02f, panelLoc / centerDivider + centerSum - 0.11f, 0.5f, mouseXPos, mouseYPos, movePanel);
		exportExtPNGButtonEnter = ui.isMouseOnButton(window, 0.012f, 0.02f, panelLoc / centerDivider + centerSum + 0.05f, 0.5f + colorBoxPickerValue_y, mouseXPos, mouseYPos, movePanel);
		exportDownloadButtonEnter = ui.isMouseOnButton(window, 0.1f, 0.04f, panelLoc / centerDivider + centerSum, 0.3f, mouseXPos, mouseYPos, movePanel);
	}
	else {
		exportPathTextBoxEnter = false;
		exportExtJPGButtonEnter = false;
		exportExtPNGButtonEnter = false;
		exportDownloadButtonEnter = false;
	}

	modelPanelButtonEnter = ui.isMouseOnPanelChangeButton(window, panelLoc, 0.8f, mouseXPos, mouseYPos);
	texturePanelButtonEnter = ui.isMouseOnPanelChangeButton(window, panelLoc, 0.72f, mouseXPos, mouseYPos);
	paintingPanelButtonEnter = ui.isMouseOnPanelChangeButton(window, panelLoc, 0.64f, mouseXPos, mouseYPos);
	exportPanelButtonEnter = ui.isMouseOnPanelChangeButton(window, panelLoc, 0.56f, mouseXPos, mouseYPos);

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
	else if (brushSizeRangeBarEnter) {
		glfwSetCursor(window, pointerCursor);
	}
	else if (colorBoxPickerEnter) {
		glfwSetCursor(window, pointerCursor);
	}
	else if (colorBoxColorRangeBarEnter) {
		glfwSetCursor(window,pointerCursor);
	}
	else if (exportDownloadButtonEnter || exportExtJPGButtonEnter || exportExtPNGButtonEnter || exportPathTextBoxEnter) {
		glfwSetCursor(window, pointerCursor);
	}
	else {
		GLFWcursor* arrowCursor = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
		glfwSetCursor(window, arrowCursor);
	}
	
}
void Callback::framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	GlSet glset;
	int screenSizeX;
	int screenSizeY;
	glfwGetWindowSize(window, &screenSizeX, &screenSizeY);
	height = width / 16 * 9;
	glfwSetWindowSize(window, width, height);
	glset.viewport(screenSizeX, screenSizeY);
}