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
#include "ModelLoader.h"
#include "UserInterface.h"
#include "Utilities.h"
#include "Callback.h"
#include "gl.h"
#include "Texture.h"

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
ModelLoader modelLoader;
UserInterface ui;
Texture txtr;
GlSet glset;
CallbckData callbackData;
Callback callback;
GLFWwindow* window = glset.getWindow();
RenderData renderData;
PanelData panelData;

bool modelPanelActive = true;
bool texturePanelActive;
bool paintingPanelActive;

bool brushSizeRangeBarPressed;
float brushSizeRangeBarValue = 0.0f;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);

unsigned int VBO, VAO; //Vertex Buffer Object, Vertex Array Object
bool cameraPosChanged = true;
bool paintingMode = false;
glm::vec3 holdCameraPos;

double xOffset;
double lastXpos;



char currentKey;
bool changeModelFilePathText = false;
//bool preventPairs = false;
//char keyCounter = 0;
string modelFilePath;

bool backfaceCulling = false;
bool enableCtrlAltC = true;
bool enableBackfaceCulling;
int isAxisPointerLoc;
int isTwoDimensionalLoc;
double mouseXpos, mouseYpos;
bool buttonGetInput = true;
bool buttonPressed = false;
bool autoTriangulateChecked = true;
bool backfaceCullingChecked = true;

string modelName;
vector<float> vertices = { 0 };

void refreshScreenDrawingTexture();
void loadModelButton();
void modelFilePathTextBoxEnter();
void autoTriangulateCheckBoxEnter();
void backfaceCullingCheckBox();
void addPlaneButtonEnter();
void addSphereButtonEnter();
void addImageButtonEnter();
void modelPanelButtonEnter();
void texturePanelButtonEnter();
void paintingPanelButtonEnter();
void addMaskTextureButtonEnter();
void brushSizeRangeBarEnter();
void uiActions();
void scroll_callback(GLFWwindow* window, double scroll, double scrollx);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void drawBrushSizeIndicator();

//Texture
string albedoTexturePath = "";
string maskTexturePath = "";

bool noButtonClick;
int screenWidth = 1920;
int screenHeight = 1080;
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	//--
}
bool brushTextureChanged = true;
GLubyte* maskTexture;
void RigidPainter::run()
{
	ColorData colorData;
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
	glBufferData(GL_ARRAY_BUFFER, 1000, NULL, GL_DYNAMIC_DRAW);

	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	ui.setViewportBgColor();
	ui.uploadChars();


	glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

	glset.uniform3fv(commonData.program, "lightPos", lightPos);
	glset.uniform1f(commonData.program, "material.shininess", 32.0f);
	glset.uniform1i(commonData.program, "maskTexture", 4);
	glset.uniform3fv(commonData.program,"textColor",colorData.textColor);
	glset.uniform1i(commonData.program, "material.diffuse", 0);
	glset.uniform1i(commonData.program, "material.specular", 1);

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glset.activeTexture(GL_TEXTURE1);
	GetTextureData getTextureData;
	getTextureData = txtr.getTexture(maskTexturePath, 0, 0, false, 0);
	glset.bindTexture(getTextureData.Id);

	TextureData textureData;
	textureData = txtr.getTextureData(maskTexturePath.c_str());

	maskTexture = new GLubyte[textureData.width * textureData.height * textureData.channels];//Deleted
	maskTexture = txtr.getTextureFromProgram(GL_TEXTURE1, textureData.width, textureData.height, textureData.channels);
	delete(maskTexture);
	GLubyte* screenTexture = new GLubyte[1920 * 1080 * 3];//Deleted

	unsigned int screenPaintingTextureId = txtr.createScreenPaintTexture(screenTexture,window);
	delete(screenTexture);

	unsigned int FBO = glset.createScreenFrameBufferObject();

	glset.enable(GL_BLEND);
	glDepthFunc(GL_LESS);
	glset.enable(GL_DEPTH_TEST);

	glm::mat4 projection = glm::ortho(0.0f, 2.0f, -1.0f, 1.0f);
	glset.uniformMatrix4fv(commonData.program, "TextProjection", projection);

	glm::mat4 renderTextureProjection = glm::ortho(0.0f, 1.77777777778f, 0.0f, 1.0f);//1920 - 1080 -> 1.77777777778 - 1
	glset.uniformMatrix4fv(commonData.program, "renderTextureProjection", renderTextureProjection);

	glset.uniform1i(commonData.program, "uiMaskTexture", 1);
	
	bool clickTaken = false;

	glset.setMatrices();

	renderData.window = window;
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();
		glset.updateViewMatrix(callbackData.cameraPos, callbackData.originPos);

		renderData.panelLoc = callbackData.panelLoc;
		renderData.modelLoadFilePath = modelName;
		renderData.isAutoTriangulateHover = callbackData.autoTriangulateCheckBoxEnter;
		renderData.isAutoTriangulateChecked = autoTriangulateChecked;
		renderData.isbackfaceCullingHover = callbackData.backfaceCullingCheckBoxEnter;
		renderData.isbackfaceCullingChecked = backfaceCullingChecked;
		panelData.movePanel = callbackData.movePanel;

		glset.render(renderData, vertices, FBO, panelData, cameraPosChanged);
		drawBrushSizeIndicator();
		//Light Obj
		//MSHP.drawLigtObject(shaderProgram,lightPos);
		
		//Paint
		if (glfwGetMouseButton(window, 0) == GLFW_PRESS) {
			txtr.drawTexture(window, maskTexturePath, brushTextureChanged, screenPaintingTextureId, brushSizeRangeBarValue);
			paintingMode = true;
			brushTextureChanged = false;
		}

		if (glfwGetMouseButton(window, 0) == GLFW_PRESS && !clickTaken) {
			if (!callbackData.addImageButtonEnter && !callbackData.addMaskTextureButtonEnter && !callbackData.addPlaneButtonEnter && !callbackData.addSphereButtonEnter && !callbackData.autoTriangulateCheckBoxEnter && !callbackData.backfaceCullingCheckBoxEnter && !callbackData.brushSizeRangeBarEnter && !callbackData.loadModelButtonEnter && !callbackData.modelFilePathTextBoxEnter && !callbackData.modelPanelButtonEnter && !callbackData.paintingPanelButtonEnter && !callbackData.texturePanelButtonEnter) {
				noButtonClick = true;
			}
			else {
				noButtonClick = false;
			}
			clickTaken = true;
		}
		if (glfwGetMouseButton(window, 0) == GLFW_RELEASE && clickTaken) {
			clickTaken = false;
		}

		glfwSwapBuffers(window);
		brushSizeRangeBarEnter();
		uiActions();
		
		if (cameraPosChanged && paintingMode) {
			refreshScreenDrawingTexture();
			paintingMode = false;
		}
	}
	glfwDestroyWindow(window);
	glfwTerminate();
}

void uiActions() {
	if (!noButtonClick) {
		if (buttonGetInput) {
			if (glfwGetMouseButton(window, 0) == GLFW_PRESS) {
				buttonGetInput = false;
				buttonPressed = true;
			}
		}
		if (glfwGetMouseButton(window, 0) == GLFW_RELEASE) {
			buttonGetInput = true;
			if (buttonPressed) {
				//Check mouse hover
				if (callbackData.addMaskTextureButtonEnter)
					addMaskTextureButtonEnter();
				if (callbackData.modelFilePathTextBoxEnter)
					modelFilePathTextBoxEnter();
				if (callbackData.modelPanelButtonEnter)
					modelPanelButtonEnter();
				if (callbackData.texturePanelButtonEnter)
					texturePanelButtonEnter();
				if (callbackData.paintingPanelButtonEnter)
					paintingPanelButtonEnter();
				if (callbackData.addImageButtonEnter)
					addImageButtonEnter();
				if (callbackData.addPlaneButtonEnter)
					addPlaneButtonEnter();
				if (callbackData.addSphereButtonEnter)
					addSphereButtonEnter();
				if (callbackData.autoTriangulateCheckBoxEnter)
					autoTriangulateCheckBoxEnter();
				if (callbackData.backfaceCullingCheckBoxEnter)
					backfaceCullingCheckBox();
				if (callbackData.loadModelButtonEnter)
					loadModelButton();
			}
			buttonPressed = false;
		}
	}
}
void refreshScreenDrawingTexture(){
	CommonData commonData;
	GLubyte* screenTextureX = new GLubyte[1920 * 1080 * 3];//Deleted
	std::fill_n(screenTextureX, 1920 * 1080 * 3, 0);
	glset.activeTexture(GL_TEXTURE4);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 1920, 1080, GL_RGB, GL_UNSIGNED_BYTE, screenTextureX); //Refresh Screen Texture
	glset.generateMipmap();
	cameraPosChanged = false;
	glset.uniform3fv(commonData.program, "viewPos", callbackData.cameraPos);
	delete(screenTextureX);
}
string maskTexturePathCheck;
void addMaskTextureButtonEnter() {
	maskTexturePathCheck = utilities.openFileDialog();
	if (maskTexturePathCheck != "") {
		maskTexturePath = maskTexturePathCheck;
		brushTextureChanged = true;
		glset.activeTexture(GL_TEXTURE1);
		GetTextureData getTextureData;
		getTextureData = txtr.getTexture(maskTexturePath, 0, 0, false, 0);
		glset.bindTexture(getTextureData.Id);
	}
}
void brushSizeRangeBarEnter() {
	if (callbackData.brushSizeRangeBarEnter && !noButtonClick) {
		if (buttonGetInput) {
			if (glfwGetMouseButton(window, 0) == GLFW_PRESS) {
				buttonGetInput = false;
				buttonPressed = true;
				brushSizeRangeBarPressed = true;
			}
		}
		if (glfwGetMouseButton(window, 0) == GLFW_RELEASE) {
			buttonGetInput = true;
			if (buttonPressed) {
				brushSizeRangeBarPressed = false;

			}
			buttonPressed = false;
		}
	}
}
void modelFilePathTextBoxEnter() {
	modelFilePath = utilities.openFileDialog();
	modelName = utilities.getLastWordBySeparatingWithChar(modelFilePath, '\\');

}
void modelPanelButtonEnter() {
	modelPanelActive = true;
	texturePanelActive = false;
	paintingPanelActive = false;

	panelData.modelPanelActive = modelPanelActive;
	panelData.texturePanelActive = texturePanelActive;
	panelData.paintingPanelActive = paintingPanelActive;
}
void texturePanelButtonEnter() {
	modelPanelActive = false;
	texturePanelActive = true;
	paintingPanelActive = false;

	panelData.modelPanelActive = modelPanelActive;
	panelData.texturePanelActive = texturePanelActive;
	panelData.paintingPanelActive = paintingPanelActive;
}
void paintingPanelButtonEnter() {
	modelPanelActive = false;
	texturePanelActive = false;
	paintingPanelActive = true;

	panelData.modelPanelActive = modelPanelActive;
	panelData.texturePanelActive = texturePanelActive;
	panelData.paintingPanelActive = paintingPanelActive;
}
string albedoPathCheck;
void addImageButtonEnter() {
	albedoPathCheck = utilities.openFileDialog();
	if (albedoPathCheck != "") {
		albedoTexturePath = albedoPathCheck;

		glset.activeTexture(GL_TEXTURE0);
		GetTextureData getTextureData;
		getTextureData = txtr.getTexture(albedoTexturePath, 0, 0, false, 0);
		glset.bindTexture(getTextureData.Id);
	}
}
void addPlaneButtonEnter() {
	vector<float> planeVertices = {
		-1 , 0 , 1 , 0 , 0 , 0 , 1 , 0 ,
		1 , 0 , 1 , 1 , 0 , 0 , 1 , 0 ,
		1 , 0 , -1 , 1 , 1 , 0 , 1 , 0 ,
		-1 , 0 , 1 , 0 , 0 , 0 , 1 , 0 ,
		1 , 0 , -1 , 1 , 1 , 0 , 1 , 0 ,
		-1 , 0 , -1 , 0 , 1 , 0 , 1 , 0 
	};
	modelName = "plane.rigidefault";
	vertices = planeVertices;
	glset.bufferData(vertices);
}
void addSphereButtonEnter() {
	Sphere sphere;
	modelName = "sphere.rigidefault";
	vertices = sphere.getSphere();
	glset.bufferData(vertices);
}
void autoTriangulateCheckBoxEnter(){
	if (autoTriangulateChecked == false)
		autoTriangulateChecked = true;
	else
		autoTriangulateChecked = false;
}
void backfaceCullingCheckBox() {
	if (backfaceCullingChecked == false) {
		backfaceCullingChecked = true;
		enableBackfaceCulling = false;
		backfaceCulling = true;
	}
	else {
		enableBackfaceCulling = true;
		renderData.backfaceCulling = enableBackfaceCulling;
		backfaceCulling = true;
		backfaceCullingChecked = false;
	}
}

void loadModelButton() {
	if (modelName != "" && modelName[modelName.size() - 1] == 'j' && modelName[modelName.size() - 2] == 'b' && modelName[modelName.size() - 3] == 'o' && modelName[modelName.size() - 4] == '.') {
		vertices.clear();
		vertices = modelLoader.OBJ_getVertices(modelFilePath, autoTriangulateChecked);
		glset.bufferData(vertices);
	}
}
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	screenWidth = width;
	screenHeight = height;
	callback.framebuffer_size_callback(window,width,height);
}
void drawBrushSizeIndicator() {
	CommonData commonData;
	float distanceX = double(brushSizeRangeBarValue + 0.1f) * 500.0 + 10.0;
	std::vector<float> paintingSquare{
		// first triangle
		 distanceX / screenWidth / 1.0f + (float)mouseXpos / screenWidth / 0.5f - 1.0f,  distanceX / screenHeight / 1.0f - (float)mouseYpos / screenHeight / 0.5f + 1.0f , 1.0f,0,0,0,0,0,  // top right
		 distanceX / screenWidth / 1.0f + (float)mouseXpos / screenWidth / 0.5f - 1.0f, -distanceX / screenHeight / 1.0f - (float)mouseYpos / screenHeight / 0.5f + 1.0f , 1.0f,0,0,0,0,0,  // bottom right
		-distanceX / screenWidth / 1.0f + (float)mouseXpos / screenWidth / 0.5f - 1.0f,  distanceX / screenHeight / 1.0f - (float)mouseYpos / screenHeight / 0.5f + 1.0f , 1.0f,0,0,0,0,0,  // top left 

		 distanceX / screenWidth / 1.0f + (float)mouseXpos / screenWidth / 0.5f - 1.0f, -distanceX / screenHeight / 1.0f - (float)mouseYpos / screenHeight / 0.5f + 1.0f , 1.0f,0,0,0,0,0,  // bottom right
		-distanceX / screenWidth / 1.0f + (float)mouseXpos / screenWidth / 0.5f - 1.0f, -distanceX / screenHeight / 1.0f - (float)mouseYpos / screenHeight / 0.5f + 1.0f , 1.0f,0,0,0,0,0,  // bottom left
		-distanceX / screenWidth / 1.0f + (float)mouseXpos / screenWidth / 0.5f - 1.0f,  distanceX / screenHeight / 1.0f - (float)mouseYpos / screenHeight / 0.5f + 1.0f , 1.0f,0,0,0,0,0  // top left
	};
	glset.uniform3f(commonData.program, "uiColor", 1.0f, 1.0f, 1.0f);
	glset.uniform1f(commonData.program, "uiOpacity", 0.1f);
	glset.drawArrays(paintingSquare, false);
	glset.uniform1f(commonData.program, "uiOpacity", 0.5f);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (glfwGetMouseButton(window, 0) == GLFW_RELEASE) {
		brushSizeRangeBarPressed = false;
	}
	xOffset = lastXpos - xpos;
	lastXpos = xpos;

	if (brushSizeRangeBarPressed) {
		if (brushSizeRangeBarValue > 0.1f) {
			brushSizeRangeBarValue = 0.1f;
		}
		else if (brushSizeRangeBarValue < -0.1f) {
			brushSizeRangeBarValue = -0.1f;
		}
		brushSizeRangeBarValue -= xOffset / 960;
	}
	renderData.brushSizeRangeBarValue = brushSizeRangeBarValue;
	callbackData = callback.mouse_callback(window, xpos, ypos, modelPanelActive, texturePanelActive, paintingPanelActive, brushSizeRangeBarValue);
	if (callbackData.cameraPos != holdCameraPos) {
		cameraPosChanged = true;
	}
	holdCameraPos = callbackData.cameraPos;
}
void scroll_callback(GLFWwindow* window, double scroll, double scrollx)
{
	callbackData = callback.scroll_callback(window, scroll, scrollx);
	if (callbackData.cameraPos != holdCameraPos) {
		cameraPosChanged = true;
	}
	holdCameraPos = callbackData.cameraPos;
}