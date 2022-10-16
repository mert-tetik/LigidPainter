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
callbckData callbkData;
callback callbk;
void framebuffer_size_callback(GLFWwindow* window, int width, int height);




void scroll_callback(GLFWwindow* window, double scroll, double scrollx) 
{
	callbkData = callbk.scroll_callback(window,scroll,scrollx);
}
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	callbkData = callbk.mouse_callback(window, xpos, ypos);
}

void MSHPApp::run()
{
	commonData cmnd;
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback); 

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
	glm::vec3 cameraDirection = glm::normalize(callbkData.cameraPos - glm::vec3(0.0f, 0.0f, 0.0f));
	glm::vec3 cameraRight = glm::normalize(glm::vec3(1.0f, 0.0f, 0.0f));

	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	//Light
	glm::vec3 lightPos(1.2f, 1.0f, 2.0f);
	
	glBindVertexArray(VAO);
	glUseProgram(cmnd.program);

	ui.setViewportBgColor();


	glUniform3fv(glGetUniformLocation(cmnd.program, "lightPos"), 1, &lightPos[0]);
	glUniform3fv(glGetUniformLocation(cmnd.program, "viewPos"), 1, &callbkData.cameraPos[0]);

	glUniform1f(glGetUniformLocation(cmnd.program, "material.shininess"), 32.0f);


	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	gs.bindFrameBuffer();

	//Texture
	string albedoTexturePath = "vergil3.jpg";
	string maskTexturePath = "testHexa.jpg";

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, txtr.getTexture(albedoTexturePath, 0, 0));

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, txtr.getTexture(maskTexturePath, 0, 0));

	glUniform1i(glGetUniformLocation(cmnd.program, "material.diffuse"), 0);
	glUniform1i(glGetUniformLocation(cmnd.program, "material.specular"), 1);

	textureData td;
	td = txtr.getTextureData(albedoTexturePath.c_str());//Albedo Texture
	GLubyte* pixelsTxtr = txtr.getTextureFromProgram(GL_TEXTURE0, td.width, td.height, td.channels);
	albedoTexture albTxtr;
	albTxtr.channels = td.channels;
	albTxtr.width = td.width;
	albTxtr.height = td.height;

	td = txtr.getTextureData(maskTexturePath.c_str());//Mask Texture
	GLubyte* pixelsMask = txtr.getTextureFromProgram(GL_TEXTURE1, td.width, td.height, td.channels);
	maskTexture mskTxtr;
	mskTxtr.channels = td.channels;
	mskTxtr.width = td.width;
	mskTxtr.height = td.height;

	bool backfaceCulling = false;
	bool enableCtrlAltC = true;
	int isAxisPointerLoc;
	int isTwoDimensionalLoc;
	double mouseXpos, mouseYpos;

	bool buttonGetInput = true;
	//Loop
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();
		MSHP.transformObject(callbkData.cameraPos, callbkData.originPos);
		MSHP.renderModel(vertices, callbkData.panelLoc);

		//Light Obj
		//MSHP.drawLigtObject(shaderProgram,lightPos);
		
		//Paint
		if (glfwGetMouseButton(window, 0) == GLFW_PRESS) {
			glfwGetCursorPos(window, &mouseXpos, &mouseYpos);
			txtrGen.paintTexture(window, vertices, callbkData.cameraPos, callbkData.originPos,mouseXpos, mouseYpos,pixelsTxtr, pixelsMask, callbkData.panelLoc,mskTxtr,albTxtr);
		}
		if (callbkData.buttonEnter) {
			if (buttonGetInput) {
				if (glfwGetMouseButton(window, 0) == GLFW_PRESS) {
					GLubyte* dwnldImage = txtr.getTextureFromProgram(GL_TEXTURE0, albTxtr.width, albTxtr.height, albTxtr.channels);

					txtr.drawTexture("downloadPath.jpg", albTxtr.width, albTxtr.height, dwnldImage,albTxtr.channels);
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
		/*if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
			cameraPos = glm::vec3(0.034906f, 0.000000f, -9.999939f);
			yaw = -90.0f;
			originPos = glm::vec3(0.0f, 0.0f, 0.0f);
			pitch = 0.0f;
			xoffset = 0;
			yoffset = 0;
		}*/
	}
	glfwDestroyWindow(window);
	glfwTerminate();
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) //Update Screen
{
	glViewport(0, 0, 1920, 1080);
}