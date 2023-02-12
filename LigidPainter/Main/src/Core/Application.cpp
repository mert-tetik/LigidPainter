//Application.cpp
//Note : Main loop is there
//Every element is called from there


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
//GL_TEXTURE12 = Modified mask texture
//GL_TEXTURE13 = skybox
//GL_TEXTURE14 = texture rendering for the ui
//GL_TEXTURE15 = BRDF
//GL_TEXTURE16 = prefilteredMap

//GL_TEXTURE 20 - 27 Is reserved for nodes
//GL_TEXTURE28 = Empty

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

unsigned int VBO, VAO, FBOScreen;

bool firstClick = false;
bool mousePress = false;

bool cameraPosChanging = true;
bool paintingMode = false; //True if painting started, False if camera position changed after painting

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
ContextMenu addNodeContextMenu;
NodePanel nodePanel;
SndPanel sndPanel;
std::vector<NodeScene> nodeScenes;
int selectedNodeScene = 0;
TextureSelectionPanel textureSelectionPanel;
BrushMaskTextures brushMaskTextures;
std::vector<aTexture> albedoTextures;
std::vector<MaterialOut> modelMaterials;
bool newModelAdded = false;
int selectedAlbedoTextureIndex;
std::vector<Node> appNodes;
ColoringPanel coloringPanel;
TextureCreatingPanel txtrCreatingPanel;
Model model;
bool bakeTheMaterial = false;
bool anyTextureNameActive = false;
std::string textureText;
unsigned int viewportBGImage = 0;


string modelName;
string customModelName; 

//--------Functions--------\\

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void joystick_callback(int jid, int event);
void scroll_callback(GLFWwindow* window, double scroll, double scrollx);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
RenderData updateRenderData(RenderData renderData, unsigned int depthTexture, int brushSizeIndicatorSize);

//--------Functions--------\\

CallbckData callbackData;
PanelData panelData;

float maskPanelSliderValue = 0.0f;
float materialsPanelSlideValue = 0.0f;
float mainPanelLoc = 1.6f;
float brushBlurVal = 1;
double lastMouseXpos = 0;
double lastMouseYpos = 0;
double holdScrollVal;
double mouseXpos; 
double mouseYpos;
bool doScrollAfterCallInPaintingMode; //Prevent painting size change after scrolling
bool exportImage;//To let render function know if exporting called
bool doPainting = true;//Prevent painting & hide the painting cursor
bool mainPanelHover = false;
bool enablePanelMovement = true; 
bool backfaceCullingChecked = true;
bool enableBackfaceCulling;
bool mirrorUsed = false;
bool panelChanging = false; 
bool brushValChanged = true; 
bool mirrorClick = false;
bool renderTheScene = true;
bool renderSphere = false;
bool renderPlane = false;
bool verticalMirror = false;
int paintingSpacing = 1;
int textBoxActiveChar = 0;
int renderTheSceneCounter = 0;
int screenWidth;
int screenHeight;
Programs programs;
glm::vec3 drawColor;
OutShaderData outShaderData;
int chosenTextureResIndex = 2; //0:256 1:512 2:1024 3:2048 4:4096 5:8412
int chosenSkyboxTexture = 0;

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
	glfwSetJoystickCallback(joystick_callback);




	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glUseProgram(programs.uiProgram);
	glset.setVertexAtribPointer();
	glBufferData(GL_ARRAY_BUFFER, 10000, NULL, GL_DYNAMIC_DRAW); 

	

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); //Wireframe
	glEnable(GL_BLEND);
	glDepthFunc(GL_LESS);
	glEnable(GL_MULTISAMPLE);


	glHint(GL_POLYGON_SMOOTH_HINT,GL_NICEST);


	Load load;
	programs = load.getProgram();

	glActiveTexture(GL_TEXTURE0);
	glset.genTextures(viewportBGImage);
	glset.bindTexture(viewportBGImage);
	txtr.getTexture("LigidPainter/Resources/Images/BGImage.jpg",1920,1080,true);

	//Loading screen
	std::vector<float> renderVertices = { 
		// first triangle
		 1.0f,  1.0f, 0.0f,1,1,0,0,0,  // top right
		 1.0f,  0.0f, 0.0f,1,0,0,0,0,  // bottom right
		 0.0f,  1.0f, 0.0f,0,1,0,0,0,  // top left 
		// second triangle	  ,0,0,0,
		 1.0f,  0.0f, 0.0f,1,0,0,0,0,  // bottom right
		 0.0f,  0.0f, 0.0f,0,0,0,0,0,  // bottom left
		 0.0f,  1.0f, 0.0f,0,1,0,0,0   // top left
	};
	glUseProgram(programs.renderTheTextureProgram);
	glm::mat4 txtprojection = glm::ortho(0,1,0,1);
	glset.uniformMatrix4fv(programs.renderTheTextureProgram,"TextProjection",txtprojection);
	glset.uniform1i(programs.renderTheTextureProgram,"texture",0);
	glset.drawArrays(renderVertices,0);
	glfwSwapBuffers(window);

	glEnable(GL_DEPTH_TEST);

	//Load nodes
	appNodes = load.loadNodes(folderDistinguisher);
	//Load chars
	load.uploadChars();
	//Load brush mask textures
	brushMaskTextures = load.loadBrushMaskTextures();
	//Load cubemaps both blury and not blury
	Cubemaps cubemaps;
	cubemaps = load.loadCubemaps();
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
	//Load context menus
	addNodeContextMenu = ui.createContextMenus(appNodes);
	//Load the prefilter map
	load.createPrefilterMap(programs,cubemaps,windowData);
	//Load general rendering FBO
	FBOScreen = load.createScreenFrameBufferObject(windowData.windowMaxWidth,windowData.windowMaxHeight);
	//Get BRDF Look Up Texture
	unsigned int BRDFLUTxtr = load.getBrdflutTexture();
	//Create the default node scene(material)
	load.getDefaultNodeScene(nodeScenes,appNodes,"material_0");
	//Create the default material out (for model)
	MaterialOut mOut;
	mOut.program = 0;
	modelMaterials.push_back(mOut);
	//Set context
	glfwMakeContextCurrent(window);
	//Create screen painting mask Texture
	GLubyte* screenTexture = new GLubyte[(windowData.windowMaxWidth) * (windowData.windowMaxHeight)];
	ScreenPaintingReturnData screenPaintingReturnData; 
	screenPaintingReturnData = txtr.createScreenPaintTexture(screenTexture,window);
	delete[] screenTexture;
	//Load screen painting FBO
	unsigned int paintingFBO = load.getPaintingFBO(windowData,screenPaintingReturnData.normalId);
	//
	glm::mat4 perspectiveProjection = render.setMatrices();






	//Send some data to related cpp files
	//--Send programs struct
	ui.sendProgramsToUserInterface(programs);
	render.sendProgramsToRender(programs);
	txtr.sendProgramsToTextures(programs);
	//--Send max window size
	callback.sendMaxWindowSize(windowData.windowMaxWidth,windowData.windowMaxHeight);
	ui.sendMaxWindowSize(windowData.windowMaxWidth,windowData.windowMaxHeight);
	uiAct.sendMaxWindowSize(windowData.windowMaxWidth,windowData.windowMaxHeight);
	render.sendMaxWindowSize(windowData.windowMaxWidth,windowData.windowMaxHeight);
	txtr.sendMaxWindowSize(windowData.windowMaxWidth,windowData.windowMaxHeight);



	glUseProgram(programs.iconsProgram);
	glset.uniform1i(programs.iconsProgram, "icon", 6);
	glUseProgram(programs.uiProgram);
	glm::vec3 textClr = glm::vec3(colorData.textColor);
	glset.uniform3fv(programs.uiProgram,"textColor",textClr);
	glset.uniform1i(programs.uiProgram, "text", 2);
	glset.uniform1i(programs.uiProgram, "modifiedMaskTexture", 12);


	ExportData exportData;
	PBRShaderData pbrShaderData;
	SkyBoxShaderData skyBoxShaderData;
	ScreenDepthShaderData screenDepthShaderData;
	AxisPointerShaderData axisPointerShaderData;
	//
	pbrShaderData.bluryskybox = 13;
	pbrShaderData.depthTexture = 9;
	pbrShaderData.materialDiffuse = 0;
	pbrShaderData.mirroredDepthTexture = 8;
	pbrShaderData.mirroredScreenMaskTexture = 3;
	pbrShaderData.screenMaskTexture = 4;
	//
	skyBoxShaderData.skybox = 13;
	skyBoxShaderData.projection = perspectiveProjection;
	//
	axisPointerShaderData.projection = perspectiveProjection;



	renderData.window = window;
	panelData.modelPanelActive = true; //Active the model panel by default
	renderOut.mouseHoverPixel = glm::vec3(0);


	bool paintRender = false;
	int paintRenderCounter = 0;
	ViewUpdateData viewUpdateData;
	double mouseDrawingPosX = 0;
	double mouseDrawingPosY = 0;
	bool doChangeStateOfTheAddNodeContextBar = true;
	float brushSize;

	screenWidth = windowData.windowMaxWidth;
	screenHeight = windowData.windowMaxHeight;

	MainLoop mainLoop;
	while (!glfwWindowShouldClose(window))//Main loop
	{
		glfwPollEvents();
		if(UIElements[UIstabilizeFpsCheckBox].checkBox.checked)
			glfwSwapInterval(1);
		else
			glfwSwapInterval(0);
		
		util.printRenderingSpeed();
		
		util.printError();

		
		mainLoop.updateCameraPosChanging(callbackData.cameraPos,cameraPosChanging);
		mainLoop.detectClick(window,mousePress,firstClick);
		mainLoop.updateRenderTheScene(window,renderTheSceneCounter,renderTheScene);
		mainLoop.setContextPanelsStates(window,coloringPanel,addNodeContextMenu,textureSelectionPanel);
		ui.sendTextBoxActiveCharToUI(textBoxActiveChar);






		if(addNodeContextMenu.active && !addNodeContextMenu.hover){
			addNodeContextMenu.active = false;
		}

		if(glfwGetMouseButton(window,1) == GLFW_PRESS && doChangeStateOfTheAddNodeContextBar){
			doChangeStateOfTheAddNodeContextBar = false;
			addNodeContextMenu.stateChanged = true; 
			if(nodePanel.panelHover){
				addNodeContextMenu.active = true;  
			} 
			else{
				addNodeContextMenu.active = false;
			}
		}
		if(glfwGetMouseButton(window,1) == GLFW_RELEASE){
			doChangeStateOfTheAddNodeContextBar = true;
		}



		mainLoop.releaseTextBoxes(window,UIElements,exportFileName,textBoxActiveChar,coloringPanel,txtrCreatingPanel,colorPicker);
		mainLoop.changeTextureDisplayersState(window,textureDisplayer);

		if( (firstClick || glfwGetMouseButton(window, 1) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS) && anyTextureNameActive){
			anyTextureNameActive = false;
			for (size_t i = 0; i < albedoTextures.size(); i++)
			{
				if(albedoTextures[i].nameTextActive && textureText != "" && textureText != albedoTextures[i].name){
					std::vector<std::string> textureNames;
			        for (size_t nameI = 0; nameI < albedoTextures.size(); nameI++)
			        {
				        textureNames.push_back(albedoTextures[nameI].name);
			        }
                    textureText = util.uniqueName(textureText,textureNames);
					albedoTextures[i].name = textureText;

				}
			}
		}


		if(!coloringPanel.active){
			coloringPanel.panelHover = false;
		}
		if(!txtrCreatingPanel.active){
			txtrCreatingPanel.panelHover = false;
		}




		mainLoop.changeColorPickersValue(window,colorPicker,coloringPanel,renderOut.mouseHoverPixel,firstClick);

		//Ui actions
		uiAct.uiActions(window,callbackData,UIElements,colorPicker,textureDisplayer,nodePanel,sndPanel,firstClick);


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

		


		//Render
		//double firstTime = glfwGetTime();
		if(renderTheScene){
			renderOut = render.render(renderData, FBOScreen, panelData,exportData,icons,maskPanelSliderValue,brushMaskTextures.textures,renderPlane,renderSphere,pbrShaderData,skyBoxShaderData,brushBlurVal,screenDepthShaderData,axisPointerShaderData,outShaderData,model,albedoTextures,paintRender,materialsPanelSlideValue,UIElements,colorPicker,textureDisplayer,cubemaps,addNodeContextMenu,nodePanel,sndPanel,selectedAlbedoTextureIndex,textureSelectionPanel,nodeScenes,selectedNodeScene,appNodes,perspectiveProjection,viewUpdateData.view, modelMaterials,newModelAdded,firstClick,viewUpdateData.cameraPos,coloringPanel,txtrCreatingPanel,chosenTextureResIndex,chosenSkyboxTexture,bakeTheMaterial,anyTextureNameActive,textureText,viewportBGImage);
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
		const bool distance_spacingCompatibility = glm::distance(glm::vec2(mouseDrawingPosX,mouseDrawingPosY),glm::vec2(mouseXpos,mouseYpos)) > paintingSpacing;
		const bool paintingCondition = glfwGetMouseButton(window, 0) == GLFW_PRESS && doPainting && !panelChanging && glfwGetMouseButton(window, 1) == GLFW_RELEASE && !colorPicker.dropperActive && distance_spacingCompatibility && !coloringPanel.saturationValueBoxPointerPressed && !coloringPanel.hueBarPointerPressed; 
		if (paintingCondition){

			mouseDrawingPosX = mouseXpos;
			mouseDrawingPosY = mouseYpos;

			//Paint
			textureGen.drawToScreen(window, screenPaintingReturnData.normalId, brushSize, FBOScreen,UIElements[UIbrushRotationRangeBar].rangeBar.value,UIElements[UIbrushOpacityRangeBar].rangeBar.value,lastMouseXpos, lastMouseYpos,mouseXpos,mouseYpos,mirrorUsed,UIElements[UIuseNegativeCheckBox].checkBox.checked,brushValChanged,programs,windowData.windowMaxWidth,windowData.windowMaxHeight,UIElements[UIbrushBordersRangeBar].rangeBar.value,brushBlurVal,paintingFBO,outShaderData,model,modelMaterials, paintingSpacing < 10,viewUpdateData.view);
			paintRenderCounter++;
			if(paintRenderCounter == 50000){
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
		exportImage = false;
		textureDisplayer.buttonClicked = false;



			callbackData = callback.mouse_callback(window, mouseXpos, mouseYpos, panelData,maskPanelSliderValue,renderOut.maskPanelMaskHover,cursors,renderOut.texturePanelButtonHover,UIElements,mainPanelLoc,colorPicker,textureDisplayer,nodePanel,addNodeContextMenu,sndPanel,coloringPanel);


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

		const int renderingThreshold = 120;
		if(renderTheSceneCounter == renderingThreshold)
			renderTheScene = false;

		addNodeContextMenu.stateChanged = false;
		
		nodePanel.zoomValChanged = false;

		//Exit message box
		if(glfwWindowShouldClose(window)){
			bool noButtonClick = true;
			bool clickTaken = false;

			panelData.exportPanelActive = false;
			panelData.paintingPanelActive = false;
			panelData.texturePanelActive = false;
			panelData.modelPanelActive = false;
			panelData.settingsPanelActive = false;
			panelData.generatorPanelActive = false;

		 	while (true)
		 	{
				//Disable painting
				doPainting = false; 
				renderData.doPainting = doPainting;

		 		glfwPollEvents();

				//Keep rendering the backside
		 		renderOut = render.render(renderData, FBOScreen, panelData,exportData,icons,maskPanelSliderValue,brushMaskTextures.textures,renderPlane,renderSphere,pbrShaderData,skyBoxShaderData,brushBlurVal,screenDepthShaderData,axisPointerShaderData,outShaderData,model,albedoTextures,paintRender,materialsPanelSlideValue,UIElements,colorPicker,textureDisplayer,cubemaps,addNodeContextMenu,nodePanel,sndPanel,selectedAlbedoTextureIndex,textureSelectionPanel,nodeScenes,selectedNodeScene,appNodes,perspectiveProjection,viewUpdateData.view,modelMaterials,newModelAdded,firstClick,viewUpdateData.cameraPos,coloringPanel,txtrCreatingPanel,chosenTextureResIndex,chosenSkyboxTexture,bakeTheMaterial,anyTextureNameActive,textureText,viewportBGImage);
		 		
				
				float messageBoxBackColor[3] = {colorData.messageBoxPanelColor.r,colorData.messageBoxPanelColor.g,colorData.messageBoxPanelColor.r};

				float messageBoxButtonColor[3] = {colorData.messageBoxButtonColor.r,colorData.messageBoxButtonColor.g,colorData.messageBoxButtonColor.r};

				//render the message box
				int result = lgdMessageBox(window,mouseXpos,mouseYpos,cursors.defaultCursor,cursors.pointerCursor,icons.Logo,programs.uiProgram,"LigidPainter will be closed. Do you want to proceed?",-0.21f,0.0f,messageBoxBackColor,messageBoxButtonColor,(float)windowData.windowMaxWidth, (float)screenWidth,programs.iconsProgram,icons,programs); //0 = Yes //1 = No //2 = None

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
	}
	
	//Close the program
	glfwDestroyWindow(window);
	glfwTerminate();
	return true;
}







//-------------CALLBACK-------------\\

bool caps = false; //GLFW_MOD_CAPS_LOCK
void joystick_callback(int jid, int event)
{
	std::cout << "hey";
    if (event == GLFW_CONNECTED)
    {
        // The joystick was connected
    }
    else if (event == GLFW_DISCONNECTED)
    {
        // The joystick was disconnected
    }
}
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
		if(ui.textInput(key,action,caps,exportFileName,exportFileNameThreshold,window,textBoxActiveChar)){
			UIElements[UIexportingFolderNameTextBox].textBox.text = exportFileName;
		}
		if(action == 1 || action == 2){
			if(glfwGetKey(window,GLFW_KEY_LEFT) == GLFW_PRESS && UIElements[UIexportingFolderNameTextBox].textBox.text.size()+textBoxActiveChar){
				textBoxActiveChar--;
			}
			if(glfwGetKey(window,GLFW_KEY_RIGHT) == GLFW_PRESS && textBoxActiveChar != 0){
				textBoxActiveChar++;
			}
		}
	}
	
	if(colorPicker.hexValTextBoxActive){
		if(ui.textInputHex(key,action,colorPicker.hexValTextBoxVal,textBoxActiveChar)){
			colorPicker.saturationValueValChanged = true;
			colorPicker.hexValTextBoxGotInput = true;
		}
		if(action == 1 || action == 2){
			if(glfwGetKey(window,GLFW_KEY_LEFT) == GLFW_PRESS && colorPicker.hexValTextBoxVal.size()+textBoxActiveChar){
				textBoxActiveChar--;
			}
			if(glfwGetKey(window,GLFW_KEY_RIGHT) == GLFW_PRESS && textBoxActiveChar != 0){
				textBoxActiveChar++;
			}
		}
	}

	if(coloringPanel.hexValTextboxActive){
		if(ui.textInputHex(key,action,coloringPanel.hexVal,textBoxActiveChar)){
			coloringPanel.newHexValTextboxEntry = true;
		}	
		if(action == 1 || action == 2){
			if(glfwGetKey(window,GLFW_KEY_LEFT) == GLFW_PRESS && coloringPanel.hexVal.size()+textBoxActiveChar){
				textBoxActiveChar--;
			}
			if(glfwGetKey(window,GLFW_KEY_RIGHT) == GLFW_PRESS && textBoxActiveChar != 0){
				textBoxActiveChar++;
			}
		}
	}

	const int maxTextureCharCount = 22;
	if(txtrCreatingPanel.textBoxActive){
		if(ui.textInput(key,action,caps,txtrCreatingPanel.textBoxVal,maxTextureCharCount,window,textBoxActiveChar)){
			txtrCreatingPanel.newTextboxEntry = true;
		}	
		if(action == 1 || action == 2){
			if(glfwGetKey(window,GLFW_KEY_LEFT) == GLFW_PRESS && txtrCreatingPanel.textBoxVal.size()+textBoxActiveChar){
				textBoxActiveChar--;
			}
			if(glfwGetKey(window,GLFW_KEY_RIGHT) == GLFW_PRESS && textBoxActiveChar != 0){
				textBoxActiveChar++;
			}
		}
	}

	if(anyTextureNameActive){
		ui.textInput(key,action,caps,textureText,exportFileNameThreshold,window,textBoxActiveChar);
		
		if(action == 1 || action == 2){
			if(glfwGetKey(window,GLFW_KEY_LEFT) == GLFW_PRESS && textureText.size()+textBoxActiveChar){
				textBoxActiveChar--;
			}
			if(glfwGetKey(window,GLFW_KEY_RIGHT) == GLFW_PRESS && textBoxActiveChar != 0){
				textBoxActiveChar++;
			}
		}
	}

	//------------------SHORTCUTS------------------

	Utilities util;

	bool shiftTabAltRelease = glfwGetKey(window, GLFW_KEY_TAB) == GLFW_RELEASE && glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE && glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_RELEASE;

	//Ctrl + x use negative checkbox
	if(util.shortCut(window,GLFW_KEY_LEFT_CONTROL,GLFW_KEY_X,0,0) && panelData.paintingPanelActive && action == 1 && shiftTabAltRelease){
		UIElements[UIuseNegativeCheckBox].checkBox.checked = !UIElements[UIuseNegativeCheckBox].checkBox.checked;
	}

	//Ctrl + h change texture demonstrator's state
	if(util.shortCut(window,GLFW_KEY_LEFT_CONTROL,GLFW_KEY_H,0,0) && action == 1 && shiftTabAltRelease){
		textureDisplayer.buttonClicked = true;
	}

	LigidPainter lp;
	//Ctrl + Tab + q switch to model panel


	if(util.shortCut(window,GLFW_KEY_LEFT_CONTROL,GLFW_KEY_TAB,GLFW_KEY_Q,0) && action == 1){
		lp.modelPanelButton();
	}
	//Ctrl + Tab + w switch to texture panel
	if(util.shortCut(window,GLFW_KEY_LEFT_CONTROL,GLFW_KEY_TAB,GLFW_KEY_W,0) && action == 1){
		lp.texturePanelButton();
	}
	//Ctrl + Tab + t switch to model panel
	if(util.shortCut(window,GLFW_KEY_LEFT_CONTROL,GLFW_KEY_TAB,GLFW_KEY_T,0) && action == 1){
		lp.paintingPanelButton();
	}
	//Ctrl + Tab + r switch to model panel
	if(util.shortCut(window,GLFW_KEY_LEFT_CONTROL,GLFW_KEY_TAB,GLFW_KEY_R,0) && action == 1){
		lp.exportPanelButton();
	}
}

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


	
	bool hideCursor = uiAct.updateRangeValues(window,xOffset,yOffset,width,height,textureDisplayer,sndPanel); 



	if(colorPicker.dropperActive){
		doPainting = false;
	}
	else if(textureDisplayer.buttonHover || callbackData.paintingPanelButtonEnter || callbackData.modelPanelButtonEnter || callbackData.exportPanelButtonEnter || callbackData.texturePanelButtonEnter || UIElements[UImirrorZCheckBox].checkBox.mouseHover || UIElements[UImirrorZCheckBox].checkBox.mouseHover || UIElements[UImirrorZCheckBox].checkBox.mouseHover){
		doPainting = false;
	}
	else if(nodePanel.panelHover || nodePanel.boundariesHover || nodePanel.boundariesPressed || sndPanel.panelHover || coloringPanel.panelHover || txtrCreatingPanel.panelHover){
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
		else if(sndPanel.panelHover){
			if(sndPanel.state == 0){
				sndPanel.texturePanelSlideVal += (float)(scrollx / 25.0);
				if(sndPanel.texturePanelSlideVal < 0.f)
					sndPanel.texturePanelSlideVal = 0.001f;
			}
			else if(sndPanel.state == 1){
				sndPanel.materialPanelSlideVal += (float)(scrollx / 25.0);
				if(sndPanel.materialPanelSlideVal < 0.f)
					sndPanel.materialPanelSlideVal = 0.001f;
			}
		}
		else if(nodePanel.panelHover){
			//Node panel scroll
			nodePanel.zoomVal += (float)(scrollx / 40.0);
			nodePanel.zoomValChanged = true;
			nodePanel.zoomVal = util.restrictBetween(nodePanel.zoomVal, 2.0f, 0.01f);//Keep in boundaries
		}
		else if(callbackData.mainPanelEnter && panelData.texturePanelActive && model.meshes.size() >= 4){
			//Materials
			materialsPanelSlideValue += scrollx/10.0f;
			if(materialsPanelSlideValue < 0.0f)
				materialsPanelSlideValue = 0.0f;
		}
		else if (!paintingMode && !mainPanelHover) {
			callbackData = callback.scroll_callback(window, scroll, scrollx);
		}
		else {
			holdScrollVal = scrollx;
			doScrollAfterCallInPaintingMode = true;
		}
		cameraPosChanging = true;
	}
}




//---------OTHER---------\\

void LigidPainter::updateColorPicker(glm::vec3 RGBval,bool changeHue,bool changeSatVal,float &hueValue,float &saturationValuePosX, float &saturationValuePosY,bool isMainColorPicker){
	int width;
	int height;
	glfwGetWindowSize(window,&width,&height);
	
	Utilities util;
	
	glm::vec3 hsvVal = util.RGBToHSVGenerator(RGBval);
	
	if(isMainColorPicker){
		drawColor = RGBval/glm::vec3(255.0f);
		colorPicker.saturationValueValChanged = true;
	}
	
	if(changeHue){
		hueValue = (hsvVal.r / 708.333333333f) - 0.18f; //0.195
		if(isMainColorPicker)
			colorPicker.updateHueVal = true;
	}
	if(changeSatVal){
		saturationValuePosX = ((hsvVal.g / 1342.10526316f) - 0.095f); //0.095
		saturationValuePosY = (hsvVal.b / 653.846153846f) - 0.195f; //0.195
	}
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
void LigidPainter::skyboxExposureRangeBar(double xOffset, int width, int height) {
	Utilities util;
	Texture txtr;
	//brushOpacityChanged = true; not used
	UIElements[UIskyBoxExposureRangeBar].rangeBar.value -= xOffset / (width / 2.0f);
	UIElements[UIskyBoxExposureRangeBar].rangeBar.value = util.restrictBetween(UIElements[UIskyBoxExposureRangeBar].rangeBar.value, 0.11f, -0.11f);//Keep in boundaries
}
void LigidPainter::skyBoxRotationRangeBar(double xOffset, int width, int height) {
	Utilities util;
	Texture txtr;
	//brushOpacityChanged = true; not used
	UIElements[UIskyBoxRotationRangeBar].rangeBar.value -= xOffset / (width / 2.0f);
	UIElements[UIskyBoxRotationRangeBar].rangeBar.value = util.restrictBetween(UIElements[UIskyBoxRotationRangeBar].rangeBar.value, 0.11f, -0.11f);//Keep in boundaries
}
void LigidPainter::normalStrengthRangeBar(double xOffset, int width, int height) {
	Utilities util;
	Texture txtr;
	//brushOpacityChanged = true; not used
	UIElements[UInormalStrengthRangeBarElement].rangeBar.value -= xOffset / (width / 2.0f);
	UIElements[UInormalStrengthRangeBarElement].rangeBar.value = util.restrictBetween(UIElements[UInormalStrengthRangeBarElement].rangeBar.value, 0.11f, -0.11f);//Keep in boundaries
}
void LigidPainter::noiseStrengthRangeBar(double xOffset, int width, int height) {
	Utilities util;
	Texture txtr;
	//brushOpacityChanged = true; not used
	UIElements[UInoiseStrengthRangeBarElement].rangeBar.value -= xOffset / (width / 2.0f);
	UIElements[UInoiseStrengthRangeBarElement].rangeBar.value = util.restrictBetween(UIElements[UInoiseStrengthRangeBarElement].rangeBar.value, 0.11f, -0.11f);//Keep in boundaries
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
void LigidPainter::generateTextureButton(){
	Utilities util;

	int txtrRes = 256;
	for (size_t i = 0; i < chosenTextureResIndex; i++)
	{
		txtrRes*=2;
	}

	glActiveTexture(GL_TEXTURE28);
		
		//FBO
		unsigned int FBO;
		glset.genFramebuffers(FBO);
		glset.bindFramebuffer(FBO);

		//Texture
		unsigned int textureColorbuffer;
		glset.genTextures(textureColorbuffer);
		glset.bindTexture(textureColorbuffer);
		glset.texImage(NULL, txtrRes,txtrRes,GL_RGB); //TODO : Use texture quality variable
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glset.generateMipmap();

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorbuffer, 0);

		glViewport(0,0,txtrRes,txtrRes);

		std::vector<float> renderVertices = { 
			// first triangle
			 1.0f,  1.0f, 0.0f,1,1,0,0,0,  // top right
			 1.0f,  0.0f, 0.0f,1,0,0,0,0,  // bottom right
			 0.0f,  1.0f, 0.0f,0,1,0,0,0,  // top left 
			// second triangle	  ,0,0,0,
			 1.0f,  0.0f, 0.0f,1,0,0,0,0,  // bottom right
			 0.0f,  0.0f, 0.0f,0,0,0,0,0,  // bottom left
			 0.0f,  1.0f, 0.0f,0,1,0,0,0   // top left
		};
		if(UIElements[UInoiseCheckBoxElement].checkBox.checked)
			glUseProgram(programs.noisyTextureProgram);
		if(UIElements[UInormalmapCheckBoxElement].checkBox.checked)
			glUseProgram(programs.normalGenProgram);
		
		glm::mat4 renderTextureProjection = glm::ortho(0.0f, 1.0f, 0.0f, 1.0f);
		
		glset.uniformMatrix4fv(programs.noisyTextureProgram,"renderTextureProjection",renderTextureProjection);
		glset.uniformMatrix4fv(programs.normalGenProgram,"renderTextureProjection",renderTextureProjection);

		glset.drawArrays(renderVertices,0);
		glUseProgram(programs.uiProgram);

		glDeleteFramebuffers(1,&FBO);	
		glset.bindFramebuffer(0);

		aTexture txtr;
		txtr.id = textureColorbuffer;
		txtr.name = "noisyMap"; 
		std::vector<std::string> textureNames;
		for (size_t i = 0; i < albedoTextures.size(); i++)
		{
			textureNames.push_back(albedoTextures[i].name);
		}
			
		//Rename if necessary
		txtr.name = util.uniqueName(txtr.name,textureNames);
		albedoTextures.push_back(txtr);

		glViewport(-(windowData.windowMaxWidth - screenWidth)/2, -(windowData.windowMaxHeight - screenHeight), windowData.windowMaxWidth, windowData.windowMaxHeight);
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
	if (!UIElements[UIjpgCheckBox].checkBox.checked) {
		UIElements[UIjpgCheckBox].checkBox.checked = true;
		UIElements[UIpngCheckBox].checkBox.checked = false;
	}
}
void LigidPainter::exportExtPNGCheckBox() {
	if (!UIElements[UIpngCheckBox].checkBox.checked) {
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
	nodeScenes[selectedNodeScene].stateChanged = true;

	panelData.modelPanelActive = true;
	panelData.texturePanelActive = false;
	panelData.paintingPanelActive = false;
	panelData.exportPanelActive = false;
	panelData.settingsPanelActive = false;
	panelData.generatorPanelActive = false;
	doPainting = false;
}
void LigidPainter::texturePanelButton() {
	nodeScenes[selectedNodeScene].stateChanged = true;

	panelData.modelPanelActive = false;
	panelData.texturePanelActive = true;
	panelData.paintingPanelActive = false;
	panelData.exportPanelActive = false;
	panelData.settingsPanelActive = false;
	panelData.generatorPanelActive = false;
	doPainting = false;
}
void LigidPainter::paintingPanelButton() {
	UserInterface ui;
	panelData.modelPanelActive = false;
	panelData.texturePanelActive = false;
	panelData.paintingPanelActive = true;
	panelData.exportPanelActive = false;
	panelData.settingsPanelActive = false;
	panelData.generatorPanelActive= false;
	doPainting = true;
	
	if(!albedoTextures.size())
		ui.alert("Warning! There Are No Textures Selected.",100);
}
void LigidPainter::exportPanelButton() {
	nodeScenes[selectedNodeScene].stateChanged = true;

	panelData.modelPanelActive = false;
	panelData.texturePanelActive = false;
	panelData.paintingPanelActive = false;
	panelData.exportPanelActive = true;
	panelData.settingsPanelActive = false;
	panelData.generatorPanelActive = false;
	doPainting = false;
}

void LigidPainter::settingsPanelButton(){
	nodeScenes[selectedNodeScene].stateChanged = true;

	panelData.modelPanelActive = false;
	panelData.texturePanelActive = false;
	panelData.paintingPanelActive = false;
	panelData.exportPanelActive = false;
	panelData.settingsPanelActive = true;
	panelData.generatorPanelActive = false;
	doPainting = false;
}
void LigidPainter::generatorPanelButton(){
	nodeScenes[selectedNodeScene].stateChanged = true;

	panelData.modelPanelActive = false;
	panelData.texturePanelActive = false;
	panelData.paintingPanelActive = false;
	panelData.exportPanelActive = false;
	panelData.settingsPanelActive = false;
	panelData.generatorPanelActive = true;
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
	renderDefaultModel = false;
	loadModelButton();
}
void LigidPainter::autoTriangulateCheckBox(){
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
		//Store the custom model file path (used for load custom model) 
		if(!renderDefaultModel)
			customModelFilePath = modelFilePath;
		

		//Load the model
		if(renderDefaultModel)
			model.loadModel(modelFilePath,UIElements[UIautoTriangulateCheckBox].checkBox.checked);
		else if(customModelFilePath != "")
			model.loadModel(customModelFilePath,UIElements[UIautoTriangulateCheckBox].checkBox.checked);


		newModelAdded = true;



		// for (size_t i = 0; i < model.meshes.size(); i++) 
		// {
		// }

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
		mainPanelLoc = util.restrictBetween(mainPanelLoc, 1.99f, 1.6f);//Keep in boundaries
    }
}
void LigidPainter::sndPanelBoundaries(float xOffset,int screenSizeX){
	if (enablePanelMovement) {
		Utilities util;
		sndPanel.position -= xOffset / (screenSizeX / 2);
		sndPanel.position = util.restrictBetween(sndPanel.position, -0.6f, -0.99f);//Keep in boundaries
    }
}
void LigidPainter::nodePanelBoundaries(float yOffset,float screenHeight){
	if (enablePanelMovement) {
		Utilities util;
		nodePanel.heigth += yOffset / (screenHeight / 2);
		nodePanel.heigth = util.restrictBetween(nodePanel.heigth, 1.92f, 0.01);//Keep in boundaries
    }
}

void LigidPainter::sndPanelMinusIcon(){
	UserInterface ui;

	if(!txtrCreatingPanel.active){
		if(sndPanel.state == 0){
			//Textures
			if(albedoTextures.size()){
				unsigned int texture;
				texture = albedoTextures[selectedAlbedoTextureIndex].id;
				glDeleteTextures(1, &texture);

				if(albedoTextures[selectedAlbedoTextureIndex].isTexture)
					albedoTextures.erase(albedoTextures.begin() + selectedAlbedoTextureIndex);
				else{
					for (size_t i = 0; i < albedoTextures.size(); i++)
					{
						//Delete the elements of the folder
						if(albedoTextures[i].folderIndex == selectedAlbedoTextureIndex){
							albedoTextures.erase(albedoTextures.begin() + i);
						}
					}
					albedoTextures.erase(albedoTextures.begin() + selectedAlbedoTextureIndex);
				}
				
				if(selectedAlbedoTextureIndex)
					selectedAlbedoTextureIndex--;
			}
			else{
				ui.alert("Warning! Deleting request is ignored. There are no texture to delete.",200);
			}
		}
		else if(sndPanel.state == 1){
			//Materials
			bool deletable = true;
			if(nodeScenes.size() != 1){
				for (size_t i = 0; i < model.meshes.size(); i++)
				{
					if(model.meshes[i].materialIndex == selectedNodeScene){
						ui.alert("Warning! Deleting request is ignored. This material is already in use.",200);
						deletable = false;
						break;
					}
				}
				if(deletable){
					nodeScenes.erase(nodeScenes.begin() + selectedNodeScene);
					modelMaterials.erase(modelMaterials.begin() + selectedNodeScene);
					if(selectedNodeScene)
						selectedNodeScene--;
					for (size_t i = 0; i < model.meshes.size(); i++)
					{
						if(selectedNodeScene < model.meshes[i].materialIndex){
							model.meshes[i].materialIndex--;
						}
					}
					
				}
			}
			else{
				ui.alert("Warning! Deleting request is ignored. Last material can't be deleted.",200);
			}
		}
	}

}
void LigidPainter::sndPanelPlusIcon(){
	if(!txtrCreatingPanel.active){
		if(sndPanel.state == 0){
			//Textures


			txtrCreatingPanel.active = true;

		}
		else if(sndPanel.state == 1){
			//Materials

			ColorData colorData;

			std::vector<Node> mainOutNodes;
			Load load;
			mainOutNodes = load.createOutputNode(appNodes);		

			NodeScene emptyNodeScene;
			emptyNodeScene.index = 0;

			const int maxMaterialSize = 100;

			for (int i = 0; i < maxMaterialSize; i++)
			{
				bool numberAvailable = false;
				for (int nodeSceneIndex = 0; nodeSceneIndex < nodeScenes.size(); nodeSceneIndex++)
				{
					if(nodeScenes[nodeSceneIndex].index == i){
						numberAvailable = true;
					}
				}
				if(!numberAvailable){
					emptyNodeScene.index = i;
					break;
				}
			}

			emptyNodeScene.sceneName = "material_" + std::to_string(emptyNodeScene.index); 
			emptyNodeScene.nodes = mainOutNodes;
			
			nodeScenes.push_back(emptyNodeScene);

			MaterialOut mOut;
			mOut.program = 0;
			modelMaterials.push_back(mOut);
		}
	}
}
void LigidPainter::sndPanelFolderIcon(){
	Utilities util;
	
	aTexture result;
	result.isTexture = false;
	result.name = "folder";
	
	std::vector<std::string> textureNames;
	for (size_t i = 0; i < albedoTextures.size(); i++)
	{
		textureNames.push_back(albedoTextures[i].name);
	}
	
	result.name = util.uniqueName(result.name,textureNames);

	albedoTextures.push_back(result);
}
void LigidPainter::sndPanelBackIcon(){
	if(sndPanel.activeFolderIndex != 10000)
		sndPanel.activeFolderIndex = albedoTextures[sndPanel.activeFolderIndex].folderIndex;
}
void LigidPainter::viewportImageTextbox(){
		glActiveTexture(GL_TEXTURE0);
		unsigned int texture;
		glset.genTextures(texture);
		glset.bindTexture(texture);

		//Load texture
		Texture txtr;
		//Filters
		char const* lFilterPatterns[2] = { "*.jpg", "*.png" };
		//File dialog
		auto albedoPathCheck = tinyfd_openFileDialog("Select Image", "", 2, lFilterPatterns, "", false);
		
		if (albedoPathCheck) {
			std::string albedoTexturePath = albedoPathCheck;
			
			int txtrRes = 256;
			for (size_t i = 0; i < chosenTextureResIndex; i++)
			{
				txtrRes*=2;
			}

			txtr.getTexture(albedoTexturePath,txtrRes,txtrRes,true); //Force albedo's ratio to be 1:1
			if(viewportBGImage)
				glDeleteTextures(1,&viewportBGImage);

			viewportBGImage = texture;
		}
}
void LigidPainter::sndPanelDownIcon(){
	if(!txtrCreatingPanel.active){
		aTexture result;

		glActiveTexture(GL_TEXTURE0);
		unsigned int texture;
		glset.genTextures(texture);
		glset.bindTexture(texture);

		//Load texture
		Texture txtr;
		//Filters
		char const* lFilterPatterns[2] = { "*.jpg", "*.png" };
		//File dialog
		auto albedoPathCheck = tinyfd_openFileDialog("Select Image", "", 2, lFilterPatterns, "", false);
		
		if (albedoPathCheck) {
			std::string albedoTexturePath = albedoPathCheck;
			
			int txtrRes = 256;
			for (size_t i = 0; i < chosenTextureResIndex; i++)
			{
				txtrRes*=2;
			}

			txtr.getTexture(albedoTexturePath,txtrRes,txtrRes,true); //Force albedo's ratio to be 1:1
			Utilities util;
			result.id = texture;
			
			std::vector<std::string> textureNames;
			for (size_t i = 0; i < albedoTextures.size(); i++)
			{
				textureNames.push_back(albedoTextures[i].name);
			}
			
			//Rename if necessary
			result.name = util.removeExtension(util.getLastWordBySeparatingWithChar(albedoTexturePath,folderDistinguisher)); 
			result.name = util.uniqueName(result.name,textureNames);
			
			albedoTextures.push_back(result);
		}
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