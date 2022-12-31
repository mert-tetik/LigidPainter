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

#include "tinyfiledialogs.h"

bool texturePanelButtonHover = false;

//Button mix val
float exportDownloadButtonMixVal = 0.0f;
float addMaskTextureButtonMixVal = 0.0f;
float loadModelButtonMixVal = 0.0f;
float addPanelButtonMixVal = 0.0f;
float addSphereButtonMixVal = 0.0f;
float addAlbedoTextureMixVal = 0.0f;
float dropperMixVal = 0.0f;
float exportFileNameTextBoxMixVal = 0.0f;
float hexValTextboxMixVal = 0.0f;
float customModelMixVal = 0.0f;
float texturePanelButtonMixVal = 0.0f;



void updateButtonColorMixValues(UiData uidata) {
	Utilities util;

	const float phaseDifference = 0.05f;

	addSphereButtonMixVal = util.transitionEffect(uidata.addSphereButtonEnter,addSphereButtonMixVal,phaseDifference);
	addPanelButtonMixVal = util.transitionEffect(uidata.addPlaneButtonEnter,addPanelButtonMixVal,phaseDifference);
	loadModelButtonMixVal = util.transitionEffect(uidata.loadModelButtonEnter,loadModelButtonMixVal,phaseDifference);
	addMaskTextureButtonMixVal = util.transitionEffect(uidata.addMaskTextureButtonEnter,addMaskTextureButtonMixVal,phaseDifference);
	exportDownloadButtonMixVal = util.transitionEffect(uidata.exportDownloadButtonEnter,exportDownloadButtonMixVal,phaseDifference);
 	dropperMixVal = util.transitionEffect(uidata.dropperEnter,dropperMixVal,phaseDifference);
 	exportFileNameTextBoxMixVal = util.transitionEffect(uidata.exportFileNameTextBoxPressed,exportFileNameTextBoxMixVal,phaseDifference);
 	hexValTextboxMixVal = util.transitionEffect(uidata.hexValTextboxPressed,hexValTextboxMixVal,phaseDifference);
 	customModelMixVal = util.transitionEffect(uidata.customModelButtonHover,customModelMixVal,phaseDifference);
 	texturePanelButtonMixVal = util.transitionEffect(texturePanelButtonHover,texturePanelButtonMixVal,phaseDifference);
}


bool changeTextureDemonstrator;

float changeTextureDemonstratorWidth = 0.4f;
float changeTextureDemonstratorHeight = 0.8f;

RenderOutData Render::renderUi(PanelData &panelData,UiData& uidata,RenderData& renderData,unsigned int FBOScreen, float brushBlurRangeBarValue, float brushRotationRangeBarValue
, float brushOpacityRangeBarValue, float brushSpacingRangeBarValue,float textureDemonstratorButtonPosX,float textureDemonstratorButtonPosY,
bool textureDemonstratorButtonPressClicked,Icons &icons,glm::vec3 colorBoxValue,const char* maskTextureFile,
const char* exportFileName,float maskPanelSliderValue,std::vector<unsigned int> &maskTextures,double mouseXpos,double mouseYpos,int screenSizeX,int screenSizeY,
std::string &colorpickerHexVal,float brushBorderRangeBarValue,float brushBlurVal,OutShaderData &outShaderData, Model &model,vector<unsigned int> &albedoTextures,
bool updateHueVal,Programs programs,int &currentMaterialIndex,int maxScreenWidth,int maxScreenHeight,float orgTextureDemonstratorWidth, float orgTextureDemonstratorHeight, 
SaturationValShaderData &saturationValShaderData,glm::vec3 &hueVal,unsigned int &currentBrushMaskTexture,float materialsPanelSlideValue,UI UIElements) {

	
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


	float screenGapX = ((float)maxScreenWidth - screenSizeX)/(((float)maxScreenWidth)/2.0f)/2.0f; 

	updateButtonColorMixValues(uidata);

	//Panel
	if(panelData.exportPanelActive || panelData.modelPanelActive || panelData.paintingPanelActive || panelData.texturePanelActive){ //Disable panel if a message box is active
		//If message box is not active
		ui.panel(renderData.panelLoc-screenGapX , 0);

		//Projection that is used for panel (Use that projection if things will move with panel (and will not be centered) or will be moved freely)
		projection = glm::ortho(0.0f, 2.0f, -1.0f, 1.0f);

		//Panel changing buttons
		glUseProgram(programs.uiProgram);
		gl.uniformMatrix4fv(programs.uiProgram, "TextProjection", projection);
		ui.panelChangeButton(renderData.panelLoc - screenGapX, 0.8f);//Model Panel
		ui.panelChangeButton(renderData.panelLoc - screenGapX, 0.72f);//Texture Panel
		ui.panelChangeButton(renderData.panelLoc - screenGapX, 0.64f);//Painting Panel
		ui.panelChangeButton(renderData.panelLoc - screenGapX, 0.56f);//Export Panel

		//Texture demonstrator	
		ui.textureDemonstrator(changeTextureDemonstratorWidth,changeTextureDemonstratorHeight,textureDemonstratorButtonPosX+screenGapX,textureDemonstratorButtonPosY,0.9999f); 

		//Panel changing button's icons
		glUseProgram(programs.iconsProgram);
		gl.uniformMatrix4fv(programs.iconsProgram, "Projection", projection);
		ui.iconBox(0.015f,0.02f,renderData.panelLoc-0.01f - screenGapX,0.795f,0.9f,icons.TDModel,0.0f,colorData.iconColor,colorData.iconColorHover);
		ui.iconBox(0.015f,0.023f,renderData.panelLoc-0.01f - screenGapX,0.715f,0.9f,icons.Material,0.0f,colorData.iconColor,colorData.iconColorHover);
		ui.iconBox(0.015f,0.02f,renderData.panelLoc-0.01f - screenGapX,0.635f,0.9f,icons.Painting,0.0f,colorData.iconColor,colorData.iconColorHover);
		ui.iconBox(0.015f,0.02f,renderData.panelLoc-0.01f - screenGapX, 0.555f,0.9f,icons.Export,0.0f,colorData.iconColor,colorData.iconColorHover);

		glUseProgram(programs.uiProgram);
	}
	else{
		//If message box is active
		projection = glm::ortho(0.0f, 2.0f, -1.0f, 1.0f);
		glUseProgram(programs.iconsProgram);
		gl.uniformMatrix4fv(programs.iconsProgram, "Projection", projection);
		glUseProgram(programs.uiProgram);
		gl.uniformMatrix4fv(programs.uiProgram, "TextProjection", projection);
	}	

	if (panelData.texturePanelActive) {
		bool mouseEnteredOnce = false;
		glUseProgram(programs.uiProgram); 

		if(model.meshes.size() == 0){
			ui.renderText(programs.uiProgram, "Materials of the 3D model", renderData.panelLoc - screenGapX  + 0.095f, 0.8f, 0.00022f);
			ui.renderText(programs.uiProgram, "will be show up there", renderData.panelLoc - screenGapX  + 0.115f, 0.75f, 0.00022f);
		}
		for (int i = 0; i < model.meshes.size(); i++)//Render buttons
		{ 	
			//Check if mouse is entered the related button
			bool textureButtonEnter = ui.isMouseOnButton(renderData.window, 0.2f, 0.06f, renderData.panelLoc - screenGapX*2 + 0.205f,0.8f - (i * 0.125f) + materialsPanelSlideValue, mouseXpos, mouseYpos,true);
			bool textureAddButtonEnter = ui.isMouseOnButton(renderData.window, 0.02f, 0.03f, renderData.panelLoc - screenGapX*2 + 0.3f,0.8f - (i * 0.125f) + materialsPanelSlideValue, mouseXpos, mouseYpos,true);
			
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
				ui.box(0.2f, 0.06f, renderData.panelLoc - screenGapX + 0.205f, 0.8f - (i * 0.125f) + materialsPanelSlideValue, model.meshes[i].materialName, colorData.buttonColor, 0.048f, true, false, 0.5f, 10000, colorData.buttonColorHover, texturePanelButtonMixVal);

			}
			else{
				//Button 
				ui.box(0.2f, 0.06f, renderData.panelLoc - screenGapX + 0.205f, 0.8f - (i * 0.125f) + materialsPanelSlideValue, model.meshes[i].materialName, colorData.buttonColor, 0.048f, true, false, 0.5f, 10000, colorData.buttonColorHover, 0);

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
			//ui.box(0.03f, 0.06f, renderData.panelLoc - screenGapX + 0.3f, 0.8f - (i * 0.125f), "", colorData.buttonColorHover, 0.048f, true, false, 0.6f, 10000, colorData.buttonColorHover, 0); 
			glUseProgram(programs.iconsProgram);
			float iconmixVal = 0.0f;
			if(textureAddButtonEnter)
				iconmixVal = 0.5f;
				
			ui.iconBox(0.02f,0.025f,renderData.panelLoc - screenGapX + 0.3f ,0.8f - (i * 0.125f) + materialsPanelSlideValue,0.99f,icons.AddTexture,iconmixVal,colorData.iconColor,colorData.iconColorHover);
			glUseProgram(programs.uiProgram); 
		}
		if(!mouseEnteredOnce){
			texturePanelButtonHover = false;
		}
	}
	//Change the projection
	float centerDivider;
	float centerSum;
	if (!panelData.movePanel) {
		//Center the panel
		centerDivider = 2.0f;
		centerSum = 0;
		projection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f);
		glUseProgram(programs.iconsProgram);
		gl.uniformMatrix4fv(programs.iconsProgram, "Projection", projection);
		glUseProgram(programs.uiProgram);
		gl.uniformMatrix4fv(programs.uiProgram, "TextProjection", projection);
	}
	else {
		//Follow the panel
		centerDivider = 1.0f;
		centerSum = 0.15f;
		projection = glm::ortho(0.0f, 2.0f, -1.0f, 1.0f);
		glUseProgram(programs.iconsProgram);
		gl.uniformMatrix4fv(programs.iconsProgram, "Projection", projection);
		glUseProgram(programs.uiProgram);
		gl.uniformMatrix4fv(programs.uiProgram, "TextProjection", projection);
	}

    //Texture Panel Was There

	if (panelData.paintingPanelActive) {
		glUseProgram(programs.uiProgram); 

		//Color Picker
		hueVal = ui.colorRect(renderData.panelLoc / centerDivider + centerSum - screenGapX + 0.1f, -0.55f, renderData.colorBoxColorRangeBarValue, FBOScreen, renderData.window,projection,updateHueVal); //Hue TODO : Get the value once value changed 

		saturationValShaderData.boxColor = hueVal / 255.0f;
		saturationValShaderData.renderTextureProjection = projection;

		gl.useSaturationValBoxShader(programs.saturationValBoxProgram,saturationValShaderData);
		ui.colorBox(renderData.panelLoc / centerDivider + centerSum - screenGapX - 0.02f, -0.55f, renderData.colorBoxPickerValue_x, renderData.colorBoxPickerValue_y);


		ui.box(0.002f, 0.025f, renderData.panelLoc / centerDivider + centerSum - screenGapX - 0.095f, -0.81f, "", glm::vec4(colorBoxValue / glm::vec3(255),1.0f), 0.075f, false, false, 0.9f, 10, glm::vec4(0), 0); //indicator for picken color of the color picker

		ui.box(0.002f, 0.035f, renderData.panelLoc / centerDivider + centerSum - screenGapX - 0.095f, -0.81f, "", colorData.panelColorSnd, 0.075f, false, false, 0.9f, 7, glm::vec4(0), 0); //decoration

		ui.box(0.04f, 0.03f, renderData.panelLoc / centerDivider + centerSum - screenGapX - 0.008f,-0.81f, util.rgbToHexGenerator(colorBoxValue), colorData.textBoxColor, 0, true, false, 0.9f, 10, colorData.textBoxColorClicked, hexValTextboxMixVal);//Hex val textbox
	}


	if (panelData.paintingPanelActive) { //Icons
		glUseProgram(programs.iconsProgram); 

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

			gl.uniform1i(programs.iconsProgram,"isMaskIcon",1);
			if(maskTextures[i] == currentBrushMaskTexture){
				gl.uniform3fv(programs.iconsProgram,"iconColor",colorData.chosenBrushMaskTextureColor);
			}
			else{
				gl.uniform3fv(programs.iconsProgram,"iconColor",colorData.brushMaskIconColor);
			}
			gl.uniform1f(programs.iconsProgram,"iconMixVal",0);
			gl.activeTexture(GL_TEXTURE6);
			gl.bindTexture(maskTextures[i]);
			gl.drawArrays(buttonCoorSq,false);
			gl.uniform1i(programs.iconsProgram,"isMaskIcon",0);

			
			if(ui.isMouseOnCoords(renderData.window,mouseXpos+screenGapX*(maxScreenWidth/2),mouseYpos,buttonCoorSq,panelData.movePanel)){
				if(glfwGetMouseButton(renderData.window, 0) == GLFW_PRESS){
					gl.activeTexture(GL_TEXTURE1);
					gl.bindTexture(maskTextures[i]);
					txtr.updateMaskTexture(FBOScreen,screenSizeX,screenSizeY,brushRotationRangeBarValue,false,brushBorderRangeBarValue,brushBlurVal,outShaderData,programs,maxScreenWidth,maxScreenHeight);
					glUseProgram(programs.iconsProgram); 
					
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

		ui.iconBox(0.02f,0.03f,renderData.panelLoc / centerDivider + centerSum - screenGapX + 0.08f, -0.81f,0.9f,icons.dropperIcon,dropperMixVal,colorData.iconColor,colorData.iconColorHover);
	}


	//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
	for (size_t i = 0; i < UIElements.uiIndex.size(); i++)
	{
		std::string currentElement = UIElements.uiIndex[i];
		cout << currentElement << ' ' << i << '\n';
		std::string currentType = UIElements.uiElements[currentElement].type;
		
		
	}
	






	projection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f);
	glUseProgram(programs.iconsProgram);
	gl.uniformMatrix4fv(programs.iconsProgram, "Projection", projection);
	glUseProgram(programs.uiProgram);
	gl.uniformMatrix4fv(programs.uiProgram, "TextProjection", projection);
	uiOut.currentBrushMaskTxtr = currentBrushMaskTexture;
	return uiOut;
}
//--------------------RENDER UI --------------------\\
