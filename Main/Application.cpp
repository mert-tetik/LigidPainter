#include<iostream>
#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include<windows.h>
#include <string>
#include <fstream>
#include <sstream>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <glm/gtx/string_cast.hpp>

#include "MSHPApp.h"
#include "MSHPModelLoader.h"

#include <vector>

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
		cout << 1;
	}
	else if (scrollx == -1) {
		radius++;
		cout << 0;
	}
	cameraPos.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch)) * radius + originPos.x;
	cameraPos.y = sin(glm::radians(pitch)) * -radius + originPos.y;
	cameraPos.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch)) * radius + originPos.z;
}
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	xoffset = xpos - lastX;
	yoffset = lastY - ypos;
	lastX = xpos;
	lastY = ypos;
	if (glfwGetMouseButton(window, 0) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
		xoffset *= sensitivity;
		yoffset *= sensitivity;

		cameraPos.x -= sin(glm::radians(yaw)) * xoffset * (sensitivity / 2) + cos(glm::radians(yaw)) * (sin(glm::radians(pitch)) * yoffset) / 4;
		originPos.x -= sin(glm::radians(yaw)) * xoffset * (sensitivity / 2) + cos(glm::radians(yaw)) * (sin(glm::radians(pitch)) * yoffset) / 4;

		cameraPos.z += cos(glm::radians(yaw)) * xoffset * (sensitivity / 2) - sin(glm::radians(yaw)) * (sin(glm::radians(pitch)) * yoffset) / 4;
		originPos.z += cos(glm::radians(yaw)) * xoffset * (sensitivity / 2) - sin(glm::radians(yaw)) * (sin(glm::radians(pitch)) * yoffset) / 4;

		cameraPos.y -= cos(glm::radians(pitch)) * yoffset * (sensitivity / 2);
		originPos.y -= cos(glm::radians(pitch)) * yoffset * (sensitivity / 2);
		cout << cos(pitch);


	}
	else if (glfwGetMouseButton(window, 0) == GLFW_PRESS) {

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
		cout << sin(glm::radians(yaw)) << '\n';
	}
}

void MSHPApp::run()
{
	MSHPApp MSHP;
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

	unsigned int VBO, VAO, EBO; //Vertex Buffer Object, Vertex Array Object***, Element Buffer Object
	//glGenBuffers(1, &EBO);
	glGenBuffers(1, &VBO);
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	/*glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);*/

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	// set the vertex attribute 
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
	glBindTexture(GL_TEXTURE_2D, MSHP.getTexture("container.png"));

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, MSHP.getTexture("container_specular.png"));

	bool backfaceCulling = false;
	bool enableCtrlAltC = true;

	//Loop
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();
		glClearColor(0.8f, 0.8f, 0.8f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glUseProgram(shaderProgram);

		const float cameraSpeed = 0.01f;

		MSHP.transformObject(shaderProgram, cameraPos, cameraFront, cameraUp, originPos);

#pragma region light_and_material_SetUp
		int objectColorLoc = glGetUniformLocation(shaderProgram, "objectColor");
		glUniform3f(objectColorLoc, 1.0f, 0.5f, 0.31f);
		int lightColorLoc = glGetUniformLocation(shaderProgram, "lightColor");
		glUniform3f(lightColorLoc, 1.0f, 1.0f, 1.0f);
		glUniform3fv(glGetUniformLocation(shaderProgram, "lightPos"), 1, &lightPos[0]);
		glUniform3fv(glGetUniformLocation(shaderProgram, "viewPos"), 1, &cameraPos[0]);
		glUniform1i(glGetUniformLocation(shaderProgram, "material.diffuse"), 0);
		glUniform1i(glGetUniformLocation(shaderProgram, "material.specular"), 1);
		glUniform1f(glGetUniformLocation(shaderProgram, "material.shininess"), 32.0f);
#pragma endregion

		int isLightSourceLoc = glGetUniformLocation(shaderProgram, "isLightSource");
		glUniform1i(isLightSourceLoc, 0);

		int isAxisPointerLoc = glGetUniformLocation(shaderProgram, "isAxisPointer");
		glUniform1i(isAxisPointerLoc, 0);

		glBindVertexArray(VAO);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW);
		glDrawArrays(GL_TRIANGLES, 0, vertices.size() / 8);

		isAxisPointerLoc = glGetUniformLocation(shaderProgram, "isAxisPointer");
		glUniform1i(isAxisPointerLoc, 1);

		glBufferData(GL_ARRAY_BUFFER, sizeof(axisPointer), axisPointer, GL_STATIC_DRAW);
		glDrawArrays(GL_LINES, 0, 6);

		//Light Obj
		//MSHP.drawLigtObject(shaderProgram,lightPos);

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
	std::system("pause>0");
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) //Update Screen
{
	glViewport(0, 0, width, height);
}