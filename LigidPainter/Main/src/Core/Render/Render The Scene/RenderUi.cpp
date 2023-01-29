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
		else if(currentType == "icon"){
			if(UIElements[i].icon.clickable){
				UIElements[i].icon.mixVal = util.transitionEffect(UIElements[i].icon.hover,UIElements[i].icon.mixVal,phaseDifference);
			}
		}
		else if(currentType == "textBox"){
			UIElements[i].textBox.transitionMixVal = util.transitionEffect(UIElements[i].textBox.clicked,UIElements[i].textBox.transitionMixVal,phaseDifference);
		}
	}

 	dropperMixVal = util.transitionEffect(colorPicker.dropperEnter,dropperMixVal,phaseDifference);
	hexValTextboxMixVal = util.transitionEffect(colorPicker.hexValTextBoxActive,hexValTextboxMixVal,phaseDifference);
 	texturePanelButtonMixVal = util.transitionEffect(texturePanelButtonHover,texturePanelButtonMixVal,phaseDifference);
}

double lastMouseX = 0;
double lastMouseY = 0;

int alertState = 0;
std::string alertMessage = "";
int alertDuration = 1;

RenderOutData Render::renderUi(PanelData &panelData,RenderData& renderData,unsigned int FBOScreen,Icons &icons,
const char* exportFileName,float maskPanelSliderValue,std::vector<unsigned int> &maskTextures,double mouseXpos,double mouseYpos,int screenSizeX,int screenSizeY,
float brushBlurVal,OutShaderData &outShaderData, Model &model,vector<aTexture> &albedoTextures,Programs programs
,int &currentMaterialIndex,int maxScreenWidth,int maxScreenHeight, SaturationValShaderData &saturationValShaderData,unsigned int &currentBrushMaskTexture,
float materialsPanelSlideValue,std::vector<UIElement> &UIElements,ColorPicker &colorPicker,TextureDisplayer &textureDisplayer,ContextMenu &addNodeContextMenu
,NodePanel &nodePanel,SndPanel &sndPanel, int& selectedAlbedoTextureIndex,TextureSelectionPanel &textureSelectionPanel,
std::vector<NodeScene>& nodeScenes,int &selectedNodeScene,std::vector<Node> appNodes,bool &newModelAdded,std::vector<MaterialOut> &modelMaterials,bool firstClick
,ColoringPanel &coloringPanel,TextureCreatingPanel &txtrCreatingPanel) {

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
	glUseProgram(programs.renderTheTextureProgram);
	gl.uniformMatrix4fv(programs.uiProgram, "TextProjection", projection);
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


		//Panel changing buttons
		glUseProgram(programs.uiProgram);

		//Texture demonstrator	
		ui.textureDisplayer(textureDisplayer.ndWidth,textureDisplayer.ndHeight,textureDisplayer.buttonPosX - 1.0f +screenGapX,textureDisplayer.buttonPosY,1.f); 


		ui.nodePanel(renderData.panelLoc-  screenGapX -1.0f,sndPanel.position + screenGapX,nodePanel.heigth,programs,icons,nodeScenes,selectedNodeScene);
		ui.nodePanelBarriers(renderData.panelLoc-  screenGapX -1.0f,sndPanel.position + screenGapX,nodePanel.heigth);
		
		double xOffset = mouseXpos - lastMouseX;
		double yOffset = mouseYpos - lastMouseY;
		

		for (size_t i = 0; i < nodeScenes[selectedNodeScene].nodes.size(); i++)
		{
			nodeScenes[selectedNodeScene].nodes[i].height = (nodeScenes[selectedNodeScene].nodes[i].inputs.size() + nodeScenes[selectedNodeScene].nodes[i].rangeBarCount + nodeScenes[selectedNodeScene].nodes[i].outputs.size())/22.f * nodePanel.zoomVal;
			nodeScenes[selectedNodeScene].nodes[i].width = 0.12f * nodePanel.zoomVal;
			ui.node(nodeScenes[selectedNodeScene].nodes[i],programs,icons,renderData.window,mouseXpos,mouseYpos,xOffset,yOffset,maxScreenWidth,maxScreenHeight,nodeScenes[selectedNodeScene],nodePanel,textureSelectionPanel,i,albedoTextures,screenGapX,firstClick,coloringPanel);
		}
		
		ui.panel(renderData.panelLoc-  screenGapX -1.0f , icons);
		ui.sndPanel(sndPanel.state,sndPanel.position + screenGapX,programs,icons,albedoTextures,renderData.window,mouseXpos,mouseYpos,screenGapX,maxScreenWidth,selectedAlbedoTextureIndex,nodeScenes,selectedNodeScene,newModelAdded,sndPanel.texturePanelSlideVal,sndPanel.materialPanelSlideVal,firstClick);



		ui.renderAlert(alertMessage,alertDuration,programs.uiProgram,alertState);		
		
		
		txtrCreatingPanel.panelPosX = sndPanel.position + screenGapX; 
		txtrCreatingPanel.panelPosY = 0.73f;

		if(txtrCreatingPanel.active)
			ui.textureCreatingPanel(txtrCreatingPanel,icons,programs,renderData.window,mouseXpos,mouseYpos, firstClick,coloringPanel,screenGapX,albedoTextures);

		if(coloringPanel.active)
			ui.coloringPanel(coloringPanel,programs,icons,renderData.window,saturationValShaderData,projection,mouseXpos,mouseYpos,firstClick,xOffset,yOffset,FBOScreen,colorPicker);

		lastMouseX = mouseXpos;
		lastMouseY = mouseYpos;





		if(textureSelectionPanel.active)
			ui.textureSelectionPanel(textureSelectionPanel,albedoTextures,programs,renderData.window,mouseXpos,mouseYpos,screenGapX, maxScreenWidth,icons.Circle);





		//Add node context menu
		if(addNodeContextMenu.active){
		ui.container(addNodeContextMenu.positionX,addNodeContextMenu.positionY,addNodeContextMenu.positionZ,addNodeContextMenu.width,addNodeContextMenu.height,colorData.nodePanelContextMenuPanelColor,programs,icons.Circle);
		glUseProgram(programs.uiProgram); 

			for (size_t i = 0; i < addNodeContextMenu.buttons.size(); i++)
			{
				addNodeContextMenu.buttons[i].transitionMixVal = (float)addNodeContextMenu.buttons[i].hover * (float)addNodeContextMenu.buttons[i].hoverAnimationActive;

				if(addNodeContextMenu.buttons[i].hover && addNodeContextMenu.buttons[i].hoverAnimationActive){
					addNodeContextMenu.buttons[i].positionZ = 0.999f;
					if(glfwGetMouseButton(renderData.window, 0) == GLFW_PRESS && i != 0){
						Node node;
						node = appNodes[i-1];
						node.positionX = (mouseXpos/screenSizeX*2 - 1.0f) / nodePanel.zoomVal - nodePanel.panelPositionX;
						node.positionY = (-mouseYpos/maxScreenHeight*2 + 1.0f) / nodePanel.zoomVal - nodePanel.panelPositionY;						

						nodeScenes[selectedNodeScene].nodes.push_back(node);
						addNodeContextMenu.active = false;
					}
				}
				else{
					addNodeContextMenu.buttons[i].positionZ = 0.99f;
				}
				ui.box(addNodeContextMenu.buttons[i].width, addNodeContextMenu.buttons[i].height, addNodeContextMenu.positionX, addNodeContextMenu.positionY + addNodeContextMenu.buttons[i].positionY, addNodeContextMenu.buttons[i].text, addNodeContextMenu.buttons[i].color, addNodeContextMenu.buttons[i].textRatio, false, false, addNodeContextMenu.buttons[i].positionZ, addNodeContextMenu.buttons[i].buttonCurveReduce, addNodeContextMenu.buttons[i].colorHover, addNodeContextMenu.buttons[i].transitionMixVal); //Add mask texture button	
			}
		}
		glUseProgram(programs.uiProgram);
	}


	if (panelData.texturePanelActive) {
		bool mouseEnteredOnce = false;
		glUseProgram(programs.uiProgram); 

		if(model.meshes.size() == 0){
			ui.renderText(programs.uiProgram, "Materials of the 3D model", renderData.panelLoc - 1.0f - screenGapX  + 0.095f, 0.8f, 0.00022f,colorData.textColor,0.99999f,false);
			ui.renderText(programs.uiProgram, "will be show up there", renderData.panelLoc - 1.0f - screenGapX  + 0.115f, 0.75f, 0.00022f,colorData.textColor,0.99999f,false);
		}
		for (int i = 0; i < model.meshes.size(); i++)//Render buttons
		{ 	
			//Check if mouse is entered the related button
			bool textureButtonEnter = ui.isMouseOnButton(renderData.window, 0.2f, 0.06f, renderData.panelLoc - 1.0f - screenGapX*2 + 0.205f,0.8f - (i * 0.125f) + materialsPanelSlideValue, mouseXpos, mouseYpos,false);
			bool textureAddButtonEnter = ui.isMouseOnButton(renderData.window, 0.02f, 0.03f, renderData.panelLoc - 1.0f - screenGapX*2 + 0.3f,0.8f - (i * 0.125f) + materialsPanelSlideValue, mouseXpos, mouseYpos,false);
			
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
				}

				glm::vec4 currentColor;
				glm::vec4 currentColorHover;
				if(i == currentMaterialIndex){
					currentColor = colorData.materialButtonColorActive;
					currentColorHover = colorData.materialButtonColorActiveHover;
				}
				else{
					currentColor = colorData.materialButtonColor;
					currentColorHover = colorData.materialButtonColorHover;
				}
				//Button (Hover)
				ui.box(0.2f, 0.06f, renderData.panelLoc - 1.0f - screenGapX + 0.205f, 0.8f - (i * 0.125f) + materialsPanelSlideValue, model.meshes[i].materialName, currentColor, 0.2f, false, false, 0.9f, 10000, currentColorHover, texturePanelButtonMixVal);

			}
			else{
				glm::vec4 currentColor;
				glm::vec4 currentColorHover;
				if(i == currentMaterialIndex){
					currentColor = colorData.materialButtonColorActive;
					currentColorHover = colorData.materialButtonColorActiveHover;
				}
				else{
					currentColor = colorData.materialButtonColor;
					currentColorHover = colorData.materialButtonColorHover;
				}

				//Button 
				ui.box(0.2f, 0.06f, renderData.panelLoc - 1.0f - screenGapX + 0.205f, 0.8f - (i * 0.125f) + materialsPanelSlideValue, model.meshes[i].materialName, currentColor, 0.2f, false, false, 0.9f, 10000, currentColorHover, 0);
			}
			if(textureAddButtonEnter){
				//Hover
				//uiOut.texturePanelButtonHover = true;
				if(firstClick){
					//Pressed
					//uiOut.texturePanelButtonClicked = true;
					currentMaterialIndex = i;
					newModelAdded = true; 

					model.meshes[i].materialIndex = selectedNodeScene;
				}
			}
			//ui.box(0.03f, 0.06f, renderData.panelLoc - screenGapX + 0.3f, 0.8f - (i * 0.125f), "", colorData.buttonColorHover, 0.048f, true, false, 0.6f, 10000, colorData.buttonColorHover, 0); 
			if(model.meshes[i].materialIndex != 10000)
				ui.renderText(programs.uiProgram,std::to_string(model.meshes[i].materialIndex),renderData.panelLoc - 1.0f - screenGapX + 0.235f,0.8f - (i * 0.125f) + materialsPanelSlideValue - 0.02,0.00022,colorData.materialIconIndexTextColor,0.99999f,false);
			
			glUseProgram(programs.iconsProgram);
			if(model.meshes[i].materialIndex != 10000)
				ui.iconBox(0.02f,0.04f,renderData.panelLoc - 1.0f - screenGapX + 0.255f ,0.8f - (i * 0.125f) + materialsPanelSlideValue,0.99f,icons.Material,0,colorData.iconColor,colorData.iconColor);
			float iconmixVal = 0.0f;
			if(textureAddButtonEnter)
				iconmixVal = 0.5f;
				
			ui.iconBox(0.02f,0.03f,renderData.panelLoc - 1.0f - screenGapX + 0.3f ,0.8f - (i * 0.125f) + materialsPanelSlideValue,0.99f,icons.AddTexture,iconmixVal,colorData.iconColor,colorData.iconColorHover);
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

	
	


	if (panelData.paintingPanelActive) {
		glUseProgram(programs.uiProgram); 

		//Color Picker
		if(colorPicker.updateHueVal)
			colorPicker.hueColorValue = ui.hueBar(centerCoords - screenGapX + 0.1f, -0.55f, colorPicker.hueValue, FBOScreen, renderData.window,projection,colorPicker.updateHueVal); 
		else
			ui.hueBar(centerCoords - screenGapX + 0.1f, -0.55f, colorPicker.hueValue, FBOScreen, renderData.window,projection,colorPicker.updateHueVal); 


		saturationValShaderData.boxColor = colorPicker.hueColorValue / 255.0f;
		saturationValShaderData.renderTextureProjection = projection;
		gl.useSaturationValBoxShader(programs.saturationValBoxProgram,saturationValShaderData);
		ui.colorBox(centerCoords - screenGapX - 0.02f, -0.55f, colorPicker.saturationValuePosX, colorPicker.saturationValuePosY,icons);
		
		ui.box(0.04f, 0.03f, centerCoords - screenGapX - 0.008f,-0.81f, util.rgbToHexGenerator(colorPicker.pickerValue), colorData.textBoxColor, 0, true, false, 0.9f, 10, colorData.textBoxColorClicked, hexValTextboxMixVal);//Hex val textbox
		
		glUseProgram(programs.iconsProgram);
		ui.iconBox(0.02*1.2,0.0364f*1.2,centerCoords - screenGapX - 0.095f,-0.81f,0.9f,icons.Circle,0,colorData.panelColorSnd,glm::vec4(0));
		ui.iconBox(0.02,0.0364f,centerCoords - screenGapX - 0.095f,-0.81f,0.91f,icons.Circle,0,glm::vec4(colorPicker.pickerValue / glm::vec3(255),1.0f),glm::vec4(0));
		ui.iconBox(0.02f,0.03f,centerCoords - screenGapX + 0.08f, -0.81f,0.9f,icons.dropperIcon,dropperMixVal,colorData.iconColor,colorData.iconColorHover);
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
				if(firstClick){
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
			
			const int uiIconStartingIndex = 30; 
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
				ui.renderText(programs.uiProgram,UIElements[i].text.text, centerCoords - screenGapX + UIElements[i].text.positionX, UIElements[i].text.positionY, UIElements[i].text.scale,colorData.textColor,0.99999f,false);
			}

			if(currentType == "rangeBar"){
				ui.rangeBar(centerCoords - screenGapX + UIElements[i].rangeBar.positionX, UIElements[i].rangeBar.positionY, UIElements[i].rangeBar.value);
			}

			if(currentType == "textBox"){
				ui.box(UIElements[i].textBox.width, UIElements[i].textBox.height,centerCoords - screenGapX + UIElements[i].textBox.position_x, UIElements[i].textBox.position_y,UIElements[i].textBox.text , colorData.textBoxColor, 0 , true, false, UIElements[i].textBox.position_z, 10 , colorData.textBoxColorClicked, UIElements[i].textBox.transitionMixVal); //Add mask texture button
			}

			if(currentType == "checkBox"){
				ui.checkBox(centerCoords - screenGapX + UIElements[i].checkBox.positionX, UIElements[i].checkBox.positionY, UIElements[i].checkBox.text,  UIElements[i].checkBox.mouseHover,  UIElements[i].checkBox.checked,icons.Circle); //jpg checkbox
			}
			if(currentType == "icon"){
				ui.iconBox(UIElements[i].icon.width,UIElements[i].icon.height,centerCoords - screenGapX + UIElements[i].icon.positionX ,UIElements[i].icon.positionY,UIElements[i].icon.positionZ,UIElements[i].icon.icon, UIElements[i].icon.mixVal , UIElements[i].icon.color , UIElements[i].icon.colorHover);
			}
		}
	}	

	alertState = 0;


	uiOut.currentBrushMaskTxtr = currentBrushMaskTexture;
	return uiOut;
}
//--------------------RENDER UI --------------------\\

void UserInterface::alert(std::string message,int duration){
	alertState = 1;
	alertMessage = message;
	alertDuration = duration;
}