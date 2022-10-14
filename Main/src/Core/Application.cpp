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
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STBI_MSC_SECURE_CRT
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

using namespace std;

MSHPApp MSHP;
MSHP_Texture_Generator txtrGen;
MSHP_Model_Loader MSHPLoader;
MSHP_userInterface ui;
GLFWwindow* window = MSHP.init();
texture txtr;
glSet gs;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);

float yaw = -90.0f;
float lastX = 400, lastY = 300;
float pitch = 0.0f;

float xoffset;
float yoffset;
float sensitivity = 0.2f;
int radius = 10;
glm::vec3 cameraPos = glm::vec3(0.034906f, 0.000000f, -9.999939f);
glm::vec3 originPos = glm::vec3(0.0f, 0.0f, 0.0f);

glm::vec3 cameraRight = glm::vec3(0.0f, 0.0f, 1.0f);
glm::vec3 cameraForward = glm::vec3(1.0f, 0.0f, 0.0f);

//Panel
int holdXPos = 1536;
float panelLoc = 1.6f;
float panelOffset;
bool changeLoc = false;
//Button
int* buttonScreenCoor;
bool buttonGetInput;
bool buttonEnter;
//Cursors
GLFWcursor* cursorHResize = glfwCreateStandardCursor(GLFW_HRESIZE_CURSOR);
GLFWcursor* arrowCursor = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
GLFWcursor* pointerCursor = glfwCreateStandardCursor(GLFW_HAND_CURSOR);


void scroll_callback(GLFWwindow* window, double scroll, double scrollx) {
	if (scrollx == 1 && radius != 1) {
		radius--;
	}
	else if (scrollx == -1) {
		radius++;
	}
	cameraPos.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch)) * radius + originPos.x;
	cameraPos.y = sin(glm::radians(pitch)) * -radius + originPos.y;
	cameraPos.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch)) * radius + originPos.z;
}
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	cout << xpos << ' ' << ypos << '\n';
	if (xpos > (1920/2 * panelLoc) - 10 && xpos < (1920/2 * panelLoc) + 10) {
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
	buttonScreenCoor = ui.buttonGetScreenCoor(0.1f, panelLoc / 2, - 0.8f);
	if (xpos < buttonScreenCoor[0] && xpos > buttonScreenCoor[4] && ypos > buttonScreenCoor[1] && ypos < buttonScreenCoor[7]) {
		glfwSetCursor(window, pointerCursor);
		buttonEnter = true;
	}
	else {
		buttonEnter = false;
	}
	cout << buttonScreenCoor[7] << ' ';
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
}

void MSHPApp::run()
{
	commonData cmnd;
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback); //Update Screen When Screen Changed

	MSHP.getProgram();
	vector<float> vertices = MSHPLoader.OBJ_getVertices();
	//glTexSubImage2D

	unsigned int VBO, VAO, EBO; //Vertex Buffer Object, Vertex Array Object, Element Buffer Object
	//glGenBuffers(1, &EBO);
	glGenBuffers(1, &VBO);

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);


	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(2);

	//Camera
	glm::vec3 cameraDirection = glm::normalize(cameraPos - glm::vec3(0.0f, 0.0f, 0.0f));
	glm::vec3 cameraRight = glm::normalize(glm::vec3(1.0f, 0.0f, 0.0f));

	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	//Light & Texture
	glm::vec3 lightPos(1.2f, 1.0f, 2.0f);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, txtr.getTexture("vergil2.jpg",0,0));

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, txtr.getTexture("testHexa.jpg",0,0));

	glBindVertexArray(VAO);
	glUseProgram(cmnd.program);
	
	ui.setViewportBgColor();


	glUniform3fv(glGetUniformLocation(cmnd.program, "lightPos"), 1, &lightPos[0]);
	glUniform3fv(glGetUniformLocation(cmnd.program, "viewPos"), 1, &cameraPos[0]);
	glUniform1i(glGetUniformLocation(cmnd.program, "material.diffuse"), 0);
	glUniform1i(glGetUniformLocation(cmnd.program, "material.specular"), 1);
	glUniform1f(glGetUniformLocation(cmnd.program, "material.shininess"), 32.0f);


	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	gs.bindFrameBuffer();

	//Texture
	GLubyte* pixelsTxtr = txtr.getTextureFromProgram(GL_TEXTURE0, 1000, 1000, 3);
	//Mask Texture
	GLubyte* pixelsMask = txtr.getTextureFromProgram(GL_TEXTURE1, 50, 50, 3);

	bool backfaceCulling = false;
	bool enableCtrlAltC = true;
	int isAxisPointerLoc;
	int isTwoDimensionalLoc;
	double mouseXpos, mouseYpos;

	//Loop
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();
		MSHP.transformObject(cameraPos, originPos);
		MSHP.renderModel(vertices,panelLoc);

		

		//Light Obj
		//MSHP.drawLigtObject(shaderProgram,lightPos);
		
		//Paint
		if (glfwGetMouseButton(window, 0) == GLFW_PRESS) {
			glfwGetCursorPos(window, &mouseXpos, &mouseYpos);
			txtrGen.paintTexture(window, vertices,cameraPos, originPos,mouseXpos, mouseYpos,pixelsTxtr, pixelsMask, panelLoc);
		}
		if (buttonEnter) {
			if (buttonGetInput) {
				if (glfwGetMouseButton(window, 0) == GLFW_PRESS) {
					GLubyte* dwnldImage = txtr.getTextureFromProgram(GL_TEXTURE0, 1000, 1000, 3);

					txtr.drawTexture("C:\\Users\\CASPER\\Desktop\\deneyis.jpg", 1000, 1000, dwnldImage,3);
					buttonGetInput = false;
				}
			}
			if (glfwGetMouseButton(window, 0) == GLFW_PRESS) {
				buttonGetInput = true;
			}
		}
		//ESC - Close The Window
		glfwSwapBuffers(window);
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
			glfwSetWindowShouldClose(window, true);
		}
		//CTRL ALT C - Enable Backface Culling
		if (enableCtrlAltC) {
			if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS) {
				Sleep(100);
				if (backfaceCulling == false) {
					glEnable(GL_CULL_FACE);
					glCullFace(GL_BACK);
					backfaceCulling = true;
				}
				else {
					glDisable(GL_CULL_FACE);
					backfaceCulling = false;
				}

			}
			enableCtrlAltC = false;
		}
		if (glfwGetKey(window, GLFW_KEY_C) == GLFW_RELEASE)
			enableCtrlAltC = true;
		//CTRL D - Get Back To Default Location
		if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
			cameraPos = glm::vec3(0.034906f, 0.000000f, -9.999939f);
			yaw = -90.0f;
			originPos = glm::vec3(0.0f, 0.0f, 0.0f);
			pitch = 0.0f;
			xoffset = 0;
			yoffset = 0;
		}
	}
	glfwDestroyWindow(window);
	glfwTerminate();
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) //Update Screen
{
	glViewport(0, 0, 1920, 1080);
}