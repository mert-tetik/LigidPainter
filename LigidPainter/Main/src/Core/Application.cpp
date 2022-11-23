#include<iostream>
#include "../../thirdparty/include/glad/glad.h"
#include "../../thirdparty/include/GLFW/glfw3.h"
#include <string>
#include <fstream>
#include <sstream>
#include "../../thirdparty/include/glm/glm.hpp"
#include "../../thirdparty/include/glm/gtc/matrix_transform.hpp"
#include "../../thirdparty/include/glm/gtc/type_ptr.hpp"
#include "../../thirdparty/include/glm/gtx/string_cast.hpp"
#include "Core/LigidPainter.h"
#include "Core/ModelRead.h"
#include "UserInterface.h"
#include "Utilities.h"
#include "Callback.h"
#include "gl.h"
#include "Texture.h"
#include "Texture Generator/TextureGenerator.h"

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

//GL_TEXTURE0 = Albedo texture
//GL_TEXTURE1 = Mask texture
//GL_TEXTURE2 = Chars
//GL_TEXTURE3 = Mirrored Screen Mask Painting Texture
//GL_TEXTURE4 = Screen Mask Painting Texture
//GL_TEXTURE5 = 1920x1080 Screen Texture
//GL_TEXTURE6 = NULL
//GL_TEXTURE7 = UV mask texture (used for painting over boundaries)
//GL_TEXTURE8 = Mirrored Depth texture
//GL_TEXTURE9 = Depth texture
//GL_TEXTURE10 = 1080x1080 Screen Texture
//GL_TEXTURE11 = NULL
//GL_TEXTURE12 = Modified mask texture
//GL_TEXTURE13 = NULL

unsigned int VBO, VAO, FBOScreen; //Vertex Buffer Object, Vertex Array Object, Framebuffer object that I used to render the screen

bool cameraPosChanging = true;
bool paintingMode = false; //True if painting started, False if camera position changed after painting
glm::vec3 holdCameraPos; //Used to detect the camera position change

//Paths
const char* modelFilePath;
string albedoTexturePath = "albedoImage.png";
string maskTexturePath = "./LigidPainter/Resources/Textures/PlainCircle.png";
string exportPath = "";
string exportFolder = "Choose Destination Path";
//Paths

string modelName;
vector<float> vertices = { 0 };

//--------Functions--------\\

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void scroll_callback(GLFWwindow* window, double scroll, double scrollx);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void updateCameraPosChanging();
RenderData updateRenderData(RenderData renderData, unsigned int depthTexture, int brushSizeIndicatorSize);
UiData updateUiData();

//--------Functions--------\\

CallbckData callbackData;
PanelData panelData;

bool doPainting = true;//Prevent painting

double holdScrollVal; //Hold scroll data to call the scroll function in while loop
bool doScrollAfterCallInPaintingMode; //Prevent painting size change after scrolling

bool exportImage;//To let render function know if exporting called

//Update once the mouse location value changed
double mouseXpos; 
double mouseYpos;
bool mousePosChanged = false;
//Update once the mouse location value changed

//Let program know if brush needs an update
bool brushSizeChanged = false;
bool brushBlurChanged = false;
bool brushRotationChanged = false;
//bool brushOpacityChanged = false; not used
bool brushTextureChanged = true;
//Let program know if brush needs an update

//-----------------------      UI     -----------------------\\

void uiActions(); //Associated with buttonPressed & buttonGetInput booleans

void isFirstClickDoneInside(); //Associated with noButtonClick boolean

//Button control
bool noButtonClick;
bool buttonGetInput = true;
bool buttonPressed = false;
//Button control

bool enablePanelMovement = true; //Panel can be moved if true. Set false while dragging range bar pointers around.

//----------PRESSED----------\\
Might be transported to callback
//(hover calculations are made in callback.cpp)

//Used to let render function know if it's supposed to change colors
void setButtonPressedFalse();
bool loadModelButtonPressed = false;
bool addPlaneButtonPressed = false;
bool addSphereButtonPressed = false;
bool addImageButtonPressed = false;
bool addMaskTextureButtonPressed = false;
bool exportDownloadButtonPressed = false;

//Used to let mouse callback function know if it's supposed to change range bar values
bool brushSizeRangeBarPressed;
bool brushBlurRangeBarPressed;
bool brushRotationRangeBarPressed;
bool brushOpacityRangeBarPressed;
bool brushSpacingRangeBarPressed;
bool colorBoxColorRangeBarPressed;
bool colorBoxPickerPressed;

bool textureDemonstratorButtonPressed;
bool textureDemonstratorBoundariesPressed;

//Checkbox
bool autoTriangulateChecked = true;

bool backfaceCullingChecked = true;
bool enableBackfaceCulling;

bool jpgFormatChecked;
bool pngFormatChecked = true;

bool mirrorXCheckBoxChecked = false;
bool mirrorYCheckBoxChecked = false;
bool mirrorZCheckBoxChecked = false;
bool mirrorUsed = false;
//Checkbox
//----------PRESSED----------\\


//----------RANGE VALUE----------\\.
float brushSizeRangeBarValue = 0.0f;
float brushBlurRangeBarValue = -0.11f;
float brushRotationRangeBarValue = -0.11f;
float brushOpacityRangeBarValue = 0.11f;
float brushSpacingRangeBarValue = -0.11f;
float colorBoxColorRangeBarValue = 0.0f;
float colorBoxPickerValue_x = 0.0f;
float colorBoxPickerValue_y = 0.0f;

float textureDemonstratorButtonPosX = 0.0;
float textureDemonstratorButtonPosY = 0.0;
//----------RANGE VALUE----------\\.

//----------ACTIONS----------\\.
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
void exportDownloadButtonEnter();
void exportPathTextBox();
void exportExtJPGCheckBox();
void exportExtPNGCheckBox();
void mirrorXCheckBox();
void mirrorYCheckBox();
void mirrorZCheckBox();
void colorBoxColorRangeBar(float yOffset, int height);
void colorBoxPickerButton(float xOffset, float yOffset, int width, int height);
void brushSizeRangeBar(float xOffset, int width);
void brushBlurRangeBar(float xOffset, int width, int height);
void brushRotationRangeBar(float xOffset, int width, int height);
void brushOpacityRangeBar(float xOffset, int width, int height);
void brushSpacingRangeBar(float xOffset, int width, int height);
void textureDemonstratorButton(float xOffset,float yOffset,int width,int height);
void textureDemonstratorBoundaries(float xOffset,float yOffset,int width,int height);
//----------ACTIONS----------\\
//-----------------------      UI     -----------------------\\

//bool albedoTextureChanged; use for texture updating conditions

int drawingSpacing = 1;
int drawingCount; // if drawingCount matches with drawingSpaces do painting

int lastMouseXpos = 0;
int lastMouseYpos = 0;

int textureDemonstratorButtonPressCounter = 0;
bool textureDemonstratorButtonPressClicked = false;
float textureDemonstratorWidth = 0.4f;
float textureDemonstratorHeight = 0.8f;
bool textureDemonstratorBoundariesHover = false;
//bool textureDemonstratorBoundariesHoverR = false;
//bool textureDemonstratorBoundariesHoverB = false;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	//Will be used for allowing writing to a text box
	//Can be deleted since program doesn't use it
}
bool LigidPainter::run()
{
	ColorData colorData;
	CommonData commonData;
	UserInterface ui;
	Texture txtr;
	RenderData renderData;
	UiData uidata;
	TextureGenerator textureGen;
	Callback callback;

	//Set Callbacks
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback); 
	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	//Set Callbacks

	glset.getProgram();
	glGenBuffers(1, &VBO);
	//glGenVertexArrays(1, &VAO);
	//glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glUseProgram(commonData.program);

	renderData.window = window;

	glset.setVertexAtribPointer();
	glBufferData(GL_ARRAY_BUFFER, 10000, NULL, GL_DYNAMIC_DRAW); 

	ui.setViewportBgColor();
	ui.uploadChars();

	glm::vec3 lightPos(1.2f, 1.0f, 2.0f);
	glset.uniform1i(commonData.program, "uvMask", 7);//use enlarged texture
	glset.uniform3fv(commonData.program, "lightPos", lightPos);
	glset.uniform1f(commonData.program, "material.shininess", 32.0f);
	glset.uniform1i(commonData.program, "screenMaskTexture", 4);
	glset.uniform1i(commonData.program, "mirroredScreenMaskTexture", 3);
	glset.uniform3fv(commonData.program,"textColor",colorData.textColor);
	glset.uniform1i(commonData.program, "material.diffuse", 0);
	glset.uniform1i(commonData.program, "material.specular", 1);
	glset.uniform1f(commonData.program, "brushBlurVal", 1);
	glset.uniform1i(commonData.program, "modifiedMaskTexture", 12);

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); //Wireframe

	//Screen Texture
	GLubyte* screenTexture = new GLubyte[1920 * 1080 * 3];
	ScreenPaintingReturnData screenPaintingReturnData; 

	screenPaintingReturnData = txtr.createScreenPaintTexture(screenTexture,window);

	delete(screenTexture);
	//Screen Texture

	//Create frame buffers for getting screen texture
	FBOScreen = glset.createScreenFrameBufferObject(); //1920x1080 get all the screen
	//Create frame buffers for getting screen texture

	glset.enable(GL_BLEND);

	glDepthFunc(GL_LESS);
	glset.enable(GL_DEPTH_TEST);

	glset.setMatrices(); //View matrix, Projection matrix

	float brushSize;
	ExportData exportData;

	//------Set Textures------\\

	glset.activeTexture(GL_TEXTURE9);
	unsigned int depthTexture;
	glset.genTextures(depthTexture);
	glset.bindTexture(depthTexture);

	glset.activeTexture(GL_TEXTURE8);
	unsigned int mirroredDepthTexture;
	glset.genTextures(mirroredDepthTexture);
	glset.bindTexture(mirroredDepthTexture);


	glset.activeTexture(GL_TEXTURE7);//Albedo
	unsigned int enlargedTexture;
	glset.genTextures(enlargedTexture);
	glset.bindTexture(enlargedTexture);

	glset.activeTexture(GL_TEXTURE12);
	unsigned int modifiedMaskTexture;
	glset.genTextures(modifiedMaskTexture);
	glset.bindTexture(modifiedMaskTexture);

	glset.activeTexture(GL_TEXTURE1);//Raw mask
	GetTextureData getTextureData;
	getTextureData = txtr.getTexture(maskTexturePath,0,0);
	glset.activeTexture(GL_TEXTURE12);//Modified mask
	getTextureData = txtr.getTexture(maskTexturePath,0,0);
	
	//------Set Textures------\\

	int screenWidth;
	int screenHeight;
	glfwGetWindowSize(window, &screenWidth, &screenHeight);
	txtr.updateMaskTexture(FBOScreen, screenWidth, screenHeight, brushRotationRangeBarValue);

	panelData.modelPanelActive = true; //Active panel by default

	Utilities util;
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();

		//util.printRenderingSpeed();

		updateCameraPosChanging();

		uiActions();

		//Update
		glset.updateViewMatrix(callbackData.cameraPos, callbackData.originPos,mirrorXCheckBoxChecked,mirrorYCheckBoxChecked,mirrorZCheckBoxChecked);
		brushSize = double(brushSizeRangeBarValue + 0.1f) * 800.0 + 20.0 ;
		renderData = updateRenderData(renderData,depthTexture, brushSize);
		uidata = updateUiData();
		panelData.movePanel = callbackData.movePanel;
		exportData.exportImage = exportImage;
		exportData.path = exportPath.c_str();

		if (242 - ((brushBlurRangeBarValue + 0.1f) * 1000.0) - 15 > 200){ //If the range bar value is low enough disable blur effect
			glset.uniform1f(commonData.program, "brushBlurVal", 1000);
		}
		else {
			glset.uniform1f(commonData.program, "brushBlurVal", 242 - ((brushBlurRangeBarValue + 0.1f) * 1000.0) - 15);
		}

		if(textureDemonstratorButtonPressed){
			textureDemonstratorButtonPressCounter++;
		}
		if(textureDemonstratorButtonPressCounter < 40 && textureDemonstratorButtonPressed && glfwGetMouseButton(window, 0) == GLFW_RELEASE){
			textureDemonstratorButtonPressClicked = true;
		}
		if(glfwGetMouseButton(window, 0) == GLFW_RELEASE){
			textureDemonstratorButtonPressCounter = 0;
			if (brushSizeRangeBarPressed){
				brushSizeChanged = true;
			}
			brushSizeRangeBarPressed = false;
			brushBlurRangeBarPressed = false;
			brushRotationRangeBarPressed = false;
			brushOpacityRangeBarPressed = false;
			brushSpacingRangeBarPressed = false;
			colorBoxPickerPressed = false;
			colorBoxColorRangeBarPressed = false;
			textureDemonstratorButtonPressed = false;
			textureDemonstratorBoundariesPressed = false;
		}


		//Update

		//Paint
		if (glfwGetMouseButton(window, 0) == GLFW_PRESS && doPainting) {
			drawingCount++;
		}
		if (glfwGetMouseButton(window, 0) == GLFW_PRESS && doPainting && drawingCount == drawingSpacing){
			textureGen.drawToScreen(window, maskTexturePath, brushTextureChanged, screenPaintingReturnData, brushSize, FBOScreen, brushBlurChanged, brushSizeChanged,brushRotationRangeBarValue, brushRotationChanged,brushOpacityRangeBarValue,lastMouseXpos, lastMouseYpos,mouseXpos,mouseYpos,mirrorUsed);

			drawingCount = 0;

			brushSizeChanged = false;
			brushBlurChanged = false;
			brushRotationChanged = false;
			//brushOpacityChanged = false; not used
			paintingMode = true;
			brushTextureChanged = false;
		}
					lastMouseXpos = mouseXpos;
			lastMouseYpos = mouseYpos;
		//Paint

		glset.render(renderData, vertices, FBOScreen, panelData,exportData,uidata,textureDemonstratorButtonPosX,textureDemonstratorButtonPosY,textureDemonstratorButtonPressClicked,textureDemonstratorWidth,textureDemonstratorHeight,textureDemonstratorBoundariesPressed);
		exportImage = false; //After exporting, set exportImage false so we won't download the texture repeatedly

		textureDemonstratorButtonPressClicked = false;

		setButtonPressedFalse();

		if (mousePosChanged) { //To make sure painting done before changing camera position
			callbackData = callback.mouse_callback(window, mouseXpos, mouseYpos, panelData, brushSizeRangeBarValue, colorBoxPickerValue_x, colorBoxPickerValue_y, colorBoxColorRangeBarValue, brushBlurRangeBarValue, enablePanelMovement,brushRotationRangeBarValue, brushOpacityRangeBarValue, brushSpacingRangeBarValue,textureDemonstratorButtonPosX,textureDemonstratorButtonPosY);
		}

		if (cameraPosChanging) { //Change the position of the camera in the shaders once camera position changed
			glset.uniform3fv(commonData.program, "viewPos", callbackData.cameraPos);
		}
		if ((cameraPosChanging && paintingMode) || glfwGetMouseButton(renderData.window, 0) == GLFW_RELEASE) { //Changing camera position or painting a stroke ends painting, than updates painted texture
			paintingMode = false;
		}
		if (doScrollAfterCallInPaintingMode) //Do a scroll after texture update using holdScroll
		{
			callbackData = callback.scroll_callback(window, 0, holdScrollVal);
			doScrollAfterCallInPaintingMode = false;
		}
		if (glfwGetMouseButton(window, 1) == GLFW_RELEASE) { //Camera position changed
			cameraPosChanging = false;
		}

		glfwSwapBuffers(window);
	}
	
	glfwDestroyWindow(window);
	glfwTerminate();
	return true;
}
RenderData updateRenderData(RenderData renderData,unsigned int depthTexture,int brushSizeIndicatorSize) {
	renderData.panelLoc = callbackData.panelLoc;
	renderData.modelLoadFilePath = modelName;
	renderData.backfaceCulling = enableBackfaceCulling;
	renderData.brushSizeRangeBarValue = brushSizeRangeBarValue;
	renderData.colorBoxPickerValue_x = colorBoxPickerValue_x;
	renderData.colorBoxPickerValue_y = colorBoxPickerValue_y;
	renderData.colorBoxColorRangeBarValue = colorBoxColorRangeBarValue;
	renderData.exportFolder = exportFolder.c_str();
	renderData.doPainting = doPainting;
	renderData.depthTexture = depthTexture;
	renderData.paintingMode = paintingMode;
	renderData.brushSizeIndicator = brushSizeIndicatorSize;
	renderData.cameraPosChanged = cameraPosChanging;
	renderData.brushBlurRangeBarValue = brushBlurRangeBarValue;
	renderData.brushRotationRangeBarValue = brushRotationRangeBarValue;
	renderData.brushOpacityRangeBarValue = brushOpacityRangeBarValue;
	renderData.brushSpacingRangeBarValue = brushSpacingRangeBarValue;
	
	renderData.textureDemonstratorButtonPosX = textureDemonstratorButtonPosX;
	renderData.textureDemonstratorButtonPosY = textureDemonstratorButtonPosY;

	return renderData;
}
UiData updateUiData() {
	UiData uidata; //gl.h UiData structure
	
	uidata.loadModelButtonEnter = callbackData.loadModelButtonEnter;
	uidata.loadModelButtonPressed;
	
	uidata.modelFilePathTextBoxEnter = callbackData.modelFilePathTextBoxEnter;
	
	uidata.autoTriangulateCheckBoxEnter = callbackData.autoTriangulateCheckBoxEnter;
	uidata.autoTriangulateCheckBoxPressed = autoTriangulateChecked;
	
	uidata.backfaceCullingCheckBoxEnter = callbackData.backfaceCullingCheckBoxEnter;
	uidata.backfaceCullingCheckBoxPressed = backfaceCullingChecked;
	
	uidata.addPlaneButtonEnter = callbackData.addPlaneButtonEnter;
	uidata.addPlaneButtonPressed;
	
	uidata.addSphereButtonEnter = callbackData.addSphereButtonEnter;
	uidata.addSphereButtonPressed;
	
	uidata.addImageButtonEnter = callbackData.addImageButtonEnter;
	uidata.addImageButtonPressed = addImageButtonPressed;
	
	uidata.addMaskTextureButtonEnter = callbackData.addMaskTextureButtonEnter;
	uidata.addMaskTextureButtonPressed;
	
	uidata.brushSizeRangeBarEnter = callbackData.brushSizeRangeBarEnter;
	
	uidata.brushBlurRangeBarEnter = callbackData.brushBlurRangeBarEnter;

	uidata.brushRotationRangeBarEnter = callbackData.brushRotationRangeBarEnter;

	uidata.brushOpacityRangeBarEnter = callbackData.brushOpacityRangeBarEnter;

	uidata.brushSpacingRangeBarEnter = callbackData.brushSpacingRangeBarEnter;
	
	uidata.colorBoxPickerEnter = callbackData.colorBoxPickerEnter;
	
	uidata.colorBoxColorRangeBarEnter = callbackData.colorBoxColorRangeBarEnter;
	
	uidata.exportPathTextBoxEnter = callbackData.exportPathTextBoxEnter;
	
	uidata.exportExtJPGCheckBoxEnter = callbackData.exportExtJPGCheckBoxEnter;
	uidata.exportExtJPGCheckBoxPressed = jpgFormatChecked;

	uidata.exportExtPNGCheckBoxEnter = callbackData.exportExtPNGCheckBoxEnter;
	uidata.exportExtPNGCheckBoxPressed = pngFormatChecked;

	uidata.mirrorXCheckBoxEnter = callbackData.mirrorXCheckBoxEnter;
	uidata.mirrorXCheckBoxPressed = mirrorXCheckBoxChecked;
	uidata.mirrorYCheckBoxEnter = callbackData.mirrorYCheckBoxEnter;
	uidata.mirrorYCheckBoxPressed = mirrorYCheckBoxChecked;
	uidata.mirrorZCheckBoxEnter = callbackData.mirrorZCheckBoxEnter;
	uidata.mirrorZCheckBoxPressed = mirrorZCheckBoxChecked;

	uidata.exportDownloadButtonEnter = callbackData.exportDownloadButtonEnter;
	uidata.exportDownloadButtonPressed;

	return uidata;
}
//-------------UI ACTIONS-------------\\

void uiActions() {
	isFirstClickDoneInside();
	if (!noButtonClick) {

		if (buttonGetInput) {
			if (glfwGetMouseButton(window, 0) == GLFW_PRESS) {
				buttonGetInput = false;
				buttonPressed = true;
				if (callbackData.brushSizeRangeBarEnter) {
					brushSizeRangeBarPressed = true;
				}
				if (callbackData.brushBlurRangeBarEnter) {
					brushBlurRangeBarPressed = true;
				}
				if (callbackData.brushRotationRangeBarEnter) {
					brushRotationRangeBarPressed = true;
				}
				if (callbackData.brushOpacityRangeBarEnter) {
					brushOpacityRangeBarPressed = true;
				}
				if (callbackData.brushSpacingRangeBarEnter) {
					brushSpacingRangeBarPressed = true;
				}
				if (callbackData.colorBoxPickerEnter) {
					colorBoxPickerPressed = true;
				}
				if (callbackData.colorBoxColorRangeBarEnter) {
					colorBoxColorRangeBarPressed= true;
				}
				if (callbackData.textureDemonstratorButtonEnter) {
					textureDemonstratorButtonPressed = true;
				}
				if (textureDemonstratorBoundariesHover) {
					textureDemonstratorBoundariesPressed = true;
				}
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
					exportPathTextBox();
				if (callbackData.exportDownloadButtonEnter)
					exportDownloadButtonEnter();
				if (callbackData.exportExtJPGCheckBoxEnter)
					exportExtJPGCheckBox();
				if (callbackData.exportExtPNGCheckBoxEnter)
					exportExtPNGCheckBox();
				if (callbackData.mirrorXCheckBoxEnter)
					mirrorXCheckBox();
				if (callbackData.mirrorYCheckBoxEnter)
					mirrorYCheckBox();
				if (callbackData.mirrorZCheckBoxEnter)
					mirrorZCheckBox();
			}
			buttonPressed = false;
		}
	}
}

void addMaskTextureButton() {
	//Needed for updating mask texture
	int width;
	int height;
	glfwGetWindowSize(window, &width, &height);
	//
	addMaskTextureButtonPressed = true;
	GlSet glset;
	Texture txtr;
	char const* lFilterPatterns[2] = { "*.jpg", "*.png" };
	auto maskTexturePathCheck = tinyfd_openFileDialog("Select Mask Texture", "", 2, lFilterPatterns, "", false);
	if (maskTexturePathCheck) {
		maskTexturePath = maskTexturePathCheck;
		brushTextureChanged = true;
		glset.activeTexture(GL_TEXTURE1);
		GetTextureData getTextureData;
		getTextureData = txtr.getTexture(maskTexturePath,0,0);
		glset.bindTexture(getTextureData.Id);
		txtr.updateMaskTexture(FBOScreen,width,height,brushRotationRangeBarValue);
	}
}
void brushSizeRangeBar(float xOffset,int width){
	Utilities util;
	brushSizeRangeBarValue -= xOffset / (width / 2);
	brushSizeRangeBarValue = util.restrictBetween(brushSizeRangeBarValue, 0.11f, -0.11f);//Keep in boundaries
}
void brushBlurRangeBar(float xOffset,int width,int height) {
	Utilities util;
	Texture txtr;
	brushBlurChanged = true;
	brushBlurRangeBarValue -= xOffset / (width / 2);
	brushBlurRangeBarValue = util.restrictBetween(brushBlurRangeBarValue, 0.11f, -0.11f);//Keep in boundaries
	txtr.updateMaskTexture(FBOScreen,width,height, brushRotationRangeBarValue);
}
void textureDemonstratorButton(float xOffset,float yOffset,int width,int height) {
	Utilities util;
	Texture txtr;
	textureDemonstratorButtonPosX -= xOffset / (width / 2);
	textureDemonstratorButtonPosY += yOffset / (height / 2);
}
void textureDemonstratorBoundaries(float xOffset,float yOffset,int width,int height) {
	Utilities util;
	Texture txtr;
	textureDemonstratorWidth -= xOffset / 960.0f;
	textureDemonstratorHeight -= yOffset / 540.0f;
}
void brushRotationRangeBar(float xOffset, int width, int height){
	Utilities util;
	Texture txtr;
	brushRotationChanged = true;
	brushRotationRangeBarValue -= xOffset / (width / 2);
	brushRotationRangeBarValue = util.restrictBetween(brushRotationRangeBarValue, 0.11f, -0.11f);//Keep in boundaries
	txtr.updateMaskTexture(FBOScreen, width, height,brushRotationRangeBarValue);
}
void brushOpacityRangeBar(float xOffset, int width, int height) {
	Utilities util;
	Texture txtr;
	//brushOpacityChanged = true; not used
	brushOpacityRangeBarValue -= xOffset / (width / 2);
	brushOpacityRangeBarValue = util.restrictBetween(brushOpacityRangeBarValue, 0.11f, -0.11f);//Keep in boundaries
}
void brushSpacingRangeBar(float xOffset, int width, int height) {
	Utilities util;
	Texture txtr;
	brushSpacingRangeBarValue -= xOffset / (width / 2);
	brushSpacingRangeBarValue = util.restrictBetween(brushSpacingRangeBarValue, 0.11f, -0.11f);//Keep in boundaries
	drawingSpacing = ((brushSpacingRangeBarValue + 0.11f) * 454.545454545) + 1; //-0.11 - 0.11 --> 1 - 101
	drawingCount = 0;
}
void colorBoxColorRangeBar(float yOffset,int height){
	Utilities util;
	colorBoxColorRangeBarValue += yOffset / (height / 2);
	colorBoxColorRangeBarValue = util.restrictBetween(colorBoxColorRangeBarValue, 0.195f, -0.195f);//Keep in boundaries
}
void colorBoxPickerButton(float xOffset, float yOffset, int width, int height) {
	Utilities util;
	colorBoxPickerValue_x -= xOffset / (width / 2);
	colorBoxPickerValue_x = util.restrictBetween(colorBoxPickerValue_x, 0.095f, -0.099f);//Keep in boundaries
	colorBoxPickerValue_y += yOffset / (height / 2);
	colorBoxPickerValue_y = util.restrictBetween(colorBoxPickerValue_y, 0.198f, -0.195f);//Keep in boundaries
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

void exportPathTextBox() {
	Utilities uti;
	Texture txtr;
	TextureData txtrData;
	auto exportPathCheck = tinyfd_selectFolderDialog("Save Texture","");
	if (exportPathCheck) {
		exportPath = exportPathCheck;
		exportFolder = uti.getLastWordBySeparatingWithChar(exportPath,'\\');
	}
}
void exportExtJPGCheckBox() {
	if (jpgFormatChecked == false) {
		jpgFormatChecked = true;
		pngFormatChecked = false;
	}
	else {
		jpgFormatChecked = false;
	}
}
void exportExtPNGCheckBox() {
	if (pngFormatChecked == false) {
		pngFormatChecked = true;
		jpgFormatChecked = false;
	}
	else {
		pngFormatChecked = false;
	}
}
void mirrorXCheckBox() {
	CommonData commonData;
	if (mirrorXCheckBoxChecked == false) {
		mirrorUsed = true;
		glset.uniform1i(commonData.program, "useMirror", 1);
		mirrorXCheckBoxChecked = true;
		mirrorYCheckBoxChecked = false;
		mirrorZCheckBoxChecked = false;
		glset.uniform1i(commonData.program, "verticalMirror", 0);
	}
	else {
		mirrorUsed = false;
		glset.uniform1i(commonData.program, "useMirror", 0);
		mirrorXCheckBoxChecked = false;
	}
}
void mirrorYCheckBox() {
	CommonData commonData;
	if (mirrorYCheckBoxChecked == false) {
		mirrorUsed = true;
		glset.uniform1i(commonData.program, "useMirror", 1);
		mirrorYCheckBoxChecked = true;
		mirrorXCheckBoxChecked = false;
		mirrorZCheckBoxChecked = false;
		glset.uniform1i(commonData.program, "verticalMirror", 1);
	}
	else {
		mirrorUsed = false;
		glset.uniform1i(commonData.program, "useMirror", 0);
		mirrorYCheckBoxChecked = false;
	}
}
void mirrorZCheckBox() {
	CommonData commonData;
	if (mirrorZCheckBoxChecked == false) {
		mirrorUsed = true;
		glset.uniform1i(commonData.program, "useMirror", 1);
		mirrorZCheckBoxChecked = true;
		mirrorYCheckBoxChecked = false;
		mirrorXCheckBoxChecked = false;
		glset.uniform1i(commonData.program, "verticalMirror", 0);
	}
	else {
		mirrorUsed = false;
		glset.uniform1i(commonData.program, "useMirror", 0);
		mirrorZCheckBoxChecked = false;
	}
}
void exportDownloadButtonEnter() {
	exportDownloadButtonPressed = true;
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
	addImageButtonPressed = true;
	Utilities utilities;
	GlSet glset;
	Texture txtr;
	char const* lFilterPatterns[2] = { "*.jpg", "*.png" };
	auto albedoPathCheck = tinyfd_openFileDialog("Select Image", "", 2, lFilterPatterns, "", false);
	if (albedoPathCheck) {
		albedoTexturePath = albedoPathCheck;
		glset.activeTexture(GL_TEXTURE0);
		GetTextureData getTextureData;
		getTextureData = txtr.getTexture(albedoTexturePath,1080,1080); //Force albedo's ratio to be 1:1
	}
}
void addPlaneButton() {
	addPlaneButtonPressed = true;
	vector<float> planeVertices = {
		-1 , 0 , 1 , 0 , 0 , 0 , 1 , 0 ,
		1 , 0 , 1 , 1 , 0 , 0 , 1 , 0 ,
		1 , 0 , -1 , 1 , 1 , 0 , 1 , 0 ,
		-1 , 0 , 1 , 0 , 0 , 0 , 1 , 0 ,
		1 , 0 , -1 , 1 , 1 , 0 , 1 , 0 ,
		-1 , 0 , -1 , 0 , 1 , 0 , 1 , 0 
	};
	modelName = "plane.ligidefault";
	vertices = planeVertices;
	glBufferData(GL_ARRAY_BUFFER, 10000, NULL, GL_DYNAMIC_DRAW);
	
}
void addSphereButton() {
	addSphereButtonPressed = true;
	GlSet glset;
	Sphere sphere;
	modelName = "sphere.ligidefault";
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
	}
	else {
		enableBackfaceCulling = true;
		backfaceCullingChecked = false;
	}
}
void loadModelButton() {
	loadModelButtonPressed = true;
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
	else{
		//If model file path is inappropriate add one
		modelFilePathTextBox();
		if(modelFilePath){
			loadModelButton();
		}
	}
}

//-------------CALLBACK-------------\\

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	Callback callback;
	callback.framebuffer_size_callback(window,width,height);
}

double lastXpos;
double lastYpos;
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	Callback callback;
	CommonData commonData;
	Utilities util;
	mousePosChanged = true;
	mouseXpos = xpos;
	mouseYpos = ypos;
	double xOffset;
	double yOffset;
	
	int width;
	int height;
	glfwGetWindowSize(window,&width,&height);
	

	//Get mouse position change
	xOffset = (lastXpos - xpos) / (1920 / width);
	lastXpos = xpos;
	yOffset = (lastYpos - ypos) / (1080 / height);
	lastYpos = ypos;
	//Get mouse position change
	float range = 0.025f;

	if(xpos > ((textureDemonstratorButtonPosX + textureDemonstratorWidth) - range) * width/2 && xpos < ((textureDemonstratorButtonPosX + textureDemonstratorWidth) + range) * width/2){
		textureDemonstratorBoundariesHover = true;
	}
	else if(height - ypos > ((textureDemonstratorButtonPosY+1.0f - textureDemonstratorHeight) - range) * height/2 && height - ypos < ((textureDemonstratorButtonPosY+1.0f - textureDemonstratorHeight) + range) * height/2 ){
		textureDemonstratorBoundariesHover = true;
	}
	else{
		textureDemonstratorBoundariesHover = false;
	}

	if (brushSizeRangeBarPressed) {
		brushSizeRangeBar(xOffset,width);//Changes the global variable
	}
	if (brushBlurRangeBarPressed) {
		brushBlurRangeBar(xOffset,width,height);//Changes the global variable
	}
	if (brushRotationRangeBarPressed) {
		brushRotationRangeBar(xOffset, width, height);//Changes the global variable
	}
	if (brushOpacityRangeBarPressed) {
		brushOpacityRangeBar(xOffset, width, height);//Changes the global variable
	}
	if (brushSpacingRangeBarPressed) {
		brushSpacingRangeBar(xOffset, width, height);//Changes the global variable
	}
	if (colorBoxColorRangeBarPressed) {
		colorBoxColorRangeBar(yOffset,height);//Changes the global variable
	}
	if (colorBoxPickerPressed) {
		colorBoxPickerButton(xOffset,yOffset,width,height);//Changes the global variable
	}
	if (textureDemonstratorButtonPressed) {
		textureDemonstratorButton(xOffset,yOffset,width,height);//Changes the global variable
	}
	if (textureDemonstratorBoundariesPressed) {
		textureDemonstratorBoundaries(xOffset,yOffset,width,height);//Changes the global variable
	}
	if (colorBoxPickerPressed || colorBoxColorRangeBarPressed || brushBlurRangeBarPressed || brushSizeRangeBarPressed || brushRotationRangeBarPressed || brushOpacityRangeBarPressed || brushSpacingRangeBarPressed || textureDemonstratorButtonPressed ) { //Set cursor as hidden and restrict panel movement if any of the rangebars value is changing
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
		enablePanelMovement = false;
		doPainting = false;
	}
	else {
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		enablePanelMovement = true;
		//Restrict painting
		if (xpos > callbackData.panelLoc * 960 && !callbackData.brushSizeRangeBarEnter && !callbackData.colorBoxColorRangeBarEnter && !callbackData.colorBoxPickerEnter) //Inside of the panel
			doPainting = false;

		else if (xpos < callbackData.panelLoc * 960 && panelData.paintingPanelActive) //Painting panel + outside of panel
			doPainting = true;
		//Restrict painting
	}
}

void scroll_callback(GLFWwindow* window, double scroll, double scrollx)
{
	Callback callback;
	if (!paintingMode) {
		callbackData = callback.scroll_callback(window, scroll, scrollx);
	}
	else {
		holdScrollVal = scrollx;
		doScrollAfterCallInPaintingMode = true;
	}
	cameraPosChanging = true;
}

//---------OTHER---------\\

void updateCameraPosChanging(){
	if (callbackData.cameraPos != holdCameraPos) {
		cameraPosChanging = true;
	}
	holdCameraPos = callbackData.cameraPos;
}
bool clickTaken = false;
void isFirstClickDoneInside() {
	if (glfwGetMouseButton(window, 0) == GLFW_PRESS && !clickTaken) {
		if (!callbackData.addImageButtonEnter && !callbackData.addMaskTextureButtonEnter && !callbackData.addPlaneButtonEnter
			//Check all the buttons
			&& !callbackData.addSphereButtonEnter && !callbackData.autoTriangulateCheckBoxEnter && !callbackData.backfaceCullingCheckBoxEnter
			&& !callbackData.brushSizeRangeBarEnter && !callbackData.loadModelButtonEnter && !callbackData.modelFilePathTextBoxEnter
			&& !callbackData.modelPanelButtonEnter && !callbackData.paintingPanelButtonEnter && !callbackData.exportPanelButtonEnter && !callbackData.texturePanelButtonEnter && !callbackData.colorBoxPickerEnter
			&& !callbackData.colorBoxColorRangeBarEnter && !callbackData.exportPathTextBoxEnter && !callbackData.exportDownloadButtonEnter && !callbackData.exportExtJPGCheckBoxEnter && !callbackData.exportExtPNGCheckBoxEnter
			&& !callbackData.brushBlurRangeBarEnter && !callbackData.brushRotationRangeBarEnter && !callbackData.brushOpacityRangeBarEnter && !callbackData.brushSpacingRangeBarEnter
			&& !callbackData.mirrorXCheckBoxEnter && !callbackData.mirrorYCheckBoxEnter && !callbackData.mirrorZCheckBoxEnter && !callbackData.textureDemonstratorButtonEnter && !textureDemonstratorBoundariesHover) {
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
}
void setButtonPressedFalse() {
	loadModelButtonPressed = false;
	addPlaneButtonPressed = false;
	addSphereButtonPressed = false;
	addImageButtonPressed = false;
	addMaskTextureButtonPressed = false;
	exportDownloadButtonPressed = false;
}