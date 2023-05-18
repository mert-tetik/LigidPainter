#include<iostream>

#include<glad/glad.h>
#include<GLFW/glfw3.h>

#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <filesystem>

#include "Core/LigidPainter.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

#include "Core/Model/model.h"

#include "Core/Render/Render.h"
#include "Core/UI/UserInterface.h"
#include "Core/Utilities.h"
#include "Core/Renderer.hpp"
#include "Core/gl.h"
#include "Core/Texture/Texture.h"
#include "Core/ProjectFile/WRLigidFile.hpp"
#include "Core/ProjectFile/WRLigidMaterialFile.hpp"
#include "Core/ProjectFile/WRLigidFolder.hpp"


#include "stb_image.h"
#include "stb_image_write.h"

#include "LibAL.h"

#include "tinyfiledialogs.h"

using namespace std;

int renderCurrentMaterialIndex = 0;

Programs renderPrograms;

//--------------------RENDER UI --------------------\\

char* projectPathC = "./Projects";

unsigned int currentBrushMaskTexture;

float createProjectPanelBlurVal = 0.f;

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

	glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)glfwGetVideoMode(glfwGetPrimaryMonitor())->width / (float)glfwGetVideoMode(glfwGetPrimaryMonitor())->height, 0.1f, 1000.0f);
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

	ViewUpdateData viewUpdateData;

	viewUpdateData.cameraPos = cameraPos;
	viewUpdateData.mirroredCameraPos = mirroredCameraPos;
	viewUpdateData.mirroredView = mirroredView;
	viewUpdateData.view = view;

	return viewUpdateData;
}

//------------CtrlZ------------
bool doCtrlZ;
void ctrlZCheck(GLFWwindow* window,std::vector<aTexture> &albedoTextures,int selectedAlbedoTextureIndex,std::vector<NodeScene>& nodeScenes,int &currentNodeScene,std::vector<NodeScene> &nodeScenesHistory,bool paintingPanelActive) {
	Texture txtr;
	GlSet glset;

	if ((glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS && doCtrlZ)) { //MAX 20
		if(nodeScenesHistory.size() && !paintingPanelActive){
			glActiveTexture(GL_TEXTURE0);
			currentNodeScene = nodeScenesHistory[nodeScenesHistory.size()-1].arrayIndex;
			nodeScenes[currentNodeScene] = nodeScenesHistory[nodeScenesHistory.size()-1];
			nodeScenesHistory.erase(nodeScenesHistory.end()-1);
			nodeScenes[currentNodeScene].stateChanged = false;
			glActiveTexture(GL_TEXTURE28);
		}
		
		if(albedoTextures.size() && paintingPanelActive){
			//Refresh the screen mask texture (Prevent bugs where might be accur trying undo while in the middle of painting)
			txtr.refreshScreenDrawingTexture();

			Utilities util;
			util.readTMPFile(albedoTextures[selectedAlbedoTextureIndex],selectedAlbedoTextureIndex,albedoTextures);
		}
		doCtrlZ = false;
	}
	if ((glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_RELEASE || glfwGetKey(window, GLFW_KEY_Z) == GLFW_RELEASE)) {
		doCtrlZ = true;
	}
}
//------------CtrlZ------------



void Render::exportTexture(bool JPG,bool PNG,const char* exportPath,const char* exportFileName,std::vector<aTexture> &albedoTextures,int chosenTextureResIndex){

	//Create the export folder
	std::string exportPathStr = exportPath;

	
	int txtrRes = 256;
	for (size_t i = 0; i < chosenTextureResIndex; i++)
	{
		txtrRes*=2;
	}


	if(exportFileName[0] != '/'){
		exportPathStr.append("/");
	}
	
	exportPathStr.append(exportFileName);

	std::filesystem::create_directories(exportPathStr.c_str());

    
	GlSet gl;
	Texture txtr;

	for (size_t i = 0; i < albedoTextures.size(); i++) //Export all the albedo textures
	{
		if(albedoTextures[i].isTexture && albedoTextures[i].folderIndex == 1){
			//Give a number to the texture name
			std::string exportFileNameStr = "";
			exportFileNameStr.append(albedoTextures[i].name);

			//Bind the related texture
			gl.activeTexture(GL_TEXTURE0);
			gl.bindTexture(albedoTextures[i].id);

			//Get the texture array
    		GLubyte* exportTxtr = txtr.getTextureFromProgram(GL_TEXTURE0,txtrRes,txtrRes,3);

			//Export
			if (JPG) {
				txtr.downloadTexture(exportPathStr.c_str(), exportFileNameStr.c_str(), 0, txtrRes, txtrRes, exportTxtr, 3);
			}
			else if (PNG) {
				txtr.downloadTexture(exportPathStr.c_str(), exportFileNameStr.c_str(), 1, txtrRes, txtrRes, exportTxtr, 3);
			}

			//Delete the array after exporting
    		delete[] exportTxtr;
			glActiveTexture(GL_TEXTURE28);
		}
	}
}

void Render::renderTexture(std::vector<float>& vertices,unsigned int width, unsigned int height,unsigned int texture,unsigned int channels,Model &model,bool useModel,std::vector<MaterialOut> &modelMaterials,glm::mat4 view,
std::vector<aTexture> albedoTextures,int chosenTextureIndex,bool useUV){
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    GlSet gl;

	unsigned int chosenTxtr = 0;
	if(albedoTextures.size() != 0)
		chosenTxtr = albedoTextures[chosenTextureIndex].id;

	if(useModel)
		model.Draw(renderCurrentMaterialIndex,renderPrograms.PBRProgram,false,modelMaterials,view,true,chosenTxtr,glm::vec3(0),0,0,true,{},0,glm::mat4(0),useUV,0);
	else
		gl.drawArrays(vertices, false); //Render Model

	int channelCount = 1;
	if(channels == GL_RGB)
		channelCount = 3;
	if(channels == GL_RGBA)
		channelCount = 4;

	std::vector<GLubyte> renderedTexture(width*height*channelCount);
	glReadPixels(0, 0, width, height, channels, GL_UNSIGNED_BYTE, &renderedTexture[0]);

	gl.activeTexture(texture);
	gl.texImage(&renderedTexture[0], width, height, channels);
	gl.generateMipmap();
}


glm::vec3 getScreenHoverPixel(double mouseXpos,double mouseYpos, int screenSizeY){
    glm::vec3 screenHoverPixel;
	GLubyte screenPixel[1 * 1 * 3];//Color val
	glReadPixels(mouseXpos,(int)((double)screenSizeY - mouseYpos), 1, 1, GL_RGB, GL_UNSIGNED_BYTE, screenPixel);//Read the pixel from the framebuffer
	screenHoverPixel.r = screenPixel[0];
	screenHoverPixel.g = screenPixel[1];
	screenHoverPixel.b = screenPixel[2];
    return screenHoverPixel;
}


int renderDepthCounter = 0;


bool lastRenderSphere = false;
bool lastRenderPlane = false;
bool renderDefault = false;


RenderOutData uiOut;


glm::vec3 screenHoverPixel;

bool lastMaterialStateChanged = false;
NodeScene lastNodeScene;

double renderlastMouseXpos;
double renderlastMouseYpos;

float startMenuNew3DProjectMixVal = 0.f;
float startMenuImportProjectMixVal = 0.f;

int materialDisplayUpdaterCounter = 201;

bool enteredOnce = true;


int paintRenderCounter = 0;

size_t renderingCounter = 0;
float lightRotVal = 0.f;



std::vector<std::string> createProject3DModelPaths;

RenderOutData Render::render(RenderData &renderData, unsigned int FBOScreen, PanelData &panelData, ExportData &exportData,
							Icons &icons, bool renderPlane,bool renderSphere,
							PBRShaderData &pbrShaderData,SkyBoxShaderData &skyBoxShaderData,float brushBlurVal,ScreenDepthShaderData &screenDepthShaderData,AxisPointerShaderData &axisPointerShaderData,
							OutShaderData &outShaderData,Model &model,std::vector<aTexture> &albedoTextures,bool paintRender,float materialsPanelSlideValue, std::vector<UIElement> &UIElements, 
							ColorPicker &colorPicker,TextureDisplayer &textureDisplayer,Cubemaps& cubemaps,ContextMenu &addNodeContextMenu,NodePanel &nodePanel,SndPanel &sndPanel
							,int& selectedAlbedoTextureIndex,TextureSelectionPanel &textureSelectionPanel,std::vector<NodeScene>& nodeScenes,int &selectedNodeScene,
							std::vector<Node> &appNodes,glm::mat4 perspectiveProjection,glm::mat4 view,std::vector<MaterialOut> &modelMaterials,bool &newModelAdded,bool &firstClick,
							glm::vec3 viewPos,ColoringPanel &coloringPanel,TextureCreatingPanel &txtrCreatingPanel,int& chosenTextureResIndex,int &chosenSkyboxTexture,bool& bakeTheMaterial
							,bool& anyTextureNameActive,std::string &textureText,int viewportBGImage,std::vector<NodeScene> &nodeScenesHistory,BrushTexture &brushMaskTextures,bool maskPanelEnter
							,bool &duplicateNodeCall,Objects &objects,int &chosenNodeResIndex,glm::vec3 &drawColor,std::vector<MirrorParam>&mirrorParams,unsigned int &depthTextureID
							,glm::vec3 cameraPos, glm::vec3 originPos,bool &startScreen, std::string &projectPath,aTexture paintOverTexture,Model &spherModel,Audios audios,
							unsigned int materialFBO,int &currentMaterialIndex,bool &textureDraggingState,bool &debugMode,bool &createProject,char* &modelFilePath,std::string &modelName,std::string &customModelName
							,glm::mat4 &modelMatrix,bool &displayProjectFolderManager,std::vector<Font> &fonts,ProjectManager &projectManager,bool firstClickR,unsigned int &generatedTextTxtr,
							Font &txtrGenSelectedFont,Renderer &renderer,float &startScreenScrollVal,float &startScreenLoadPanelScrollVal,bool& starctScCreateProjectMode,bool&  starctScLoadProjectMode
							,int &TDModelsPanelScrollVal) {
	
	renderCurrentMaterialIndex = currentMaterialIndex;
	
	//SETUP
	GlSet gls;
	ColorData colorData;
	Utilities util;
	colorPicker.pickerValue = util.hexToRGBConverter(colorPicker.hexValTextBoxVal);
	if(model.meshes.size()-1 < currentMaterialIndex){
		currentMaterialIndex = 0; 
	}
	glClearColor(colorData.viewportBackColor.r,colorData.viewportBackColor.g,colorData.viewportBackColor.b,1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //Clear before rendering
	//Get screen and mouse info
	int screenSizeX;
	int screenSizeY;
	glfwGetFramebufferSize(renderData.window, &screenSizeX, &screenSizeY);
	double mouseXpos;
	double mouseYpos;
	glfwGetCursorPos(renderData.window, &mouseXpos, &mouseYpos);
	//-------------------------
	float screenGapX = ((float)glfwGetVideoMode(glfwGetPrimaryMonitor())->width - screenSizeX)/(((float)glfwGetVideoMode(glfwGetPrimaryMonitor())->width)/2.0f)/2.0f; 

	//Panel scroll restrictions
	if(UIElements[UIdynamicPaintingCheckBoxElement].checkBox.checked)
		panelData.paintingPanelSlideVal = 0.18;
	else if(panelData.paintingPanelSlideVal > 0.5){
		panelData.paintingPanelSlideVal = 0.5;
	}
	if(panelData.settingsPanelSlideVal > 0.3){
		panelData.settingsPanelSlideVal = 0.3;
	}
	if(panelData.generatorPanelSlideVal > 0.3){
		panelData.generatorPanelSlideVal = 0.3;
	}

	if(!startScreen && !createProject && !displayProjectFolderManager){
		glActiveTexture(GL_TEXTURE9);
		glBindTexture(GL_TEXTURE_2D,depthTextureID);
		glActiveTexture(GL_TEXTURE28);

		//Process Textures
		//Render depth once painting started
		if (renderData.paintingMode) renderDepthCounter++;
		else renderDepthCounter = 0;
		if (renderDepthCounter == 1) {//Get depth texture
			getDepthTexture(FBOScreen,screenSizeX,screenSizeY,screenDepthShaderData,model,renderDefault,modelMaterials,renderPrograms,currentMaterialIndex, glfwGetVideoMode(glfwGetPrimaryMonitor())->width , glfwGetVideoMode(glfwGetPrimaryMonitor())->height,view,albedoTextures,selectedAlbedoTextureIndex,mirrorParams,depthTextureID,cameraPos, originPos,UIElements[UImirrorXRangeBarElement].rangeBar.value*40.f,UIElements[UImirrorYRangeBarElement].rangeBar.value*40.f,UIElements[UImirrorZRangeBarElement].rangeBar.value*40.f,UIElements[UIuseUVCheckBox].checkBox.checked);
		}
		bool isRenderTexture = (renderData.cameraPosChanged && renderData.paintingMode) || exportData.exportImage || (glfwGetMouseButton(renderData.window, 0) == GLFW_RELEASE && renderData.paintingMode); //addImageButtonPressed = albedo texture changed
		if (isRenderTexture || paintRender) {
			renderTextures(FBOScreen,screenSizeX, screenSizeY,pbrShaderData,model,renderDefault,albedoTextures,false,isRenderTexture,paintRender,currentMaterialIndex,renderPrograms,glfwGetVideoMode(glfwGetPrimaryMonitor())->width,glfwGetVideoMode(glfwGetPrimaryMonitor())->height,modelMaterials,view,selectedAlbedoTextureIndex,chosenTextureResIndex,UIElements[UIuseUVCheckBox].checkBox.checked);
		}
		if(exportData.exportImage){
    		exportTexture(UIElements[UIjpgCheckBox].checkBox.checked, UIElements[UIpngCheckBox].checkBox.checked,exportData.path,exportData.fileName,albedoTextures,chosenTextureResIndex);
		}
		//---------------------------


		//Background (skybox or an image)
		glActiveTexture(GL_TEXTURE13);
		glBindTexture(GL_TEXTURE_CUBE_MAP,cubemaps.cubemap);
		glActiveTexture(GL_TEXTURE16);
		glBindTexture(GL_TEXTURE_CUBE_MAP,cubemaps.prefiltered);
		if(UIElements[UIimageCheckBoxElement].checkBox.checked)
			renderBackgroundImage(renderPrograms.renderTheTextureProgram, viewportBGImage);
		if(UIElements[UIskyboxCheckBox].checkBox.checked)
			renderSkyBox(skyBoxShaderData,renderPrograms,UIElements[UIskyBoxExposureRangeBar].rangeBar.value,UIElements[UIskyBoxRotationRangeBar].rangeBar.value);
		//---------------------------------	


		//3D-------------------------
		glActiveTexture(GL_TEXTURE18);
		gls.bindTexture(paintOverTexture.id);
		glUseProgram(renderPrograms.PBRProgram);
		int channelState = 0;
		if(UIElements[UIroughnessChannelCheckBoxElement].checkBox.checked)
			channelState = 1;
		if(UIElements[UImetallicChannelCheckBoxElement].checkBox.checked)
			channelState = 2;
		if(UIElements[UInormalChannelCheckBoxElement].checkBox.checked)
			channelState = 3;
		gls.uniform1i(renderPrograms.PBRProgram,"useUV",UIElements[UIuseUVCheckBox].checkBox.checked);
		gls.uniform1i(renderPrograms.PBRProgram,"dynamicPainting",UIElements[UIdynamicPaintingCheckBoxElement].checkBox.checked);
		gls.uniform1i(renderPrograms.PBRProgram,"useMirror",UIElements[UImirrorXCheckBox].checkBox.checked || UIElements[UImirrorYCheckBox].checkBox.checked || UIElements[UImirrorZCheckBox].checkBox.checked);
		gls.uniform1i(renderPrograms.PBRProgram,"channelState",channelState);
		gls.uniform1f(renderPrograms.PBRProgram,"paintingOpacity",(UIElements[UIbrushOpacityRangeBar].rangeBar.value+0.11f)*4.545454545f);
		gls.uniform1i(renderPrograms.PBRProgram,"paintThrough",(int)UIElements[UIpaintThroughCheckBoxElement].checkBox.checked);
		gls.uniform1i(renderPrograms.PBRProgram,"doPaintOver",(int)UIElements[UIpaintOverCheckBoxElement].checkBox.checked);
		gls.uniform1f(renderPrograms.PBRProgram,"mirrorOriginPosX",UIElements[UImirrorXRangeBarElement].rangeBar.value * 10.f + ((float)!UIElements[UImirrorXCheckBox].checkBox.checked*100000.f));
		gls.uniform1f(renderPrograms.PBRProgram,"mirrorOriginPosY",UIElements[UImirrorYRangeBarElement].rangeBar.value * 10.f + ((float)!UIElements[UImirrorYCheckBox].checkBox.checked*100000.f));
		gls.uniform1f(renderPrograms.PBRProgram,"mirrorOriginPosZ",UIElements[UImirrorZRangeBarElement].rangeBar.value * 10.f + ((float)! UIElements[UImirrorZCheckBox].checkBox.checked*100000.f));
		gls.uniformMatrix4fv(renderPrograms.PBRProgram,"modelMatrix",modelMatrix);
		glUseProgram(renderPrograms.screenDepthProgram);
		gls.uniform1i(renderPrograms.screenDepthProgram,"useUV",UIElements[UIuseUVCheckBox].checkBox.checked);
		gls.uniformMatrix4fv(renderPrograms.screenDepthProgram,"modelMatrix",modelMatrix);
		if(materialDisplayUpdaterCounter > 200){
			//TODO : Update only selected one
			for (size_t i = 0; i < nodeScenes.size(); i++)
			{
				glActiveTexture(GL_TEXTURE28);

				unsigned int dFBO;
				gls.genFramebuffers(dFBO);
				gls.bindFramebuffer(dFBO);
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, nodeScenes[i].renderedTexture, 0);

				unsigned int RBO;
				gls.genRenderbuffers(RBO);
				gls.bindRenderBuffer(RBO);
				glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, 100, 100);
				glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, RBO);


				glClearColor(0.043f,0.635f,0.823f,1);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
				glViewport(0,0,100,100);
				
				glUseProgram(renderPrograms.PBRProgram);
				gls.uniform1f(renderPrograms.PBRProgram,"opacity",1.f);

				glm::mat4 displayMatrix = glm::mat4(0);
				displayMatrix = glm::lookAt(glm::vec3(6.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f));
				
				std::vector<MaterialOut> matDisplays;
				matDisplays.push_back(modelMaterials[i]);

				spherModel.Draw(0,renderPrograms.PBRProgram,false,matDisplays,displayMatrix,false,0,glm::vec3(6.f,0.f,0.f),0,0,false,{},0,glm::mat4(2),0,.0);
				
				gls.bindFramebuffer(0);
				gls.deleteFramebuffers(dFBO);
				glUseProgram(renderPrograms.uiProgram);
				LigidPainter lp;
				lp.setViewportToDefault();

				glDeleteRenderbuffers(1,&RBO);
			}
			
			materialDisplayUpdaterCounter = 0;	
		}
		materialDisplayUpdaterCounter++;


		glUseProgram(renderPrograms.outProgram);
		gls.uniform1i(renderPrograms.outProgram,"dynamicPainting",UIElements[UIdynamicPaintingCheckBoxElement].checkBox.checked);
		gls.uniform1i(renderPrograms.outProgram,"useMirror",UIElements[UImirrorXCheckBox].checkBox.checked || UIElements[UImirrorYCheckBox].checkBox.checked || UIElements[UImirrorZCheckBox].checkBox.checked);
		gls.uniform1f(renderPrograms.outProgram,"paintingOpacity",(UIElements[UIbrushOpacityRangeBar].rangeBar.value+0.11f)*4.545454545f);
		gls.uniform1i(renderPrograms.outProgram,"paintThrough",(int)UIElements[UIpaintThroughCheckBoxElement].checkBox.checked);
		gls.uniform1i(renderPrograms.outProgram,"doPaintOver",(int)UIElements[UIpaintOverCheckBoxElement].checkBox.checked);
		gls.uniformMatrix4fv(renderPrograms.outProgram,"modelMatrix",modelMatrix);

		glActiveTexture(GL_TEXTURE13);
		glBindTexture(GL_TEXTURE_CUBE_MAP,cubemaps.cubemap);
		renderModel(renderData.backfaceCulling,pbrShaderData,model,renderDefault,modelMaterials,renderPrograms,currentMaterialIndex,view,panelData.paintingPanelActive,albedoTextures,selectedAlbedoTextureIndex,
					viewPos,UIElements[UIskyBoxExposureRangeBar].rangeBar.value,UIElements[UIskyBoxRotationRangeBar].rangeBar.value,objects,nodeScenes,modelMatrix,UIElements[UIuseUVCheckBox].checkBox.checked,
					lightRotVal);

		renderAxisPointer(axisPointerShaderData,renderPrograms);
		//-------------------------

		
		//UI
		glClear(GL_DEPTH_BUFFER_BIT);
		if((glfwGetKey(renderData.window,GLFW_KEY_F3) == GLFW_PRESS || (UIElements[UIdisplayPaintOverTextureCheckBoxElement].checkBox.checked && UIElements[UIpaintOverCheckBoxElement].checkBox.checked)) && panelData.paintingPanelActive && (UIElements[UIdynamicPaintingCheckBoxElement].checkBox.checked || UIElements[UImaskPaintingCheckBoxElement].checkBox.checked)){
			glm::mat4 projection;
			projection = glm::ortho(0,1,0,1);
			
			glUseProgram(renderPrograms.outProgram);
			glUniform2f(glGetUniformLocation(renderPrograms.outProgram , "paintOverPos"),UIElements[UIpaintOverPosXRangeBarElement].rangeBar.value, UIElements[UIpaintOverPosYRangeBarElement].rangeBar.value);
			glUniform2f(glGetUniformLocation(renderPrograms.outProgram , "paintOverScale"),UIElements[UIpaintOverScaleXRangeBarElement].rangeBar.value, UIElements[UIpaintOverScaleYRangeBarElement].rangeBar.value);
			glUseProgram(renderPrograms.PBRProgram);
			glUniform2f(glGetUniformLocation(renderPrograms.PBRProgram , "paintOverPos"),UIElements[UIpaintOverPosXRangeBarElement].rangeBar.value, UIElements[UIpaintOverPosYRangeBarElement].rangeBar.value);
			glUniform2f(glGetUniformLocation(renderPrograms.PBRProgram , "paintOverScale"),UIElements[UIpaintOverScaleXRangeBarElement].rangeBar.value, UIElements[UIpaintOverScaleYRangeBarElement].rangeBar.value);
			
			glUseProgram(renderPrograms.paintOverProgram);
			gls.uniformMatrix4fv(renderPrograms.paintOverProgram,"TextProjection",projection);
			gls.uniform1i(renderPrograms.paintOverProgram,"txtr",0);
			gls.uniform1f(renderPrograms.paintOverProgram,"opacity",0.3f);
			glActiveTexture(GL_TEXTURE0);
			gls.bindTexture(paintOverTexture.id);
			glUniform2f(glGetUniformLocation(renderPrograms.paintOverProgram , "pos"),UIElements[UIpaintOverPosXRangeBarElement].rangeBar.value, UIElements[UIpaintOverPosYRangeBarElement].rangeBar.value);
			glUniform2f(glGetUniformLocation(renderPrograms.paintOverProgram , "scale"),UIElements[UIpaintOverScaleXRangeBarElement].rangeBar.value, UIElements[UIpaintOverScaleYRangeBarElement].rangeBar.value);
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
			gls.drawArrays(renderVertices,0);
			double xOffset = mouseXpos - renderlastMouseXpos;
			double yOffset = mouseYpos - renderlastMouseYpos;
			if(glfwGetMouseButton(renderData.window,0) == GLFW_PRESS && glfwGetKey(renderData.window,GLFW_KEY_F3) == GLFW_PRESS){
				if(!glfwGetKey(renderData.window,GLFW_KEY_Y) == GLFW_PRESS)
					UIElements[UIpaintOverPosXRangeBarElement].rangeBar.value -= xOffset/glfwGetVideoMode(glfwGetPrimaryMonitor())->width;
				if(!glfwGetKey(renderData.window,GLFW_KEY_X) == GLFW_PRESS)
					UIElements[UIpaintOverPosYRangeBarElement].rangeBar.value += yOffset/glfwGetVideoMode(glfwGetPrimaryMonitor())->height;
			}
			if(glfwGetMouseButton(renderData.window,1) == GLFW_PRESS && glfwGetKey(renderData.window,GLFW_KEY_F3) == GLFW_PRESS){
				if(!glfwGetKey(renderData.window,GLFW_KEY_Y) == GLFW_PRESS)
				UIElements[UIpaintOverScaleXRangeBarElement].rangeBar.value += yOffset/glfwGetVideoMode(glfwGetPrimaryMonitor())->height;
				if(!glfwGetKey(renderData.window,GLFW_KEY_X) == GLFW_PRESS)
				UIElements[UIpaintOverScaleYRangeBarElement].rangeBar.value += yOffset/glfwGetVideoMode(glfwGetPrimaryMonitor())->height;
			}
			if(glfwGetKey(renderData.window,GLFW_KEY_R) == GLFW_PRESS && glfwGetKey(renderData.window,GLFW_KEY_F3) == GLFW_PRESS){
				UIElements[UIpaintOverPosXRangeBarElement].rangeBar.value = 0;
				UIElements[UIpaintOverPosYRangeBarElement].rangeBar.value = 0;
				UIElements[UIpaintOverScaleXRangeBarElement].rangeBar.value = 1;
				UIElements[UIpaintOverScaleYRangeBarElement].rangeBar.value = 1;
			}
		}

		
		glUseProgram(renderPrograms.uiProgram);
		if(renderData.doPainting){
			glUseProgram(renderPrograms.brushCursor);
			gls.uniform1i(renderPrograms.brushCursor,"drawCircle",UIElements[UIdynamicPaintingCheckBoxElement].checkBox.checked);
			renderModifiedBrushCursor(renderData.brushSizeIndicator, screenSizeX, screenSizeY, mouseXpos, mouseYpos, drawColor,glfwGetVideoMode(glfwGetPrimaryMonitor())->width,glfwGetVideoMode(glfwGetPrimaryMonitor())->height,renderPrograms);
			glUseProgram(renderPrograms.uiProgram);
		}
		if(glfwGetKey(renderData.window,GLFW_KEY_J) == GLFW_RELEASE)
		if(!panelData.paintingPanelActive){
			UIElements[UIfocusModeCheckBox].checkBox.checked = false;
		}
		if(!UIElements[UIfocusModeCheckBox].checkBox.checked){
			uiOut = renderUi(panelData, renderData, FBOScreen,icons
				,exportData.fileName,mouseXpos,mouseYpos,screenSizeX,screenSizeY,
				brushBlurVal,outShaderData,model,albedoTextures,renderPrograms,currentMaterialIndex,glfwGetVideoMode(glfwGetPrimaryMonitor())->width,
				glfwGetVideoMode(glfwGetPrimaryMonitor())->height, saturationValShaderData,currentBrushMaskTexture,materialsPanelSlideValue,UIElements,
				colorPicker,textureDisplayer,addNodeContextMenu,nodePanel,sndPanel,selectedAlbedoTextureIndex,textureSelectionPanel,
				nodeScenes,selectedNodeScene,appNodes,newModelAdded,modelMaterials,firstClick,coloringPanel,txtrCreatingPanel,
				chosenTextureResIndex,chosenSkyboxTexture,bakeTheMaterial,anyTextureNameActive,textureText,nodeScenesHistory
				,brushMaskTextures,maskPanelEnter,duplicateNodeCall,cubemaps,objects,screenHoverPixel,chosenNodeResIndex,audios,textureDraggingState
				,lightRotVal,projectPath,fonts,txtrGenSelectedFont,generatedTextTxtr,TDModelsPanelScrollVal);
				
				UserInterface ui;
				if(colorPicker.dropperActive || coloringPanel.dropperActive){
					//TODO : Use with colorpicker
					glUseProgram(renderPrograms.iconsProgram);
					//ui.iconBox(0.0275f*2.f,0.055f*2.f,(mouseXpos/(screenSizeX/2)-1.f),-mouseYpos/(screenSizeY/2.f) + 1.f,0.999f,icons.O,0.f,colorData.panelColor,glm::vec4(screenHoverPixel,1));
					//ui.iconBox(0.0225f*2.f,0.045f*2.f,(mouseXpos/(screenSizeX/2)-1.f),-mouseYpos/(screenSizeY/2.f) + 1.f,0.9991f,icons.O,0.f,colorData.panelColor,glm::vec4(screenHoverPixel,1));
					glDisable(GL_DEPTH_TEST);
					//TODO : Fix window ratio problem
					ui.iconBox(0.025f*4.f,0.05f*4.f,(mouseXpos/(glfwGetVideoMode(glfwGetPrimaryMonitor())->width/2)-1.f),-mouseYpos/(glfwGetVideoMode(glfwGetPrimaryMonitor())->height/2.f) + 1.f,0.9999f,icons.O,0.f,glm::vec4(screenHoverPixel.r/255.f,screenHoverPixel.g/255.f,screenHoverPixel.b/255.f,1),glm::vec4(screenHoverPixel,1));
					glEnable(GL_DEPTH_TEST);
					glUseProgram(renderPrograms.uiProgram);
				}
		} 
		else{
			renderFocusModeUI(renderPrograms,renderData,UIElements,icons,coloringPanel,saturationValShaderData,mouseXpos,mouseYpos,firstClick,FBOScreen,colorPicker,
								screenHoverPixel,drawColor,outShaderData);
		}
		//-------------------------


		//If nodes are changed send the current nodescene data to nodescene history array
		if(nodeScenes[selectedNodeScene].stateChanged && !lastMaterialStateChanged && lastNodeScene.nodes.size())
    		nodeScenesHistory.push_back(lastNodeScene);
		lastMaterialStateChanged = nodeScenes[selectedNodeScene].stateChanged;
		lastNodeScene = nodeScenes[selectedNodeScene];
		//----------------------

		if(model.meshes.size() != 0){
			//Output the material
			if(UIElements[UIrealtimeMaterialRenderingCheckBox].checkBox.checked){
				if(renderingCounter % nodeScenes.size()/2 == 0){
					for (size_t i = 0; i < nodeScenes.size(); i++)
					{
						modelMaterials[i] = renderTheNodes(nodeScenes[i],model,perspectiveProjection,view,glfwGetVideoMode(glfwGetPrimaryMonitor())->width,screenSizeX,glfwGetVideoMode(glfwGetPrimaryMonitor())->height,screenSizeY,appNodes,chosenTextureResIndex,bakeTheMaterial,albedoTextures,currentMaterialIndex,nodeScenesHistory,chosenNodeResIndex,materialFBO,renderPrograms);
					}
				}
			}
			else{
				if(nodeScenes[selectedNodeScene].stateChanged || newModelAdded){
					modelMaterials[selectedNodeScene] = renderTheNodes(nodeScenes[selectedNodeScene],model,perspectiveProjection,view,glfwGetVideoMode(glfwGetPrimaryMonitor())->width,screenSizeX,glfwGetVideoMode(glfwGetPrimaryMonitor())->height,screenSizeY,appNodes,chosenTextureResIndex,bakeTheMaterial,albedoTextures,currentMaterialIndex,nodeScenesHistory,chosenNodeResIndex,materialFBO,renderPrograms);
					newModelAdded = false;
				}
			}
		}
		nodeScenes[selectedNodeScene].stateChanged = false;


		colorPicker.updatePickerVal = colorPicker.saturationValueValChanged && !colorPicker.hexValTextBoxGotInput; 
		if (colorPicker.updatePickerVal) { //Get value of color box
			colorPicker.pickerValue = getColorPickerValue(FBOScreen, colorPicker ,screenSizeX, screenSizeY,renderPrograms,glfwGetVideoMode(glfwGetPrimaryMonitor())->width,glfwGetVideoMode(glfwGetPrimaryMonitor())->height,saturationValShaderData);
		}

		//CTRLZ
		ctrlZCheck(renderData.window,albedoTextures,selectedAlbedoTextureIndex,nodeScenes,selectedNodeScene,nodeScenesHistory,panelData.paintingPanelActive);


		if(colorPicker.dropperActive || colorPicker.saturationValueBoxHover || colorPicker.hueValueBarHover || coloringPanel.colorBoxHover || coloringPanel.hueBarHover){
			screenHoverPixel = getScreenHoverPixel(mouseXpos,mouseYpos,screenSizeY);
		}

		
	}
	else if(startScreen){
		startScreenPanel(UIElements,renderPrograms,cubemaps,skyBoxShaderData,createProjectPanelBlurVal,projectPath,screenGapX,renderData.window,icons,mouseXpos,mouseYpos,firstClick,
						  displayProjectFolderManager,fonts,projectManager,albedoTextures,1024,nodeScenes,appNodes,addNodeContextMenu,model,firstClickR,renderer,startScreenScrollVal
						  ,startScreen,startScreenLoadPanelScrollVal,chosenSkyboxTexture,nodePanel,starctScCreateProjectMode,starctScLoadProjectMode,chosenTextureResIndex);
		if(!startScreen){
			//Update window
			//glfwSetWindowAspectRatio(renderData.window,1,1);
			glfwSetWindowSizeLimits(renderData.window,glfwGetVideoMode(glfwGetPrimaryMonitor())->width/1.7f,glfwGetVideoMode(glfwGetPrimaryMonitor())->height/4.f,glfwGetVideoMode(glfwGetPrimaryMonitor())->width,glfwGetVideoMode(glfwGetPrimaryMonitor())->height);
			glfwSetWindowPos(renderData.window,0,20);
			glfwSetWindowSize(renderData.window,glfwGetVideoMode(glfwGetPrimaryMonitor())->width,glfwGetVideoMode(glfwGetPrimaryMonitor())->height);

			//Refresh the skybox texture after quiting from the start panel
			Load load;

			std::vector<std::string> faces
			{
			    "LigidPainter/Resources/Cubemap/Skybox/sky"+std::to_string(chosenSkyboxTexture+1)+"/px.png",
			    "LigidPainter/Resources/Cubemap/Skybox/sky"+std::to_string(chosenSkyboxTexture+1)+"/nx.png",
			    "LigidPainter/Resources/Cubemap/Skybox/sky"+std::to_string(chosenSkyboxTexture+1)+"/ny.png",
			    "LigidPainter/Resources/Cubemap/Skybox/sky"+std::to_string(chosenSkyboxTexture+1)+"/py.png",
			    "LigidPainter/Resources/Cubemap/Skybox/sky"+std::to_string(chosenSkyboxTexture+1)+"/pz.png",
			    "LigidPainter/Resources/Cubemap/Skybox/sky"+std::to_string(chosenSkyboxTexture+1)+"/nz.png"
			};

			unsigned int cubemapTexture = load.loadCubemap(faces,GL_TEXTURE13);  
			cubemaps.cubemap = cubemapTexture;
			//TODO Fix loading prefiltered map
			unsigned int prefilteredMap = load.createPrefilterMap(renderPrograms,cubemaps,glfwGetVideoMode(glfwGetPrimaryMonitor())->width,glfwGetVideoMode(glfwGetPrimaryMonitor())->height);
			cubemaps.prefiltered = prefilteredMap;
		}
		// gls.blendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		// glActiveTexture(GL_TEXTURE13);
		// glBindTexture(GL_TEXTURE_CUBE_MAP,cubemaps.cubemap);
		// glActiveTexture(GL_TEXTURE16);
		// glBindTexture(GL_TEXTURE_CUBE_MAP,cubemaps.prefiltered);
		// renderSkyBox(skyBoxShaderData,renderPrograms,UIElements[UIskyBoxExposureRangeBar].rangeBar.value,UIElements[UIskyBoxRotationRangeBar].rangeBar.value);

		// UserInterface ui;
		// glm::mat4 projection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f);
		// glUseProgram(renderPrograms.uiProgram);
		// gls.uniformMatrix4fv(renderPrograms.uiProgram, "TextProjection", projection);
		// glUseProgram(renderPrograms.iconsProgram);
		// gls.uniformMatrix4fv(renderPrograms.iconsProgram, "Projection", projection);
	
		// // float new2DProjectMixVal = 0.f;
		// // if(ui.isMouseOnButton(renderData.window,0.1f,0.3f,-0.4f,0.0f,mouseXpos,mouseYpos,false)){
		// // 	new2DProjectMixVal = 1.f;
		// // }
		// ui.container(-0.4f,0.0f,0.8f,0.1f,0.3f,colorData.buttonColorHover,renderPrograms,icons.Circle,colorData.buttonColorHover,0);
		// glUseProgram(renderPrograms.iconsProgram);
		// ui.iconBox(0.07,0.14f,-0.4f,0.1f,1.f,icons.TwoDProject,0.f,colorData.iconColor,colorData.iconColor);
		// glUseProgram(renderPrograms.uiProgram);
		// ui.renderText(renderPrograms.uiProgram,"New 2D Project",-0.4f-0.06f,-0.2f,0.00022f,colorData.textColor,1.f,false);
		// ui.renderText(renderPrograms.uiProgram,"Will be available",-0.4f-0.07f,-0.3f,0.00022f,glm::vec4(colorData.LigidPainterThemeColor,1),1.f,false);
		// ui.renderText(renderPrograms.uiProgram,"on the next release",-0.42f-0.06f,-0.32f,0.00022f,glm::vec4(colorData.LigidPainterThemeColor,1),1.f,false);

		// float new3DProjectMixVal = 0.f;
		// float importProjectMixVal = 0.f;
		
		// startMenuNew3DProjectMixVal = util.transitionEffect(ui.isMouseOnButton(renderData.window,0.12f,0.3f,0.f-screenGapX,0.0f,mouseXpos,mouseYpos,false),startMenuNew3DProjectMixVal,0.05f);
		// startMenuImportProjectMixVal = util.transitionEffect(ui.isMouseOnButton(renderData.window,0.12f,0.3f,0.4f-screenGapX,0.0f,mouseXpos,mouseYpos,false),startMenuImportProjectMixVal,0.05f);
		
		// if(ui.isMouseOnButton(renderData.window,0.12f,0.3f,0.f-screenGapX,0.0f,mouseXpos,mouseYpos,false)){
		// 	if(enteredOnce){
		// 		LibAL_stopPlaying(audios.ButtonEnter);
		// 		LibAL_playAudioObject(audios.ButtonEnter);
		// 		enteredOnce = false;
		// 	}
			
		// 	nodePanel.pointerCursor = true;
		// 	new3DProjectMixVal = 1.f;
		// 	if(firstClick){
		// 		LibAL_playAudioObject(audios.Login);
		// 		startScreen = false;
		// 		createProject = true;
		// 		if(glfwGetKey(renderData.window,GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS && glfwGetKey(renderData.window,GLFW_KEY_K) == GLFW_PRESS && glfwGetKey(renderData.window,GLFW_KEY_J) == GLFW_PRESS)
		// 			debugMode = true;
		// 	}
		// }
		// else if(ui.isMouseOnButton(renderData.window,0.12f,0.3f,0.4f-screenGapX,0.0f,mouseXpos,mouseYpos,false)){
		// 	if(enteredOnce){
		// 		LibAL_stopPlaying(audios.ButtonEnter);
		// 		LibAL_playAudioObject(audios.ButtonEnter);
		// 		enteredOnce = false;
		// 	}
			
		// 	nodePanel.pointerCursor = true;
		// 	importProjectMixVal = 1.f;
		// 	if(firstClick){
		// 		char const* lFilterPatterns[1] = { "*.ligid" };
		// 		//File dialog
		// 		auto path = tinyfd_openFileDialog("Select LigidPainter Project File", "", 1, lFilterPatterns, "", false);
				
		// 		if(path){
		// 			projectPath = path;
		// 			ProjectFolder projectFolder;
		// 			projectFolder.readFolder(path,nodeScenes,appNodes,addNodeContextMenu,model,UIElements,albedoTextures,fonts);
		// 			LibAL_playAudioObject(audios.Login);
		// 			startScreen = false;
		// 		}
		// 	}
		// }
		// else{
		// 	enteredOnce = true;
		// 	nodePanel.pointerCursor = false;
		// }

		// #if defined(_WIN32) || defined(_WIN64)
		//     char folderDistinguisher = '\\';
		// #else
		// 	char folderDistinguisher = '/'; 
		// #endif

		// int bI = 0;
		// ui.container(0.f,-0.7f,0.5f,0.4f,0.15f,colorData.panelColor,renderPrograms,icons.Circle,glm::vec4(0),0.f);
		// glUseProgram(renderPrograms.uiProgram);

		// ui.renderText(renderPrograms.uiProgram,"From ./Projects",-0.4f,-0.55f,0.00022f,colorData.textColor,0.91f,false);
		
		// ui.renderText(renderPrograms.uiProgram, "Project name" ,-0.4f,-0.59f,0.00022f,colorData.textColor,0.93f,false);
		// ui.renderText(renderPrograms.uiProgram, "Creation Date" ,-0.15f,-0.59f,0.00022f,colorData.textColor,0.93f,false);
		// ui.renderText(renderPrograms.uiProgram, "Last Opening Date" ,0.15f,-0.59f,0.00022f,colorData.textColor,0.93f,false);
		// int projectFileCounter = 0;
		// for (const auto & entry : std::filesystem::directory_iterator("./Projects")){
		// 	if(projectFileCounter == 9) //7
		// 		break;

		// 	std::string fileName = entry.path().string();
		// 	std::string file = util.getLastWordBySeparatingWithChar(fileName,folderDistinguisher);
			
		// 	std::string path = fileName + folderDistinguisher + file + ".ligid";

		// 	//Version 1.4
		// 	uint64_t h1 = 0xAB428C9F; 
        // 	uint64_t h2 = 0xFF8A1C1C; 
        // 	uint64_t h3 = 0x4B4B9AAA; 
        // 	std::ifstream rf(path, std::ios::out | std::ios::binary);
			
		// 	if(rf) {
		// 		std::string Cdt; //Creation date
	    // 		std::string Ldt; //Last opening date
		
		// 		uint64_t c1; 
        // 		uint64_t c2; 
        // 		uint64_t c3; 
        // 		rf.read(reinterpret_cast<char*>(&c1),sizeof(uint64_t));
        // 		rf.read(reinterpret_cast<char*>(&c2),sizeof(uint64_t));
        // 		rf.read(reinterpret_cast<char*>(&c3),sizeof(uint64_t));
		
        // 		if(c1 == h1 && c2 == h2 && c3 == h3){
					
		// 			uint64_t timestrsize;
        //     		rf.read(reinterpret_cast<char*>(&timestrsize),sizeof(uint64_t));
        //     		for (size_t i = 0; i < timestrsize; i++)
        //     		{
		// 				char c;
        //     		    rf.read(reinterpret_cast<char*>(&c),sizeof(char));
		// 				Cdt.push_back(c);
        //     		}
					
		// 			uint64_t timestrsize2;
        //     		rf.read(reinterpret_cast<char*>(&timestrsize2),sizeof(uint64_t));
        //     		for (size_t i = 0; i < timestrsize2; i++)
        //     		{
		// 				char c;
        //     		    rf.read(reinterpret_cast<char*>(&c),sizeof(char));
		// 				Ldt.push_back(c);
        //     		}
		// 			bool buttonHover = ui.isMouseOnButton(renderData.window,0.4f,0.015f,0.f-screenGapX,-0.62f - ((float)bI * 0.03),mouseXpos,mouseYpos,false);
		// 			ui.box(0.4f,0.015f,0.f,-0.62f - ((float)bI * 0.03),util.cropString(file,18),colorData.buttonColor,+0.39f,false,false,0.5f+(buttonHover/10000.f),10,colorData.buttonColorHover,buttonHover);

		// 			ui.renderText(renderPrograms.uiProgram, (std::string)Cdt ,-0.15f,-0.63f - ((float)bI * 0.03),0.00022f,colorData.textColor,0.93f,false);

		// 			ui.renderText(renderPrograms.uiProgram, (std::string)Ldt ,0.15f,-0.63f - ((float)bI * 0.03),0.00022f,colorData.textColor,0.93f,false);

		// 			bI++;
		// 			if(firstClick && buttonHover){
		// 				projectPath = fileName;
		// 				ProjectFolder projectFolder;
		// 				projectFolder.readFolder(fileName + folderDistinguisher + file + ".ligid",nodeScenes,appNodes,addNodeContextMenu,model,UIElements,albedoTextures,fonts);
		// 				LibAL_playAudioObject(audios.Login);
		// 				startScreen = false;
		// 			}
        // 		}
        // 	}
		// 	projectFileCounter++;
		// }



		// ui.container(-0.0f,0.0f,0.8f,0.1f,0.3f,colorData.buttonColor,renderPrograms,icons.Circle,colorData.buttonColorHover,startMenuNew3DProjectMixVal);
		// glUseProgram(renderPrograms.iconsProgram);
		// ui.iconBox(0.07,0.14f,0.0f,0.1f,1.f,icons.ThreeDProject,0.f,colorData.iconColor,colorData.iconColor);
		// glUseProgram(renderPrograms.uiProgram);
		// ui.renderText(renderPrograms.uiProgram,"New 3D Project",0.0f-0.06f,-0.2f,0.00022f,colorData.textColor,1.f,false);
		
		// ui.container(0.4f,0.0f,0.8f,0.1f,0.3f,colorData.buttonColor,renderPrograms,icons.Circle,colorData.buttonColorHover,startMenuImportProjectMixVal);
		// glUseProgram(renderPrograms.iconsProgram);
		// ui.iconBox(0.07,0.14f,0.4f,0.1f,1.f,icons.ImportProject,0.f,colorData.iconColor,colorData.iconColor);
		// glUseProgram(renderPrograms.uiProgram);
		// ui.renderText(renderPrograms.uiProgram,"Import Project",0.4f-0.06f,-0.2f,0.00022f,colorData.textColor,1.f,false);
	}
	else if(createProject){
		UserInterface ui;
		glm::mat4 projection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f);
		glUseProgram(renderPrograms.iconsProgram);
		gls.uniformMatrix4fv(renderPrograms.iconsProgram, "Projection", projection);
		glUseProgram(renderPrograms.uiProgram);
		gls.uniformMatrix4fv(renderPrograms.uiProgram, "TextProjection", projection);
		glUseProgram(renderPrograms.renderTheTextureBlur);
		gls.uniformMatrix4fv(renderPrograms.renderTheTextureBlur, "TextProjection", projection);

		#if defined(_WIN32) || defined(_WIN64)
		    char folderDistinguisher = '\\';
		#else
			char folderDistinguisher = '/'; 
		#endif
		


		//Left panel
		renderBlurySkybox(cubemaps,skyBoxShaderData,renderPrograms,UIElements,createProjectPanelBlurVal,0.2f,1.f,-0.8+screenGapX,0.f,true);
		
		glUseProgram(renderPrograms.iconsProgram);
		bool add3DModelIconHover = false;
		if(ui.isMouseOnButton(renderData.window,0.015f,0.03f,-0.65,0.95f,mouseXpos,mouseYpos,0)){
			add3DModelIconHover = true;
		}
		ui.iconBox(0.015f,0.03f,-0.65 + screenGapX,0.95f,0.95f,icons.Plus,add3DModelIconHover,colorData.iconColor,colorData.iconColorHover);

		if(add3DModelIconHover && firstClick){
			char const* lFilterPatterns[11] = { "*.obj","*.gltf", "*.fbx", "*.stp", "*.max","*.x3d","*.obj","*.vrml","*.3ds","*.stl","*.dae" };
			auto modelFilePathCheck = tinyfd_openFileDialog("Select 3D Model","",11, lFilterPatterns,"",false);
			if(modelFilePathCheck){
				createProject3DModelPaths.push_back(modelFilePathCheck);
			}
		}

		glUseProgram(renderPrograms.uiProgram);
		ui.renderText(renderPrograms.uiProgram,"Add 3D models to your project",-0.98f,0.95f,0.00022f,colorData.buttonColor,0.95f,0);

		for (size_t i = 0; i < createProject3DModelPaths.size(); i++)
		{
			bool tdModelListButtonHover = ui.isMouseOnButton(renderData.window,0.2f,0.04f,-0.8,0.95f - (i+1)*0.08f,mouseXpos,mouseYpos,false);
			ui.box(0.2f,0.04f,-0.8+screenGapX,0.95f - (i+1)*0.08f,createProject3DModelPaths[i],colorData.buttonColor,0.2f,0,0,0.93f,1000,colorData.buttonColorHover,tdModelListButtonHover);
			if(tdModelListButtonHover && firstClick){
				createProject3DModelPaths.erase(createProject3DModelPaths.begin()+i);
				break;
			}
		}
		






		for (size_t i = 0; i < UIElements.size(); i++)
		{
			std::string currentType = UIElements[i].type;
					
			if(UIElements[i].createProject == true){
				glUseProgram(renderPrograms.uiProgram);
	
				if(currentType == "button"){
					ui.box(UIElements[i].button.width, UIElements[i].button.height, UIElements[i].createProjectPos.x, UIElements[i].createProjectPos.y, UIElements[i].button.text, UIElements[i].button.color, UIElements[i].button.textRatio, false, false, UIElements[i].button.positionZ, UIElements[i].button.buttonCurveReduce, UIElements[i].button.colorHover, UIElements[i].button.transitionMixVal); //Add mask texture button
				}
				if(currentType == "text"){	
					ui.renderText(renderPrograms.uiProgram,UIElements[i].text.text, UIElements[i].createProjectPos.x, UIElements[i].createProjectPos.y, UIElements[i].text.scale,colorData.textColor,0.9f,false);
				}
				if(currentType == "rangeBar"){
					if(!UIElements[i].rangeBar.isConstant)
						ui.rangeBar(UIElements[i].createProjectPos.x, UIElements[i].createProjectPos.y, UIElements[i].rangeBar.value,UIElements[i].rangeBar.widthDivider);
					else{
						if(!UIElements[i].rangeBar.isRich)
							ui.constRangeBar(UIElements[i].createProjectPos.x, UIElements[i].createProjectPos.y, UIElements[i].rangeBar.value,icons,UIElements[i].rangeBar.mixVal,UIElements[i].rangeBar.lastVal,UIElements[i].rangeBar.increase);
						else
							ui.richConstRangeBar(UIElements[i].createProjectPos.x, UIElements[i].createProjectPos.y, UIElements[i].rangeBar.value,icons,UIElements[i].rangeBar.mixVal,UIElements[i].rangeBar.lastVal,UIElements[i].rangeBar.increase,UIElements[i].rangeBar.hover,UIElements[i].rangeBar.name);
					}
				}
				if(currentType == "textBox"){
					ui.box(UIElements[i].textBox.width, UIElements[i].textBox.height,UIElements[i].createProjectPos.x, UIElements[i].createProjectPos.y,UIElements[i].textBox.text , colorData.textBoxColor, 0 , true, false, UIElements[i].textBox.position_z, 10 , colorData.textBoxColorClicked, UIElements[i].textBox.transitionMixVal); //Add mask texture button
				}
				if(currentType == "checkBox"){
					ui.checkBox(UIElements[i].createProjectPos.x, UIElements[i].createProjectPos.y, UIElements[i].checkBox.text,  UIElements[i].checkBox.mouseHover,  UIElements[i].checkBox.checked,UIElements[i].checkBox.texture,icons,UIElements[i].checkBox.mixVal); //jpg checkbox
				}
				if(currentType == "icon"){
					glUseProgram(renderPrograms.iconsProgram);
					ui.iconBox(UIElements[i].icon.width,UIElements[i].icon.height,UIElements[i].createProjectPos.x, UIElements[i].createProjectPos.y,UIElements[i].icon.positionZ,UIElements[i].icon.icon, UIElements[i].icon.mixVal , UIElements[i].icon.color , UIElements[i].icon.colorHover);
				}
			}
		}	

		UIElements[UIgenerateTextTextureTextTextBoxElement].textBox.transitionMixVal = util.transitionEffect(UIElements[UIgenerateTextTextureTextTextBoxElement].textBox.clicked,UIElements[UIgenerateTextTextureTextTextBoxElement].textBox.transitionMixVal,0.1f);
		if(ui.isMouseOnButton(renderData.window,0.06f,0.04f,0.f-screenGapX,0.55f,mouseXpos,mouseYpos,false)){
			if(firstClick)
				UIElements[UIgenerateTextTextureTextTextBoxElement].textBox.clicked = !UIElements[UIgenerateTextTextureTextTextBoxElement].textBox.clicked;
		}
		
		if(ui.isMouseOnButton(renderData.window,0.06f,0.04f,0.f-screenGapX,0.3f,mouseXpos,mouseYpos,false)){
			if(firstClick){
				char const* lFilterPatterns[11] = { "*.obj","*.gltf", "*.fbx", "*.stp", "*.max","*.x3d","*.obj","*.vrml","*.3ds","*.stl","*.dae" };
	
				auto modelFilePathCheck = tinyfd_openFileDialog("Select 3D Model","",11, lFilterPatterns,"",false);

				if (modelFilePathCheck) {
					Utilities util;
					modelFilePath = modelFilePathCheck;
					modelName = util.getLastWordBySeparatingWithChar(modelFilePath,folderDistinguisher);
					UIElements[UIUploadingModelPathTextBox].textBox.text = modelName; 
					customModelName = modelName;
				}
			}
		}

		ui.listBox(-0.15f , 0.1f , 0.9f,"Texture Resolution",0.1f,icons,{"256","512","1024","2048","4096"},true,renderData.window,mouseXpos,mouseYpos,firstClick,chosenTextureResIndex,screenGapX);
		bool skyboxlistStateChanged = ui.listBox(0.15f , 0.1f , 0.9f,"Skybox",0.1f,icons,{"1","2","3","4","5","6"},true,renderData.window,mouseXpos,mouseYpos,firstClick,chosenSkyboxTexture,screenGapX);

		if(skyboxlistStateChanged){
			Load load;
			glDeleteTextures(1,&cubemaps.cubemap);
			glDeleteTextures(1,&cubemaps.prefiltered);

			std::vector<std::string> faces
			{
			    "LigidPainter/Resources/Cubemap/Skybox/sky"+std::to_string(chosenSkyboxTexture+1)+"/px.png",
			    "LigidPainter/Resources/Cubemap/Skybox/sky"+std::to_string(chosenSkyboxTexture+1)+"/nx.png",
			    "LigidPainter/Resources/Cubemap/Skybox/sky"+std::to_string(chosenSkyboxTexture+1)+"/ny.png",
			    "LigidPainter/Resources/Cubemap/Skybox/sky"+std::to_string(chosenSkyboxTexture+1)+"/py.png",
			    "LigidPainter/Resources/Cubemap/Skybox/sky"+std::to_string(chosenSkyboxTexture+1)+"/pz.png",
			    "LigidPainter/Resources/Cubemap/Skybox/sky"+std::to_string(chosenSkyboxTexture+1)+"/nz.png"
			};
			unsigned int cubemapTexture = load.loadCubemap(faces,GL_TEXTURE13);  
			cubemaps.cubemap = cubemapTexture;
			unsigned int prefilteredMap = load.createPrefilterMap(renderPrograms,cubemaps,glfwGetVideoMode(glfwGetPrimaryMonitor())->width,glfwGetVideoMode(glfwGetPrimaryMonitor())->height);
			cubemaps.prefiltered = prefilteredMap;
		}


		if(ui.isMouseOnButton(renderData.window,0.015f,0.03f,UIElements[UIbackfaceCullingCheckBox].createProjectPos.x-screenGapX,UIElements[UIbackfaceCullingCheckBox].createProjectPos.y,mouseXpos,mouseYpos,false)){
			UIElements[UIbackfaceCullingCheckBox].checkBox.mouseHover = true;
		}
		else
			UIElements[UIbackfaceCullingCheckBox].checkBox.mouseHover = false;
		
		if(ui.isMouseOnButton(renderData.window,0.015f,0.03f,UIElements[UIuseUVCheckBox].createProjectPos.x-screenGapX,UIElements[UIuseUVCheckBox].createProjectPos.y,mouseXpos,mouseYpos,false)){
			UIElements[UIuseUVCheckBox].checkBox.mouseHover = true;
		}
		else
			UIElements[UIuseUVCheckBox].checkBox.mouseHover = false;

		glUseProgram(renderPrograms.uiProgram);
		
		bool projectPathTextboxHover = ui.isMouseOnButton(renderData.window,0.06f,0.04f,0.f-screenGapX,0.45f,mouseXpos,mouseYpos,false);
		ui.box(0.12f,0.03f,0.f,0.45f, projectPathC ? util.cropString(projectPathC,22) : "Project Path Here" ,colorData.textBoxColor,0.11f,false,false,0.91f,10,colorData.buttonColorHover,projectPathTextboxHover);
		if(projectPathTextboxHover && firstClick){
			projectPathC = tinyfd_selectFolderDialog("Choose a path","./Projects");
			if(!projectPathC)
				projectPathC = "./Projects";
		}
		
		bool createButtonHover = ui.isMouseOnButton(renderData.window,0.06f,0.04f,0.f-screenGapX,-0.55f,mouseXpos,mouseYpos,false);
		ui.box(0.06f,0.04f,0.f,-0.55f,"Create",colorData.buttonColor,0.03f,false,false,0.91f,10,colorData.buttonColorHover,createButtonHover);


		glUseProgram(renderPrograms.iconsProgram);
		bool undoButtonHover = ui.isMouseOnButton(renderData.window,0.02,0.04,-0.3f-screenGapX,0.6,mouseXpos,mouseYpos,false);
		ui.iconBox(0.02,0.04,-0.3f,0.6,0.9f,icons.Undo,undoButtonHover,colorData.iconColor,colorData.iconColorHover);
		if((undoButtonHover && firstClick) || glfwGetKey(renderData.window,GLFW_KEY_ESCAPE) == GLFW_PRESS){
			startScreen = true;
			createProject = false;
		}
		glUseProgram(renderPrograms.uiProgram);

		if(createButtonHover && firstClick){
			if(projectPathC){
				//Main folder
				std::string path = projectPathC;
				path += folderDistinguisher;
				path += UIElements[UIgenerateTextTextureTextTextBoxElement].textBox.text;
				projectPath = path;
				if(std::filesystem::is_directory(path)){
					LigidPainter lp;
					if(lp.ligidMessageBox("There is already a folder with the same name",-0.19f,"Do you want to proceed?",-0.1f)){
						std::filesystem::remove_all(path);
					}
					else{
						goto skipProjectFolder;
					}
				}
				ProjectFolder project;
				//project.initFolder(path,modelFilePath,UIElements,UIElements[UIbackfaceCullingCheckBox].checkBox.checked,UIElements[UIuseUVCheckBox].checkBox.checked,createProject3DModelPaths);

				project.readFolder(path + folderDistinguisher + UIElements[UIgenerateTextTextureTextTextBoxElement].textBox.text + ".ligid",nodeScenes,appNodes,addNodeContextMenu,model,UIElements,
									albedoTextures,fonts,chosenSkyboxTexture,chosenTextureResIndex);

				LigidPainter lp;
				lp.loadModelButton();
		 		createProject = false;

				UIElements[UIbackfaceCullingCheckBox].checkBox.text = "Backface Culling";
				UIElements[UIuseUVCheckBox].checkBox.text = "Auto triangulate";
				UIElements[UIbackfaceCullingCheckBox].checkBox.checked = false;
				UIElements[UIuseUVCheckBox].checkBox.checked = true;
			}
			else{
				LigidPainter lp;
				lp.ligidMessageBox("Project path is not selected",-0.1f,"",0.f);
			}
		} 

	}
	else if(displayProjectFolderManager){		
		int txtrRes = 256;
		for (size_t i = 0; i < chosenTextureResIndex; i++)
		{
			txtrRes*=2;
		}
		projectFolderManagerPanel(UIElements,renderPrograms,cubemaps,skyBoxShaderData,createProjectPanelBlurVal,projectPath,screenGapX,renderData.window,icons,mouseXpos,mouseYpos,firstClick,
								  displayProjectFolderManager,fonts,projectManager,albedoTextures,txtrRes,nodeScenes,appNodes,addNodeContextMenu,model,firstClickR,chosenSkyboxTexture,chosenTextureResIndex);
	}

	glUseProgram(renderPrograms.solidRenderer);
	gls.uniformMatrix4fv(renderPrograms.solidRenderer,"projection",perspectiveProjection);

	createProjectPanelBlurVal = util.transitionEffect(createProject,createProjectPanelBlurVal,0.1f);
	
	skipProjectFolder:
 	renderlastMouseXpos = mouseXpos;
	renderlastMouseYpos = mouseYpos;
	RenderOutData renderOut;
	renderOut.mouseHoverPixel = screenHoverPixel;
	renderOut.maskPanelMaskClicked = uiOut.maskPanelMaskClicked;
	renderOut.maskPanelMaskHover = uiOut.maskPanelMaskHover;
	renderOut.texturePanelButtonHover = uiOut.texturePanelButtonHover;
	renderOut.texturePanelButtonClicked = uiOut.texturePanelButtonClicked;
	renderOut.currentBrushMaskTxtr = uiOut.currentBrushMaskTxtr;
	
	colorPicker.hexValTextBoxVal = util.rgbToHexGenerator(colorPicker.pickerValue);
	
	renderingCounter++;

	return renderOut;
}
void Render::sendProgramsToRender(Programs apprenderPrograms){
	renderPrograms = apprenderPrograms;
}
void Render::sendMaxWindowSize(int maxScreenWidth,int maxScreenHeight){
}