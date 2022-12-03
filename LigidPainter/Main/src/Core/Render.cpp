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
#include "Render.h"
#include "LigidPainter.h"
#include "UserInterface.h"
#include "Utilities.h"
#include "gl.h"
#include "Texture.h"
#include <vector>
#include "stb_image.h"
#include "stb_image_write.h"

Programs renderPrograms;

//--------------------RENDER UI --------------------\\
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

	if (uidata.addImageButtonEnter && addAlbedoTextureMixVal <= 1.0f) {
		addAlbedoTextureMixVal += phaseDifference;
	}
	else if (!uidata.addImageButtonEnter && addAlbedoTextureMixVal >= 0.0f) {
		addAlbedoTextureMixVal -= phaseDifference;
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
}
//Button mix val
float changeTextureDemonstratorWidth = 0.4f;
float changeTextureDemonstratorHeight = 0.8f;

float orgTextureDemonstratorWidth = 0.4f;
float orgTextureDemonstratorHeight = 0.8f;
bool changeTextureDemonstrator;



unsigned int currentBrushMaskTexture;

RenderOutData Render::renderUi(PanelData panelData,UiData uidata,RenderData renderData,unsigned int FBOScreen, float brushBlurRangeBarValue, float brushRotationRangeBarValue, float brushOpacityRangeBarValue, float brushSpacingRangeBarValue,float textureDemonstratorButtonPosX,float textureDemonstratorButtonPosY,bool textureDemonstratorButtonPressClicked,Icons icons,glm::vec3 colorBoxValue,const char* maskTextureFile,int paintingFillNumericModifierVal,const char* exportFileName,float maskPanelSliderValue,std::vector<unsigned int> &maskTextures,double mouseXpos,double mouseYpos,int screenSizeX,int screenSizeY,std::string colorpickerHexVal) {
	ColorData colorData;
	glm::mat4 projection;
	UserInterface ui;
	Utilities util;
    GlSet gl; 
	RenderOutData uiOut;
	uiOut.maskPanelMaskClicked = false;
	uiOut.maskPanelMaskHover = false;

	gl.uniform1i(renderPrograms.program, "isTwoDimensional", 1);
	gl.uniform1i(renderPrograms.program, "is2D", 1);

	float screenGapX = (1920.0 - screenSizeX)/960.0; 

	//Panel
	ui.panel(renderData.panelLoc-screenGapX , 0);
	projection = glm::ortho(0.0f, 2.0f, -1.0f, 1.0f);
	glUseProgram(renderPrograms.iconsProgram);
	gl.uniformMatrix4fv(12, "Projection", projection);
	glUseProgram(renderPrograms.program);
	gl.uniformMatrix4fv(3, "TextProjection", projection);
	ui.panelChangeButton(renderData.panelLoc - screenGapX, 0.8f);//Model Panel
	ui.iconBox(0.015f,0.02f,renderData.panelLoc-0.01f - screenGapX,0.795f,0.9,icons.TDModel,0);
	ui.panelChangeButton(renderData.panelLoc - screenGapX, 0.72f);//Texture Panel
	ui.iconBox(0.015f,0.02f,renderData.panelLoc-0.01f - screenGapX,0.715f,0.9,icons.ImportTexture,0);
	ui.panelChangeButton(renderData.panelLoc - screenGapX, 0.64f);//Painting Panel
	ui.iconBox(0.015f,0.02f,renderData.panelLoc-0.01f - screenGapX,0.635f,0.9,icons.Painting,0);//Dropper
	ui.panelChangeButton(renderData.panelLoc - screenGapX, 0.56f);//Export Panel
	ui.iconBox(0.015f,0.02f,renderData.panelLoc-0.01f - screenGapX, 0.555f,0.9,icons.Export,0);
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

		ui.checkBox(renderData.panelLoc- 0.16f- screenGapX, 0.9f, "X", colorData.checkBoxColor, uidata.mirrorXCheckBoxEnter, uidata.mirrorXCheckBoxPressed); //X mirror checkbox
		ui.checkBox(renderData.panelLoc- 0.10f- screenGapX, 0.9f, "Y", colorData.checkBoxColor, uidata.mirrorYCheckBoxEnter, uidata.mirrorYCheckBoxPressed); //Z mirror checkbox
		ui.checkBox(renderData.panelLoc- 0.04f- screenGapX, 0.9f, "Z", colorData.checkBoxColor, uidata.mirrorZCheckBoxEnter, uidata.mirrorZCheckBoxPressed); //Y mirror checkbox
		ui.box(0.25f, 0.22f, renderData.panelLoc + 0.25f- screenGapX, -0.6f, "", colorData.panelColorSnd, 0.075f, false, false, 0.1f, 10000, glm::vec3(0), 0);//Backside decoration
		ui.iconBox(0.015f,0.02f,renderData.panelLoc - 0.09f- screenGapX,0.95f,0.9,icons.Mirror,0);
	}

	float centerDivider;
	float centerSum;
	gl.uniform1f(renderPrograms.program, "uiOpacity", 0.5f);
	if (!panelData.movePanel) {
		centerDivider = 2.0f;
		centerSum = 0;
		projection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f);
		glUseProgram(renderPrograms.iconsProgram);
		gl.uniformMatrix4fv(12, "Projection", projection);
		glUseProgram(renderPrograms.program);
		gl.uniformMatrix4fv(3, "TextProjection", projection);
	}
	else {
		centerDivider = 1.0f;
		centerSum = 0.15f;
		projection = glm::ortho(0.0f, 2.0f, -1.0f, 1.0f);
		glUseProgram(renderPrograms.iconsProgram);
		gl.uniformMatrix4fv(12, "Projection", projection);
		glUseProgram(renderPrograms.program);
		gl.uniformMatrix4fv(3, "TextProjection", projection);
	}
	if (panelData.modelPanelActive) {
		//File path textbox
		ui.box(0.12f, 0.03f, renderData.panelLoc / centerDivider + centerSum - screenGapX, 0.6f, renderData.modelLoadFilePath, colorData.textBoxColor, 0, true, false, 0.9f, 10, glm::vec3(0), 0);
		ui.renderText(renderPrograms.program, "File Path", renderData.panelLoc / centerDivider + centerSum - screenGapX - 0.05f, 0.64f, 0.00022f);
		ui.iconBox(0.020f,0.04f,renderData.panelLoc / centerDivider + centerSum - screenGapX + 0.1f,0.6f,1,icons.Folder,0);
		
		ui.box(0.08f, 0.04f, renderData.panelLoc / centerDivider + centerSum - screenGapX, 0.4f, "Load", colorData.buttonColor, 0.022f, false, false, 0.9f, 10, colorData.buttonColorHover, loadModelButtonMixVal);//Load model button
		ui.box(0.012f, 0.04f, renderData.panelLoc / centerDivider + centerSum - screenGapX, 0.0f, "", colorData.buttonColor, 0.047f, false, false, 0.9f, 7, colorData.buttonColorHover, addPanelButtonMixVal);//Load a panel button
		ui.iconBox(0.03f,0.04f,renderData.panelLoc / centerDivider + centerSum - screenGapX,0.0,0.99,icons.Panel,0);
		ui.box(0.012f, 0.04f, renderData.panelLoc / centerDivider + centerSum - screenGapX, -0.1f, "", colorData.buttonColor, 0.055f, false, false, 0.9f, 7, colorData.buttonColorHover, addSphereButtonMixVal);//Load a sphere button
		ui.iconBox(0.03f,0.04f,renderData.panelLoc / centerDivider + centerSum - screenGapX,-0.1f,0.99,icons.Sphere,0);


		ui.checkBox(renderData.panelLoc / centerDivider + centerSum - screenGapX - 0.08f, 0.3f, "Auto triangulate", colorData.checkBoxColor, uidata.autoTriangulateCheckBoxEnter, uidata.autoTriangulateCheckBoxPressed); //Auto triangulate checkbox
		ui.checkBox(renderData.panelLoc / centerDivider + centerSum - screenGapX - 0.08f, 0.2f, "Backface culling", colorData.checkBoxColor, uidata.backfaceCullingCheckBoxEnter, uidata.backfaceCullingCheckBoxPressed); //Backface culling checkbox

	}
	if (panelData.texturePanelActive) {
		ui.box(0.1f, 0.04f, renderData.panelLoc / centerDivider + centerSum - screenGapX, 0.8f, "Add Texture", colorData.buttonColor, 0.048f, false, false, 0.9f, 10, colorData.buttonColorHover, addAlbedoTextureMixVal); //Add albedo texture button
	}

	if (panelData.paintingPanelActive) {
		ui.box(0.1f, 0.04f, renderData.panelLoc / centerDivider + centerSum - screenGapX, 0.9f, "Add Mask Texture", colorData.buttonColor, 0.095f, false, false, 0.9f, 10, colorData.buttonColorHover, addMaskTextureButtonMixVal); //Add mask texture button
		
		ui.box(0.005f, 0.015f, renderData.panelLoc / centerDivider + centerSum - screenGapX + 0.13f, 0.8f + maskPanelSliderValue, "", glm::vec3(0), 0.095f, false, false, 0.9f, 30, glm::vec3(0), 0); //Mask panel slider
		ui.box(0.15f, 0.15f, renderData.panelLoc / centerDivider + centerSum - screenGapX, 0.675f, "", colorData.buttonColor, 0.095f, false, false, 0.9f, 1000, glm::vec3(0), 0); //Mask panel
		
		gl.uniform1i(renderPrograms.program, "isMaskPanelDisplay", 1);

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
			ColorData clrData;
			Texture txtr;

			glUseProgram(renderPrograms.iconsProgram);
			gl.uniform1i(12,"isMaskIcon",1);
			gl.uniform3fv(12,"iconColor",clrData.iconColor);
			gl.uniform3fv(12,"iconColorHover",clrData.iconColorHover);
			gl.uniform1f(12,"iconMixVal",0);
			gl.activeTexture(GL_TEXTURE6);
			gl.bindTexture(maskTextures[i]);
			gl.drawArrays(buttonCoorSq,false);
			gl.uniform1i(12,"isMaskIcon",0);
			glUseProgram(renderPrograms.program);

			//TODO : Check once the mouse pos changed
			if(ui.isMouseOnCoords(renderData.window,mouseXpos,mouseYpos,buttonCoorSq,panelData.movePanel)){
				if(glfwGetMouseButton(renderData.window, 0) == GLFW_PRESS){
					gl.activeTexture(GL_TEXTURE1);
					gl.bindTexture(maskTextures[i]);
					txtr.updateMaskTexture(FBOScreen,screenSizeX,screenSizeY,brushRotationRangeBarValue,false);
					gl.uniform1i(renderPrograms.program, "isTwoDimensional", 1);
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

		gl.uniform1i(renderPrograms.program, "isMaskPanelDisplay", 0);
		ui.checkBox(renderData.panelLoc / centerDivider + centerSum - screenGapX - 0.03f, 0.45f, "Use Negative", colorData.checkBoxColor, uidata.useNegativeForDrawingCheckboxEnter, uidata.useNegativeForDrawingCheckboxPressed); //Auto triangulate checkbox
		ui.renderText(renderPrograms.program, maskTextureFile, renderData.panelLoc / centerDivider + centerSum - screenGapX - 0.03f, 0.37f, 0.00022f);
		ui.box(0.035f, 0.07f, renderData.panelLoc / centerDivider + centerSum - screenGapX - 0.1f, 0.42f, "", colorData.buttonColor, 0.075f, false, true, 0.9f, 1000, glm::vec3(0), 0); //Mask texture displayer / GL_TEXTURE12

		//Brush size rangebar
		ui.renderText(renderPrograms.program, "Size", renderData.panelLoc / centerDivider + centerSum - screenGapX - 0.02f, 0.25f+0.04f, 0.00022f);
		ui.rangeBar(renderData.panelLoc / centerDivider + centerSum - screenGapX, 0.22f+0.02f, renderData.brushSizeRangeBarValue);
		//ui.iconBox(0.03f,0.04f,renderData.panelLoc / centerDivider + centerSum - screenGapX - 0.05f, 0.27f,1,icons.MaskScale);

		//Brush blur rangebar
		ui.renderText(renderPrograms.program, "Blur", renderData.panelLoc / centerDivider + centerSum - screenGapX - 0.02f, +0.125f+0.06f, 0.00022f);
		ui.rangeBar(renderData.panelLoc / centerDivider + centerSum - screenGapX, 0.09f+0.04f, brushBlurRangeBarValue);
		//ui.iconBox(0.03f,0.04f,renderData.panelLoc / centerDivider + centerSum - screenGapX - 0.05f, 0.14f,1,icons.MaskGausBlur);

		//Brush rotation rangebar
		ui.renderText(renderPrograms.program, "Rotation", renderData.panelLoc / centerDivider + centerSum - screenGapX - 0.04f, -0.005f+0.08f, 0.00022f);
		ui.rangeBar(renderData.panelLoc / centerDivider + centerSum - screenGapX, -0.04f+0.06f, brushRotationRangeBarValue);
		//ui.iconBox(0.03f,0.04f,renderData.panelLoc / centerDivider + centerSum - screenGapX - 0.05f, +0.01f,1,icons.MaskRotation);

		//Brush opacity rangebar
		ui.renderText(renderPrograms.program, "Opacity", renderData.panelLoc / centerDivider + centerSum - screenGapX - 0.035f, -0.135f+0.10f, 0.00022f);
		ui.rangeBar(renderData.panelLoc / centerDivider + centerSum - screenGapX, -0.17f+0.08f, brushOpacityRangeBarValue);
		//ui.iconBox(0.03f,0.04f,renderData.panelLoc / centerDivider + centerSum - screenGapX - 0.05f, -0.12f,1,icons.MaskOpacity);

		//Brush spacing rangebar
		ui.renderText(renderPrograms.program, "Spacing", renderData.panelLoc / centerDivider + centerSum - screenGapX - 0.035f, -0.265f+0.12f, 0.00022f);
		ui.rangeBar(renderData.panelLoc / centerDivider + centerSum - screenGapX, -0.30f+0.1f, brushSpacingRangeBarValue);
		//ui.iconBox(0.03f,0.04f,renderData.panelLoc / centerDivider + centerSum - screenGapX - 0.05f, -0.25f,1,icons.MaskSpacing);

        ui.numericModifier(renderData.panelLoc / centerDivider + centerSum - screenGapX,-0.3,icons.ArrowLeft,icons.ArrowRight,0.9f, paintingFillNumericModifierVal,fillBetweenResNumericModifiermixValP,fillBetweenResNumericModifiermixValN); //Fill quality

		//Color Picker
		//ui.box(0.1f, 0.2f, renderData.panelLoc / centerDivider + centerSum - screenGapX-0.02f, -0.6f, "", colorBoxValue / glm::vec3(255), 0.075f, false, false, 0.9f, 1000, glm::vec3(0), 0); //indicator for picken color of the color picker
		ui.colorBox(renderData.panelLoc / centerDivider + centerSum - screenGapX - 0.02f, -0.6f, renderData.colorBoxPickerValue_x, renderData.colorBoxPickerValue_y);
		
		ui.colorRect(renderData.panelLoc / centerDivider + centerSum - screenGapX + 0.1f, -0.6f, renderData.colorBoxColorRangeBarValue, FBOScreen, renderData.window);

		gl.uniform1f(renderPrograms.program, "uiOpacity", 1.0f);
		ui.box(0.005f, 0.025f, renderData.panelLoc / centerDivider + centerSum - screenGapX - 0.095f, -0.85f, "", colorBoxValue / glm::vec3(255), 0.075f, false, false, 0.9f, 10, glm::vec3(0), 0); //indicator for picken color of the color picker
		gl.uniform1f(renderPrograms.program, "uiOpacity", 0.5f);
		ui.decorationSquare(renderData.panelLoc / centerDivider + centerSum - screenGapX - 0.1f, -0.84f); //Decoration for color indicator

		ui.box(0.04f, 0.03f, renderData.panelLoc / centerDivider + centerSum - screenGapX - 0.0f,-0.86f, util.rgbToHexGenerator(colorBoxValue), colorData.textBoxColor, 0, true, false, 0.9f, 10, colorData.textBoxColorClicked, hexValTextboxMixVal);//Hex val textbox
		ui.iconBox(0.02f,0.03f,renderData.panelLoc / centerDivider + centerSum - screenGapX + 0.08f, -0.86f,0.9,icons.dropperIcon,dropperMixVal);
	}

	if (panelData.exportPanelActive) {
		ui.box(0.12f, 0.03f, renderData.panelLoc / centerDivider + centerSum - screenGapX, 0.6f, renderData.exportFolder, colorData.textBoxColor, 0, true, false, 0.9f, 10, glm::vec3(0), 0); //Path textbox
		ui.box(0.12f, 0.03f, renderData.panelLoc / centerDivider + centerSum - screenGapX, 0.5f, exportFileName,colorData.textBoxColor, 0, true, false, 0.9f, 10, colorData.textBoxColorClicked, exportFileNameTextBoxMixVal); //File name textbox

		ui.checkBox(renderData.panelLoc / centerDivider + centerSum - screenGapX - 0.11f, 0.4f, "", colorData.checkBoxColor, uidata.exportExtJPGCheckBoxEnter, uidata.exportExtJPGCheckBoxPressed); //jpg checkbox
		ui.iconBox(0.05f,0.065f,renderData.panelLoc / centerDivider + centerSum - screenGapX - 0.06f, 0.4f,0.9,icons.JpgFile,0);

		ui.checkBox(renderData.panelLoc / centerDivider + centerSum - screenGapX + 0.05f, 0.4f, "", colorData.checkBoxColor, uidata.exportExtPNGCheckBoxEnter, uidata.exportExtPNGCheckBoxPressed); //png checkbox
		ui.iconBox(0.05f,0.065f,renderData.panelLoc / centerDivider + centerSum - screenGapX + 0.1f, 0.4f,0.9,icons.PngFile,0);

		ui.box(0.1f, 0.04f, renderData.panelLoc / centerDivider + centerSum - screenGapX, 0.2f, "Download", colorData.buttonColor, 0.045f, false, false, 0.9f, 10, colorData.buttonColorHover, exportDownloadButtonMixVal); //Download Button
	}
	projection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f);
	glUseProgram(renderPrograms.iconsProgram);
	gl.uniformMatrix4fv(12, "Projection", projection);
	glUseProgram(renderPrograms.program);
	gl.uniformMatrix4fv(3, "TextProjection", projection);
	uiOut.currentBrushMaskTxtr = currentBrushMaskTexture;
	return uiOut;
}
//--------------------RENDER UI --------------------\\


void Render::renderModel(bool backfaceCulling, std::vector<float>& vertices) {
    GlSet gl;
	if (backfaceCulling) { //if backface culling checked in the model panel
		gl.enable(GL_CULL_FACE);
		gl.cullFace(GL_BACK);
	}
	gl.meshDataToShaders();
	gl.drawArrays(vertices, false);

	gl.disable(GL_CULL_FACE); //Disable backface culling if enabled
}
void Render::renderSkyBox() {
	GlSet gls;
	glUseProgram(renderPrograms.skyboxProgram);

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
	
	glUseProgram(renderPrograms.program);
}
void Render::setMatrices() {
    GlSet gl;
	glm::mat4 textProjection = glm::ortho(0.0f, 2.0f, -1.0f, 1.0f);
	glUseProgram(renderPrograms.iconsProgram);
	gl.uniformMatrix4fv(renderPrograms.iconsProgram, "Projection", textProjection);
	glUseProgram(renderPrograms.program);
	gl.uniformMatrix4fv(renderPrograms.program, "TextProjection", textProjection);

	glm::mat4 renderTextureProjection = glm::ortho(0.0f, 1.77777777778f, 0.0f, 1.0f);//1920 - 1080 -> 1.77777777778 - 1
	glUseProgram(renderPrograms.blurProgram);
	gl.uniformMatrix4fv(renderPrograms.blurProgram, "renderTextureProjection", renderTextureProjection);
	glUseProgram(renderPrograms.program);
	gl.uniformMatrix4fv(renderPrograms.program, "renderTextureProjection", renderTextureProjection);

	glm::mat4 model = glm::mat4(1.0f);
	gl.uniformMatrix4fv(renderPrograms.program, "model", model);

	glm::mat4 projection = glm::perspective(glm::radians(45.0f), 1920.0f / 1080.0f, 0.1f, 1000.0f);
	glUseProgram(renderPrograms.skyboxProgram);
	gl.uniformMatrix4fv(renderPrograms.skyboxProgram, "projection",projection);
	glUseProgram(renderPrograms.program);
	gl.uniformMatrix4fv(renderPrograms.program, "projection",projection);


	ProjectionData pd;
	pd.modelMat = model;
	pd.projMat = projection;
	//pd.viewMat = view;
}
void Render::updateViewMatrix(glm::vec3 cameraPos, glm::vec3 originPos,bool mirrorX,bool mirrorY,bool mirrorZ) {
	glm::mat4 view;
    GlSet gl;
	view = glm::lookAt(cameraPos, originPos, glm::vec3(0.0, 1.0, 0.0)); 

	glm::vec3 mirrorVec = glm::vec3(1.0f - (int)mirrorX*2, 1.0f -(int)mirrorY * 2, 1.0f - (int)mirrorZ * 2);
	glm::mat4 mirroredView;
	mirroredView = glm::lookAt(cameraPos * mirrorVec, originPos * mirrorVec, glm::vec3(0.0, 1.0, 0.0));

	glUseProgram(renderPrograms.skyboxProgram);
	gl.uniformMatrix4fv(6, "view", view);
	glUseProgram(renderPrograms.program);
	gl.uniformMatrix4fv(3, "view", view);

	gl.uniform3fv(renderPrograms.program, "viewPos", cameraPos);
	gl.uniform3fv(renderPrograms.program, "mirroredViewPos", cameraPos * mirrorVec);

	gl.uniformMatrix4fv(renderPrograms.program, "mirroredView", mirroredView);
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

	// int lightColorLoc = glGetUniformLocation(renderPrograms.program, "lightColor");
	// glUniform3f(lightColorLoc, 10.0f, 10.0f, 10.0f);
	// int isLightSourceLoc = glGetUniformLocation(renderPrograms.program, "isLightSource");
	// glUniform1i(isLightSourceLoc, 1);
	// int modelLoc = glGetUniformLocation(renderPrograms.program, "model");
	// glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	// glDrawArrays(GL_TRIANGLES, 0, 36);
}

void Render::getDepthTexture(std::vector<float>& vertices,unsigned int FBOScreen, unsigned int screenSizeX, unsigned int screenSizeY) {
	Texture txtr;
    GlSet gl;

	gl.viewport(1920, 1080);
	gl.bindFramebuffer(FBOScreen);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	gl.uniform1i(renderPrograms.program, "isTwoDimensional", 0);
	gl.uniform1i(renderPrograms.program, "isRenderTextureMode", 1);
	gl.uniform1i(renderPrograms.program, "isRenderTextureModeV", 0);
	gl.uniform1i(renderPrograms.program, "depthTexture", 9);
	gl.uniform1i(renderPrograms.program, "mirroredDepthTexture", 8);
	gl.uniform1i(renderPrograms.program, "renderDepth", 1);
	gl.uniform1i(renderPrograms.program, "renderMirroredDepth", 0);
	gl.drawArrays(vertices, false);
	GLubyte* screen = txtr.getTextureFromProgram(GL_TEXTURE5, 1920, 1080, 3);
	gl.activeTexture(GL_TEXTURE9);
	gl.texImage(screen, 1920, 1080, GL_RGB);
	gl.generateMipmap();
	delete[]screen;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	gl.uniform1i(renderPrograms.program, "renderMirroredDepth", 1);
	gl.uniform1i(renderPrograms.program, "renderMirroredDepthV", 1);
	gl.drawArrays(vertices, false);
	GLubyte* screenMirrored = txtr.getTextureFromProgram(GL_TEXTURE5, 1920, 1080, 3);
	gl.activeTexture(GL_TEXTURE8);
	gl.texImage(screenMirrored, 1920, 1080, GL_RGB);
	gl.generateMipmap();
	delete[] screenMirrored;

	gl.uniform1i(renderPrograms.program, "isRenderTextureModeV", 0);
	gl.uniform1i(renderPrograms.program, "renderMirroredDepthV", 0);
	gl.uniform1i(renderPrograms.program, "isRenderTextureMode", 0);
	gl.uniform1i(renderPrograms.program, "renderDepth", 0);
	gl.uniform1i(renderPrograms.program, "renderMirroredDepth", 0);
	glViewport(0, -(1072 - screenSizeY), 1920, 1072);

	gl.bindFramebuffer(0);
}

//------------CtrlZ------------
std::vector<GLubyte*> undoTextures; 
bool doCtrlZ;
void ctrlZCheck(GLFWwindow* window) {
	Texture txtr;
	GlSet glset;
	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS && doCtrlZ && undoTextures.size() != 0) { //MAX 20
		txtr.refreshScreenDrawingTexture();
		glset.activeTexture(GL_TEXTURE0);
		glset.texImage(undoTextures[undoTextures.size() - 1], 1080, 1080, GL_RGB);
		glset.generateMipmap();

		GLubyte* previousTexture  = undoTextures[undoTextures.size() - 1];
		delete[] previousTexture;

		undoTextures.pop_back();
		doCtrlZ = false;
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_RELEASE || glfwGetKey(window, GLFW_KEY_Z) == GLFW_RELEASE) {
		doCtrlZ = true;
	}
}
//------------CtrlZ------------

void drawBrushIndicator(float distanceX,float screenWidth,float screenHeight,float mouseXpos,float mouseYpos,glm::vec3 color) {

	float sizeX = distanceX / ((1920.0f / (float)screenWidth)); //Match the size of the window

	GlSet glset;
	glset.uniform1i(renderPrograms.program, "drawBrushIndicator", 1);
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
	glset.uniform1f(renderPrograms.program, "uiOpacity", 0.2f);
	glset.drawArrays(paintingSquare, false);
	glset.uniform1f(renderPrograms.program, "uiOpacity", 0.5f);

	glset.uniform1i(renderPrograms.program, "drawBrushIndicator", 0);
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


void Render::exportTexture(bool JPG,bool PNG,const char* exportPath,const char* exportFileName){
    Texture txtr;
    GLubyte* exportTxtr = txtr.getTextureFromProgram(GL_TEXTURE0,1080,1080,3);
	if (JPG) {
		txtr.downloadTexture(exportPath, exportFileName, 0, 1080, 1080, exportTxtr, 3);
	}
	else if (PNG) {
		txtr.downloadTexture(exportPath, exportFileName, 1, 1080, 1080, exportTxtr, 3);
	}
    delete[] exportTxtr;
}
void Render::renderTexture(std::vector<float>& vertices,unsigned int width, unsigned int height,unsigned int texture){
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    GlSet gl;

	gl.drawArrays(vertices, false); //Render Model
	GLubyte* renderedTexture = new GLubyte[width * 1080 * 3 * sizeof(GLubyte)];
	glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, renderedTexture);

	gl.activeTexture(texture);
	gl.texImage(renderedTexture, width, height, GL_RGB);
	gl.generateMipmap();
	delete[]renderedTexture;
}

void Render::renderTextures(unsigned int FBOScreen, std::vector<float>& vertices,bool exportImage, bool JPG, bool PNG, const char* exportPath,unsigned int screenSizeX, unsigned int screenSizeY,const char* exportFileName) {
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
	Texture txtr;

    //Send the texture to the undoTextures vector before updating the texture
	GLubyte* originalImage = txtr.getTextureFromProgram(GL_TEXTURE0, 1080, 1080, 3);
	undoTextures.push_back(originalImage);

    //Delete the first texture from undoTextures array once the size of the vector hits 20
	if (undoTextures.size() > maxTextureHistoryHold){
		GLubyte* previousTexture  = undoTextures[0];
		delete[] previousTexture;
		undoTextures.erase(undoTextures.begin());
	}

    GlSet gl;
	//Setup
	gl.uniform1i(renderPrograms.program, "isTwoDimensional", 0);
	gl.uniform1i(renderPrograms.program, "isRenderTextureMode", 1);
	gl.uniform1i(renderPrograms.program, "isRenderTextureModeV", 1);
	gl.viewport(1920, 1080);
	gl.bindFramebuffer(FBOScreen);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//Setup

	//Render painted image
	gl.drawArrays(vertices, false);
	if (!exportImage)
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
	gl.uniform1i(renderPrograms.program, "whiteRendering", 1);
	renderTexture(vertices,1080, 1080,GL_TEXTURE7);
	gl.uniform1i(renderPrograms.program, "whiteRendering", 0);
	//Render uv mask

	//interpret the albedo with ui mask texture
	gl.uniform1i(renderPrograms.program, "interpretWithUvMask", 1);
	renderTexture(renderVertices,1080, 1080,GL_TEXTURE0);//Render enlarged texture
	gl.uniform1i(renderPrograms.program, "interpretWithUvMask", 0);
	//interpret the albedo with ui mask texture

	//Download enlarged texture
	if (exportImage) {
        exportTexture(JPG,PNG,exportPath,exportFileName);
	}

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Finish
	gl.uniform1i(renderPrograms.program, "isRenderTextureModeV", 0);
	gl.uniform1i(renderPrograms.program, "isRenderTextureMode", 0);
	gl.bindFramebuffer(0);
	glViewport(0, -(1072 - screenSizeY), 1920, 1072);

	//Finish
}
glm::vec3 getScreenHoverPixel(double mouseXpos,double mouseYpos,unsigned int screenSizeY){
    glm::vec3 screenHoverPixel;
	GLubyte* screenPixel = new GLubyte[1 * 1 * 3];//Color val
	glReadPixels(mouseXpos,screenSizeY - mouseYpos, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, screenPixel);//Read the pixel from the framebuffer
	screenHoverPixel.r = screenPixel[0];
	screenHoverPixel.g = screenPixel[1];
	screenHoverPixel.b = screenPixel[2];
	delete[] screenPixel;  
    return screenHoverPixel;
}
glm::vec3 Render::getColorBoxValue(unsigned int FBOScreen,float colorBoxPickerValue_x, float colorBoxPickerValue_y, unsigned int screenSizeX, unsigned int screenSizeY) {
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
	//Setup
	gl.viewport(1920, 1080);
	gl.uniform1i(renderPrograms.program, "isRenderTextureModeV", 1);
	gl.uniform1i(renderPrograms.program, "isRenderTextureMode", 1);
	gl.bindFramebuffer(FBOScreen);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//Setup

	//Render color box
	GLubyte* colorBoxPixel = new GLubyte[1 * 1 * 3];//Color val
	gl.uniform1i(renderPrograms.program, "isColorBox", 1);
	gl.drawArrays(colorBox, false); //Render Model
	gl.uniform1i(renderPrograms.program, "isColorBox", 0);
	glReadPixels(1080 - ((colorBoxPickerValue_x * -1.0f + 0.1f) * 5.0f * 1080), (colorBoxPickerValue_y + 0.2f) * 2.5f * 1080, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, colorBoxPixel);
	gl.uniform3f(renderPrograms.program, "drawColor", colorBoxPixel[0] / 255.0f, colorBoxPixel[1] / 255.0f, colorBoxPixel[2] / 255.0f);
	//Render color box

	//Finish
	gl.uniform1i(renderPrograms.program, "isRenderTextureMode", 0);
	gl.uniform1i(renderPrograms.program, "isRenderTextureModeV", 0);
	gl.bindFramebuffer(0);
	glViewport(0, -(1072 - screenSizeY), 1920, 1072);


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


RenderOutData Render::render(RenderData renderData, std::vector<float>& vertices, unsigned int FBOScreen, PanelData panelData, ExportData exportData,UiData uidata,float textureDemonstratorButtonPosX,float textureDemonstratorButtonPosY, bool textureDemonstratorButtonPressClicked,float textureDemonstratorWidth, float textureDemonstratorHeight,bool textureDemonstratorBoundariesPressed,Icons icons,const char* maskTextureFile,int paintingFillNumericModifierVal,float maskPanelSliderValue,std::vector<unsigned int> &maskTextures,std::string colorpickerHexVal,bool colorpickerHexValTextboxValChanged,bool colorBoxValChanged) {
	GlSet gls;
	UserInterface ui;
	ColorData colorData;
	Utilities util;
	Texture txtr;
	
	colorBoxVal = util.hexToRGBConverter(colorpickerHexVal);

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
		renderTextures(FBOScreen,vertices,exportData.exportImage,uidata.exportExtJPGCheckBoxPressed, uidata.exportExtPNGCheckBoxPressed,exportData.path,screenSizeX, screenSizeY,exportData.fileName);
	}

	renderSkyBox();
	renderModel(renderData.backfaceCulling,vertices);
	drawAxisPointer();

	RenderOutData uiOut;
	uiOut = renderUi(panelData, uidata, renderData, FBOScreen, renderData.brushBlurRangeBarValue,renderData.brushRotationRangeBarValue, renderData.brushOpacityRangeBarValue, renderData.brushSpacingRangeBarValue,textureDemonstratorButtonPosX,textureDemonstratorButtonPosY,textureDemonstratorButtonPressClicked,icons,colorBoxVal,maskTextureFile,paintingFillNumericModifierVal,exportData.fileName, maskPanelSliderValue,maskTextures,mouseXpos,mouseYpos,screenSizeX,screenSizeY,colorpickerHexVal);


	if (colorBoxValChanged && !colorpickerHexValTextboxValChanged) { //Get value of color box
		colorBoxVal = getColorBoxValue(FBOScreen, renderData.colorBoxPickerValue_x, renderData.colorBoxPickerValue_y,screenSizeX, screenSizeY);
	}
	ctrlZCheck(renderData.window);


	updateButtonColorMixValues(uidata);

	glm::vec3 screenHoverPixel = getScreenHoverPixel(mouseXpos,mouseYpos,screenSizeY);

	if(renderData.doPainting)
		drawBrushIndicator(renderData.brushSizeIndicator, 1920, 1072, mouseXpos, mouseYpos, colorBoxVal);


	RenderOutData renderOut;
	renderOut.mouseHoverPixel = screenHoverPixel;
	renderOut.maskPanelMaskClicked = uiOut.maskPanelMaskClicked;
	renderOut.maskPanelMaskHover = uiOut.maskPanelMaskHover;
	renderOut.currentBrushMaskTxtr = uiOut.currentBrushMaskTxtr;
	renderOut.colorpickerHexVal = util.rgbToHexGenerator(colorBoxVal);
	return renderOut;
}
void Render::sendProgramsToRender(Programs apprenderPrograms){
	renderPrograms = apprenderPrograms;
}
