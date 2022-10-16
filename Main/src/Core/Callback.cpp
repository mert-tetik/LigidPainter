#include<iostream>
#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include<windows.h>
#include <string>
#include <fstream>
#include <sstream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include "MSHPApp.h"
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
//Button
int* buttonScreenCoor;
//Cursors
GLFWcursor* cursorHResize = glfwCreateStandardCursor(GLFW_HRESIZE_CURSOR);
GLFWcursor* arrowCursor = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
GLFWcursor* pointerCursor = glfwCreateStandardCursor(GLFW_HAND_CURSOR);

bool buttonEnter;
glm::vec3 originPos = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 cameraPos = glm::vec3(0.034906f, 0.000000f, -9.999939f);

callbckData callback::scroll_callback(GLFWwindow* window, double scroll, double scrollx) {
	callbckData callbk;

	if (scrollx == 1 && radius != 1) {
		radius--;
	}
	else if (scrollx == -1) {
		radius++;
	}
	cameraPos.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch)) * radius + originPos.x;
	cameraPos.y = sin(glm::radians(pitch)) * -radius + originPos.y;
	cameraPos.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch)) * radius + originPos.z;

	callbk.buttonEnter = buttonEnter;
	callbk.cameraPos = cameraPos;
	callbk.originPos = originPos;
	callbk.panelLoc = panelLoc;
	return callbk;
}
callbckData callback::mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	callbckData callbk;
	MSHP_userInterface ui;
	if (xpos > (1920 / 2 * panelLoc) - 10 && xpos < (1920 / 2 * panelLoc) + 10) {
		if (glfwGetMouseButton(window, 0) == GLFW_PRESS) {
			changeLoc = true;
		}
		glfwSetCursor(window, cursorHResize);
	}
	else {
		//Return Once
		glfwSetCursor(window, arrowCursor);
	}
	panelOffset = xpos - holdXPos;
	holdXPos = xpos;
	if (changeLoc) {
		panelLoc += panelOffset / 1000;
		if (panelLoc > 1.8f)
			panelLoc = 1.8f;
		if (panelLoc < 1.4f)
			panelLoc = 1.4f;
		if (glfwGetMouseButton(window, 0) == GLFW_RELEASE) {
			changeLoc = false;
		}
	}
	buttonScreenCoor = ui.buttonGetScreenCoor(0.1f, panelLoc / 2, -0.8f);
	if (xpos < buttonScreenCoor[0] && xpos > buttonScreenCoor[4] && ypos > buttonScreenCoor[1] && ypos < buttonScreenCoor[7]) {
		glfwSetCursor(window, pointerCursor);
		callbk.buttonEnter = true;
	}
	else {
		callbk.buttonEnter = false;
	}
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
	callbk.buttonEnter = buttonEnter;
	callbk.cameraPos = cameraPos;
	callbk.originPos = originPos;
	callbk.panelLoc = panelLoc;
	return callbk;
}