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
#include "LigidPainter.h"
#include "UserInterface.h"
#include "Utilities.h"
#include "gl.h"
#include "Texture.h"
#include <vector>
#include "stb_image.h"
#include "stb_image_write.h"
void GlSet::drawArrays(std::vector<float> &vertices,bool isLine) {
	if (!isLine) {
		glBufferSubData(GL_ARRAY_BUFFER,0,vertices.size() * sizeof(float), &vertices[0]);
		glDrawArrays(GL_TRIANGLES, 0, vertices.size() / 8);
	}
	else {
		glBufferSubData(GL_ARRAY_BUFFER, 0,vertices.size() * sizeof(float), &vertices[0]);
		glDrawArrays(GL_LINES, 0, vertices.size() / 8);
	}
}
void GlSet::uiDataToShaders(glm::vec3 color) {
	CommonData cmnd;
	int isAxisPointerLoc = glGetUniformLocation(cmnd.program, "isAxisPointer");
	glUniform1i(isAxisPointerLoc, 0);
	int isTwoDimensionalLoc = glGetUniformLocation(cmnd.program, "isTwoDimensional");
	glUniform1i(isTwoDimensionalLoc, 1);
	int uiColorLoc = glGetUniformLocation(cmnd.program, "uiColor");
	glUniform3f(uiColorLoc, color.x, color.y, color.z);
	int is2DLoc = glGetUniformLocation(cmnd.program, "is2D");
	glUniform1i(is2DLoc, 1);
}
void GlSet::meshDataToShaders() {
	CommonData cmnd;
	int isAxisPointerLoc = glGetUniformLocation(cmnd.program, "isAxisPointer");
	glUniform1i(isAxisPointerLoc, 0);
	int isTwoDimensionalLoc = glGetUniformLocation(cmnd.program, "isTwoDimensional");
	glUniform1i(isTwoDimensionalLoc, 0);
	int is2DLoc = glGetUniformLocation(cmnd.program, "is2D");
	glUniform1i(is2DLoc, 0);
	int isLightSourceLoc = glGetUniformLocation(cmnd.program, "isLightSource");
	glUniform1i(isLightSourceLoc, 0);
}
void GlSet::axisPointerDataToShaders() {
	CommonData cmnd;
	int isAxisPointerLoc = glGetUniformLocation(cmnd.program, "isAxisPointer");
	glUniform1i(isAxisPointerLoc, 1);
	int isTwoDimensionalLoc = glGetUniformLocation(cmnd.program, "isTwoDimensional");
	glUniform1i(isTwoDimensionalLoc, 0);
	int is2DLoc = glGetUniformLocation(cmnd.program, "is2D");
	glUniform1i(is2DLoc, 0);
	int isLightSourceLoc = glGetUniformLocation(cmnd.program, "isLightSource");
	glUniform1i(isLightSourceLoc, 0);
}
void GlSet::bindFramebuffer(unsigned int FBO) {
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
}
void GlSet::genFramebuffers(unsigned int &FBO) {
	glGenFramebuffers(1, &FBO);
}
void GlSet::deleteFramebuffers(unsigned int& FBO) {
	glDeleteFramebuffers(1, &FBO);
}
void GlSet::setVertexAtribPointer() {
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(2);
}
void GlSet::bufferData(std::vector<float> &vertices) {
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_DYNAMIC_DRAW);
}
void GlSet::activeTexture(unsigned int texture) {
	glActiveTexture(texture);
}
void GlSet::bindTexture(unsigned int texture) {
	glBindTexture(GL_TEXTURE_2D, texture);
}
void GlSet::genTextures(unsigned int &texture) {
	glGenTextures(1, &texture);
}
void GlSet::uniform1i(unsigned int program, const char* target, int intValue){
	glUniform1i(glGetUniformLocation(program, target), intValue);
}
void GlSet::uniform1f(unsigned int program, const char* target, float floatValue) {
	glUniform1f(glGetUniformLocation(program, target), floatValue);
}
void GlSet::uniform3fv(unsigned int program, const char* target, glm::vec3 &vectorValue) {
	glUniform3fv(glGetUniformLocation(program, target),1, &vectorValue[0]);
}
void GlSet::uniform3f(unsigned int program, const char* target, float f1, float f2, float f3) {
	glUniform3f(glGetUniformLocation(program, target), f1,f2,f3);
}
void GlSet::uniformMatrix4fv(unsigned int program, const char* target, glm::mat4x4 matrixVal) {
	glUniformMatrix4fv(glGetUniformLocation(program, target), 1,GL_FALSE, glm::value_ptr(matrixVal));
}
void GlSet::enable(unsigned int target) {
	glEnable(target);
}
void GlSet::disable(unsigned int target) {
	glDisable(target);
}
void GlSet::cullFace(unsigned int target) {
	glCullFace(target);
}
void GlSet::viewport(int width,int height) {
	glViewport(0, 0, width, height);
}
void GlSet::blendFunc(unsigned int sfactor, unsigned int dfactor) {
	glBlendFunc(sfactor, dfactor);
}
void GlSet::texImage(GLubyte* data,int width,int height,unsigned int channels) {
	glTexImage2D(GL_TEXTURE_2D, 0, channels, width, height, 0, channels, GL_UNSIGNED_BYTE, data);
}
void GlSet::generateMipmap() {
	glGenerateMipmap(GL_TEXTURE_2D);
}
void GlSet::genRenderbuffers(unsigned int &RBO) {
	glGenRenderbuffers(1, &RBO);
}
void GlSet::bindRenderBuffer(unsigned int RBO) {
	glBindRenderbuffer(GL_RENDERBUFFER, RBO);
}
void GlSet::getProgram() {//Prepare shader program | Usen once
	CommonData cmnd;
	Utilities utilities;
	std::string fshader = utilities.readFile("LigidPainter/Resources/Shaders/fragmentShaderSource.glsl");
	fshader.pop_back();
	const char* fragmentShaderSource = fshader.c_str();
	std::string vshader = utilities.readFile("LigidPainter/Resources/Shaders/vertexShaderSource.glsl");
	vshader.pop_back();
	const char* vertexShaderSource = vshader.c_str();
	unsigned int vertexShader;
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);
#pragma region vertex_compilation_check
	int success;
	char infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	};
#pragma endregion
	unsigned int fragmentShader;
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);

#pragma region fragment_compilation_check
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	};
#pragma endregion

	//Program
	cmnd.program = glCreateProgram();
	glAttachShader(cmnd.program, vertexShader);
	glAttachShader(cmnd.program, fragmentShader);
	glLinkProgram(cmnd.program);
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
#pragma region program_link_check
	glGetProgramiv(cmnd.program, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(cmnd.program, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}
#pragma endregion
}

std::vector<GLubyte*> previousAlbedoTextures; //Used for ctrl z
bool doCtrlZ;
void ctrlZ(GLFWwindow* window) {
	Texture txtr;
	GlSet glset;
	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS && doCtrlZ && previousAlbedoTextures.size() != 0) { //MAX 20
		txtr.refreshScreenDrawingTexture();
		glset.activeTexture(GL_TEXTURE0);
		glset.texImage(previousAlbedoTextures[previousAlbedoTextures.size() - 1], 1080, 1080, GL_RGB);
		glset.generateMipmap();

		GLubyte* previousTexture  = previousAlbedoTextures[previousAlbedoTextures.size() - 1];
		delete[] previousTexture;

		previousAlbedoTextures.pop_back();
		doCtrlZ = false;
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_RELEASE || glfwGetKey(window, GLFW_KEY_Z) == GLFW_RELEASE) {
		doCtrlZ = true;
	}
}
int a = 0;
void drawBrushIndicator(float distanceX,float screenWidth,float screenHeight,float mouseXpos,float mouseYpos,glm::vec3 color) {

	float sizeX = distanceX / ((1920.0f / (float)screenWidth)); //Match the size of the window

	CommonData commonData;
	GlSet glset;
	glset.uniform1i(commonData.program, "drawBrushIndicator", 1);
	std::vector<float> paintingSquare{
		// first triangle
		 sizeX / screenWidth / 1.0f + (float)mouseXpos / screenWidth / 0.5f - 1.0f,  sizeX / screenHeight / 1.0f - (float)mouseYpos / screenHeight / 0.5f + 1.0f , 1.0f,1,1,0,0,0,  // top right
		 sizeX / screenWidth / 1.0f + (float)mouseXpos / screenWidth / 0.5f - 1.0f, -sizeX / screenHeight / 1.0f - (float)mouseYpos / screenHeight / 0.5f + 1.0f , 1.0f,1,0,0,0,0,  // bottom right
		-sizeX / screenWidth / 1.0f + (float)mouseXpos / screenWidth / 0.5f - 1.0f,  sizeX / screenHeight / 1.0f - (float)mouseYpos / screenHeight / 0.5f + 1.0f , 1.0f,0,1,0,0,0,  // top left 

		 sizeX / screenWidth / 1.0f + (float)mouseXpos / screenWidth / 0.5f - 1.0f, -sizeX / screenHeight / 1.0f - (float)mouseYpos / screenHeight / 0.5f + 1.0f , 1.0f,1,0,0,0,0,  // bottom right
		-sizeX / screenWidth / 1.0f + (float)mouseXpos / screenWidth / 0.5f - 1.0f, -sizeX / screenHeight / 1.0f - (float)mouseYpos / screenHeight / 0.5f + 1.0f , 1.0f,0,0,0,0,0,  // bottom left
		-sizeX / screenWidth / 1.0f + (float)mouseXpos / screenWidth / 0.5f - 1.0f,  sizeX / screenHeight / 1.0f - (float)mouseYpos / screenHeight / 0.5f + 1.0f , 1.0f,0,1,0,0,0  // top left
	};
	glset.uiDataToShaders(color/glm::vec3(255.0f));
	glset.uniform1f(commonData.program, "uiOpacity", 0.2f);
	glset.drawArrays(paintingSquare, false);
	glset.uniform1f(commonData.program, "uiOpacity", 0.5f);

	glset.uniform1i(commonData.program, "drawBrushIndicator", 0);
}

float lastColorBoxPickerValue_x = 7;
float lastColorBoxPickerValue_y = 3;
float lastColorBoxRangeValue = 3;
bool isColorBoxValueChanged(RenderData renderData){
	bool colorBoxValChanged = true;
	if (lastColorBoxPickerValue_x != renderData.colorBoxPickerValue_x || lastColorBoxPickerValue_y != renderData.colorBoxPickerValue_y || lastColorBoxRangeValue != renderData.colorBoxColorRangeBarValue) {
		colorBoxValChanged = true;
	}
	else {
		colorBoxValChanged = false;
	}
	lastColorBoxPickerValue_x = renderData.colorBoxPickerValue_x;
	lastColorBoxPickerValue_y = renderData.colorBoxPickerValue_y;
	lastColorBoxRangeValue = renderData.colorBoxColorRangeBarValue;
	return colorBoxValChanged;
}
void drawAxisPointer() {
	GlSet glset;
	//Axis Pointer - start from middle of the scene
	std::vector<float>axisPointer{
	0.0f, -100.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, //Y
	0.0f, 100.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,

	-100.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, //X
	100.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,

	0.0f, 0.0f, -100.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, //Z
	0.0f, 0.0f, 100.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
	};
	glset.axisPointerDataToShaders();
	glset.blendFunc(GL_SRC_ALPHA, GL_SRC_ALPHA);
	glset.drawArrays(axisPointer, true);
	glset.blendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//Axis pointer
}

void GlSet::renderTexture(unsigned int FBOScreen, std::vector<float>& vertices,bool exportImage, bool JPG, bool PNG, const char* exportPath,unsigned int screenSizeX, unsigned int screenSizeY) {
	int maxTextureHistoryHold = 20;

	std::vector<float> renderVertices = { //Render backside of the uv
	// first triangle
	 1.0f,  1.0f, 0.0f,1,1,0,0,0,  // top right
	 1.0f,  0.0f, 0.0f,1,0,0,0,0,  // bottom right
	 0.0f,  1.0f, 0.0f,0,1,0,0,0,  // top left 
	// second triangle	  ,0,0,0,
	 1.0f,  0.0f, 0.0f,1,0,0,0,0,  // bottom right
	 0.0f,  0.0f, 0.0f,0,0,0,0,0,  // bottom left
	 0.0f,  1.0f, 0.0f,0,1,0,0,0   // top left
	};
	
	std::vector<float> enlargingVertices = { //Enlarge rendered image with 1 pixel so we wont have seams
		// first triangle
		 1.0f,  1.002f, 0.0f,1,1,0,0,0,  // top right
		 1.0f,  -0.002f, 0.0f,1,0,0,0,0,  // bottom right
		 -0.0f,  1.002f, 0.0f,0,1,0,0,0,  // top left 
		// second triangle	  ,0,0,0,
		 1.0f,  -0.002f, 0.0f,1,0,0,0,0,  // bottom right
		 -0.0f,  -0.002f, 0.0f,0,0,0,0,0,  // bottom left
		 -0.0f,  1.002f, 0.0f,0,1,0,0,0   // top left
	};

	Texture txtr;
	CommonData commonData;

	GLubyte* originalImage = txtr.getTextureFromProgram(GL_TEXTURE0, 1080, 1080, 3);
	previousAlbedoTextures.push_back(originalImage);

	if (previousAlbedoTextures.size() > maxTextureHistoryHold){
		GLubyte* previousTexture  = previousAlbedoTextures[0];
		delete[] previousTexture;
		previousAlbedoTextures.erase(previousAlbedoTextures.begin());
	}


	//Setup
	uniform1i(commonData.program, "isTwoDimensional", 0);
	viewport(1920, 1080);
	uniform1i(commonData.program, "isRenderTextureMode", 1);
	uniform1i(commonData.program, "isRenderTextureModeV", 1);
	bindFramebuffer(FBOScreen);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//Setup

	//Render painted image
	drawArrays(vertices, false);
	if (!exportImage)
		drawArrays(renderVertices, false);
	//GLubyte* renderedImage = txtr.getTextureFromProgram(GL_TEXTURE10,1080, 1080, 3);
	GLubyte* renderedImage = new GLubyte[1080 * 1080 * 3 * sizeof(GLubyte)];
	glReadPixels(0, 0, 1080, 1080, GL_RGB, GL_UNSIGNED_BYTE, renderedImage);
	activeTexture(GL_TEXTURE0);
	texImage(renderedImage, 1080, 1080, GL_RGB);
	generateMipmap();
	//Render painted image

	txtr.refreshScreenDrawingTexture();

	//Render uv mask
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	uniform1i(commonData.program, "whiteRendering", 1);
	drawArrays(vertices, false); //Render Model
	uniform1i(commonData.program, "whiteRendering", 0);
	GLubyte* uvMask = new GLubyte[1080 * 1080 * 3 * sizeof(GLubyte)];
	glReadPixels(0, 0, 1080, 1080, GL_RGB, GL_UNSIGNED_BYTE, uvMask);

	activeTexture(GL_TEXTURE7);
	texImage(uvMask, 1080, 1080, GL_RGB);
	generateMipmap();
	//Render uv mask

	//interpret the albedo with ui mask texture
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	uniform1i(commonData.program, "interpretWithUvMask", 1);
	drawArrays(renderVertices, false); //Render Model
	uniform1i(commonData.program, "interpretWithUvMask", 0);
	GLubyte* enlargedTxtr = new GLubyte[1080 * 1080 * 3 * sizeof(GLubyte)];
	glReadPixels(0, 0, 1080, 1080, GL_RGB, GL_UNSIGNED_BYTE, enlargedTxtr);

	activeTexture(GL_TEXTURE0);
	texImage(enlargedTxtr, 1080, 1080, GL_RGB);
	generateMipmap();
	//interpret the albedo with ui mask texture

	//Download enlarged image
	if (exportImage) {
		if (JPG) {
			txtr.downloadTexture(exportPath, "LigidPainter Export", 0, 1080, 1080, enlargedTxtr, 3);
		}
		else if (PNG) {
			txtr.downloadTexture(exportPath, "LigidPainter Export", 1, 1080, 1080, enlargedTxtr, 3);
		}
	}
	//Download enlarged image
	//Finish rendering albedo texture
	delete[]renderedImage;
	delete[]uvMask;
	delete[]enlargedTxtr;
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//Finish rendering albedo texture

	//Finish
	uniform1i(commonData.program, "isRenderTextureModeV", 0);
	uniform1i(commonData.program, "isRenderTextureMode", 0);
	bindFramebuffer(0);
	viewport(screenSizeX, screenSizeY);
	//Finish
}
glm::vec3 colorBoxVal = glm::vec3(0);
void GlSet::getColorBoxValue(unsigned int FBOScreen,float colorBoxPickerValue_x, float colorBoxPickerValue_y, unsigned int screenSizeX, unsigned int screenSizeY) {
	std::vector<float> colorBox = { //Render color box into the screen so we can get it's value
	// first triangle
	 0.0f,  1.0f, 0.0f,1,1,1,1,1,  // top right
	 0.0f,  0.0f, 0.0f,1,0,0,0,0,  // bottom right
	 1.0f,  1.0f, 0.0f,0,1,0,0,0,  // top left 
	// second triangle	  ,0,0,0,
	 0.0f,  0.0f, 0.0f,1,0,0,0,0,  // bottom right7
	 1.0f,  0.0f, 0.0f,0,0,0,0,0,  // bottom left
	 1.0f,  1.0f, 0.0f,0,1,0,0,0   // top left
	};

	CommonData commonData;
	//Setup
	viewport(1920, 1080);
	uniform1i(commonData.program, "isRenderTextureModeV", 1);
	uniform1i(commonData.program, "isRenderTextureMode", 1);
	bindFramebuffer(FBOScreen);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//Setup

	//Render color box
	GLubyte* colorBoxPixel = new GLubyte[1 * 1 * 3];//Color val
	uniform1i(commonData.program, "isColorBox", 1);
	drawArrays(colorBox, false); //Render Model
	uniform1i(commonData.program, "isColorBox", 0);
	glReadPixels(1080 - ((colorBoxPickerValue_x * -1.0f + 0.1f) * 5.0f * 1080), (colorBoxPickerValue_y + 0.2f) * 2.5f * 1080, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, colorBoxPixel);
	uniform3f(commonData.program, "drawColor", colorBoxPixel[0] / 255.0f, colorBoxPixel[1] / 255.0f, colorBoxPixel[2] / 255.0f);
	//Render color box

	//Finish
	uniform1i(commonData.program, "isRenderTextureMode", 0);
	uniform1i(commonData.program, "isRenderTextureModeV", 0);
	bindFramebuffer(0);
	viewport(screenSizeX, screenSizeY);

	//Get color value to the color vec
	colorBoxVal.r = colorBoxPixel[0];
	colorBoxVal.g = colorBoxPixel[1];
	colorBoxVal.b = colorBoxPixel[2];
	delete[]colorBoxPixel;
	//Finish
}

int renderDepthCounter = 0;

float exportDownloadButtonMixVal = 0.0f;
float addMaskTextureButtonMixVal = 0.0f;
float loadModelButtonMixVal = 0.0f;
float addPanelButtonMixVal = 0.0f;
float addSphereButtonMixVal = 0.0f;
float addAlbedoTextureMixVal = 0.0f;

float changeTextureDemonstratorWidth = 0.4f;
float changeTextureDemonstratorHeight = 0.8f;

float orgTextureDemonstratorWidth = 0.4f;
float orgTextureDemonstratorHeight = 0.8f;
glm::vec3 GlSet::render(RenderData renderData, std::vector<float>& vertices, unsigned int FBOScreen, PanelData panelData, ExportData exportData,UiData uidata,float textureDemonstratorButtonPosX,float textureDemonstratorButtonPosY, bool textureDemonstratorButtonPressClicked,float textureDemonstratorWidth, float textureDemonstratorHeight,bool textureDemonstratorBoundariesPressed,Icons icons) {
	GlSet gls;
	UserInterface ui;
	CommonData commonData;
	ColorData colorData;
	Utilities util;
	Texture txtr;

	if(textureDemonstratorBoundariesPressed){
		orgTextureDemonstratorWidth = textureDemonstratorWidth;
		orgTextureDemonstratorHeight = textureDemonstratorHeight;
	}

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //Clear before rendering

	//Get screen and mouse info
	int screenSizeX;
	int screenSizeY;
	glfwGetWindowSize(renderData.window, &screenSizeX, &screenSizeY);
	double mouseXpos;
	double mouseYpos;
	glfwGetCursorPos(renderData.window, &mouseXpos, &mouseYpos);
	//Get screen and mouse info

	bool colorBoxValChanged = isColorBoxValueChanged(renderData);

	drawAxisPointer();
	
	//Render depth once painting started
	if (renderData.paintingMode) { 
		renderDepthCounter++;
	}
	else {
		renderDepthCounter = 0;
	}
	if (renderDepthCounter == 1) {//Get depth texture
		getDepthTexture(vertices,FBOScreen,screenSizeX,screenSizeY);
	}
	//Render depth once painting started

	bool isRenderTexture = (renderData.cameraPosChanged && renderData.paintingMode) || exportData.exportImage || uidata.addImageButtonPressed || (colorBoxValChanged && renderData.paintingMode) || (glfwGetMouseButton(renderData.window, 0) == GLFW_RELEASE && renderData.paintingMode); //addImageButtonPressed = albedo texture changed
	if (isRenderTexture) { //colorboxvalchanged has to trigger paintingmode to false
		renderTexture(FBOScreen,vertices,exportData.exportImage,uidata.exportExtJPGCheckBoxPressed, uidata.exportExtPNGCheckBoxPressed,exportData.path,screenSizeX, screenSizeY);
	}
	renderUi(panelData, uidata, renderData, FBOScreen, renderData.brushBlurRangeBarValue,renderData.brushRotationRangeBarValue, renderData.brushOpacityRangeBarValue, renderData.brushSpacingRangeBarValue,textureDemonstratorButtonPosX,textureDemonstratorButtonPosY,textureDemonstratorButtonPressClicked,icons);

	if (colorBoxValChanged) { //Get value of color box
		getColorBoxValue(FBOScreen, lastColorBoxPickerValue_x, lastColorBoxPickerValue_y,screenSizeX, screenSizeY);
	}

	ctrlZ(renderData.window);

	renderModel(renderData.backfaceCulling,vertices);

	updateButtonColorMixValues(uidata);


	
	glm::vec3 screenHoverPixel;

	GLubyte* screenPixel = new GLubyte[1 * 1 * 3];//Color val
	glReadPixels(mouseXpos,screenSizeY - mouseYpos, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, screenPixel);

	screenHoverPixel.r = screenPixel[0];
	screenHoverPixel.g = screenPixel[1];
	screenHoverPixel.b = screenPixel[2];
	delete[] screenPixel;  

	if(renderData.doPainting)
		drawBrushIndicator(renderData.brushSizeIndicator, screenSizeX, screenSizeY, mouseXpos, mouseYpos, colorBoxVal);

	return screenHoverPixel;
}

void GlSet::updateButtonColorMixValues(UiData uidata) {
	if (uidata.addSphereButtonEnter && addSphereButtonMixVal <= 1.0f) {
		addSphereButtonMixVal += 0.05f;
	}
	else if (!uidata.addSphereButtonEnter && addSphereButtonMixVal >= 0.0f) {
		addSphereButtonMixVal -= 0.05f;
	}

	if (uidata.addPlaneButtonEnter && addPanelButtonMixVal <= 1.0f) {
		addPanelButtonMixVal += 0.05f;
	}
	else if (!uidata.addPlaneButtonEnter && addPanelButtonMixVal >= 0.0f) {
		addPanelButtonMixVal -= 0.05f;
	}

	if (uidata.addImageButtonEnter && addAlbedoTextureMixVal <= 1.0f) {
		addAlbedoTextureMixVal += 0.05f;
	}
	else if (!uidata.addImageButtonEnter && addAlbedoTextureMixVal >= 0.0f) {
		addAlbedoTextureMixVal -= 0.05f;
	}

	if (uidata.loadModelButtonEnter && loadModelButtonMixVal <= 1.0f) {
		loadModelButtonMixVal += 0.05f;
	}
	else if (!uidata.loadModelButtonEnter && loadModelButtonMixVal >= 0.0f) {
		loadModelButtonMixVal -= 0.05f;
	}

	if (uidata.addMaskTextureButtonEnter && addMaskTextureButtonMixVal <= 1.0f) {
		addMaskTextureButtonMixVal += 0.05f;
	}
	else if (!uidata.addMaskTextureButtonEnter && addMaskTextureButtonMixVal >= 0.0f) {
		addMaskTextureButtonMixVal -= 0.05f;
	}

	if (uidata.exportDownloadButtonEnter && exportDownloadButtonMixVal <= 1.0f) {
		exportDownloadButtonMixVal += 0.05f;
	}
	else if (!uidata.exportDownloadButtonEnter && exportDownloadButtonMixVal >= 0.0f) {
		exportDownloadButtonMixVal -= 0.05f;
	}
}
void GlSet::renderModel(bool backfaceCulling, std::vector<float>& vertices) {
	CommonData commonData;
	if (backfaceCulling) { //if backface culling checked in the model panel
		enable(GL_CULL_FACE);
		cullFace(GL_BACK);
	}
	meshDataToShaders();
	drawArrays(vertices, false);

	disable(GL_CULL_FACE); //Disable backface culling if enabled
}
bool changeTextureDemonstrator;

void GlSet::renderUi(PanelData panelData,UiData uidata,RenderData renderData,unsigned int FBOScreen, float brushBlurRangeBarValue, float brushRotationRangeBarValue, float brushOpacityRangeBarValue, float brushSpacingRangeBarValue,float textureDemonstratorButtonPosX,float textureDemonstratorButtonPosY,bool textureDemonstratorButtonPressClicked,Icons icons) {
	ColorData colorData;
	CommonData commonData;
	glm::mat4 projection;
	UserInterface ui;
	Utilities util;
	uniform1i(commonData.program, "isTwoDimensional", 1);
	uniform1i(commonData.program, "is2D", 1);

	//Panel
	ui.panel(renderData.panelLoc, 0);
	projection = glm::ortho(0.0f, 2.0f, -1.0f, 1.0f);
	uniformMatrix4fv(commonData.program, "TextProjection", projection);
	ui.panelChangeButton(renderData.panelLoc, 0.8f);//Model Panel
	ui.iconBox(0.015f,0.02f,renderData.panelLoc-0.01f,0.795f,1,icons.TDModel);
	ui.panelChangeButton(renderData.panelLoc, 0.72f);//Texture Panel
	ui.iconBox(0.015f,0.02f,renderData.panelLoc-0.01f,0.715f,1,icons.ImportTexture);
	ui.panelChangeButton(renderData.panelLoc, 0.64f);//Painting Panel
	//ui.iconBox(0.03f,0.04f,renderData.panelLoc,0.64f,1,icons.);//Dropper
	ui.panelChangeButton(renderData.panelLoc, 0.56f);//Export Panel
	ui.iconBox(0.015f,0.02f,renderData.panelLoc-0.01f, 0.555f,1,icons.Export);
	
	//Panel


	if(textureDemonstratorButtonPressClicked){
		if(changeTextureDemonstrator){
			changeTextureDemonstrator = false;
		}
		else{
			changeTextureDemonstrator = true;
		}
	}

	if(changeTextureDemonstrator){
		changeTextureDemonstratorWidth -= 0.035f;
		changeTextureDemonstratorHeight -= 0.07f;
		
		if(changeTextureDemonstratorWidth < 0.0f){
			changeTextureDemonstratorWidth = 0.0f;
		}
		if(changeTextureDemonstratorHeight < 0.0f){
			changeTextureDemonstratorHeight = 0.0f;
		}
	}
	else{
		changeTextureDemonstratorWidth += 0.035f;
		changeTextureDemonstratorHeight += 0.07f;
		if(changeTextureDemonstratorWidth > orgTextureDemonstratorWidth){
			changeTextureDemonstratorWidth = orgTextureDemonstratorWidth;
		}
		if(changeTextureDemonstratorHeight > orgTextureDemonstratorHeight){
			changeTextureDemonstratorHeight = orgTextureDemonstratorHeight;
		}
	}
	ui.textureDemonstrator(changeTextureDemonstratorWidth,changeTextureDemonstratorHeight,textureDemonstratorButtonPosX,textureDemonstratorButtonPosY,1.0f); 


	if (panelData.paintingPanelActive){

		ui.checkBox(renderData.panelLoc- 0.16f, 0.9f, "X", colorData.checkBoxColor, uidata.mirrorXCheckBoxEnter, uidata.mirrorXCheckBoxPressed); //X mirror checkbox
		ui.checkBox(renderData.panelLoc- 0.10f, 0.9f, "Y", colorData.checkBoxColor, uidata.mirrorYCheckBoxEnter, uidata.mirrorYCheckBoxPressed); //Z mirror checkbox
		ui.checkBox(renderData.panelLoc- 0.04f, 0.9f, "Z", colorData.checkBoxColor, uidata.mirrorZCheckBoxEnter, uidata.mirrorZCheckBoxPressed); //Y mirror checkbox
		ui.box(0.25f, 0.22f, renderData.panelLoc + 0.25f, -0.6f, "", colorData.panelColorSnd, 0.075f, false, false, 0.1f, 10000, glm::vec3(0), 0);//Backside decoration
		ui.iconBox(0.015f,0.02f,renderData.panelLoc - 0.09f,0.95f,1,icons.Mirror);
	}

	float centerDivider;
	float centerSum;
	uniform1f(commonData.program, "uiOpacity", 0.5f);
	if (!panelData.movePanel) {
		centerDivider = 2.0f;
		centerSum = 0;
		projection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f);
		uniformMatrix4fv(commonData.program, "TextProjection", projection);
	}
	else {
		centerDivider = 1.0f;
		centerSum = 0.15f;
		projection = glm::ortho(0.0f, 2.0f, -1.0f, 1.0f);
		uniformMatrix4fv(commonData.program, "TextProjection", projection);
	}
	if (panelData.modelPanelActive) {
		//File path textbox
		ui.box(0.12f, 0.03f, renderData.panelLoc / centerDivider + centerSum, 0.6f, renderData.modelLoadFilePath, colorData.textBoxColor, 0, true, false, 0.9f, 10, glm::vec3(0), 0);
		ui.renderText(commonData.program, "File Path", renderData.panelLoc / centerDivider + centerSum - 0.05f, 0.64f, 0.00022f);
		ui.iconBox(0.020f,0.04f,renderData.panelLoc / centerDivider + centerSum + 0.1f,0.6f,1,icons.Folder);
		


		ui.box(0.08f, 0.04f, renderData.panelLoc / centerDivider + centerSum, 0.4f, "Load", colorData.buttonColor, 0.022f, false, false, 0.9f, 10, colorData.buttonColorHover, loadModelButtonMixVal);//Load model button
		ui.box(0.012f, 0.04f, renderData.panelLoc / centerDivider + centerSum, 0.0f, "", colorData.buttonColor, 0.047f, false, false, 0.9f, 7, colorData.buttonColorHover, addPanelButtonMixVal);//Load a panel button
		ui.iconBox(0.03f,0.04f,renderData.panelLoc / centerDivider + centerSum,0.0,1,icons.Panel);
		ui.box(0.012f, 0.04f, renderData.panelLoc / centerDivider + centerSum, -0.1f, "", colorData.buttonColor, 0.055f, false, false, 0.9f, 7, colorData.buttonColorHover, addSphereButtonMixVal);//Load a sphere button
		ui.iconBox(0.03f,0.04f,renderData.panelLoc / centerDivider + centerSum,-0.1f,1,icons.Sphere);


		ui.checkBox(renderData.panelLoc / centerDivider + centerSum - 0.08f, 0.3f, "Auto triangulate", colorData.checkBoxColor, uidata.autoTriangulateCheckBoxEnter, uidata.autoTriangulateCheckBoxPressed); //Auto triangulate checkbox
		ui.checkBox(renderData.panelLoc / centerDivider + centerSum - 0.08f, 0.2f, "Backface culling", colorData.checkBoxColor, uidata.backfaceCullingCheckBoxEnter, uidata.backfaceCullingCheckBoxPressed); //Backface culling checkbox

	}
	if (panelData.texturePanelActive) {
		ui.box(0.1f, 0.04f, renderData.panelLoc / centerDivider + centerSum, 0.8f, "Add Image", colorData.buttonColor, 0.07f, false, false, 0.9f, 10, colorData.buttonColorHover, addAlbedoTextureMixVal); //Add albedo texture button
	}

	if (panelData.paintingPanelActive) {
		ui.box(0.1f, 0.04f, renderData.panelLoc / centerDivider + centerSum, 0.85f, "Add Mask Texture", colorData.buttonColor, 0.095f, false, false, 0.9f, 10, colorData.buttonColorHover, addMaskTextureButtonMixVal); //Add mask texture button
		
		ui.checkBox(renderData.panelLoc / centerDivider + centerSum - 0.1f, 0.76f, "Use Negative", colorData.checkBoxColor, uidata.useNegativeForDrawingCheckboxEnter, uidata.useNegativeForDrawingCheckboxPressed); //Auto triangulate checkbox

		ui.box(0.14f, 0.28f, renderData.panelLoc / centerDivider + centerSum, 0.45f, "", colorData.buttonColor, 0.075f, false, true, 0.9f, 1000, glm::vec3(0), 0); //Mask texture displayer / GL_TEXTURE12

		//Brush size rangebar
		ui.renderText(commonData.program, "Size", renderData.panelLoc / centerDivider + centerSum, 0.13f, 0.00022f);
		ui.rangeBar(renderData.panelLoc / centerDivider + centerSum, 0.1f, renderData.brushSizeRangeBarValue);
		ui.iconBox(0.03f,0.04f,renderData.panelLoc / centerDivider + centerSum - 0.05f, 0.145f,1,icons.MaskScale);

		//Brush blur rangebar
		ui.renderText(commonData.program, "Blur", renderData.panelLoc / centerDivider + centerSum, +0.035f, 0.00022f);
		ui.rangeBar(renderData.panelLoc / centerDivider + centerSum, -0.00f, brushBlurRangeBarValue);
		ui.iconBox(0.03f,0.04f,renderData.panelLoc / centerDivider + centerSum - 0.05f, 0.05f,1,icons.MaskGausBlur);

		//Brush rotation rangebar
		ui.renderText(commonData.program, "Rotation", renderData.panelLoc / centerDivider + centerSum, -0.065f, 0.00022f);
		ui.rangeBar(renderData.panelLoc / centerDivider + centerSum, -0.10f, brushRotationRangeBarValue);
		ui.iconBox(0.03f,0.04f,renderData.panelLoc / centerDivider + centerSum - 0.05f, -0.05f,1,icons.MaskRotation);

		//Brush opacity rangebar
		ui.renderText(commonData.program, "Opacity", renderData.panelLoc / centerDivider + centerSum, -0.165f, 0.00022f);
		ui.rangeBar(renderData.panelLoc / centerDivider + centerSum, -0.20f, brushOpacityRangeBarValue);
		ui.iconBox(0.03f,0.04f,renderData.panelLoc / centerDivider + centerSum - 0.05f, -0.15f,1,icons.MaskOpacity);

		//Brush spacing rangebar
		ui.renderText(commonData.program, "Spacing", renderData.panelLoc / centerDivider + centerSum, -0.265f, 0.00022f);
		ui.rangeBar(renderData.panelLoc / centerDivider + centerSum, -0.30f, brushSpacingRangeBarValue);
		ui.iconBox(0.03f,0.04f,renderData.panelLoc / centerDivider + centerSum - 0.05f, -0.25f,1,icons.MaskSpacing);

		//Color Picker
		ui.colorBox(renderData.panelLoc / centerDivider + centerSum - 0.02f, -0.6f, renderData.colorBoxPickerValue_x, renderData.colorBoxPickerValue_y);
		ui.colorRect(renderData.panelLoc / centerDivider + centerSum + 0.1f, -0.6f, renderData.colorBoxColorRangeBarValue, FBOScreen, renderData.window);

		uniform1f(commonData.program, "uiOpacity", 1.0f);
		ui.box(0.005f, 0.025f, renderData.panelLoc / centerDivider + centerSum - 0.095f, -0.85f, "", colorBoxVal / glm::vec3(255), 0.075f, false, false, 0.9f, 10, glm::vec3(0), 0); //indicator for picken color of the color picker
		uniform1f(commonData.program, "uiOpacity", 0.5f);
		ui.decorationSquare(renderData.panelLoc / centerDivider + centerSum - 0.1f, -0.84f); //Decoration for color indicator

		ui.renderText(commonData.program, util.rgbToHexGenerator(colorBoxVal), renderData.panelLoc / centerDivider + centerSum - 0.05f, -0.86f, 0.00022f); //Hex value of the picken color 
		ui.iconBox(0.02f,0.03f,renderData.panelLoc / centerDivider + centerSum + 0.05f, -0.86f,1,icons.dropperIcon);
	}

	if (panelData.exportPanelActive) {
		ui.box(0.12f, 0.03f, renderData.panelLoc / centerDivider + centerSum, 0.6f, renderData.exportFolder, colorData.textBoxColor, 0, true, false, 0.9f, 10, glm::vec3(0), 0); //Path textbox
		ui.checkBox(renderData.panelLoc / centerDivider + centerSum - 0.11f, 0.5f, "", colorData.checkBoxColor, uidata.exportExtJPGCheckBoxEnter, uidata.exportExtJPGCheckBoxPressed); //jpg checkbox
		ui.iconBox(0.05f,0.065f,renderData.panelLoc / centerDivider + centerSum - 0.06f, 0.5f,1,icons.JpgFile);

		ui.checkBox(renderData.panelLoc / centerDivider + centerSum + 0.05f, 0.5f, "", colorData.checkBoxColor, uidata.exportExtPNGCheckBoxEnter, uidata.exportExtPNGCheckBoxPressed); //png checkbox
		ui.iconBox(0.05f,0.065f,renderData.panelLoc / centerDivider + centerSum + 0.1f, 0.5f,1,icons.PngFile);

		ui.box(0.1f, 0.04f, renderData.panelLoc / centerDivider + centerSum, 0.3f, "Download", colorData.buttonColor, 0.045f, false, false, 0.9f, 10, colorData.buttonColorHover, exportDownloadButtonMixVal); //Download Button
	}
	projection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f);
	uniformMatrix4fv(commonData.program, "TextProjection", projection);
}
GLFWwindow* GlSet::getWindow() {
	glfwInit();
	//glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	GLFWwindow* window = glfwCreateWindow(1920, 1080, "Mesh Painter", NULL, NULL);
	glfwMakeContextCurrent(window);

	GLFWimage images[1];
	images[0].pixels = stbi_load("LigidPainter/Resources/Icons/logo-1080x.png", &images[0].width, &images[0].height, 0, 4); //rgba channels 
	glfwSetWindowIcon(window, 1, images);
	stbi_image_free(images[0].pixels);

	gladLoadGL();
	return window;
}
void GlSet::setMatrices() {
	CommonData cmnd;

	glm::mat4 textProjection = glm::ortho(0.0f, 2.0f, -1.0f, 1.0f);
	uniformMatrix4fv(cmnd.program, "TextProjection", textProjection);

	glm::mat4 renderTextureProjection = glm::ortho(0.0f, 1.77777777778f, 0.0f, 1.0f);//1920 - 1080 -> 1.77777777778 - 1
	uniformMatrix4fv(cmnd.program, "renderTextureProjection", renderTextureProjection);

	glm::mat4 model = glm::mat4(1.0f);
	uniformMatrix4fv(cmnd.program, "model", model);

	glm::mat4 projection = glm::perspective(glm::radians(45.0f), 1920.0f / 1080.0f, 0.1f, 1000.0f);
	uniformMatrix4fv(cmnd.program, "projection",projection);

	ProjectionData pd;
	pd.modelMat = model;
	pd.projMat = projection;
	//pd.viewMat = view;
}
void GlSet::updateViewMatrix(glm::vec3 cameraPos, glm::vec3 originPos,bool mirrorX,bool mirrorY,bool mirrorZ) {
	CommonData cmnd;
	glm::mat4 view;
	view = glm::lookAt(cameraPos, originPos, glm::vec3(0.0, 1.0, 0.0)); 

	glm::vec3 mirrorVec = glm::vec3(1.0f - (int)mirrorX*2, 1.0f -(int)mirrorY * 2, 1.0f - (int)mirrorZ * 2);
	glm::mat4 mirroredView;
	mirroredView = glm::lookAt(cameraPos * mirrorVec, originPos * mirrorVec, glm::vec3(0.0, 1.0, 0.0));

	uniformMatrix4fv(cmnd.program, "view", view);
	uniformMatrix4fv(cmnd.program, "mirroredView", mirroredView);
}
void GlSet::getUnprojection(glm::vec3 vPos, glm::vec3 cameraPos, glm::vec3 originPos) { //Not used
	/*GlSet glset;
	CommonData cmnd;
	ProjectionData pd = glset.setMatrices(cameraPos, originPos);
	return glm::project(vPos, pd.modelMat * pd.viewMat, pd.projMat, glm::vec4(0, 0, 1920, 1080));*/
}

void GlSet::drawLightObject(glm::vec3 lightPos) {
	/*CommonData cmnd;
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::mat4(1.0f);
	model = glm::translate(model, lightPos);
	model = glm::scale(model, glm::vec3(0.2f));

	int lightColorLoc = glGetUniformLocation(cmnd.program, "lightColor");
	glUniform3f(lightColorLoc, 10.0f, 10.0f, 10.0f);
	int isLightSourceLoc = glGetUniformLocation(cmnd.program, "isLightSource");
	glUniform1i(isLightSourceLoc, 1);
	int modelLoc = glGetUniformLocation(cmnd.program, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	glDrawArrays(GL_TRIANGLES, 0, 36);*/
}
unsigned int GlSet::createScreenFrameBufferObject() {
	activeTexture(GL_TEXTURE5);
	unsigned int FBO;
	genFramebuffers(FBO);
	bindFramebuffer(FBO);
	unsigned int textureColorbuffer;
	genTextures(textureColorbuffer);
	bindTexture(textureColorbuffer);
	texImage(NULL, 1920,1080,GL_RGB);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorbuffer, 0);
	unsigned int RBO;
	genRenderbuffers(RBO);
	bindRenderBuffer(RBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 1920, 1080);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	return FBO;
}
void GlSet::getDepthTexture(std::vector<float>& vertices,unsigned int FBOScreen, unsigned int screenSizeX, unsigned int screenSizeY) { //Set viewport to screen size and bind the FRAMEBUFFER_0
	CommonData commonData;
	Texture txtr;
	viewport(1920, 1080);
	bindFramebuffer(FBOScreen);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	uniform1i(commonData.program, "isTwoDimensional", 0);
	uniform1i(commonData.program, "isRenderTextureMode", 1);
	uniform1i(commonData.program, "isRenderTextureModeV", 0);
	uniform1i(commonData.program, "depthTexture", 9);
	uniform1i(commonData.program, "mirroredDepthTexture", 8);
	uniform1i(commonData.program, "renderDepth", 1);
	uniform1i(commonData.program, "renderMirroredDepth", 0);
	drawArrays(vertices, false);
	GLubyte* screen = txtr.getTextureFromProgram(GL_TEXTURE5, 1920, 1080, 3);
	activeTexture(GL_TEXTURE9);
	texImage(screen, 1920, 1080, GL_RGB);
	generateMipmap();
	delete[]screen;

	uniform1i(commonData.program, "renderMirroredDepth", 1);
	drawArrays(vertices, false);
	GLubyte* screenMirrored = txtr.getTextureFromProgram(GL_TEXTURE5, 1920, 1080, 3);
	activeTexture(GL_TEXTURE8);
	texImage(screenMirrored, 1920, 1080, GL_RGB);
	generateMipmap();
	delete[] screenMirrored;

	uniform1i(commonData.program, "isRenderTextureModeV", 0);
	uniform1i(commonData.program, "isRenderTextureMode", 0);
	uniform1i(commonData.program, "renderDepth", 0);
	uniform1i(commonData.program, "renderMirroredDepth", 0);
	viewport(screenSizeX, screenSizeY);
	bindFramebuffer(0);
}