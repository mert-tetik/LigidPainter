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
#include "Render.h"
#include "Texture.h"
#include "UiActions.h"
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
WindowData windowData;
GLFWwindow* window;


#ifdef _WIN32
    char folderDistinguisher = '\\';
#else
	char folderDistinguisher = '/'; 
#endif

//GL_TEXTURE0 = Albedo texture
//GL_TEXTURE1 = Mask texture
//GL_TEXTURE2 = Chars
//GL_TEXTURE3 = Mirrored Screen Mask Painting Texture
//GL_TEXTURE4 = Screen Mask Painting Texture
//GL_TEXTURE5 = 1920x1080 Screen Texture
//GL_TEXTURE6 = Icons
//GL_TEXTURE7 = UV mask texture (used for painting over boundaries)
//GL_TEXTURE8 = Mirrored Depth texture
//GL_TEXTURE9 = Depth texture
//GL_TEXTURE10 = 1080x1080 Screen Texture
//GL_TEXTURE11 = NULL
//GL_TEXTURE12 = Modified mask texture
//GL_TEXTURE13 = skybox

//Shortcuts

//Left CTRL + Z : Undo painting
//Left CTRL + X : change use negative checkbox's state
//Left CTRL + H : hide or show the texture demonstrator
//Left CTRL + Q + scroll = change brush size range bar value
//Left CTRL + W + scroll = change brush blur range bar value
//Left CTRL + E + scroll = change brush rotation range bar value
//Left CTRL + R + scroll = change brush opacity range bar value
//Left CTRL + T + scroll = change brush spacing range bar value
//Left CTRL + Y + scroll = change brush borders range bar value
//Left CTRL + B = set painting fill between quality to 1 or 10
//Left CTRL + TAB + Q = Switch to model panel
//Left CTRL + TAB + W = Switch to texture panel
//Left CTRL + TAB + T = Switch to painting panel
//Left CTRL + TAB + R = Switch to export panel

unsigned int VBO, VAO, FBOScreen; //Vertex Buffer Object, Vertex Array Object, Framebuffer object that I used to render the screen

bool cameraPosChanging = true;
bool paintingMode = false; //True if painting started, False if camera position changed after painting
glm::vec3 holdCameraPos; //Used to detect the camera position change

//Paths
const char* modelFilePath;
string albedoTexturePath = "albedoImage.png";
string maskTexturePath = "./LigidPainter/Resources/Textures/PlainCircle.png";
string maskTextureFile = "PlainCircle.png";
string exportPath = "";
string exportFolder = "Choose Destination Path";
string exportFileName = "LP_Export";
//Paths

string colorpickerHexVal = "#408181";

string modelName;
string customModelName;
vector<float> vertices = { 0 };

//--------Functions--------\\

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void scroll_callback(GLFWwindow* window, double scroll, double scrollx);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void updateCameraPosChanging();
RenderData updateRenderData(RenderData renderData, unsigned int depthTexture, int brushSizeIndicatorSize);
UiData updateUiData();
void updateColorPicker(glm::vec3 RGBval,bool changeHue,bool changeSatV);
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


//-----------------------      UI     -----------------------\\


//Button control

//Button control

bool enablePanelMovement = true; //Panel can be moved if true. Set false while dragging range bar pointers around.

//----------PRESSED----------\\
Might be transported to callback
//(hover calculations are made in callback.cpp)

//Used to let render function know if it's supposed to change colors
void setButtonPressedFalse();
//TOOD : Use struct

bool loadModelButtonPressed = false;
bool addPlaneButtonPressed = false;
bool addSphereButtonPressed = false;
bool addImageButtonPressed = false;
bool addMaskTextureButtonPressed = false;
bool exportDownloadButtonPressed = false;
bool paintingDropperPressed = false;
bool exportFileNameTextBoxPressed = false;
bool hexValTextboxPressed = false;
bool colorBoxPickerButtonPressed = false;

//Used to let mouse callback function know if it's supposed to change range bar values

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

//TOOD : Use struct
float brushSizeRangeBarValue = 0.0f;
float brushBlurRangeBarValue = -0.11f;
float brushRotationRangeBarValue = -0.11f;
float brushOpacityRangeBarValue = 0.11f;
float brushSpacingRangeBarValue = -0.11f;
float brushBorderRangeBarValue = 0.06f;
float colorBoxColorRangeBarValue = 0.0f;
float colorBoxPickerValue_x = 0.0f;
float colorBoxPickerValue_y = 0.0f;

float textureDemonstratorButtonPosX = 0.0f;
float textureDemonstratorButtonPosY = 0.0f;

float maskPanelSliderValue = 0.0f;
//----------RANGE VALUE----------\\.
//-----------------------      UI     -----------------------\\

//bool albedoTextureChanged; use for texture updating conditions

int drawingSpacing = 1;
int drawingCount; // if drawingCount matches with drawingSpaces do painting

int lastMouseXpos = 0;
int lastMouseYpos = 0;

int textureDemonstratorButtonPressCounter = 0;
bool textureDemonstratorButtonPressClicked = true;
bool textureDemonstratorButtonPosChanged = false;
float textureDemonstratorWidth = 0.4f;
float textureDemonstratorHeight = 0.8f;
bool textureDemonstratorBoundariesHover = false;
//bool textureDemonstratorBoundariesHoverR = false;
//bool textureDemonstratorBoundariesHoverB = false;

bool useNegativeForDrawing;

bool panelChanging = false; //Disable painting while changing panel sizes

bool brushValChanged = true;

int paintingFillNumericModifierVal = 10;

bool caps = false; //GLFW_MOD_CAPS_LOCK

char pressedChar;
bool pressedCharChanged;

bool mirrorClick = false;


int textBoxActiveChar = 6;
bool colorpickerHexValTextboxValChanged = true;


bool reduceScreenPaintingQuality = true;
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {

	//------------------TEXT------------------


	if(key >= 320 && key <=329){
		key -= 272;
	}
	if(key == 280 && action == 0){
		if(caps)
			caps = false;
		else
			caps = true;
	}
	if(action == 0 || action == 2){
		if((true)){
			if(exportFileNameTextBoxPressed && exportFileName.size() < 20){
				if(key == 32){
					exportFileName += ' ';
				}
				else if(isalpha((char)key)){
					if(!caps){
						exportFileName+=(char)key+32;//lowercase
					}
					else{
						exportFileName+=(char)key;//UPPERCASE
					}
				}
				else if(isdigit((char)key)){
					exportFileName+=(char)(key);//lowercase
				}
			}
			if(hexValTextboxPressed && textBoxActiveChar != 7){
				if(isdigit((char)key)){
					colorpickerHexVal[textBoxActiveChar]=(char)(key);//lowercase
					textBoxActiveChar++;
					colorpickerHexValTextboxValChanged = true;
				}
				else if(key == 'A'){
					colorpickerHexVal[textBoxActiveChar] = 'a';
					textBoxActiveChar++;
					colorpickerHexValTextboxValChanged = true;
				}
				else if(key == 'B'){
					colorpickerHexVal[textBoxActiveChar] = 'b';
					textBoxActiveChar++;
					colorpickerHexValTextboxValChanged = true;
				}
				else if(key == 'C'){
					colorpickerHexVal[textBoxActiveChar] = 'c';
					textBoxActiveChar++;
					colorpickerHexValTextboxValChanged = true;
				}
				else if(key == 'D'){
					colorpickerHexVal[textBoxActiveChar] = 'd';
					textBoxActiveChar++;
					colorpickerHexValTextboxValChanged = true;
				}
				else if(key == 'E'){
					colorpickerHexVal[textBoxActiveChar] = 'e';
					textBoxActiveChar++;
					colorpickerHexValTextboxValChanged = true;
				}
				else if(key == 'F'){
					colorpickerHexVal[textBoxActiveChar] = 'f';
					textBoxActiveChar++;
					colorpickerHexValTextboxValChanged = true;
				}
			}
			pressedCharChanged = false;
		}
		if(key == 259){
			if(exportFileNameTextBoxPressed && exportFileName != ""){
				exportFileName.pop_back();
			}
			if(hexValTextboxPressed && textBoxActiveChar != 0){
				colorpickerHexVal[textBoxActiveChar] = '0';
				textBoxActiveChar--;
				colorpickerHexValTextboxValChanged = true;
			}
			pressedCharChanged = false;
		}
	}
	pressedCharChanged = true;
	pressedChar = key;



	//------------------SHORTCUTS------------------



	Utilities util;

	bool shiftTabAltRelease =  glfwGetKey(window, GLFW_KEY_TAB) == GLFW_RELEASE && glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE && glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_RELEASE;

	//Ctrl + x use negative checkbox
	if(util.shortCut(window,GLFW_KEY_LEFT_CONTROL,GLFW_KEY_X,NULL,NULL) && panelData.paintingPanelActive && action == 0 && shiftTabAltRelease){
		if(useNegativeForDrawing){
			useNegativeForDrawing = false;
		}
		else{
			useNegativeForDrawing = true;
		}
	}

	//Ctrl + h change texture demonstrator's state
	if(util.shortCut(window,GLFW_KEY_LEFT_CONTROL,GLFW_KEY_H,NULL,NULL) && action == 0 && shiftTabAltRelease){
		textureDemonstratorButtonPressClicked = true;
	}

	//Ctrl + b change fill between numeric modifier's state
	if(util.shortCut(window,GLFW_KEY_LEFT_CONTROL,GLFW_KEY_B,NULL,NULL) && action == 0 && shiftTabAltRelease && panelData.paintingPanelActive){
		Texture txtr;
		if(paintingFillNumericModifierVal == 10){
			paintingFillNumericModifierVal = 1;
			reduceScreenPaintingQuality = false;
		}
		else if(paintingFillNumericModifierVal == 1){
			paintingFillNumericModifierVal = 10;
			reduceScreenPaintingQuality = true;
		}
		else{
			paintingFillNumericModifierVal = 10;
			reduceScreenPaintingQuality = true;
		}
		brushValChanged = true;
		txtr.refreshScreenDrawingTexture(reduceScreenPaintingQuality);
	}

	LigidPainter lp;
	//Ctrl + Tab + q switch to model panel
	if(util.shortCut(window,GLFW_KEY_LEFT_CONTROL,GLFW_KEY_TAB,GLFW_KEY_Q,NULL) && action == 0){
		lp.modelPanelButton();
	}
	//Ctrl + Tab + w switch to texture panel
	if(util.shortCut(window,GLFW_KEY_LEFT_CONTROL,GLFW_KEY_TAB,GLFW_KEY_W,NULL) && action == 0){
		lp.texturePanelButton();
	}
	//Ctrl + Tab + t switch to model panel
	if(util.shortCut(window,GLFW_KEY_LEFT_CONTROL,GLFW_KEY_TAB,GLFW_KEY_T,NULL) && action == 0){
		lp.paintingPanelButton();
	}
	//Ctrl + Tab + r switch to model panel
	if(util.shortCut(window,GLFW_KEY_LEFT_CONTROL,GLFW_KEY_TAB,GLFW_KEY_R,NULL) && action == 0){
		lp.exportPanelButton();
	}
}

bool colorBoxClicked = false;
bool hueBarClicked = false;

BrushMaskTextures brushMaskTextures;

Programs programs;

bool colorBoxValChanged = true;

bool renderSphere = false;
bool renderPlane = false;

vector<float> sphereVertices;

bool hueValChanging;




glm::vec3 drawColor;

float brushBlurVal = 1000;

bool LigidPainter::run()
{
	ColorData colorData;
	UserInterface ui;
	Texture txtr;
	RenderData renderData;
	UiData uidata;
	TextureGenerator textureGen;
	Callback callback;
	Render render;
	UiActions uiAct;
	UiActionsData uiActData;
	InitializedTextures textures;
	RenderOutData renderOut;
	LigidPainter ligid;
	uiActData.textureDemonstratorBoundariesPressed = false;
	uiActData.textureDemonstratorButtonPressed = false;

	windowData = glset.getWindow();
	window = windowData.window;

	//Set Callbacks
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback); 
	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	//Set Callbacks


	ui.setViewportBgColor();
	
	
	programs = glset.getProgram();

	glGenBuffers(1, &VBO);
	//glGenVertexArrays(1, &VAO);
	//glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glUseProgram(programs.program);

	ui.sendProgramsToUserInterface(programs);
	render.sendProgramsToRender(programs);
	txtr.sendProgramsToTextures(programs);

	callback.sendMaxWindowSize(windowData.windowMaxWidth,windowData.windowMaxHeight);
	ui.sendMaxWindowSize(windowData.windowMaxWidth,windowData.windowMaxHeight);
	uiAct.sendMaxWindowSize(windowData.windowMaxWidth,windowData.windowMaxHeight);
	render.sendMaxWindowSize(windowData.windowMaxWidth,windowData.windowMaxHeight);
	txtr.sendMaxWindowSize(windowData.windowMaxWidth,windowData.windowMaxHeight);

	renderData.window = window;

	glset.setVertexAtribPointer();
	glBufferData(GL_ARRAY_BUFFER, 10000, NULL, GL_DYNAMIC_DRAW); 

	ui.uploadChars();

	//Load brush mask textures
	

	brushMaskTextures = glset.loadBrushMaskTextures();

	glset.loadCubemaps();



	glm::vec3 lightPos(1.2f, 1.0f, 2.0f);
	glset.uniform1i(programs.program, "uvMask", 7);
	
	glUseProgram(programs.iconsProgram);
	glset.uniform1i(programs.iconsProgram, "icon", 6);
	glUseProgram(programs.program);

	Utilities util;

	glset.uniform3fv(programs.program, "lightPos", lightPos);
	glset.uniform1f(programs.program, "material.shininess", 32.0f);
	glset.uniform1i(programs.program, "screenMaskTexture", 4);
	glset.uniform1i(programs.program, "mirroredScreenMaskTexture", 3);
	glset.uniform3fv(programs.program,"textColor",colorData.textColor);
	glset.uniform1i(programs.program, "material.diffuse", 0);
	glset.uniform1i(programs.program, "material.specular", 1);
	glset.uniform1i(programs.program, "depthTexture", 9);
	glset.uniform1i(programs.program, "mirroredDepthTexture", 8);
	glset.uniform1i(programs.program, "text", 2);


	glset.uniform1i(programs.program, "modifiedMaskTexture", 12);

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); //Wireframe

	//Screen Texture
	GLubyte* screenTexture = new GLubyte[(windowData.windowMaxWidth/2) * (windowData.windowMaxHeight/2)];
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

	glm::mat4 perspectiveProjection = render.setMatrices();

	float brushSize;
	ExportData exportData;

	Icons icons;
	icons = ui.loadIcons();

	//------Set Textures------\\

	textures = txtr.initTextures(maskTexturePath.c_str());

	//------Set Textures------\\

	int screenWidth;
	int screenHeight;
	glfwGetWindowSize(window, &screenWidth, &screenHeight);
	txtr.updateMaskTexture(FBOScreen, screenWidth, screenHeight, brushRotationRangeBarValue,false,brushBorderRangeBarValue,brushBlurVal);

	panelData.modelPanelActive = true; //Active panel by default


	int charPressingSensivity = 30;
	int charPressCounter = 0;

	glEnable(GL_MULTISAMPLE);

	//Cursors
	LigidCursors cursors;
	cursors = glset.loadCursors();

	bool textureDemonstratorButtonPressed = false;

	
	//Plane Model
	vector<float> planeVertices = {
		-1 , 0 , 1 , 0 , 0 , 0 , 1 , 0 ,
		1 , 0 , 1 , 1 , 0 , 0 , 1 , 0 ,
		1 , 0 , -1 , 1 , 1 , 0 , 1 , 0 ,
		-1 , 0 , 1 , 0 , 0 , 0 , 1 , 0 ,
		1 , 0 , -1 , 1 , 1 , 0 , 1 , 0 ,
		-1 , 0 , -1 , 0 , 1 , 0 , 1 , 0 
	};
	//Sphere Model
	Sphere sphere;
	sphereVertices = sphere.getSphere();

	ViewUpdateData viewUpdateData;

	unsigned int paintingFBO;
	glset.genFramebuffers(paintingFBO);


	PBRShaderData pbrShaderData;
	SkyBoxShaderData skyBoxShaderData;
	ScreenDepthShaderData screenDepthShaderData;
	AxisPointerShaderData axisPointerShaderData;

	pbrShaderData.bluryskybox = 16;
	pbrShaderData.depthTexture = 9;
	pbrShaderData.materialDiffuse = 0;
	pbrShaderData.mirroredDepthTexture = 8;
	pbrShaderData.mirroredScreenMaskTexture = 3;
	pbrShaderData.screenMaskTexture = 4;

	skyBoxShaderData.skybox = 13;
	skyBoxShaderData.projection = perspectiveProjection;

	axisPointerShaderData.projection = perspectiveProjection;

	
	while (true)//Main loop
	{
		glfwPollEvents();

		util.printRenderingSpeed();

		updateCameraPosChanging();



		//Release textboxes
		if ((glfwGetMouseButton(window, 0) == GLFW_PRESS || glfwGetMouseButton(window, 1) == GLFW_PRESS)){
			if(!callbackData.exportFileNameTextBoxEnter && exportFileNameTextBoxPressed){
				exportFileNameTextBoxPressed = false;
				if(exportFileName == ""){
					exportFileName = "LP_Export";
				}	
			}
			if(!callbackData.hexValueTextboxEnter && exportFileNameTextBoxPressed){
				hexValTextboxPressed = false;
				textBoxActiveChar = 6;
			}
		}



		//Ui actions
		uiActData = uiAct.uiActions(window,callbackData,textureDemonstratorBoundariesHover);
		



		//Check if texture demonstrator button clicked
		if(uiActData.textureDemonstratorButtonPressed){
			textureDemonstratorButtonPressCounter++;
			textureDemonstratorButtonPressed = true;
		}
		if(textureDemonstratorButtonPressCounter < 20 && textureDemonstratorButtonPressed && glfwGetMouseButton(window, 0) == GLFW_RELEASE && !textureDemonstratorButtonPosChanged){
			textureDemonstratorButtonPressClicked = true;
			textureDemonstratorButtonPressed = false;

		}
		if(glfwGetMouseButton(window, 0) == GLFW_RELEASE){
			textureDemonstratorButtonPressCounter = 0;
			textureDemonstratorButtonPressed = false;
			textureDemonstratorButtonPosChanged = false;
		}



		//Painting
		if (glfwGetMouseButton(window, 0) == GLFW_PRESS && doPainting && glfwGetMouseButton(window, 1) == GLFW_RELEASE && !paintingDropperPressed) {//Used for spacing
			drawingCount++;
		}
		if (glfwGetMouseButton(window, 0) == GLFW_PRESS && doPainting && drawingCount == drawingSpacing && !panelChanging && !callbackData.panelChangeLoc && glfwGetMouseButton(window, 1) == GLFW_RELEASE && !paintingDropperPressed){
			textureGen.drawToScreen(window, maskTexturePath, screenPaintingReturnData.normalId, brushSize, FBOScreen,brushRotationRangeBarValue,brushOpacityRangeBarValue,lastMouseXpos, lastMouseYpos,mouseXpos,mouseYpos,mirrorUsed,useNegativeForDrawing,brushValChanged,paintingFillNumericModifierVal,programs,windowData.windowMaxWidth,windowData.windowMaxHeight,reduceScreenPaintingQuality,brushBorderRangeBarValue,brushBlurVal,paintingFBO);
			brushValChanged = false;
			drawingCount = 0;
			paintingMode = true;
		}
		if(drawingCount > drawingSpacing)
			drawingCount = 0;
		panelChanging = false;
		lastMouseXpos = mouseXpos;
		lastMouseYpos = mouseYpos;



		//Change color picker's value
		if((paintingDropperPressed && glfwGetMouseButton(window, 0) == GLFW_PRESS) || (colorBoxClicked && !callbackData.colorBoxPickerEnter) || (hueBarClicked && !callbackData.colorBoxPickerEnter && !hueValChanging)|| colorpickerHexValTextboxValChanged){
			if(colorpickerHexValTextboxValChanged){
				updateColorPicker(util.hexToRGBConverter(colorpickerHexVal),true,true);//Update colorbox val once color picker hex value textbox value changed
			}
			else{
				updateColorPicker(renderOut.mouseHoverPixel,true,!hueBarClicked);//Update colorbox val once dropper is used or colorbox is clicked
			}
		}
		if(glfwGetMouseButton(window, 0) == GLFW_RELEASE)
			hueValChanging = false;
		if(colorBoxClicked)
			colorBoxClicked = false;
		if(hueBarClicked)
			hueBarClicked = false;
		if(colorBoxPickerButtonPressed)
			colorBoxPickerButtonPressed = false;


		//Update
		brushSize = double(brushSizeRangeBarValue + 0.1f) * 800.0 + 20.0 ;
		renderData = updateRenderData(renderData,textures.depthTexture, brushSize);
		uidata = updateUiData();
		panelData.movePanel = callbackData.movePanel;
		exportData.exportImage = exportImage;
		exportData.path = exportPath.c_str();
		exportData.fileName = exportFileName.c_str();
		if (cameraPosChanging || mirrorClick){
			viewUpdateData = render.updateViewMatrix(callbackData.cameraPos, callbackData.originPos,mirrorXCheckBoxChecked,mirrorYCheckBoxChecked,mirrorZCheckBoxChecked); 
		}




		pbrShaderData.drawColor = drawColor;
		pbrShaderData.mirroredView = viewUpdateData.mirroredView;
		pbrShaderData.mirroredViewPos = viewUpdateData.mirroredCameraPos;
		pbrShaderData.projection = perspectiveProjection;
		pbrShaderData.useMirror = mirrorUsed;
		pbrShaderData.view = viewUpdateData.view;
		pbrShaderData.viewPos = viewUpdateData.cameraPos;

		skyBoxShaderData.view = viewUpdateData.view;

		screenDepthShaderData.mirroredView = viewUpdateData.mirroredView;
		screenDepthShaderData.projection = perspectiveProjection;
		screenDepthShaderData.view = viewUpdateData.view;

		axisPointerShaderData.view = viewUpdateData.view;



		//Render
		renderOut = render.render(renderData, vertices, FBOScreen, panelData,exportData,uidata,textureDemonstratorButtonPosX,textureDemonstratorButtonPosY,textureDemonstratorButtonPressClicked,textureDemonstratorWidth,textureDemonstratorHeight,uiActData.textureDemonstratorBoundariesPressed,icons,maskTextureFile.c_str(),paintingFillNumericModifierVal,maskPanelSliderValue,brushMaskTextures.textures,colorpickerHexVal,colorpickerHexValTextboxValChanged,colorBoxValChanged,planeVertices,sphereVertices,renderPlane,renderSphere,reduceScreenPaintingQuality,pbrShaderData,skyBoxShaderData,brushBlurVal,screenDepthShaderData,axisPointerShaderData);
		drawColor = renderOut.colorBoxVal/255.0f;//TODO : Once the value changed
		colorBoxValChanged = false;
		colorpickerHexValTextboxValChanged = false;
		colorpickerHexVal = renderOut.colorpickerHexVal;




		//Update brush mask texture file's name once the brush mask texture changed
		if(renderOut.maskPanelMaskClicked){
			for (size_t i = 0; i < brushMaskTextures.textures.size(); i++)
			{
				if(brushMaskTextures.textures[i] == renderOut.currentBrushMaskTxtr){
					maskTextureFile = util.cropString(brushMaskTextures.names[i],20);
				}
			}
		}
		if(renderOut.maskPanelMaskClicked){
			brushValChanged = true;
		}

		exportImage = false; //After exporting, set exportImage false so we won't download the texture repeatedly
		setButtonPressedFalse();
		textureDemonstratorButtonPressClicked = false;


		if (mousePosChanged) { //To make sure painting done before changing camera position
			callbackData = callback.mouse_callback(window, mouseXpos, mouseYpos, panelData, brushSizeRangeBarValue, colorBoxPickerValue_x, colorBoxPickerValue_y, colorBoxColorRangeBarValue, brushBlurRangeBarValue, enablePanelMovement,brushRotationRangeBarValue, brushOpacityRangeBarValue, brushSpacingRangeBarValue,textureDemonstratorButtonPosX,textureDemonstratorButtonPosY,maskPanelSliderValue,renderOut.maskPanelMaskHover,cursors,paintingDropperPressed,brushBorderRangeBarValue);
		}
		mirrorClick = false;

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

		

		if(glfwWindowShouldClose(window)){
			int dialogResult = tinyfd_messageBox("Warning","LigidPainter will be closed. Do you want to proceed?","yesno","warning",1);
			if(dialogResult)
				break;
			else
				glfwSetWindowShouldClose(window,0);
		}
	}
	
	glfwDestroyWindow(window);
	glfwTerminate();
	return true;
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
	Utilities util;
	UiActions uiAct; 
	mousePosChanged = true;
	mouseXpos = xpos;
	mouseYpos = ypos;
	double xOffset;
	double yOffset;
	
	int width;
	int height;
	glfwGetWindowSize(window,&width,&height);
	
	//Get mouse position change
	xOffset = (lastXpos - xpos) / (windowData.windowMaxWidth / width);
	lastXpos = xpos;
	yOffset = (lastYpos - ypos) / (windowData.windowMaxHeight / height);
	lastYpos = ypos;
	//Get mouse position change
	

	float screenGapX = (windowData.windowMaxWidth - width); 


	//Texture demonstrator
	float range = 0.05f;
	if(xpos > ((textureDemonstratorButtonPosX + textureDemonstratorWidth) - range) * width/2 && xpos < ((textureDemonstratorButtonPosX + textureDemonstratorWidth) + range) * width/2 && height - ypos > ((textureDemonstratorButtonPosY+1.0f - textureDemonstratorHeight) - range) * height/2 && height - ypos < ((textureDemonstratorButtonPosY+1.0f - textureDemonstratorHeight) + range) * height/2 ){
		textureDemonstratorBoundariesHover = true;
	}
	else{
		textureDemonstratorBoundariesHover = false;
	}
	//Texture demonstrator
	bool hideCursor = uiAct.updateRangeValues(window,xOffset,yOffset,width,height); 
	if(paintingDropperPressed){
		doPainting = false;
	}
	else if(callbackData.textureDemonstratorButtonEnter || callbackData.paintingPanelButtonEnter || callbackData.modelPanelButtonEnter || callbackData.exportPanelButtonEnter || callbackData.texturePanelButtonEnter || callbackData.mirrorXCheckBoxEnter|| callbackData.mirrorYCheckBoxEnter|| callbackData.mirrorZCheckBoxEnter){
		doPainting = false;
	}
	else if (hideCursor) { //Set cursor as hidden and restrict panel movement if any of the rangebars value is changing
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
		enablePanelMovement = false;
		doPainting = false;
	}
	else {
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		enablePanelMovement = true;
		
		//Check if cursor is inside of the panel

		if (xpos > ((windowData.windowMaxWidth / 2) * callbackData.panelLoc) - screenGapX && !callbackData.brushSizeRangeBarEnter && !callbackData.colorBoxColorRangeBarEnter && !callbackData.colorBoxPickerEnter) //Inside of the panel
			doPainting = false;

		else if (xpos < ((windowData.windowMaxWidth / 2) * callbackData.panelLoc) - screenGapX  && panelData.paintingPanelActive) //Painting panel + outside of panel
			doPainting = true;
	}
}

void scroll_callback(GLFWwindow* window, double scroll, double scrollx)
{
	Callback callback;
	Utilities util;
	LigidPainter ligid;
	int screenSizeX;
	int screenSizeY;
	glfwGetWindowSize(window,&screenSizeX,&screenSizeY);

	//Ctrl + q = change brush size range bar value
	if(glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS && panelData.paintingPanelActive){
		ligid.brushSizeRangeBar((float)scrollx*10.0f,screenSizeX);
	}
	//Ctrl + w = change brush blur range bar value
	else if(glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS && panelData.paintingPanelActive){
		ligid.brushBlurRangeBar((float)scrollx*10.0f,screenSizeX,screenSizeY,false);
	}
	//Ctrl + e = change brush rotation range bar value
	else if(glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS && panelData.paintingPanelActive){
		ligid.brushRotationRangeBar((float)scrollx*10.0f,screenSizeX,screenSizeY);
	}
	//Ctrl + r = change brush opacity range bar value
	else if(glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS && panelData.paintingPanelActive){
		ligid.brushOpacityRangeBar((float)scrollx*10.0f,screenSizeX,screenSizeY);
	}
	//Ctrl + t = change brush spacing range bar value
	else if(glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS && panelData.paintingPanelActive){
		ligid.brushSpacingRangeBar((float)scrollx*10.0f,screenSizeX,screenSizeY);
	}
	//Ctrl + y = change brush borders range bar value
	else if(glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS && panelData.paintingPanelActive){
		ligid.brushBordersRangeBar((float)scrollx*10.0f,screenSizeX,screenSizeY);
	}
	else{
		if (!paintingMode && !callbackData.maskPanelEnter) {
			callbackData = callback.scroll_callback(window, scroll, scrollx);
		}
		else if(callbackData.maskPanelEnter){
			maskPanelSliderValue += scrollx / 40.0;
			maskPanelSliderValue = util.restrictBetween(maskPanelSliderValue, 0.0f, -0.25f);//Keep in boundaries
		}
		else {
			holdScrollVal = scrollx;
			doScrollAfterCallInPaintingMode = true;
		}
		cameraPosChanging = true;
	}
}




//---------OTHER---------\\

void updateCameraPosChanging(){
	if (callbackData.cameraPos != holdCameraPos) {
		cameraPosChanging = true;
	}
	holdCameraPos = callbackData.cameraPos;
}

void setButtonPressedFalse() {
	loadModelButtonPressed = false;
	addPlaneButtonPressed = false;
	addSphereButtonPressed = false;
	addImageButtonPressed = false;
	addMaskTextureButtonPressed = false;
	exportDownloadButtonPressed = false;
}

void updateColorPicker(glm::vec3 RGBval,bool changeHue,bool changeSatVal){
	int width;
	int height;
	glfwGetWindowSize(window,&width,&height);
	
	Utilities util;
	
	colorBoxValChanged = true;
	glm::vec3 hsvVal = util.RGBToHSVGenerator(RGBval);

	drawColor = RGBval/glm::vec3(255.0f);
	
	glset.uniform3fv(programs.program, "drawColor", RGBval/glm::vec3(255.0f));

	if(changeHue){
		colorBoxColorRangeBarValue = (hsvVal.r / 708.333333333f) - 0.18f; //0.195
	}
	if(changeSatVal){
		colorBoxPickerValue_x = (hsvVal.g / 1342.10526316f) - 0.095f; //0.095
		colorBoxPickerValue_y = (hsvVal.b / 653.846153846f) - 0.195f; //0.195
	}
	paintingDropperPressed = false;
}

//-----------------------------UI ACTIONS-----------------------------\\

void LigidPainter::addMaskTextureButton() {
	//Needed for updating mask texture
	int width;
	int height;
	glfwGetWindowSize(window, &width, &height);
	//
	addMaskTextureButtonPressed = true;
	GlSet glset;
	Texture txtr;
	Utilities util;
	char const* lFilterPatterns[2] = { "*.jpg", "*.png" };
	auto maskTexturePathCheck = tinyfd_openFileDialog("Select Mask Texture", "", 2, lFilterPatterns, "", false);
	if (maskTexturePathCheck) {
		maskTexturePath = maskTexturePathCheck;
		maskTextureFile = util.getLastWordBySeparatingWithChar(maskTexturePath,folderDistinguisher); 
		brushValChanged = true;
		glset.activeTexture(GL_TEXTURE1);

		brushMaskTextures.textures.push_back(txtr.getTexture(maskTexturePath,0,0,false));
		brushMaskTextures.names.push_back(maskTexturePath);

		txtr.updateMaskTexture(FBOScreen,width,height,brushRotationRangeBarValue,false,brushBorderRangeBarValue,brushBlurVal);
	}
}
void LigidPainter::brushSizeRangeBar(float xOffset,int width){
	brushValChanged = true;
	Utilities util;
	brushSizeRangeBarValue -= xOffset / (width / 2);
	brushSizeRangeBarValue = util.restrictBetween(brushSizeRangeBarValue, 0.11f, -0.11f);//Keep in boundaries
}
void LigidPainter::colorBox(){
	colorBoxClicked = true;
}
void LigidPainter::hueBar(){
	hueBarClicked = true;
}
void LigidPainter::brushBlurRangeBar(float xOffset,int width,int height,bool renderTiny) {
	Utilities util;
	Texture txtr;
	brushValChanged = true;
	brushBlurRangeBarValue -= xOffset / (width / 2);
	brushBlurRangeBarValue = util.restrictBetween(brushBlurRangeBarValue, 0.11f, -0.11f);//Keep in boundaries
	txtr.updateMaskTexture(FBOScreen,width,height, brushRotationRangeBarValue,renderTiny,brushBorderRangeBarValue,brushBlurVal);

	if (242 - ((brushBlurRangeBarValue + 0.1f) * 1000.0) - 15 > 200){ //If the range bar value is low enough disable blur effect
		brushBlurVal = 1000;
	}
	else {
		brushBlurVal = 242 - ((brushBlurRangeBarValue + 0.1f) * 1000.0) - 8;
	}
}
void LigidPainter::textureDemonstratorButton(float xOffset,float yOffset,int width,int height) {
	panelChanging = true;
	Utilities util;
	Texture txtr;

	textureDemonstratorButtonPosChanged = true;

	textureDemonstratorButtonPosX -= xOffset / (windowData.windowMaxWidth / 2);
	textureDemonstratorButtonPosX = util.restrictBetween(textureDemonstratorButtonPosX,2.0f,0.0f);
	textureDemonstratorButtonPosY += yOffset / (windowData.windowMaxHeight / 2);
	textureDemonstratorButtonPosY = util.restrictBetween(textureDemonstratorButtonPosY,0.97f,-1.0f);

}
void LigidPainter::textureDemonstratorBoundaries(float xOffset,float yOffset,int width,int height) {
	panelChanging = true;
	Utilities util;
	Texture txtr;
	textureDemonstratorWidth -= xOffset / 960.0f;
	textureDemonstratorHeight -= yOffset / 540.0f;
}
void LigidPainter::brushRotationRangeBar(float xOffset, int width, int height){
	Utilities util;
	Texture txtr;
	brushValChanged = true;
	brushRotationRangeBarValue -= xOffset / (width / 2);
	brushRotationRangeBarValue = util.restrictBetween(brushRotationRangeBarValue, 0.11f, -0.11f);//Keep in boundaries
	txtr.updateMaskTexture(FBOScreen, width, height,brushRotationRangeBarValue,true,brushBorderRangeBarValue,brushBlurVal);
}
void LigidPainter::brushOpacityRangeBar(float xOffset, int width, int height) {
	Utilities util;
	Texture txtr;
	//brushOpacityChanged = true; not used
	brushOpacityRangeBarValue -= xOffset / (width / 2);
	brushOpacityRangeBarValue = util.restrictBetween(brushOpacityRangeBarValue, 0.11f, -0.11f);//Keep in boundaries
}
void LigidPainter::brushSpacingRangeBar(float xOffset, int width, int height) {
	Utilities util;
	Texture txtr;
	brushSpacingRangeBarValue -= xOffset / (width / 2);
	brushSpacingRangeBarValue = util.restrictBetween(brushSpacingRangeBarValue, 0.11f, -0.11f);//Keep in boundaries
	drawingSpacing = ((brushSpacingRangeBarValue + 0.11f) * 454.545454545) + 1; //-0.11 - 0.11 --> 1 - 101
	drawingCount = 0;
}
void LigidPainter::brushBordersRangeBar(float xOffset, int width, int height) {
	Utilities util;
	Texture txtr;
	brushValChanged = true;
	brushBorderRangeBarValue -= xOffset / (width / 2);
	brushBorderRangeBarValue = util.restrictBetween(brushBorderRangeBarValue, 0.11f, -0.11f);//Keep in boundaries
	txtr.updateMaskTexture(FBOScreen, width, height,brushRotationRangeBarValue,true,brushBorderRangeBarValue,brushBlurVal);
}
void LigidPainter::colorBoxColorRangeBar(float yOffset,int height){
	Utilities util;
	hueValChanging = true;
	colorBoxColorRangeBarValue += yOffset / (height / 2);
	colorBoxColorRangeBarValue = util.restrictBetween(colorBoxColorRangeBarValue, 0.180f, -0.180f);//Keep in boundaries
	colorBoxValChanged = true;
}
void LigidPainter::colorBoxPickerButton(float xOffset, float yOffset, int width, int height) {
	colorBoxPickerButtonPressed = true;
	Utilities util;
	colorBoxPickerValue_x -= xOffset / (width / 2);
	colorBoxPickerValue_x = util.restrictBetween(colorBoxPickerValue_x, 0.099f, -0.1f);//Keep in boundaries
	colorBoxPickerValue_y += yOffset / (height / 2);
	colorBoxPickerValue_y = util.restrictBetween(colorBoxPickerValue_y, 0.199f, -0.2f);//Keep in boundaries
	colorBoxValChanged = true;
}

void LigidPainter::modelFilePathTextBox() {
	Utilities utilities;
	char const* lFilterPatterns[1] = { "*.obj" };
	auto modelFilePathCheck = tinyfd_openFileDialog("Select 3D Model","",1, lFilterPatterns,"",false);
	if (modelFilePathCheck) {
		modelFilePath = modelFilePathCheck;
		modelName = utilities.getLastWordBySeparatingWithChar(modelFilePath,folderDistinguisher);
		customModelName = modelName;
	}
}

void LigidPainter::exportPathTextBox() {
	Utilities uti;
	Texture txtr;
	TextureData txtrData;
	auto exportPathCheck = tinyfd_selectFolderDialog("Save Texture","");
	if (exportPathCheck) {
		exportPath = exportPathCheck;
		exportFolder = uti.getLastWordBySeparatingWithChar(exportPath,folderDistinguisher);
	}
}
void LigidPainter::exportFileNameTextBox() {
	exportFileNameTextBoxPressed = true;
}
void LigidPainter::exportExtJPGCheckBox() {
	if (jpgFormatChecked == false) {
		jpgFormatChecked = true;
		pngFormatChecked = false;
	}
}
void LigidPainter::exportExtPNGCheckBox() {
	if (pngFormatChecked == false) {
		pngFormatChecked = true;
		jpgFormatChecked = false;
	}
}
void LigidPainter::mirrorXCheckBox() {
	mirrorClick = true;
	if (mirrorXCheckBoxChecked == false) {
		mirrorUsed = true;
		glset.uniform1i(programs.program, "useMirror", 1);
		mirrorXCheckBoxChecked = true;
		mirrorYCheckBoxChecked = false;
		mirrorZCheckBoxChecked = false;
		glset.uniform1i(programs.program, "verticalMirror", 0);
	}
	else {
		mirrorUsed = false;
		glset.uniform1i(programs.program, "useMirror", 0);
		mirrorXCheckBoxChecked = false;
	}
}
void LigidPainter::mirrorYCheckBox() {
	mirrorClick = true;
	if (mirrorYCheckBoxChecked == false) {
		mirrorUsed = true;
		glset.uniform1i(programs.program, "useMirror", 1);
		mirrorYCheckBoxChecked = true;
		mirrorXCheckBoxChecked = false;
		mirrorZCheckBoxChecked = false;
		glset.uniform1i(programs.program, "verticalMirror", 1);
	}
	else {
		mirrorUsed = false;
		glset.uniform1i(programs.program, "useMirror", 0);
		mirrorYCheckBoxChecked = false;
	}
}
void LigidPainter::mirrorZCheckBox() {
	mirrorClick = true;
	if (mirrorZCheckBoxChecked == false) {
		mirrorUsed = true;
		glset.uniform1i(programs.program, "useMirror", 1);
		mirrorZCheckBoxChecked = true;
		mirrorYCheckBoxChecked = false;
		mirrorXCheckBoxChecked = false;
		glset.uniform1i(programs.program, "verticalMirror", 0);
	}
	else {
		mirrorUsed = false;
		glset.uniform1i(programs.program, "useMirror", 0);
		mirrorZCheckBoxChecked = false;
	}
}
void LigidPainter::exportDownloadButtonEnter() {
	exportDownloadButtonPressed = true;
	exportImage = true;
}

void LigidPainter::modelPanelButton() {
	panelData.modelPanelActive = true;
	panelData.texturePanelActive = false;
	panelData.paintingPanelActive = false;
	panelData.exportPanelActive = false;
	doPainting = false;
}
void LigidPainter::texturePanelButton() {
	panelData.modelPanelActive = false;
	panelData.texturePanelActive = true;
	panelData.paintingPanelActive = false;
	panelData.exportPanelActive = false;
	doPainting = false;
}
void LigidPainter::paintingPanelButton() {
	panelData.modelPanelActive = false;
	panelData.texturePanelActive = false;
	panelData.paintingPanelActive = true;
	panelData.exportPanelActive = false;
	doPainting = true;
}
void LigidPainter::exportPanelButton() {
	panelData.modelPanelActive = false;
	panelData.texturePanelActive = false;
	panelData.paintingPanelActive = false;
	panelData.exportPanelActive = true;
	doPainting = false;
}
void LigidPainter::addImageButton() {
	addImageButtonPressed = true;
	Utilities utilities;
	GlSet glset;
	Texture txtr;
	char const* lFilterPatterns[2] = { "*.jpg", "*.png" };
	auto albedoPathCheck = tinyfd_openFileDialog("Select Image", "", 2, lFilterPatterns, "", false);
	if (albedoPathCheck) {
		albedoTexturePath = albedoPathCheck;
		glset.activeTexture(GL_TEXTURE0);
		txtr.getTexture(albedoTexturePath,1080,1080,true); //Force albedo's ratio to be 1:1
	}
}
void LigidPainter::addPlaneButton() {
	renderPlane = true;
	renderSphere = false;

	addPlaneButtonPressed = true;
	modelName = "plane.default";	
}
void LigidPainter::addSphereButton() {
	renderPlane = false;
	renderSphere = true;
	addSphereButtonPressed = true;
	modelName = "sphere.default";
	glset.bufferData(sphereVertices);
}
void LigidPainter::loadCustomModel(){
	modelName = customModelName;
	if(modelName == ""){
		//If model file path is null add one
		modelFilePathTextBox();
		if(modelFilePath){
			loadModelButton();
		}
	}
	hexValTextboxPressed = true;

	renderPlane = false;
	renderSphere = false;

	if(vertices.size() > 10000)
		glset.bufferData(vertices);
	else {
		glBufferData(GL_ARRAY_BUFFER, 10000, NULL, GL_DYNAMIC_DRAW);
	}


}
void LigidPainter::autoTriangulateCheckBox(){
	if (autoTriangulateChecked == false)
		autoTriangulateChecked = true;
	else
		autoTriangulateChecked = false;
}
void LigidPainter::backfaceCullingCheckBox() {
	if (backfaceCullingChecked == false) {
		backfaceCullingChecked = true;
		enableBackfaceCulling = false;
	}
	else {
		enableBackfaceCulling = true;
		backfaceCullingChecked = false;
	}
}
void LigidPainter::useNegativeForDrawingCheckbox(){
	if (useNegativeForDrawing == false) {
		useNegativeForDrawing = true;
	}
	else {
		useNegativeForDrawing = false;
	}
}
void LigidPainter::loadModelButton() {
	renderPlane = false;
	renderSphere = false;

	loadModelButtonPressed = true;
	Texture txtr;
	txtr.refreshScreenDrawingTexture(reduceScreenPaintingQuality);
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
void LigidPainter::paintingDropper(){
	paintingDropperPressed = true;
}
void LigidPainter::paintingFillNumericModifier(bool p, bool n){
	Texture txtr;
	if(p && paintingFillNumericModifierVal < 10)
		paintingFillNumericModifierVal++;
	if(n && paintingFillNumericModifierVal > 1)
		paintingFillNumericModifierVal--;
	
	if(paintingFillNumericModifierVal < 8){
		reduceScreenPaintingQuality = false;
	}
	else{
		reduceScreenPaintingQuality = true;
	}
	brushValChanged = true;
	txtr.refreshScreenDrawingTexture(reduceScreenPaintingQuality);
}
void LigidPainter::maskPanelSlider(float yOffset,int screenSizeY){
	Utilities util;
	maskPanelSliderValue += yOffset / (screenSizeY / 2);
	maskPanelSliderValue = util.restrictBetween(maskPanelSliderValue, 0.0f, -0.25f);//Keep in boundaries
}
void LigidPainter::hexValTextbox(){
	hexValTextboxPressed = true;
}





//-----------------------------PREPARE STRUCTS-----------------------------\\



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
	renderData.brushBorderRangeBarValue = brushBorderRangeBarValue;
	
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

	uidata.useNegativeForDrawingCheckboxEnter = callbackData.useNegativeForDrawingCheckboxEnter;
	uidata.useNegativeForDrawingCheckboxPressed = useNegativeForDrawing;
	
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

	uidata.paintingFillNumericModifierPEnter = callbackData.paintingFillNumericModifierPEnter;
	uidata.paintingFillNumericModifierNEnter = callbackData.paintingFillNumericModifierNEnter;

	uidata.dropperEnter = callbackData.paintingDropperEnter;

	uidata.exportFileNameTextBoxPressed = exportFileNameTextBoxPressed;

	uidata.hexValTextboxPressed = hexValTextboxPressed;
	
	uidata.customModelButtonHover = callbackData.loadCustomModelEnter;

	return uidata;
}