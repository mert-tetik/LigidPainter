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

#include "LibAL.h"


float dropperMixVal = 0.0f;
float hexValTextboxMixVal = 0.0f;

unsigned int generatedTextTxtr = 0;

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
		else if(currentType == "rangeBar"){
			UIElements[i].rangeBar.mixVal = util.transitionEffect(UIElements[i].rangeBar.pressed,UIElements[i].rangeBar.mixVal,phaseDifference);
		}
	}

 	dropperMixVal = util.transitionEffect(colorPicker.dropperEnter,dropperMixVal,phaseDifference);
	hexValTextboxMixVal = util.transitionEffect(colorPicker.hexValTextBoxActive,hexValTextboxMixVal,phaseDifference);


	sndpanel.minusSignMixVal = util.transitionEffect(sndpanel.minusSignHover,sndpanel.minusSignMixVal,phaseDifference);
	sndpanel.backSignMixVal = util.transitionEffect(sndpanel.backSignHover,sndpanel.backSignMixVal,phaseDifference);
	sndpanel.duplicateSignMixVal = util.transitionEffect(sndpanel.duplicateSignHover,sndpanel.duplicateSignMixVal,phaseDifference);
	sndpanel.downSignMixVal = util.transitionEffect(sndpanel.downSignHover,sndpanel.downSignMixVal,phaseDifference);
	sndpanel.plusSignMixVal = util.transitionEffect(sndpanel.plusSignHover,sndpanel.plusSignMixVal,phaseDifference);
	sndpanel.folderSignMixVal = util.transitionEffect(sndpanel.folderSignHover,sndpanel.folderSignMixVal,phaseDifference);
}

double lastMouseX = 0;
double lastMouseY = 0;

bool alertIsSuccess = false;
int alertState = 0;
std::string alertMessage = "";
int alertDuration = 1;

float sPX = 0,sPY = 0;
float dPX = 0,dPY = 0;

bool showTheSelectionBox = true;
std::vector<float> selectionBoxCoords = {};
bool selectionActive = false;

int previousTextureResIndex;

Audios uiAudios;

RenderOutData Render::renderUi(PanelData &panelData,RenderData& renderData,unsigned int FBOScreen,Icons &icons,
const char* exportFileName,double mouseXpos,double mouseYpos,int screenSizeX,int screenSizeY,
float brushBlurVal,OutShaderData &outShaderData, Model &model,std::vector<aTexture> &albedoTextures,Programs programs
,int &currentMaterialIndex,int removeThisParam,int removeThisParam2, SaturationValShaderData &saturationValShaderData,unsigned int &currentBrushMaskTexture,
float materialsPanelSlideValue,std::vector<UIElement> &UIElements,ColorPicker &colorPicker,TextureDisplayer &textureDisplayer,ContextMenu &addNodeContextMenu
,NodePanel &nodePanel,SndPanel &sndPanel, int& selectedAlbedoTextureIndex,TextureSelectionPanel &textureSelectionPanel,
std::vector<NodeScene>& nodeScenes,int &selectedNodeScene,std::vector<Node> appNodes,bool &newModelAdded,std::vector<MaterialOut> &modelMaterials,bool &firstClick
,ColoringPanel &coloringPanel,TextureCreatingPanel &txtrCreatingPanel,int& chosenTextureResIndex,int &chosenSkyboxTexture,bool& bakeTheMaterial,bool& anyTextureNameActive
,std::string &textureText,std::vector<NodeScene> &nodeScenesHistory,BrushTexture &brushMaskTextures,bool maskPanelEnter,bool &duplicateNodeCall,Cubemaps &cubemaps
,Objects &objects,glm::vec3 screenHoverPixel,int &chosenNodeResIndex,Audios audios,bool &textureDraggingState) {
	uiAudios = audios;

	ColorData colorData;
	glm::mat4 projection;
	UserInterface ui;
	Utilities util;
    GlSet gl; 
	RenderOutData uiOut;
	Texture txtr;
	uiOut.maskPanelMaskClicked = false;
	uiOut.maskPanelMaskHover = false;

	float centerDivider;
	float centerSum;
	centerDivider = 2.0f;
	centerSum = 0;
	glm::mat4 scprojection = glm::ortho(0.f, (float)glfwGetVideoMode(glfwGetPrimaryMonitor())->width, 0.f, (float)glfwGetVideoMode(glfwGetPrimaryMonitor())->height);
	projection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f);
	glUseProgram(programs.rampProgram);
	gl.uniformMatrix4fv(programs.rampProgram, "TextProjection", projection);
	glUseProgram(programs.iconsProgram);
	gl.uniformMatrix4fv(programs.iconsProgram, "Projection", projection);
	glUseProgram(programs.renderTheTextureProgram);
	gl.uniformMatrix4fv(programs.renderTheTextureProgram, "TextProjection", projection);
	glUseProgram(programs.curveProgram);
	gl.uniformMatrix4fv(programs.curveProgram, "TextProjection", projection);
	glUseProgram(programs.textureDisplayer);
	gl.uniformMatrix4fv(programs.textureDisplayer, "TextProjection", projection);
	glUseProgram(programs.brushCursor);
	gl.uniformMatrix4fv(programs.brushCursor, "TextProjection", projection);
	glUseProgram(programs.dotsProgram);
	gl.uniformMatrix4fv(programs.dotsProgram, "TextProjection", scprojection);
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


	float screenGapX = ((float)glfwGetVideoMode(glfwGetPrimaryMonitor())->width - screenSizeX)/(((float)glfwGetVideoMode(glfwGetPrimaryMonitor())->width)/2.0f)/2.0f; 

	updateButtonColorMixValues(UIElements,colorPicker,sndPanel);
	double xOffset = mouseXpos - lastMouseX;
	double yOffset = mouseYpos - lastMouseY;	
	//Panel
	if(panelData.exportPanelActive || panelData.modelPanelActive || panelData.paintingPanelActive || panelData.texturePanelActive || panelData.settingsPanelActive || panelData.generatorPanelActive){ //Disable panel if a message box is active
		//If message box is not active


		//Panel changing buttons
		glUseProgram(programs.uiProgram);

		if(!panelData.paintingPanelActive){
			ui.nodePanel(renderData.panelLoc-  screenGapX -1.0f,sndPanel.position + screenGapX,nodePanel.heigth,programs,icons,nodeScenes,selectedNodeScene);
			ui.nodePanelBarriers(renderData.panelLoc-  screenGapX -1.0f,sndPanel.position + screenGapX,nodePanel.heigth);
		}
		 
		
		std::vector<Node> duplicatedNodes;
		std::vector<int> deletedNodeIndexes;

		nodePanel.pointerCursor = false;
		for (size_t i = 0; i < nodeScenes[selectedNodeScene].nodes.size() * !(float)panelData.paintingPanelActive; i++)
		{
			if(nodePanel.heigth > 0.02){
				if(nodeScenes[selectedNodeScene].nodes[i].hide)
					nodeScenes[selectedNodeScene].nodes[i].height = ((nodeScenes[selectedNodeScene].nodes[i].inputs.size() + nodeScenes[selectedNodeScene].nodes[i].outputs.size())/25.f + 0.07 * !nodeScenes[selectedNodeScene].nodes[i].isMainOut) * nodePanel.zoomVal;
				else
					nodeScenes[selectedNodeScene].nodes[i].height = ((nodeScenes[selectedNodeScene].nodes[i].inputs.size() + nodeScenes[selectedNodeScene].nodes[i].rangeBarCount + nodeScenes[selectedNodeScene].nodes[i].outputs.size())/25.f + 0.07 * !nodeScenes[selectedNodeScene].nodes[i].isMainOut) * nodePanel.zoomVal;
				nodeScenes[selectedNodeScene].nodes[i].width = 0.12f * nodePanel.zoomVal;
				bool deleted = ui.node(nodeScenes[selectedNodeScene].nodes[i],programs,icons,renderData.window,mouseXpos,mouseYpos,xOffset,yOffset,glfwGetVideoMode(glfwGetPrimaryMonitor())->width,glfwGetVideoMode(glfwGetPrimaryMonitor())->height,nodeScenes[selectedNodeScene],nodePanel,textureSelectionPanel,i,albedoTextures,screenGapX,firstClick,coloringPanel,duplicateNodeCall,duplicatedNodes);
				if(deleted)
					deletedNodeIndexes.push_back(i);
			}
		}
		for (size_t i = 0; i < deletedNodeIndexes.size(); i++)
		{

			//Remove related connections
			for (int dOutI = 0; dOutI <  nodeScenes[selectedNodeScene].nodes[deletedNodeIndexes[i]].outputs.size(); dOutI++)
			{
				for (size_t coni = 0; coni <  nodeScenes[selectedNodeScene].nodes[deletedNodeIndexes[i]].outputs[dOutI].connections.size(); coni++)
				{
					 nodeScenes[selectedNodeScene].nodes[ nodeScenes[selectedNodeScene].nodes[deletedNodeIndexes[i]].outputs[dOutI].connections[coni].nodeConnectionIndex].inputs[ nodeScenes[selectedNodeScene].nodes[deletedNodeIndexes[i]].outputs[dOutI].connections[coni].inputConnectionIndex].nodeConnectionIndex = 10000;
					 nodeScenes[selectedNodeScene].nodes[ nodeScenes[selectedNodeScene].nodes[deletedNodeIndexes[i]].outputs[dOutI].connections[coni].nodeConnectionIndex].inputs[ nodeScenes[selectedNodeScene].nodes[deletedNodeIndexes[i]].outputs[dOutI].connections[coni].inputConnectionIndex].inputConnectionIndex = 10000;
				}
			}
			for (int dInI = 0; dInI <  nodeScenes[selectedNodeScene].nodes[deletedNodeIndexes[i]].inputs.size(); dInI++)
			{
				if( nodeScenes[selectedNodeScene].nodes[deletedNodeIndexes[i]].inputs[dInI].nodeConnectionIndex != 10000){
					for (size_t coni = 0; coni <  nodeScenes[selectedNodeScene].nodes[ nodeScenes[selectedNodeScene].nodes[deletedNodeIndexes[i]].inputs[dInI].nodeConnectionIndex].outputs[ nodeScenes[selectedNodeScene].nodes[deletedNodeIndexes[i]].inputs[dInI].inputConnectionIndex].connections.size(); coni++)
					{
						 nodeScenes[selectedNodeScene].nodes[ nodeScenes[selectedNodeScene].nodes[deletedNodeIndexes[i]].inputs[dInI].nodeConnectionIndex].outputs[ nodeScenes[selectedNodeScene].nodes[deletedNodeIndexes[i]].inputs[dInI].inputConnectionIndex].connections.erase( nodeScenes[selectedNodeScene].nodes[ nodeScenes[selectedNodeScene].nodes[deletedNodeIndexes[i]].inputs[dInI].nodeConnectionIndex].outputs[ nodeScenes[selectedNodeScene].nodes[deletedNodeIndexes[i]].inputs[dInI].inputConnectionIndex].connections.begin()+coni);
					}
				}
			}
			
			//Remove it's own connections
			for (size_t inI = 0; inI < nodeScenes[selectedNodeScene].nodes[deletedNodeIndexes[i]].inputs.size(); inI++)
			{
				nodeScenes[selectedNodeScene].nodes[deletedNodeIndexes[i]].inputs[inI].nodeConnectionIndex = 10000;
				nodeScenes[selectedNodeScene].nodes[deletedNodeIndexes[i]].inputs[inI].inputConnectionIndex = 10000;
			}
			for (size_t outI = 0; outI < nodeScenes[selectedNodeScene].nodes[deletedNodeIndexes[i]].outputs.size(); outI++)
			{
				nodeScenes[selectedNodeScene].nodes[deletedNodeIndexes[i]].outputs[outI].connections.clear();
			}
			

			nodeScenes[selectedNodeScene].stateChanged = true;


			//Delete the texture outputs of the node
			for (size_t delTxtri = 0; delTxtri < nodeScenes[selectedNodeScene].nodes[deletedNodeIndexes[i]].outputs.size(); delTxtri++)
			{
				glDeleteTextures(1,&nodeScenes[selectedNodeScene].nodes[deletedNodeIndexes[i]].outputs[delTxtri].result);
			}
		}
		for (size_t i = 0; i < deletedNodeIndexes.size(); i++)
		{
			nodeScenes[selectedNodeScene].nodes.erase( nodeScenes[selectedNodeScene].nodes.begin() + deletedNodeIndexes[i]);
			
			for (size_t delI = 0; delI < deletedNodeIndexes.size(); delI++)
			{
				if(deletedNodeIndexes[i] < deletedNodeIndexes[delI])
					deletedNodeIndexes[delI]--;
			}
			
			for (size_t delI = 0; delI <  nodeScenes[selectedNodeScene].nodes.size(); delI++)
			{
				for (size_t delInI = 0; delInI <  nodeScenes[selectedNodeScene].nodes[delI].inputs.size(); delInI++)
				{
					if(deletedNodeIndexes[i] < nodeScenes[selectedNodeScene].nodes[delI].inputs[delInI].nodeConnectionIndex &&  nodeScenes[selectedNodeScene].nodes[delI].inputs[delInI].nodeConnectionIndex != 10000){
						 nodeScenes[selectedNodeScene].nodes[delI].inputs[delInI].nodeConnectionIndex--;
					}
				}
				for (size_t delOutI = 0; delOutI <  nodeScenes[selectedNodeScene].nodes[delI].outputs.size(); delOutI++)
				{
						 for (size_t delConi = 0; delConi < nodeScenes[selectedNodeScene].nodes[delI].outputs[delOutI].connections.size(); delConi++)
						 {
							if(deletedNodeIndexes[i] <  nodeScenes[selectedNodeScene].nodes[delI].outputs[delOutI].connections[delConi].nodeConnectionIndex){
						 		nodeScenes[selectedNodeScene].nodes[delI].outputs[delOutI].connections[delConi].nodeConnectionIndex--;
						 }						 
					}
				}
			}
		}
		
		deletedNodeIndexes.clear();
		

		for (size_t i = 0; i < duplicatedNodes.size(); i++)
		{
			for (size_t rI = 0; rI < duplicatedNodes.size(); rI++)
			{
				for (size_t outI = 0; outI < duplicatedNodes[rI].outputs.size(); outI++)
				{
					for (size_t conI = 0; conI < duplicatedNodes[rI].outputs[outI].connections.size(); conI++)
					{
						if(duplicatedNodes[rI].outputs[outI].connections[conI].nodeConnectionIndex == duplicatedNodes[i].dupI)
							duplicatedNodes[rI].outputs[outI].connections[conI].nodeConnectionIndex = nodeScenes[selectedNodeScene].nodes.size()+i;
					}
				}
				for (size_t inI = 0; inI < duplicatedNodes[rI].inputs.size(); inI++)
				{
					if(duplicatedNodes[rI].inputs[inI].nodeConnectionIndex == duplicatedNodes[i].dupI)
						duplicatedNodes[rI].inputs[inI].nodeConnectionIndex = nodeScenes[selectedNodeScene].nodes.size()+i;
				}
			}
			duplicatedNodes[i].dupI = 0;
		}
		for (size_t i = 0; i < duplicatedNodes.size(); i++)
		{
			for (size_t outI = 0; outI < duplicatedNodes[i].outputs.size(); outI++)
			{
				for (size_t conI = 0; conI < duplicatedNodes[i].outputs[outI].connections.size(); conI++)
				{
					if(duplicatedNodes[i].outputs[outI].connections[conI].nodeConnectionIndex < nodeScenes[selectedNodeScene].nodes.size())
						duplicatedNodes[i].outputs[outI].connections.erase(duplicatedNodes[i].outputs[outI].connections.begin()+conI);
				}
			}

		}
		for (size_t i = 0; i < duplicatedNodes.size(); i++){
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

		if(!panelData.paintingPanelActive){
			float dotrad = 3.f;
			float dotdepth = 0.1f;
			std::vector<float> dotVertices = { 
			 	dotrad,  dotrad, dotdepth,1,1,0,0,0,  // top right
			 	dotrad,  0.0f, dotdepth,1,0,0,0,0,  // bottom right
			 	0.0f,  dotrad, dotdepth,0,1,0,0,0,  // top left 
				dotrad,  0.0f, dotdepth,1,0,0,0,0,  // bottom right
			 	0.0f,  0.0f, dotdepth,0,0,0,0,0,  // bottom left
			 	0.0f,  dotrad, dotdepth,0,1,0,0,0   // top left
			};
			glUseProgram(programs.dotsProgram);
			gl.uniform1f(programs.dotsProgram,"zoom",nodePanel.zoomVal);
			glUniform2f(glGetUniformLocation(programs.dotsProgram , "transform"), nodePanel.panelPositionX* nodePanel.zoomVal *((float)glfwGetVideoMode(glfwGetPrimaryMonitor())->width/2), nodePanel.panelPositionY * nodePanel.zoomVal*((float)glfwGetVideoMode(glfwGetPrimaryMonitor())->height/2));
			glBufferSubData(GL_ARRAY_BUFFER , 0 , dotVertices.size() * sizeof(float) , &dotVertices[0]);
			glDrawArraysInstanced(GL_TRIANGLES , 0 , 6 , 200);
		}

		if(!panelData.paintingPanelActive){
			if(firstClick && anyNodeHover){
				showTheSelectionBox = false;
			}
			if(firstClick && !nodePanel.panelHover)
				showTheSelectionBox = false;

			if(glfwGetMouseButton(renderData.window,0) == GLFW_PRESS && nodePanel.panelHover && showTheSelectionBox){
				dPX = 0;
				dPY = 0;
				if(firstClick){
					sPX = (mouseXpos/glfwGetVideoMode(glfwGetPrimaryMonitor())->width*2 - 1.0f + screenGapX);
					sPY = (-mouseYpos/glfwGetVideoMode(glfwGetPrimaryMonitor())->height*2 + 1.0f);
				}
				dPX = (mouseXpos/glfwGetVideoMode(glfwGetPrimaryMonitor())->width*2 - 1.0f + screenGapX);
				dPY = ((-mouseYpos/glfwGetVideoMode(glfwGetPrimaryMonitor())->height*2 + 1.0f));
				if(!glfwGetKey(renderData.window,GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
					selectionBoxCoords = ui.selectionBox(true,sPX,sPY,dPX,dPY,0.3f);
				else
					ui.drawLine(sPX,sPY,0.3f,dPX,dPY,10,glm::vec4(1,0,0,1),true);
				selectionActive = true;
			}
			else{
				//sPX = (mouseXpos/screenSizeX*2 - 1.0f);
				//sPY = (-mouseYpos/glfwGetVideoMode(glfwGetPrimaryMonitor())->height*2 + 1.0f);
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
							if(ui.isMouseOnCoords(renderData.window,((nodeScenes[selectedNodeScene].nodes[i].positionX + nodePanel.panelPositionX) * nodePanel.zoomVal) * (glfwGetVideoMode(glfwGetPrimaryMonitor())->width/2.f) + glfwGetVideoMode(glfwGetPrimaryMonitor())->width/2.f,(glfwGetVideoMode(glfwGetPrimaryMonitor())->height) - (((nodeScenes[selectedNodeScene].nodes[i].positionY + nodePanel.panelPositionY) * nodePanel.zoomVal) * (glfwGetVideoMode(glfwGetPrimaryMonitor())->height/2.f) + glfwGetVideoMode(glfwGetPrimaryMonitor())->height/2.f),selectionBoxCoords,false)){
								nodeScenes[selectedNodeScene].nodes[i].active = true;
							}
							else{
								if(!glfwGetKey(renderData.window,GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
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
		}

		
		ui.panel(renderData.panelLoc-  screenGapX -1.0f , icons,panelData);
		int brushMaskTexturesState;
		if(UIElements[UImaskPaintingCheckBoxElement].checkBox.checked)
			brushMaskTexturesState = 0;
		else if(UIElements[UIcolorPaintingCheckBoxElement].checkBox.checked)
			brushMaskTexturesState = 1;
		else if(UIElements[UInormalmapPaintingCheckBoxElement].checkBox.checked)
			brushMaskTexturesState = 2;
		else 
			brushMaskTexturesState = 0;
		
		glUseProgram(programs.renderTheTextureProgram);
		GlSet gls;
		gls.uniform1i(programs.renderTheTextureProgram,"roundCorners",1);
		glUseProgram(programs.uiProgram);
		ui.sndPanel(sndPanel.state,sndPanel.position + screenGapX,programs,icons,albedoTextures,renderData.window,mouseXpos,mouseYpos,screenGapX,
					glfwGetVideoMode(glfwGetPrimaryMonitor())->width,selectedAlbedoTextureIndex,nodeScenes,selectedNodeScene,newModelAdded,sndPanel.texturePanelSlideVal,
					sndPanel.materialPanelSlideVal,firstClick,coloringPanel,txtrCreatingPanel,anyTextureNameActive,textureText,sndPanel.activeFolderIndex,nodePanel,
					appNodes,sndPanel,brushMaskTextures,maskPanelEnter,yOffset,nodeScenesHistory,brushMaskTexturesState,chosenTextureResIndex,textureDraggingState);
		glUseProgram(programs.renderTheTextureProgram);
		gls.uniform1i(programs.renderTheTextureProgram,"roundCorners",0);


		glUseProgram(programs.uiProgram);
		ui.renderAlert(alertMessage,alertDuration,programs.uiProgram,alertState,alertIsSuccess);		
		
		
		txtrCreatingPanel.panelPosX = sndPanel.position + screenGapX; 
		txtrCreatingPanel.panelPosY = 0.73f;

		if(txtrCreatingPanel.active)
			ui.textureCreatingPanel(txtrCreatingPanel,icons,programs,renderData.window,mouseXpos,mouseYpos, firstClick,coloringPanel,screenGapX,albedoTextures,sndPanel.activeFolderIndex);

		if(coloringPanel.active)
			ui.coloringPanel(coloringPanel,programs,icons,renderData.window,saturationValShaderData,projection,mouseXpos,mouseYpos,firstClick,xOffset,yOffset,FBOScreen,colorPicker,screenGapX,screenHoverPixel,UIElements,false);




		//Add node context menu
		if(addNodeContextMenu.active){
			ui.container(addNodeContextMenu.positionX,addNodeContextMenu.positionY - addNodeContextMenu.height,addNodeContextMenu.positionZ,addNodeContextMenu.width,addNodeContextMenu.height,colorData.nodePanelContextMenuPanelColor,programs,icons.Circle,glm::vec4(0),0);
			glUseProgram(programs.uiProgram); 

			int indepI = 0;

			for (size_t i = 0; i < addNodeContextMenu.buttons.size(); i++)
			{
				if(indepI <= 9 && (i == 0||((util.isMatch(addNodeContextMenu.buttons[i].text,addNodeContextMenu.searchText) && addNodeContextMenu.searchText != "search") || (addNodeContextMenu.scroll < i && i < min(addNodeContextMenu.buttons.size(),(size_t)10)+addNodeContextMenu.scroll && addNodeContextMenu.searchText == "search")))){
					addNodeContextMenu.buttons[i].transitionMixVal = (float)addNodeContextMenu.buttons[i].hover * (float)addNodeContextMenu.buttons[i].hoverAnimationActive;

					if((addNodeContextMenu.buttons[i].hover && addNodeContextMenu.buttons[i].hoverAnimationActive) || (addNodeContextMenu.selectedButtonIndex == indepI && glfwGetKey(renderData.window,GLFW_KEY_ENTER) == GLFW_PRESS)){
						addNodeContextMenu.buttons[i].positionZ = 0.899f;
						if((glfwGetMouseButton(renderData.window, 0) == GLFW_PRESS && i != 0) || (glfwGetKey(renderData.window,GLFW_KEY_ENTER) == GLFW_PRESS && addNodeContextMenu.selectedButtonIndex == indepI && addNodeContextMenu.selectedButtonIndex != 0)){
							Node node;
							node = appNodes[i-1];
							for (size_t nodeAI = 0; nodeAI < nodeScenes[selectedNodeScene].nodes.size(); nodeAI++)
							{
								nodeScenes[selectedNodeScene].nodes[nodeAI].active = false;
							}
							
							node.active = true;
							selectionActive = false;
							showTheSelectionBox = false;
							node.stateChanged = true;
							node.positionX = (mouseXpos/screenSizeX*2 - 1.0f) / nodePanel.zoomVal - nodePanel.panelPositionX;
							node.positionY = ((-mouseYpos/glfwGetVideoMode(glfwGetPrimaryMonitor())->height*2 + 1.0f) / nodePanel.zoomVal - nodePanel.panelPositionY);						

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
			const bool searchButtonEnter = ui.isMouseOnButton(renderData.window,addNodeContextMenu.buttons[1].width, addNodeContextMenu.buttons[1].height, addNodeContextMenu.positionX-screenGapX, addNodeContextMenu.positionY - 0.265f*2.05f,mouseXpos,mouseYpos,false); 
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
	
	//! Main panel barriers
	float barrierW = 0.18f;
	if(panelData.texturePanelActive)
		barrierW = 0.22f;
	
	ui.box(barrierW, 0.07f, renderData.panelLoc - screenGapX - 1.0f + barrierW, 0.97, "", glm::vec4(0), 0.075f, false, false, 0.9999f, 10000, glm::vec4(0), 0);
	ui.box(barrierW, 0.07f, renderData.panelLoc - screenGapX - 1.0f + barrierW, -0.97, "", glm::vec4(0), 0.075f, false, false, 0.9999, 10000, glm::vec4(0), 0);

	if (panelData.texturePanelActive) {
		ui.modelMaterialPanel(model,programs,renderData,screenGapX,materialsPanelSlideValue,mouseXpos,mouseYpos,uiOut,currentMaterialIndex,firstClick,newModelAdded,selectedNodeScene,icons,nodeScenes,albedoTextures,textureSelectionPanel);
	}


	if(addNodeContextMenu.stateChanged){
		addNodeContextMenu.positionX = mouseXpos/(glfwGetVideoMode(glfwGetPrimaryMonitor())->width/2.f) - 1.0f + screenGapX ;
		addNodeContextMenu.positionY = -mouseYpos/glfwGetVideoMode(glfwGetPrimaryMonitor())->height*2.f + 1.0f;
	}


	if (panelData.paintingPanelActive && renderData.panelLoc < 1.98f) {
		glUseProgram(programs.uiProgram); 

		//Color Picker
		if(colorPicker.updateHueVal)
			colorPicker.hueColorValue = ui.hueBar(centerCoords - screenGapX + 0.1f, -0.55f+panelData.paintingPanelSlideVal, colorPicker.hueValue, FBOScreen, renderData.window,projection,colorPicker.updateHueVal); 
		else
			ui.hueBar(centerCoords - screenGapX + 0.1f, -0.55f+panelData.paintingPanelSlideVal, colorPicker.hueValue, FBOScreen, renderData.window,projection,colorPicker.updateHueVal); 


		saturationValShaderData.boxColor = colorPicker.hueColorValue / 255.0f;
		saturationValShaderData.renderTextureProjection = projection;
		gl.useSaturationValBoxShader(programs.saturationValBoxProgram,saturationValShaderData);
		ui.colorBox(centerCoords - screenGapX - 0.02f, -0.55f+panelData.paintingPanelSlideVal, colorPicker.saturationValuePosX, colorPicker.saturationValuePosY,icons);
		
		ui.box(0.04f, 0.03f, centerCoords - screenGapX - 0.008f,-0.81f+panelData.paintingPanelSlideVal, util.rgbToHexGenerator(colorPicker.pickerValue), colorData.textBoxColor, 0, true, false, 0.9f, 10, colorData.textBoxColorClicked, hexValTextboxMixVal);//Hex val textbox
		
		glUseProgram(programs.iconsProgram);
		ui.iconBox(0.02*1.2,0.0364f*1.2,centerCoords - screenGapX - 0.095f,-0.81f+panelData.paintingPanelSlideVal,0.9f,icons.Circle,0,colorData.panelColorSnd,glm::vec4(0));
		ui.iconBox(0.02,0.0364f,centerCoords - screenGapX - 0.095f,-0.81f+panelData.paintingPanelSlideVal,0.91f,icons.Circle,0,glm::vec4(colorPicker.pickerValue / glm::vec3(255),1.0f),glm::vec4(0));
		ui.iconBox(0.02f,0.03f,centerCoords - screenGapX + 0.08f, -0.81f+panelData.paintingPanelSlideVal,0.9f,icons.dropperIcon,dropperMixVal,colorData.iconColor,colorData.iconColorHover);
	}
		glUseProgram(programs.PBRProgram);
		gl.uniform1i(programs.PBRProgram,"maskMode",UIElements[UImaskPaintingCheckBoxElement].checkBox.checked || UIElements[UIdynamicPaintingCheckBoxElement].checkBox.checked);
		gl.uniform1i(programs.PBRProgram,"normalPainting",UIElements[UInormalmapPaintingCheckBoxElement].checkBox.checked);
		
		glUseProgram(programs.outProgram);
		gl.uniform1i(programs.outProgram,"maskMode",UIElements[UImaskPaintingCheckBoxElement].checkBox.checked || UIElements[UIdynamicPaintingCheckBoxElement].checkBox.checked);
		gl.uniform1i(programs.outProgram,"normalPainting",UIElements[UInormalmapPaintingCheckBoxElement].checkBox.checked);

	float maskPanelSliderValue = 0.f;
	int maskPanelElementSize = 0;
	if(UIElements[UImaskPaintingCheckBoxElement].checkBox.checked){
		maskPanelSliderValue = brushMaskTextures.maskTexturesSliderValue;
	}
	if(UIElements[UIcolorPaintingCheckBoxElement].checkBox.checked){
		maskPanelSliderValue = brushMaskTextures.colorTexturesSliderValue;
	}
	if(UIElements[UInormalmapPaintingCheckBoxElement].checkBox.checked){
		maskPanelSliderValue = brushMaskTextures.normalTexturesSliderValue;
	}

	std::vector<aTexture> brushTextures;
	
	if (panelData.paintingPanelActive && !UIElements[UIdynamicPaintingCheckBoxElement].checkBox.checked) { //Icons
		int state = 0;
		

		if(UIElements[UImaskPaintingCheckBoxElement].checkBox.checked){

			state = 0;
			for (size_t i = 0; i < albedoTextures.size(); i++)
			{
				if(albedoTextures[i].folderIndex == 3)
					brushTextures.push_back(albedoTextures[i]);
			}
			
		}
		if(UIElements[UIcolorPaintingCheckBoxElement].checkBox.checked){
			for (size_t i = 0; i < albedoTextures.size(); i++)
			{
				if(albedoTextures[i].folderIndex == 4)
					brushTextures.push_back(albedoTextures[i]);
			}	
			state = 1;
		}
		if(UIElements[UInormalmapPaintingCheckBoxElement].checkBox.checked){
			
			for (size_t i = 0; i < albedoTextures.size(); i++)
			{
				if(albedoTextures[i].folderIndex == 5)
					brushTextures.push_back(albedoTextures[i]);
			}	
			state = 2;
		}	

		ui.brushMaskTexturePanel(programs,brushTextures,centerCoords,screenGapX,maskPanelSliderValue,currentBrushMaskTexture,firstClick,renderData.window,mouseXpos,mouseYpos,FBOScreen,panelData,screenSizeX,screenSizeY,uiOut,UIElements,brushBlurVal,outShaderData,0.8f + panelData.paintingPanelSlideVal,state,textureSelectionPanel);
	}

	maskPanelElementSize = brushTextures.size();


	//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
	if(panelData.paintingPanelActive && renderData.panelLoc < 1.98f && !UIElements[UIdynamicPaintingCheckBoxElement].checkBox.checked){
		glUseProgram(programs.uiProgram);

		const float maskPanelRange = ceil((int)maskPanelElementSize/3.f) / 8.33333333333 - (0.8f - 0.55f); 

		if(UIElements[UImaskPaintingCheckBoxElement].checkBox.checked)
			ui.verticalRangeBar((renderData.panelLoc + max(renderData.panelLoc - 1.7f,0.0f)) / centerDivider + centerSum - screenGapX + 0.13f,0.8f+panelData.paintingPanelSlideVal,0.125,brushMaskTextures.maskTexturesSliderValue,(0.25f / (maskPanelRange/4.f+0.001f)) * (maskPanelSliderValue*-1.f),renderData.window,mouseXpos,mouseYpos,yOffset,firstClick,(int)brushMaskTextures.maskTextures.size(),screenGapX);
		if(UIElements[UIcolorPaintingCheckBoxElement].checkBox.checked)
			ui.verticalRangeBar((renderData.panelLoc + max(renderData.panelLoc - 1.7f,0.0f)) / centerDivider + centerSum - screenGapX + 0.13f,0.8f+panelData.paintingPanelSlideVal,0.125,brushMaskTextures.colorTexturesSliderValue,(0.25f / (maskPanelRange/4.f+0.001f)) * (maskPanelSliderValue*-1.f),renderData.window,mouseXpos,mouseYpos,yOffset,firstClick,(int)brushMaskTextures.maskTextures.size(),screenGapX);
		if(UIElements[UInormalmapPaintingCheckBoxElement].checkBox.checked)	
			ui.verticalRangeBar((renderData.panelLoc + max(renderData.panelLoc - 1.7f,0.0f)) / centerDivider + centerSum - screenGapX + 0.13f,0.8f+panelData.paintingPanelSlideVal,0.125,brushMaskTextures.normalTexturesSliderValue,(0.25f / (maskPanelRange/4.f+0.001f)) * (maskPanelSliderValue*-1.f),renderData.window,mouseXpos,mouseYpos,yOffset,firstClick,(int)brushMaskTextures.maskTextures.size(),screenGapX);

		//Display brush texture
		ui.box(0.035f, 0.07f, centerCoords - screenGapX - 0.1f, 0.42f+panelData.paintingPanelSlideVal, "", colorData.buttonColor, 0.075f, false, true, 0.9f, 1000, glm::vec4(0), 0);
	}
	if(panelData.paintingPanelActive && renderData.panelLoc < 1.98f && UIElements[UIdynamicPaintingCheckBoxElement].checkBox.checked){
		glUseProgram(programs.dynamicPaintingProgram);
		GlSet glset;
		glset.uniformMatrix4fv(programs.dynamicPaintingProgram,"renderProjection",projection);
		
		
		glset.uniform1f(programs.dynamicPaintingProgram,"radius",(UIElements[UIbrushSizeRangeBar].rangeBar.value+0.11f)*1000);
		glset.uniform1f(programs.dynamicPaintingProgram,"hardness",(UIElements[UIbrushBlurRangeBar].rangeBar.value-0.09f)*4.5454545*50);
		glset.uniform1f(programs.dynamicPaintingProgram,"opacity",(UIElements[UIbrushOpacityRangeBar].rangeBar.value + 0.11)*4.5454545);
		glset.uniform1i(programs.dynamicPaintingProgram, "posCount" , 1);
		glset.uniform1i(programs.dynamicPaintingProgram, "bgtxtr" , 4);
		glset.uniform1i(programs.dynamicPaintingProgram, "displayingTheBrush" , 1);
		glUniform2f(glGetUniformLocation(programs.dynamicPaintingProgram , "positions[0]"), 0.5f, 0.5f);
		glm::vec2 brushDisplayerScale = glm::vec2(0.06f,0.12f);
		glm::vec2 brushDisplayerPos = glm::vec2((renderData.panelLoc + max(renderData.panelLoc - 1.7f,0.0f)) / centerDivider + centerSum,0.5f + panelData.paintingPanelSlideVal) - screenGapX;
		std::vector<float> brushDisplayer = { 
			// first triangle
			 brushDisplayerScale.x + brushDisplayerPos.x,  brushDisplayerScale.y + brushDisplayerPos.y, 0.9f,1,1,0,0,0,  // top right
			 brushDisplayerScale.x + brushDisplayerPos.x, -brushDisplayerScale.y + brushDisplayerPos.y, 0.9f,1,0,0,0,0,  // bottom right
			-brushDisplayerScale.x + brushDisplayerPos.x,  brushDisplayerScale.y + brushDisplayerPos.y, 0.9f,0,1,0,0,0,  // top left

			 brushDisplayerScale.x + brushDisplayerPos.x, -brushDisplayerScale.y + brushDisplayerPos.y, 0.9f,1,0,0,0,0,  // bottom right
			-brushDisplayerScale.x + brushDisplayerPos.x, -brushDisplayerScale.y + brushDisplayerPos.y, 0.9f,0,0,0,0,0,  // bottom left
			-brushDisplayerScale.x + brushDisplayerPos.x,  brushDisplayerScale.y + brushDisplayerPos.y, 0.9f,0,1,0,0,0   // top left
		};
		glset.drawArrays(brushDisplayer,0);
		glset.uniform1i(programs.dynamicPaintingProgram, "displayingTheBrush" , 0);
		glset.uniform1f(programs.dynamicPaintingProgram,"radius",(UIElements[UIbrushSizeRangeBar].rangeBar.value+0.11f)*300);
		glset.uniform1f(programs.dynamicPaintingProgram,"hardness",(UIElements[UIbrushBlurRangeBar].rangeBar.value-0.09f)*4.5454545*50);
		glset.uniform1f(programs.dynamicPaintingProgram,"opacity",(UIElements[UIbrushOpacityRangeBar].rangeBar.value + 0.11)*4.5454545);

		//Paint over texture name
		glUseProgram(programs.uiProgram);
		ui.box(0.09f,0.03,((renderData.panelLoc + max(renderData.panelLoc - 1.7f,0.0f)) / centerDivider + centerSum) - 0.0f - screenGapX,-0.17f + panelData.paintingPanelSlideVal,"", colorData.buttonColor,0.f,0,0,0.8f,0.8f,colorData.buttonColor,0.f);
	}

	bool usingUiProgram = false;
	bool usingIconProgram = false;
	
	glUseProgram(programs.uiProgram);
	if(panelData.paintingPanelActive){

		ui.box(0.0025f, 0.6f, renderData.panelLoc - 1.0f - screenGapX + 0.0025f, 0.25f - panelData.paintingPanelSlideVal, "",colorData.mainPanelSliderColor, 0., false, false, 0.91f, 1000, colorData.mainPanelSliderColor, 0.f); //Add mask texture button
	}
	if(panelData.settingsPanelActive){

		ui.box(0.0025f, 0.7f, renderData.panelLoc - 1.0f - screenGapX + 0.0025f, 0.15f - panelData.settingsPanelSlideVal, "",colorData.mainPanelSliderColor, 0., false, false, 0.91f, 1000, colorData.mainPanelSliderColor, 0.f); //Add mask texture button
	}

	for (size_t i = 0; i < UIElements.size(); i++)
	{
		std::string currentType = UIElements[i].type;
				
		if(!UIElements[i].attachedToMainPanel || UIElements[i].focusMode){
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
		if(renderData.panelLoc > 1.98f && !UIElements[i].focusMode)
			panelCompatibility = false;
		if(!UIElements[UIdynamicPaintingCheckBoxElement].checkBox.checked && (i == UIpaintOverCheckBoxElement || (i == UIdisplayPaintOverTextureCheckBoxElement) || (i == UIselectPaintOverTextureNameTextElement) || (i == UIpaintOverPosXRangeBarElement) || (i == UIpaintOverPosYRangeBarElement) || (i == UIpaintOverScaleXRangeBarElement) || (i == UIpaintOverScaleYRangeBarElement) || (i == UIpaintOverPosTextElement) || (i == UIpaintOverScaleTextElement) || (i == UIpaintOverXTextElement) || (i == UIpaintOverYTextElement) || (i == UIselectPaintOverTextureIconElement)))
			panelCompatibility = false;

		if(panelCompatibility && !(UIElements[UIdynamicPaintingCheckBoxElement].checkBox.checked && (i == UIbrushBordersRangeBar|| i == UIbrushBordersText || i == UIbrushRotationRangeBar|| i == UIbrushRotationText || i == UIbrushSpacingRangeBar || i == UIbrushSpacingText || i == UIuseNegativeCheckBox || i == UIaddBrushMaskTextureIcon ||  i == UIselectBrushMaskTextureIcon || i == UImaskTextureFileNameText))){
			float slideVal = 0.0f;
			
			if(panelData.paintingPanelActive && !UIElements[i].focusMode)
				slideVal = panelData.paintingPanelSlideVal;
			if(panelData.settingsPanelActive && !UIElements[i].focusMode)
				slideVal = panelData.settingsPanelSlideVal;

				glUseProgram(programs.uiProgram);

			if(currentType == "button"){
				ui.box(UIElements[i].button.width, UIElements[i].button.height, centerCoords - screenGapX + UIElements[i].button.positionX, UIElements[i].button.positionY+slideVal, UIElements[i].button.text, UIElements[i].button.color, UIElements[i].button.textRatio, false, false, UIElements[i].button.positionZ, UIElements[i].button.buttonCurveReduce, UIElements[i].button.colorHover, UIElements[i].button.transitionMixVal); //Add mask texture button
			}
			if(currentType == "text"){	
				ui.renderText(programs.uiProgram,UIElements[i].text.text, centerCoords - screenGapX + UIElements[i].text.positionX, UIElements[i].text.positionY+slideVal, UIElements[i].text.scale,colorData.textColor,0.9f,false);
			}
			if(currentType == "rangeBar"){
				if(!UIElements[i].rangeBar.isConstant)
					ui.rangeBar(centerCoords - screenGapX + UIElements[i].rangeBar.positionX, UIElements[i].rangeBar.positionY+slideVal, UIElements[i].rangeBar.value,UIElements[i].rangeBar.widthDivider);
				else
					ui.constRangeBar(centerCoords - screenGapX + UIElements[i].rangeBar.positionX, UIElements[i].rangeBar.positionY+slideVal, UIElements[i].rangeBar.value,icons,UIElements[i].rangeBar.mixVal,UIElements[i].rangeBar.lastVal,UIElements[i].rangeBar.increase);
			}
			if(currentType == "textBox"){
				ui.box(UIElements[i].textBox.width, UIElements[i].textBox.height,centerCoords - screenGapX + UIElements[i].textBox.position_x, UIElements[i].textBox.position_y+slideVal,UIElements[i].textBox.text , colorData.textBoxColor, 0 , true, false, UIElements[i].textBox.position_z, 10 , colorData.textBoxColorClicked, UIElements[i].textBox.transitionMixVal); //Add mask texture button
			}
			if(currentType == "checkBox"){
				if(!(UIElements[i].checkBox.text == "Focus Mode" && centerCoords - screenGapX + UIElements[i].checkBox.positionX < -0.25f)){
					if(!(i == UIdynamicPaintingCheckBoxElement && centerCoords - screenGapX + UIElements[i].checkBox.positionX < -0.23f)){
							ui.checkBox(centerCoords - screenGapX + UIElements[i].checkBox.positionX, UIElements[i].checkBox.positionY+slideVal, UIElements[i].checkBox.text,  UIElements[i].checkBox.mouseHover,  UIElements[i].checkBox.checked,UIElements[i].checkBox.texture); //jpg checkbox
						
					}
				}
			}
			if(currentType == "icon"){
				if(!(i == UIdynamicPaintingIconElement && centerCoords - screenGapX + UIElements[i].icon.positionX < -0.21f)){
					glUseProgram(programs.iconsProgram);
					ui.iconBox(UIElements[i].icon.width,UIElements[i].icon.height,centerCoords - screenGapX + UIElements[i].icon.positionX ,UIElements[i].icon.positionY+slideVal,UIElements[i].icon.positionZ,UIElements[i].icon.icon, UIElements[i].icon.mixVal , UIElements[i].icon.color , UIElements[i].icon.colorHover);
				}
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
			gl.uniform1i(programs.normalGenProgram,"displayingMode",1);
			float rangeBarVal = (UIElements[UInormalStrengthRangeBarElement].rangeBar.value + 0.11f) * 50.f;
			gl.uniform1f(programs.normalGenProgram,"normalStrength",rangeBarVal);
			glActiveTexture(GL_TEXTURE28);

		}
		else if(UIElements[UInoiseCheckBoxElement].checkBox.checked){
			glUseProgram(programs.noisyTextureProgram);

			glm::mat4 renderTextureProjection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f);
		
			glActiveTexture(GL_TEXTURE0);

			gl.uniformMatrix4fv(programs.noisyTextureProgram,"renderTextureProjection",renderTextureProjection);
			gl.uniform1i(programs.noisyTextureProgram,"inputTexture",0);
			gl.uniform1i(programs.noisyTextureProgram,"displayingMode",1);
			float rangeBarVal = ((0.22f-(UIElements[UInoiseStrengthRangeBarElement].rangeBar.value + 0.11f))+0.05f)  * 50.f;
			gl.uniform1f(programs.noisyTextureProgram,"value",rangeBarVal);
			glActiveTexture(GL_TEXTURE28);
		}
		else{
		//*Generate the text texture
			int txtrRes = 256;
			for (size_t i = 0; i < chosenTextureResIndex; i++)
			{
				txtrRes*=2;
			}
		
			glActiveTexture(GL_TEXTURE28);
				
				//FBO
				GlSet glset;

				unsigned int FBO;
				glset.genFramebuffers(FBO);
				glset.bindFramebuffer(FBO);
		
				//Texture
				if(!generatedTextTxtr)
					glset.genTextures(generatedTextTxtr);
				glset.bindTexture(generatedTextTxtr);
				glset.texImage(NULL, txtrRes,txtrRes,GL_RGBA); //TODO : Use texture quality variable
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		
				glset.generateMipmap();
		
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, generatedTextTxtr, 0);
		
				glViewport(0,0,txtrRes,txtrRes);
		
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
				if(UIElements[UInoiseCheckBoxElement].checkBox.checked)
					glUseProgram(programs.noisyTextureProgram);
				else if(UIElements[UInormalmapCheckBoxElement].checkBox.checked)
					glUseProgram(programs.normalGenProgram);
				else{
					UserInterface ui;
					glm::mat4 projection = glm::ortho(-1.0f, 0.5f, -1.f, 1.f);
					glUseProgram(programs.uiProgram);
					glset.uniformMatrix4fv(programs.uiProgram, "TextProjection", projection);
					
					glUseProgram(programs.uiProgram);
					float thic = (UIElements[UIgenerateTextSizeRangeBarElement].rangeBar.value+0.11f)/100.f;

					int s = ((UIElements[UIgenerateTextTextureTextTextBoxElement].textBox.text.size()/(((0.23f-(UIElements[UIgenerateTextSizeRangeBarElement].rangeBar.value+0.11f))*100)*2.f))+1);
					int lineCounter = 0;
					int cCntL = 0;
					for (size_t i = 0; i < s; i++)
					{
						bool added = false;
						for (size_t si = 0; si < ((0.23f-(UIElements[UIgenerateTextSizeRangeBarElement].rangeBar.value+0.11f))*100)*3.f; si++)
						{
							if(cCntL <  UIElements[UIgenerateTextTextureTextTextBoxElement].textBox.text.size()){
								cCntL++;
								added = true;
							}
						}
						if(added)
							lineCounter++;
					}
					
					int cCnt = 0;
					for (size_t i = 0; i < s; i++)
					{
						std::string dTxt;
						for (size_t si = 0; si < ((0.23f-(UIElements[UIgenerateTextSizeRangeBarElement].rangeBar.value+0.11f))*100)*3.f; si++)
						{
							if(cCnt <  UIElements[UIgenerateTextTextureTextTextBoxElement].textBox.text.size()){
								dTxt.push_back(UIElements[UIgenerateTextTextureTextTextBoxElement].textBox.text[cCnt]);
								cCnt++;
							}
						}
						
						float aposY = lineCounter/10.f - 0.1f;

						if(UIElements[UIgenerateTextLeftAlignCheckBoxElement].checkBox.checked){
							float aposX = -1.f;
							if(dTxt.size())
								ui.renderText(programs.uiProgram,dTxt,aposX,aposY - ((UIElements[UIgenerateTextSizeRangeBarElement].rangeBar.value+0.11f) * i),thic,glm::vec4(1),0.9f,false);
						}
						if(UIElements[UIgenerateTextMidAlignCheckBoxElement].checkBox.checked){
							float aposX = -.5f;
							if(dTxt.size())
								ui.renderTextM(programs.uiProgram,dTxt,aposX,aposY - ((UIElements[UIgenerateTextSizeRangeBarElement].rangeBar.value+0.11f) * i),thic,glm::vec4(1),0.9f,false);
						}
						if(UIElements[UIgenerateTextRightAlignCheckBoxElement].checkBox.checked){
							float aposX = .5f;
							if(dTxt.size())
								ui.renderTextR(programs.uiProgram,dTxt,aposX,aposY - ((UIElements[UIgenerateTextSizeRangeBarElement].rangeBar.value+0.11f) * i),thic,glm::vec4(1),0.9f,false);
						}

						
					}
					
					projection = glm::ortho(-1.0f, 1.f, -1.f, 1.f);
					glUseProgram(programs.uiProgram);
					glset.uniformMatrix4fv(programs.uiProgram, "TextProjection", projection);
				}

			glUseProgram(programs.noisyTextureProgram);

			glm::mat4 renderTextureProjection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f);
		
			glActiveTexture(GL_TEXTURE28);
			glset.bindTexture(generatedTextTxtr);

			gl.uniformMatrix4fv(programs.noisyTextureProgram,"renderTextureProjection",renderTextureProjection);
			gl.uniform1i(programs.noisyTextureProgram,"inputTexture",28);
			gl.uniform1i(programs.noisyTextureProgram,"displayingMode",1);
			float rangeBarVal = ((0.22f-(UIElements[UInoiseStrengthRangeBarElement].rangeBar.value + 0.11f))+0.05f)  * 50.f;
			gl.uniform1f(programs.noisyTextureProgram,"value",rangeBarVal);
			glActiveTexture(GL_TEXTURE28);

			LigidPainter lp;
			lp.setViewportToDefault();
			
			glset.bindFramebuffer(0);
			glset.deleteFramebuffers(FBO);
		///////////////////////
			
		}
		gl.drawArrays(renderVertices,0);
		if(UIElements[UIgenerateTextCheckBoxElement].checkBox.checked){
			GlSet glset;
			
			unsigned int FBO;
			glset.genFramebuffers(FBO);
			glset.bindFramebuffer(FBO);
	
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, generatedTextTxtr, 0);
		
			glClearColor(0,0,0,0);
			glClear(GL_COLOR_BUFFER_BIT);
		
			glset.bindFramebuffer(0);
			glset.deleteFramebuffers(FBO);
		}
		gl.uniform1i(programs.noisyTextureProgram,"displayingMode",0);
		gl.uniform1i(programs.normalGenProgram,"displayingMode",0);
	}
	bool skyboxlistStateChanged = false;
	bool resolutionChanged = false;
	if(panelData.settingsPanelActive){
		resolutionChanged = ui.listBox(centerCoords - screenGapX,0.6f+panelData.settingsPanelSlideVal,0.9f,"Texture Resolution",0.1f,icons,{"256","512","1024","2048","4096"},true,renderData.window,mouseXpos,mouseYpos,firstClick,chosenTextureResIndex,screenGapX);
		ui.listBox(centerCoords - screenGapX,0.13f+panelData.settingsPanelSlideVal,0.9f,"Node Resolution",0.1f,icons,{"256","512","1024","2048"},true,renderData.window,mouseXpos,mouseYpos,firstClick,chosenNodeResIndex,screenGapX);
		skyboxlistStateChanged = ui.listBox(centerCoords - screenGapX,-0.5f+panelData.settingsPanelSlideVal,0.9f,"Skybox",0.1f,icons,{"1","2","3","4","5","6"},true,renderData.window,mouseXpos,mouseYpos,firstClick,chosenSkyboxTexture,screenGapX);
	}
	
	if(resolutionChanged){
		LigidPainter lp;
		if(lp.ligidMessageBox("Textures will be compressed into the chosen resolution.",-0.224f,"Do you want to proceed?",-0.1f)){
			for (size_t i = 0; i < albedoTextures.size(); i++)
			{
				if(albedoTextures[i].isTexture){
					int txtrRes = 256;
					for (size_t i = 0; i < previousTextureResIndex; i++)
					{
						txtrRes*=2;
					}

					glActiveTexture(GL_TEXTURE28);
					glBindTexture(GL_TEXTURE_2D,albedoTextures[i].id);
					Texture texture;
					GLubyte* data = texture.getTextureFromProgram(GL_TEXTURE28,txtrRes,txtrRes,4);
					int desiredRes = 256;
					for (size_t i = 0; i < chosenTextureResIndex; i++)
					{
						desiredRes*=2;
					}
					
					GLubyte* resizedData = new GLubyte[desiredRes*desiredRes*4];
					stbir_resize_uint8(data, txtrRes, txtrRes, 0,resizedData,desiredRes, desiredRes, 0, 4);
					gl.texImage(resizedData,desiredRes,desiredRes,GL_RGBA);
					gl.generateMipmap();
					delete[] resizedData;
				}
			}
		}
		else{
			chosenTextureResIndex = previousTextureResIndex;
		}
	}
	previousTextureResIndex = chosenTextureResIndex;
	

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
		unsigned int prefilteredMap = load.createPrefilterMap(programs,cubemaps,glfwGetVideoMode(glfwGetPrimaryMonitor())->width,glfwGetVideoMode(glfwGetPrimaryMonitor())->height);
		cubemaps.prefiltered = prefilteredMap;
	}
	glUseProgram(programs.uiProgram);
	
	if(textureSelectionPanel.active){
		ui.textureSelectionPanel(textureSelectionPanel,albedoTextures,programs,renderData.window,mouseXpos,mouseYpos,screenGapX, glfwGetVideoMode(glfwGetPrimaryMonitor())->width,icons.Circle,firstClick);
		textureSelectionPanel.firstCycle = false;
	}
	else
		textureSelectionPanel.firstCycle = true;

	ui.textureDisplayer(textureDisplayer.ndWidth,textureDisplayer.ndHeight,textureDisplayer.buttonPosX - 1.0f +screenGapX,textureDisplayer.buttonPosY,0.999999f); 


	alertState = 0;
	
	lastMouseX = mouseXpos;
	lastMouseY = mouseYpos;

	uiOut.currentBrushMaskTxtr = currentBrushMaskTexture;
	return uiOut;
}
//--------------------RENDER UI --------------------\\

void UserInterface::alert(std::string message,int duration,bool success){
	LibAL_stopPlaying(uiAudios.Alert);
	LibAL_playAudioObject(uiAudios.Alert);
	
	alertIsSuccess = success;
	alertState = 1;
	alertMessage = message;
	alertDuration = duration;
}