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
	if(glfwGetMouseButton(window, 0) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS){
		xoffset *= sensitivity;
		yoffset *= sensitivity;

		cameraPos.x -= sin(glm::radians(yaw)) * xoffset  * (sensitivity / 2) + cos(glm::radians(yaw))*(sin(glm::radians(pitch)) * yoffset) / 4;
		originPos.x -= sin(glm::radians(yaw)) * xoffset  * (sensitivity / 2) + cos(glm::radians(yaw)) * (sin(glm::radians(pitch)) * yoffset) / 4;

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

int main() 
{
	cout << glm::to_string(glm::vec3(1.0f) - glm::normalize(glm::vec3(3.0f, 2.0f, 1.0f)));
	MSHPApp MSHP;
	MSHP_Model_Loader MSHPLoader;
	GLFWwindow* window = MSHP.MHSPInit();
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback); //Update Screen When Screen Changed
	
	vector<float> vertices = MSHPLoader.OBJ_getVertices();
	//Shaders 
	unsigned int shaderProgram = MSHP.SetShadersAndProgram();


	unsigned int VBO,VAO,EBO; //Vertex Buffer Object, Vertex Array Object***, Element Buffer Object
	//glGenBuffers(1, &EBO);
	glGenBuffers(1, &VBO);
	glGenVertexArrays(1, &VAO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW);
	/*glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);*/



	//LIGTING
	glm::vec3 coral(1.0f, 0.5f, 0.31f);
	glm::vec3 lightColor(0.33f, 0.42f, 0.18f);
	glm::vec3 toyColor(1.0f, 0.5f, 0.31f);
	glm::vec3 result = lightColor * toyColor; // = (0.33f, 0.21f, 0.06f);



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

	//Loop
	while (!glfwWindowShouldClose(window))
	{


		glfwPollEvents();//Get window interactions
		glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT); //Clean Back Buffer and assign the specified RGBA value

		//DrawObject
		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glUseProgram(shaderProgram);
		/*glUniform1i(glGetUniformLocation(shaderProgram, "texture1"), 0); 
		glUniform1i(glGetUniformLocation(shaderProgram, "texture2"), 1); */

		const float cameraSpeed = 0.01f; 
		#pragma region User_Inputs
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
			if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
				cameraPos += cameraSpeed * cameraFront;
			}
			else {
				cameraPos += cameraSpeed * cameraUp;
			}
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
			if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
				cameraPos -= cameraSpeed * cameraFront;
			}
			else {
				cameraPos -= cameraSpeed * cameraUp;
			}
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
			cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
			cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
		#pragma endregion


		MSHP.SetLocations(shaderProgram, cameraPos, cameraFront, cameraUp, originPos);

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

		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, vertices.size()/8);

		//Light Obj
		//MSHP.drawLigtObject(shaderProgram,lightPos);

		//ESC - Close The Window
		glfwSwapBuffers(window);
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
			glfwSetWindowShouldClose(window, true);
		}
		//CTRL D - Get Back To Default Location
		if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
			cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
			cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
			yaw = -90.0f;
			pitch = 0.0f;
			xoffset = 0;
			yoffset = 0;
		}
	}
	system("pause>0");
	return 0;

}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) //Update Screen
{
	glViewport(0,0, width, height);
}