#include<iostream>
#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include <string>
#include <fstream>
#include <sstream>

#include <direct.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

#include "model.h"

#include "Render.h"
#include "LigidPainter.h"
#include "UserInterface.h"
#include "Utilities.h"
#include "gl.h"
#include "Texture.h"
#include <vector>
#include "stb_image.h"
#include "stb_image_write.h"
#include "tinyfiledialogs.h"

struct UndoActions
{
	GLubyte* undoTextures; 
	unsigned int activeMaterial; 
};

std::vector<UndoActions> undoList; 



unsigned int currentMaterialIndex = 0;

Programs renderPrograms;

int renderMaxScreenWidth;
int renderMaxScreenHeight;

//--------------------RENDER UI --------------------\\

bool texturePanelButtonHover = false;

//Button mix val
float exportDownloadButtonMixVal = 0.0f;
float addMaskTextureButtonMixVal = 0.0f;
float loadModelButtonMixVal = 0.0f;
float addPanelButtonMixVal = 0.0f;
float addSphereButtonMixVal = 0.0f;
float addAlbedoTextureMixVal = 0.0f;
float fillBetweenResNumericModifiermixValN = 0.0f;
float fillBetweenResNumericModifiermixValP = 0.0f;
float dropperMixVal = 0.0f;
float exportFileNameTextBoxMixVal = 0.0f;
float hexValTextboxMixVal = 0.0f;
float customModelMixVal = 0.0f;
float texturePanelButtonMixVal = 0.0f;

void updateButtonColorMixValues(UiData uidata) {
	
	float phaseDifference = 0.05f;
	if (uidata.addSphereButtonEnter && addSphereButtonMixVal <= 1.0f) {
		addSphereButtonMixVal += phaseDifference;
	}
	else if (!uidata.addSphereButtonEnter && addSphereButtonMixVal >= 0.0f) {
		addSphereButtonMixVal -= phaseDifference;
	}

	if (uidata.addPlaneButtonEnter && addPanelButtonMixVal <= 1.0f) {
		addPanelButtonMixVal += phaseDifference;
	}
	else if (!uidata.addPlaneButtonEnter && addPanelButtonMixVal >= 0.0f) {
		addPanelButtonMixVal -= phaseDifference;
	}

	if (uidata.loadModelButtonEnter && loadModelButtonMixVal <= 1.0f) {
		loadModelButtonMixVal += phaseDifference;
	}
	else if (!uidata.loadModelButtonEnter && loadModelButtonMixVal >= 0.0f) {
		loadModelButtonMixVal -= phaseDifference;
	}

	if (uidata.addMaskTextureButtonEnter && addMaskTextureButtonMixVal <= 1.0f) {
		addMaskTextureButtonMixVal += phaseDifference;
	}
	else if (!uidata.addMaskTextureButtonEnter && addMaskTextureButtonMixVal >= 0.0f) {
		addMaskTextureButtonMixVal -= phaseDifference;
	}
	
	if (uidata.exportDownloadButtonEnter && exportDownloadButtonMixVal <= 1.0f) {
		exportDownloadButtonMixVal += phaseDifference;
	}
	else if (!uidata.exportDownloadButtonEnter && exportDownloadButtonMixVal >= 0.0f) {
		exportDownloadButtonMixVal -= phaseDifference;
	}

	if (uidata.paintingFillNumericModifierNEnter && fillBetweenResNumericModifiermixValN <= 1.0f) {
		fillBetweenResNumericModifiermixValN += phaseDifference;
	}
	else if (!uidata.paintingFillNumericModifierNEnter && fillBetweenResNumericModifiermixValN >= 0.0f) {
		fillBetweenResNumericModifiermixValN -= phaseDifference;
	}

	if (uidata.paintingFillNumericModifierPEnter && fillBetweenResNumericModifiermixValP <= 1.0f) {
		fillBetweenResNumericModifiermixValP += phaseDifference;
	}
	else if (!uidata.paintingFillNumericModifierPEnter && fillBetweenResNumericModifiermixValP >= 0.0f) {
		fillBetweenResNumericModifiermixValP -= phaseDifference;
	}

	if (uidata.dropperEnter && dropperMixVal <= 1.0f) {
		dropperMixVal += phaseDifference;
	}
	else if (!uidata.dropperEnter && dropperMixVal >= 0.0f) {
		dropperMixVal -= phaseDifference;
	}

	if (uidata.exportFileNameTextBoxPressed && exportFileNameTextBoxMixVal <= 1.0f) {
		exportFileNameTextBoxMixVal += phaseDifference;
	}
	else if (!uidata.exportFileNameTextBoxPressed && exportFileNameTextBoxMixVal >= 0.0f) {
		exportFileNameTextBoxMixVal -= phaseDifference;
	}

	if (uidata.hexValTextboxPressed && hexValTextboxMixVal <= 1.0f) {
		hexValTextboxMixVal += phaseDifference;
	}
	else if (!uidata.hexValTextboxPressed && hexValTextboxMixVal >= 0.0f) {
		hexValTextboxMixVal -= phaseDifference;
	}

	if (uidata.customModelButtonHover && customModelMixVal <= 1.0f) {
		customModelMixVal += phaseDifference;
	}
	else if (!uidata.customModelButtonHover && customModelMixVal >= 0.0f) {
		customModelMixVal -= phaseDifference;
	}

	if (texturePanelButtonHover && texturePanelButtonMixVal <= 1.0f) {
		texturePanelButtonMixVal += phaseDifference;
	}
	else if (!texturePanelButtonHover && texturePanelButtonMixVal >= 0.0f) {
		texturePanelButtonMixVal -= phaseDifference;
	}
}
//Button mix val
float changeTextureDemonstratorWidth = 0.4f;
float changeTextureDemonstratorHeight = 0.8f;

float orgTextureDemonstratorWidth = 0.4f;
float orgTextureDemonstratorHeight = 0.8f;
bool changeTextureDemonstrator;


unsigned int currentBrushMaskTexture;


SaturationValShaderData saturationValShaderData;
glm::vec3 hueVal;		

RenderOutData Render::renderUi(PanelData &panelData,UiData& uidata,RenderData& renderData,unsigned int FBOScreen, float brushBlurRangeBarValue, float brushRotationRangeBarValue, float brushOpacityRangeBarValue, float brushSpacingRangeBarValue,float textureDemonstratorButtonPosX,float textureDemonstratorButtonPosY,bool textureDemonstratorButtonPressClicked,Icons &icons,glm::vec3 colorBoxValue,const char* maskTextureFile,int paintingFillNumericModifierVal,const char* exportFileName,float maskPanelSliderValue,std::vector<unsigned int> &maskTextures,double mouseXpos,double mouseYpos,int screenSizeX,int screenSizeY,std::string &colorpickerHexVal,float brushBorderRangeBarValue,float brushBlurVal,OutShaderData &outShaderData, Model &model,vector<unsigned int> &albedoTextures,bool updateHueVal) {

	
	//---EveryFrame---


	ColorData colorData;
	glm::mat4 projection;
	UserInterface ui;
	Utilities util;
    GlSet gl; 
	RenderOutData uiOut;
	Texture txtr;
	uiOut.maskPanelMaskClicked = false;
	uiOut.maskPanelMaskHover = false;
	uiOut.texturePanelButtonClicked = false;
	uiOut.texturePanelButtonHover = false;

	float screenGapX = ((float)renderMaxScreenWidth - screenSizeX)/(((float)renderMaxScreenWidth)/2.0f)/2.0f; 

	

	//Panel
	if(panelData.exportPanelActive || panelData.modelPanelActive || panelData.paintingPanelActive || panelData.texturePanelActive){ //Disable panel if a message box is active
		//If message box is not active
		ui.panel(renderData.panelLoc-screenGapX , 0);

		//Projection that is used for panel (Use that projection if things will move with panel (and will not be centered) or will be moved freely)
		projection = glm::ortho(0.0f, 2.0f, -1.0f, 1.0f);

		//Panel changing buttons
		glUseProgram(renderPrograms.uiProgram);
		gl.uniformMatrix4fv(renderPrograms.uiProgram, "TextProjection", projection);
		ui.panelChangeButton(renderData.panelLoc - screenGapX, 0.8f);//Model Panel
		ui.panelChangeButton(renderData.panelLoc - screenGapX, 0.72f);//Texture Panel
		ui.panelChangeButton(renderData.panelLoc - screenGapX, 0.64f);//Painting Panel
		ui.panelChangeButton(renderData.panelLoc - screenGapX, 0.56f);//Export Panel

		//Panel changing button's icons
		glUseProgram(renderPrograms.iconsProgram);
		gl.uniformMatrix4fv(renderPrograms.iconsProgram, "Projection", projection);
		ui.iconBox(0.015f,0.02f,renderData.panelLoc-0.01f - screenGapX,0.795f,0.9,icons.TDModel,0,colorData.iconColor,colorData.iconColorHover);
		ui.iconBox(0.015f,0.02f,renderData.panelLoc-0.01f - screenGapX,0.715f,0.9,icons.ImportTexture,0,colorData.iconColor,colorData.iconColorHover);
		ui.iconBox(0.015f,0.02f,renderData.panelLoc-0.01f - screenGapX,0.635f,0.9,icons.Painting,0,colorData.iconColor,colorData.iconColorHover);
		ui.iconBox(0.015f,0.02f,renderData.panelLoc-0.01f - screenGapX, 0.555f,0.9,icons.Export,0,colorData.iconColor,colorData.iconColorHover);

		glUseProgram(renderPrograms.uiProgram);
	}
	else{
		//If message box is active
		projection = glm::ortho(0.0f, 2.0f, -1.0f, 1.0f);
		glUseProgram(renderPrograms.iconsProgram);
		gl.uniformMatrix4fv(renderPrograms.iconsProgram, "Projection", projection);
		glUseProgram(renderPrograms.uiProgram);
		gl.uniformMatrix4fv(renderPrograms.uiProgram, "TextProjection", projection);
	}	

	if (panelData.texturePanelActive) {
		bool mouseEnteredOnce = false;
		glUseProgram(renderPrograms.uiProgram); 

		for (int i = 0; i < model.meshes.size(); i++)//Render buttons
		{ 	
			//Check if mouse is entered the related button
			bool textureButtonEnter = ui.isMouseOnButton(renderData.window, 0.2f, 0.06f, renderData.panelLoc - screenGapX*2 + 0.205f,0.8f - (i * 0.125f), mouseXpos, mouseYpos,true);
			bool textureAddButtonEnter = ui.isMouseOnButton(renderData.window, 0.03f, 0.06f, renderData.panelLoc - screenGapX*2 + 0.3f,0.8f - (i * 0.125f), mouseXpos, mouseYpos,true);
			
			if(textureButtonEnter){
				//Hover
				if(!textureAddButtonEnter){
					mouseEnteredOnce = true;
					texturePanelButtonHover = true;
				}
				uiOut.texturePanelButtonHover = true;
				if(glfwGetMouseButton(renderData.window, 0) == GLFW_PRESS){
					//Pressed
					currentMaterialIndex = i;
					uiOut.texturePanelButtonClicked = true;

					//Bind the related texture
					gl.activeTexture(GL_TEXTURE0);
					gl.bindTexture(albedoTextures[currentMaterialIndex]);
				}

				//Button (Hover)
				ui.box(0.2f, 0.06f, renderData.panelLoc - screenGapX + 0.205f, 0.8f - (i * 0.125f), model.meshes[i].materialName, colorData.buttonColor, 0.048f, true, false, 0.5f, 10000, colorData.buttonColorHover, texturePanelButtonMixVal);

			}
			else{
				//Button 
				ui.box(0.2f, 0.06f, renderData.panelLoc - screenGapX + 0.205f, 0.8f - (i * 0.125f), model.meshes[i].materialName, colorData.buttonColor, 0.048f, true, false, 0.5f, 10000, colorData.buttonColorHover, 0);

			}
			if(textureAddButtonEnter){
				//Hover
				//uiOut.texturePanelButtonHover = true;
				if(glfwGetMouseButton(renderData.window, 0) == GLFW_PRESS){
					//Pressed
					//uiOut.texturePanelButtonClicked = true;
					currentMaterialIndex = i;

					//Bind the related texture
					gl.activeTexture(GL_TEXTURE0);
					gl.bindTexture(albedoTextures[currentMaterialIndex]);

					//Load texture
					Texture txtr;
					//Filters
					char const* lFilterPatterns[2] = { "*.jpg", "*.png" };
					//File dialog
					auto albedoPathCheck = tinyfd_openFileDialog("Select Image", "", 2, lFilterPatterns, "", false);

					if (albedoPathCheck) {
						std::string albedoTexturePath = albedoPathCheck;

						txtr.getTexture(albedoTexturePath,1080,1080,true); //Force albedo's ratio to be 1:1
					}
				}
			}

			ui.box(0.03f, 0.06f, renderData.panelLoc - screenGapX + 0.3f, 0.8f - (i * 0.125f), "", colorData.buttonColorHover, 0.048f, true, false, 0.6f, 10000, colorData.buttonColorHover, 0); 
		}
		if(!mouseEnteredOnce){
			texturePanelButtonHover = false;
		}
	}



	//Texture demonstrator transition animation
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
	//Texture demonstrator
	ui.textureDemonstrator(changeTextureDemonstratorWidth,changeTextureDemonstratorHeight,textureDemonstratorButtonPosX+screenGapX,textureDemonstratorButtonPosY,1.0f); 





	if (panelData.paintingPanelActive){
		//Mirror checkboxes which uses the panel's projection
		glUseProgram(renderPrograms.uiProgram); 
		ui.checkBox(renderData.panelLoc- 0.16f- screenGapX, 0.9f, "X", colorData.checkBoxColor, uidata.mirrorXCheckBoxEnter, uidata.mirrorXCheckBoxPressed); //X mirror checkbox
		ui.checkBox(renderData.panelLoc- 0.10f- screenGapX, 0.9f, "Y", colorData.checkBoxColor, uidata.mirrorYCheckBoxEnter, uidata.mirrorYCheckBoxPressed); //Z mirror checkbox
		ui.checkBox(renderData.panelLoc- 0.04f- screenGapX, 0.9f, "Z", colorData.checkBoxColor, uidata.mirrorZCheckBoxEnter, uidata.mirrorZCheckBoxPressed); //Y mirror checkbox
		glUseProgram(renderPrograms.iconsProgram); 
		ui.iconBox(0.015f,0.02f,renderData.panelLoc - 0.09f- screenGapX,0.95f,0.9,icons.Mirror,0,colorData.iconColor,colorData.iconColorHover);
	}


	//Change the projection
	float centerDivider;
	float centerSum;
	gl.uniform1f(renderPrograms.uiProgram, "uiOpacity", 0.5f);
	if (!panelData.movePanel) {
		//Center the panel
		centerDivider = 2.0f;
		centerSum = 0;
		projection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f);
		glUseProgram(renderPrograms.iconsProgram);
		gl.uniformMatrix4fv(renderPrograms.iconsProgram, "Projection", projection);
		glUseProgram(renderPrograms.uiProgram);
		gl.uniformMatrix4fv(renderPrograms.uiProgram, "TextProjection", projection);
	}
	else {
		//Follow the panel
		centerDivider = 1.0f;
		centerSum = 0.15f;
		projection = glm::ortho(0.0f, 2.0f, -1.0f, 1.0f);
		glUseProgram(renderPrograms.iconsProgram);
		gl.uniformMatrix4fv(renderPrograms.iconsProgram, "Projection", projection);
		glUseProgram(renderPrograms.uiProgram);
		gl.uniformMatrix4fv(renderPrograms.uiProgram, "TextProjection", projection);
	}




	if (panelData.modelPanelActive) { //Other
		glUseProgram(renderPrograms.uiProgram); 
		//File path textbox
		ui.box(0.12f, 0.03f, renderData.panelLoc / centerDivider + centerSum - screenGapX, 0.6f, renderData.modelLoadFilePath, colorData.textBoxColor, 0, true, false, 0.9f, 10, glm::vec3(0), 0);
		ui.renderText(renderPrograms.uiProgram, "File Path", renderData.panelLoc / centerDivider + centerSum - screenGapX - 0.05f, 0.64f, 0.00022f);
		ui.renderText(renderPrograms.uiProgram, ".OBJ only", renderData.panelLoc / centerDivider + centerSum - screenGapX - 0.1f, 0.54f, 0.00022f);

		
		ui.box(0.08f, 0.04f, renderData.panelLoc / centerDivider + centerSum - screenGapX, 0.4f, "Load", colorData.buttonColor, 0.022f, false, false, 0.9f, 10, colorData.buttonColorHover, loadModelButtonMixVal);//Load model button
		

		ui.renderText(renderPrograms.uiProgram, "3D Models", renderData.panelLoc / centerDivider + centerSum - screenGapX - 0.05f, 0.08f-0.1f, 0.00022f);

		ui.box(0.008f, 0.045f, renderData.panelLoc / centerDivider + centerSum - screenGapX, 0.0f-0.1f, "", colorData.buttonColor, 0.047f-0.1f, false, false, 0.9f, 6, colorData.buttonColorHover, addPanelButtonMixVal);//Load a panel button

		ui.box(0.008f, 0.045f, renderData.panelLoc / centerDivider + centerSum - screenGapX, -0.1f-0.1f, "", colorData.buttonColor, 0.055f, false, false, 0.9f, 6, colorData.buttonColorHover, addSphereButtonMixVal);//Load a sphere button

		ui.checkBox(renderData.panelLoc / centerDivider + centerSum - screenGapX - 0.08f, 0.3f, "Auto triangulate", colorData.checkBoxColor, uidata.autoTriangulateCheckBoxEnter, uidata.autoTriangulateCheckBoxPressed); //Auto triangulate checkbox
		ui.checkBox(renderData.panelLoc / centerDivider + centerSum - screenGapX - 0.08f, 0.2f, "Backface culling", colorData.checkBoxColor, uidata.backfaceCullingCheckBoxEnter, uidata.backfaceCullingCheckBoxPressed); //Backface culling checkbox

		ui.box(0.06f, 0.04f, renderData.panelLoc / centerDivider + centerSum - screenGapX, -0.2f-0.1f, "Custom", colorData.buttonColor, 0.032f, false, false, 0.9f, 10, colorData.buttonColorHover, customModelMixVal);//Load custom model button
	}
	if (panelData.modelPanelActive) { //Icons
		glUseProgram(renderPrograms.iconsProgram); 
		//File path textbox
		ui.iconBox(0.020f,0.04f,renderData.panelLoc / centerDivider + centerSum - screenGapX + 0.1f,0.6f,1,icons.Folder,0,colorData.iconColor,colorData.iconColorHover);
		ui.iconBox(0.03f,0.04f,renderData.panelLoc / centerDivider + centerSum - screenGapX,0.0-0.1f,0.99,icons.Panel,0,colorData.iconColor,colorData.iconColorHover);
		ui.iconBox(0.03f,0.04f,renderData.panelLoc / centerDivider + centerSum - screenGapX - 0.002,-0.1f-0.1f,0.99,icons.Sphere,0,colorData.iconColor,colorData.iconColorHover);
	}







    //Texture Panel Was There





	if (panelData.paintingPanelActive) {
		glUseProgram(renderPrograms.uiProgram); 

		ui.box(0.1f, 0.04f, renderData.panelLoc / centerDivider + centerSum - screenGapX, 0.9f, "Add Mask Texture", colorData.buttonColor, 0.075f, false, false, 0.9f, 10, colorData.buttonColorHover, addMaskTextureButtonMixVal); //Add mask texture button
		

		ui.box(0.0f, 0.015f, renderData.panelLoc / centerDivider + centerSum - screenGapX + 0.13f, 0.8f + maskPanelSliderValue, "",colorData.maskPanelSliderColor, 0.095f, false, false, 0.9f, 20, glm::vec3(0), 0); //Mask panel slider


		ui.box(0.008f, 0.125f, renderData.panelLoc / centerDivider + centerSum - screenGapX + 0.13f, 0.675f, "",colorData.maskPanelSliderBackgroundColor, 0.095f, false, false, 0.9f, 10000, glm::vec3(0), 0); //Mask panel slider background
		ui.box(0.0f, 0.015f, renderData.panelLoc / centerDivider + centerSum - screenGapX + 0.13f, 0.8f , "", colorData.maskPanelSliderBackgroundColor, 0.095f, false, false, 0.9f, 20, glm::vec3(0), 0); //Mask panel slider background
		ui.box(0.0f, 0.015f, renderData.panelLoc / centerDivider + centerSum - screenGapX + 0.13f, 0.8f - 0.25, "", colorData.maskPanelSliderBackgroundColor, 0.095f, false, false, 0.9f, 20, glm::vec3(0), 0); //Mask panel slider background

		gl.uniform1f(renderPrograms.uiProgram, "uiOpacity", 0.3f);
		ui.box(0.15f, 0.15f, renderData.panelLoc / centerDivider + centerSum - screenGapX, 0.675f, "", colorData.buttonMaskTxtrPanelColor, 0.095f, false, false, 0.9f, 1000, glm::vec3(0), 0); //Mask panel
		
		ui.checkBox(renderData.panelLoc / centerDivider + centerSum - screenGapX - 0.03f, 0.45f, "Use Negative", colorData.checkBoxColor, uidata.useNegativeForDrawingCheckboxEnter, uidata.useNegativeForDrawingCheckboxPressed); //Auto triangulate checkbox
		gl.uniform1f(renderPrograms.uiProgram, "uiOpacity", 1.0f);
		
		ui.renderText(renderPrograms.uiProgram, maskTextureFile, renderData.panelLoc / centerDivider + centerSum - screenGapX - 0.03f, 0.37f, 0.00022f);
		ui.box(0.035f, 0.07f, renderData.panelLoc / centerDivider + centerSum - screenGapX - 0.1f, 0.42f, "", colorData.buttonColor, 0.075f, false, true, 0.9f, 1000, glm::vec3(0), 0); //Mask texture displayer / GL_TEXTURE12
		

		//Brush size rangebar
		ui.renderText(renderPrograms.uiProgram, "Size", renderData.panelLoc / centerDivider + centerSum - screenGapX - 0.11f, 0.25f+0.06f-0.035f, 0.00022f);
		ui.rangeBar(renderData.panelLoc / centerDivider + centerSum - screenGapX, 0.22f+0.02f+0.02f, renderData.brushSizeRangeBarValue);
		//ui.iconBox(0.03f,0.04f,renderData.panelLoc / centerDivider + centerSum - screenGapX - 0.05f, 0.27f,1,icons.MaskScale);

		//Brush blur rangebar
		ui.renderText(renderPrograms.uiProgram, "Blur", renderData.panelLoc / centerDivider + centerSum - screenGapX - 0.11f, +0.125f+0.08f-0.035f, 0.00022f);
		ui.rangeBar(renderData.panelLoc / centerDivider + centerSum - screenGapX, 0.09f+0.04f+0.02f, brushBlurRangeBarValue);
		//ui.iconBox(0.03f,0.04f,renderData.panelLoc / centerDivider + centerSum - screenGapX - 0.05f, 0.14f,1,icons.MaskGausBlur);

		//Brush rotation rangebar
		ui.renderText(renderPrograms.uiProgram, "Rotation", renderData.panelLoc / centerDivider + centerSum - screenGapX - 0.11f, -0.005f+0.1f-0.035f, 0.00022f);
		ui.rangeBar(renderData.panelLoc / centerDivider + centerSum - screenGapX, -0.04f+0.06f+0.02f, brushRotationRangeBarValue);
		//ui.iconBox(0.03f,0.04f,renderData.panelLoc / centerDivider + centerSum - screenGapX - 0.05f, +0.01f,1,icons.MaskRotation);

		//Brush opacity rangebar
		ui.renderText(renderPrograms.uiProgram, "Opacity", renderData.panelLoc / centerDivider + centerSum - screenGapX - 0.11f, -0.135f+0.12f-0.035f, 0.00022f);
		ui.rangeBar(renderData.panelLoc / centerDivider + centerSum - screenGapX, -0.17f+0.08f+0.02f, brushOpacityRangeBarValue);
		//ui.iconBox(0.03f,0.04f,renderData.panelLoc / centerDivider + centerSum - screenGapX - 0.05f, -0.12f,1,icons.MaskOpacity);

		//Brush spacing rangebar
		ui.renderText(renderPrograms.uiProgram, "Spacing", renderData.panelLoc / centerDivider + centerSum - screenGapX - 0.11f, -0.265f+0.14f-0.035f, 0.00022f);
		ui.rangeBar(renderData.panelLoc / centerDivider + centerSum - screenGapX, -0.30f+0.1f+0.02f, brushSpacingRangeBarValue);
		//ui.iconBox(0.03f,0.04f,renderData.panelLoc / centerDivider + centerSum - screenGapX - 0.05f, -0.25f,1,icons.MaskSpacing);

		//Brush borders rangebar
		ui.renderText(renderPrograms.uiProgram, "Borders", renderData.panelLoc / centerDivider + centerSum - screenGapX - 0.11f, -0.285f + 0.02f, 0.00022f);
		ui.rangeBar(renderData.panelLoc / centerDivider + centerSum - screenGapX, -0.31f+0.02f, brushBorderRangeBarValue);

        ui.numericModifier(renderData.panelLoc / centerDivider + centerSum - screenGapX,-0.37,icons.ArrowLeft,icons.ArrowRight,0.9f, paintingFillNumericModifierVal,fillBetweenResNumericModifiermixValP,fillBetweenResNumericModifiermixValN); //Fill quality

		//Color Picker
		hueVal = ui.colorRect(renderData.panelLoc / centerDivider + centerSum - screenGapX + 0.1f, -0.65f, renderData.colorBoxColorRangeBarValue, FBOScreen, renderData.window,projection,updateHueVal); //Hue TODO : Get the value once value changed 

		saturationValShaderData.boxColor = hueVal / 255.0f;
		saturationValShaderData.renderTextureProjection = projection;

		gl.useSaturationValBoxShader(renderPrograms.saturationValBoxProgram,saturationValShaderData);
		ui.colorBox(renderData.panelLoc / centerDivider + centerSum - screenGapX - 0.02f, -0.65f, renderData.colorBoxPickerValue_x, renderData.colorBoxPickerValue_y);


		gl.uniform1f(renderPrograms.uiProgram, "uiOpacity", 1.0f);
		ui.box(0.002f, 0.025f, renderData.panelLoc / centerDivider + centerSum - screenGapX - 0.095f, -0.91f, "", colorBoxValue / glm::vec3(255), 0.075f, false, false, 0.9f, 10, glm::vec3(0), 0); //indicator for picken color of the color picker
		gl.uniform1f(renderPrograms.uiProgram, "uiOpacity", 0.5f);
		ui.box(0.002f, 0.035f, renderData.panelLoc / centerDivider + centerSum - screenGapX - 0.095f, -0.91f, "", colorData.panelColorSnd, 0.075f, false, false, 0.9f, 7, glm::vec3(0), 0); //decoration

		ui.box(0.04f, 0.03f, renderData.panelLoc / centerDivider + centerSum - screenGapX - 0.008f,-0.91f, util.rgbToHexGenerator(colorBoxValue), colorData.textBoxColor, 0, true, false, 0.9f, 10, colorData.textBoxColorClicked, hexValTextboxMixVal);//Hex val textbox
	}

	if (panelData.paintingPanelActive) { //Icons
		glUseProgram(renderPrograms.iconsProgram); 

		#pragma region brushMaskPanel
		float maskXpos = 0.0f;
		float maskYpos = 0.0f;
		for (size_t i = 0; i < maskTextures.size(); i++)
		{
			if(i % 3 == 0 && i != 0){
				maskYpos-=0.15f;
				maskXpos=0.0f;
			}
			maskXpos-=0.08f;
			float position_x = renderData.panelLoc / centerDivider + centerSum - screenGapX - maskXpos - 0.175f;
			float position_y = 0.8f + maskYpos - maskPanelSliderValue*(maskTextures.size()/4) - 0.05f;
			//ui.iconBox(0.025f, 0.05f,renderData.panelLoc / centerDivider + centerSum - screenGapX - maskXpos - 0.2f,0.8f + maskYpos - maskPanelSliderValue*(maskTextures.size()/4) - 0.05f,1,maskTextures[i],0);
			float upBotDifMin = std::min(0.05f + position_y,0.8f) - std::min(-0.05f + position_y,0.8f);
			float upBotDifMax = std::max(0.05f + position_y,0.55f) - std::max(-0.05f + position_y,0.55f);
			std::vector<float> buttonCoorSq{
				// first trianglev 
				 0.03f + position_x,  std::min(std::max(0.06f + position_y,0.55f),0.8f), 1,1,upBotDifMin*10,0,0,0,  // top right
				 0.03f + position_x,  std::min(std::max(-0.06f + position_y,0.55f),0.8f), 1,1,1.0f-upBotDifMax*10,0,0,0,  // bottom right
				-0.03f + position_x,  std::min(std::max(0.06f + position_y,0.55f),0.8f), 1,0,upBotDifMin*10,0,0,0,  // top left 
				// second triangle						   
				 0.03f + position_x,  std::min(std::max(-0.06f + position_y,0.55f),0.8f), 1,1,1.0f-upBotDifMax*10,0,0,0,  // bottom right
				-0.03f + position_x,  std::min(std::max(-0.06f + position_y,0.55f),0.8f), 1,0,1.0f-upBotDifMax*10,0,0,0,  // bottom left
				-0.03f + position_x,  std::min(std::max(0.06f + position_y,0.55f),0.8f), 1,0,upBotDifMin*10,0,0,0  // top left
			};

			gl.uniform1i(renderPrograms.iconsProgram,"isMaskIcon",1);
			if(maskTextures[i] == currentBrushMaskTexture){
				gl.uniform3fv(renderPrograms.iconsProgram,"iconColor",colorData.chosenBrushMaskTextureColor);
			}
			else{
				gl.uniform3fv(renderPrograms.iconsProgram,"iconColor",colorData.brushMaskIconColor);
			}
			gl.uniform1f(renderPrograms.iconsProgram,"iconMixVal",0);
			gl.activeTexture(GL_TEXTURE6);
			gl.bindTexture(maskTextures[i]);
			gl.drawArrays(buttonCoorSq,false);
			gl.uniform1i(renderPrograms.iconsProgram,"isMaskIcon",0);

			
			if(ui.isMouseOnCoords(renderData.window,mouseXpos+screenGapX*(renderMaxScreenWidth/2),mouseYpos,buttonCoorSq,panelData.movePanel)){
				if(glfwGetMouseButton(renderData.window, 0) == GLFW_PRESS){
					gl.activeTexture(GL_TEXTURE1);
					gl.bindTexture(maskTextures[i]);
					txtr.updateMaskTexture(FBOScreen,screenSizeX,screenSizeY,brushRotationRangeBarValue,false,brushBorderRangeBarValue,brushBlurVal,outShaderData);
					uiOut.maskPanelMaskClicked = true;
					currentBrushMaskTexture = maskTextures[i];
				}
				else{
					uiOut.maskPanelMaskClicked = false;
				}
				uiOut.maskPanelMaskHover = true;
			}
		}
		#pragma endregion brushMaskPanel


		ui.iconBox(0.02f,0.03f,renderData.panelLoc / centerDivider + centerSum - screenGapX + 0.08f, -0.91f,0.9,icons.dropperIcon,dropperMixVal,colorData.iconColor,colorData.iconColorHover);
	}








	if (panelData.exportPanelActive) { //Others
		glUseProgram(renderPrograms.uiProgram); 

		ui.box(0.12f, 0.03f, renderData.panelLoc / centerDivider + centerSum - screenGapX, 0.6f, renderData.exportFolder, colorData.textBoxColor, 0, true, false, 0.9f, 10, glm::vec3(0), 0); //Path textbox
		ui.box(0.12f, 0.03f, renderData.panelLoc / centerDivider + centerSum - screenGapX, 0.5f, exportFileName,colorData.textBoxColor, 0, true, false, 0.9f, 10, colorData.textBoxColorClicked, exportFileNameTextBoxMixVal); //File name textbox

		ui.checkBox(renderData.panelLoc / centerDivider + centerSum - screenGapX - 0.11f, 0.4f, "", colorData.checkBoxColor, uidata.exportExtJPGCheckBoxEnter, uidata.exportExtJPGCheckBoxPressed); //jpg checkbox

		ui.checkBox(renderData.panelLoc / centerDivider + centerSum - screenGapX + 0.05f, 0.4f, "", colorData.checkBoxColor, uidata.exportExtPNGCheckBoxEnter, uidata.exportExtPNGCheckBoxPressed); //png checkbox

		ui.box(0.1f, 0.04f, renderData.panelLoc / centerDivider + centerSum - screenGapX, 0.2f, "Download", colorData.buttonColor, 0.045f, false, false, 0.9f, 10, colorData.buttonColorHover, exportDownloadButtonMixVal); //Download Button
	}
	if (panelData.exportPanelActive) { //Icons
		glUseProgram(renderPrograms.iconsProgram); 

		ui.iconBox(0.05f,0.065f,renderData.panelLoc / centerDivider + centerSum - screenGapX - 0.06f, 0.4f,0.9,icons.JpgFile,0,colorData.iconColor,colorData.iconColorHover);
		ui.iconBox(0.05f,0.065f,renderData.panelLoc / centerDivider + centerSum - screenGapX + 0.1f, 0.4f,0.9,icons.PngFile,0,colorData.iconColor,colorData.iconColorHover);

	}









	projection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f);
	glUseProgram(renderPrograms.iconsProgram);
	gl.uniformMatrix4fv(renderPrograms.iconsProgram, "Projection", projection);
	glUseProgram(renderPrograms.uiProgram);
	gl.uniformMatrix4fv(renderPrograms.uiProgram, "TextProjection", projection);
	uiOut.currentBrushMaskTxtr = currentBrushMaskTexture;
	return uiOut;
}
//--------------------RENDER UI --------------------\\


void Render::renderModel(bool backfaceCulling,PBRShaderData &data,Model &model,bool renderDefault, vector<unsigned int> &albedoTextures) {
	glDepthFunc(GL_LESS); 

    GlSet gl;
	gl.usePBRShader(renderPrograms.PBRProgram,data);

	if (backfaceCulling) { //if backface culling checked in the model panel
		gl.enable(GL_CULL_FACE);
		gl.cullFace(GL_BACK);
	}
	gl.meshDataToShaders();
	
	model.Draw(currentMaterialIndex,renderPrograms.PBRProgram,true,albedoTextures);


	gl.disable(GL_CULL_FACE); //Disable backface culling if enabled

	glUseProgram(renderPrograms.uiProgram);
	glDepthFunc(GL_LESS); 

}
void Render::renderSkyBox(SkyBoxShaderData data) {
	GlSet gls;

	gls.useSkyBoxShader(renderPrograms.skyboxProgram,data);
	
	glDepthMask(GL_FALSE);
	glDepthFunc(GL_LEQUAL);

	std::vector<float> skyboxVertices = {
	    // positions          
	    -1.0f,  1.0f, -1.0f ,0,0,  0,0,0,
	    -1.0f, -1.0f, -1.0f ,0,0,  0,0,0,
	     1.0f, -1.0f, -1.0f ,0,0,  0,0,0,
	     1.0f, -1.0f, -1.0f ,0,0,  0,0,0,
	     1.0f,  1.0f, -1.0f ,0,0,  0,0,0,
	    -1.0f,  1.0f, -1.0f ,0,0,  0,0,0,
	    -1.0f, -1.0f,  1.0f ,0,0,  0,0,0,
	    -1.0f, -1.0f, -1.0f ,0,0,  0,0,0,
	    -1.0f,  1.0f, -1.0f ,0,0,  0,0,0,
	    -1.0f,  1.0f, -1.0f ,0,0,  0,0,0,
	    -1.0f,  1.0f,  1.0f ,0,0,  0,0,0,
	    -1.0f, -1.0f,  1.0f ,0,0,  0,0,0,
	     1.0f, -1.0f, -1.0f ,0,0,  0,0,0,
	     1.0f, -1.0f,  1.0f ,0,0,  0,0,0,
	     1.0f,  1.0f,  1.0f ,0,0,  0,0,0,
	     1.0f,  1.0f,  1.0f ,0,0,  0,0,0,
	     1.0f,  1.0f, -1.0f ,0,0,  0,0,0,
	     1.0f, -1.0f, -1.0f ,0,0,  0,0,0,
	    -1.0f, -1.0f,  1.0f ,0,0,  0,0,0,
	    -1.0f,  1.0f,  1.0f ,0,0,  0,0,0,
	     1.0f,  1.0f,  1.0f ,0,0,  0,0,0,
	     1.0f,  1.0f,  1.0f ,0,0,  0,0,0,
	     1.0f, -1.0f,  1.0f ,0,0,  0,0,0,
	    -1.0f, -1.0f,  1.0f ,0,0,  0,0,0,
	    -1.0f,  1.0f, -1.0f ,0,0,  0,0,0,
	     1.0f,  1.0f, -1.0f ,0,0,  0,0,0,
	     1.0f,  1.0f,  1.0f ,0,0,  0,0,0,
	     1.0f,  1.0f,  1.0f ,0,0,  0,0,0,
	    -1.0f,  1.0f,  1.0f ,0,0,  0,0,0,
	    -1.0f,  1.0f, -1.0f ,0,0,  0,0,0,
	    -1.0f, -1.0f, -1.0f ,0,0,  0,0,0,
	    -1.0f, -1.0f,  1.0f ,0,0,  0,0,0,
	     1.0f, -1.0f, -1.0f ,0,0,  0,0,0,
	     1.0f, -1.0f, -1.0f ,0,0,  0,0,0,
	    -1.0f, -1.0f,  1.0f ,0,0,  0,0,0,
	     1.0f, -1.0f,  1.0f ,0,0,  0,0,0
	};

	gls.drawArrays(skyboxVertices,false);
	glDepthFunc(GL_LESS);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glDepthMask(GL_TRUE);
	
	glUseProgram(renderPrograms.uiProgram);
}
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

void Render::getDepthTexture(unsigned int FBOScreen,  int screenSizeX,  int screenSizeY,ScreenDepthShaderData screenDepthShaderData,Model &model,bool renderDefault,vector<unsigned int> &albedoTextures) {
	Texture txtr;
    GlSet gl;

	gl.viewport(1920, 1080);
	gl.bindFramebuffer(FBOScreen);
	
	screenDepthShaderData.renderMirrored = 0;
	gl.useScreenDepthShader(renderPrograms.screenDepthProgram, screenDepthShaderData);
	

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	model.Draw(currentMaterialIndex,renderPrograms.PBRProgram,false,albedoTextures);

	GLubyte* screen = txtr.getTextureFromProgram(GL_TEXTURE5, 1920, 1080, 3);
	gl.activeTexture(GL_TEXTURE9);
	gl.texImage(screen, 1920, 1080, GL_RGB);
	gl.generateMipmap();
	delete[]screen;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	//Mirrored
	screenDepthShaderData.renderMirrored = 1;
	gl.useScreenDepthShader(renderPrograms.screenDepthProgram, screenDepthShaderData);

	
	model.Draw(currentMaterialIndex,renderPrograms.PBRProgram,false,albedoTextures);

	GLubyte* screenMirrored = txtr.getTextureFromProgram(GL_TEXTURE5, 1920, 1080, 3);
	gl.activeTexture(GL_TEXTURE8);
	gl.texImage(screenMirrored, 1920, 1080, GL_RGB);
	gl.generateMipmap();
	delete[] screenMirrored;

	glUseProgram(renderPrograms.uiProgram);
	
	glViewport(-(renderMaxScreenWidth - screenSizeX)/2, -(renderMaxScreenHeight - screenSizeY), renderMaxScreenWidth, renderMaxScreenHeight);

	gl.bindFramebuffer(0);
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

void drawBrushIndicator(float distanceX,float screenWidth,float screenHeight,float mouseXpos,float mouseYpos,glm::vec3 color) {

	float sizeX = distanceX; //Match the size of the window
	float screenGapX = ((float)renderMaxScreenWidth - (float)screenWidth)/(((float)renderMaxScreenWidth)/2.0f)/2.0f; 
	
	GlSet glset;
	glset.uniform1i(renderPrograms.uiProgram, "drawBrushIndicator", 1); //TODO : Create shader for brush indicator
	std::vector<float> paintingSquare{
		// first triangle
		( sizeX / renderMaxScreenWidth / 1.0f + (float)mouseXpos / renderMaxScreenWidth / 0.5f - 1.0f)+screenGapX,  sizeX / renderMaxScreenHeight / 1.0f - (float)mouseYpos / renderMaxScreenHeight / 0.5f + 1.0f , 1.0f,1,1,0,0,0,  // top right
		( sizeX / renderMaxScreenWidth / 1.0f + (float)mouseXpos / renderMaxScreenWidth / 0.5f - 1.0f)+screenGapX, -sizeX / renderMaxScreenHeight / 1.0f - (float)mouseYpos / renderMaxScreenHeight / 0.5f + 1.0f , 1.0f,1,0,0,0,0,  // bottom right
		(-sizeX / renderMaxScreenWidth / 1.0f + (float)mouseXpos / renderMaxScreenWidth / 0.5f - 1.0f)+screenGapX,  sizeX / renderMaxScreenHeight / 1.0f - (float)mouseYpos / renderMaxScreenHeight / 0.5f + 1.0f , 1.0f,0,1,0,0,0,  // top left 
		( sizeX / renderMaxScreenWidth / 1.0f + (float)mouseXpos / renderMaxScreenWidth / 0.5f - 1.0f)+screenGapX, -sizeX / renderMaxScreenHeight / 1.0f - (float)mouseYpos / renderMaxScreenHeight / 0.5f + 1.0f , 1.0f,1,0,0,0,0,  // bottom right
		(-sizeX / renderMaxScreenWidth / 1.0f + (float)mouseXpos / renderMaxScreenWidth / 0.5f - 1.0f)+screenGapX, -sizeX / renderMaxScreenHeight / 1.0f - (float)mouseYpos / renderMaxScreenHeight / 0.5f + 1.0f , 1.0f,0,0,0,0,0,  // bottom left
		(-sizeX / renderMaxScreenWidth / 1.0f + (float)mouseXpos / renderMaxScreenWidth / 0.5f - 1.0f)+screenGapX,  sizeX / renderMaxScreenHeight / 1.0f - (float)mouseYpos / renderMaxScreenHeight / 0.5f + 1.0f , 1.0f,0,1,0,0,0  // top left
	};
	glset.uiDataToShaders(color/glm::vec3(255.0f));
	glset.uniform1f(renderPrograms.uiProgram, "uiOpacity", 0.2f);
	glset.drawArrays(paintingSquare, false);
	glset.uniform1f(renderPrograms.uiProgram, "uiOpacity", 0.5f);

	glset.uniform1i(renderPrograms.uiProgram, "drawBrushIndicator", 0);
}

void drawAxisPointer(AxisPointerShaderData axisPointerShaderData) {
	GlSet glset;
		//Axis Pointer
		std::vector<float>axisPointer{
		0.0f, -10000.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, //Y
		0.0f, 10000.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,

		-10000.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, //X
		10000.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,

		0.0f, 0.0f, -10000.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, //Z
		0.0f, 0.0f, 10000.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
	};
	glset.useAxisPointerShader(renderPrograms.axisPointerProgram,axisPointerShaderData);
	
	glset.blendFunc(GL_SRC_ALPHA, GL_SRC_ALPHA);
	glset.drawArrays(axisPointer, true);
	glset.blendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glUseProgram(renderPrograms.uiProgram);
}


void Render::exportTexture(bool JPG,bool PNG,const char* exportPath,const char* exportFileName,vector<unsigned int> &albedoTextures){

	//Create the export folder
	std::string exportPathStr = exportPath;
	exportPathStr.append("/LigidExportFolder");
	mkdir(exportPathStr.c_str());
    
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

void Render::renderTextures(unsigned int FBOScreen, bool exportImage, bool JPG, bool PNG, const char* exportPath, int screenSizeX,  int screenSizeY,const char* exportFileName, OutShaderData outShaderData,Model &model,bool renderDefault,vector<unsigned int> &albedoTextures,bool paintOut) {
	int maxHistoryHold = 20;

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
	Texture txtr;

    //Send the texture to the undoTextures vector before updating the texture
	GLubyte* originalImage = txtr.getTextureFromProgram(GL_TEXTURE0, 1080, 1080, 3);
	

	//Send the texture to the undoList before processing the texture (will be used for ctrl z)
	UndoActions undoAct;
	undoAct.activeMaterial = currentMaterialIndex;
	undoAct.undoTextures = originalImage; 
	undoList.push_back(undoAct);


    //Delete the first element from undoList if undoList's count is greated than max history holding value which is 20
	if (undoList.size() > maxHistoryHold){
		//Delete the texture
		GLubyte* undoTexture = undoList[0].undoTextures;
		delete[] undoTexture;

		//Remove the element
		undoList.erase(undoList.begin());
	}

    GlSet gl;
	//Setup
	glm::mat4 projection = glm::ortho(0.0f, 1.77777777778f, 0.0f, 1.0f);
	outShaderData.renderTextureProjection = projection;
	gl.useOutShader(renderPrograms.outProgram, outShaderData);


	gl.uniform1i(renderPrograms.outProgram, "isTwoDimensional", 0);
	gl.viewport(1920, 1080);
	gl.bindFramebuffer(FBOScreen);
	glClearColor(0,0,0,1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//Setup



	gl.uniform1i(renderPrograms.outProgram,"renderPaintedTxtrMask",1);


	//Prevent uv stacking
	model.Draw(currentMaterialIndex,renderPrograms.PBRProgram,false,albedoTextures);

	GLubyte* paintedMask = new GLubyte[1080 * 1080 * 3 * sizeof(GLubyte)];
	glReadPixels(0, 0, 1080, 1080, GL_RGB, GL_UNSIGNED_BYTE, paintedMask);
	gl.activeTexture(GL_TEXTURE11);
	gl.texImage(paintedMask, 1080, 1080, GL_RGB);
	gl.generateMipmap();
	delete[]paintedMask;


	UserInterface ui;
	ui.setViewportBgColor();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	gl.uniform1i(renderPrograms.outProgram,"renderPaintedTxtrMask",0);



	//Render painted image
	model.Draw(currentMaterialIndex,renderPrograms.PBRProgram,false,albedoTextures);

	if (!paintOut)
		gl.drawArrays(renderVertices, false);

	GLubyte* renderedImage = new GLubyte[1080 * 1080 * 3 * sizeof(GLubyte)];
	glReadPixels(0, 0, 1080, 1080, GL_RGB, GL_UNSIGNED_BYTE, renderedImage);
	gl.activeTexture(GL_TEXTURE0);
	gl.texImage(renderedImage, 1080, 1080, GL_RGB);
	gl.generateMipmap();
	delete[]renderedImage;
	//Render painted image

	txtr.refreshScreenDrawingTexture();

	//Render uv mask
	gl.uniform1i(renderPrograms.outProgram, "whiteRendering", 1);
	renderTexture(renderVertices,1080, 1080,GL_TEXTURE7,GL_RGB,model,true,albedoTextures);
	gl.uniform1i(renderPrograms.outProgram, "whiteRendering", 0);
	//Render uv mask

	//interpret the albedo with ui mask texture
	gl.uniform1i(renderPrograms.outProgram, "interpretWithUvMask", 1);
	renderTexture(renderVertices,1080, 1080,GL_TEXTURE0,GL_RGB,model, false,albedoTextures);//Render enlarged texture
	gl.uniform1i(renderPrograms.outProgram, "interpretWithUvMask", 0);
	//interpret the albedo with ui mask texture

	//Download enlarged texture
	if (exportImage) {
        exportTexture(JPG,PNG,exportPath,exportFileName,albedoTextures);
	}

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Finish
	gl.bindFramebuffer(0);
	glViewport(-(renderMaxScreenWidth - screenSizeX)/2, -(renderMaxScreenHeight - screenSizeY), renderMaxScreenWidth, renderMaxScreenHeight);

	//Finish
	glUseProgram(renderPrograms.uiProgram);

}
glm::vec3 getScreenHoverPixel(double mouseXpos,double mouseYpos, int screenSizeY){
    glm::vec3 screenHoverPixel;
	GLubyte* screenPixel = new GLubyte[1 * 1 * 3];//Color val
	glReadPixels(mouseXpos,screenSizeY - mouseYpos, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, screenPixel);//Read the pixel from the framebuffer
	screenHoverPixel.r = screenPixel[0];
	screenHoverPixel.g = screenPixel[1];
	screenHoverPixel.b = screenPixel[2];
	delete[] screenPixel;  
    return screenHoverPixel;
}
glm::vec3 Render::getColorBoxValue(unsigned int FBOScreen,float colorBoxPickerValue_x, float colorBoxPickerValue_y,  int screenSizeX,  int screenSizeY) {
	std::vector<float> colorBox = { //Render color box into the screen
	// first triangle
	 0.0f,  1.0f, 0.0f,1,1,1,1,1,  // top right
	 0.0f,  0.0f, 0.0f,1,0,0,0,0,  // bottom right
	 1.0f,  1.0f, 0.0f,0,1,0,0,0,  // top left 
	// second triangle	  ,0,0,0,
	 0.0f,  0.0f, 0.0f,1,0,0,0,0,  // bottom right7
	 1.0f,  0.0f, 0.0f,0,0,0,0,0,  // bottom left
	 1.0f,  1.0f, 0.0f,0,1,0,0,0   // top left
	};

    GlSet gl;
	glm::mat4 projection = glm::ortho(0.0f, 1.77777777778f, 0.0f, 1.0f);
	saturationValShaderData.renderTextureProjection = projection;
	saturationValShaderData.boxColor = hueVal / 255.0f;
    gl.useSaturationValBoxShader(renderPrograms.saturationValBoxProgram,saturationValShaderData);

	//Setup
	gl.viewport(1920, 1080);
	gl.bindFramebuffer(FBOScreen);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//Setup

	//Render color box
	GLubyte* colorBoxPixel = new GLubyte[1 * 1 * 3];//Color val
	gl.drawArrays(colorBox, false); //Render Model
	glReadPixels(1080 - ((colorBoxPickerValue_x * -1.0f + 0.1f) * 5.0f * 1080), (colorBoxPickerValue_y + 0.2f) * 2.5f * 1080, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, colorBoxPixel);
	//Render color box

	//Finish
	gl.bindFramebuffer(0);
	glViewport(-(renderMaxScreenWidth - screenSizeX)/2, -(renderMaxScreenHeight - screenSizeY), renderMaxScreenWidth, renderMaxScreenHeight);

	glUseProgram(renderPrograms.uiProgram);


	//Get color value to the color vec
    glm::vec3 colorBoxPixelVal = glm::vec3(0);
	colorBoxPixelVal.r = colorBoxPixel[0];
	colorBoxPixelVal.g = colorBoxPixel[1];
	colorBoxPixelVal.b = colorBoxPixel[2];
	delete[]colorBoxPixel;
    return colorBoxPixelVal;
	//Finish
}

int renderDepthCounter = 0;
glm::vec3 colorBoxVal = glm::vec3(0);


bool lastRenderSphere = false;
bool lastRenderPlane = false;
bool renderDefault = false;


RenderOutData uiOut;


glm::vec3 screenHoverPixel;

RenderOutData Render::render(RenderData &renderData, std::vector<float>& vertices, unsigned int FBOScreen, PanelData &panelData, ExportData &exportData,UiData &uidata,float textureDemonstratorButtonPosX,float textureDemonstratorButtonPosY, bool textureDemonstratorButtonPressClicked,float textureDemonstratorWidth, float textureDemonstratorHeight,bool textureDemonstratorBoundariesPressed,Icons &icons,const char* maskTextureFile,int paintingFillNumericModifierVal,float maskPanelSliderValue,std::vector<unsigned int> &maskTextures,std::string &colorpickerHexVal,bool colorpickerHexValTextboxValChanged,bool colorBoxValChanged,bool renderPlane,bool renderSphere,PBRShaderData &pbrShaderData,SkyBoxShaderData &skyBoxShaderData,float brushBlurVal,ScreenDepthShaderData &screenDepthShaderData,AxisPointerShaderData &axisPointerShaderData,OutShaderData &outShaderData,Model &model,vector<unsigned int> &albedoTextures, bool updateHueVal,bool paintingDropperPressed) {
	GlSet gls;
	UserInterface ui;
	ColorData colorData;
	Utilities util;
	Texture txtr;

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
		getDepthTexture(FBOScreen,screenSizeX,screenSizeY,screenDepthShaderData,model,renderDefault,albedoTextures);
	}


	bool isRenderTexture = (renderData.cameraPosChanged && renderData.paintingMode) || exportData.exportImage || uidata.addImageButtonPressed ||(glfwGetMouseButton(renderData.window, 0) == GLFW_RELEASE && renderData.paintingMode); //addImageButtonPressed = albedo texture changed
	if (isRenderTexture) { //colorboxvalchanged has to trigger paintingmode to false
		
		if(exportData.exportImage){
			int lastMaterialIndex = currentMaterialIndex;
			currentMaterialIndex = 0;		
			for (size_t i = 0; i < albedoTextures.size(); i++) //Paint outside of the uv's of the albedo textures in one color before exporting
			{
				//Bind the related texture
				gls.activeTexture(GL_TEXTURE0);
				gls.bindTexture(albedoTextures[i]);

				//Render the texture 	
				renderTextures(FBOScreen, (i == albedoTextures.size()-1) ,uidata.exportExtJPGCheckBoxPressed, uidata.exportExtPNGCheckBoxPressed,exportData.path,screenSizeX, screenSizeY,exportData.fileName,outShaderData,model,renderDefault,albedoTextures,true);

				//Render material by material
				currentMaterialIndex++;
			}

			//Set everything back to normal
			currentMaterialIndex = lastMaterialIndex;
			gls.activeTexture(GL_TEXTURE0);
			gls.bindTexture(albedoTextures[currentMaterialIndex]);

		}
		else
			renderTextures(FBOScreen,exportData.exportImage,uidata.exportExtJPGCheckBoxPressed, uidata.exportExtPNGCheckBoxPressed,exportData.path,screenSizeX, screenSizeY,exportData.fileName,outShaderData,model,renderDefault,albedoTextures,false);
	}


	renderSkyBox(skyBoxShaderData);
	renderModel(renderData.backfaceCulling,pbrShaderData,model,renderDefault,albedoTextures);
	drawAxisPointer(axisPointerShaderData);

	uiOut = renderUi(panelData, uidata, renderData, FBOScreen, renderData.brushBlurRangeBarValue,renderData.brushRotationRangeBarValue, renderData.brushOpacityRangeBarValue, renderData.brushSpacingRangeBarValue,textureDemonstratorButtonPosX,textureDemonstratorButtonPosY,textureDemonstratorButtonPressClicked,icons,colorBoxVal,maskTextureFile,paintingFillNumericModifierVal,exportData.fileName, maskPanelSliderValue,maskTextures,mouseXpos,mouseYpos,screenSizeX,screenSizeY,colorpickerHexVal,renderData.brushBorderRangeBarValue,brushBlurVal,outShaderData,model,albedoTextures,updateHueVal);

	if (colorBoxValChanged && !colorpickerHexValTextboxValChanged) { //Get value of color box
		colorBoxVal = getColorBoxValue(FBOScreen, renderData.colorBoxPickerValue_x, renderData.colorBoxPickerValue_y,screenSizeX, screenSizeY);
	}
	ctrlZCheck(renderData.window,albedoTextures);


	updateButtonColorMixValues(uidata);

	if(paintingDropperPressed)
		screenHoverPixel = getScreenHoverPixel(mouseXpos,mouseYpos,screenSizeY);

	if(renderData.doPainting)
		drawBrushIndicator(renderData.brushSizeIndicator, screenSizeX, screenSizeY, mouseXpos, mouseYpos, colorBoxVal);
 
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