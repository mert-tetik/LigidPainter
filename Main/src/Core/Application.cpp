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

#include "tinyfiledialogs.h"

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
const char* modelFilePath;

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
void exportPanelButton();
void addMaskTextureButton();
void brushSizeRangeBar();
void colorBoxColorRangeBar();
void colorBoxPickerButton();
void uiActions();
void scroll_callback(GLFWwindow* window, double scroll, double scrollx);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);

void exportPathTextBoxEnter();
void exportExtJPGButtonEnter();
void exportExtPNGButtonEnter();
void exportDownloadButtonEnter();

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
bool colorBoxColorRangeBarPressed;
bool colorBoxPickerPressed;

float brushSizeRangeBarValue = 0.0f;
float colorBoxColorRangeBarValue = 0.0f;
float colorBoxPickerValue_x = 0.0f;
float colorBoxPickerValue_y = 0.0f;

string exportPath = "";
string exportFolder = "Choose Destination Path";

bool jpgFormatChecked;
bool pngFormatChecked = true;

bool doPainting = true;

double holdScroll;
bool doScrollAfterCallInPaintingMode;

bool exportImage;

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
	glBufferData(GL_ARRAY_BUFFER, 10000, NULL, GL_DYNAMIC_DRAW);

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
	unsigned int FBOSq = glset.createScreenFrameBufferObjectSq();

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

	glset.activeTexture(GL_TEXTURE6);

	unsigned int paintingSqTxtr;
	Callback callback;
	ExportData exportData;

	glset.activeTexture(GL_TEXTURE9);

	unsigned int depthTexture;
	glset.genTextures(depthTexture);
	glset.bindTexture(depthTexture);
	while (!glfwWindowShouldClose(window))
	{
		if (callbackData.cameraPos != holdCameraPos) {
			cameraPosChanged = true;
		}
		holdCameraPos = callbackData.cameraPos;



		uiActions();

		glfwPollEvents();
		glset.updateViewMatrix(callbackData.cameraPos, callbackData.originPos);

		renderData = updateRenderData(renderData);

		brushSizeIndicatorDist = double(brushSizeRangeBarValue + 0.1f) * 500.0 + 10.0 ;
		renderData.brushSizeIndicator = brushSizeIndicatorDist;

		panelData.movePanel = callbackData.movePanel;

		if (glfwGetMouseButton(window, 0) == GLFW_PRESS && doPainting) {
			textureGen.drawTexture(window, maskTexturePath, brushTextureChanged, screenPaintingTextureId, brushSizeRangeBarValue);
			paintingMode = true;
			brushTextureChanged = false;
		}

		if (colorBoxPickerValue_x > 0.095f) {
			colorBoxPickerValue_x = 0.095f;
		}
		else if (colorBoxPickerValue_x < -0.095f) {
			colorBoxPickerValue_x = -0.095f;
		}

		if (colorBoxPickerValue_y > 0.195f) {
			colorBoxPickerValue_y = 0.195f;
		}
		else if (colorBoxPickerValue_y < -0.195f) {
			colorBoxPickerValue_y = -0.195f;
		}

		if (colorBoxColorRangeBarValue > 0.195f) {
			colorBoxColorRangeBarValue = 0.195f;
		}
		else if (colorBoxColorRangeBarValue < -0.195f) {
			colorBoxColorRangeBarValue = -0.195f;
		}
		exportData.exportImage = exportImage;
		exportData.path = exportPath.c_str();

		glset.render(renderData, vertices, FBO, panelData, cameraPosChanged,axisPointer, colorBoxPickerValue_x, colorBoxPickerValue_y, 0, colorBoxColorRangeBarValue,exportFolder.c_str(),jpgFormatChecked,pngFormatChecked,callbackData.exportExtJPGButtonEnter,callbackData.exportExtPNGButtonEnter,doPainting,exportData, depthTexture,FBOSq);
		exportImage = false;
		//Light Obj
		//MSHP.drawLigtObject(shaderProgram,lightPos);
		
		//Paint


		if (glfwGetMouseButton(window, 0) == GLFW_PRESS && !clickTaken) {
			if (!callbackData.addImageButtonEnter && !callbackData.addMaskTextureButtonEnter && !callbackData.addPlaneButtonEnter 
				&& !callbackData.addSphereButtonEnter && !callbackData.autoTriangulateCheckBoxEnter && !callbackData.backfaceCullingCheckBoxEnter 
				&& !callbackData.brushSizeRangeBarEnter && !callbackData.loadModelButtonEnter && !callbackData.modelFilePathTextBoxEnter 
				&& !callbackData.modelPanelButtonEnter && !callbackData.paintingPanelButtonEnter && !callbackData.exportPanelButtonEnter && !callbackData.texturePanelButtonEnter && !callbackData.colorBoxPickerEnter
				&& !callbackData.colorBoxColorRangeBarEnter && !callbackData.exportPathTextBoxEnter && !callbackData.exportDownloadButtonEnter && !callbackData.exportExtJPGButtonEnter && !callbackData.exportExtPNGButtonEnter) {
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
		if (cameraPosChanged) {
			glset.uniform3fv(commonData.program, "viewPos", callbackData.cameraPos);
		}
		if (cameraPosChanged && paintingMode) {
			txtr.refreshScreenDrawingTexture();
			paintingMode = false;
		}
		if (doScrollAfterCallInPaintingMode)
		{
			callbackData = callback.scroll_callback(window, 0, holdScroll);
			doScrollAfterCallInPaintingMode = false;
		}
		cameraPosChanged = false;
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
	colorBoxPickerButton();
	brushSizeRangeBar();
	colorBoxColorRangeBar();
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
				if (callbackData.exportPanelButtonEnter)
					exportPanelButton();
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
				if (callbackData.exportPathTextBoxEnter) 
					exportPathTextBoxEnter();
				if (callbackData.exportDownloadButtonEnter)
					exportDownloadButtonEnter();
				if (callbackData.exportExtJPGButtonEnter)
					exportExtJPGButtonEnter();
				if (callbackData.exportExtPNGButtonEnter)
					exportExtPNGButtonEnter();
			}
			buttonPressed = false;
		}
	}
}

void addMaskTextureButton() {
	GlSet glset;
	Texture txtr;
	char const* lFilterPatterns[2] = { "*.jpg", "*.png" };
	auto maskTexturePathCheck = tinyfd_openFileDialog("Select Mask Texture", "", 2, lFilterPatterns, "", false);
	if (maskTexturePathCheck) {
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
void colorBoxColorRangeBar() {
	if (callbackData.colorBoxColorRangeBarEnter && !noButtonClick) {
		if (buttonGetInput) {
			if (glfwGetMouseButton(window, 0) == GLFW_PRESS) {
				buttonGetInput = false;
				buttonPressed = true;
				colorBoxColorRangeBarPressed = true;
			}
		}
		if (glfwGetMouseButton(window, 0) == GLFW_RELEASE) {
			buttonGetInput = true;
			if (buttonPressed) {
				colorBoxColorRangeBarPressed = false;
			}
			buttonPressed = false;
		}
	}
}
void colorBoxPickerButton() {
	if (callbackData.colorBoxPickerEnter && !noButtonClick) {

		if (buttonGetInput) {
			if (glfwGetMouseButton(window, 0) == GLFW_PRESS) {
				Texture txtr;
				buttonGetInput = false;
				buttonPressed = true;
				colorBoxPickerPressed = true;
				cameraPosChanged = true;
			}
		}
		if (glfwGetMouseButton(window, 0) == GLFW_RELEASE) {
			buttonGetInput = true;
			if (buttonPressed) {
				colorBoxPickerPressed = false;
			}
			buttonPressed = false;
		}
	}
}

void modelFilePathTextBox() {
	Utilities utilities;
	char const* lFilterPatterns[1] = { "*.obj" };
	auto modelFilePathCheck = tinyfd_openFileDialog("Select 3D Model","",1, lFilterPatterns,"",false);
	if (modelFilePathCheck) {
		modelFilePath = modelFilePathCheck;
		modelName = utilities.getLastWordBySeparatingWithChar(modelFilePath,'\\');
	}
}

void exportPathTextBoxEnter() {
	Utilities uti;
	string exportPathCheck = "";
	Texture txtr;
	TextureData txtrData;
	exportPathCheck = tinyfd_selectFolderDialog("Save Texture","");
	if (exportPathCheck != "") {
		exportPath = exportPathCheck;
		exportFolder = uti.getLastWordBySeparatingWithChar(exportPath,'\\');
	}
}
void exportExtJPGButtonEnter() {
	if (jpgFormatChecked == false) {
		jpgFormatChecked = true;
		pngFormatChecked = false;
	}
	else {
		jpgFormatChecked = false;
	}
}
void exportExtPNGButtonEnter() {
	if (pngFormatChecked == false) {
		pngFormatChecked = true;
		jpgFormatChecked = false;
	}
	else {
		pngFormatChecked = false;
	}
}
void exportDownloadButtonEnter() {
	exportImage = true;
}

void modelPanelButton() {
	panelData.modelPanelActive = true;
	panelData.texturePanelActive = false;
	panelData.paintingPanelActive = false;
	panelData.exportPanelActive = false;
	doPainting = false;
}
void texturePanelButton() {
	panelData.modelPanelActive = false;
	panelData.texturePanelActive = true;
	panelData.paintingPanelActive = false;
	panelData.exportPanelActive = false;
	doPainting = false;
}
void paintingPanelButton() {
	panelData.modelPanelActive = false;
	panelData.texturePanelActive = false;
	panelData.paintingPanelActive = true;
	panelData.exportPanelActive = false;
	doPainting = true;
}
void exportPanelButton() {
	panelData.modelPanelActive = false;
	panelData.texturePanelActive = false;
	panelData.paintingPanelActive = false;
	panelData.exportPanelActive = true;
	doPainting = false;
}
void addImageButton() {
	Utilities utilities;
	GlSet glset;
	Texture txtr;
	char const* lFilterPatterns[2] = { "*.jpg", "*.png" };
	auto albedoPathCheck = tinyfd_openFileDialog("Select Image", "", 2, lFilterPatterns, "", false);
	if (albedoPathCheck) {
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
	glBufferData(GL_ARRAY_BUFFER, 10000, NULL, GL_DYNAMIC_DRAW);
	
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
	Texture txtr;
	txtr.refreshScreenDrawingTexture();
	GlSet glset;
	ModelLoader modelLoader;
	if (modelName != "" && modelName[modelName.size() - 1] == 'j' && modelName[modelName.size() - 2] == 'b' && modelName[modelName.size() - 3] == 'o' && modelName[modelName.size() - 4] == '.') {
		vertices.clear();
		vertices = modelLoader.OBJ_getVertices(modelFilePath, autoTriangulateChecked);
		if(vertices.size() > 10000)
			glset.bufferData(vertices);
		else {
			glBufferData(GL_ARRAY_BUFFER, 10000, NULL, GL_DYNAMIC_DRAW);
		}
	}
}
//Callback
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	Callback callback;
	callback.framebuffer_size_callback(window,width,height);
}

double lastXpos;
double lastYpos;
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	double xOffset;
	double yOffset;
	
	int width;
	int height;
	glfwGetWindowSize(window,&width,&height);

	Callback callback;
	if (glfwGetMouseButton(window, 0) == GLFW_RELEASE) {
		brushSizeRangeBarPressed = false;
		colorBoxPickerPressed = false;
		colorBoxColorRangeBarPressed = false;
	}
	xOffset = (lastXpos - xpos) / (1920 / width);
	lastXpos = xpos;
	yOffset = (lastYpos - ypos) / (1080 / height);
	lastYpos = ypos;



	if (xpos > callbackData.panelLoc * 960 && !callbackData.brushSizeRangeBarEnter && !callbackData.colorBoxColorRangeBarEnter && !callbackData.colorBoxPickerEnter)
		doPainting = false;
	else if(xpos < callbackData.panelLoc * 960 && panelData.paintingPanelActive)
		doPainting = true;
	if (callbackData.brushSizeRangeBarEnter || callbackData.colorBoxColorRangeBarEnter || callbackData.colorBoxPickerEnter)
		doPainting = true;

	if (brushSizeRangeBarPressed) {
		if (brushSizeRangeBarValue > 0.1f) {
			brushSizeRangeBarValue = 0.1f;
		}
		else if (brushSizeRangeBarValue < -0.1f) {
			brushSizeRangeBarValue = -0.1f;
		}
		brushSizeRangeBarValue -= xOffset / 960;
		doPainting = true;
	}
	if (colorBoxColorRangeBarPressed) {
		colorBoxColorRangeBarValue += yOffset / 540;
		doPainting = true;
	}

	//Change color box picker value-location
	if (colorBoxPickerPressed) {
		colorBoxPickerValue_x -= xOffset / 960;
		colorBoxPickerValue_y += yOffset / 540;
		doPainting = true;
	}
	callbackData = callback.mouse_callback(window, xpos, ypos, panelData, brushSizeRangeBarValue, colorBoxPickerValue_x, colorBoxPickerValue_y, colorBoxColorRangeBarValue);
}

void scroll_callback(GLFWwindow* window, double scroll, double scrollx)
{
	Callback callback;
	if (!paintingMode) {
		callbackData = callback.scroll_callback(window, scroll, scrollx);
	}
	else {
		holdScroll = scrollx;
		doScrollAfterCallInPaintingMode = true;
	}
	cameraPosChanged = true;

}