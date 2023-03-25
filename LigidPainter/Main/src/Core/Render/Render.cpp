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
#include "Core/ProjectFile/WRLigidFile.hpp"

#include "stb_image.h"
#include "stb_image_write.h"

#include "tinyfiledialogs.h"




int currentMaterialIndex = 0;

Programs renderPrograms;

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

	//TODO : CTRL Z leads to an error after deleting a material
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
			if(albedoTextures[selectedAlbedoTextureIndex].undoList.size()){
				//Refresh the screen mask texture (Prevent bugs where might be accur trying undo while in the middle of painting)
				txtr.refreshScreenDrawingTexture();

				//Bind the related texture
				//glset.activeTexture(GL_TEXTURE0);
				unsigned int currentTexture = albedoTextures[selectedAlbedoTextureIndex].id;
				for (size_t sceneI = 0; sceneI < nodeScenes.size(); sceneI++)
				{
					for (size_t nodeI = 0; nodeI < nodeScenes[sceneI].nodes.size(); nodeI++)
					{
						for (size_t inI = 0; inI < nodeScenes[sceneI].nodes[nodeI].inputs.size(); inI++)
						{
							if(nodeScenes[sceneI].nodes[nodeI].inputs[inI].selectedTexture == currentTexture){
								nodeScenes[sceneI].nodes[nodeI].inputs[inI].selectedTexture = albedoTextures[selectedAlbedoTextureIndex].undoList[albedoTextures[selectedAlbedoTextureIndex].undoList.size()-1];
							}
						}
					}
				}

				glDeleteTextures(1,&currentTexture);

				albedoTextures[selectedAlbedoTextureIndex].id = albedoTextures[selectedAlbedoTextureIndex].undoList[albedoTextures[selectedAlbedoTextureIndex].undoList.size()-1];

				//Remove the last element
				albedoTextures[selectedAlbedoTextureIndex].undoList.pop_back();
			}
		}
		doCtrlZ = false;
	}
	if ((glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_RELEASE || glfwGetKey(window, GLFW_KEY_Z) == GLFW_RELEASE)) {
		doCtrlZ = true;
	}
}
//------------CtrlZ------------



void Render::exportTexture(bool JPG,bool PNG,const char* exportPath,const char* exportFileName,vector<aTexture> &albedoTextures,int chosenTextureResIndex){

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
		if(albedoTextures[i].isTexture){
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

void Render::renderTexture(std::vector<float>& vertices,unsigned int width, unsigned int height,unsigned int texture,unsigned int channels,Model &model,bool useModel,vector<MaterialOut> &modelMaterials,glm::mat4 view,
std::vector<aTexture> albedoTextures,int chosenTextureIndex){
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    GlSet gl;

	unsigned int chosenTxtr = 0;
	if(albedoTextures.size() != 0)
		chosenTxtr = albedoTextures[chosenTextureIndex].id;

	if(useModel)
		model.Draw(currentMaterialIndex,renderPrograms.PBRProgram,false,modelMaterials,view,true,chosenTxtr,glm::vec3(0),0,0);
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


int paintRenderCounter = 0;
RenderOutData Render::render(RenderData &renderData, unsigned int FBOScreen, PanelData &panelData, ExportData &exportData,
Icons &icons,float &maskPanelSliderValue, bool renderPlane,bool renderSphere,
PBRShaderData &pbrShaderData,SkyBoxShaderData &skyBoxShaderData,float brushBlurVal,ScreenDepthShaderData &screenDepthShaderData,AxisPointerShaderData &axisPointerShaderData,
OutShaderData &outShaderData,Model &model,vector<aTexture> &albedoTextures,bool paintRender,float materialsPanelSlideValue, std::vector<UIElement> &UIElements, 
ColorPicker &colorPicker,TextureDisplayer &textureDisplayer,Cubemaps& cubemaps,ContextMenu &addNodeContextMenu,NodePanel &nodePanel,SndPanel &sndPanel
,int& selectedAlbedoTextureIndex,TextureSelectionPanel &textureSelectionPanel,std::vector<NodeScene>& nodeScenes,int &selectedNodeScene,
std::vector<Node> appNodes,glm::mat4 perspectiveProjection,glm::mat4 view,std::vector<MaterialOut> &modelMaterials,bool &newModelAdded,bool &firstClick,
glm::vec3 viewPos,ColoringPanel &coloringPanel,TextureCreatingPanel &txtrCreatingPanel,int& chosenTextureResIndex,int &chosenSkyboxTexture,bool& bakeTheMaterial
,bool& anyTextureNameActive,std::string &textureText,int viewportBGImage,std::vector<NodeScene> &nodeScenesHistory,BrushTexture &brushMaskTextures,bool maskPanelEnter
,bool &duplicateNodeCall,Objects &objects,int &chosenNodeResIndex,glm::vec3 &drawColor,std::vector<MirrorParam>&mirrorParams,unsigned int &depthTextureID
,glm::vec3 cameraPos, glm::vec3 originPos,bool &startScreen, std::string &projectFilePath) {
	

	
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

	if(!startScreen){
		glActiveTexture(GL_TEXTURE9);
		glBindTexture(GL_TEXTURE_2D,depthTextureID);
		glActiveTexture(GL_TEXTURE28);

		//Process Textures
		//Render depth once painting started
		if (renderData.paintingMode) renderDepthCounter++;
		else renderDepthCounter = 0;
		if (renderDepthCounter == 1) {//Get depth texture
			getDepthTexture(FBOScreen,screenSizeX,screenSizeY,screenDepthShaderData,model,renderDefault,modelMaterials,renderPrograms,currentMaterialIndex, glfwGetVideoMode(glfwGetPrimaryMonitor())->width , glfwGetVideoMode(glfwGetPrimaryMonitor())->height,view,albedoTextures,selectedAlbedoTextureIndex,mirrorParams,depthTextureID,cameraPos, originPos,UIElements[UImirrorXRangeBarElement].rangeBar.value*40.f,UIElements[UImirrorYRangeBarElement].rangeBar.value*40.f,UIElements[UImirrorZRangeBarElement].rangeBar.value*40.f);
		}
		bool isRenderTexture = (renderData.cameraPosChanged && renderData.paintingMode) || exportData.exportImage || (glfwGetMouseButton(renderData.window, 0) == GLFW_RELEASE && renderData.paintingMode); //addImageButtonPressed = albedo texture changed
		if (isRenderTexture || paintRender) {
			renderTextures(FBOScreen,screenSizeX, screenSizeY,outShaderData,model,renderDefault,albedoTextures,false,isRenderTexture,paintRender,currentMaterialIndex,renderPrograms,glfwGetVideoMode(glfwGetPrimaryMonitor())->width,glfwGetVideoMode(glfwGetPrimaryMonitor())->height,modelMaterials,view,selectedAlbedoTextureIndex,chosenTextureResIndex);
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
		glActiveTexture(GL_TEXTURE13);
		glBindTexture(GL_TEXTURE_CUBE_MAP,cubemaps.blurycubemap);
		glUseProgram(renderPrograms.PBRProgram);
		gls.uniform1i(renderPrograms.PBRProgram,"paintThrough",(int)UIElements[UIpaintThroughCheckBoxElement].checkBox.checked);
		gls.uniform1f(renderPrograms.PBRProgram,"mirrorOriginPosX",UIElements[UImirrorXRangeBarElement].rangeBar.value * 10.f + ((float)!UIElements[UImirrorXCheckBox].checkBox.checked*100000.f));
		gls.uniform1f(renderPrograms.PBRProgram,"mirrorOriginPosY",UIElements[UImirrorYRangeBarElement].rangeBar.value * 10.f + ((float)!UIElements[UImirrorYCheckBox].checkBox.checked*100000.f));
		gls.uniform1f(renderPrograms.PBRProgram,"mirrorOriginPosZ",UIElements[UImirrorZRangeBarElement].rangeBar.value * 10.f + ((float)! UIElements[UImirrorZCheckBox].checkBox.checked*100000.f));

		glUseProgram(renderPrograms.outProgram);
		gls.uniform1i(renderPrograms.outProgram,"paintThrough",(int)UIElements[UIpaintThroughCheckBoxElement].checkBox.checked);
	
		renderModel(renderData.backfaceCulling,pbrShaderData,model,renderDefault,modelMaterials,renderPrograms,currentMaterialIndex,view,panelData.paintingPanelActive,albedoTextures,selectedAlbedoTextureIndex,viewPos,UIElements[UIskyBoxExposureRangeBar].rangeBar.value,UIElements[UIskyBoxRotationRangeBar].rangeBar.value,objects);

		renderAxisPointer(axisPointerShaderData,renderPrograms);
		//-------------------------


		//UI
		glClear(GL_DEPTH_BUFFER_BIT);
		if(renderData.doPainting)
			renderModifiedBrushCursor(renderData.brushSizeIndicator, screenSizeX, screenSizeY, mouseXpos, mouseYpos, drawColor,glfwGetVideoMode(glfwGetPrimaryMonitor())->width,glfwGetVideoMode(glfwGetPrimaryMonitor())->height,renderPrograms);
		if(glfwGetKey(renderData.window,GLFW_KEY_J) == GLFW_RELEASE)
		if(!panelData.paintingPanelActive){
			UIElements[UIfocusModeCheckBox].checkBox.checked = false;
		}
		if(!UIElements[UIfocusModeCheckBox].checkBox.checked){
			uiOut = renderUi(panelData, renderData, FBOScreen,icons
				,exportData.fileName, maskPanelSliderValue,mouseXpos,mouseYpos,screenSizeX,screenSizeY,
				brushBlurVal,outShaderData,model,albedoTextures,renderPrograms,currentMaterialIndex,glfwGetVideoMode(glfwGetPrimaryMonitor())->width,
				glfwGetVideoMode(glfwGetPrimaryMonitor())->height, saturationValShaderData,currentBrushMaskTexture,materialsPanelSlideValue,UIElements,
				colorPicker,textureDisplayer,addNodeContextMenu,nodePanel,sndPanel,selectedAlbedoTextureIndex,textureSelectionPanel,
				nodeScenes,selectedNodeScene,appNodes,newModelAdded,modelMaterials,firstClick,coloringPanel,txtrCreatingPanel,
				chosenTextureResIndex,chosenSkyboxTexture,bakeTheMaterial,anyTextureNameActive,textureText,nodeScenesHistory
				,brushMaskTextures,maskPanelEnter,duplicateNodeCall,cubemaps,objects,screenHoverPixel,chosenNodeResIndex);
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
			if(nodeScenes[selectedNodeScene].stateChanged || newModelAdded){
				modelMaterials[selectedNodeScene] = renderTheNodes(nodeScenes[selectedNodeScene],model,perspectiveProjection,view,glfwGetVideoMode(glfwGetPrimaryMonitor())->width,screenSizeX,glfwGetVideoMode(glfwGetPrimaryMonitor())->height,screenSizeY,appNodes,chosenTextureResIndex,bakeTheMaterial,albedoTextures,currentMaterialIndex,nodeScenesHistory,chosenNodeResIndex);
				newModelAdded = false;
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

		UserInterface ui;
		glUseProgram(renderPrograms.uiProgram);
		ui.renderText(renderPrograms.uiProgram,"Save",-0.05f,0.95f,0.00022f,colorData.textColor,0.9f,false);
		//TODO : Screengapx
		if(ui.isMouseOnButton(renderData.window,0.025f,0.02f,-0.05f,0.95f,mouseXpos,mouseYpos,false)){
			nodePanel.pointerCursor = true;
			if(firstClick){
				char* projectFilePathCheck;
				if(projectFilePath == ""){
					//File dialog
					char const* lFilterPatterns[1] = { "*.ligid" };
					projectFilePathCheck = tinyfd_saveFileDialog("Save The Project","", 1, lFilterPatterns, "");
				}

				if(projectFilePath != ""){
					LigidFile ligidFile;
					ligidFile.writeTheFile(projectFilePath.c_str(),model,albedoTextures,nodeScenes);
				}
				else if(projectFilePathCheck){
					LigidFile ligidFile;
					projectFilePath = projectFilePathCheck;
					ligidFile.writeTheFile(projectFilePath.c_str(),model,albedoTextures,nodeScenes);
				}
			}
		}
		if(ui.isMouseOnButton(renderData.window,0.025f,0.02f,0.05f,0.95f,mouseXpos,mouseYpos,false)){
			nodePanel.pointerCursor = true;
			if(firstClick){
				if(tinyfd_messageBox("Warning!","Another projected will be loaded. Unsaved data will be lost.","okcancel","warning",0)){
					char const* lFilterPatterns[1] = { "*.ligid" };
					//File dialog
					auto path = tinyfd_openFileDialog("Select LigidPainter Project File", "", 1, lFilterPatterns, "", false);

					if(path){
						LigidFile ligidFile;
						ligidFile.readTheFile(path,model,albedoTextures);
						projectFilePath = path;
						startScreen = false;
					}
				}
			}
		}
		ui.renderText(renderPrograms.uiProgram,"Load",0.05f,0.95f,0.00022f,colorData.textColor,0.9f,false);
	}
	else{
		gls.blendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glActiveTexture(GL_TEXTURE13);
		glBindTexture(GL_TEXTURE_CUBE_MAP,cubemaps.cubemap);
		glActiveTexture(GL_TEXTURE16);
		glBindTexture(GL_TEXTURE_CUBE_MAP,cubemaps.prefiltered);
		renderSkyBox(skyBoxShaderData,renderPrograms,UIElements[UIskyBoxExposureRangeBar].rangeBar.value,UIElements[UIskyBoxRotationRangeBar].rangeBar.value);

		UserInterface ui;
		glm::mat4 projection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f);
		glUseProgram(renderPrograms.uiProgram);
		gls.uniformMatrix4fv(renderPrograms.uiProgram, "TextProjection", projection);
		glUseProgram(renderPrograms.iconsProgram);
		gls.uniformMatrix4fv(renderPrograms.iconsProgram, "Projection", projection);
	
		float new2DProjectMixVal = 0.f;
		if(ui.isMouseOnButton(renderData.window,0.1f,0.3f,-0.4f,0.0f,mouseXpos,mouseYpos,false)){
			new2DProjectMixVal = 1.f;
		}
		ui.container(-0.4f,0.0f,0.8f,0.1f,0.3f,colorData.buttonColor,renderPrograms,icons.Circle,colorData.buttonColorHover,new2DProjectMixVal);
		glUseProgram(renderPrograms.iconsProgram);
		ui.iconBox(0.07,0.14f,-0.4f,0.1f,1.f,icons.Texture,0.f,colorData.iconColor,colorData.iconColor);
		glUseProgram(renderPrograms.uiProgram);
		ui.renderText(renderPrograms.uiProgram,"New 2D Project",-0.4f-0.06f,-0.2f,0.00022f,colorData.textColor,1.f,false);

		float new3DProjectMixVal = 0.f;
		if(ui.isMouseOnButton(renderData.window,0.1f,0.3f,0.f,0.0f,mouseXpos,mouseYpos,false)){
			new3DProjectMixVal = 1.f;
			if(firstClick)
				startScreen = false;
		}
		ui.container(-0.0f,0.0f,0.8f,0.1f,0.3f,colorData.buttonColor,renderPrograms,icons.Circle,colorData.buttonColorHover,new3DProjectMixVal);
		glUseProgram(renderPrograms.iconsProgram);
		ui.iconBox(0.07,0.14f,0.0f,0.1f,1.f,icons.TDModel,0.f,colorData.iconColor,colorData.iconColor);
		glUseProgram(renderPrograms.uiProgram);
		ui.renderText(renderPrograms.uiProgram,"New 3D Project",0.0f-0.06f,-0.2f,0.00022f,colorData.textColor,1.f,false);
		
		float importProjectMixVal = 0.f;
		if(ui.isMouseOnButton(renderData.window,0.1f,0.3f,0.4f,0.0f,mouseXpos,mouseYpos,false)){
			importProjectMixVal = 1.f;
			if(firstClick){
				char const* lFilterPatterns[1] = { "*.ligid" };
				//File dialog
				auto path = tinyfd_openFileDialog("Select LigidPainter Project File", "", 1, lFilterPatterns, "", false);
				
				if(path){
					LigidFile ligidFile;
					ligidFile.readTheFile(path,model,albedoTextures);
					projectFilePath = path;
					startScreen = false;
				}
			}
		}
		ui.container(0.4f,0.0f,0.8f,0.1f,0.3f,colorData.buttonColor,renderPrograms,icons.Circle,colorData.buttonColorHover,importProjectMixVal);
		glUseProgram(renderPrograms.iconsProgram);
		ui.iconBox(0.07,0.14f,0.4f,0.1f,1.f,icons.ImportModel,0.f,colorData.iconColor,colorData.iconColor);
		glUseProgram(renderPrograms.uiProgram);
		ui.renderText(renderPrograms.uiProgram,"Import Project",0.4f-0.06f,-0.2f,0.00022f,colorData.textColor,1.f,false);
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
}