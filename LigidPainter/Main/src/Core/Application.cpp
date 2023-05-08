//? GL_TEXTURE0 = Albedo texture
//? GL_TEXTURE1 = Mask texture
//? GL_TEXTURE2 = Chars
//? GL_TEXTURE3 = Mirrored Screen Mask Painting Texture
//? GL_TEXTURE4 = Screen Mask Painting Texture
//? GL_TEXTURE5 = 1920x1080 Screen Texture
//? GL_TEXTURE6 = Icons
//? GL_TEXTURE7 = UV mask texture (used for painting over boundaries)
//? GL_TEXTURE8 = 3D Rendered mask texture
//? GL_TEXTURE9 = Depth texture
//? GL_TEXTURE10 = 1080x1080 Screen Texture
//? GL_TEXTURE12 = Modified mask texture
//? GL_TEXTURE13 = skybox
//? GL_TEXTURE14 = texture rendering for the ui
//? GL_TEXTURE15 = BRDF
//? GL_TEXTURE16 = prefilteredMap
//? GL_TEXTURE17 = subSelectedTexture
   
//? GL_TEXTURE 20 - 27 Is reserved for nodes
//? GL_TEXTURE28 = Empty

//TODO Fix mirror origin pos
//TODO Fix mirror paint over
//TODO Flip
//TODO Preview for skybox listbox
//TODO Remove seams
//TODO Texture history HDD
//TODO Node context menu
//TODO Project manager context menu
//TODO Project manager drag & drop
//TODO Utilities::transitionEffect glfwtime
//TODO Text new line 
//TODO History settings
//TODO Fix sndpanel texture folder slide bar bla bla
//TODO Sound for success alert
//TODO Update projectsettings 
//TODO Multiple texture selection
//TODO Brush textures interactions 
//TODO Fix project folder save options
//TODO start screen project displayer - restrict size
//TODO Default pbr node light rot
//TODO Fix add node context menu few elements
//TODO Spinner screengap
//TODO Fix UI Shaders
//TODO Fix folder icons
//TODO Drag to coloring panel color picker
//TODO Node & textbox copy paste
//TODO Resize certain textures

//TODO Borders

//TODO For tomorrow
//Fix the painting
//Apply 3D model settings to the painting
//Make the project manager functional
//Fix textboxes
//Fix the design of the new project panel

//TODO FixCircular range bar for light pos

//TODO Search for brush textures
//TODO Color id
//TODO Color pallette"-
//TODO Light source rotation from 3D Model displayer
//TODO RTX Support

//TODO 2D Painting
// - Lasso Tool 
// - Quick Selection
// - Brush
// - Eraser
// - Text tool

#include<iostream>

#include "../../thirdparty/include/glad/glad.h"
#include "../../thirdparty/include/GLFW/glfw3.h"

#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <thread>
#include <chrono>

#include "../../thirdparty/include/glm/glm.hpp"
#include "../../thirdparty/include/glm/gtc/matrix_transform.hpp"
#include "../../thirdparty/include/glm/gtc/type_ptr.hpp"
#include "../../thirdparty/include/glm/gtx/string_cast.hpp"



#include "LigidPainter.h"
#include "UI/UserInterface.h"
#include "Utilities.h"
#include "Callback.h"
#include "gl.h"
#include "Load.hpp"
#include "Render/Render.h"
#include "Model/mesh.h"
#include "Model/model.h"
#include "Texture/Texture.h"
#include "UI/UiActions.h"
#include "Texture Generator/TextureGenerator.h"
#include "messageBox.h"
#include "ProjectFile/WRLigidFile.hpp"
#include "ProjectFile/WRLigidMaterialFile.hpp"
#include "Prep.hpp"

#define LIBAL_OPENAL_IMPLEMENTATION
#include "LibAL.h"

#define STB_IMAGE_IMPLEMENTATION
#include "../../thirdparty/stb_image.h"

#define STBI_MSC_SECURE_CRT
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../../thirdparty/stb_image_write.h"

#include "../../thirdparty/tinyfiledialogs.h"

using namespace std;

GlSet glset;
WindowData windowData;
GLFWwindow* window;


#if defined(_WIN32) || defined(_WIN64)
    char folderDistinguisher = '\\';
#else
	char folderDistinguisher = '/'; 
#endif

unsigned int FBOScreen;

bool firstClick = false;
bool firstClickR = false;
bool mousePress = false;
bool doubleClick = false;
bool doubleClickR = false;

bool cameraPosChanging = true;
bool paintingMode = false; //True if painting started, False if camera position changed after painting

//Paths
char* modelFilePath;
char* customModelFilePath;
string albedoTexturePath = "albedoImage.png";
string maskTexturePath = "./LigidPainter/Resources/Textures/Mask/mask16.jpg";
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
std::vector<NodeScene> nodeScenesHistory;
int selectedNodeScene = 0;
TextureSelectionPanel textureSelectionPanel;
BrushTexture brushMaskTextures;
std::vector<aTexture> albedoTextures;
std::vector<MaterialOut> modelMaterials;
bool newModelAdded = false;
int selectedAlbedoTextureIndex = 1;
std::vector<Node> appNodes;
ColoringPanel coloringPanel;
TextureCreatingPanel txtrCreatingPanel;
Model model;
bool bakeTheMaterial = false;
bool anyTextureNameActive = false;
std::string textureText;
unsigned int viewportBGImage = 0;
Objects objects;
std::vector<MirrorParam> mirrorParams;
unsigned int depthTextureID;
bool startScreen = true;
bool createProject = false;
bool debugMode = false;
std::string projectFilePath;
std::vector<Font> fonts;
ProjectManager projectManager;
unsigned int generatedTextTxtr = 0;
Font txtrGenSelectedFont;

string modelName;
string customModelName; 

//--------Functions--------\\

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void drop_callback(GLFWwindow* window, int count, const char** paths);
void monitor_callback(GLFWmonitor* monitor, int action);
void joystick_callback(int jid, int event);
void scroll_callback(GLFWwindow* window, double scroll, double scrollx);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
RenderData updateRenderData(RenderData renderData, unsigned int depthTexture, int brushSizeIndicatorSize);

//--------Functions--------\\

CallbckData callbackData;
PanelData panelData;

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
bool doPainting = false;//Prevent painting & hide the painting cursor
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
int chosenNodeResIndex = 1; //0:256 1:512 2:1024 3:2048 
int chosenSkyboxTexture = 1;
bool duplicateNodeCall = false;
aTexture paintOverTexture;
bool selectingPaintOverTexture;
Cubemaps cubemaps;
LigidCursors cursors;
Icons icons;
Audios audios;
Model sphereModel;
int currentMaterialIndex = 0;
bool textureDraggingState = false;
bool mirrorRangeBarsPressed = false;
bool displayProjectFolderManager = true;
glm::mat4 modelMatrix = glm::mat4(1);


ExportData exportData;
PBRShaderData pbrShaderData;
SkyBoxShaderData skyBoxShaderData;
ScreenDepthShaderData screenDepthShaderData;
AxisPointerShaderData axisPointerShaderData;

int messageBoxRes = 0; //0 : ignore / 1 : yes / -1 : no

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if(action == 1 && button == 0)
		firstClick = true;
    if(action == 1 && button == 1)
		firstClickR = true;
}
	bool brushMaskTextureSelectionPanelActive = false;

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

	glfwSetWindowAttrib(window,GLFW_DECORATED,GLFW_FALSE);
	glfwSetWindowSize(window,glfwGetVideoMode(glfwGetPrimaryMonitor())->width/3.5,glfwGetVideoMode(glfwGetPrimaryMonitor())->height/3.5);
	glfwSetWindowPos(window,glfwGetVideoMode(glfwGetPrimaryMonitor())->width/2-(glfwGetVideoMode(glfwGetPrimaryMonitor())->width/7),glfwGetVideoMode(glfwGetPrimaryMonitor())->height/2-(glfwGetVideoMode(glfwGetPrimaryMonitor())->height/7));
	
	glViewport(0,0,glfwGetVideoMode(glfwGetPrimaryMonitor())->width/3.5,glfwGetVideoMode(glfwGetPrimaryMonitor())->height/3.5);


	//
	prepGLFW(window,monitor_callback,framebuffer_size_callback,key_callback,mouse_callback,scroll_callback,joystick_callback,mouse_button_callback,drop_callback);
	
	//Create square vbo and custom vbo
	//Square vao has the coordinates of a square
	//Custom buffer is used for nonsquare or unstable coordinates
	prepVertexBuffers(objects.sqrVBO,objects.sqrVAO,objects.VBO,objects.VAO);

	prepStartLoadingScreen(window,programs.renderTheTextureProgram,viewportBGImage);
	
	glEnable(GL_BLEND);
	glDepthFunc(GL_LESS);
	glEnable(GL_MULTISAMPLE);
	glEnable(GL_DEPTH_TEST);
	
	//Generate audio objects via libal
	prepLibAL(audios.MessageBox,audios.Login,audios.ButtonEnter,audios.Alert);

	prepLoadStuff(cubemaps,icons,cursors,textures,UIElements,FBOScreen,programs,maskTexturePath);






	Load load;



	txtr.initUpdateMaskTxtr();
	textureGen.initDrawToScreen();

	//Create the default material out (for model)
	MaterialOut mOut;
	mOut.program = 0;
	modelMaterials.push_back(mOut);
	//Set context
	glfwMakeContextCurrent(window);
	
	//Create screen painting mask Texture
	GLubyte* screenTexture = new GLubyte[(glfwGetVideoMode(glfwGetPrimaryMonitor())->width) * (glfwGetVideoMode(glfwGetPrimaryMonitor())->height)*4];
	ScreenPaintingReturnData screenPaintingReturnData; 
	screenPaintingReturnData = txtr.createScreenPaintTexture(screenTexture,window);
	delete[] screenTexture;
	//Load screen painting FBO
	unsigned int paintingFBO = load.getPaintingFBO(windowData,screenPaintingReturnData.normalId);

	//TODO : Rewrite
	glm::mat4 perspectiveProjection = render.setMatrices();



	glset.genTextures(depthTextureID);


	//Send some data to related cpp files
	//--Send programs struct
	ui.sendProgramsToUserInterface(programs);
	render.sendProgramsToRender(programs);
	txtr.sendProgramsToTextures(programs);
	//--Send max window size
	ui.sendObjectsToUI(objects,icons.Circle,icons.SmoothSquare);

	model.sendObjectsToModel(objects.VAO,objects.VBO);
	sphereModel.sendObjectsToModel(objects.VAO,objects.VBO);

	glfwSetWindowSizeLimits(window,glfwGetVideoMode(glfwGetPrimaryMonitor())->width/1.7,0,glfwGetVideoMode(glfwGetPrimaryMonitor())->width,glfwGetVideoMode(glfwGetPrimaryMonitor())->height);


	


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

	screenWidth = glfwGetVideoMode(glfwGetPrimaryMonitor())->width;
	screenHeight = glfwGetVideoMode(glfwGetPrimaryMonitor())->height;

	MainLoop mainLoop;
	
	glfwSetWindowAttrib(window,GLFW_DECORATED,GLFW_TRUE);
	glfwSetWindowSize(window,glfwGetVideoMode(glfwGetPrimaryMonitor())->width,glfwGetVideoMode(glfwGetPrimaryMonitor())->height);
	glfwSetWindowPos(window,0,20);
	
	glfwPollEvents();

	//unsigned int qrTxtr = util.createQRCode("https://ligidtools.com/ligidpainter",colorData.LigidPainterThemeColor);

	aTexture trashFolder;
	trashFolder.name = "Trash";
	trashFolder.isTexture = false;
	trashFolder.isTrashFolder = true;
	albedoTextures.push_back(trashFolder);

	aTexture exportFolder;
	exportFolder.isTexture = false;
	exportFolder.name = "Export";
	aTexture brushFolder;
	brushFolder.isTexture = false;
	brushFolder.name = "Brush Textures";
	
	aTexture maskFolder;
	maskFolder.folderIndex = 2;
	maskFolder.isTexture = false;
	maskFolder.name = "Mask";
	aTexture rgbFolder;
	rgbFolder.folderIndex = 2;
	rgbFolder.isTexture = false;
	rgbFolder.name = "RGB";
	aTexture normalMapFolder;
	normalMapFolder.folderIndex = 2;
	normalMapFolder.isTexture = false;
	normalMapFolder.name = "Normal Map";
	
	
	albedoTextures.push_back(exportFolder);	
	albedoTextures.push_back(brushFolder);
	
	albedoTextures.push_back(maskFolder);
	albedoTextures.push_back(rgbFolder);
	albedoTextures.push_back(normalMapFolder);

	//load.loadBrushMaskTextures(albedoTextures);

	
	//aTexture aqrTxtr;
	//aqrTxtr.id = qrTxtr;
	//aqrTxtr.name = "LigidTools.com";
	//albedoTextures.push_back(aqrTxtr);
	
	//addSphereButton();

	bool monitorUsed = false;

	bool f11done = false;

	int whileCounter = 0;


	//Is used for f11
	GLFWmonitor* lastmonitor;
	int lastWidth;
	int lastHeight;
	int lastPosx;
	int lastPosy;


	bool countTheClicks = false;
	int clickCounter = 0;
	int mouseClickPosx = 0;
	int mouseClickPosy = 0;

	bool moveCamera = false;
	bool zoomInOutCamera = false;
	
	double lastTime = glfwGetTime();

	subSelectedImagePowerRangeBar(0,glfwGetVideoMode(glfwGetPrimaryMonitor())->width,glfwGetVideoMode(glfwGetPrimaryMonitor())->height);

	bool firstStroke = false;

	


	glActiveTexture(GL_TEXTURE28);
	int defaultNodePosCorrectorCounter = 0;
	bool didDefaultNodesMakeToTheCenter = false;


	sphereModel.loadModel("./LigidPainter/Resources/3D Models/triangulatedHalfSphere.obj",true);
	sphereModel.meshes[0].sendObjectsToMesh(objects.VAO,objects.VBO);
	glBindBuffer(GL_ARRAY_BUFFER, objects.VBO);
	glBindVertexArray(objects.VAO);

	addSphereButton();

	glHint(GL_LINE_SMOOTH_HINT,GL_FASTEST);
	glHint(GL_POLYGON_SMOOTH_HINT,GL_FASTEST);
	glHint(GL_TEXTURE_COMPRESSION_HINT,GL_FASTEST);
	glHint(GL_FRAGMENT_SHADER_DERIVATIVE_HINT,GL_FASTEST);

	unsigned int materialFBO;
	glset.genFramebuffers(materialFBO);

	txtr.refreshScreenDrawingTexture();
	LigidPainter lp;

	txtr.updateMaskTexture(FBOScreen,glfwGetVideoMode(glfwGetPrimaryMonitor())->width,glfwGetVideoMode(glfwGetPrimaryMonitor())->height,UIElements[UIbrushRotationRangeBar].rangeBar.value,false,UIElements[UIbrushBordersRangeBar].rangeBar.value,brushBlurVal,outShaderData,programs,glfwGetVideoMode(glfwGetPrimaryMonitor())->width,glfwGetVideoMode(glfwGetPrimaryMonitor())->height);	

	prepPrograms(programs,UIElements,modelMatrix);

	if(!std::filesystem::is_directory("./Projects"))
		std::filesystem::create_directories("./Projects");

	while (!glfwWindowShouldClose(window))//Main loop
	{
		for (size_t i = 0; i < albedoTextures.size(); i++)
		{
			if(albedoTextures[i].isTexture)
				util.extensionCheckForTexture(albedoTextures[i].name);
		}
		
		if(!startScreen && !didDefaultNodesMakeToTheCenter && !createProject)
			defaultNodePosCorrectorCounter++;

		if(defaultNodePosCorrectorCounter > 10){
			didDefaultNodesMakeToTheCenter = true;
			defaultNodePosCorrectorCounter = 0;
			nodeScenes[0].nodes[1].positionX = -0.5f;
			nodeScenes[0].nodes[1].positionY = -2.1f;
			
			nodeScenes[0].nodes[0].positionX = 0.f;
			nodeScenes[0].nodes[0].positionY = -1.6f;
		}

		whileCounter++;
		if(whileCounter > 10000)
			whileCounter = 0;

		glfwPollEvents();
		if(UIElements[UIstabilizeFpsCheckBox].checkBox.checked)
			glfwSwapInterval(1);
		else
			glfwSwapInterval(0);
		
		util.printRenderingSpeed(debugMode,window);
		
		// util.printError();
		if(selectingPaintOverTexture && !textureSelectionPanel.active)
			selectingPaintOverTexture = false;

		if(selectingPaintOverTexture && textureSelectionPanel.textureClicked){
			textureSelectionPanel.textureClicked = false;
			paintOverTexture = albedoTextures[textureSelectionPanel.selectedIndex];
			textureSelectionPanel.active = false;
			selectingPaintOverTexture = false;
			UIElements[UIselectPaintOverTextureNameTextElement].text.text = util.cropString(paintOverTexture.name,13);
		}

		mainLoop.updateCameraPosChanging(callbackData.cameraPos,cameraPosChanging);
		//mainLoop.detectClick(window,mousePress,firstClick);
		
		if(firstClick){
			clickCounter = 0;
			if(countTheClicks && glm::distance(glm::vec2(mouseClickPosx,mouseClickPosy),glm::vec2(mouseXpos,mouseYpos)) < 30){
				countTheClicks = false;
				if(!callbackData.mainPanelEnter && !nodePanel.panelHover && !nodePanel.boundariesHover && !nodePanel.boundariesPressed && !sndPanel.panelHover && !coloringPanel.active && !textureSelectionPanel.active && !txtrCreatingPanel.active)
					doubleClick = true;
			}
			else{
				mouseClickPosx = mouseXpos;
				mouseClickPosy = mouseYpos;
				countTheClicks = true;	
			}
		}
		
		double currentTime = glfwGetTime();
		
		if(brushMaskTextureSelectionPanelActive){
			if(textureSelectionPanel.textureClicked){
				//Import brush mask textures from albedotextures
				brushMaskTextureSelectionPanelActive = false;
				textureSelectionPanel.active = false;
				
				glActiveTexture(GL_TEXTURE28);
				glBindTexture(GL_TEXTURE_2D,albedoTextures[textureSelectionPanel.selectedIndex].id);

				aTexture atxtr = albedoTextures[textureSelectionPanel.selectedIndex];
				GLubyte* txtrData = txtr.getTextureFromProgram(GL_TEXTURE28,atxtr.width,atxtr.height,4);

				glActiveTexture(GL_TEXTURE0);
				unsigned int texture;
				glGenTextures(1,&texture);
				glBindTexture(GL_TEXTURE_2D,texture);
				glset.texImage(txtrData,atxtr.width,atxtr.height,GL_RGBA);
				glset.generateMipmap();

				Utilities util;

				atxtr.id = texture;	
				atxtr.changed = true;

				if(UIElements[UImaskPaintingCheckBoxElement].checkBox.checked)
					atxtr.folderIndex = 3;
				else if(UIElements[UIcolorPaintingCheckBoxElement].checkBox.checked)
					atxtr.folderIndex = 4;
				else if(UIElements[UInormalmapPaintingCheckBoxElement].checkBox.checked)
					atxtr.folderIndex = 5;

				albedoTextures.push_back(atxtr);
				
				textureSelectionPanel.textureClicked = false;
			}
		}

		if(countTheClicks){
			if (currentTime - lastTime >= 0.02) {
				clickCounter++;
				lastTime = glfwGetTime();
			}
		}
		if(clickCounter > 20)
			countTheClicks = false;
		
		if(doubleClick)
			txtr.refreshScreenDrawingTexture();
		if(doubleClick){
			moveCamera = true;
		}
		if(glfwGetMouseButton(window, 0) == GLFW_RELEASE){
			moveCamera = false;
		}
		
		if(firstClickR){
			clickCounter = 0;
			if(countTheClicks && glm::distance(glm::vec2(mouseClickPosx,mouseClickPosy),glm::vec2(mouseXpos,mouseYpos)) < 30){
				countTheClicks = false;
				if(!mainPanelHover && !nodePanel.panelHover && !sndPanel.panelHover && !coloringPanel.active && !textureSelectionPanel.active && !txtrCreatingPanel.active)
					doubleClickR = true;
			}
			else{
				mouseClickPosx = mouseXpos;
				mouseClickPosy = mouseYpos;
				countTheClicks = true;	
			}
		}
		
		if(countTheClicks){
			if (currentTime - lastTime >= 0.02) {
				clickCounter++;
				lastTime = glfwGetTime();
			}
		}
		if(clickCounter > 20)
			countTheClicks = false;
		
		if(doubleClickR)
			txtr.refreshScreenDrawingTexture();
		if(doubleClickR){
			zoomInOutCamera = true;
		}
		if(glfwGetMouseButton(window, 1) == GLFW_RELEASE){
			zoomInOutCamera = false;
		}


		mainLoop.updateRenderTheScene(window,renderTheSceneCounter,renderTheScene);
		mainLoop.setContextPanelsStates(window,coloringPanel,addNodeContextMenu,textureSelectionPanel);
		ui.sendTextBoxActiveCharToUI(textBoxActiveChar);

		LigidPainter lp;
		if(whileCounter % 10 == 0)
			lp.setViewportToDefault();

		if(glfwGetKey(window,GLFW_KEY_F11) == GLFW_PRESS){
			
			if(!f11done){
				if(!monitorUsed){
					lastmonitor = glfwGetWindowMonitor(window);
					glfwGetWindowSize(window,&lastWidth,&lastHeight);
					glfwGetWindowPos(window,&lastPosx,&lastPosy);
					
					monitorUsed = true;
					glfwSetWindowMonitor(window,glfwGetPrimaryMonitor(),0,0,glfwGetVideoMode(glfwGetPrimaryMonitor())->width,glfwGetVideoMode(glfwGetPrimaryMonitor())->height,glfwGetVideoMode(glfwGetPrimaryMonitor())->refreshRate);
				}
				else{
					monitorUsed = false;
					glfwSetWindowMonitor(window,lastmonitor,lastPosx,lastPosy,lastWidth,lastHeight,glfwGetVideoMode(glfwGetPrimaryMonitor())->refreshRate);
				}
			}
			f11done = true;
		}
		else{
			f11done = false;
		}

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


		if(glfwGetKey(window,GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS && glfwGetKey(window,GLFW_KEY_S) == GLFW_PRESS){
			displayProjectFolderManager = true;
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
		if (cameraPosChanging || mirrorClick || glfwGetKey(window,GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS){
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
		outShaderData.view = viewUpdateData.view;
		outShaderData.viewPos = viewUpdateData.cameraPos;

		//Render
		//double firstTime = glfwGetTime();
		if(renderTheScene){
			renderOut = render.render(renderData, FBOScreen, panelData,exportData,icons,renderPlane,renderSphere,pbrShaderData,skyBoxShaderData
										,brushBlurVal,screenDepthShaderData,axisPointerShaderData,outShaderData,model,albedoTextures,paintRender,materialsPanelSlideValue,UIElements,colorPicker
										,textureDisplayer,cubemaps,addNodeContextMenu,nodePanel,sndPanel,selectedAlbedoTextureIndex,textureSelectionPanel,nodeScenes,selectedNodeScene
										,appNodes,perspectiveProjection,viewUpdateData.view, modelMaterials,newModelAdded,firstClick,viewUpdateData.cameraPos,coloringPanel,
										txtrCreatingPanel,chosenTextureResIndex,chosenSkyboxTexture,bakeTheMaterial,anyTextureNameActive,textureText,viewportBGImage,nodeScenesHistory
										,brushMaskTextures,callbackData.maskPanelEnter,duplicateNodeCall,objects,chosenNodeResIndex,drawColor,mirrorParams,depthTextureID,callbackData.cameraPos,
										 callbackData.originPos,startScreen,projectFilePath,paintOverTexture,sphereModel,audios,materialFBO,currentMaterialIndex,textureDraggingState
										 ,debugMode,createProject,modelFilePath,modelName,customModelName,modelMatrix,displayProjectFolderManager,fonts,projectManager,firstClickR,generatedTextTxtr
										 ,txtrGenSelectedFont);
		}
		duplicateNodeCall = false;
		
		//double lastTime = glfwGetTime();
		//cout <<  (lastTime - firstTime) * 1000  << '\n';
		
		paintRender = false;

		//
		if(panelData.paintingPanelActive)
			colorPicker.updateHueVal = false; //Hue value will be updated after rendering the ui
		
		drawColor = colorPicker.pickerValue/255.0f;
		colorPicker.saturationValueValChanged = false;
		colorPicker.hexValTextBoxGotInput = false;

		if(firstClick)
			firstStroke = true;

		//Painting
		const bool distance_spacingCompatibility = glm::distance(glm::vec2(mouseDrawingPosX,mouseDrawingPosY),glm::vec2(mouseXpos,mouseYpos)) > paintingSpacing;
		const bool paintingCondition = glfwGetMouseButton(window, 0) == GLFW_PRESS && doPainting && !panelChanging && glfwGetMouseButton(window, 1) == GLFW_RELEASE && !colorPicker.dropperActive && (distance_spacingCompatibility) && !coloringPanel.saturationValueBoxPointerPressed && !coloringPanel.hueBarPointerPressed; 
		if (paintingCondition && !moveCamera && (glfwGetKey(window,GLFW_KEY_Z) == GLFW_RELEASE) && glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_RELEASE && glfwGetKey(window, GLFW_KEY_F3) == GLFW_RELEASE){
			firstStroke = false;
			

			mouseDrawingPosX = mouseXpos;
			mouseDrawingPosY = mouseYpos;

			//Paint
			textureGen.drawToScreen(window, screenPaintingReturnData.normalId, brushSize, FBOScreen,UIElements[UIbrushRotationRangeBar].rangeBar.value,UIElements[UIbrushOpacityRangeBar].rangeBar.value,lastMouseXpos, lastMouseYpos,mouseXpos,mouseYpos,UIElements[UIuseNegativeCheckBox].checkBox.checked,brushValChanged,programs,glfwGetVideoMode(glfwGetPrimaryMonitor())->width,glfwGetVideoMode(glfwGetPrimaryMonitor())->height,UIElements[UIbrushBordersRangeBar].rangeBar.value,brushBlurVal,paintingFBO,outShaderData,model,modelMaterials, paintingSpacing < 10,viewUpdateData.view,mirrorParams,callbackData.cameraPos, callbackData.originPos,UIElements[UImirrorXRangeBarElement].rangeBar.value*40.f,UIElements[UImirrorYRangeBarElement].rangeBar.value*40.f,UIElements[UImirrorZRangeBarElement].rangeBar.value*40.f,UIElements[UIdynamicPaintingCheckBoxElement].checkBox.checked,firstStroke,UIElements[UIbrushSpacingRangeBar].rangeBar.value,UIElements[UIuseUVCheckBox].checkBox.checked);
			paintRenderCounter++;
			if(paintRenderCounter == 50000){
				paintRender = true;
				paintRenderCounter = 0;
			}

			brushValChanged = false; //After updating the brush mask texture set brushValChanged to false so brush mask texture won't be updated repeatedly 
			paintingMode = true;
		}

		if (doScrollAfterCallInPaintingMode || zoomInOutCamera || (glfwGetKey(window,GLFW_KEY_Z) == GLFW_PRESS && glfwGetKey(window,GLFW_KEY_LEFT_CONTROL) == GLFW_RELEASE)) //Do a scroll after texture update using holdScroll
		{
			float scrollVal;
			if(doScrollAfterCallInPaintingMode)
				scrollVal = holdScrollVal;
			if(zoomInOutCamera || (glfwGetKey(window,GLFW_KEY_Z) == GLFW_PRESS && glfwGetKey(window,GLFW_KEY_LEFT_CONTROL) == GLFW_RELEASE))
				scrollVal = mouseYpos - lastMouseYpos;
			if(!startScreen && !createProject)
				callbackData = callback.scroll_callback(window, 0, scrollVal);
			doScrollAfterCallInPaintingMode = false;
		}

		panelChanging = false;
		lastMouseXpos = mouseXpos;
		lastMouseYpos = mouseYpos;


		mirrorRangeBarsPressed = false;


		if(renderOut.maskPanelMaskClicked){
			//If a new mask texture is selected from mask texture panel set brushValChanged to true to update brush mask texture 
			brushValChanged = true;
		}


		//After rendering
		exportImage = false;
		textureDisplayer.buttonClicked = false;

		float maskPanelSliderValue = brushMaskTextures.maskTexturesSliderValue;

		if(UIElements[UIcolorPaintingCheckBoxElement].checkBox.checked){
			maskPanelSliderValue = brushMaskTextures.colorTexturesSliderValue;
		}
		if(UIElements[UInormalmapPaintingCheckBoxElement].checkBox.checked){
			maskPanelSliderValue = brushMaskTextures.normalTexturesSliderValue;
		}

		if(!startScreen && !createProject)
			callbackData = callback.mouse_callback(window, mouseXpos, mouseYpos, panelData,maskPanelSliderValue,renderOut.maskPanelMaskHover,cursors,renderOut.texturePanelButtonHover,UIElements,mainPanelLoc,colorPicker,textureDisplayer,nodePanel,addNodeContextMenu,sndPanel,coloringPanel,moveCamera,zoomInOutCamera);


		mirrorClick = false;

		if ((cameraPosChanging && paintingMode) || glfwGetMouseButton(renderData.window, 0) == GLFW_RELEASE) { //Changing camera position or painting a stroke ends painting, than updates painted texture
			paintingMode = false;
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
		doubleClick = false;
		firstClick = false;
		doubleClickR = false;
		firstClickR = false;
		
		if(glfwWindowShouldClose(window) && !startScreen){
			if(!ligidMessageBox("LigidPainter will be closed. Do you want to proceed?",-0.21f,"",0))
				glfwSetWindowShouldClose(window,GLFW_FALSE);
		}
	}

	//Close the program
	glfwDestroyWindow(window);
	glfwTerminate();
	return true;
}

int LigidPainter::ligidMessageBox(std::string message,float messagePosX,std::string bMessage,float bMessagePosX){
	
	bool noButtonClick = true;
	bool clickTaken = false;

	
	using namespace std::chrono_literals;

	ColorData colorData;
	Render render;
	LibAL_stopPlaying(audios.MessageBox);
	LibAL_playAudioObject(audios.MessageBox);
	
	std::this_thread::sleep_for(100ms);
	
	glm::mat4 projection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f);
	glUseProgram(programs.renderTheTextureBlur);
	glset.uniformMatrix4fv(programs.renderTheTextureBlur, "TextProjection", projection);
	glset.uniform1i(programs.renderTheTextureBlur, "txtr", 14);
	
	while (true)
	{

		//Disable painting
		doPainting = false; 
		//renderData.doPainting = doPainting;
		glfwPollEvents();
		//Keep rendering the backside
		glClearColor(colorData.viewportBackColor.r,colorData.viewportBackColor.g,colorData.viewportBackColor.b,1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //Clear before rendering

		glUseProgram(programs.renderTheTextureBlur);
		float blurVal = 0.5f;
		render.renderBlurySkybox(cubemaps,skyBoxShaderData,programs,UIElements,blurVal,1.f,1.f,0,0,false);
		glUseProgram(programs.uiProgram);
		
		
		float messageBoxBackColor[3] = {colorData.messageBoxPanelColor.r,colorData.messageBoxPanelColor.g,colorData.messageBoxPanelColor.r};
		float messageBoxButtonColor[3] = {colorData.messageBoxButtonColor.r,colorData.messageBoxButtonColor.g,colorData.messageBoxButtonColor.r};
		//render the message box
		int result = lgdMessageBox(window,mouseXpos,mouseYpos,cursors.defaultCursor,cursors.pointerCursor,icons.Logo,programs.uiProgram,message.c_str(),messagePosX,0.0f,messageBoxBackColor,messageBoxButtonColor,(float)glfwGetVideoMode(glfwGetPrimaryMonitor())->width, (float)screenWidth,programs.iconsProgram,icons,programs,bMessage,bMessagePosX); //0 = Yes //1 = No //2 = None
		//Process the message box input
		if(result == 0 || glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS){
			return 1;
		}
		else if(result == 1 || glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS){
			return 0;
		}
		
		glfwSwapBuffers(window);
	}
}




//-------------CALLBACK-------------\\

bool caps = false; //GLFW_MOD_CAPS_LOCK
void joystick_callback(int jid, int event)
{
    if (event == GLFW_CONNECTED)
    {
        // The joystick was connected
    }
    else if (event == GLFW_DISCONNECTED)
    {
        // The joystick was disconnected
    }
}
void monitor_callback(GLFWmonitor* monitor, int action){
	if (action == GLFW_CONNECTED)
    {
        //Monitor connected
		LigidPainter lp;
		lp.setViewportToDefault();
	}
    else if (action == GLFW_DISCONNECTED)
    {
        //Monitor disconnected
		LigidPainter lp;
		lp.setViewportToDefault();
	}
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	//------------------TEXT------------------
	
	if(glfwGetKey(window,GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS && key == GLFW_KEY_D && action == 0){
		duplicateNodeCall = true;
	}

	if(addNodeContextMenu.active && key != GLFW_KEY_ENTER && key != GLFW_KEY_LEFT && key != GLFW_KEY_LEFT && key != GLFW_KEY_DOWN && key != GLFW_KEY_UP){
		if(addNodeContextMenu.searchText == "search")
			addNodeContextMenu.searchText = "";
		addNodeContextMenu.searchTextboxActive = true;
	}

	if(key == GLFW_KEY_UP && (action == 0 || action == 2)){
		if(addNodeContextMenu.selectedButtonIndex != 0)
			addNodeContextMenu.selectedButtonIndex--;
		else{
			Utilities util;
			addNodeContextMenu.scroll--;
			addNodeContextMenu.scroll = util.restrictBetween(addNodeContextMenu.scroll, addNodeContextMenu.buttons.size()-10, 0);
		}
	}
	else if(key == GLFW_KEY_DOWN && (action == 0 || action == 2)){
		if(addNodeContextMenu.selectedButtonIndex != 9)
			addNodeContextMenu.selectedButtonIndex++;
		else{
			Utilities util;
			addNodeContextMenu.scroll++;
			addNodeContextMenu.scroll = util.restrictBetween(addNodeContextMenu.scroll, addNodeContextMenu.buttons.size()-10, 0);
		}
		
	}
	else if(key != GLFW_KEY_DOWN && key != GLFW_KEY_UP && key != GLFW_KEY_ENTER){
		addNodeContextMenu.selectedButtonIndex = 0;
	}

	renderTheScene = true;
	renderTheSceneCounter = 0;

	if(key == 280 && action == 0){
		//Change capslock's state
		caps = !caps;
	}


	UserInterface ui;
	if(UIElements[UIexportingFolderNameTextBox].textBox.clicked){
		if(ui.textInput(key,action,caps,exportFileName,100,window,textBoxActiveChar)){
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

	if(UIElements[UIgenerateTextTextureTextTextBoxElement].textBox.clicked){
		if(ui.textInput(key,action,caps,UIElements[UIgenerateTextTextureTextTextBoxElement].textBox.text,200,window,textBoxActiveChar)){
		}
		if(action == 1 || action == 2){
			if(glfwGetKey(window,GLFW_KEY_LEFT) == GLFW_PRESS && UIElements[UIgenerateTextTextureTextTextBoxElement].textBox.text.size()+textBoxActiveChar){
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
		if(ui.textInput(key,action,caps,txtrCreatingPanel.textBoxVal,200,window,textBoxActiveChar)){
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
		ui.textInput(key,action,caps,textureText,200,window,textBoxActiveChar);
		
		if(action == 1 || action == 2){
			if(glfwGetKey(window,GLFW_KEY_LEFT) == GLFW_PRESS && textureText.size()+textBoxActiveChar){
				textBoxActiveChar--;
			}
			if(glfwGetKey(window,GLFW_KEY_RIGHT) == GLFW_PRESS && textBoxActiveChar != 0){
				textBoxActiveChar++;
			}
		}
	}
	
	if(addNodeContextMenu.active){
		ui.textInput(key,action,caps,addNodeContextMenu.searchText,15,window,textBoxActiveChar);
		
		if(action == 1 || action == 2){
			if(glfwGetKey(window,GLFW_KEY_LEFT) == GLFW_PRESS && addNodeContextMenu.searchText.size()+textBoxActiveChar){
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
	Callback cb;
	cb.key_callback(key,action,window);
}

void drop_callback(GLFWwindow* window, int count, const char** paths){
	//Load texture
	Texture txtr;
		std::vector<std::string> albedoTexturePaths;
		for (size_t i = 0; i < count; i++)
		{
			albedoTexturePaths.push_back(paths[i]);
		}
		
		
		for (size_t i = 0; i < albedoTexturePaths.size(); i++)
		{
			aTexture result;
			
			glActiveTexture(GL_TEXTURE0);
			
			result.id = txtr.getTexture(albedoTexturePaths[i],false,result.width,result.height); //Force albedo's ratio to be 1:1
			
			Utilities util;
			 
			std::vector<std::string> textureNames;
			for (size_t i = 0; i < albedoTextures.size(); i++)
			{
				textureNames.push_back(albedoTextures[i].name);
			}
			//Rename if necessary
			result.name = util.removeExtension(util.getLastWordBySeparatingWithChar(albedoTexturePaths[i],folderDistinguisher)); 
			result.name = util.uniqueName(result.name,textureNames);
			albedoTextures.push_back(result);
			glActiveTexture(GL_TEXTURE28);
		}
		
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	Callback clback;
	UserInterface ui;
	UiActions uiAct;
	Render render;
	Texture txtr;

	renderTheScene = true;
	renderTheSceneCounter = 0;

	Callback callback;
	callback.framebuffer_size_callback(window,width,height);

	screenWidth = width;
	screenHeight = height;
}

void LigidPainter::setViewportToDefault(){
	GLFWvidmode videomode;

	#ifdef __APPLE__//2560 1664
		glViewport(-(glfwGetVideoMode(glfwGetPrimaryMonitor())->width - screenWidth)/2, -(glfwGetVideoMode(glfwGetPrimaryMonitor())->height - screenHeight), glfwGetVideoMode(glfwGetPrimaryMonitor())->width,glfwGetVideoMode(glfwGetPrimaryMonitor())->height);
	#else
		glViewport(-(glfwGetVideoMode(glfwGetPrimaryMonitor())->width - screenWidth)/2, -(glfwGetVideoMode(glfwGetPrimaryMonitor())->height - screenHeight), glfwGetVideoMode(glfwGetPrimaryMonitor())->width,glfwGetVideoMode(glfwGetPrimaryMonitor())->height);
	#endif
}

double lastXpos;
double lastYpos;
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if(startScreen || createProject){
		if(nodePanel.pointerCursor)
			glfwSetCursor(window, cursors.pointerCursor);
		else
			glfwSetCursor(window, cursors.defaultCursor);
	}

	renderTheScene = true;
	renderTheSceneCounter = 0;

	UiActions uiAct; 
	mouseXpos = xpos;
	mouseYpos = ypos;
	double xOffset;
	double yOffset;
	
	int width;
	int height;
	glfwGetFramebufferSize(window,&width,&height);
	
	//Get mouse position offset
	xOffset = (lastXpos - xpos) / ((double)glfwGetVideoMode(glfwGetPrimaryMonitor())->width / (double)width);
	yOffset = (lastYpos - ypos) / ((double)glfwGetVideoMode(glfwGetPrimaryMonitor())->height / (double)height);
	lastXpos = xpos;
	lastYpos = ypos;
	

	float screenGapX = ((float)glfwGetVideoMode(glfwGetPrimaryMonitor())->width - (float)width); 



	//Texture demonstrator corner hover
	float range = 0.05f;
	if(xpos-screenGapX/2 > ((textureDisplayer.buttonPosX + textureDisplayer.width) - range) * width/2 && xpos-screenGapX/2 < ((textureDisplayer.buttonPosX + textureDisplayer.width) + range) * width/2 && height - ypos > ((textureDisplayer.buttonPosY+1.0f - textureDisplayer.height) - range) * height/2 && height - ypos < ((textureDisplayer.buttonPosY+1.0f - textureDisplayer.height) + range) * height/2 ){
		textureDisplayer.cornerHover = true;
	}
	else{
		textureDisplayer.cornerHover = false;
	}


	
	bool hideCursor = uiAct.updateRangeValues(window,xOffset,yOffset,width,height,textureDisplayer,sndPanel,UIElements); 



	if(colorPicker.dropperActive || textureDraggingState || mirrorRangeBarsPressed || textureSelectionPanel.active || UIElements[UImirrorXCheckBox].checkBox.mouseHover || UIElements[UImirrorYCheckBox].checkBox.mouseHover || UIElements[UImirrorZCheckBox].checkBox.mouseHover){
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
	coloringPanel.active = false;

	renderTheScene = true;
	renderTheSceneCounter = 0;

	Callback callback;
	Utilities util;
	LigidPainter ligid;
	int screenSizeX;
	int screenSizeY;
	glfwGetFramebufferSize(window,&screenSizeX,&screenSizeY);

	if(projectManager.elementPanelHover && displayProjectFolderManager){
		if(scrollx > 0 && projectManager.projectManagerScrollVal != 0)
			projectManager.projectManagerScrollVal++;
		if(scrollx < 0)
			projectManager.projectManagerScrollVal--;
	}

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
			if(UIElements[UImaskPaintingCheckBoxElement].checkBox.checked){
				int txtrSize = 0;
				for (size_t i = 0; i < albedoTextures.size(); i++)
				{
					if(albedoTextures[i].folderIndex == 3)
						txtrSize++;
				}
				brushMaskTextures.maskTexturesSliderValue += (float)(scrollx / 40.0);
				const float maskPanelRange = ceil((float)txtrSize/3.f) / 8.33333333333 - (0.8f - 0.55f); 
				brushMaskTextures.maskTexturesSliderValue = util.restrictBetween(brushMaskTextures.maskTexturesSliderValue, 0.0f, -maskPanelRange/4.f);//Keep in boundaries
			}
			if(UIElements[UIcolorPaintingCheckBoxElement].checkBox.checked){
				int txtrSize = 0;
				for (size_t i = 0; i < albedoTextures.size(); i++)
				{
					if(albedoTextures[i].folderIndex == 4)
						txtrSize++;
				}
				brushMaskTextures.colorTexturesSliderValue += (float)(scrollx / 40.0);
				const float maskPanelRange = ceil((float)txtrSize/3.f) / 8.33333333333 - (0.8f - 0.55f); 
				brushMaskTextures.colorTexturesSliderValue = util.restrictBetween(brushMaskTextures.colorTexturesSliderValue, 0.0f, -maskPanelRange/4.f);//Keep in boundaries
			}
			if(UIElements[UInormalmapPaintingCheckBoxElement].checkBox.checked){
				int txtrSize = 0;
				for (size_t i = 0; i < albedoTextures.size(); i++)
				{
					if(albedoTextures[i].folderIndex == 5)
						txtrSize++;
				}
				brushMaskTextures.normalTexturesSliderValue += (float)(scrollx / 40.0);
				const float maskPanelRange = ceil((float)txtrSize/3.f) / 8.33333333333 - (0.8f - 0.55f); 
				brushMaskTextures.normalTexturesSliderValue = util.restrictBetween(brushMaskTextures.normalTexturesSliderValue, 0.0f, -maskPanelRange/4.f);//Keep in boundaries
			}
		}
		else if(addNodeContextMenu.active){
			if(scrollx > 0)
				addNodeContextMenu.scroll --;
			if(scrollx < 0)
				addNodeContextMenu.scroll ++;
			addNodeContextMenu.scroll = util.restrictBetween(addNodeContextMenu.scroll, addNodeContextMenu.buttons.size()-10, 0);//Keep in boundaries
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
		else if(callbackData.mainPanelEnter && panelData.texturePanelActive){
			//Materials
			int matsize = model.meshes.size();
			for (size_t i = 0; i < model.meshes.size(); i++)
			{
				matsize += model.meshes[i].submeshes.size();
			}
			
			if(matsize >= 10){
				materialsPanelSlideValue += scrollx/10.0f;
				if(materialsPanelSlideValue < 0.0f)
					materialsPanelSlideValue = 0.0f;
			}
		}
		else if(callbackData.mainPanelEnter && panelData.paintingPanelActive){
			 panelData.paintingPanelSlideVal -= scrollx/10.0f;
			 if(panelData.paintingPanelSlideVal < 0.0f)
			 	panelData.paintingPanelSlideVal = 0.0f;
		}
		else if(callbackData.mainPanelEnter && panelData.settingsPanelActive){
			 panelData.settingsPanelSlideVal -= scrollx/10.0f;
			 if(panelData.settingsPanelSlideVal < 0.0f)
			 	panelData.settingsPanelSlideVal = 0.0f;
		}
		else if(callbackData.mainPanelEnter && panelData.generatorPanelActive){
			 //panelData.generatorPanelSlideVal -= scrollx/10.0f;
			 //if(panelData.generatorPanelSlideVal < 0.0f)
			 //	panelData.generatorPanelSlideVal = 0.0f;
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
	glfwGetFramebufferSize(window,&width,&height);
	
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
		saturationValuePosX = ((hsvVal.g / 1342.10526316f) - 0.099f); //0.095
		saturationValuePosY = (hsvVal.b / 653.846153846f) - 0.199f; //0.195
	}
}

//-----------------------------UI ACTIONS-----------------------------\\

void LigidPainter:: addMaskTextureButton() {
	//Needed for updating mask texture
	int width;
	int height;
	glfwGetFramebufferSize(window, &width, &height);

	GlSet glset;
	Texture txtr;
	Utilities util;

	//Filter
	char const* lFilterPatterns[3] = { "*.jpg", "*.png", "*.jpeg" };

	//Open the "open file dialog"
	auto maskTexturePathCheck = tinyfd_openFileDialog("Select Mask Texture", "", 3, lFilterPatterns, "", false);


	if (maskTexturePathCheck) { 
		maskTexturePath = maskTexturePathCheck;
		UIElements[UImaskTextureFileNameText].text.text = util.getLastWordBySeparatingWithChar(maskTexturePath,'/'); 
		brushValChanged = true;
		glset.activeTexture(GL_TEXTURE1);

		aTexture brushTxtr;
		brushTxtr.id = txtr.getTexture(maskTexturePath,false,brushTxtr.width,brushTxtr.height);
		brushTxtr.name = maskTexturePath;

		if(UIElements[UImaskPaintingCheckBoxElement].checkBox.checked)
			brushTxtr.folderIndex = 3;
		else if(UIElements[UIcolorPaintingCheckBoxElement].checkBox.checked)
			brushTxtr.folderIndex = 4;
		else if(UIElements[UInormalmapPaintingCheckBoxElement].checkBox.checked)
			brushTxtr.folderIndex = 5;

		albedoTextures.push_back(brushTxtr);

		txtr.updateMaskTexture(FBOScreen,width,height,UIElements[UIbrushRotationRangeBar].rangeBar.value,false,UIElements[UIbrushBordersRangeBar].rangeBar.value,brushBlurVal,outShaderData,programs,glfwGetVideoMode(glfwGetPrimaryMonitor())->width,glfwGetVideoMode(glfwGetPrimaryMonitor())->height);
	}
}
void LigidPainter::bakeButton(){
	bakeTheMaterial = true;
	nodeScenes[selectedNodeScene].stateChanged = true;
}
void LigidPainter::brushSizeRangeBar(double xOffset,int width){
	brushValChanged = true;
	Utilities util;
	UIElements[UIbrushSizeRangeBar].rangeBar.value -= xOffset / (width / 2);
	UIElements[UIbrushSizeRangeBar].rangeBar.value = util.restrictBetween(UIElements[UIbrushSizeRangeBar].rangeBar.value, 0.11f, -0.11f);//Keep in boundaries
	glUseProgram(programs.dynamicPaintingProgram);
	glset.uniform1f(programs.dynamicPaintingProgram,"radius",(UIElements[UIbrushSizeRangeBar].rangeBar.value+0.11f)*300);
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

	txtr.updateMaskTexture(FBOScreen,width,height, UIElements[UIbrushRotationRangeBar].rangeBar.value,renderTiny,UIElements[UIbrushBordersRangeBar].rangeBar.value,brushBlurVal,outShaderData,programs,glfwGetVideoMode(glfwGetPrimaryMonitor())->width,glfwGetVideoMode(glfwGetPrimaryMonitor())->height);
	
	glUseProgram(programs.dynamicPaintingProgram);
	glset.uniform1f(programs.dynamicPaintingProgram,"hardness",(UIElements[UIbrushBlurRangeBar].rangeBar.value-0.09f)*4.5454545*50);
}
void LigidPainter::textureDisplayerButton(double xOffset,double yOffset,int width,int height) {
	panelChanging = true;
	Utilities util;

	textureDisplayer.positionChanged = true;

	textureDisplayer.buttonPosX -= xOffset / (double)(screenWidth / 2.0f);
	textureDisplayer.buttonPosX = util.restrictBetween(textureDisplayer.buttonPosX,2.0f,0.0f);

	textureDisplayer.buttonPosY += yOffset / (double)(screenHeight / 2.0f);
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
	txtr.updateMaskTexture(FBOScreen, width, height,UIElements[UIbrushRotationRangeBar].rangeBar.value,true,UIElements[UIbrushBordersRangeBar].rangeBar.value,brushBlurVal,outShaderData,programs,glfwGetVideoMode(glfwGetPrimaryMonitor())->width,glfwGetVideoMode(glfwGetPrimaryMonitor())->height);
}
void LigidPainter::brushOpacityRangeBar(double xOffset, int width, int height) {
	Utilities util;
	Texture txtr;
	//brushOpacityChanged = true; not used
	UIElements[UIbrushOpacityRangeBar].rangeBar.value -= xOffset / (width / 2.0f);
	UIElements[UIbrushOpacityRangeBar].rangeBar.value = util.restrictBetween(UIElements[UIbrushOpacityRangeBar].rangeBar.value, 0.11f, -0.11f);//Keep in boundaries

	glUseProgram(programs.dynamicPaintingProgram);
	glset.uniform1f(programs.dynamicPaintingProgram,"opacity",(UIElements[UIbrushOpacityRangeBar].rangeBar.value + 0.11)*4.5454545);
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
	txtr.updateMaskTexture(FBOScreen, width, height,UIElements[UIbrushRotationRangeBar].rangeBar.value,true,UIElements[UIbrushBordersRangeBar].rangeBar.value,brushBlurVal,outShaderData,programs,glfwGetVideoMode(glfwGetPrimaryMonitor())->width,glfwGetVideoMode(glfwGetPrimaryMonitor())->height);
}
void LigidPainter::subSelectedImagePowerRangeBar(double xOffset, int width, int height) {
	Utilities util;
	Texture txtr;
	brushValChanged = true;
	UIElements[UIsubSelectedImagePowerRangeBarElement].rangeBar.value -= xOffset / (width / 2.0f);
	UIElements[UIsubSelectedImagePowerRangeBarElement].rangeBar.value = util.restrictBetween(UIElements[UIsubSelectedImagePowerRangeBarElement].rangeBar.value, 0.11f, -0.11f);//Keep in boundaries
	glUseProgram(programs.PBRProgram);
	glset.uniform1f(programs.PBRProgram,"subSelectedImagePower",(UIElements[UIsubSelectedImagePowerRangeBarElement].rangeBar.value+0.11f)*4.54545454545);
}
void LigidPainter::TDModelSizeRangeBar(float xOffset, int width,bool size,bool posx,bool posy,bool posz) {
	Utilities util;
	Texture txtr;
	if(size){
		UIElements[UITDModelSizeRangeBarElement].rangeBar.value -= xOffset / (width / 2.0f) * 10.f;
		UIElements[UITDModelSizeRangeBarElement].rangeBar.value = util.restrictBetween(UIElements[UITDModelSizeRangeBarElement].rangeBar.value, 10.f, 0.f);//Keep in boundaries
	}
	if(posx){
		UIElements[UITDModelPosXRangeBarElement].rangeBar.value -= xOffset / (width / 2.0f) * 10.f;
		UIElements[UITDModelPosXRangeBarElement].rangeBar.value = util.restrictBetween(UIElements[UITDModelPosXRangeBarElement].rangeBar.value, 10.f, -10.f);//Keep in boundaries
	}
	if(posy){
		UIElements[UITDModelPosYRangeBarElement].rangeBar.value -= xOffset / (width / 2.0f) * 10.f;
		UIElements[UITDModelPosYRangeBarElement].rangeBar.value = util.restrictBetween(UIElements[UITDModelPosYRangeBarElement].rangeBar.value, 10.f, -10.f);//Keep in boundaries
	}
	if(posz){
		UIElements[UITDModelPosZRangeBarElement].rangeBar.value -= xOffset / (width / 2.0f) * 10.f;
		UIElements[UITDModelPosZRangeBarElement].rangeBar.value = util.restrictBetween(UIElements[UITDModelPosZRangeBarElement].rangeBar.value, 10.f, -10.f);//Keep in boundaries
	}
	
	modelMatrix = glm::mat4(1);
	modelMatrix = glm::translate(modelMatrix,glm::vec3(UIElements[UITDModelPosXRangeBarElement].rangeBar.value ,UIElements[UITDModelPosYRangeBarElement].rangeBar.value ,UIElements[UITDModelPosZRangeBarElement].rangeBar.value ));
	modelMatrix = glm::scale(modelMatrix,glm::vec3(UIElements[UITDModelSizeRangeBarElement].rangeBar.value * 4.f));
}
void LigidPainter::selectBrushMaskTexture(){
	textureSelectionPanel.active = true;
	textureSelectionPanel.posX = 0.72f;
	textureSelectionPanel.posY = 0.6f;
	brushMaskTextureSelectionPanelActive = true;
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
		glset.texImage(NULL, txtrRes,txtrRes,GL_RGBA); //TODO : Use texture quality variable
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
		else if(UIElements[UInormalmapCheckBoxElement].checkBox.checked)
			glUseProgram(programs.normalGenProgram);
		else if(UIElements[UIgenerateBlackToAlphaCheckBoxElement].checkBox.checked)
			glUseProgram(programs.blackToAlphaProgram);
		else{
			//*Generate the text texture
			int txtrRes = 256;
			for (size_t i = 0; i < chosenTextureResIndex; i++)
			{
				txtrRes*=2;
			}
		
			glActiveTexture(GL_TEXTURE28);
				
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
				else if(UIElements[UInormalmapCheckBoxElement].checkBox.checked)
					glUseProgram(programs.normalGenProgram);
				else if(UIElements[UIgenerateBlackToAlphaCheckBoxElement].checkBox.checked)
					glUseProgram(programs.blackToAlphaProgram);
				else{
					UserInterface ui;
					glm::mat4 projection = glm::ortho(-1.0f, 0.5f, -1.f, 1.f);
					glUseProgram(programs.uiProgram);
					glset.uniformMatrix4fv(programs.uiProgram, "TextProjection", projection);
					
					glUseProgram(programs.uiProgram);
					float thic = (UIElements[UIgenerateTextSizeRangeBarElement].rangeBar.value+0.11f)/100.f;

					int s = ((UIElements[UIgenerateTextTextureTextTextBoxElement].textBox.text.size()/(((0.23f-(UIElements[UIgenerateTextSizeRangeBarElement].rangeBar.value+0.11f))*100)*2.f))+1);
					
					float aposY = 0.f;

					if(UIElements[UIgenerateTextLeftAlignCheckBoxElement].checkBox.checked){
						float aposX = -1.f;
						ui.renderText(programs.uiProgram,UIElements[UIgenerateTextTextureTextTextBoxElement].textBox.text,aposX,aposY,thic,glm::vec4(1),0.9f,false,txtrGenSelectedFont,0.45f);
					}
					if(UIElements[UIgenerateTextMidAlignCheckBoxElement].checkBox.checked){
						float aposX = -.5f;
						ui.renderTextM(programs.uiProgram,UIElements[UIgenerateTextTextureTextTextBoxElement].textBox.text,aposX,aposY,thic,glm::vec4(1),0.9f,false,txtrGenSelectedFont);
					}
					if(UIElements[UIgenerateTextRightAlignCheckBoxElement].checkBox.checked){
						float aposX = .5f;
						ui.renderTextR(programs.uiProgram,UIElements[UIgenerateTextTextureTextTextBoxElement].textBox.text,aposX,aposY,thic,glm::vec4(1),0.9f,false,txtrGenSelectedFont);
					}

						
					projection = glm::ortho(-1.0f, 1.f, -1.f, 1.f);
					glUseProgram(programs.uiProgram);
					glset.uniformMatrix4fv(programs.uiProgram, "TextProjection", projection);
				}
		}
			

		
		glm::mat4 renderTextureProjection = glm::ortho(0.0f, 1.0f, 0.0f, 1.0f);
		//TODO Fix Text generation
		
		if(!UIElements[UIgenerateTextCheckBoxElement].checkBox.checked){
			glset.uniformMatrix4fv(programs.noisyTextureProgram,"renderTextureProjection",renderTextureProjection);
			glset.uniformMatrix4fv(programs.normalGenProgram,"renderTextureProjection",renderTextureProjection);
			glset.uniformMatrix4fv(programs.blackToAlphaProgram,"renderTextureProjection",renderTextureProjection);
			glset.drawArrays(renderVertices,0);
		}
		glUseProgram(programs.uiProgram);

		glDeleteFramebuffers(1,&FBO);	
		glset.bindFramebuffer(0);

		aTexture txtr;
		txtr.width = txtrRes;
		txtr.height = txtrRes;
		txtr.id = textureColorbuffer;
		txtr.name = "generatedTexture"; 
		std::vector<std::string> textureNames;
		for (size_t i = 0; i < albedoTextures.size(); i++)
		{
			textureNames.push_back(albedoTextures[i].name);
		}
			
		//Rename if necessary
		txtr.name = util.uniqueName(txtr.name,textureNames);
		albedoTextures.push_back(txtr);
		
		setViewportToDefault();
}
void LigidPainter::colorBoxColorRangeBar(double yOffset,int height){
	Utilities util;
	colorPicker.huePointerChanging = true;
	colorPicker.updateHueVal = true;
	colorPicker.hueValue += yOffset / (height / 2.0f);
	colorPicker.hueValue = util.restrictBetween(colorPicker.hueValue, 0.180f, -0.180f);//Keep in boundaries
	colorPicker.saturationValueValChanged = true;
}
void LigidPainter::generateTextSizeRangeBar(float xOffset,int width){
	Utilities util;
	UIElements[UIgenerateTextSizeRangeBarElement].rangeBar.value -= xOffset / (width / 2.0f);
	UIElements[UIgenerateTextSizeRangeBarElement].rangeBar.value = util.restrictBetween(UIElements[UIgenerateTextSizeRangeBarElement].rangeBar.value, 0.11f, -0.11f);//Keep in boundaries
	
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
void LigidPainter::selectingPaintOverTextureIcon(){
	selectingPaintOverTexture = true;
	textureSelectionPanel.posX = 0.65f;
	textureSelectionPanel.posY = -0.2f;
	textureSelectionPanel.active = true;
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
	UIElements[UImirrorXCheckBox].checkBox.checked = !UIElements[UImirrorXCheckBox].checkBox.checked;

	Utilities util;
	
	mirrorParams.clear();
	std::vector<glm::vec3> positions = util.getMirrorVectors(UIElements[UImirrorXCheckBox].checkBox.checked,UIElements[UImirrorYCheckBox].checkBox.checked,UIElements[UImirrorZCheckBox].checkBox.checked);
	for (size_t i = 0; i < positions.size(); i++)
	{
		MirrorParam mirror;
		glGenTextures(1,&mirror.depthTexture);
		mirror.pos = positions[i];
		mirrorParams.push_back(mirror);
	}

}
void LigidPainter::mirrorYCheckBox() {
	UIElements[UImirrorYCheckBox].checkBox.checked = !UIElements[UImirrorYCheckBox].checkBox.checked;

	Utilities util;
	mirrorParams.clear();
	std::vector<glm::vec3> positions = util.getMirrorVectors(UIElements[UImirrorXCheckBox].checkBox.checked,UIElements[UImirrorYCheckBox].checkBox.checked,UIElements[UImirrorZCheckBox].checkBox.checked);
	for (size_t i = 0; i < positions.size(); i++)
	{
		MirrorParam mirror;
		glGenTextures(1,&mirror.depthTexture);
		mirror.pos = positions[i];
		mirrorParams.push_back(mirror);
	}
	
}
void LigidPainter::mirrorZCheckBox() {
	UIElements[UImirrorZCheckBox].checkBox.checked = !UIElements[UImirrorZCheckBox].checkBox.checked;

	Utilities util;
	mirrorParams.clear();
	std::vector<glm::vec3> positions = util.getMirrorVectors(UIElements[UImirrorXCheckBox].checkBox.checked,UIElements[UImirrorYCheckBox].checkBox.checked,UIElements[UImirrorZCheckBox].checkBox.checked);
	for (size_t i = 0; i < positions.size(); i++)
	{
		MirrorParam mirror;
		glGenTextures(1,&mirror.depthTexture);
		mirror.pos = positions[i];
		mirrorParams.push_back(mirror);
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
		ui.alert("Warning! There Are No Textures Selected.",100,false);
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
void LigidPainter::useUVCheckBox(){
	UIElements[UIuseUVCheckBox].checkBox.checked = !UIElements[UIuseUVCheckBox].checkBox.checked;
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
			model.loadModel(modelFilePath,true);
		else if(customModelFilePath != "")
			model.loadModel(customModelFilePath,true);

		newModelAdded = true;

		model.meshes[0].sendObjectsToMesh(objects.VAO,objects.VAO);

		// for (size_t i = 0; i < model.meshes.size(); i++) 
		// {
		// }
		glUseProgram(programs.uiProgram);
		glBindBuffer(GL_ARRAY_BUFFER, objects.VBO);
		glBindVertexArray(objects.VAO);
	}
	else{
		//If model file path is inappropriate add one
		modelFilePathTextBox();
		if(modelFilePath){
			loadModelButton();
		}
	}
}
void LigidPainter::paintoverTransformRanegBars(float xOffset, int screenWidth,int screenHeight,bool pX,bool pY,bool sX, bool sY){
	if (pX) {
		Utilities util;
		UIElements[UIpaintOverPosXRangeBarElement].rangeBar.value -= xOffset / (screenWidth / 2) *1.7;
		//UIElements[UImirrorXRangeBarElement].rangeBar.value = util.restrictBetween(UIElements[UImirrorXRangeBarElement].rangeBar.value, 0.11f, -0.11f);//Keep in boundaries
    }
	if (pY) {
		Utilities util;
		UIElements[UIpaintOverPosYRangeBarElement].rangeBar.value -= xOffset / (screenWidth / 2) *1.7;
		//UIElements[UImirrorXRangeBarElement].rangeBar.value = util.restrictBetween(UIElements[UImirrorXRangeBarElement].rangeBar.value, 0.11f, -0.11f);//Keep in boundaries
    }
	if (sX) {
		Utilities util;
		UIElements[UIpaintOverScaleXRangeBarElement].rangeBar.value -= xOffset / (screenWidth / 2) *1.7;
		//UIElements[UImirrorXRangeBarElement].rangeBar.value = util.restrictBetween(UIElements[UImirrorXRangeBarElement].rangeBar.value, 0.11f, -0.11f);//Keep in boundaries
    }
	if (sY) {
		Utilities util;
		UIElements[UIpaintOverScaleYRangeBarElement].rangeBar.value -= xOffset / (screenWidth / 2) *1.7;
		//UIElements[UImirrorXRangeBarElement].rangeBar.value = util.restrictBetween(UIElements[UImirrorXRangeBarElement].rangeBar.value, 0.11f, -0.11f);//Keep in boundaries
    }
}
void LigidPainter::paintingDropper(){
	colorPicker.dropperActive = true;
}
void LigidPainter::maskPanelSlider(double yOffset,int screenSizeY){
	Utilities util;

	if(UIElements[UImaskPaintingCheckBoxElement].checkBox.checked){
		brushMaskTextures.maskTexturesSliderValue += yOffset / (screenSizeY / 2);
		brushMaskTextures.maskTexturesSliderValue = util.restrictBetween(brushMaskTextures.maskTexturesSliderValue, 0.0f, -0.25f);//Keep in boundaries
	}
	if(UIElements[UIcolorPaintingCheckBoxElement].checkBox.checked){
		brushMaskTextures.colorTexturesSliderValue += yOffset / (screenSizeY / 2);
		brushMaskTextures.colorTexturesSliderValue = util.restrictBetween(brushMaskTextures.colorTexturesSliderValue, 0.0f, -0.25f);//Keep in boundaries
	}
	if(UIElements[UInormalmapPaintingCheckBoxElement].checkBox.checked){
		brushMaskTextures.normalTexturesSliderValue += yOffset / (screenSizeY / 2);
		brushMaskTextures.normalTexturesSliderValue = util.restrictBetween(brushMaskTextures.normalTexturesSliderValue, 0.0f, -0.25f);//Keep in boundaries
	}
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
void LigidPainter::outSubmeshesButton(){
	if(model.meshes[currentMaterialIndex].submeshes.size() > 1){
		int txtrRes = 256;
		for (size_t i = 0; i < chosenTextureResIndex; i++)
		{
			txtrRes*=2;
		}

		std::vector<std::vector<unsigned int>> pipeline;

		for (size_t smi = 0; smi < model.meshes[currentMaterialIndex].submeshes.size(); smi++)
		{
			Node outnode;
			//Determine the node that is connected to the shader out node
			for (size_t nodi = 0; nodi < nodeScenes[model.meshes[currentMaterialIndex].submeshes[smi].materialIndex].nodes.size(); nodi++)
			{
				if(nodeScenes[model.meshes[currentMaterialIndex].submeshes[smi].materialIndex].nodes[nodi].outputs.size()){
					if(nodeScenes[model.meshes[currentMaterialIndex].submeshes[smi].materialIndex].nodes[nodi].outputs[0].isConnectedToShaderInput){
						outnode = nodeScenes[model.meshes[currentMaterialIndex].submeshes[smi].materialIndex].nodes[nodi];
						if(outnode.title != "01 PBR"){
							UserInterface ui;
							ui.alert("WARNING! A node that is other than pbr node is connected to the shader out at the material " + nodeScenes[model.meshes[currentMaterialIndex].submeshes[smi].materialIndex].sceneName,200,false);
						}
					}
				}
			}
			if(!outnode.outputs.size()){
				UserInterface ui;
				ui.alert("WARNING! There are no node connected to the shader output at the material " + nodeScenes[model.meshes[currentMaterialIndex].submeshes[smi].materialIndex].sceneName,200,false);
			}

			std::vector<unsigned int> part;
			for (size_t oniI = 0; oniI < outnode.inputs.size(); oniI++)
			{
				unsigned int texture;
				texture = outnode.inputs[oniI].generatedTexture;
				part.push_back(texture);
			}
			pipeline.push_back(part);
		}

		for (size_t pipI = 0; pipI < pipeline[0].size(); pipI++)
		{
			glUseProgram(programs.materialResult);
	
    	    for (size_t sI = 0; sI < pipeline.size(); sI++)//20 21 22 23 24 25 
    	    {
    	        glActiveTexture(GL_TEXTURE20+sI);
    	        glBindTexture(GL_TEXTURE_2D,pipeline[sI][pipI]);
    	        glUniform1i(glGetUniformLocation(programs.materialResult, ("material" + std::to_string(sI)).c_str()), 20+sI);
    	    }
    	    for (size_t sI = 0; sI < pipeline.size(); sI++)//26 27 28 29 30 31
    	    {
    	        glActiveTexture(GL_TEXTURE26+sI);
    	        glBindTexture(GL_TEXTURE_2D,model.meshes[currentMaterialIndex].submeshes[sI].maskTexture);
    	        glUniform1i(glGetUniformLocation(programs.materialResult, ("mask" + std::to_string(sI)).c_str()), 26+sI);
    	    }
    	    glUniform1i(glGetUniformLocation(programs.materialResult, "submeshCount"), pipeline.size());

			glActiveTexture(GL_TEXTURE28);

			unsigned int FBO;
			glset.genFramebuffers(FBO);
			glset.bindFramebuffer(FBO);

			unsigned int textureColorbuffer;
			glset.genTextures(textureColorbuffer);
			glset.bindTexture(textureColorbuffer);
			glset.texImage(NULL, txtrRes,txtrRes,GL_RGB); //TODO : Use texture quality variable
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			glset.generateMipmap();

			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorbuffer, 0);

			glm::mat4 textProjection = glm::ortho(0.0f, 1.0f, 0.0f, 1.0f);
			glset.uniform1i(programs.materialResult,"render2D",1);
			glset.uniformMatrix4fv(programs.materialResult,"ortprojection",textProjection);

			glset.viewport(txtrRes,txtrRes);

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

			glset.drawArrays(renderVertices,0);

			aTexture txtr;
			txtr.width = txtrRes;
			txtr.height = txtrRes;
			txtr.id = textureColorbuffer;
			if(pipI == 0)txtr.name = "albedo";if(pipI == 1)txtr.name = "roughness";if(pipI == 2)txtr.name = "metallic";if(pipI == 3)txtr.name = "normal_map";if(pipI == 4)txtr.name = "ambient_occlusion";if(pipI == 5)txtr.name = "emission";if(pipI == 6)txtr.name = "emission_strength";

			std::vector<std::string> albedoTxtrStr;
			for (size_t i = 0; i < albedoTextures.size(); i++)
			{
				albedoTxtrStr.push_back(albedoTextures[i].name);
			}
			Utilities util;
			txtr.name = util.uniqueName(txtr.name,albedoTxtrStr);

			albedoTextures.push_back(txtr);
	
			glset.bindFramebuffer(0);
			glset.deleteFramebuffers(FBO);
			glset.uniform1i(programs.materialResult,"render2D",0);
			setViewportToDefault();
		}	
	}
	else{
		UserInterface ui;
		ui.alert("You have to have at least 2 submeshes for the selected 3D model's material",200,false);
	}
}
void LigidPainter::mirrorRangeBars(double xOffset, int width, int height,bool x,bool y,bool z){
	mirrorRangeBarsPressed = true;
	if (x) {
		Utilities util;
		UIElements[UImirrorXRangeBarElement].rangeBar.value -= xOffset / (width / 2) *1.7;
		//UIElements[UImirrorXRangeBarElement].rangeBar.value = util.restrictBetween(UIElements[UImirrorXRangeBarElement].rangeBar.value, 0.11f, -0.11f);//Keep in boundaries
    }
	if (y) {
		Utilities util;
		UIElements[UImirrorYRangeBarElement].rangeBar.value -= xOffset / (width / 2) *1.7;
		//UIElements[UImirrorYRangeBarElement].rangeBar.value = util.restrictBetween(UIElements[UImirrorYRangeBarElement].rangeBar.value, 0.11f, -0.11f);//Keep in boundaries
    }
	if (z) {
		Utilities util;
		UIElements[UImirrorZRangeBarElement].rangeBar.value -= xOffset / (width / 2) *1.7;
		//UIElements[UImirrorZRangeBarElement].rangeBar.value = util.restrictBetween(UIElements[UImirrorZRangeBarElement].rangeBar.value, 0.11f, -0.11f);//Keep in boundaries
    }
}
void LigidPainter::nodePanelBoundaries(float yOffset,float screenHeight){
	if (enablePanelMovement) {
		Utilities util;
		nodePanel.heigth += yOffset / (screenHeight / 2);
		nodePanel.heigth = util.restrictBetween(nodePanel.heigth, 1.85f, 0.01);//Keep in boundaries
    }
}

void LigidPainter::sndPanelMinusIcon(){
	UserInterface ui;

	if(!txtrCreatingPanel.active){
		if(sndPanel.state == 0){
			if(!albedoTextures[selectedAlbedoTextureIndex].isTrashFolder){
				if(selectedAlbedoTextureIndex == 1 || selectedAlbedoTextureIndex == 2){
					ui.alert("This folder can't be deleted",200,false);
					return;
				}
				//Textures
				if(albedoTextures.size()){

					if(albedoTextures[selectedAlbedoTextureIndex].isTexture){
						if(albedoTextures[selectedAlbedoTextureIndex].folderIndex == 0){
							Texture txtr;
							txtr.deleteOpenglTexture(albedoTextures[selectedAlbedoTextureIndex]);
							albedoTextures.erase(albedoTextures.begin() + selectedAlbedoTextureIndex);

						}
						else
							albedoTextures[selectedAlbedoTextureIndex].folderIndex = 0;
					}
					else{
						for (size_t i = 0; i < albedoTextures.size(); i++)
						{
							Texture txtr;
							if(albedoTextures[i].isTexture)
								txtr.deleteOpenglTexture(albedoTextures[i]);

							//Delete the elements of the folder
							if(albedoTextures[i].folderIndex == selectedAlbedoTextureIndex){
								albedoTextures.erase(albedoTextures.begin() + i);
								i--;
							}
						}
						albedoTextures.erase(albedoTextures.begin() + selectedAlbedoTextureIndex);
					}

					if(selectedAlbedoTextureIndex)
						selectedAlbedoTextureIndex--;
				}
				else{
					ui.alert("Warning! Deleting request is ignored. There are no textures to delete.",200,false);
				}
			}
			else{
				ui.alert("Warning! Trash folder can't be deleted.",200,false);
			}
		}
		else if(sndPanel.state == 1){
			nodeScenesHistory.clear();

			//Materials
			bool deletable = true;
			if(nodeScenes.size() != 1){
				if(ligidMessageBox("Selected material will be deleted. Do you want to proceed?",-0.2375f,"",0))
				{
					for (size_t i = 0; i < model.meshes.size(); i++)
					{
						if(model.meshes[i].materialIndex == selectedNodeScene){
							ui.alert("Warning! Deleting request is ignored. This material is already in use.",200,false);
							deletable = false;
							break;
						}
					}
					if(deletable){
						//TODO : Update history array indexes
						for (size_t i = 0; i < nodeScenesHistory.size(); i++)
						{	
							if(nodeScenesHistory[i].arrayIndex == selectedNodeScene)
								nodeScenesHistory.erase(nodeScenesHistory.begin()+i);
						}
						for (size_t i = 0; i < nodeScenesHistory.size(); i++)
						{
							if(selectedNodeScene < nodeScenesHistory[i].arrayIndex){
								nodeScenesHistory[i].arrayIndex--;
							}
						}

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
				
			}
			else{
				ui.alert("Warning! Deleting request is ignored. Last material can't be deleted.",200,false);
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

			nodeScenesHistory.clear();

			ColorData colorData;

			std::vector<Node> mainOutNodes;
			Load load;
			mainOutNodes = load.createOutputNode(appNodes);		

			NodeScene emptyNodeScene;
			emptyNodeScene.index = 0;
			emptyNodeScene.arrayIndex = nodeScenes.size();

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
			
			glActiveTexture(GL_TEXTURE28);
			unsigned int renderTexture;
			glset.genTextures(renderTexture);
			glset.bindTexture(renderTexture);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glset.texImage(nullptr,100,100,GL_RGBA);
			glset.generateMipmap();

			emptyNodeScene.renderedTexture = renderTexture;
			
			unsigned int outTexture;
			glset.genTextures(outTexture);
			glset.bindTexture(outTexture);
			glset.texImage(nullptr,1024,1024,GL_RGBA);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glset.generateMipmap();

			emptyNodeScene.outTexture = outTexture;
			
			nodeScenes.push_back(emptyNodeScene);

			MaterialOut mOut;
			mOut.program = 0;
			modelMaterials.push_back(mOut);
		}
	}
}
void LigidPainter::sndPanelDuplicateIcon(){
	if(!txtrCreatingPanel.active){
		if(sndPanel.state == 0){
			if(albedoTextures.size()){
				Texture txtr;
				int txtrRes = 256;
				for (size_t i = 0; i < chosenTextureResIndex; i++)
				{
					txtrRes*=2;
				}
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D,albedoTextures[selectedAlbedoTextureIndex].id);

				aTexture atxtr = albedoTextures[selectedAlbedoTextureIndex];
				GLubyte* txtrData = txtr.getTextureFromProgram(GL_TEXTURE0,atxtr.width,atxtr.height,4);

				glActiveTexture(GL_TEXTURE0);
				unsigned int texture;
				glGenTextures(1,&texture);
				glBindTexture(GL_TEXTURE_2D,texture);
				glset.texImage(txtrData,atxtr.width,atxtr.height,GL_RGBA);
				glset.generateMipmap();


				Utilities util;

				std::vector<std::string> albedoTxtrStr;
				for (size_t i = 0; i < albedoTextures.size(); i++)
				{
					albedoTxtrStr.push_back(albedoTextures[i].name);
				}


				atxtr.name = util.uniqueName(atxtr.name,albedoTxtrStr);
				atxtr.id = texture;

				albedoTextures.push_back(atxtr);

				delete[]txtrData;
			}
			else{
				UserInterface ui;
				ui.alert("Warning! Duplication request is ignored. There are no texture to duplicate.",200,false);
			}
		}
		else if(sndPanel.state == 1){
			//Materials
			nodeScenesHistory.clear();
			ColorData colorData;

			std::vector<Node> mainOutNodes;
			Load load;
			mainOutNodes = load.createOutputNode(appNodes);		

			NodeScene emptyNodeScene;
			emptyNodeScene = nodeScenes[selectedNodeScene];
			emptyNodeScene.index = 0;
			emptyNodeScene.arrayIndex = nodeScenes.size();

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
			
			glActiveTexture(GL_TEXTURE28);
			unsigned int renderTexture;
			glset.genTextures(renderTexture);
			glset.bindTexture(renderTexture);
			glset.texImage(nullptr,100,100,GL_RGBA);
			glset.generateMipmap();

			emptyNodeScene.renderedTexture = renderTexture;

			unsigned int outTexture;
			glset.genTextures(outTexture);
			glset.bindTexture(outTexture);
			glset.texImage(nullptr,1024,1024,GL_RGBA);
			glset.generateMipmap();

			emptyNodeScene.outTexture = outTexture;

			nodeScenes.push_back(emptyNodeScene);

			MaterialOut mOut;
			mOut.program = 0;
			modelMaterials.push_back(mOut);
		}
	}
	glActiveTexture(GL_TEXTURE28);
}
void LigidPainter::sndPanelFolderIcon(){
	Utilities util;
	if(sndPanel.state == 0){
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
	else{
		MaterialFile materialFile;
		char const* lFilterPatterns[1] = { "*.material" };
		char* materialFilePath = tinyfd_openFileDialog("Import a Material","", 1, lFilterPatterns, "",0);
		NodeScene material;
		
		if(materialFilePath){
			material = materialFile.readTheFile(materialFilePath);

			material.index = 0;
			material.arrayIndex = nodeScenes.size();
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
					material.index = i;
					break;
				}
			}
			material.sceneName = "material_" + std::to_string(material.index); 

			glActiveTexture(GL_TEXTURE28);
			unsigned int renderTexture;
			glset.genTextures(renderTexture);
			glset.bindTexture(renderTexture);
			glset.texImage(nullptr,100,100,GL_RGBA);
			glset.generateMipmap();

			material.renderedTexture = renderTexture;

			unsigned int outTexture;
			glset.genTextures(outTexture);
			glset.bindTexture(outTexture);
			glset.texImage(nullptr,1024,1024,GL_RGBA);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glset.generateMipmap();

			material.outTexture = outTexture;

			material.stateChanged = true;
			for (size_t mi = 0; mi < material.nodes.size(); mi++)
			{
				material.nodes[mi].stateChanged = true;
			}
			
			selectedNodeScene = nodeScenes.size();

			nodeScenes.push_back(material);
			MaterialOut mOut;
			mOut.program = 0;
			modelMaterials.push_back(mOut);
		}

		nodeScenesHistory.clear();

	}
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
		glActiveTexture(GL_TEXTURE28);
}
void LigidPainter::sndPanelDownIcon(){
	if(!txtrCreatingPanel.active && sndPanel.state == 0){

		//Load texture
		Texture txtr;
		//Filters
		char const* lFilterPatterns[2] = { "*.jpg", "*.png" };
		//File dialog
		auto albedoPathCheck = tinyfd_openFileDialog("Select Image", "", 2, lFilterPatterns, "", true);
		if (albedoPathCheck) {
			std::string albedoTexturePath = albedoPathCheck;
			Utilities util;
			std::vector<std::string> albedoTexturePaths = util.seperateFilePaths(albedoTexturePath,'|');
			
			int txtrRes = 256;
			for (size_t i = 0; i < chosenTextureResIndex; i++)
			{
				txtrRes*=2;
			}
			for (size_t i = 0; i < albedoTexturePaths.size(); i++)
			{
				aTexture result;
				
				glActiveTexture(GL_TEXTURE0);

				
				result.id = txtr.getTexture(albedoTexturePaths[i],false,result.width,result.height); //Force albedo's ratio to be 1:1
				
				Utilities util;
				 
				std::vector<std::string> textureNames;
				for (size_t i = 0; i < albedoTextures.size(); i++)
				{
					textureNames.push_back(albedoTextures[i].name);
				}

				//Rename if necessary
				result.name = util.removeExtension(util.getLastWordBySeparatingWithChar(albedoTexturePaths[i],folderDistinguisher)); 
				result.name = util.uniqueName(result.name,textureNames);

				albedoTextures.push_back(result);
				glActiveTexture(GL_TEXTURE28);
			}
		}
	}
	else if(sndPanel.state == 1){
		MaterialFile materialFile;
		char const* lFilterPatterns[1] = { "*.material" };
		char* materialFilePath = tinyfd_saveFileDialog("Save The Material","", 1, lFilterPatterns, "");
		if(materialFilePath)
			materialFile.writeTheFile(materialFilePath,nodeScenes[selectedNodeScene]);
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