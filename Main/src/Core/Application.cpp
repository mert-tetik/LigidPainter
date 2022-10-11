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

using namespace std;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);

float yaw = -90.0f;
float lastX = 400, lastY = 300;
float pitch = 0.0f;

float xoffset;
float yoffset;
float sensitivity = 0.2f;
int radius = 10;
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraPos = glm::vec3(0.034906f, 0.000000f, -9.999939f);
glm::vec3 originPos = glm::vec3(0.0f, 0.0f, 0.0f);

glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
glm::vec3 cameraRight = glm::vec3(0.0f, 0.0f, 1.0f);
glm::vec3 cameraForward = glm::vec3(1.0f, 0.0f, 0.0f);

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
	//std::cout << '\n' << xpos << " / " << ypos;
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
	MSHPApp MSHP;
	MSHP_Texture_Generator txtrGen;
	MSHP_Model_Loader MSHPLoader;
	GLFWwindow* window = MSHP.init();
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback); //Update Screen When Screen Changed

	vector<float> vertices = MSHPLoader.OBJ_getVertices();

	float axisPointer[] = {
		0.0f, -100.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, //Y
		0.0f, 100.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, //Y
		-100.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, //X
		100.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, //X
		0.0f, 0.0f, -100.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, //Z
		0.0f, 0.0f, 100.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, //Z
	};
	//Shaders 
	unsigned int shaderProgram = MSHP.getProgram();
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
	glm::vec3 cameraUp = glm::cross(cameraDirection, cameraRight);

	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	//Light & Texture
	glm::vec3 lightPos(1.2f, 1.0f, 2.0f);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, MSHP.getTexture("vergil2.jpg",0,0));

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, MSHP.getTexture("testHexa.jpg",0,0));

	bool backfaceCulling = false;
	bool enableCtrlAltC = true;
	int a = 0;
	glBindVertexArray(VAO);
	glClearColor(0.8f, 0.8f, 0.8f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glUseProgram(shaderProgram);
	const float cameraSpeed = 0.01f;
	

	int objectColorLoc = glGetUniformLocation(shaderProgram, "objectColor");
	glUniform3f(objectColorLoc, 1.0f, 0.5f, 0.31f);
	int lightColorLoc = glGetUniformLocation(shaderProgram, "lightColor");
	glUniform3f(lightColorLoc, 1.0f, 1.0f, 1.0f);
	glUniform3fv(glGetUniformLocation(shaderProgram, "lightPos"), 1, &lightPos[0]);
	glUniform3fv(glGetUniformLocation(shaderProgram, "viewPos"), 1, &cameraPos[0]);
	glUniform1i(glGetUniformLocation(shaderProgram, "material.diffuse"), 0);
	glUniform1i(glGetUniformLocation(shaderProgram, "material.specular"), 1);
	glUniform1f(glGetUniformLocation(shaderProgram, "material.shininess"), 32.0f);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	//vector<float> dataX;
	unsigned int FBO;
	glGenFramebuffers(1, &FBO);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, FBO);

	//Texture
	GLubyte* pixelsTxtr = new GLubyte[1000 * 1000 * 3 * sizeof(GLubyte)];
	glActiveTexture(GL_TEXTURE0);
	glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_UNSIGNED_BYTE, pixelsTxtr);
	//Mask Texture
	GLubyte* pixelsMask = new GLubyte[50 * 50 * 3 * sizeof(GLubyte)*2];
	glActiveTexture(GL_TEXTURE1);
    glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_UNSIGNED_BYTE, pixelsMask);

	int isAxisPointerLoc;

	double mouseXpos, mouseYpos;

	//Loop
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();
		MSHP.transformObject(shaderProgram, cameraPos, cameraFront, cameraUp, originPos);

#pragma region light_and_material_SetUp
		
#pragma endregion

		MSHP.renderModel(vertices, shaderProgram);

		isAxisPointerLoc = glGetUniformLocation(shaderProgram, "isAxisPointer");
		glUniform1i(isAxisPointerLoc, 1);

		glBufferData(GL_ARRAY_BUFFER, sizeof(axisPointer), axisPointer, GL_STATIC_DRAW);
		glDrawArrays(GL_LINES, 0, 6);

		//Light Obj
		//MSHP.drawLigtObject(shaderProgram,lightPos);
#pragma region Paint
		if (glfwGetMouseButton(window, 0) == GLFW_PRESS) {
			glfwGetCursorPos(window, &mouseXpos, &mouseYpos);
			txtrGen.paintTexture(window, vertices, shaderProgram, cameraPos, cameraFront, cameraUp, originPos,mouseXpos, mouseYpos,pixelsTxtr, pixelsMask);
		}
#pragma endregion

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
	glViewport(0, 0, 800, 800);
}