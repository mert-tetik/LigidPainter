#include<iostream>

#include<glad/glad.h>
#include<GLFW/glfw3.h>

#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <filesystem>


#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

#include "Core/Model/model.h"

#include "Core/Render/Render.h"
#include "Core/LigidPainter.h"
#include "Core/UI/UserInterface.h"
#include "Core/Utilities.h"
#include "Core/gl.h"
#include "Core/Texture/Texture.h"

#include "stb_image.h"
#include "stb_image_write.h"

#include "tinyfiledialogs.h"




int currentMaterialIndex = 0;

Programs renderPrograms;

int renderMaxScreenWidth;
int renderMaxScreenHeight;

//--------------------RENDER UI --------------------\\


unsigned int currentBrushMaskTexture;


SaturationValShaderData saturationValShaderData;

glm::mat4 Render::setMatrices() {
    GlSet gl;
	glm::mat4 textProjection = glm::ortho(0.0f, 2.0f, -1.0f, 1.0f);
	glUseProgram(renderPrograms.iconsProgram);
	gl.uniformMatrix4fv(renderPrograms.iconsProgram, "Projection", textProjection);
	glUseProgram(renderPrograms.uiProgram);
	gl.uniformMatrix4fv(renderPrograms.uiProgram, "TextProjection", textProjection);

	glm::mat4 renderTextureProjection = glm::ortho(0.0f, 1.77777777778f, 0.0f, 1.0f);//1920 - 1080 -> 1.77777777778 - 1
	glUseProgram(renderPrograms.blurProgram);
	gl.uniformMatrix4fv(renderPrograms.blurProgram, "renderTextureProjection", renderTextureProjection);
	glUseProgram(renderPrograms.outProgram);
	gl.uniformMatrix4fv(renderPrograms.outProgram, "renderTextureProjection", renderTextureProjection);
	glUseProgram(renderPrograms.uiProgram);
	gl.uniformMatrix4fv(renderPrograms.uiProgram, "renderTextureProjection", renderTextureProjection);

	glm::mat4 model = glm::mat4(1.0f);
	gl.uniformMatrix4fv(renderPrograms.uiProgram, "model", model);

	glm::mat4 projection = glm::perspective(glm::radians(45.0f), 1920.0f / 1080.0f, 0.1f, 1000.0f);
	glUseProgram(renderPrograms.uiProgram);
	gl.uniformMatrix4fv(renderPrograms.uiProgram, "projection",projection);

	return projection;
	//pd.viewMat = view;
}
ViewUpdateData Render::updateViewMatrix(glm::vec3 cameraPos, glm::vec3 originPos,bool mirrorX,bool mirrorY,bool mirrorZ) {
	glm::mat4 view;
    GlSet gl;
	view = glm::lookAt(cameraPos, originPos, glm::vec3(0.0, 1.0, 0.0)); 

	glm::vec3 mirrorVec = glm::vec3(1.0f - (int)mirrorX*2, 1.0f -(int)mirrorY * 2, 1.0f - (int)mirrorZ * 2);
	glm::mat4 mirroredView;
	mirroredView = glm::lookAt(cameraPos * mirrorVec, originPos * mirrorVec, glm::vec3(0.0, 1.0, 0.0));

	glUseProgram(renderPrograms.uiProgram);
	gl.uniformMatrix4fv(renderPrograms.uiProgram, "view", view);

	gl.uniform3fv(renderPrograms.uiProgram, "viewPos", cameraPos);

	glm::vec3 mirroredCameraPos = cameraPos * mirrorVec;

	gl.uniform3fv(renderPrograms.uiProgram, "mirroredViewPos", mirroredCameraPos);

	gl.uniformMatrix4fv(renderPrograms.uiProgram, "mirroredView", mirroredView);

	ViewUpdateData viewUpdateData;

	viewUpdateData.cameraPos = cameraPos;
	viewUpdateData.mirroredCameraPos = mirroredCameraPos;
	viewUpdateData.mirroredView = mirroredView;
	viewUpdateData.view = view;

	return viewUpdateData;
}

//------------CtrlZ------------
bool doCtrlZ;
void ctrlZCheck(GLFWwindow* window,std::vector<aTexture> &albedoTextures,int selectedAlbedoTextureIndex) {
	Texture txtr;
	GlSet glset;

	if ((glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS && doCtrlZ && albedoTextures[selectedAlbedoTextureIndex].undoList.size() != 0) && albedoTextures.size()) { //MAX 20
		//Refresh the screen mask texture (Prevent bugs where might be accur trying undo while in the middle of painting)
		txtr.refreshScreenDrawingTexture();
		
		//Bind the related texture
		//glset.activeTexture(GL_TEXTURE0);
		unsigned int currentTexture = albedoTextures[selectedAlbedoTextureIndex].id;
		glDeleteTextures(1,&currentTexture);

		albedoTextures[selectedAlbedoTextureIndex].id = albedoTextures[selectedAlbedoTextureIndex].undoList[albedoTextures[selectedAlbedoTextureIndex].undoList.size()-1];

		//Remove the last element
		albedoTextures[selectedAlbedoTextureIndex].undoList.pop_back();
		doCtrlZ = false;

	}
	if ((glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_RELEASE || glfwGetKey(window, GLFW_KEY_Z) == GLFW_RELEASE) && albedoTextures.size()) {
		doCtrlZ = true;
	}
}
//------------CtrlZ------------



void Render::exportTexture(bool JPG,bool PNG,const char* exportPath,const char* exportFileName,vector<aTexture> &albedoTextures){

	//Create the export folder
	std::string exportPathStr = exportPath;

	if(exportFileName[0] != '/'){
		exportPathStr.append("/");
	}
	
	exportPathStr.append(exportFileName);

	std::filesystem::create_directories(exportPathStr.c_str());

    
	GlSet gl;
	Texture txtr;

	
	for (size_t i = 0; i < albedoTextures.size(); i++) //Export all the albedo textures
	{
		//Give a number to the texture name
		std::string exportFileNameStr = "";
		exportFileNameStr.append(albedoTextures[i].name);

		//Bind the related texture
		gl.activeTexture(GL_TEXTURE0);
		gl.bindTexture(albedoTextures[i].id);

		//Get the texture array
    	GLubyte* exportTxtr = txtr.getTextureFromProgram(GL_TEXTURE0,1080,1080,3);

		//Export
		if (JPG) {
			txtr.downloadTexture(exportPathStr.c_str(), exportFileNameStr.c_str(), 0, 1080, 1080, exportTxtr, 3);
		}
		else if (PNG) {
			txtr.downloadTexture(exportPathStr.c_str(), exportFileNameStr.c_str(), 1, 1080, 1080, exportTxtr, 3);
		}

		//Delete the array after exporting
    	delete[] exportTxtr;
	}
}

void Render::renderTexture(std::vector<float>& vertices,unsigned int width, unsigned int height,unsigned int texture,unsigned int channels,Model &model,bool useModel,vector<MaterialOut> &modelMaterials,glm::mat4 view,
std::vector<aTexture> albedoTextures,int chosenTextureIndex){
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    GlSet gl;

	unsigned int chosenTxtr = 0;
	if(albedoTextures.size() != 0)
		chosenTxtr = albedoTextures[chosenTextureIndex].id;

	if(useModel)
		model.Draw(currentMaterialIndex,renderPrograms.PBRProgram,false,modelMaterials,view,true,chosenTxtr,glm::vec3(0));
	else
		gl.drawArrays(vertices, false); //Render Model

	GLubyte* renderedTexture = new GLubyte[width * 1080 * 3 * sizeof(GLubyte)];
	glReadPixels(0, 0, width, height, channels, GL_UNSIGNED_BYTE, renderedTexture);

	gl.activeTexture(texture);
	gl.texImage(renderedTexture, width, height, channels);
	gl.generateMipmap();
	delete[]renderedTexture;
}


glm::vec3 getScreenHoverPixel(double mouseXpos,double mouseYpos, int screenSizeY){
    glm::vec3 screenHoverPixel;
	GLubyte* screenPixel = new GLubyte[1 * 1 * 3];//Color val
	glReadPixels(mouseXpos,(int)((double)screenSizeY - mouseYpos), 1, 1, GL_RGB, GL_UNSIGNED_BYTE, screenPixel);//Read the pixel from the framebuffer
	screenHoverPixel.r = screenPixel[0];
	screenHoverPixel.g = screenPixel[1];
	screenHoverPixel.b = screenPixel[2];
	delete[] screenPixel;  
    return screenHoverPixel;
}


int renderDepthCounter = 0;


bool lastRenderSphere = false;
bool lastRenderPlane = false;
bool renderDefault = false;


RenderOutData uiOut;


glm::vec3 screenHoverPixel;

int paintRenderCounter = 0;
RenderOutData Render::render(RenderData &renderData, unsigned int FBOScreen, PanelData &panelData, ExportData &exportData,
Icons &icons,float maskPanelSliderValue,std::vector<unsigned int> &maskTextures, bool renderPlane,bool renderSphere,
PBRShaderData &pbrShaderData,SkyBoxShaderData &skyBoxShaderData,float brushBlurVal,ScreenDepthShaderData &screenDepthShaderData,AxisPointerShaderData &axisPointerShaderData,
OutShaderData &outShaderData,Model &model,vector<aTexture> &albedoTextures,bool paintRender,float materialsPanelSlideValue, std::vector<UIElement> &UIElements, 
ColorPicker &colorPicker,TextureDisplayer &textureDisplayer,Cubemaps cubemaps,ContextMenu &addNodeContextMenu,NodePanel &nodePanel,SndPanel &sndPanel
,int& selectedAlbedoTextureIndex,TextureSelectionPanel &textureSelectionPanel,std::vector<NodeScene>& nodeScenes,int &selectedNodeScene,
std::vector<Node> appNodes,glm::mat4 perspectiveProjection,glm::mat4 view,std::vector<MaterialOut> &modelMaterials,bool &newModelAdded,bool firstClick,
glm::vec3 viewPos,ColoringPanel &coloringPanel,TextureCreatingPanel &txtrCreatingPanel) {
	GlSet gls;
	ColorData colorData;
	Utilities util;

	colorPicker.pickerValue = util.hexToRGBConverter(colorPicker.hexValTextBoxVal);

	if(model.meshes.size()-1 < currentMaterialIndex){
		currentMaterialIndex = 0; 
	}

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //Clear before rendering


	//Get screen and mouse info
	int screenSizeX;
	int screenSizeY;
	glfwGetWindowSize(renderData.window, &screenSizeX, &screenSizeY);
	double mouseXpos;
	double mouseYpos;
	glfwGetCursorPos(renderData.window, &mouseXpos, &mouseYpos);


	//Render depth once painting started
	if (renderData.paintingMode) { 
		renderDepthCounter++;
	}
	else {
		renderDepthCounter = 0;
	}
	if (renderDepthCounter == 1) {//Get depth texture
		getDepthTexture(FBOScreen,screenSizeX,screenSizeY,screenDepthShaderData,model,renderDefault,modelMaterials,renderPrograms,currentMaterialIndex, renderMaxScreenWidth , renderMaxScreenHeight,view,albedoTextures,selectedAlbedoTextureIndex);
	}


	bool isRenderTexture = (renderData.cameraPosChanged && renderData.paintingMode) || exportData.exportImage || (glfwGetMouseButton(renderData.window, 0) == GLFW_RELEASE && renderData.paintingMode); //addImageButtonPressed = albedo texture changed
	
	bool firstPaint = false; //Take the texture to the undo list
	
	if(paintRender)
		paintRenderCounter++;
	if(isRenderTexture)
		paintRenderCounter = 0;
	
	if(paintRenderCounter == 1)
		firstPaint = true;


	if (isRenderTexture || paintRender) {
		renderTextures(FBOScreen,screenSizeX, screenSizeY,outShaderData,model,renderDefault,albedoTextures,false,isRenderTexture,paintRender,firstPaint,currentMaterialIndex,renderPrograms,renderMaxScreenWidth,renderMaxScreenHeight,modelMaterials,view,selectedAlbedoTextureIndex);
		
		//Download enlarged texture
		
	}
	if(exportData.exportImage){
    	exportTexture(UIElements[UIjpgCheckBox].checkBox.checked, UIElements[UIpngCheckBox].checkBox.checked,exportData.path,exportData.fileName,albedoTextures);
	}

	

	glActiveTexture(GL_TEXTURE13);
	glBindTexture(GL_TEXTURE_CUBE_MAP,cubemaps.cubemap);
	renderSkyBox(skyBoxShaderData,renderPrograms);
	


	if(model.meshes.size() != 0){
		if(nodeScenes[selectedNodeScene].stateChanged || newModelAdded){
			modelMaterials[selectedNodeScene] = renderTheNodes(nodeScenes[selectedNodeScene],model,perspectiveProjection,view,renderMaxScreenWidth,screenSizeX,renderMaxScreenHeight,screenSizeY,appNodes);
			nodeScenes[selectedNodeScene].stateChanged = false;
			newModelAdded = false;
		}
	}

	glActiveTexture(GL_TEXTURE13);
	glBindTexture(GL_TEXTURE_CUBE_MAP,cubemaps.blurycubemap);
	renderModel(renderData.backfaceCulling,pbrShaderData,model,renderDefault,modelMaterials,renderPrograms,currentMaterialIndex,view,panelData.paintingPanelActive,albedoTextures,selectedAlbedoTextureIndex,viewPos);
	
	if(renderData.doPainting)
		renderModifiedBrushCursor(renderData.brushSizeIndicator, screenSizeX, screenSizeY, mouseXpos, mouseYpos, colorPicker.pickerValue,renderMaxScreenWidth,renderMaxScreenHeight,renderPrograms);

	renderAxisPointer(axisPointerShaderData,renderPrograms);
	uiOut = renderUi(panelData, renderData, FBOScreen,icons
		,exportData.fileName, maskPanelSliderValue,maskTextures,mouseXpos,mouseYpos,screenSizeX,screenSizeY,
		brushBlurVal,outShaderData,model,albedoTextures,renderPrograms,currentMaterialIndex,renderMaxScreenWidth,
		renderMaxScreenHeight, saturationValShaderData,currentBrushMaskTexture,materialsPanelSlideValue,UIElements,
		colorPicker,textureDisplayer,addNodeContextMenu,nodePanel,sndPanel,selectedAlbedoTextureIndex,textureSelectionPanel,
		nodeScenes,selectedNodeScene,appNodes,newModelAdded,modelMaterials,firstClick,coloringPanel,txtrCreatingPanel);


	colorPicker.updatePickerVal = colorPicker.saturationValueValChanged && !colorPicker.hexValTextBoxGotInput; 
	if (colorPicker.updatePickerVal) { //Get value of color box
		colorPicker.pickerValue = getColorPickerValue(FBOScreen, colorPicker ,screenSizeX, screenSizeY,renderPrograms,renderMaxScreenWidth,renderMaxScreenHeight,saturationValShaderData);
	}
	ctrlZCheck(renderData.window,albedoTextures,selectedAlbedoTextureIndex);



	if(colorPicker.dropperActive || colorPicker.saturationValueBoxHover || colorPicker.hueValueBarHover){
		screenHoverPixel = getScreenHoverPixel(mouseXpos,mouseYpos,screenSizeY);
	}
 
	RenderOutData renderOut;
	renderOut.mouseHoverPixel = screenHoverPixel;
	renderOut.maskPanelMaskClicked = uiOut.maskPanelMaskClicked;
	renderOut.maskPanelMaskHover = uiOut.maskPanelMaskHover;
	renderOut.texturePanelButtonHover = uiOut.texturePanelButtonHover;
	renderOut.texturePanelButtonClicked = uiOut.texturePanelButtonClicked;
	renderOut.currentBrushMaskTxtr = uiOut.currentBrushMaskTxtr;
	
	colorPicker.hexValTextBoxVal = util.rgbToHexGenerator(colorPicker.pickerValue);
	
	return renderOut;
}
void Render::sendProgramsToRender(Programs apprenderPrograms){
	renderPrograms = apprenderPrograms;
}
void Render::sendMaxWindowSize(int maxScreenWidth,int maxScreenHeight){
	renderMaxScreenHeight = maxScreenHeight;
	renderMaxScreenWidth = maxScreenWidth;
}