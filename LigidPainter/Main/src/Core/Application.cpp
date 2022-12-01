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
#include <dirent.h>
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

string colorpickerHexVal = "";

string modelName;
vector<float> vertices = { 0 };

//--------Functions--------\\

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void scroll_callback(GLFWwindow* window, double scroll, double scrollx);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void updateCameraPosChanging();
RenderData updateRenderData(RenderData renderData, unsigned int depthTexture, int brushSizeIndicatorSize);
UiData updateUiData();
void updateColorPicker(glm::vec3 RGBval,bool changeHue);
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
float brushSizeRangeBarValue = 0.0f;
float brushBlurRangeBarValue = -0.11f;
float brushRotationRangeBarValue = -0.11f;
float brushOpacityRangeBarValue = 0.11f;
float brushSpacingRangeBarValue = -0.11f;
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
bool textureDemonstratorButtonPressClicked = false;
bool textureDemonstratorButtonPosChanged = false;
float textureDemonstratorWidth = 0.4f;
float textureDemonstratorHeight = 0.8f;
bool textureDemonstratorBoundariesHover = false;
//bool textureDemonstratorBoundariesHoverR = false;
//bool textureDemonstratorBoundariesHoverB = false;

bool useNegativeForDrawing;

bool panelChanging = false; //Disable painting while changing panel sizes

bool brushValChanged = true;

int paintingFillNumericModifierVal = 5;

bool caps = false; //GLFW_MOD_CAPS_LOCK

char pressedChar;
bool pressedCharChanged;
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {

	if(key >= 320 && key <=329){
		key -= 272;
	}
	if(key == 280 && action == 0){
		if(caps)
			caps = false;
		else
			caps = true;
	}
	pressedChar = key;
	pressedCharChanged = true;
}

bool colorBoxClicked = false;

vector<unsigned int> maskTextures;
vector<string> maskTextureNames;
unsigned int loadCubemap(std::vector<std::string> faces)
{
	glset.activeTexture(GL_TEXTURE13);
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 
                         0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
            );
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Cubemap tex failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}
Programs programs;
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
	uiActData.textureDemonstratorBoundariesPressed = false;
	uiActData.textureDemonstratorButtonPressed = false;

	//Set Callbacks
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback); 
	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	//Set Callbacks

	programs = glset.getProgram();

	glGenBuffers(1, &VBO);
	//glGenVertexArrays(1, &VAO);
	//glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glUseProgram(programs.program);

	ui.sendProgramsToUserInterface(programs);
	render.sendProgramsToRender(programs);
	txtr.sendProgramsToTextures(programs);

	renderData.window = window;

	glset.setVertexAtribPointer();
	glBufferData(GL_ARRAY_BUFFER, 10000, NULL, GL_DYNAMIC_DRAW); 

	ui.setViewportBgColor();
	ui.uploadChars();

	//Load brush mask textures

	struct dirent *d;
    DIR *dr;
    dr = opendir("./LigidPainter/Resources/Textures");
    if(dr!=NULL)
    {
        cout<<"List of Files & Folders:-\n";
        for(d=readdir(dr); d!=NULL; d=readdir(dr))
        {
			glset.activeTexture(GL_TEXTURE1);//Raw mask
			string fileName =d->d_name;
			if(fileName.size() > 3){
				if(fileName[fileName.size()-1] != 't' && fileName[fileName.size()-2] != 'x' && fileName[fileName.size()-3] != 't'){
					maskTextures.push_back(txtr.getTexture("./LigidPainter/Resources/Textures/" + fileName,0,0,false));
					maskTextureNames.push_back(fileName);
				}
			}		
        }
        closedir(dr);
    }
    else
        cout<<"\nError Occurred!";
    cout<<endl;


	//Load cubemap
	vector<std::string> faces
	{
	    "LigidPainter/Resources/Cubemap/Skybox/px.png",
	    "LigidPainter/Resources/Cubemap/Skybox/nx.png",
	    "LigidPainter/Resources/Cubemap/Skybox/ny.png",
	    "LigidPainter/Resources/Cubemap/Skybox/py.png",
	    "LigidPainter/Resources/Cubemap/Skybox/pz.png",
	    "LigidPainter/Resources/Cubemap/Skybox/nz.png"
	};
	unsigned int cubemapTexture = loadCubemap(faces);  


	glUseProgram(programs.skyboxProgram);
	glset.uniform1i(6, "skybox", 13);
	glUseProgram(programs.program);
	glset.uniform1i(3, "skybox", 13);


	glm::vec3 lightPos(1.2f, 1.0f, 2.0f);
	glset.uniform1i(programs.program, "uvMask", 7);
	
	glUseProgram(programs.iconsProgram);
	glset.uniform1i(12, "icon", 6);
	glUseProgram(programs.program);

	glset.uniform3fv(programs.program, "lightPos", lightPos);
	glset.uniform1f(programs.program, "material.shininess", 32.0f);
	glset.uniform1i(programs.program, "screenMaskTexture", 4);
	glset.uniform1i(programs.program, "mirroredScreenMaskTexture", 3);
	glset.uniform3fv(programs.program,"textColor",colorData.textColor);
	glset.uniform1i(programs.program, "material.diffuse", 0);
	glset.uniform1i(programs.program, "material.specular", 1);

	glUseProgram(programs.blurProgram);
	glset.uniform1f(9, "brushBlurVal", 1000);
	glUseProgram(programs.program);

	glset.uniform1i(programs.program, "modifiedMaskTexture", 12);

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

	render.setMatrices(); //View matrix, Projection matrix

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
	txtr.updateMaskTexture(FBOScreen, screenWidth, screenHeight, brushRotationRangeBarValue,false);

	panelData.modelPanelActive = true; //Active panel by default

	Utilities util;

	int charPressingSensivity = 30;
	int charPressCounter = 0;

	while (true)
	{
		glfwPollEvents();

		//util.printRenderingSpeed();

		updateCameraPosChanging();

		//Check if texture demonstrator button clicked
		if(uiActData.textureDemonstratorButtonPressed){
			textureDemonstratorButtonPressCounter++;
		}
		if(textureDemonstratorButtonPressCounter < 20 && uiActData.textureDemonstratorButtonPressed && glfwGetMouseButton(window, 0) == GLFW_RELEASE && !textureDemonstratorButtonPosChanged){
			textureDemonstratorButtonPressClicked = true;
		}
		if(glfwGetMouseButton(window, 0) == GLFW_RELEASE){
			textureDemonstratorButtonPressCounter = 0;
		}
		textureDemonstratorButtonPosChanged = false;


		//Release textboxes
		if ((glfwGetMouseButton(window, 0) == GLFW_PRESS || glfwGetMouseButton(window, 1) == GLFW_PRESS)){
			if(!callbackData.exportFileNameTextBoxEnter)
				exportFileNameTextBoxPressed = false;
			if(!callbackData.hexValueTextboxEnter)
				hexValTextboxPressed = false;
		}



		uiActData = uiAct.uiActions(window,callbackData,textureDemonstratorBoundariesHover);
		
		


		if((paintingDropperPressed && glfwGetMouseButton(window, 0) == GLFW_PRESS) || (colorBoxClicked && !callbackData.colorBoxPickerEnter)){
			updateColorPicker(renderOut.mouseHoverPixel,true);
		}
		colorBoxClicked = false;
		colorBoxPickerButtonPressed = false;


		//Update
		render.updateViewMatrix(callbackData.cameraPos, callbackData.originPos,mirrorXCheckBoxChecked,mirrorYCheckBoxChecked,mirrorZCheckBoxChecked);
		brushSize = double(brushSizeRangeBarValue + 0.1f) * 800.0 + 20.0 ;
		renderData = updateRenderData(renderData,textures.depthTexture, brushSize);
		uidata = updateUiData();
		panelData.movePanel = callbackData.movePanel;
		exportData.exportImage = exportImage;
		exportData.path = exportPath.c_str();
		exportData.fileName = exportFileName.c_str();

		//Painting
		if (glfwGetMouseButton(window, 0) == GLFW_PRESS && doPainting && glfwGetMouseButton(window, 1) == GLFW_RELEASE) {//Used for spacing
			drawingCount++;
		}
		if (glfwGetMouseButton(window, 0) == GLFW_PRESS && doPainting && drawingCount == drawingSpacing && !panelChanging && !callbackData.panelChangeLoc && glfwGetMouseButton(window, 1) == GLFW_RELEASE){
			textureGen.drawToScreen(window, maskTexturePath, screenPaintingReturnData.normalId, brushSize, FBOScreen,brushRotationRangeBarValue,brushOpacityRangeBarValue,lastMouseXpos, lastMouseYpos,mouseXpos,mouseYpos,mirrorUsed,useNegativeForDrawing,brushValChanged,paintingFillNumericModifierVal,programs);
			brushValChanged = false;
			drawingCount = 0;
			//brushOpacityChanged = false; not used
			paintingMode = true;
		}
		if(drawingCount > drawingSpacing)
			drawingCount = 0;
		panelChanging = false;
		lastMouseXpos = mouseXpos;
		lastMouseYpos = mouseYpos;


		//Text input
		charPressCounter++;
		if(glfwGetKey(window,pressedChar) && (charPressCounter % charPressingSensivity == 0 || pressedCharChanged)){
			if(exportFileNameTextBoxPressed){
				if(pressedChar == 32){
					exportFileName += ' ';
				}
				else if(isalpha((char)pressedChar)){
					if(!caps){
						exportFileName+=(char)pressedChar+32;//lowercase
					}
					else{
						exportFileName+=(char)pressedChar;//UPPERCASE
					}
				}
				else if(isdigit((char)pressedChar)){
					exportFileName+=(char)(pressedChar);//lowercase
				}
			}
			if(hexValTextboxPressed){
				if(pressedChar == 32){
					colorpickerHexVal += ' ';
				}
				else if(isalpha((char)pressedChar)){
					if(!caps){
						colorpickerHexVal+=(char)pressedChar+32;//lowercase
					}
					else{
						colorpickerHexVal+=(char)pressedChar;//UPPERCASE
					}
				}
				else if(isdigit((char)pressedChar)){
					colorpickerHexVal+=(char)(pressedChar);//lowercase
				}
			}
			charPressCounter = 0;
			pressedCharChanged = false;
		}
		if(glfwGetKey(window,GLFW_KEY_BACKSPACE) && (charPressCounter % charPressingSensivity == 0|| pressedCharChanged)){
			if(exportFileNameTextBoxPressed && exportFileName != ""){
				exportFileName.pop_back();
			}
			if(hexValTextboxPressed && colorpickerHexVal != ""){
				colorpickerHexVal.pop_back();
			}
			pressedCharChanged = false;
		}

		
		//Render
		renderOut = render.render(renderData, vertices, FBOScreen, panelData,exportData,uidata,textureDemonstratorButtonPosX,textureDemonstratorButtonPosY,textureDemonstratorButtonPressClicked,textureDemonstratorWidth,textureDemonstratorHeight,uiActData.textureDemonstratorBoundariesPressed,icons,maskTextureFile.c_str(),paintingFillNumericModifierVal,maskPanelSliderValue,maskTextures,colorpickerHexVal);


		//Update brush mask texture file's name once the brush mask texture changed
		if(renderOut.maskPanelMaskClicked){
			for (size_t i = 0; i < maskTextures.size(); i++)
			{
				if(maskTextures[i] == renderOut.currentBrushMaskTxtr){
					maskTextureFile = maskTextureNames[i];
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
			callbackData = callback.mouse_callback(window, mouseXpos, mouseYpos, panelData, brushSizeRangeBarValue, colorBoxPickerValue_x, colorBoxPickerValue_y, colorBoxColorRangeBarValue, brushBlurRangeBarValue, enablePanelMovement,brushRotationRangeBarValue, brushOpacityRangeBarValue, brushSpacingRangeBarValue,textureDemonstratorButtonPosX,textureDemonstratorButtonPosY,maskPanelSliderValue,renderOut.maskPanelMaskHover);
		}
		if (cameraPosChanging) { //Change the position of the camera in the shaders once camera position changed
			glset.uniform3fv(programs.program, "viewPos", callbackData.cameraPos);
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
	xOffset = (lastXpos - xpos) / (1925 / width);
	lastXpos = xpos;
	yOffset = (lastYpos - ypos) / (1085 / height);
	lastYpos = ypos;
	//Get mouse position change
	
	//Texture demonstrator
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
	//Texture demonstrator
	bool hideCursor = uiAct.updateRangeValues(window,xOffset,yOffset,width,height); 
	if (hideCursor) { //Set cursor as hidden and restrict panel movement if any of the rangebars value is changing
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
	Utilities util;
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

void updateColorPicker(glm::vec3 RGBval,bool changeHue){
	int width;
	int height;
	glfwGetWindowSize(window,&width,&height);
	
	Utilities util;
	glm::vec3 hsvVal = util.RGBToHSVGenerator(RGBval);
	if(changeHue){
		if(hsvVal.r > 247.0){
			colorBoxColorRangeBarValue = 0.195f;	
		}
		else if(hsvVal.r < 247.0 && hsvVal.r > 242.0){
			colorBoxColorRangeBarValue = 0.185f;	
		}
		else if(hsvVal.r < 242.0 && hsvVal.r > 237.0){
			colorBoxColorRangeBarValue = 0.175f;	
		}
		else if(hsvVal.r < 237.0 && hsvVal.r > 232.0){
			colorBoxColorRangeBarValue = 0.165f;	
		}
		else if(hsvVal.r < 232.0 && hsvVal.r > 228.0){
			colorBoxColorRangeBarValue = 0.155f;	
		}
		else if(hsvVal.r < 228.0 && hsvVal.r > 224.0){
			colorBoxColorRangeBarValue = 0.145f;	
		}
		else if(hsvVal.r < 224.0 && hsvVal.r > 218.0){
			colorBoxColorRangeBarValue = 0.134f;	
		}
		else if(hsvVal.r < 218.0 && hsvVal.r > 210.0){
			colorBoxColorRangeBarValue = 0.125f;	
		}
		else if(hsvVal.r < 210.0 && hsvVal.r > 205.0){
			colorBoxColorRangeBarValue = 0.115f;	
		}
		else if(hsvVal.r < 205.0 && hsvVal.r > 200.0){
			colorBoxColorRangeBarValue = 0.110f;	
		}
		else if(hsvVal.r < 200.0 && hsvVal.r > 195.0){
			colorBoxColorRangeBarValue = 0.105f;	
		}
		else if(hsvVal.r < 195.0 && hsvVal.r > 190.0){
			colorBoxColorRangeBarValue = 0.1f;	
		}
		else if(hsvVal.r < 190.0 && hsvVal.r > 185.0){
			colorBoxColorRangeBarValue = 0.095f;	
		}
		else if(hsvVal.r < 185.0 && hsvVal.r > 175.0){
			colorBoxColorRangeBarValue = 0.085f;	
		}
		else if(hsvVal.r < 155.0 && hsvVal.r > 145.0){
			colorBoxColorRangeBarValue = 0.057f;	
		}
		else if(hsvVal.r < 145.0 && hsvVal.r > 135.0){
			colorBoxColorRangeBarValue = 0.05f;	
		}
		else if(hsvVal.r < 135.0 && hsvVal.r > 130.0){
			colorBoxColorRangeBarValue = 0.042f;	
		}
		else if(hsvVal.r < 130.0 && hsvVal.r > 125.0){
			colorBoxColorRangeBarValue = 0.042f;	
		}
		else if(hsvVal.r < 125.0 && hsvVal.r > 117.0){
			colorBoxColorRangeBarValue = 0.028f;	
		}
		else if(hsvVal.r < 117.0 && hsvVal.r > 113.0){
			colorBoxColorRangeBarValue = 0.017f;	
		}
		else if(hsvVal.r < 113.0 && hsvVal.r > 107.0){
			colorBoxColorRangeBarValue = 0.013f;	
		}
		else if(hsvVal.r < 107.0 && hsvVal.r > 103.0){
			colorBoxColorRangeBarValue = 0.004f;	
		}
		else if(hsvVal.r < 103.0 && hsvVal.r > 97.0){
			colorBoxColorRangeBarValue = -0.007f;	
		}
		else if(hsvVal.r < 97.0 && hsvVal.r > 92.0){
			colorBoxColorRangeBarValue = -0.015f;	
		}
		else if(hsvVal.r < 92.0 && hsvVal.r > 85.0){
			colorBoxColorRangeBarValue = -0.025f;	
		}
		else if(hsvVal.r < 85.0 && hsvVal.r > 75.0){
			colorBoxColorRangeBarValue = -0.035f;	
		}
		else if(hsvVal.r < 75.0 && hsvVal.r > 70.0){
			colorBoxColorRangeBarValue = -0.043f;	
		}
		else if(hsvVal.r < 70.0 && hsvVal.r > 65.0){
			colorBoxColorRangeBarValue = -0.048f;	
		}
		else if(hsvVal.r < 65.0 && hsvVal.r > 55.0){
			colorBoxColorRangeBarValue = -0.059f;	
		}
		else if(hsvVal.r < 55.0 && hsvVal.r > 45.0){
			colorBoxColorRangeBarValue = -0.075f;	
		}
		else if(hsvVal.r < 42.0 && hsvVal.r > 35.0){
			colorBoxColorRangeBarValue = -0.13f;	
		}
		else if(hsvVal.r < 35.0 && hsvVal.r > 30.0){
			colorBoxColorRangeBarValue = -0.14f;	
		}
		else if(hsvVal.r < 30.0 && hsvVal.r > 25.0){
			colorBoxColorRangeBarValue = -0.15f;	
		}
		else if(hsvVal.r < 25.0 && hsvVal.r > 20.0){
			colorBoxColorRangeBarValue = -0.157f;	
		}
		else if(hsvVal.r < 20.0 && hsvVal.r > 15.0){
			colorBoxColorRangeBarValue = -0.168f;	
		}
		else if(hsvVal.r < 15.0 && hsvVal.r > 10.0){
			colorBoxColorRangeBarValue = -0.178f;	
		}
		else if(hsvVal.r < 10.0 && hsvVal.r > 5.0){
			colorBoxColorRangeBarValue = -0.188f;	
		}
		else if(hsvVal.r < 5.0){
			colorBoxColorRangeBarValue = -0.195f;	
		}
		else{
			colorBoxColorRangeBarValue = (hsvVal.r / 653.846153846f) - 0.195f + 0.02f; //0.195
		}
	}
	colorBoxPickerValue_x = (hsvVal.g / 1342.10526316f) - 0.095f; //0.095
	colorBoxPickerValue_y = (hsvVal.b / 653.846153846f) - 0.195f; //0.195
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

		maskTextures.push_back(txtr.getTexture(maskTexturePath,0,0,false));
		maskTextureNames.push_back(maskTexturePath);

		txtr.updateMaskTexture(FBOScreen,width,height,brushRotationRangeBarValue,false);
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
void LigidPainter::brushBlurRangeBar(float xOffset,int width,int height) {
	Utilities util;
	Texture txtr;
	brushValChanged = true;
	brushBlurRangeBarValue -= xOffset / (width / 2);
	brushBlurRangeBarValue = util.restrictBetween(brushBlurRangeBarValue, 0.11f, -0.11f);//Keep in boundaries
	txtr.updateMaskTexture(FBOScreen,width,height, brushRotationRangeBarValue,true);

	if (242 - ((brushBlurRangeBarValue + 0.1f) * 1000.0) - 15 > 200){ //If the range bar value is low enough disable blur effect
		glUseProgram(programs.blurProgram);
		glset.uniform1f(9, "brushBlurVal", 1000);
		glUseProgram(programs.program);
	}
	else {
		glUseProgram(programs.blurProgram);
		glset.uniform1f(9, "brushBlurVal", 242 - ((brushBlurRangeBarValue + 0.1f) * 1000.0) - 15);
		glUseProgram(programs.program);
	}
}
void LigidPainter::textureDemonstratorButton(float xOffset,float yOffset,int width,int height) {
	panelChanging = true;
	Utilities util;
	Texture txtr;

	textureDemonstratorButtonPosChanged = true;

	textureDemonstratorButtonPosX -= xOffset / (width / 2);
	textureDemonstratorButtonPosX = util.restrictBetween(textureDemonstratorButtonPosX,2.0f,0.0f);
	textureDemonstratorButtonPosY += yOffset / (height / 2);
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
	txtr.updateMaskTexture(FBOScreen, width, height,brushRotationRangeBarValue,true);
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
void LigidPainter::colorBoxColorRangeBar(float yOffset,int height){
	Utilities util;
	colorBoxColorRangeBarValue += yOffset / (height / 2);
	colorBoxColorRangeBarValue = util.restrictBetween(colorBoxColorRangeBarValue, 0.195f, -0.195f);//Keep in boundaries
}
void LigidPainter::colorBoxPickerButton(float xOffset, float yOffset, int width, int height) {
	colorBoxPickerButtonPressed = true;
	Utilities util;
	colorBoxPickerValue_x -= xOffset / (width / 2);
	colorBoxPickerValue_x = util.restrictBetween(colorBoxPickerValue_x, 0.095f, -0.095f);//Keep in boundaries
	colorBoxPickerValue_y += yOffset / (height / 2);
	colorBoxPickerValue_y = util.restrictBetween(colorBoxPickerValue_y, 0.195f, -0.195f);//Keep in boundaries
}

void LigidPainter::modelFilePathTextBox() {
	Utilities utilities;
	char const* lFilterPatterns[1] = { "*.obj" };
	auto modelFilePathCheck = tinyfd_openFileDialog("Select 3D Model","",1, lFilterPatterns,"",false);
	if (modelFilePathCheck) {
		modelFilePath = modelFilePathCheck;
		modelName = utilities.getLastWordBySeparatingWithChar(modelFilePath,folderDistinguisher);
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
void LigidPainter::addSphereButton() {
	addSphereButtonPressed = true;
	GlSet glset;
	Sphere sphere;
	modelName = "sphere.ligidefault";
	vertices = sphere.getSphere();
	glset.bufferData(vertices);
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
void LigidPainter::paintingDropper(){
	paintingDropperPressed = true;
}
void LigidPainter::paintingFillNumericModifier(bool p, bool n){
	if(p && paintingFillNumericModifierVal < 10)
		paintingFillNumericModifierVal++;
	if(n && paintingFillNumericModifierVal > 1)
		paintingFillNumericModifierVal--;
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

	return uidata;
}