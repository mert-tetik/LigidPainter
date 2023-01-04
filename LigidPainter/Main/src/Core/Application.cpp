#include<iostream>

#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtx/string_cast.hpp"

#include "Core/LigidPainter.h"
#include "Core/UI/UserInterface.h"
#include "Core/Utilities.h"
#include "Core/Callback.h"
#include "Core/gl.h"
#include "Core/Load.hpp"
#include "Core/Render/Render.h"
#include "Core/Model/model.h"
#include "Core/Texture/Texture.h"
#include "Core/UI/UiActions.h"
#include "Core/Texture Generator/TextureGenerator.h"
#include "Core/messageBox.h"


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


#if defined(_WIN32) || defined(_WIN64)
    char folderDistinguisher = '\\';
#else
	char folderDistinguisher = '/'; 
#endif

BrushMaskTextures brushMaskTextures;
std::vector<unsigned int> albedoTextures;

//TODO : Reduce GPU Usage
//TODO : Specialized vao for each shader
//TODO : Use texture struct


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
//GL_TEXTURE11 = Painted mask
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
const char* customModelFilePath;
string albedoTexturePath = "albedoImage.png";
string maskTexturePath = "./LigidPainter/Resources/Textures/PlainCircle.png";
string exportPath = "";
string exportFolder = "Choose Destination Path";
string exportFileName = "LP_Export";
//Paths

std::vector<UIElement> UIElements;
ColorPicker colorPicker;
TextureDisplayer textureDisplayer;


string modelName;
string customModelName; 

//--------Functions--------\\

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void scroll_callback(GLFWwindow* window, double scroll, double scrollx);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void updateCameraPosChanging();
RenderData updateRenderData(RenderData renderData, unsigned int depthTexture, int brushSizeIndicatorSize);
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

bool mainPanelHover = false;


bool enablePanelMovement = true; //Panel can be moved if true. Set false while dragging range bar pointers around.


//Used to let render function know if it's supposed to change colors


//Checkbox
bool autoTriangulateChecked = true;

bool backfaceCullingChecked = true;
bool enableBackfaceCulling;

bool jpgFormatChecked;
bool pngFormatChecked = true;

bool mirrorUsed = false;
//Checkbox

float maskPanelSliderValue = 0.0f;


//bool albedoTextureChanged; use for texture updating conditions

int paintingSpacing = 1;

//Last mouse position (used in drawToScreen.cpp)
double lastMouseXpos = 0;
double lastMouseYpos = 0;

bool useNegativeForDrawing;

bool panelChanging = false; //Disable painting while changing panel sizes

bool brushValChanged = true; //Update brush mask texture in drawToScreen.cpp if true


bool caps = false; //GLFW_MOD_CAPS_LOCK

bool mirrorClick = false;

int textBoxActiveChar = 6;

bool renderTheScene = true;//Set true in the callback functions & set renderTheSceneCounter to 0 if renderTheScene's state is changed
int renderTheSceneCounter = 0;
const int renderingThreshold = 120;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	//TODO : Dynamic textbox

	//------------------TEXT------------------
	
	renderTheScene = true;
	renderTheSceneCounter = 0;

	if(key == 280 && action == 0){
		//Change capslock's state
		caps = !caps;
	}

	UserInterface ui;
	const int exportFileNameThreshold = 20;
	if(UIElements[UIexportingFolderNameTextBox].textBox.clicked){
		if(ui.textInput(key,action,caps,exportFileName,exportFileNameThreshold)){
			UIElements[UIexportingFolderNameTextBox].textBox.text = exportFileName;
		}
	}
	
	if(colorPicker.hexValTextBoxActive){
		if(ui.textInputHex(key,action,colorPicker.hexValTextBoxVal,textBoxActiveChar)){
			colorPicker.saturationValueValChanged = true;
			colorPicker.hexValTextBoxGotInput = true;
		}
	}


	//------------------SHORTCUTS------------------

	Utilities util;

	bool shiftTabAltRelease =  glfwGetKey(window, GLFW_KEY_TAB) == GLFW_RELEASE && glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE && glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_RELEASE;

	//Ctrl + x use negative checkbox
	if(util.shortCut(window,GLFW_KEY_LEFT_CONTROL,GLFW_KEY_X,NULL,NULL) && panelData.paintingPanelActive && action == 1 && shiftTabAltRelease){
		if(useNegativeForDrawing){
			useNegativeForDrawing = false;
		}
		else{
			useNegativeForDrawing = true;
		}
	}

	//Ctrl + h change texture demonstrator's state
	if(util.shortCut(window,GLFW_KEY_LEFT_CONTROL,GLFW_KEY_H,NULL,NULL) && action == 1 && shiftTabAltRelease){
		textureDisplayer.buttonClicked = true;
	}

	LigidPainter lp;
	//Ctrl + Tab + q switch to model panel


	if(util.shortCut(window,GLFW_KEY_LEFT_CONTROL,GLFW_KEY_TAB,GLFW_KEY_Q,NULL) && action == 1){
		lp.modelPanelButton();
	}
	//Ctrl + Tab + w switch to texture panel
	if(util.shortCut(window,GLFW_KEY_LEFT_CONTROL,GLFW_KEY_TAB,GLFW_KEY_W,NULL) && action == 1){
		lp.texturePanelButton();
	}
	//Ctrl + Tab + t switch to model panel
	if(util.shortCut(window,GLFW_KEY_LEFT_CONTROL,GLFW_KEY_TAB,GLFW_KEY_T,NULL) && action == 1){
		lp.paintingPanelButton();
	}
	//Ctrl + Tab + r switch to model panel
	if(util.shortCut(window,GLFW_KEY_LEFT_CONTROL,GLFW_KEY_TAB,GLFW_KEY_R,NULL) && action == 1){
		lp.exportPanelButton();
	}
}


Programs programs;


bool renderSphere = false;
bool renderPlane = false;

glm::vec3 drawColor;

float brushBlurVal = 1;


bool verticalMirror = false;

OutShaderData outShaderData;


int screenWidth;
int screenHeight;

Model model;


float materialsPanelSlideValue = 0.0f;

float mainPanelLoc = 1.6f;
bool LigidPainter::run()
{
	ColorData colorData;
	UserInterface ui;
	Texture txtr;
	RenderData renderData;
	TextureGenerator textureGen;
	Callback callback;
	Render render;
	UiActions uiAct;
	InitializedTextures textures;
	RenderOutData renderOut;
	Utilities util;

	windowData = glset.getWindow();
	window = windowData.window;

	//Set Callbacks
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback); 
	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	//Changes clearColor
	ui.setViewportBgColor();
	
	Load load;
	programs = load.getProgram();
	//Load chars
	load.uploadChars();
	//Load brush mask textures
	brushMaskTextures = load.loadBrushMaskTextures();
	//Load cubemaps both blury and not blury
	load.loadCubemaps();
	//Load icons
	Icons icons;
	icons = load.loadIcons();
	//Load cursors
	LigidCursors cursors;
	cursors = load.loadCursors();
	//Create textures
	textures = load.initTextures(maskTexturePath.c_str());
	//Load UI
	UIElements = ui.getUiElements(icons);


	glGenBuffers(1, &VBO);
	//glGenVertexArrays(1, &VAO);
	//glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glUseProgram(programs.uiProgram);

	//Send some data to related cpp files
	//Send programs struct
	ui.sendProgramsToUserInterface(programs);
	render.sendProgramsToRender(programs);
	txtr.sendProgramsToTextures(programs);
	
	//Send max window size
	callback.sendMaxWindowSize(windowData.windowMaxWidth,windowData.windowMaxHeight);
	ui.sendMaxWindowSize(windowData.windowMaxWidth,windowData.windowMaxHeight);
	uiAct.sendMaxWindowSize(windowData.windowMaxWidth,windowData.windowMaxHeight);
	render.sendMaxWindowSize(windowData.windowMaxWidth,windowData.windowMaxHeight);
	txtr.sendMaxWindowSize(windowData.windowMaxWidth,windowData.windowMaxHeight);

	renderData.window = window;

	glset.setVertexAtribPointer();
	glBufferData(GL_ARRAY_BUFFER, 10000, NULL, GL_DYNAMIC_DRAW); 

	glUseProgram(programs.iconsProgram);
	glset.uniform1i(programs.iconsProgram, "icon", 6);
	glUseProgram(programs.uiProgram);


	glset.uniform3fv(programs.uiProgram,"textColor",glm::vec3(colorData.textColor));
	glset.uniform1i(programs.uiProgram, "text", 2);
	glset.uniform1i(programs.uiProgram, "currentTexture", 0);
	glset.uniform1i(programs.uiProgram, "modifiedMaskTexture", 12);

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); //Wireframe

	//Create screen painting mask Texture
	GLubyte* screenTexture = new GLubyte[(windowData.windowMaxWidth) * (windowData.windowMaxHeight)];
	ScreenPaintingReturnData screenPaintingReturnData; 
	screenPaintingReturnData = txtr.createScreenPaintTexture(screenTexture,window);
	delete(screenTexture);

	//Create a framebuffer (Will be used to reading from screen)
	FBOScreen = glset.createScreenFrameBufferObject();

	
	glset.enable(GL_BLEND);
	glDepthFunc(GL_LESS);
	glset.enable(GL_DEPTH_TEST);
	glset.enable(GL_MULTISAMPLE);


	glm::mat4 perspectiveProjection = render.setMatrices();


	float brushSize;
	ExportData exportData;

	PBRShaderData pbrShaderData;
	SkyBoxShaderData skyBoxShaderData;
	ScreenDepthShaderData screenDepthShaderData;
	AxisPointerShaderData axisPointerShaderData;
	//OutShaderData outShaderData; = global


	//Process the default mask brush
	glfwGetWindowSize(window, &screenWidth, &screenHeight);
	txtr.updateMaskTexture(FBOScreen, screenWidth, screenHeight, UIElements[UIbrushRotationRangeBar].rangeBar.value,false,UIElements[UIbrushBordersRangeBar].rangeBar.value,brushBlurVal,outShaderData,programs,windowData.windowMaxWidth,windowData.windowMaxHeight);


	panelData.modelPanelActive = true; //Active the model panel by default

	ViewUpdateData viewUpdateData;

	//Framebuffer used in drawToScreen.cpp
	unsigned int paintingFBO;
	glset.genFramebuffers(paintingFBO);
	glset.bindFramebuffer(paintingFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,  screenPaintingReturnData.normalId, 0);

	unsigned int RBO;
	glset.genRenderbuffers(RBO);
	glset.bindRenderBuffer(RBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, windowData.windowMaxWidth, windowData.windowMaxHeight);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, RBO);

	glClear(GL_DEPTH_BUFFER_BIT);

	glset.bindFramebuffer(0);
	glset.bindRenderBuffer(0);


	pbrShaderData.bluryskybox = 16;
	pbrShaderData.depthTexture = 9;
	pbrShaderData.materialDiffuse = 0;
	pbrShaderData.mirroredDepthTexture = 8;
	pbrShaderData.mirroredScreenMaskTexture = 3;
	pbrShaderData.screenMaskTexture = 4;

	skyBoxShaderData.skybox = 13;
	skyBoxShaderData.projection = perspectiveProjection;

	axisPointerShaderData.projection = perspectiveProjection;


	glfwMakeContextCurrent(window);

	bool paintRender = false;
	int paintRenderCounter = 0;


	double mouseDrawingPosX = 0;
	double mouseDrawingPosY = 0;
		

	while (!glfwWindowShouldClose(window))//Main loop
	{
		
		glfwPollEvents();
		glfwSwapInterval(1);

		//util.printRenderingSpeed();

		//Check if camera pos changed
		updateCameraPosChanging();

		renderTheSceneCounter++;

		if(glfwGetMouseButton(window, 0) == GLFW_PRESS || glfwGetMouseButton(window, 1) == GLFW_PRESS || glfwGetMouseButton(window, 2) == GLFW_PRESS){
			renderTheScene = true;
			renderTheSceneCounter = 0;
		}


		//Release textboxes
		if ((glfwGetMouseButton(window, 0) == GLFW_PRESS || glfwGetMouseButton(window, 1) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS)){
			if(!UIElements[UIexportingFolderNameTextBox].textBox.hover && UIElements[UIexportingFolderNameTextBox].textBox.clicked){
				UIElements[UIexportingFolderNameTextBox].textBox.clicked = false; 
				if(exportFileName == ""){
					exportFileName = "LP_Export";
					UIElements[UIexportingFolderNameTextBox].textBox.text = exportFileName;

				}	
			}
			if(!colorPicker.hexValTextBoxEnter || glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS){
				colorPicker.hexValTextBoxActive = false;
				textBoxActiveChar = 6;
			}
		}



		//Check if texture demonstrator button clicked
		if(textureDisplayer.buttonPressed){
			textureDisplayer.buttonPressedCounter++;
			textureDisplayer.buttonPressed = true;
		}
		if(textureDisplayer.buttonPressedCounter < 20 && textureDisplayer.buttonPressed && glfwGetMouseButton(window, 0) == GLFW_RELEASE && !textureDisplayer.positionChanged){
			textureDisplayer.buttonClicked = true;
			textureDisplayer.buttonPressed = false;

		}
		if(glfwGetMouseButton(window, 0) == GLFW_RELEASE){
			textureDisplayer.buttonPressedCounter = 0;
			textureDisplayer.buttonPressed = false;
			textureDisplayer.positionChanged = false;
		}

		//Ui actions
		uiAct.uiActions(window,callbackData,UIElements,colorPicker,textureDisplayer);

		//Change color picker's value
		if((colorPicker.dropperActive && glfwGetMouseButton(window, 0) == GLFW_PRESS) || (colorPicker.saturationValueBoxClicked && !colorPicker.saturationValuePointerHover) || (colorPicker.hueBarClicked && !colorPicker.saturationValuePointerHover && !colorPicker.huePointerChanging)|| colorPicker.hexValTextBoxGotInput){
			if(colorPicker.hexValTextBoxGotInput){
				updateColorPicker(util.hexToRGBConverter(colorPicker.hexValTextBoxVal),true,true);//Update colorbox val once color picker hex value textbox value changed
			}
			else{
				updateColorPicker(renderOut.mouseHoverPixel,true,!colorPicker.hueBarClicked);//Update colorbox val once dropper is used or colorbox is clicked
			}
		}
		if(glfwGetMouseButton(window, 0) == GLFW_RELEASE)
			colorPicker.huePointerChanging = false;
		if(colorPicker.saturationValueBoxClicked)
			colorPicker.saturationValueBoxClicked = false;
		if(colorPicker.hueBarClicked)
			colorPicker.hueBarClicked = false;
		if(colorPicker.saturationValuePointerChanging)
			colorPicker.saturationValuePointerChanging = false;



		//Update
		brushSize = double(UIElements[UIbrushSizeRangeBar].rangeBar.value + 0.1f) * 800.0f + 20.0f ;
		renderData = updateRenderData(renderData,textures.depthTexture, (int)brushSize);
		panelData.movePanel = callbackData.movePanel;
		exportData.exportImage = exportImage;
		exportData.path = exportPath.c_str();
		exportData.fileName = exportFileName.c_str();
		if (cameraPosChanging || mirrorClick){
			viewUpdateData = render.updateViewMatrix(callbackData.cameraPos, callbackData.originPos,UIElements[UImirrorXCheckBox].checkBox.checked ,UIElements[UImirrorYCheckBox].checkBox.checked ,UIElements[UImirrorZCheckBox].checkBox.checked ); 
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

		outShaderData.depthTexture = 9;
		outShaderData.drawColor = drawColor;
		outShaderData.interpretWithUvMask;//
		outShaderData.isRenderScreenMaskMode;//
		outShaderData.isTwoDimensional;
		outShaderData.mirroredDepthTexture = 8;
		outShaderData.mirroredScreenMaskTexture = 3;
		outShaderData.mirroredView = viewUpdateData.mirroredView;
		outShaderData.mirroredViewPos = viewUpdateData.mirroredCameraPos;
		outShaderData.modifiedMaskTexture = 12;
		outShaderData.paintedTxtrMask = 11;
		outShaderData.projection = perspectiveProjection; // 
		outShaderData.renderDepth; //****
		outShaderData.renderMaskBrush; //
		outShaderData.renderTextureProjection; //
		outShaderData.renderTrans; //
		outShaderData.screenMaskTexture = 4; //
		outShaderData.useMirror = mirrorUsed;
		outShaderData.uvMask = 7;
		outShaderData.verticalMirror = verticalMirror; //
		outShaderData.view = viewUpdateData.view;
		outShaderData.viewPos = viewUpdateData.cameraPos;



		//Exit message box
		if(glfwWindowShouldClose(window)){
			bool noButtonClick = true;
			bool clickTaken = false;

			panelData.exportPanelActive = false;
			panelData.paintingPanelActive = false;
			panelData.texturePanelActive = false;
			panelData.modelPanelActive = false;

		 	while (true)
		 	{
				//Disable painting
				doPainting = false; 
				renderData.doPainting = doPainting;

		 		glfwPollEvents();

				//Keep rendering the backside
		 		renderOut = render.render(renderData, FBOScreen, panelData,exportData,icons,maskPanelSliderValue,brushMaskTextures.textures,renderPlane,renderSphere,pbrShaderData,skyBoxShaderData,brushBlurVal,screenDepthShaderData,axisPointerShaderData,outShaderData,model,albedoTextures,paintRender,materialsPanelSlideValue,UIElements,colorPicker,textureDisplayer);
		 		
				
				float messageBoxBackColor[3] = {colorData.messageBoxPanelColor.r,colorData.messageBoxPanelColor.g,colorData.messageBoxPanelColor.r};

				float messageBoxButtonColor[3] = {colorData.messageBoxButtonColor.r,colorData.messageBoxButtonColor.g,colorData.messageBoxButtonColor.r};

				//render the message box
				int result = lgdMessageBox(window,mouseXpos,mouseYpos,cursors.defaultCursor,cursors.pointerCursor,icons.Logo,programs.uiProgram,"LigidPainter will be closed. Do you want to proceed?",-0.21f,0.0f,messageBoxBackColor,messageBoxButtonColor,(float)windowData.windowMaxWidth, (float)screenWidth,programs.iconsProgram); //0 = Yes //1 = No //2 = None

				//Process the message box input
				if(result == 0 || glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS){
					break; //Close the app
				}
				else if(result == 1 || glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS){
					panelData.modelPanelActive = true;
					glfwSetWindowShouldClose(window,GLFW_FALSE);
					break; //Act like nothing happened
				}
	
				glfwSwapBuffers(window);
		 	}
		}

		


		//Render
		//double firstTime = glfwGetTime();
		if(renderTheScene){
			renderOut = render.render(renderData, FBOScreen, panelData,exportData,icons,maskPanelSliderValue,brushMaskTextures.textures,renderPlane,renderSphere,pbrShaderData,skyBoxShaderData,brushBlurVal,screenDepthShaderData,axisPointerShaderData,outShaderData,model,albedoTextures,paintRender,materialsPanelSlideValue,UIElements,colorPicker,textureDisplayer);
		}
		
		//double lastTime = glfwGetTime();
		//cout <<  (lastTime - firstTime) * 1000  << '\n';
		
		paintRender = false;

		//
		if(panelData.paintingPanelActive)
			colorPicker.updateHueVal = false; //Hue value will be updated after rendering the ui

		drawColor = colorPicker.pickerValue/255.0f;
		colorPicker.saturationValueValChanged = false;
		colorPicker.hexValTextBoxGotInput = false;


		//Painting
		bool distance_spacingCompatibility = glm::distance(glm::vec2(mouseDrawingPosX,mouseDrawingPosY),glm::vec2(mouseXpos,mouseYpos)) > paintingSpacing;
		if (glfwGetMouseButton(window, 0) == GLFW_PRESS && doPainting && !panelChanging && glfwGetMouseButton(window, 1) == GLFW_RELEASE && !colorPicker.dropperActive && distance_spacingCompatibility){

			mouseDrawingPosX = mouseXpos;
			mouseDrawingPosY = mouseYpos;

			//Paint
			textureGen.drawToScreen(window, screenPaintingReturnData.normalId, brushSize, FBOScreen,UIElements[UIbrushRotationRangeBar].rangeBar.value,UIElements[UIbrushOpacityRangeBar].rangeBar.value,lastMouseXpos, lastMouseYpos,mouseXpos,mouseYpos,mirrorUsed,useNegativeForDrawing,brushValChanged,programs,windowData.windowMaxWidth,windowData.windowMaxHeight,UIElements[UIbrushBordersRangeBar].rangeBar.value,brushBlurVal,paintingFBO,outShaderData,model,albedoTextures, paintingSpacing < 10);
			paintRenderCounter++;
			if(paintRenderCounter == 5){
				paintRender = true;
				paintRenderCounter = 0;
			}

			brushValChanged = false; //After updating the brush mask texture set brushValChanged to false so brush mask texture won't be updated repeatedly 
			paintingMode = true;
		}

		panelChanging = false;
		lastMouseXpos = mouseXpos;
		lastMouseYpos = mouseYpos;




		//Update brush mask texture file's name once the brush mask texture changed
		if(renderOut.maskPanelMaskClicked){
			for (size_t i = 0; i < brushMaskTextures.textures.size(); i++)
			{
				if(brushMaskTextures.textures[i] == renderOut.currentBrushMaskTxtr){
					//Restrict brush mask texture's name (20 chars)
					std::string maskTextureName = util.getLastWordBySeparatingWithChar(brushMaskTextures.names[i],folderDistinguisher);  
					UIElements[UImaskTextureFileNameText].text.text = util.cropString(maskTextureName,20); 
				}
			}
		}
		if(renderOut.maskPanelMaskClicked){
			//If a new mask texture is selected from mask texture panel set brushValChanged to true to update brush mask texture 
			brushValChanged = true;
		}



		//After rendering
		exportImage = false; //After exporting, set exportImage false so we won't download the texture repeatedly
		textureDisplayer.buttonClicked = false;




		if (mousePosChanged) { //To make sure painting done before changing camera position
			callbackData = callback.mouse_callback(window, mouseXpos, mouseYpos, panelData,maskPanelSliderValue,renderOut.maskPanelMaskHover,cursors,renderOut.texturePanelButtonHover,UIElements,mainPanelLoc,colorPicker,textureDisplayer);
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
		if(renderTheScene)
			glfwSwapBuffers(window);

		if(renderTheSceneCounter == renderingThreshold)
			renderTheScene = false;
	}
	
	//Close the program
	glfwDestroyWindow(window);
	glfwTerminate();
	return true;
}







//-------------CALLBACK-------------\\

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	renderTheScene = true;
	renderTheSceneCounter = 0;

	Callback callback;
	callback.framebuffer_size_callback(window,width,height);

	screenWidth = width;
	screenHeight = height;
}

double lastXpos;
double lastYpos;
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	renderTheScene = true;
	renderTheSceneCounter = 0;

	UiActions uiAct; 
	mousePosChanged = true;
	mouseXpos = xpos;
	mouseYpos = ypos;
	double xOffset;
	double yOffset;
	
	int width;
	int height;
	glfwGetWindowSize(window,&width,&height);
	
	//Get mouse position offset
	xOffset = (lastXpos - xpos) / ((double)windowData.windowMaxWidth / (double)width);
	yOffset = (lastYpos - ypos) / ((double)windowData.windowMaxHeight / (double)height);
	lastXpos = xpos;
	lastYpos = ypos;
	

	float screenGapX = ((float)windowData.windowMaxWidth - (float)width); 



	//Texture demonstrator corner hover
	float range = 0.05f;
	if(xpos > ((textureDisplayer.buttonPosX + textureDisplayer.width) - range) * width/2 && xpos < ((textureDisplayer.buttonPosX + textureDisplayer.width) + range) * width/2 && height - ypos > ((textureDisplayer.buttonPosY+1.0f - textureDisplayer.height) - range) * height/2 && height - ypos < ((textureDisplayer.buttonPosY+1.0f - textureDisplayer.height) + range) * height/2 ){
		textureDisplayer.cornerHover = true;
	}
	else{
		textureDisplayer.cornerHover = false;
	}


	
	bool hideCursor = uiAct.updateRangeValues(window,xOffset,yOffset,width,height,textureDisplayer); 



	if(colorPicker.dropperActive){
		doPainting = false;
	}
	else if(textureDisplayer.buttonHover || callbackData.paintingPanelButtonEnter || callbackData.modelPanelButtonEnter || callbackData.exportPanelButtonEnter || callbackData.texturePanelButtonEnter || UIElements[UImirrorZCheckBox].checkBox.mouseHover || UIElements[UImirrorZCheckBox].checkBox.mouseHover || UIElements[UImirrorZCheckBox].checkBox.mouseHover){
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
		if (callbackData.mainPanelEnter){ 
			//Inside of the panel
			mainPanelHover = true;
			doPainting = false;
		} 
		else if(panelData.paintingPanelActive){
			//Painting panel + outside of panel
			mainPanelHover = false;
			doPainting = true;
		}
	}
}

void scroll_callback(GLFWwindow* window, double scroll, double scrollx)
{
	renderTheScene = true;
	renderTheSceneCounter = 0;

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
		if(callbackData.maskPanelEnter){
			//Brush mask panel scroll
			maskPanelSliderValue += (float)(scrollx / 40.0);
			const float maskPanelRange = ceil((int)brushMaskTextures.textures.size()/3.f) / 8.33333333333 - (0.8f - 0.55f); 
			maskPanelSliderValue = util.restrictBetween(maskPanelSliderValue, 0.0f, -maskPanelRange/4.f);//Keep in boundaries
		}
		else if (!paintingMode && !mainPanelHover) {
			callbackData = callback.scroll_callback(window, scroll, scrollx);
		}
		else if(mainPanelHover && panelData.texturePanelActive){
			//Materials
			materialsPanelSlideValue += scrollx/10.0f;
			if(materialsPanelSlideValue > 0.0f)
				materialsPanelSlideValue = 0.0f;
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


void updateColorPicker(glm::vec3 RGBval,bool changeHue,bool changeSatVal){
	int width;
	int height;
	glfwGetWindowSize(window,&width,&height);
	
	Utilities util;
	
	colorPicker.saturationValueValChanged = true;
	glm::vec3 hsvVal = util.RGBToHSVGenerator(RGBval);

	drawColor = RGBval/glm::vec3(255.0f);
	
	if(changeHue){
		colorPicker.hueValue = (hsvVal.r / 708.333333333f) - 0.18f; //0.195
		colorPicker.updateHueVal = true;
	}
	if(changeSatVal){
		colorPicker.saturationValuePosX = (hsvVal.g / 1342.10526316f) - 0.095f; //0.095
		colorPicker.saturationValuePosY = (hsvVal.b / 653.846153846f) - 0.195f; //0.195
	}
	colorPicker.dropperActive = false;
}

//-----------------------------UI ACTIONS-----------------------------\\

void LigidPainter::addMaskTextureButton() {
	//Needed for updating mask texture
	int width;
	int height;
	glfwGetWindowSize(window, &width, &height);

	GlSet glset;
	Texture txtr;
	Utilities util;

	//Filter
	char const* lFilterPatterns[3] = { "*.jpg", "*.png", "*.jpeg" };

	//Open the "open file dialog"
	auto maskTexturePathCheck = tinyfd_openFileDialog("Select Mask Texture", "", 3, lFilterPatterns, "", false);


	if (maskTexturePathCheck) { 
		maskTexturePath = maskTexturePathCheck;
		UIElements[UImaskTextureFileNameText].text.text = util.getLastWordBySeparatingWithChar(maskTexturePath,folderDistinguisher); 
		brushValChanged = true;
		glset.activeTexture(GL_TEXTURE1);

		brushMaskTextures.textures.push_back(txtr.getTexture(maskTexturePath,0,0,false));
		brushMaskTextures.names.push_back(maskTexturePath);

		txtr.updateMaskTexture(FBOScreen,width,height,UIElements[UIbrushRotationRangeBar].rangeBar.value,false,UIElements[UIbrushBordersRangeBar].rangeBar.value,brushBlurVal,outShaderData,programs,windowData.windowMaxWidth,windowData.windowMaxHeight);
	}
}
void LigidPainter::brushSizeRangeBar(double xOffset,int width){
	brushValChanged = true;
	Utilities util;
	UIElements[UIbrushSizeRangeBar].rangeBar.value -= xOffset / (width / 2);
	UIElements[UIbrushSizeRangeBar].rangeBar.value = util.restrictBetween(UIElements[UIbrushSizeRangeBar].rangeBar.value, 0.11f, -0.11f);//Keep in boundaries
}
void LigidPainter::colorBox(){
	colorPicker.saturationValueBoxClicked = true;
}
void LigidPainter::hueBar(){
	colorPicker.hueBarClicked = true;
}
void LigidPainter::brushBlurRangeBar(double xOffset,int width,int height,bool renderTiny) {
	Utilities util;
	Texture txtr;
	brushValChanged = true;
	
	UIElements[UIbrushBlurRangeBar].rangeBar.value -= xOffset / (width / 2);
	UIElements[UIbrushBlurRangeBar].rangeBar.value = util.restrictBetween(UIElements[UIbrushBlurRangeBar].rangeBar.value, 0.11f, -0.11f);//Keep in boundaries
	brushBlurVal = ((UIElements[UIbrushBlurRangeBar].rangeBar.value + 0.11f) * 545.454545455f) + 1.0f; //Max 120

	txtr.updateMaskTexture(FBOScreen,width,height, UIElements[UIbrushRotationRangeBar].rangeBar.value,renderTiny,UIElements[UIbrushBordersRangeBar].rangeBar.value,brushBlurVal,outShaderData,programs,windowData.windowMaxWidth,windowData.windowMaxHeight);

}
void LigidPainter::textureDisplayerButton(double xOffset,double yOffset,int width,int height) {
	panelChanging = true;
	Utilities util;

	textureDisplayer.positionChanged = true;

	textureDisplayer.buttonPosX -= xOffset / (double)(windowData.windowMaxWidth / 2.0f);
	textureDisplayer.buttonPosX = util.restrictBetween(textureDisplayer.buttonPosX,2.0f,0.0f);
	textureDisplayer.buttonPosY += yOffset / (double)(windowData.windowMaxHeight / 2.0f);
	textureDisplayer.buttonPosY = util.restrictBetween(textureDisplayer.buttonPosY,0.97f,-1.0f);

}
void LigidPainter::textureDisplayerBoundaries(double xOffset,double yOffset,int width,int height) {
	panelChanging = true;
	textureDisplayer.width -= xOffset / 960.0;
	textureDisplayer.height -= yOffset / 540.0;
}
void LigidPainter::brushRotationRangeBar(double xOffset, int width, int height){
	Utilities util;
	Texture txtr;
	brushValChanged = true;
	UIElements[UIbrushRotationRangeBar].rangeBar.value -= xOffset / (width / 2.0f);
	UIElements[UIbrushRotationRangeBar].rangeBar.value = util.restrictBetween(UIElements[UIbrushRotationRangeBar].rangeBar.value, 0.11f, -0.11f);//Keep in boundaries
	txtr.updateMaskTexture(FBOScreen, width, height,UIElements[UIbrushRotationRangeBar].rangeBar.value,true,UIElements[UIbrushBordersRangeBar].rangeBar.value,brushBlurVal,outShaderData,programs,windowData.windowMaxWidth,windowData.windowMaxHeight);
}
void LigidPainter::brushOpacityRangeBar(double xOffset, int width, int height) {
	Utilities util;
	Texture txtr;
	//brushOpacityChanged = true; not used
	UIElements[UIbrushOpacityRangeBar].rangeBar.value -= xOffset / (width / 2.0f);
	UIElements[UIbrushOpacityRangeBar].rangeBar.value = util.restrictBetween(UIElements[UIbrushOpacityRangeBar].rangeBar.value, 0.11f, -0.11f);//Keep in boundaries
}
void LigidPainter::brushSpacingRangeBar(double xOffset, int width, int height) {
	Utilities util;
	Texture txtr;
	UIElements[UIbrushSpacingRangeBar].rangeBar.value -= xOffset / (width / 2.0f);
	UIElements[UIbrushSpacingRangeBar].rangeBar.value = util.restrictBetween(UIElements[UIbrushSpacingRangeBar].rangeBar.value, 0.11f, -0.11f);//Keep in boundaries
	paintingSpacing = ((UIElements[UIbrushSpacingRangeBar].rangeBar.value + 0.11f) * 454.545454545f) + 1.0f; //-0.11 - 0.11 --> 1 - 101
}
void LigidPainter::brushBordersRangeBar(double xOffset, int width, int height) {
	Utilities util;
	Texture txtr;
	brushValChanged = true;
	UIElements[UIbrushBordersRangeBar].rangeBar.value -= xOffset / (width / 2.0f);
	UIElements[UIbrushBordersRangeBar].rangeBar.value = util.restrictBetween(UIElements[UIbrushBordersRangeBar].rangeBar.value, 0.11f, -0.11f);//Keep in boundaries
	txtr.updateMaskTexture(FBOScreen, width, height,UIElements[UIbrushRotationRangeBar].rangeBar.value,true,UIElements[UIbrushBordersRangeBar].rangeBar.value,brushBlurVal,outShaderData,programs,windowData.windowMaxWidth,windowData.windowMaxHeight);
}
void LigidPainter::colorBoxColorRangeBar(double yOffset,int height){
	Utilities util;
	colorPicker.huePointerChanging = true;
	colorPicker.updateHueVal = true;
	colorPicker.hueValue += yOffset / (height / 2.0f);
	colorPicker.hueValue = util.restrictBetween(colorPicker.hueValue, 0.180f, -0.180f);//Keep in boundaries
	colorPicker.saturationValueValChanged = true;
}
void LigidPainter::colorBoxPickerButton(double xOffset, double yOffset, int width, int height) {
	colorPicker.saturationValuePointerChanging = true;
	Utilities util;
	colorPicker.saturationValuePosX -= xOffset / (width / 2.0f);
	colorPicker.saturationValuePosX = util.restrictBetween(colorPicker.saturationValuePosX, 0.099f, -0.1f);//Keep in boundaries
	colorPicker.saturationValuePosY += yOffset / (height / 2.0f);
	colorPicker.saturationValuePosY = util.restrictBetween(colorPicker.saturationValuePosY, 0.199f, -0.2f);//Keep in boundaries
	colorPicker.saturationValueValChanged = true;
}

void LigidPainter::modelFilePathTextBox() {
	Utilities utilities;

	//Filter
	char const* lFilterPatterns[11] = { "*.obj","*.gltf", "*.fbx", "*.stp", "*.max","*.x3d","*.obj","*.vrml","*.3ds","*.stl","*.dae" };
	
	auto modelFilePathCheck = tinyfd_openFileDialog("Select 3D Model","",11, lFilterPatterns,"",false);

	if (modelFilePathCheck) {
		modelFilePath = modelFilePathCheck;
		modelName = utilities.getLastWordBySeparatingWithChar(modelFilePath,folderDistinguisher);
		UIElements[UIUploadingModelPathTextBox].textBox.text = modelName; 
		customModelName = modelName;
	}
}

void LigidPainter::exportPathTextBox() {
	Utilities uti;
	auto exportPathCheck = tinyfd_selectFolderDialog("Save Texture","");
	if (exportPathCheck) {
		exportPath = exportPathCheck;
		exportFolder = uti.getLastWordBySeparatingWithChar(exportPath,folderDistinguisher);
		UIElements[UIexportingPathTextBox].textBox.text = exportFolder;
	}
}
void LigidPainter::exportFileNameTextBox() {

}
void LigidPainter::exportExtJPGCheckBox() {
	if (jpgFormatChecked == false) {
		jpgFormatChecked = true;
		pngFormatChecked = false;
		UIElements[UIjpgCheckBox].checkBox.checked = true;
		UIElements[UIpngCheckBox].checkBox.checked = false;
	}
}
void LigidPainter::exportExtPNGCheckBox() {
	if (pngFormatChecked == false) {
		pngFormatChecked = true;
		jpgFormatChecked = false;
		UIElements[UIpngCheckBox].checkBox.checked = true;
		UIElements[UIjpgCheckBox].checkBox.checked = false;
	}
}
void LigidPainter::mirrorXCheckBox() {
	mirrorClick = true;
	if (UIElements[UImirrorXCheckBox].checkBox.checked  == false) {
		mirrorUsed = true;
		UIElements[UImirrorXCheckBox].checkBox.checked  = true;
		UIElements[UImirrorYCheckBox].checkBox.checked  = false;
		UIElements[UImirrorZCheckBox].checkBox.checked  = false;
		verticalMirror = false;
	}
	else {
		mirrorUsed = false;
		UIElements[UImirrorXCheckBox].checkBox.checked  = false;
	}

}
void LigidPainter::mirrorYCheckBox() {
	mirrorClick = true;
	if (UIElements[UImirrorYCheckBox].checkBox.checked  == false) {
		mirrorUsed = true;
		UIElements[UImirrorYCheckBox].checkBox.checked  = true;
		UIElements[UImirrorXCheckBox].checkBox.checked  = false;
		UIElements[UImirrorZCheckBox].checkBox.checked  = false;
		verticalMirror = true;
	}
	else {
		mirrorUsed = false;
		UIElements[UImirrorYCheckBox].checkBox.checked  = false;
	}
}
void LigidPainter::mirrorZCheckBox() {
	mirrorClick = true;
	if (UIElements[UImirrorZCheckBox].checkBox.checked  == false) {
		mirrorUsed = true;
		UIElements[UImirrorZCheckBox].checkBox.checked  = true;
		UIElements[UImirrorYCheckBox].checkBox.checked  = false;
		UIElements[UImirrorXCheckBox].checkBox.checked  = false;
		verticalMirror = false;
	}
	else {
		mirrorUsed = false;
		UIElements[UImirrorZCheckBox].checkBox.checked  = false;
	}
}
void LigidPainter::exportDownloadButtonEnter() {
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
	Utilities utilities;
	GlSet glset;
	Texture txtr;

}
bool renderDefaultModel = false;
void LigidPainter::addPlaneButton() {
	modelFilePath = "./LigidPainter/Resources/3D Models/plane.fbx";
	modelName = "plane.fbx";
	UIElements[UIUploadingModelPathTextBox].textBox.text = modelName;
	renderDefaultModel = true;
	loadModelButton();
}
void LigidPainter::addSphereButton() {
	modelFilePath = "./LigidPainter/Resources/3D Models/sphere.fbx";
	modelName = "sphere.fbx";
	UIElements[UIUploadingModelPathTextBox].textBox.text = modelName;
	renderDefaultModel = true;
	loadModelButton();
}
void LigidPainter::loadCustomModel(){
	modelName = customModelName;
	UIElements[UIUploadingModelPathTextBox].textBox.text = modelName;
	modelFilePath = customModelFilePath;
	loadModelButton();
	renderDefaultModel = false;
}
void LigidPainter::autoTriangulateCheckBox(){
	autoTriangulateChecked = !autoTriangulateChecked;
	UIElements[UIautoTriangulateCheckBox].checkBox.checked = !UIElements[UIautoTriangulateCheckBox].checkBox.checked;
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
	UIElements[UIbackfaceCullingCheckBox].checkBox.checked = !UIElements[UIbackfaceCullingCheckBox].checkBox.checked;
}
void LigidPainter::useNegativeForDrawingCheckbox(){
	UIElements[UIuseNegativeCheckBox].checkBox.checked = !UIElements[UIuseNegativeCheckBox].checkBox.checked;
}
void LigidPainter::loadModelButton() {
	renderPlane = false;
	renderSphere = false;

	//txtr.refreshScreenDrawingTexture(reduceScreenPaintingQuality);
	GlSet glset;

	if (modelName != "") {
		
		//Load the model
		model.loadModel(modelFilePath,autoTriangulateChecked);



		//Store the custom model file path (used for load custom model) 
		if(!renderDefaultModel)
			customModelFilePath = modelFilePath;


		for (size_t i = 0; i < albedoTextures.size(); i++) //Delete albedo textures
		{
			unsigned int albedoTexture = albedoTextures[i];
			glDeleteTextures(1,&albedoTextures[i]);
		}

		
		albedoTextures.clear(); //Clear the array


		for (size_t i = 0; i < model.meshes.size(); i++) //Create albedo textures
		{
			unsigned int albedoTexture;
			glset.genTextures(albedoTexture);
			albedoTextures.push_back(albedoTexture);
		}


		//Bind the first albedo texture by default
		glset.activeTexture(GL_TEXTURE0);
		glset.bindTexture(albedoTextures[0]);


		//After loading the model get back to previous state
		glUseProgram(programs.uiProgram);
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);


		//Make sure GL_ARRAY_BUFFER is capable of rendering ui
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
	colorPicker.dropperActive = true;
}
void LigidPainter::maskPanelSlider(double yOffset,int screenSizeY){
	Utilities util;
	maskPanelSliderValue += yOffset / (screenSizeY / 2);
	maskPanelSliderValue = util.restrictBetween(maskPanelSliderValue, 0.0f, -0.25f);//Keep in boundaries
}
void LigidPainter::hexValTextbox(){
	colorPicker.hexValTextBoxActive = true;
}
void LigidPainter::mainPanelBoundaries(float xOffset,int screenSizeX){
	if (enablePanelMovement) {
		Utilities util;
		mainPanelLoc -= xOffset / (screenSizeX / 2);
		cout << mainPanelLoc << ' ';
		mainPanelLoc = util.restrictBetween(mainPanelLoc, 1.98f, 1.6f);//Keep in boundaries
    }
}



//-----------------------------PREPARE STRUCTS-----------------------------\\



RenderData updateRenderData(RenderData renderData,unsigned int depthTexture,int brushSizeIndicatorSize) {
	renderData.panelLoc = mainPanelLoc;
	renderData.modelLoadFilePath = modelName;
	renderData.backfaceCulling = enableBackfaceCulling;
	renderData.exportFolder = exportFolder.c_str();
	renderData.doPainting = doPainting;
	renderData.depthTexture = depthTexture;
	renderData.paintingMode = paintingMode;
	renderData.brushSizeIndicator = brushSizeIndicatorSize;
	renderData.cameraPosChanged = cameraPosChanging;

	return renderData;
}