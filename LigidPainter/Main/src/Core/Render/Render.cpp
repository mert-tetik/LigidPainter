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



std::vector<UndoActions> undoList; 



int currentMaterialIndex = 0;

Programs renderPrograms;

int renderMaxScreenWidth;
int renderMaxScreenHeight;

//--------------------RENDER UI --------------------\\






float orgTextureDemonstratorWidth = 0.4f;
float orgTextureDemonstratorHeight = 0.8f;


unsigned int currentBrushMaskTexture;


SaturationValShaderData saturationValShaderData;
glm::vec3 hueVal;		





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
void Render::getUnprojection(glm::vec3 vPos, glm::vec3 cameraPos, glm::vec3 originPos) { //Not used
	/*GlSet glset;
	ProjectionData pd = glset.setMatrices(cameraPos, originPos);
	return glm::project(vPos, pd.modelMat * pd.viewMat, pd.projMat, glm::vec4(0, 0, 1920, 1080));*/
}

void Render::drawLightObject(glm::vec3 lightPos) {
	// glm::mat4 model = glm::mat4(1.0f);
	// model = glm::mat4(1.0f);
	// model = glm::translate(model, lightPos);
	// model = glm::scale(model, glm::vec3(0.2f));

	// int lightColorLoc = glGetUniformLocation(renderPrograms.uiProgram, "lightColor");
	// glUniform3f(lightColorLoc, 10.0f, 10.0f, 10.0f);
	// int isLightSourceLoc = glGetUniformLocation(renderPrograms.uiProgram, "isLightSource");
	// glUniform1i(isLightSourceLoc, 1);
	// int modelLoc = glGetUniformLocation(renderPrograms.uiProgram, "model");
	// glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	// glDrawArrays(GL_TRIANGLES, 0, 36);
}



//------------CtrlZ------------
bool doCtrlZ;
void ctrlZCheck(GLFWwindow* window,std::vector<unsigned int> &albedoTextures) {
	Texture txtr;
	GlSet glset;

	

	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS && doCtrlZ && undoList.size() != 0) { //MAX 20
		
		//Refresh the screen mask texture (Prevent bugs where might be accur trying undo while in the middle of painting)
		txtr.refreshScreenDrawingTexture();
		
		//Bind the related texture
		glset.activeTexture(GL_TEXTURE0);
		currentMaterialIndex = undoList[undoList.size() - 1].activeMaterial;
		glset.bindTexture(albedoTextures[currentMaterialIndex]);

		//Change the texture to the last texture
		glset.texImage(undoList[undoList.size() - 1].undoTextures, 1080, 1080, GL_RGB);
		glset.generateMipmap();

		//Delete the texture
		GLubyte* undoTexture = undoList[undoList.size() - 1].undoTextures;
		delete[] undoTexture;

		//Remove the last element
		undoList.pop_back();
		doCtrlZ = false;
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_RELEASE || glfwGetKey(window, GLFW_KEY_Z) == GLFW_RELEASE) {
		doCtrlZ = true   ;
	}
}
//------------CtrlZ------------



void Render::exportTexture(bool JPG,bool PNG,const char* exportPath,const char* exportFileName,vector<unsigned int> &albedoTextures){

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
		std::string exportFileNameStr = exportFileName;
		exportFileNameStr.append("_Albedo_");
		exportFileNameStr.append(to_string(i));

		//Bind the related texture
		gl.activeTexture(GL_TEXTURE0);
		gl.bindTexture(albedoTextures[i]);

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

void Render::renderTexture(std::vector<float>& vertices,unsigned int width, unsigned int height,unsigned int texture,unsigned int channels,Model &model,bool useModel,vector<unsigned int> &albedoTextures){
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    GlSet gl;

	if(useModel)
		model.Draw(currentMaterialIndex,renderPrograms.PBRProgram,false,albedoTextures);
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
glm::vec3 colorBoxVal = glm::vec3(0);


bool lastRenderSphere = false;
bool lastRenderPlane = false;
bool renderDefault = false;


RenderOutData uiOut;


glm::vec3 screenHoverPixel;

int paintRenderCounter = 0;
RenderOutData Render::render(RenderData &renderData, std::vector<float>& vertices, unsigned int FBOScreen, PanelData &panelData, ExportData &exportData,UiData &uidata,float textureDemonstratorButtonPosX,float textureDemonstratorButtonPosY, bool textureDemonstratorButtonPressClicked,float textureDemonstratorWidth, float textureDemonstratorHeight,bool textureDemonstratorBoundariesPressed,Icons &icons,const char* maskTextureFile,float maskPanelSliderValue,std::vector<unsigned int> &maskTextures,std::string &colorpickerHexVal,bool colorpickerHexValTextboxValChanged,bool colorBoxValChanged,bool renderPlane,bool renderSphere,PBRShaderData &pbrShaderData,SkyBoxShaderData &skyBoxShaderData,float brushBlurVal,ScreenDepthShaderData &screenDepthShaderData,AxisPointerShaderData &axisPointerShaderData,OutShaderData &outShaderData,Model &model,vector<unsigned int> &albedoTextures, bool updateHueVal,bool paintingDropperPressed, bool paintRender,bool colorBoxEnter,bool hueBarEnter,float materialsPanelSlideValue, std::vector<UIElement> &UIElements) {
	GlSet gls;
	ColorData colorData;
	Utilities util;

	colorBoxVal = util.hexToRGBConverter(colorpickerHexVal);

	if(model.meshes.size()-1 < currentMaterialIndex){
		currentMaterialIndex = 0; 
	}
	
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


	//Render depth once painting started
	if (renderData.paintingMode) { 
		renderDepthCounter++;
	}
	else {
		renderDepthCounter = 0;
	}
	if (renderDepthCounter == 1) {//Get depth texture
		getDepthTexture(FBOScreen,screenSizeX,screenSizeY,screenDepthShaderData,model,renderDefault,albedoTextures,renderPrograms,currentMaterialIndex, renderMaxScreenWidth , renderMaxScreenHeight);
	}


	bool isRenderTexture = (renderData.cameraPosChanged && renderData.paintingMode) || exportData.exportImage || uidata.addImageButtonPressed ||(glfwGetMouseButton(renderData.window, 0) == GLFW_RELEASE && renderData.paintingMode); //addImageButtonPressed = albedo texture changed
	
	bool firstPaint = false; //Take the texture to the undo list
	
	if(paintRender)
		paintRenderCounter++;
	if(isRenderTexture)
		paintRenderCounter = 0;
	
	if(paintRenderCounter == 1)
		firstPaint = true;


	if (isRenderTexture || paintRender) { //colorboxvalchanged has to trigger paintingmode to false
		
		if(exportData.exportImage){
			int lastMaterialIndex = currentMaterialIndex;
			currentMaterialIndex = 0;		
			for (size_t i = 0; i < albedoTextures.size(); i++) //Paint outside of the uv's of the albedo textures in one color before exporting
			{
				//Bind the related texture
				gls.activeTexture(GL_TEXTURE0);
				gls.bindTexture(albedoTextures[i]);

				//Render the texture 	
				renderTextures(FBOScreen, (i == albedoTextures.size()-1) ,uidata.exportExtJPGCheckBoxPressed, uidata.exportExtPNGCheckBoxPressed,exportData.path,screenSizeX, screenSizeY,exportData.fileName,outShaderData,model,renderDefault,albedoTextures,true,isRenderTexture,paintRender,firstPaint,currentMaterialIndex,undoList,renderPrograms,renderMaxScreenWidth,renderMaxScreenHeight);

				//Render material by material
				currentMaterialIndex++;
			}

			//Set everything back to normal
			currentMaterialIndex = lastMaterialIndex;
			gls.activeTexture(GL_TEXTURE0);
			gls.bindTexture(albedoTextures[currentMaterialIndex]);

		}
		else
			renderTextures(FBOScreen,exportData.exportImage,uidata.exportExtJPGCheckBoxPressed, uidata.exportExtPNGCheckBoxPressed,exportData.path,screenSizeX, screenSizeY,exportData.fileName,outShaderData,model,renderDefault,albedoTextures,false,isRenderTexture,paintRender,firstPaint,currentMaterialIndex,undoList,renderPrograms,renderMaxScreenWidth,renderMaxScreenHeight);
	}


	renderSkyBox(skyBoxShaderData,renderPrograms);
	renderModel(renderData.backfaceCulling,pbrShaderData,model,renderDefault,albedoTextures,renderPrograms,currentMaterialIndex);
	renderAxisPointer(axisPointerShaderData,renderPrograms);
		uiOut = renderUi(panelData, uidata, renderData, FBOScreen, renderData.brushBlurValue,renderData.brushRotationValue, renderData.brushOpacityValue
		, renderData.brushSpacingValue,textureDemonstratorButtonPosX,textureDemonstratorButtonPosY,textureDemonstratorButtonPressClicked,icons,colorBoxVal,maskTextureFile
		,exportData.fileName, maskPanelSliderValue,maskTextures,mouseXpos,mouseYpos,screenSizeX,screenSizeY,colorpickerHexVal,
		renderData.brushBorderValue,brushBlurVal,outShaderData,model,albedoTextures,updateHueVal,renderPrograms,currentMaterialIndex,renderMaxScreenWidth,
		renderMaxScreenHeight,orgTextureDemonstratorWidth, orgTextureDemonstratorHeight, saturationValShaderData,hueVal,currentBrushMaskTexture,materialsPanelSlideValue,UIElements);


	if (colorBoxValChanged && !colorpickerHexValTextboxValChanged) { //Get value of color box
		colorBoxVal = getColorBoxValue(FBOScreen, renderData.colorBoxPickerValue_x, renderData.colorBoxPickerValue_y,screenSizeX, screenSizeY,hueVal,renderPrograms,renderMaxScreenWidth,renderMaxScreenHeight,saturationValShaderData);
	}
	ctrlZCheck(renderData.window,albedoTextures);



	if(paintingDropperPressed || colorBoxEnter || hueBarEnter){
		screenHoverPixel = getScreenHoverPixel(mouseXpos,mouseYpos,screenSizeY);
	}

	if(renderData.doPainting)
		renderModifiedBrushCursor(renderData.brushSizeIndicator, screenSizeX, screenSizeY, mouseXpos, mouseYpos, colorBoxVal,renderMaxScreenWidth,renderMaxScreenHeight,renderPrograms);
 
	RenderOutData renderOut;
	renderOut.mouseHoverPixel = screenHoverPixel;
	renderOut.maskPanelMaskClicked = uiOut.maskPanelMaskClicked;
	renderOut.maskPanelMaskHover = uiOut.maskPanelMaskHover;
	renderOut.texturePanelButtonHover = uiOut.texturePanelButtonHover;
	renderOut.texturePanelButtonClicked = uiOut.texturePanelButtonClicked;
	renderOut.currentBrushMaskTxtr = uiOut.currentBrushMaskTxtr;
	renderOut.colorpickerHexVal = util.rgbToHexGenerator(colorBoxVal);
	renderOut.colorBoxVal = colorBoxVal;
	return renderOut;
}
void Render::sendProgramsToRender(Programs apprenderPrograms){
	renderPrograms = apprenderPrograms;
}
void Render::sendMaxWindowSize(int maxScreenWidth,int maxScreenHeight){
	renderMaxScreenHeight = maxScreenHeight;
	renderMaxScreenWidth = maxScreenWidth;
}