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
#include "TextureGenerator.h"

#include <vector>
#include <map>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STBI_MSC_SECURE_CRT
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

using namespace std;

GlSet glset;
GLFWwindow* window = glset.getWindow();

void framebuffer_size_callback(GLFWwindow* window, int width, int height);

unsigned int VBO, VAO; //Vertex Buffer Object, Vertex Array Object
bool cameraPosChanged = true;
bool paintingMode = false;
glm::vec3 holdCameraPos;

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
bool buttonGetInput = true;
bool buttonPressed = false;
bool autoTriangulateChecked = true;
bool backfaceCullingChecked = true;

string modelName;
vector<float> vertices = { 0 };

void loadModelButton();
void modelFilePathTextBox();
void autoTriangulateCheckBox();
void backfaceCullingCheckBox();
void addPlaneButton();
void addSphereButton();
void addImageButton();
void modelPanelButton();
void texturePanelButton();
void paintingPanelButton();
void addMaskTextureButton();
void brushSizeRangeBar();
void uiActions();
void scroll_callback(GLFWwindow* window, double scroll, double scrollx);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
RenderData updateRenderData(RenderData renderData);

//Texture
string albedoTexturePath = "";
string maskTexturePath = "";

CallbckData callbackData;
PanelData panelData;

bool noButtonClick;
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	//--
}
bool brushTextureChanged = true;
GLubyte* maskTexture;

bool brushSizeRangeBarPressed;
float brushSizeRangeBarValue = 0.0f;
bool RigidPainter::run()
{
	ColorData colorData;
	CommonData commonData;
	UserInterface ui;
	Texture txtr;

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

	glset.uniform1i(commonData.program, "uiMaskTexture", 1);
	
	bool clickTaken = false;

	glset.setMatrices();

	float brushSizeIndicatorDist;

	std::vector<float>axisPointer{
		0.0f, -100.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, //Y
		0.0f, 100.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 

		-100.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, //X
		100.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 

		0.0f, 0.0f, -100.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, //Z
		0.0f, 0.0f, 100.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 
	};
	RenderData renderData;
	TextureGenerator textureGen;
	renderData.window = window;

	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();
		glset.updateViewMatrix(callbackData.cameraPos, callbackData.originPos);

		renderData = updateRenderData(renderData);

		brushSizeIndicatorDist = double(brushSizeRangeBarValue + 0.1f) * 500.0 + 10.0 ;
		renderData.brushSizeIndicator = brushSizeIndicatorDist;

		panelData.movePanel = callbackData.movePanel;

		if (glfwGetMouseButton(window, 0) == GLFW_PRESS) {
			textureGen.drawTexture(window, maskTexturePath, brushTextureChanged, screenPaintingTextureId, brushSizeRangeBarValue);
			paintingMode = true;
			brushTextureChanged = false;
		}
		glset.render(renderData, vertices, FBO, panelData, cameraPosChanged,axisPointer);
		//Light Obj
		//MSHP.drawLigtObject(shaderProgram,lightPos);
		
		//Paint


		if (glfwGetMouseButton(window, 0) == GLFW_PRESS && !clickTaken) {
			if (!callbackData.addImageButtonEnter && !callbackData.addMaskTextureButtonEnter && !callbackData.addPlaneButtonEnter 
				&& !callbackData.addSphereButtonEnter && !callbackData.autoTriangulateCheckBoxEnter && !callbackData.backfaceCullingCheckBoxEnter 
				&& !callbackData.brushSizeRangeBarEnter && !callbackData.loadModelButtonEnter && !callbackData.modelFilePathTextBoxEnter 
				&& !callbackData.modelPanelButtonEnter && !callbackData.paintingPanelButtonEnter && !callbackData.texturePanelButtonEnter) {
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
		uiActions();
		if (cameraPosChanged) {
			glset.uniform3fv(commonData.program, "viewPos", callbackData.cameraPos);
		}
		if (cameraPosChanged && paintingMode) {
			txtr.refreshScreenDrawingTexture();
			cameraPosChanged = false;
			paintingMode = false;
		}
	}
	glfwDestroyWindow(window);
	glfwTerminate();
	return true;
}
RenderData updateRenderData(RenderData renderData) {
	renderData.panelLoc = callbackData.panelLoc;
	renderData.modelLoadFilePath = modelName;
	renderData.isAutoTriangulateHover = callbackData.autoTriangulateCheckBoxEnter;
	renderData.isAutoTriangulateChecked = autoTriangulateChecked;
	renderData.isbackfaceCullingHover = callbackData.backfaceCullingCheckBoxEnter;
	renderData.isbackfaceCullingChecked = backfaceCullingChecked;
	renderData.backfaceCulling = enableBackfaceCulling;
	renderData.brushSizeRangeBarValue = brushSizeRangeBarValue;
	return renderData;
}
//Ui Actions

void uiActions() {
	brushSizeRangeBar();
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
					addMaskTextureButton();
				if (callbackData.modelFilePathTextBoxEnter)
					modelFilePathTextBox();
				if (callbackData.modelPanelButtonEnter)
					modelPanelButton();
				if (callbackData.texturePanelButtonEnter)
					texturePanelButton();
				if (callbackData.paintingPanelButtonEnter)
					paintingPanelButton();
				if (callbackData.addImageButtonEnter)
					addImageButton();
				if (callbackData.addPlaneButtonEnter)
					addPlaneButton();
				if (callbackData.addSphereButtonEnter)
					addSphereButton();
				if (callbackData.autoTriangulateCheckBoxEnter)
					autoTriangulateCheckBox();
				if (callbackData.backfaceCullingCheckBoxEnter)
					backfaceCullingCheckBox();
				if (callbackData.loadModelButtonEnter)
					loadModelButton();
			}
			buttonPressed = false;
		}
	}
}

string maskTexturePathCheck;
void addMaskTextureButton() {
	Utilities utilities;
	GlSet glset;
	Texture txtr;
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
void brushSizeRangeBar() {
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
void modelFilePathTextBox() {
	Utilities utilities;
	modelFilePath = utilities.openFileDialog();
	modelName = utilities.getLastWordBySeparatingWithChar(modelFilePath, '\\');

}
void modelPanelButton() {
	panelData.modelPanelActive = true;
	panelData.texturePanelActive = false;
	panelData.paintingPanelActive = false;
}
void texturePanelButton() {
	panelData.modelPanelActive = false;
	panelData.texturePanelActive = true;
	panelData.paintingPanelActive = false;
}
void paintingPanelButton() {
	panelData.modelPanelActive = false;
	panelData.texturePanelActive = false;
	panelData.paintingPanelActive = true;
}
string albedoPathCheck;
void addImageButton() {
	Utilities utilities;
	GlSet glset;
	Texture txtr;
	albedoPathCheck = utilities.openFileDialog();
	if (albedoPathCheck != "") {
		albedoTexturePath = albedoPathCheck;

		glset.activeTexture(GL_TEXTURE0);
		GetTextureData getTextureData;
		getTextureData = txtr.getTexture(albedoTexturePath, 0, 0, false, 0);
		glset.bindTexture(getTextureData.Id);
	}
}
void addPlaneButton() {
	GlSet glset;
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
void addSphereButton() {
	GlSet glset;
	Sphere sphere;
	modelName = "sphere.rigidefault";
	vertices = sphere.getSphere();
	glset.bufferData(vertices);
}
void autoTriangulateCheckBox(){
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
		backfaceCulling = true;
		backfaceCullingChecked = false;
	}
}
void loadModelButton() {
	GlSet glset;
	ModelLoader modelLoader;
	if (modelName != "" && modelName[modelName.size() - 1] == 'j' && modelName[modelName.size() - 2] == 'b' && modelName[modelName.size() - 3] == 'o' && modelName[modelName.size() - 4] == '.') {
		vertices.clear();
		vertices = modelLoader.OBJ_getVertices(modelFilePath, autoTriangulateChecked);
		glset.bufferData(vertices);
	}
}
//Callback
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	Callback callback;
	callback.framebuffer_size_callback(window,width,height);
}

double lastXpos;
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	double xOffset;

	Callback callback;
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
	callbackData = callback.mouse_callback(window, xpos, ypos, panelData, brushSizeRangeBarValue);
	if (callbackData.cameraPos != holdCameraPos) {
		cameraPosChanged = true;
	}
	holdCameraPos = callbackData.cameraPos;
}

void scroll_callback(GLFWwindow* window, double scroll, double scrollx)
{
	Callback callback;
	callbackData = callback.scroll_callback(window, scroll, scrollx);
	if (callbackData.cameraPos != holdCameraPos) {
		cameraPosChanged = true;
	}
	holdCameraPos = callbackData.cameraPos;
}