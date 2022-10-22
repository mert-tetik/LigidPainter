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

bool movePanel;


glm::vec3 originPos = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 cameraPos = glm::vec3(0.034906f, 0.000000f, -9.999939f);

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

	callbk.loadModelButtonEnter = loadModelButtonEnter;
	callbk.modelFilePathTextBoxEnter = modelFilePathTextBoxEnter;
	callbk.cameraPos = cameraPos;
	callbk.originPos = originPos;
	callbk.panelLoc = panelLoc;
	return callbk;
}
CallbckData Callback::mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	CallbckData callbk;
	UserInterface ui;
	
	//Get window size
	int screenSizeX;
	int screenSizeY;
	glfwGetWindowSize(window, &screenSizeX, &screenSizeY);

	panelCheck(window,xpos,screenSizeX);
	
	buttonCheck(window, xpos, ypos);
	

	xoffset = xpos - lastX;
	yoffset = lastY - ypos;
	lastX = xpos;
	lastY = ypos;
	if (glfwGetMouseButton(window, 1) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
		xoffset *= sensitivity;
		yoffset *= sensitivity;

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

		if (pitch > 89.0f)
			pitch = 89.0f;
		if (pitch < -89.0f)
			pitch = -89.0f;
		cameraPos.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch)) * radius + originPos.x;
		cameraPos.y = sin(glm::radians(pitch)) * -radius + originPos.y;
		cameraPos.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch)) * radius + originPos.z;
	}
	callbk.loadModelButtonEnter = loadModelButtonEnter;
	callbk.modelFilePathTextBoxEnter = modelFilePathTextBoxEnter;
	callbk.autoTriangulateCheckBoxEnter = autoTriangulateCheckBoxEnter;
	callbk.backfaceCullingCheckBoxEnter = backfaceCullingCheckBoxEnter;
	callbk.addPlaneButtonEnter = addPlaneButtonEnter;
	callbk.addSphereButtonEnter = addSphereButtonEnter;
	callbk.movePanel = movePanel;

	callbk.cameraPos = cameraPos;
	callbk.originPos = originPos;
	callbk.panelLoc = panelLoc;
	return callbk;
}
void Callback::panelCheck(GLFWwindow* window, int mouseXpos, int screenSizeX) {
	panelOffset = mouseXpos - holdXPos;
	holdXPos = mouseXpos;
	if (changeLoc) {
		panelLoc += panelOffset / 1000;
		if (panelLoc > 1.7f)
			movePanel = true;
		if (panelLoc < 1.5f)
			panelLoc = 1.5f;
		if (glfwGetMouseButton(window, 0) == GLFW_RELEASE) {
			changeLoc = false;
		}
	}

	if (mouseXpos > (screenSizeX / 2 * panelLoc) - 10 && mouseXpos < (screenSizeX / 2 * panelLoc) + 40) {
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
void Callback::buttonCheck(GLFWwindow* window, int mouseXPos,int mouseYPos) {
	UserInterface ui;

	
	GLFWcursor* pointerCursor = glfwCreateStandardCursor(GLFW_HAND_CURSOR);
	loadModelButtonEnter = ui.isMouseOnButton(window, 0.08f, 0.04f, panelLoc / 2, 0.4f, mouseXPos, mouseYPos);
	modelFilePathTextBoxEnter = ui.isMouseOnButton(window, 0.12f, 0.03f, panelLoc / 2, 0.6f, mouseXPos, mouseYPos);
	autoTriangulateCheckBoxEnter = ui.isMouseOnButton(window, 0.012f, 0.02f, panelLoc / 2 - 0.08f, 0.3f, mouseXPos, mouseYPos);
	backfaceCullingCheckBoxEnter = ui.isMouseOnButton(window, 0.012f, 0.02f, panelLoc / 2 - 0.08f, 0.2f, mouseXPos, mouseYPos);
	addPlaneButtonEnter = ui.isMouseOnButton(window, 0.08f, 0.04f, panelLoc / 2, 0.0f, mouseXPos, mouseYPos);
	addSphereButtonEnter = ui.isMouseOnButton(window, 0.08f, 0.04f, panelLoc / 2, -0.1f, mouseXPos, mouseYPos);
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