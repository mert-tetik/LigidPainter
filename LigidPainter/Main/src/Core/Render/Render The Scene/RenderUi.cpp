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

float dropperMixVal = 0.0f;
float hexValTextboxMixVal = 0.0f;
float texturePanelButtonMixVal = 0.0f;


void updateButtonColorMixValues(std::vector<UIElement> &UIElements,ColorPicker &colorPicker) {
	Utilities util;

	const float phaseDifference = 0.1f;

	for (size_t i = 0; i < UIElements.size(); i++)
	{
		std::string currentType = UIElements[i].type;
		
		if(currentType == "button"){
			UIElements[i].button.transitionMixVal = util.transitionEffect(UIElements[i].button.hover,UIElements[i].button.transitionMixVal,phaseDifference);
		}
		else if(currentType == "textBox"){
			UIElements[i].textBox.transitionMixVal = util.transitionEffect(UIElements[i].textBox.clicked,UIElements[i].textBox.transitionMixVal,phaseDifference);
		}
	}

 	dropperMixVal = util.transitionEffect(colorPicker.dropperEnter,dropperMixVal,phaseDifference);
	hexValTextboxMixVal = util.transitionEffect(colorPicker.hexValTextBoxActive,hexValTextboxMixVal,phaseDifference);
 	texturePanelButtonMixVal = util.transitionEffect(texturePanelButtonHover,texturePanelButtonMixVal,phaseDifference);
}

RenderOutData Render::renderUi(PanelData &panelData,RenderData& renderData,unsigned int FBOScreen,Icons &icons,
const char* exportFileName,float maskPanelSliderValue,std::vector<unsigned int> &maskTextures,double mouseXpos,double mouseYpos,int screenSizeX,int screenSizeY,
float brushBlurVal,OutShaderData &outShaderData, Model &model,vector<unsigned int> &albedoTextures,Programs programs
,int &currentMaterialIndex,int maxScreenWidth,int maxScreenHeight, SaturationValShaderData &saturationValShaderData,unsigned int &currentBrushMaskTexture,
float materialsPanelSlideValue,std::vector<UIElement> &UIElements,ColorPicker &colorPicker,TextureDisplayer &textureDisplayer,ContextMenu &addNodeContextMenu
,NodePanel &nodePanel) {

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



		//Change the projection
	float centerDivider;
	float centerSum;
	centerDivider = 2.0f;
	centerSum = 0;
	projection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f);
	glUseProgram(programs.iconsProgram);
	gl.uniformMatrix4fv(programs.iconsProgram, "Projection", projection);
	glUseProgram(programs.uiProgram);
	gl.uniformMatrix4fv(programs.uiProgram, "TextProjection", projection);

	float centerCoords = (renderData.panelLoc + max(renderData.panelLoc - 1.7f,0.0f)) / centerDivider + centerSum;


	//Texture demonstrator transition animation
	if(textureDisplayer.buttonClicked){
		if(textureDisplayer.changeState){
			textureDisplayer.changeState = false;
		}
		else{
			textureDisplayer.changeState = true;
		}
	}
	if(textureDisplayer.changeState){
		textureDisplayer.ndWidth -= 0.035f;
		textureDisplayer.ndHeight -= 0.07f;
		
		if(textureDisplayer.ndWidth < 0.0f){
			textureDisplayer.ndWidth = 0.0f;
		}
		if(textureDisplayer.ndHeight < 0.0f){
			textureDisplayer.ndHeight = 0.0f;
		}
	}
	else{
		textureDisplayer.ndWidth += 0.035f;
		textureDisplayer.ndHeight += 0.07f;
		if(textureDisplayer.ndWidth > textureDisplayer.width){
			textureDisplayer.ndWidth = textureDisplayer.width;
		}
		if(textureDisplayer.ndHeight > textureDisplayer.height){
			textureDisplayer.ndHeight = textureDisplayer.height;
		}
	}


	float screenGapX = ((float)maxScreenWidth - screenSizeX)/(((float)maxScreenWidth)/2.0f)/2.0f; 

	updateButtonColorMixValues(UIElements,colorPicker);

	//Panel
	if(panelData.exportPanelActive || panelData.modelPanelActive || panelData.paintingPanelActive || panelData.texturePanelActive){ //Disable panel if a message box is active
		//If message box is not active
		ui.panel(renderData.panelLoc-  screenGapX -1.0f , 0);
		ui.nodePanel(renderData.panelLoc - 1.0f - screenGapX,nodePanel.heigth);

		//Projection that is used for panel (Use that projection if things will move with panel (and will not be centered) or will be moved freely)

		//Panel changing buttons
		glUseProgram(programs.uiProgram);
		ui.panelChangeButton(renderData.panelLoc - 1.0f - screenGapX, 0.8f);//Model Panel
		ui.panelChangeButton(renderData.panelLoc - 1.0f - screenGapX, 0.72f);//Texture Panel
		ui.panelChangeButton(renderData.panelLoc - 1.0f - screenGapX, 0.64f);//Painting Panel
		ui.panelChangeButton(renderData.panelLoc - 1.0f - screenGapX, 0.56f);//Export Panel

		//Texture demonstrator	
		ui.textureDisplayer(textureDisplayer.ndWidth,textureDisplayer.ndHeight,textureDisplayer.buttonPosX - 1.0f +screenGapX,textureDisplayer.buttonPosY,0.9999f); 

		//Panel changing button's icons
		glUseProgram(programs.iconsProgram);
		ui.iconBox(0.015f,0.02f,renderData.panelLoc - 1.01f - screenGapX, 0.795f , 0.9f , icons.TDModel , 0.0f , colorData.iconColor , colorData.iconColorHover);
		ui.iconBox(0.015f,0.02f,renderData.panelLoc - 1.01f - screenGapX, 0.715f , 0.9f , icons.Material , 0.0f , colorData.iconColor , colorData.iconColorHover);
		ui.iconBox(0.015f,0.02f,renderData.panelLoc - 1.01f - screenGapX, 0.635f , 0.9f , icons.Painting , 0.0f , colorData.iconColor , colorData.iconColorHover);
		ui.iconBox(0.015f,0.02f,renderData.panelLoc - 1.01f - screenGapX, 0.555f , 0.9f , icons.Export , 0.0f , colorData.iconColor , colorData.iconColorHover);

		glUseProgram(programs.uiProgram);
	}


	if (panelData.texturePanelActive) {
		bool mouseEnteredOnce = false;
		glUseProgram(programs.uiProgram); 

		if(model.meshes.size() == 0){
			ui.renderText(programs.uiProgram, "Materials of the 3D model", renderData.panelLoc - 1.0f - screenGapX  + 0.095f, 0.8f, 0.00022f);
			ui.renderText(programs.uiProgram, "will be show up there", renderData.panelLoc - 1.0f - screenGapX  + 0.115f, 0.75f, 0.00022f);
		}
		for (int i = 0; i < model.meshes.size(); i++)//Render buttons
		{ 	
			//Check if mouse is entered the related button
			bool textureButtonEnter = ui.isMouseOnButton(renderData.window, 0.2f, 0.06f, renderData.panelLoc - 1.0f - screenGapX*2 + 0.205f,0.8f - (i * 0.125f) + materialsPanelSlideValue, mouseXpos, mouseYpos,true);
			bool textureAddButtonEnter = ui.isMouseOnButton(renderData.window, 0.02f, 0.03f, renderData.panelLoc - 1.0f - screenGapX*2 + 0.3f,0.8f - (i * 0.125f) + materialsPanelSlideValue, mouseXpos, mouseYpos,true);
			
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
				ui.box(0.2f, 0.06f, renderData.panelLoc - 1.0f - screenGapX + 0.205f, 0.8f - (i * 0.125f) + materialsPanelSlideValue, model.meshes[i].materialName, colorData.buttonColor, 0.048f, true, false, 0.9f, 10000, colorData.buttonColorHover, texturePanelButtonMixVal);

			}
			else{
				//Button 
				ui.box(0.2f, 0.06f, renderData.panelLoc - 1.0f - screenGapX + 0.205f, 0.8f - (i * 0.125f) + materialsPanelSlideValue, model.meshes[i].materialName, colorData.buttonColor, 0.048f, true, false, 0.9f, 10000, colorData.buttonColorHover, 0);

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
				
			ui.iconBox(0.02f,0.025f,renderData.panelLoc - 1.0f - screenGapX + 0.3f ,0.8f - (i * 0.125f) + materialsPanelSlideValue,0.99f,icons.AddTexture,iconmixVal,colorData.iconColor,colorData.iconColorHover);
			glUseProgram(programs.uiProgram); 
		}
		if(!mouseEnteredOnce){
			texturePanelButtonHover = false;
		}
	}


	if(addNodeContextMenu.stateChanged){
		addNodeContextMenu.positionX = mouseXpos/(maxScreenWidth/2.f) - 1.0f + screenGapX ;
		addNodeContextMenu.positionY = -mouseYpos/maxScreenHeight*2.f + 1.0f;
	}

	if(addNodeContextMenu.active){
		ui.container(addNodeContextMenu.positionX,addNodeContextMenu.positionY,addNodeContextMenu.positionZ,addNodeContextMenu.width,addNodeContextMenu.height,colorData.nodePanelContextMenuPanelColor,programs,icons.Circle);
		glUseProgram(programs.uiProgram); 

		for (size_t i = 0; i < addNodeContextMenu.buttons.size(); i++)
		{
			ui.box(addNodeContextMenu.buttons[i].width, addNodeContextMenu.buttons[i].height, addNodeContextMenu.positionX, addNodeContextMenu.positionY + addNodeContextMenu.buttons[i].positionY, addNodeContextMenu.buttons[i].text, addNodeContextMenu.buttons[i].color, addNodeContextMenu.buttons[i].textRatio, false, false, addNodeContextMenu.buttons[i].positionZ, addNodeContextMenu.buttons[i].buttonCurveReduce, addNodeContextMenu.buttons[i].colorHover, addNodeContextMenu.buttons[i].transitionMixVal); //Add mask texture button	
		}
	}
	


	if (panelData.paintingPanelActive) {
		glUseProgram(programs.uiProgram); 


		//Color Picker
		colorPicker.hueColorValue = ui.hueBar(centerCoords - screenGapX + 0.1f, -0.55f, colorPicker.hueValue, FBOScreen, renderData.window,projection,colorPicker.updateHueVal); 
		saturationValShaderData.boxColor = colorPicker.hueColorValue / 255.0f;
		saturationValShaderData.renderTextureProjection = projection;
		gl.useSaturationValBoxShader(programs.saturationValBoxProgram,saturationValShaderData);
		ui.colorBox(centerCoords - screenGapX - 0.02f, -0.55f, colorPicker.saturationValuePosX, colorPicker.saturationValuePosY);
		ui.box(0.002f, 0.025f, centerCoords - screenGapX - 0.095f, -0.81f, "", glm::vec4(colorPicker.pickerValue / glm::vec3(255),1.0f), 0.075f, false, false, 0.9f, 12, glm::vec4(0), 0); //indicator for picken color of the color picker
		ui.box(0.004f, 0.035f, centerCoords - screenGapX - 0.095f, -0.81f, "", colorData.panelColorSnd, 0.075f, false, false, 0.9f, 10, glm::vec4(0), 0); //decoration
		ui.box(0.04f, 0.03f, centerCoords - screenGapX - 0.008f,-0.81f, util.rgbToHexGenerator(colorPicker.pickerValue), colorData.textBoxColor, 0, true, false, 0.9f, 10, colorData.textBoxColorClicked, hexValTextboxMixVal);//Hex val textbox
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
			float position_x = centerCoords - screenGapX - maskXpos - 0.175f;
			float position_y = 0.8f + maskYpos - maskPanelSliderValue*(maskTextures.size()/4) - 0.05f;
			//ui.iconBox(0.025f, 0.05f,centerCoords - screenGapX - maskXpos - 0.2f,0.8f + maskYpos - maskPanelSliderValue*(maskTextures.size()/4) - 0.05f,1,maskTextures[i],0);
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
				gl.uniform4fv(programs.iconsProgram,"iconColor",glm::vec4(colorData.chosenBrushMaskTextureColor,1.0f));
			}
			else{
				gl.uniform4fv(programs.iconsProgram,"iconColor",colorData.brushMaskIconColor);
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
					txtr.updateMaskTexture(FBOScreen,screenSizeX,screenSizeY,UIElements[UIbrushRotationRangeBar].rangeBar.value,false,UIElements[UIbrushBordersRangeBar].rangeBar.value,brushBlurVal,outShaderData,programs,maxScreenWidth,maxScreenHeight);
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

		ui.iconBox(0.02f,0.03f,centerCoords - screenGapX + 0.08f, -0.81f,0.9f,icons.dropperIcon,dropperMixVal,colorData.iconColor,colorData.iconColorHover);
	}


	//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
	if(panelData.paintingPanelActive){
		glUseProgram(programs.uiProgram);

		const float maskPanelRange = ceil((int)maskTextures.size()/3.f) / 8.33333333333 - (0.8f - 0.55f); 
		
		ui.verticalRangeBar( renderData.panelLoc / centerDivider + centerSum - screenGapX + 0.13f,0.8f,0.125,(0.25f / (maskPanelRange/4.f+0.001f)) * (maskPanelSliderValue*-1.f));

		ui.box(0.035f, 0.07f, centerCoords - screenGapX - 0.1f, 0.42f, "", colorData.buttonColor, 0.075f, false, true, 0.9f, 1000, glm::vec4(0), 0);
	}

	bool usingUiProgram = false;
	bool usingIconProgram = false;

	
	for (size_t i = 0; i < UIElements.size(); i++)
	{
		std::string currentType = UIElements[i].type;
				
		if(UIElements[i].attachedToMainPanel == false){
			centerCoords =  renderData.panelLoc - 1.0f;
		}
		else{
			centerCoords = (renderData.panelLoc + max(renderData.panelLoc - 1.7f,0.0f)) / centerDivider + centerSum;
		}


		bool panelCompatibility;
		if(UIElements[i].panel == 1 && panelData.modelPanelActive || UIElements[i].panel == 2 && panelData.texturePanelActive || UIElements[i].panel == 3 && panelData.paintingPanelActive || UIElements[i].panel == 4 && panelData.exportPanelActive || UIElements[i].panel == 0){
			panelCompatibility = true;
		}
		else{
			panelCompatibility = false;
		}
		if(panelCompatibility){
			
			const int uiIconStartingIndex = 32; 
			if(i < uiIconStartingIndex && !usingUiProgram){
				glUseProgram(programs.uiProgram);
				usingUiProgram = true;
			}
			else if(i >= uiIconStartingIndex && !usingIconProgram){
				glUseProgram(programs.iconsProgram);
				usingIconProgram = true;
			}

			if(currentType == "button"){
				ui.box(UIElements[i].button.width, UIElements[i].button.height, centerCoords - screenGapX + UIElements[i].button.positionX, UIElements[i].button.positionY, UIElements[i].button.text, UIElements[i].button.color, UIElements[i].button.textRatio, false, false, UIElements[i].button.positionZ, UIElements[i].button.buttonCurveReduce, UIElements[i].button.colorHover, UIElements[i].button.transitionMixVal); //Add mask texture button
			}
			
			if(currentType == "text"){	
				ui.renderText(programs.uiProgram,UIElements[i].text.text, centerCoords - screenGapX + UIElements[i].text.positionX, UIElements[i].text.positionY, UIElements[i].text.scale);
			}

			if(currentType == "rangeBar"){
				ui.rangeBar(centerCoords - screenGapX + UIElements[i].rangeBar.positionX, UIElements[i].rangeBar.positionY, UIElements[i].rangeBar.value);
			}

			if(currentType == "textBox"){
				ui.box(UIElements[i].textBox.width, UIElements[i].textBox.height,centerCoords - screenGapX + UIElements[i].textBox.position_x, UIElements[i].textBox.position_y,UIElements[i].textBox.text , colorData.textBoxColor, 0 , true, false, UIElements[i].textBox.position_z, 10 , colorData.textBoxColorClicked, UIElements[i].textBox.transitionMixVal); //Add mask texture button
			}

			if(currentType == "checkBox"){
				ui.checkBox(centerCoords - screenGapX + UIElements[i].checkBox.positionX, UIElements[i].checkBox.positionY, UIElements[i].checkBox.text,  UIElements[i].checkBox.mouseHover,  UIElements[i].checkBox.checked); //jpg checkbox
			}
			if(currentType == "icon"){
				ui.iconBox(UIElements[i].icon.width,UIElements[i].icon.height,centerCoords - screenGapX + UIElements[i].icon.positionX ,UIElements[i].icon.positionY,UIElements[i].icon.positionZ,UIElements[i].icon.icon, 0.0f , colorData.iconColor , colorData.iconColorHover);
			}
		}
	}	

	uiOut.currentBrushMaskTxtr = currentBrushMaskTexture;
	return uiOut;
}
//--------------------RENDER UI --------------------\\
