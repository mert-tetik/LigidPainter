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
#include "Fadenode.h"
#include <vector>
#include <map>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STBI_MSC_SECURE_CRT
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

using namespace std;

Utilities utilities;
Texture_Generator txtrGen;
Model_Loader modelLoader;
UserInterface ui;
Texture txtr;
GlSet glset;
CallbckData callbackData;
Callback callback;
GLFWwindow* window = glset.getWindow();
void framebuffer_size_callback(GLFWwindow* window, int width, int height);

unsigned int VBO, VAO; //Vertex Buffer Object, Vertex Array Object

void scroll_callback(GLFWwindow* window, double scroll, double scrollx) 
{
	callbackData = callback.scroll_callback(window,scroll,scrollx);
}
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	callbackData = callback.mouse_callback(window, xpos, ypos);
}
char currentKey;
bool changeModelFilePathText = false;
//bool preventPairs = false;
//char keyCounter = 0;
string modelFilePath;

bool backfaceCulling = false;
bool enableCtrlAltC = true;
int isAxisPointerLoc;
int isTwoDimensionalLoc;
double mouseXpos, mouseYpos;
bool buttonGetInput = true;
bool buttonPressed = false;
string modelName;
vector<float> vertices = { 0 };

void ctrlAltEsc();
void loadModelButton();
void modelFilePathTextBoxEnter();


void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	//--
}

void Fadenode::run()
{

	CommonData commonData;
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback); 
	glfwSetKeyCallback(window, key_callback);

	glset.getProgram();

	glGenBuffers(1, &VBO);
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glUseProgram(commonData.program);


	glset.setVertexAtribPointer();
	glset.bufferData(vertices);

	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	glm::mat4 projection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f);
	glset.uniformMatrix4fv(commonData.program, "TextProjection", projection);
	ui.setViewportBgColor();
	ui.uploadChars();


	glm::vec3 lightPos(1.2f, 1.0f, 2.0f);
	

	glset.uniform3fv(commonData.program, "lightPos", lightPos);
	glset.uniform3fv(commonData.program, "viewPos", callbackData.cameraPos);
	glset.uniform1f(commonData.program, "material.shininess", 32.0f);


	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glset.bindFrameBuffer();

	//Texture
	string albedoTexturePath = "vergil3.jpg";
	string maskTexturePath = "testHexa.jpg";

	glset.activeTexture(GL_TEXTURE0);
	glset.bindTexture(txtr.getTexture(albedoTexturePath, 0, 0));

	glset.activeTexture(GL_TEXTURE1);
	glset.bindTexture(txtr.getTexture(maskTexturePath, 0, 0));


	glset.uniform1i(commonData.program, "material.diffuse", 0);
	glset.uniform1i(commonData.program, "material.specular", 1);

	framebuffer_size_callback(window,1900,1000);
	//Loop
	while (!glfwWindowShouldClose(window))
	{
		/*int isTextLoc = glGetUniformLocation(cmnd.program, "isText");
		glUniform1i(isTextLoc, 0);
		int isTextFLoc = glGetUniformLocation(cmnd.program, "isTextF");
		glUniform1i(isTextFLoc, 0);*/

		glfwPollEvents();
		glset.setMatrices(callbackData.cameraPos, callbackData.originPos);
		glset.render(window,vertices, callbackData.panelLoc, modelName);

		//Light Obj
		//MSHP.drawLigtObject(shaderProgram,lightPos);
		
		//Paint
		if (glfwGetMouseButton(window, 0) == GLFW_PRESS) {
			glfwGetCursorPos(window, &mouseXpos, &mouseYpos);
			//txtrGen.paintTexture(window, vertices, callbkData.cameraPos, callbkData.originPos,mouseXpos, mouseYpos,pixelsTxtr, pixelsMask, callbkData.panelLoc,mskTxtr,albTxtr);
			//glGenerateMipmap(GL_TEXTURE_2D);
		}
		modelFilePathTextBoxEnter();
		loadModelButton();//
		ctrlAltEsc();//Close Window
		glfwSwapBuffers(window);
		/*if (enableCtrlAltC) {
			if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS) {
				Sleep(100);
				if (backfaceCulling == false) {
					glset.enable(GL_CULL_FACE);
					glset.cullFace(GL_BACK);
					backfaceCulling = true;
				}
				else {
					glset.disable(GL_CULL_FACE);
					backfaceCulling = false;
				}

			}
			enableCtrlAltC = false;
		}
		if (glfwGetKey(window, GLFW_KEY_C) == GLFW_RELEASE)
			enableCtrlAltC = true;*/

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
void modelFilePathTextBoxEnter() {
	if (callbackData.modelFilePathTextBoxEnter) {
		if (buttonGetInput) {
			if (glfwGetMouseButton(window, 0) == GLFW_PRESS) {
				buttonGetInput = false;
				buttonPressed = true;
			}
		}
		if (glfwGetMouseButton(window, 0) == GLFW_RELEASE) {
			buttonGetInput = true;
			if (buttonPressed) {
				modelFilePath = utilities.openFileDialog();
				modelName = utilities.getLastWordBySeparatingWithChar(modelFilePath, '\\');
			}
			buttonPressed = false;
		}
	}
}
void ctrlAltEsc() {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
}
void loadModelButton() {
	if (callbackData.loadModelButtonEnter) {
		if (buttonGetInput) {
			if (glfwGetMouseButton(window, 0) == GLFW_PRESS) {
				buttonGetInput = false;
				buttonPressed = true;
			}
		}
		if (glfwGetMouseButton(window, 0) == GLFW_RELEASE && buttonPressed) {
			buttonGetInput = true;
			if (modelName != "" && modelName[modelName.size() - 1] == 'j' && modelName[modelName.size() - 2] == 'b' && modelName[modelName.size() - 3] == 'o' && modelName[modelName.size() - 4] == '.') {
				vertices.clear();
				vertices = modelLoader.OBJ_getVertices(modelFilePath);
			}
			buttonPressed = false;
		}
	}
}
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	callback.framebuffer_size_callback(window,width,height);
}


