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
#include "Core/Load.hpp"

#include "tinyfiledialogs.h"

bool texturePanelButtonHover = false;

float dropperMixVal = 0.0f;
float hexValTextboxMixVal = 0.0f;
float texturePanelButtonMixVal = 0.0f;


void updateButtonColorMixValues(std::vector<UIElement> &UIElements,ColorPicker &colorPicker,SndPanel &sndpanel) {
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


	sndpanel.minusSignMixVal = util.transitionEffect(sndpanel.minusSignHover,sndpanel.minusSignMixVal,phaseDifference);
	sndpanel.backSignMixVal = util.transitionEffect(sndpanel.backSignHover,sndpanel.backSignMixVal,phaseDifference);
	sndpanel.duplicateSignMixVal = util.transitionEffect(sndpanel.duplicateSignHover,sndpanel.duplicateSignMixVal,phaseDifference);
	sndpanel.downSignMixVal = util.transitionEffect(sndpanel.downSignHover,sndpanel.downSignMixVal,phaseDifference);
	sndpanel.plusSignMixVal = util.transitionEffect(sndpanel.plusSignHover,sndpanel.plusSignMixVal,phaseDifference);
	sndpanel.folderSignMixVal = util.transitionEffect(sndpanel.folderSignHover,sndpanel.folderSignMixVal,phaseDifference);
}

double lastMouseX = 0;
double lastMouseY = 0;

int alertState = 0;
std::string alertMessage = "";
int alertDuration = 1;

float sPX = 0,sPY = 0;
float dPX = 0,dPY = 0;

bool showTheSelectionBox = true;
std::vector<float> selectionBoxCoords = {};
bool selectionActive = false;


RenderOutData Render::renderUi(PanelData &panelData,RenderData& renderData,unsigned int FBOScreen,Icons &icons,
const char* exportFileName,float maskPanelSliderValue,std::vector<unsigned int> &maskTextures,double mouseXpos,double mouseYpos,int screenSizeX,int screenSizeY,
float brushBlurVal,OutShaderData &outShaderData, Model &model,std::vector<aTexture> &albedoTextures,Programs programs
,int &currentMaterialIndex,int maxScreenWidth,int maxScreenHeight, SaturationValShaderData &saturationValShaderData,unsigned int &currentBrushMaskTexture,
float materialsPanelSlideValue,std::vector<UIElement> &UIElements,ColorPicker &colorPicker,TextureDisplayer &textureDisplayer,ContextMenu &addNodeContextMenu
,NodePanel &nodePanel,SndPanel &sndPanel, int& selectedAlbedoTextureIndex,TextureSelectionPanel &textureSelectionPanel,
std::vector<NodeScene>& nodeScenes,int &selectedNodeScene,std::vector<Node> appNodes,bool &newModelAdded,std::vector<MaterialOut> &modelMaterials,bool firstClick
,ColoringPanel &coloringPanel,TextureCreatingPanel &txtrCreatingPanel,int& chosenTextureResIndex,int &chosenSkyboxTexture,bool& bakeTheMaterial,bool& anyTextureNameActive
,std::string &textureText,std::vector<NodeScene> &nodeScenesHistory,BrushMaskTextures &brushMaskTextures,bool maskPanelEnter,bool &duplicateNodeCall,Cubemaps &cubemaps
,Objects &objects) {

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
	glUseProgram(programs.rampProgram);
	gl.uniformMatrix4fv(programs.rampProgram, "TextProjection", projection);
	glUseProgram(programs.iconsProgram);
	gl.uniformMatrix4fv(programs.iconsProgram, "Projection", projection);
	glUseProgram(programs.renderTheTextureProgram);
	gl.uniformMatrix4fv(programs.renderTheTextureProgram, "TextProjection", projection);
	glUseProgram(programs.curveProgram);
	gl.uniformMatrix4fv(programs.curveProgram, "TextProjection", projection);
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

	updateButtonColorMixValues(UIElements,colorPicker,sndPanel);

	//Panel
	if(panelData.exportPanelActive || panelData.modelPanelActive || panelData.paintingPanelActive || panelData.texturePanelActive || panelData.settingsPanelActive || panelData.generatorPanelActive){ //Disable panel if a message box is active
		//If message box is not active


		//Panel changing buttons
		glUseProgram(programs.uiProgram);

		//Texture demonstrator	
		ui.textureDisplayer(textureDisplayer.ndWidth,textureDisplayer.ndHeight,textureDisplayer.buttonPosX - 1.0f +screenGapX,textureDisplayer.buttonPosY,1.f); 


		ui.nodePanel(renderData.panelLoc-  screenGapX -1.0f,sndPanel.position + screenGapX,nodePanel.heigth,programs,icons,nodeScenes,selectedNodeScene);
		ui.nodePanelBarriers(renderData.panelLoc-  screenGapX -1.0f,sndPanel.position + screenGapX,nodePanel.heigth);
		
		double xOffset = mouseXpos - lastMouseX;
		double yOffset = mouseYpos - lastMouseY;
		
		std::vector<Node> duplicatedNodes;

		nodePanel.pointerCursor = false;
		for (size_t i = 0; i < nodeScenes[selectedNodeScene].nodes.size(); i++)
		{
			if(nodePanel.heigth > 0.02){
				nodeScenes[selectedNodeScene].nodes[i].height = ((nodeScenes[selectedNodeScene].nodes[i].inputs.size() + nodeScenes[selectedNodeScene].nodes[i].rangeBarCount + nodeScenes[selectedNodeScene].nodes[i].outputs.size())/25.f + 0.07 * !nodeScenes[selectedNodeScene].nodes[i].isMainOut) * nodePanel.zoomVal;
				nodeScenes[selectedNodeScene].nodes[i].width = 0.12f * nodePanel.zoomVal;
				bool deleted = ui.node(nodeScenes[selectedNodeScene].nodes[i],programs,icons,renderData.window,mouseXpos,mouseYpos,xOffset,yOffset,maxScreenWidth,maxScreenHeight,nodeScenes[selectedNodeScene],nodePanel,textureSelectionPanel,i,albedoTextures,screenGapX,firstClick,coloringPanel,duplicateNodeCall,duplicatedNodes);
				if(deleted)
					i--;
			}
		}
		for (size_t i = 0; i < duplicatedNodes.size(); i++)
		{
			nodeScenes[selectedNodeScene].nodes.push_back(duplicatedNodes[i]);
		}
		duplicatedNodes.clear();
		


		bool anyNodeHover = false;

		for (size_t i = 0; i < nodeScenes[selectedNodeScene].nodes.size(); i++)
		{
			if(nodeScenes[selectedNodeScene].nodes[i].panelHover || nodeScenes[selectedNodeScene].nodes[i].barHover){
				anyNodeHover = true;
				break;
			}
		}
		for (size_t i = 0; i < nodeScenes[selectedNodeScene].nodes.size(); i++)
		{
			for (size_t outi = 0; outi < nodeScenes[selectedNodeScene].nodes[i].outputs.size(); outi++)
			{
				if(nodeScenes[selectedNodeScene].nodes[i].outputs[outi].connectionHover){
					anyNodeHover = true;
					break;
				}
			}
			
		}
		if(firstClick && anyNodeHover){
			showTheSelectionBox = false;
		}
		if(firstClick && !nodePanel.panelHover)
			showTheSelectionBox = false;

		if(glfwGetMouseButton(renderData.window,0) == GLFW_PRESS && nodePanel.panelHover && showTheSelectionBox){
			dPX = 0;
			dPY = 0;
			if(firstClick){
				sPX = (mouseXpos/maxScreenWidth*2 - 1.0f + screenGapX);
				sPY = (-mouseYpos/maxScreenHeight*2 + 1.0f);
			}
			dPX = (mouseXpos/maxScreenWidth*2 - 1.0f + screenGapX);
			dPY = ((-mouseYpos/maxScreenHeight*2 + 1.0f));
			if(!glfwGetKey(renderData.window,GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
				selectionBoxCoords = ui.selectionBox(true,sPX,sPY,dPX,dPY,0.3f);
			else
				ui.drawLine(sPX,sPY,0.3f,dPX,dPY,10,glm::vec4(1,0,0,1),true);
			selectionActive = true;
		}
		else{
			//sPX = (mouseXpos/screenSizeX*2 - 1.0f);
			//sPY = (-mouseYpos/maxScreenHeight*2 + 1.0f);
		}
		if(glfwGetMouseButton(renderData.window,0) == GLFW_RELEASE){
			showTheSelectionBox = true;
		}
		if(glfwGetMouseButton(renderData.window,0) == GLFW_RELEASE && selectionActive){
			selectionActive = false;
			for (size_t i = 0; i < nodeScenes[selectedNodeScene].nodes.size(); i++)
			{
				if(!glfwGetKey(renderData.window,GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS){
					
					if(selectionBoxCoords.size()){
						if(ui.isMouseOnCoords(renderData.window,((nodeScenes[selectedNodeScene].nodes[i].positionX + nodePanel.panelPositionX) * nodePanel.zoomVal) * (maxScreenWidth/2.f) + maxScreenWidth/2.f,(maxScreenHeight) - (((nodeScenes[selectedNodeScene].nodes[i].positionY + nodePanel.panelPositionY) * nodePanel.zoomVal) * (maxScreenHeight/2.f) + maxScreenHeight/2.f),selectionBoxCoords,false)){
							nodeScenes[selectedNodeScene].nodes[i].active = true;
						}
						else{
							nodeScenes[selectedNodeScene].nodes[i].active = false;
						}
					}
				}
				else{
					for (size_t outi = 0; outi < nodeScenes[selectedNodeScene].nodes[i].outputs.size(); outi++)
					{
						for (size_t coni = 0; coni < nodeScenes[selectedNodeScene].nodes[i].outputs[outi].connections.size(); coni++)
						{
							glm::vec2 conPos1;
							conPos1.x = nodeScenes[selectedNodeScene].nodes[i].outputs[outi].connections[coni].connectionPosX;
							conPos1.y = nodeScenes[selectedNodeScene].nodes[i].outputs[outi].connections[coni].connectionPosY;
							glm::vec2 conPos2;
							conPos2.x = nodeScenes[selectedNodeScene].nodes[i].outputs[outi].posX;
							conPos2.y = nodeScenes[selectedNodeScene].nodes[i].outputs[outi].posY;

							glm::vec2 conPos3 = glm::vec2(sPX,sPY);
							glm::vec2 conPos4 = glm::vec2(dPX,dPY);

							if(util.intersect(conPos1,conPos2,conPos3,conPos4)){
								if(nodeScenes[selectedNodeScene].nodes[i].outputs[outi].connections[coni].nodeConnectionIndex != 10000){
									nodeScenes[selectedNodeScene].nodes[nodeScenes[selectedNodeScene].nodes[i].outputs[outi].connections[coni].nodeConnectionIndex].inputs[nodeScenes[selectedNodeScene].nodes[i].outputs[outi].connections[coni].inputConnectionIndex].nodeConnectionIndex = 10000;
									nodeScenes[selectedNodeScene].nodes[nodeScenes[selectedNodeScene].nodes[i].outputs[outi].connections[coni].nodeConnectionIndex].inputs[nodeScenes[selectedNodeScene].nodes[i].outputs[outi].connections[coni].inputConnectionIndex].inputConnectionIndex = 10000;
								}
								nodeScenes[selectedNodeScene].nodes[i].outputs[outi].connections.erase(nodeScenes[selectedNodeScene].nodes[i].outputs[outi].connections.begin() + (coni));
								nodeScenes[selectedNodeScene].stateChanged = true;
								for (size_t severei = 0; severei < nodeScenes[selectedNodeScene].nodes.size(); severei++)
								{
									nodeScenes[selectedNodeScene].nodes[severei].stateChanged = true;
								}
								
								coni--;
							}
						}
					}
				}
			}
		}
		
		ui.panel(renderData.panelLoc-  screenGapX -1.0f , icons,panelData);
		ui.sndPanel(sndPanel.state,sndPanel.position + screenGapX,programs,icons,albedoTextures,renderData.window,mouseXpos,mouseYpos,screenGapX,maxScreenWidth,selectedAlbedoTextureIndex,nodeScenes,selectedNodeScene,newModelAdded,sndPanel.texturePanelSlideVal,sndPanel.materialPanelSlideVal,firstClick,coloringPanel,txtrCreatingPanel,anyTextureNameActive,textureText,sndPanel.activeFolderIndex,nodePanel,appNodes,sndPanel,brushMaskTextures,maskPanelEnter);


		ui.renderAlert(alertMessage,alertDuration,programs.uiProgram,alertState);		
		
		
		txtrCreatingPanel.panelPosX = sndPanel.position + screenGapX; 
		txtrCreatingPanel.panelPosY = 0.73f;

		if(txtrCreatingPanel.active)
			ui.textureCreatingPanel(txtrCreatingPanel,icons,programs,renderData.window,mouseXpos,mouseYpos, firstClick,coloringPanel,screenGapX,albedoTextures,sndPanel.activeFolderIndex);

		if(coloringPanel.active)
			ui.coloringPanel(coloringPanel,programs,icons,renderData.window,saturationValShaderData,projection,mouseXpos,mouseYpos,firstClick,xOffset,yOffset,FBOScreen,colorPicker,screenGapX);

		lastMouseX = mouseXpos;
		lastMouseY = mouseYpos;





		if(textureSelectionPanel.active)
			ui.textureSelectionPanel(textureSelectionPanel,albedoTextures,programs,renderData.window,mouseXpos,mouseYpos,screenGapX, maxScreenWidth,icons.Circle);





		//Add node context menu
		if(addNodeContextMenu.active){
			ui.container(addNodeContextMenu.positionX,addNodeContextMenu.positionY - addNodeContextMenu.height,addNodeContextMenu.positionZ,addNodeContextMenu.width,addNodeContextMenu.height,colorData.nodePanelContextMenuPanelColor,programs,icons.Circle);
			glUseProgram(programs.uiProgram); 

			int indepI = 0;

			for (size_t i = 0; i < addNodeContextMenu.buttons.size(); i++)
			{
				//TODO : Scroll after search
				if(indepI <= 9 && (i == 0||((util.isMatch(addNodeContextMenu.buttons[i].text,addNodeContextMenu.searchText) && addNodeContextMenu.searchText != "search") || (addNodeContextMenu.scroll < i && i < min(addNodeContextMenu.buttons.size(),(size_t)10)+addNodeContextMenu.scroll && addNodeContextMenu.searchText == "search")))){
					addNodeContextMenu.buttons[i].transitionMixVal = (float)addNodeContextMenu.buttons[i].hover * (float)addNodeContextMenu.buttons[i].hoverAnimationActive;

					if((addNodeContextMenu.buttons[i].hover && addNodeContextMenu.buttons[i].hoverAnimationActive) || (addNodeContextMenu.selectedButtonIndex == indepI && glfwGetKey(renderData.window,GLFW_KEY_ENTER) == GLFW_PRESS)){
						addNodeContextMenu.buttons[i].positionZ = 0.899f;
						if((glfwGetMouseButton(renderData.window, 0) == GLFW_PRESS && i != 0) || (glfwGetKey(renderData.window,GLFW_KEY_ENTER) == GLFW_PRESS && addNodeContextMenu.selectedButtonIndex == indepI && addNodeContextMenu.selectedButtonIndex != 0)){
							Node node;
							node = appNodes[i-1];
							node.active = true;
							selectionActive = false;
							showTheSelectionBox = false;
							node.stateChanged = true;
							node.positionX = (mouseXpos/screenSizeX*2 - 1.0f) / nodePanel.zoomVal - nodePanel.panelPositionX;
							node.positionY = ((-mouseYpos/maxScreenHeight*2 + 1.0f) / nodePanel.zoomVal - nodePanel.panelPositionY);						

							nodeScenesHistory.push_back(nodeScenes[selectedNodeScene]);
							nodeScenes[selectedNodeScene].nodes.push_back(node);
							addNodeContextMenu.active = false;
						}
					}
					else{
						addNodeContextMenu.buttons[i].positionZ = 0.8f;
					}
					ui.box(addNodeContextMenu.buttons[i].width, addNodeContextMenu.buttons[i].height, addNodeContextMenu.positionX, addNodeContextMenu.positionY + addNodeContextMenu.buttons[indepI].positionY, addNodeContextMenu.buttons[i].text, addNodeContextMenu.buttons[i].color, addNodeContextMenu.buttons[i].textRatio, false, false, addNodeContextMenu.buttons[i].positionZ + 0.099f * (float)(addNodeContextMenu.selectedButtonIndex == indepI), addNodeContextMenu.buttons[i].buttonCurveReduce, addNodeContextMenu.buttons[i].colorHover, ((bool)addNodeContextMenu.buttons[i].transitionMixVal || addNodeContextMenu.selectedButtonIndex == indepI)); //Add mask texture button	
					addNodeContextMenu.buttons[i].hover = ui.isMouseOnButton(renderData.window, addNodeContextMenu.buttons[i].width, addNodeContextMenu.buttons[i].height, addNodeContextMenu.positionX - screenGapX, addNodeContextMenu.buttons[indepI].positionY + addNodeContextMenu.positionY, mouseXpos, mouseYpos, false);
					
					indepI++;
				}
			}
			const bool searchButtonEnter = ui.isMouseOnButton(renderData.window,addNodeContextMenu.buttons[1].width, addNodeContextMenu.buttons[1].height, addNodeContextMenu.positionX, addNodeContextMenu.positionY - 0.265f*2.05f,mouseXpos,mouseYpos,false); 
			glm::vec4 selectedTextboxColor;
			
			if(searchButtonEnter && firstClick){
				addNodeContextMenu.searchTextboxActive = !addNodeContextMenu.searchTextboxActive;
				addNodeContextMenu.searchText = "";
			}
			
			if(!addNodeContextMenu.searchTextboxActive){
				if(searchButtonEnter)
					selectedTextboxColor = colorData.buttonColorHover;
				else
					selectedTextboxColor = colorData.buttonColor;
			}
			else if(addNodeContextMenu.searchTextboxActive){
				if(searchButtonEnter)
					selectedTextboxColor = glm::vec4(colorData.LigidPainterThemeColor,0.5f);
				else
					selectedTextboxColor = glm::vec4(colorData.LigidPainterThemeColor,1);
			}
				 
			ui.box(addNodeContextMenu.buttons[1].width, addNodeContextMenu.buttons[1].height, addNodeContextMenu.positionX, addNodeContextMenu.positionY - 0.265f*2.1, addNodeContextMenu.searchText, selectedTextboxColor, 0.05f, addNodeContextMenu.searchTextboxActive, false, 0.99f, addNodeContextMenu.buttons[1].buttonCurveReduce, selectedTextboxColor, addNodeContextMenu.searchTextboxActive); //Add mask texture button	
			glUseProgram(programs.iconsProgram);
			if(!addNodeContextMenu.searchTextboxActive)
				ui.iconBox(0.007f,0.014f,addNodeContextMenu.positionX-addNodeContextMenu.buttons[1].width*0.7, addNodeContextMenu.positionY - 0.265f*2.1, 0.9999,icons.ArrowDown,0,colorData.iconColor,colorData.iconColor);

			if(addNodeContextMenu.scroll != addNodeContextMenu.buttons.size()-10){	
				ui.iconBox(0.01f,0.02f,addNodeContextMenu.positionX, addNodeContextMenu.positionY - 0.265f*1.87f, 0.9999,icons.ArrowDown,0,colorData.iconColor,colorData.iconColor);
			}
		}
		else{
			addNodeContextMenu.scroll = 0;
			addNodeContextMenu.selectedButtonIndex = 0;
		}
		glUseProgram(programs.uiProgram);

		if(!addNodeContextMenu.active){
			addNodeContextMenu.searchText = "search";
			addNodeContextMenu.searchTextboxActive = false;
		}
	}


	if (panelData.texturePanelActive) {
		ui.modelMaterialPanel(model,programs,renderData,screenGapX,materialsPanelSlideValue,mouseXpos,mouseYpos,texturePanelButtonHover,uiOut,currentMaterialIndex,firstClick,newModelAdded,texturePanelButtonMixVal,selectedNodeScene,icons,nodeScenes);
	}


	if(addNodeContextMenu.stateChanged){
		addNodeContextMenu.positionX = mouseXpos/(maxScreenWidth/2.f) - 1.0f + screenGapX ;
		addNodeContextMenu.positionY = -mouseYpos/maxScreenHeight*2.f + 1.0f;
	}

	
	


	if (panelData.paintingPanelActive && renderData.panelLoc < 1.98f) {
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
		ui.brushMaskTexturePanel(programs,maskTextures,centerCoords,screenGapX,maskPanelSliderValue,currentBrushMaskTexture,firstClick,renderData.window,mouseXpos,mouseYpos,FBOScreen,panelData,screenSizeX,screenSizeY,uiOut,UIElements,brushBlurVal,outShaderData);
	}


	//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
	if(panelData.paintingPanelActive && renderData.panelLoc < 1.98f){
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
		if(UIElements[i].panel == 1 && panelData.modelPanelActive || UIElements[i].panel == 2 && panelData.texturePanelActive || UIElements[i].panel == 3 && panelData.paintingPanelActive || UIElements[i].panel == 4 && panelData.exportPanelActive || UIElements[i].panel == 5 && panelData.settingsPanelActive || UIElements[i].panel == 6 && panelData.generatorPanelActive || UIElements[i].panel == 0){
			panelCompatibility = true;
		}
		else{
			panelCompatibility = false;
		}
		if(renderData.panelLoc > 1.98f)
			panelCompatibility = false;

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
	
	if(panelData.generatorPanelActive){
		const float width = 0.15f;
		const float height = width * 2.f;

		const float posX = centerCoords - screenGapX;
		const float posY = 0.5f;
		const float posZ = 0.9f;
		
		std::vector<float> renderVertices = { 
			// first triangle
			 width + posX,  height + posY, posZ,1,1,0,0,0,  // top right
			 width + posX,  -height+ posY, posZ,1,0,0,0,0,  // bottom right
			 -width + posX,  height+ posY, posZ,0,1,0,0,0,  // top left 
			 
			 width + posX,  -height+ posY, posZ,1,0,0,0,0,  // bottom right
			 -width + posX,  -height+ posY, posZ,0,0,0,0,0,  // bottom left
			 -width + posX,  height+ posY, posZ,0,1,0,0,0   // top left
		};

		if(UIElements[UInormalmapCheckBoxElement].checkBox.checked){
			glUseProgram(programs.normalGenProgram);
			glm::mat4 renderTextureProjection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f);
		
			glActiveTexture(GL_TEXTURE0);

			gl.uniformMatrix4fv(programs.normalGenProgram,"renderTextureProjection",renderTextureProjection);
			gl.uniform1i(programs.normalGenProgram,"inputTexture",0);
			float rangeBarVal = (UIElements[UInormalStrengthRangeBarElement].rangeBar.value + 0.11f) * 50.f;
			gl.uniform1f(programs.normalGenProgram,"normalStrength",rangeBarVal);

		}
		else if(UIElements[UInoiseCheckBoxElement].checkBox.checked){
			glUseProgram(programs.noisyTextureProgram);

			glm::mat4 renderTextureProjection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f);
		
			glActiveTexture(GL_TEXTURE0);

			gl.uniformMatrix4fv(programs.noisyTextureProgram,"renderTextureProjection",renderTextureProjection);
			gl.uniform1i(programs.noisyTextureProgram,"inputTexture",0);
			float rangeBarVal = ((0.22f-(UIElements[UInoiseStrengthRangeBarElement].rangeBar.value + 0.11f))+0.05f)  * 50.f;
			gl.uniform1f(programs.noisyTextureProgram,"value",rangeBarVal);
		}

		gl.drawArrays(renderVertices,0);
	}
	bool skyboxlistStateChanged = false;
	if(panelData.settingsPanelActive){
		ui.listBox(centerCoords - screenGapX,0.8f,0.9f,"Texture Resolution",0.1f,icons,{"256","512","1024","2048","4096"},true,renderData.window,mouseXpos,mouseYpos,firstClick,chosenTextureResIndex,screenGapX);
		skyboxlistStateChanged = ui.listBox(centerCoords - screenGapX,-0.1f,0.9f,"Skybox",0.1f,icons,{"1","2","3","4","5","6"},true,renderData.window,mouseXpos,mouseYpos,firstClick,chosenSkyboxTexture,screenGapX);
	}
	
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
		unsigned int prefilteredMap = load.createPrefilterMap(programs,cubemaps,maxScreenWidth,maxScreenHeight);
		cubemaps.prefiltered = prefilteredMap;
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