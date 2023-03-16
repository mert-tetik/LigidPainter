#include<iostream>

#include <string>
#include <vector>
#include <map>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

#include "Core/LigidPainter.h"
#include "Core/UI/UserInterface.h"
#include "Core/gl.h"
#include "Core/Utilities.h"
#include "Core/Render/Render.h"
#include "Core/Load.hpp"

Programs uiPrograms;

ColorData colorD;

Objects uiObjects;

int uiMaxScreenWidth;
int uiMaxScreenHeight;

int uiTextBoxActiveChar;

unsigned int circleIcon;

void UserInterface::box(float width, float height, float position_x, float position_y,std::string text,glm::vec4 color, float textRatio,bool isTextBox,bool isMaskImageBox,float z,float buttonCurveReduce, glm::vec4 colorTransitionColor, float mixVal) {
	
	//buttonCurveReduce = 10 | normal  
	//buttonCurveReduce = >10 | smaller
	//buttonCurveReduce = <10 | bigger
	//buttonCurveReduce = 10000 to get a square

	//parameter z = position_z
	GlSet glset;

	glm::mat4 scale = glm::mat4(1);
	scale = glm::scale(scale,glm::vec3(width,height,1));
	glset.uniformMatrix4fv(uiPrograms.uiProgram,"scale",scale);
	
	glm::vec3 pos = glm::vec3(position_x,position_y,z);
	glset.uniform3fv(uiPrograms.uiProgram,"pos",pos);

	glset.uniform1i(uiPrograms.uiProgram,"isUiTextureUsed",isMaskImageBox);
	glset.uniform4fv(uiPrograms.uiProgram,"uiColor",color);
	glset.uniform4fv(uiPrograms.uiProgram, "uiTransitionColor", colorTransitionColor);
	glset.uniform1f(uiPrograms.uiProgram, "uiTransitionMixVal", mixVal);

	glBindBuffer(GL_ARRAY_BUFFER,uiObjects.sqrVBO);
	glBindVertexArray(uiObjects.sqrVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	ColorData colorData;


	if (!isTextBox && text != "") {
		renderText(uiPrograms.uiProgram, text, position_x -textRatio, position_y - 0.01f, 0.00022f,colorData.textColor,z+0.001f,false);
	}
	else if(text != ""){
		renderText(uiPrograms.uiProgram, text, -width + position_x, position_y - 0.01f, 0.00022f,colorData.textColor,z+0.001f,mixVal > 0.f);
	}

	scale = glm::mat4(1);
	pos = glm::vec3(0);
	glset.uniformMatrix4fv(uiPrograms.uiProgram,"scale",scale);
	glset.uniform3fv(uiPrograms.uiProgram,"pos",pos);

	glset.uniform1i(uiPrograms.uiProgram, "isUiTextureUsed", 0);

	if(buttonCurveReduce <= 200){
		glUseProgram(uiPrograms.iconsProgram);
		if(width != 0.)
			iconBox((height/2.)*1.3,height*1.3,position_x-width,position_y,z,circleIcon,mixVal,color,colorTransitionColor);
		iconBox((height/2.)*1.3,height*1.3,position_x+width,position_y,z,circleIcon,mixVal,color,colorTransitionColor);
		glUseProgram(uiPrograms.uiProgram);
	}
	glBindBuffer(GL_ARRAY_BUFFER,uiObjects.VBO);
	glBindVertexArray(uiObjects.VAO);
}

void UserInterface::panel(float panelLoc, Icons icons,PanelData panelData) {
	GlSet glset;

	const float panelWidth = 0.2f;
	const float panelHeigth = 0.88f;
	
	const float cornerWidth = 0.04f;

	const float panelZ = 0.2f;

	glUseProgram(uiPrograms.uiProgram);
	box(panelWidth, panelHeigth - cornerWidth, panelLoc + panelWidth + cornerWidth, 0.0f, "", colorD.panelColor, 0.022f, false, false, panelZ, 10000, colorD.panelColor, 0);
	
	box(panelWidth, cornerWidth, panelLoc + panelWidth + cornerWidth, panelHeigth, "", colorD.panelColor, 0.022f, false, false, panelZ, 10000, colorD.panelColor, 0);
	box(panelWidth, cornerWidth, panelLoc + panelWidth + cornerWidth, -panelHeigth, "", colorD.panelColor, 0.022f, false, false, panelZ, 10000, colorD.panelColor, 0);
	box(cornerWidth, panelHeigth - cornerWidth, panelLoc + cornerWidth, 0.0f, "", colorD.panelColor, 0.022f, false, false, panelZ, 10000, colorD.panelColor, 0);
	
	glUseProgram(uiPrograms.iconsProgram);
	circle(panelLoc + cornerWidth - 0.002f, panelHeigth - cornerWidth + 0.002f,panelZ,cornerWidth+0.01f,(cornerWidth+0.01f)*2,icons.Circle,colorD.panelColor);
	circle(panelLoc + cornerWidth,-panelHeigth + cornerWidth,panelZ,cornerWidth+0.01f,(cornerWidth+0.01f)*2,icons.Circle,colorD.panelColor);

	//Texture panel button
	iconBox(0.017f,0.034f,panelLoc - 0.017f,0.8f,panelZ+0.01f,icons.PanelButtonR,0,colorD.panelHoldColor,colorD.panelHoldColor);
	iconBox(0.01f,0.02f,panelLoc - 0.013f,0.808f,0.6f,icons.TDModel,0,colorD.iconColor,colorD.iconColor);
	
	//Material panel button
	iconBox(0.017f,0.034f,panelLoc - 0.017f,0.72f,panelZ+0.01f,icons.PanelButtonR,0,colorD.panelHoldColor,colorD.panelHoldColor);
	iconBox(0.012f,0.024f,panelLoc - 0.013f,0.725f,0.6f,icons.Material,0,colorD.iconColor,colorD.iconColor);

	//Painting panel button
	iconBox(0.017f,0.034f,panelLoc - 0.017f,0.64f,panelZ+0.01f,icons.PanelButtonR,0,colorD.panelHoldColor,colorD.panelHoldColor);
	iconBox(0.012f,0.024f,panelLoc - 0.013f,0.647f,0.6f,icons.Painting,0,colorD.iconColor,colorD.iconColor);

	//Export panel button
	iconBox(0.017f,0.034f,panelLoc - 0.017f,0.56f,panelZ+0.01f,icons.PanelButtonR,0,colorD.panelHoldColor,colorD.panelHoldColor);
	iconBox(0.012f,0.024f,panelLoc - 0.013f,0.567f,0.6f,icons.Export,0,colorD.iconColor,colorD.iconColor);

	//Settings panel button
	iconBox(0.017f,0.034f,panelLoc - 0.017f,0.48f,panelZ+0.01f,icons.PanelButtonR,0,colorD.panelHoldColor,colorD.panelHoldColor);
	iconBox(0.011f,0.022f,panelLoc - 0.013f,0.485f,0.6f,icons.Gear,0,colorD.iconColor,colorD.iconColor);

	//Generator panel button
	iconBox(0.017f,0.034f,panelLoc - 0.017f,0.4f,panelZ+0.01f,icons.PanelButtonR,0,colorD.panelHoldColor,colorD.panelHoldColor);
	iconBox(0.011f,0.022f,panelLoc - 0.013f,0.405f,0.6f,icons.TextureGenerator,0,colorD.iconColor,colorD.iconColor);

	glUseProgram(uiPrograms.uiProgram);
	
	//Barriers
	if(panelData.texturePanelActive){
		box(panelWidth*1.2, cornerWidth*2, panelLoc + panelWidth + cornerWidth, panelHeigth + cornerWidth*2, "", glm::vec4(0), 0.022f, false, false, 1.f, 10000, glm::vec4(0), 0);
		box(panelWidth*1.2, cornerWidth*2, panelLoc + panelWidth + cornerWidth, -panelHeigth - cornerWidth*2, "", glm::vec4(0), 0.022f, false, false, 1.f, 10000, glm::vec4(0), 0);
	}

}

bool sndpanelMoveTexture = false;
bool sndpanelFolderPressed = false;
int sndpanelFolderCounter = 0;

bool sndpanelSliderPressed = false;

void UserInterface::sndPanel(int state,float panelLoc,Programs programs,Icons icons,std::vector<aTexture> &albedoTextures, GLFWwindow* window,double mouseXpos,double mouseYpos,float screenGapX,float maxScreenWidth, int& selectedAlbedoTextureIndex,std::vector<NodeScene>& nodeScenes,int &selectedNodeScene,bool& newModelAdded,float &txtrSlideVal,float &materialSlideVal,bool &firstClick,ColoringPanel &clringPanel,TextureCreatingPanel &txtrCreatingPanel,bool& anyTextureNameActive,std::string &textureText,int& folderIndex,NodePanel &nodePanel,std::vector<Node> appNodes,SndPanel &sndpnl,BrushMaskTextures &brushMaskTextures,bool maskPanelEnter,float yOffset,std::vector<NodeScene> &nodeScenesHistory) {
	GlSet glset;
	ColorData colorData;
	
	int screenWidth;
	int screenHeight;
	glfwGetFramebufferSize(window,&screenWidth,&screenHeight);

	if(!anyTextureNameActive){
		for (int i = 0; i < albedoTextures.size(); i++)
		{
			albedoTextures[i].nameTextActive = false;
		}
	}

	const float panelWidth = 0.2f;
	const float panelHeigth = 0.88f;
	
	const float cornerWidth = 0.04f;

	const float panelZ = 0.2f;
	
	glUseProgram(programs.uiProgram);
	
	

	//Slider
	box(0.007f, panelHeigth - cornerWidth, panelLoc - panelWidth*2 + cornerWidth/2.5f , +0.014f, "", colorD.panelColorSnd, 0.022f, false, false, panelZ+0.01, 10000, colorD.panelColor, 0);
	int elementSize = 0;
	float slideVal = 0;
	if(sndpnl.state == 0){
		slideVal = sndpnl.texturePanelSlideVal;
		elementSize = albedoTextures.size();
	}
	if(sndpnl.state == 1){
		slideVal = sndpnl.materialPanelSlideVal;
		elementSize = nodeScenes.size();
	}
		
	float slidePHeight = (panelHeigth - cornerWidth) * (7.f/(max((float)albedoTextures.size()/3.f,7.f))); 
	slideVal *= slidePHeight;
	
	if(sndpnl.state == 0){
		if(albedoTextures.size() > 21 && albedoTextures.size() <= 27){
			slidePHeight -= 0.1f;
		}
	}
	if(sndpnl.state == 1){
		if(nodeScenes.size() > 21 && nodeScenes.size() <= 27){
			slidePHeight -= 0.1f;
		}
	}

	if(firstClick && isMouseOnButton(window,0.007f,slidePHeight,panelLoc - panelWidth*2 + cornerWidth/2.5f,max(-slideVal + (panelHeigth - cornerWidth), -(panelHeigth - cornerWidth-0.014f)) - slidePHeight,mouseXpos,mouseYpos,false)){
		sndpanelSliderPressed = true;
	}
	if(sndpanelSliderPressed){
		if(!sndpnl.state){
			txtrSlideVal += yOffset/(uiMaxScreenHeight/2);
			if(txtrSlideVal < 0.f)
					txtrSlideVal = 0.001f;
		}
		if(sndpnl.state){
			materialSlideVal += yOffset/(uiMaxScreenHeight/2);
			if(materialSlideVal < 0.f)
				materialSlideVal = 0.001f;
		}
		slideVal += (yOffset/(uiMaxScreenHeight/2))*slidePHeight;
		if(slideVal < 0.f)
			slideVal = 0.001f;
	}
	if(glfwGetMouseButton(window,0) == GLFW_RELEASE){
		sndpanelSliderPressed = false;
	}

	if(slidePHeight + slideVal/2.f > 0.82f){
		slideVal = (+0.82 - slidePHeight)*2;
		if(!sndpnl.state)
			txtrSlideVal = slideVal/slidePHeight;
		if(sndpnl.state)
			materialSlideVal = slideVal/slidePHeight;
	}
	


	box(0.007f, slidePHeight, panelLoc - panelWidth*2 + cornerWidth/2.5f , max(-slideVal + (panelHeigth - cornerWidth), -(panelHeigth - cornerWidth-0.014f)) - slidePHeight, "", colorD.panelHoldColor, 0.022f, false, false, panelZ+0.02, 10000, colorD.panelColor, 0);
	
	
	box(panelWidth, panelHeigth - cornerWidth, panelLoc - panelWidth - cornerWidth, 0.0f, "", colorD.panelColor, 0.022f, false, false, panelZ, 10000, colorD.panelColor, 0);
	
	box(panelWidth, cornerWidth, panelLoc - panelWidth - cornerWidth, panelHeigth, "", colorD.panelColor, 0.022f, false, false, panelZ, 10000, colorD.panelColor, 0);
	
	box(panelWidth, cornerWidth, panelLoc - panelWidth - cornerWidth, -panelHeigth, "", colorD.panelColor, 0.022f, false, false, panelZ, 10000, colorD.panelColor, 0);
	box(cornerWidth, panelHeigth - cornerWidth, panelLoc - cornerWidth, 0.0f, "", colorD.panelColor, 0.022f, false, false, panelZ, 10000, colorD.panelColor, 0);

	glUseProgram(programs.iconsProgram);
	circle(panelLoc - cornerWidth + 0.002f,panelHeigth - cornerWidth + 0.002f,panelZ,cornerWidth+0.01f,(cornerWidth+0.01f)*2,icons.Circle,colorD.panelColor);
	circle(panelLoc - cornerWidth,-panelHeigth + cornerWidth,panelZ,cornerWidth+0.01f,(cornerWidth+0.01f)*2,icons.Circle,colorD.panelColor);

	//Texture panel button
	iconBox(0.017f,0.034f,panelLoc + 0.017f,0.8f,panelZ+0.01f,icons.PanelButtonL,0,colorD.panelHoldColor,colorD.panelHoldColor);
	iconBox(0.01f,0.02f,panelLoc + 0.013f,0.808f,panelZ+0.02f,icons.Texture,0,colorD.iconColor,colorD.iconColor);


	//Material panel button
	iconBox(0.017f,0.034f,panelLoc + 0.017f,0.72f,panelZ+0.01f,icons.PanelButtonL,0,colorD.panelHoldColor,colorD.panelHoldColor);
	iconBox(0.012f,0.024f,panelLoc + 0.013f,0.725f,panelZ+0.02f,icons.Material,0,colorD.iconColor,colorD.iconColor);
	
	if(panelLoc > -0.98f){

		if(state == 0){
			if(folderIndex != 10000)
				iconBox(0.01f,0.02f,panelLoc - 0.365f,0.86f,panelZ+0.01f,icons.Undo,sndpnl.backSignMixVal,colorD.iconColor,colorD.iconColorHover);
			iconBox(0.01f,0.02f,panelLoc - 0.05f,0.85f,panelZ+0.01f,icons.ArrowDown,sndpnl.downSignMixVal,colorD.iconColor,colorD.iconColorHover);
			iconBox(0.01f,0.02f,panelLoc - 0.17f,0.85f,panelZ+0.01f,icons.LoadFolder,sndpnl.folderSignMixVal,colorD.iconColor,colorD.iconColorHover);
		}

		iconBox(0.01f,0.02f,panelLoc - 0.08f,0.85f,panelZ+0.01f,icons.Plus,sndpnl.plusSignMixVal,colorD.iconColor,colorD.iconColorHover);
		iconBox(0.01f,0.02f,panelLoc - 0.11f,0.85f,panelZ+0.01f,icons.Minus,sndpnl.minusSignMixVal,colorD.iconColor,colorD.iconColorHover);
		iconBox(0.01f,0.02f,panelLoc - 0.14f,0.85f,panelZ+0.01f,icons.Mirror,sndpnl.duplicateSignMixVal,colorD.iconColor,colorD.iconColorHover);
	


		glUseProgram(programs.uiProgram);

		//Barrier
		box(panelWidth+0.05f, 0.5f, panelLoc - panelWidth - cornerWidth, panelHeigth+0.42f, "", glm::vec4(0), 0.022f, false, false, panelZ+0.05, 10000, colorD.panelColor, 0);
		box(panelWidth+0.05f, 0.5f, panelLoc - panelWidth - cornerWidth, -panelHeigth-0.42f, "", glm::vec4(0), 0.022f, false, false, panelZ+0.05, 10000, colorD.panelColor, 0);

		const float screenGapXSc = maxScreenWidth-screenWidth;

		if(sndpanelMoveTexture){
			float posx = ((mouseXpos + screenGapXSc/2.f)/(maxScreenWidth/2))-1.f;
			float posy = (2.f - (mouseYpos/(uiMaxScreenHeight/2.f)))-1.f;
			std::vector<float> renderVertices = { 
				// first triangle
				 0.05f + posx,  0.1f+ posy, 1.0f,1,1,0,0,0,  // top right
				 0.05f+ posx,  -0.1f+ posy, 1.0f,1,0,0,0,0,  // bottom right
				 -0.05f+ posx,  0.1f+ posy, 1.0f,0,1,0,0,0,  // top left 
				// second tria0gle	  ,0,0,0,
				 0.05f+ posx,  -0.1f+ posy, 1.0f,1,0,0,0,0,  // bottom right
				 -0.05f+ posx,  -0.1f+ posy, 1.0f,0,0,0,0,0,  // bottom left
				 -0.05f+ posx,  0.1f+ posy, 1.0f,0,1,0,0,0   // top left
			};
			if(albedoTextures[selectedAlbedoTextureIndex].isTexture){
				glUseProgram(programs.renderTheTextureProgram);
				glActiveTexture(GL_TEXTURE14);
				glBindTexture(GL_TEXTURE_2D,albedoTextures[selectedAlbedoTextureIndex].id);
				glset.uniform1i(programs.renderTheTextureProgram, "isPressed" ,0);
				glset.uniform1i(programs.renderTheTextureProgram, "isHover" ,0);
				glset.drawArrays(renderVertices,false);
			}
			else{
				glUseProgram(programs.iconsProgram);
				glActiveTexture(GL_TEXTURE6);
				glBindTexture(GL_TEXTURE_2D,icons.Folder);
				glset.uniform4fv(programs.iconsProgram,"iconColor",colorData.iconColor);
				glset.uniform1f(programs.iconsProgram,"iconMixVal",0);
				glset.drawArrays(renderVertices,false);
			}
		}
		if(albedoTextures.size()){
			if(albedoTextures[selectedAlbedoTextureIndex].isTexture && sndpanelMoveTexture && glfwGetMouseButton(window,0) == GLFW_RELEASE && maskPanelEnter){
				brushMaskTextures.textures.push_back(albedoTextures[selectedAlbedoTextureIndex].id);
				brushMaskTextures.names.push_back(albedoTextures[selectedAlbedoTextureIndex].name);
			}
			if(albedoTextures[selectedAlbedoTextureIndex].isTexture && sndpanelMoveTexture && glfwGetMouseButton(window,0) == GLFW_RELEASE && nodePanel.panelHover){
				Node imageNode;
				imageNode = appNodes[1];
				imageNode.outputs[0].nodeConnectionIndex = 10000;
				imageNode.outputs[0].inputConnectionIndex = 10000;
				imageNode.outputs[0].pressed = false;
				imageNode.outputs[0].connectionHover = false;
				imageNode.marked = false;
				imageNode.stateChanged = true;
				imageNode.active = true;

				imageNode.inputs[0].selectedTexture = albedoTextures[selectedAlbedoTextureIndex].id;
				imageNode.inputs[0].selectedTextureName = albedoTextures[selectedAlbedoTextureIndex].name;
				imageNode.inputs[0].selectedTextureIndex = selectedAlbedoTextureIndex;

				imageNode.positionX = (mouseXpos/screenWidth*2 - 1.0f) / nodePanel.zoomVal - nodePanel.panelPositionX;
				imageNode.positionY = ((-mouseYpos/uiMaxScreenHeight*2 + 1.0f) / nodePanel.zoomVal - nodePanel.panelPositionY);

				nodeScenes[selectedNodeScene].nodes.push_back(imageNode);
			}
		}

		if(state == 0){
			glUseProgram(programs.uiProgram);
			if(folderIndex != 10000)
				renderText(programs.uiProgram,albedoTextures[folderIndex].name,panelLoc-0.35f,0.84f,0.00032f,colorData.textColor,0.99999f,false);
			else
				renderText(programs.uiProgram,"Textures",panelLoc-0.35f,0.84f,0.00032f,colorData.textColor,0.99999f,false);

			glUseProgram(programs.renderTheTextureProgram);

			glActiveTexture(GL_TEXTURE14);
			glset.uniform1i(uiPrograms.renderTheTextureProgram, "txtr" ,14);


			float maskXpos = 0.0f;
			float maskYpos = 0.0f;
			//RENDER THE TEXTURES
			int lC = 0;
			for (size_t i = 0; i < albedoTextures.size(); i++)
			{
				if(albedoTextures[i].folderIndex == folderIndex){
					glUseProgram(programs.renderTheTextureProgram);

					if(lC % 3 == 0 && lC != 0){
						maskYpos-=0.23f;
						maskXpos=0.0f;
					}
					maskXpos-=0.12f;

					const float startingPoint = 0.7f;

					float position_x = panelLoc - maskXpos - panelWidth*2 - 0.04f;
					float position_y = startingPoint + maskYpos + txtrSlideVal;
					//ui.iconBox(0.025f, 0.05f,centerCoords - screenGapX - maskXpos - 0.2f,0.8f + maskYpos - maskPanelSliderValue*(maskTextures.size()/4) - 0.05f,1,maskTextures[i],0);

					const float textureWidth = 0.05f;

					const float maxTop = startingPoint + textureWidth*2;
					const float minBot = -0.8f;

					float upBotDifMin = std::min(0.05f + position_y,maxTop) - std::min(-0.05f + position_y,maxTop);
					float upBotDifMax = std::max(0.05f + position_y,minBot) - std::max(-0.05f + position_y,minBot);



					std::vector<float> buttonCoorSq{
						// first triangle
						 textureWidth + position_x,  std::min(std::max( textureWidth*2 + position_y,minBot),maxTop), 	panelZ+0.02f,	1,		upBotDifMin*10			,0,0,0,  // top right
						 textureWidth + position_x,  std::min(std::max(-textureWidth*2 + position_y,minBot),maxTop), 	panelZ+0.02f,	1,		1.0f-upBotDifMax*10		,0,0,0,  // bottom right
						-textureWidth + position_x,  std::min(std::max( textureWidth*2 + position_y,minBot),maxTop), 	panelZ+0.02f,	0,		upBotDifMin*10			,0,0,0,  // top left 
						// second triangle						   	
						 textureWidth + position_x,  std::min(std::max(-textureWidth*2 + position_y,minBot),maxTop), 	panelZ+0.02f,	1,		1.0f-upBotDifMax*10		,0,0,0,  // bottom right
						-textureWidth + position_x,  std::min(std::max(-textureWidth*2 + position_y,minBot),maxTop), 	panelZ+0.02f,	0,		1.0f-upBotDifMax*10		,0,0,0,  // bottom left
						-textureWidth + position_x,  std::min(std::max( textureWidth*2 + position_y,minBot),maxTop), 	panelZ+0.02f,	0,		upBotDifMin*10			,0,0,0  // top left
					};
					bool isHover = false;
					bool isPressed = false;

					if(isMouseOnCoords(window,mouseXpos+screenGapX*(maxScreenWidth/2),mouseYpos,buttonCoorSq,false) && !clringPanel.active && !txtrCreatingPanel.active){
						glset.uniform1i(uiPrograms.renderTheTextureProgram, "isHover" ,1);
						isHover = true;
						if(!albedoTextures[i].isTexture && sndpanelMoveTexture && glfwGetMouseButton(window,0) == GLFW_RELEASE){
							if(selectedAlbedoTextureIndex != i)
								albedoTextures[selectedAlbedoTextureIndex].folderIndex = i;
						}
						if(firstClick){
							if(albedoTextures[i].isTexture){
								glActiveTexture(GL_TEXTURE0);
								glBindTexture(GL_TEXTURE_2D,albedoTextures[selectedAlbedoTextureIndex].id);
								selectedAlbedoTextureIndex = i;
							}
							else{
								sndpanelFolderPressed = true;
								selectedAlbedoTextureIndex = i;
							}
							sndpanelMoveTexture = true;
						}
					}
					else{
						glset.uniform1i(uiPrograms.renderTheTextureProgram, "isHover" ,0);
					}

					if(selectedAlbedoTextureIndex == i){
						isPressed = true;
						glset.uniform1i(uiPrograms.renderTheTextureProgram, "isPressed" ,1);
					}
					else{
						glset.uniform1i(uiPrograms.renderTheTextureProgram, "isPressed" ,0);
					}
					glActiveTexture(GL_TEXTURE14);
					glBindTexture(GL_TEXTURE_2D,albedoTextures[i].id);

					if(!albedoTextures[i].isTexture){
						glm::vec4 iconColor = glm::vec4(1);


					if(!isHover && !isPressed)
						iconColor = colorData.materialIconColor;

					else if(isHover && !isPressed)
						iconColor = colorData.materialIconColorHover;

					else if(!isHover && isPressed)
						iconColor = colorData.materialIconColorActive;

					else if(isHover && isPressed)
						iconColor = colorData.materialIconColorActiveHover;

						glUseProgram(programs.iconsProgram);
						glActiveTexture(GL_TEXTURE6);
						glBindTexture(GL_TEXTURE_2D,icons.Folder);
						glset.uniform4fv(programs.iconsProgram,"iconColor",iconColor);
						glset.uniform1f(programs.iconsProgram,"iconMixVal",0);
					}
					glset.drawArrays(buttonCoorSq,false);

					glUseProgram(programs.uiProgram);

					bool textHover = isMouseOnButton(window,0.06f,0.015f,position_x-screenGapX,position_y - textureWidth*2.5,mouseXpos,mouseYpos,false);

					if(textHover && firstClick && !anyTextureNameActive){
						albedoTextures[i].nameTextActive = true;
						textureText = albedoTextures[i].name;
						anyTextureNameActive = true;
					}
					if(albedoTextures[i].nameTextActive){
						box(0.06f, 0.015f, position_x,position_y - textureWidth*2.5, textureText, colorData.buttonColor, 0.022f, true, false, panelZ+0.05, 10000, colorD.panelColor, 0.001f);
					}
					else{
						Utilities util;
						renderText(programs.uiProgram,util.cropString(albedoTextures[i].name,15),position_x- textureWidth ,position_y - textureWidth*2.5,0.00017f,colorData.textColor,panelZ+0.02f,false);
					}
					lC++;
				}
			}
		}

		if(state == 1){

			glUseProgram(programs.uiProgram);
			renderText(programs.uiProgram,"Materials",panelLoc-0.35f,0.84f,0.00032f,colorData.textColor,0.99999f,false);

			//RENDER THE NODES
			float maskXpos = 0.0f;
			float maskYpos = 0.0f;
			for (size_t i = 0; i < nodeScenes.size(); i++)
			{
				if(i % 3 == 0 && i != 0){
					maskYpos-=0.23f;
					maskXpos=0.0f;
				}
				maskXpos-=0.12f;

				const float startingPoint = 0.7f;

				float position_x = panelLoc - maskXpos - panelWidth*2 - 0.04f;
				float position_y = startingPoint + maskYpos + materialSlideVal;
				//ui.iconBox(0.025f, 0.05f,centerCoords - screenGapX - maskXpos - 0.2f,0.8f + maskYpos - maskPanelSliderValue*(maskTextures.size()/4) - 0.05f,1,maskTextures[i],0);

				const float textureWidth = 0.05f;

				const float maxTop = startingPoint + textureWidth*2;
				const float minBot = -0.8f;

				float upBotDifMin = std::min(0.05f + position_y,maxTop) - std::min(-0.05f + position_y,maxTop);
				float upBotDifMax = std::max(0.05f + position_y,minBot) - std::max(-0.05f + position_y,minBot);

				std::vector<float> buttonCoorSq{
					// first triangle
					 textureWidth + position_x,  std::min(std::max( textureWidth*2 + position_y,minBot),maxTop), 	panelZ+0.02f,	1,		upBotDifMin*10			,0,0,0,  // top right
					 textureWidth + position_x,  std::min(std::max(-textureWidth*2 + position_y,minBot),maxTop), 	panelZ+0.02f,	1,		1.0f-upBotDifMax*10		,0,0,0,  // bottom right
					-textureWidth + position_x,  std::min(std::max( textureWidth*2 + position_y,minBot),maxTop), 	panelZ+0.02f,	0,		upBotDifMin*10			,0,0,0,  // top left 
					// second triangle						   
					 textureWidth + position_x,  std::min(std::max(-textureWidth*2 + position_y,minBot),maxTop), 	panelZ+0.02f,	1,		1.0f-upBotDifMax*10		,0,0,0,  // bottom right
					-textureWidth + position_x,  std::min(std::max(-textureWidth*2 + position_y,minBot),maxTop), 	panelZ+0.02f,	0,		1.0f-upBotDifMax*10		,0,0,0,  // bottom left
					-textureWidth + position_x,  std::min(std::max( textureWidth*2 + position_y,minBot),maxTop), 	panelZ+0.02f,	0,		upBotDifMin*10			,0,0,0  // top left
				};
				ColorData colorData;
				bool isHover;
				bool isPressed;

				glm::vec4 iconColor;
				if(isMouseOnCoords(window,mouseXpos+screenGapX*(maxScreenWidth/2),mouseYpos,buttonCoorSq,false)){
					isHover = true;
	
					if(firstClick){
						nodeScenesHistory.clear(); 
						selectedNodeScene = i;
					}
				}
				else{
					isHover = false;
				}

				if(selectedNodeScene == i){
					isPressed = true;
				}
				else{
					isPressed = false;
				}



				if(!isHover && !isPressed)
					iconColor = colorData.materialIconColor;

				else if(isHover && !isPressed)
					iconColor = colorData.materialIconColorHover;

				else if(!isHover && isPressed)
					iconColor = colorData.materialIconColorActive;

				else if(isHover && isPressed)
					iconColor = colorData.materialIconColorActiveHover;



				glUseProgram(programs.iconsProgram);
				glActiveTexture(GL_TEXTURE6);
				glBindTexture(GL_TEXTURE_2D,icons.Material);
				glset.uniform4fv(programs.iconsProgram,"iconColor",iconColor);
				glset.uniform1f(programs.iconsProgram,"iconMixVal",0);
				glset.drawArrays(buttonCoorSq,false);

				glUseProgram(programs.uiProgram);
				renderText(programs.uiProgram,nodeScenes[i].sceneName,position_x- textureWidth ,position_y - textureWidth*2,0.00022f,colorData.textColor,panelZ+0.03f,false);

				renderText(programs.uiProgram,std::to_string(nodeScenes[i].index),position_x - textureWidth ,position_y - textureWidth,0.00042f,colorData.materialIconIndexTextColor,panelZ+0.03f,false);
			}
		}
		if(sndpanelMoveTexture && glfwGetMouseButton(window,0) == GLFW_RELEASE && sndpnl.backSignHover){
			albedoTextures[selectedAlbedoTextureIndex].folderIndex = 10000;
		}
		if(glfwGetMouseButton(window,0) == GLFW_RELEASE){

			if(sndpanelFolderCounter < 30 && sndpanelFolderPressed){
				folderIndex = selectedAlbedoTextureIndex;
			} 
			sndpanelMoveTexture = false;
			sndpanelFolderPressed = false;
			sndpanelFolderCounter = 0;
		}
		else{
			sndpanelFolderCounter++;
		}
		glUseProgram(programs.uiProgram);
	}

}

void UserInterface::textureSelectionPanel(TextureSelectionPanel &textureSelectionPanel,std::vector<aTexture> &albedoTextures,Programs programs,GLFWwindow* window,double mouseXpos,double mouseYpos,float screenGapX,int maxScreenWidth,unsigned int circleTexture){
	ColorData clrData;
	GlSet glset;

	const float boxWidth = 0.25f;


	textureSelectionPanel.panelHover = isMouseOnButton(window,boxWidth,boxWidth,textureSelectionPanel.posX - screenGapX,textureSelectionPanel.posY,mouseXpos,mouseYpos,false);
	if(!textureSelectionPanel.panelHover){
		textureSelectionPanel.active = false;
	}

	container(textureSelectionPanel.posX,textureSelectionPanel.posY,0.99999f,boxWidth,boxWidth,clrData.textureSelectionPanelColor,programs,circleTexture);
	//box(boxWidth,boxWidth,textureSelectionPanel.posX,textureSelectionPanel.posY,"", clrData.textureSelectionPanelColor ,0,0,0,0.99999,10000,clrData.textureSelectionPanelColor,0);

	glUseProgram(programs.renderTheTextureProgram);
	
	glActiveTexture(GL_TEXTURE14);
	glset.uniform1i(uiPrograms.renderTheTextureProgram, "txtr" ,14);


	float maskXpos = 0.0f;
	float maskYpos = 0.0f;

	for (size_t i = 0; i < albedoTextures.size(); i++)
	{
		if(albedoTextures[i].isTexture){
			if(i % 10 == 0 && i != 0){
				maskYpos-=0.1f;
				maskXpos=0.0f;
			}
			maskXpos-=0.05f;

			const float startingPoint = textureSelectionPanel.posY;

			const float textureWidth = 0.022f;

			float position_x = textureSelectionPanel.posX - maskXpos - boxWidth - textureWidth*1.2;
			float position_y = startingPoint + maskYpos + boxWidth - textureWidth*2;
			//ui.iconBox(0.025f, 0.05f,centerCoords - screenGapX - maskXpos - 0.2f,0.8f + maskYpos - maskPanelSliderValue*(maskTextures.size()/4) - 0.05f,1,maskTextures[i],0);


			const float maxTop = startingPoint + textureWidth*2 + boxWidth;
			const float minBot = -0.8f;

			float upBotDifMin = std::min(0.05f + position_y,maxTop) - std::min(-0.05f + position_y,maxTop);
			float upBotDifMax = std::max(0.05f + position_y,minBot) - std::max(-0.05f + position_y,minBot);
			
			glActiveTexture(GL_TEXTURE14);
			glBindTexture(GL_TEXTURE_2D,albedoTextures[i].id);

			std::vector<float> buttonCoorSq{
				// first triangle
				 textureWidth + position_x,  std::min(std::max( textureWidth*2 + position_y,minBot),maxTop), 	1,	1,		upBotDifMin*10			,0,0,0,  // top right
				 textureWidth + position_x,  std::min(std::max(-textureWidth*2 + position_y,minBot),maxTop), 	1,	1,		1.0f-upBotDifMax*10		,0,0,0,  // bottom right
				-textureWidth + position_x,  std::min(std::max( textureWidth*2 + position_y,minBot),maxTop), 	1,	0,		upBotDifMin*10			,0,0,0,  // top left 
				// second triangle						   	
				 textureWidth + position_x,  std::min(std::max(-textureWidth*2 + position_y,minBot),maxTop), 	1,	1,		1.0f-upBotDifMax*10		,0,0,0,  // bottom right
				-textureWidth + position_x,  std::min(std::max(-textureWidth*2 + position_y,minBot),maxTop), 	1,	0,		1.0f-upBotDifMax*10		,0,0,0,  // bottom left
				-textureWidth + position_x,  std::min(std::max( textureWidth*2 + position_y,minBot),maxTop), 	1,	0,		upBotDifMin*10			,0,0,0  // top left
			};
			glset.uniform1i(uiPrograms.renderTheTextureProgram, "isPressed" ,0);
			if(isMouseOnCoords(window,mouseXpos+screenGapX*(maxScreenWidth/2) - screenGapX,mouseYpos,buttonCoorSq,false)){
				glUseProgram(programs.uiProgram);
				box(0.1f,0.02f,textureSelectionPanel.posX-boxWidth+0.1f,textureSelectionPanel.posY+boxWidth*1.35f,albedoTextures[i].name, clrData.textureSelectionPanelColor ,0,true,0,0.8,10,clrData.textureSelectionPanelColor,0);
				glUseProgram(programs.renderTheTextureProgram);

				glset.uniform1i(uiPrograms.renderTheTextureProgram, "isHover" ,1);
				if(glfwGetMouseButton(window,0) == GLFW_PRESS){
					textureSelectionPanel.selectedIndex = i;
					textureSelectionPanel.selectedTextureName = albedoTextures[i].name;
					textureSelectionPanel.textureClicked = true;
				}
			}
			else{
				glset.uniform1i(uiPrograms.renderTheTextureProgram, "isHover" ,0);
			}

			glset.drawArrays(buttonCoorSq,false);
		}
	}
	glUseProgram(programs.uiProgram);
}

void UserInterface::textureDisplayer(float width,float height, float position_x,float position_y,float z){ 
	GlSet glset;
	ColorData clrData;
	std::vector<float> buttonCoorSq{
		// first triangle
		 width + position_x,  position_y, z,1,1,0,0,0,  // top right
		 width + position_x,  -height +position_y, z,1,0,0,0,0,  // bottom right
		 position_x,  position_y, z,0,1,0,0,0,  // top left 
		// second triangle						   
		 width + position_x,   -height +position_y, z,1,0,0,0,0,  // bottom right
		 position_x, -height +position_y, z,0,0,0,0,0,  // bottom left
		 position_x,  position_y, z,0,1,0,0,0  // top left
	};

	glset.uniform1i(uiPrograms.uiProgram,"drawTxtrDemonstrator",1);
	glset.drawArrays(buttonCoorSq,false);
	glset.uniform1i(uiPrograms.uiProgram,"drawTxtrDemonstrator",0);
	box(0.0f,0.035f,position_x+0.005f,position_y-0.01f,"", clrData.textureDisplayerButtonColor,0,0,0,1,10,glm::vec4(0,0,0,1),0);
}

bool vertRGBarPressed = false;
bool UserInterface::verticalRangeBar(float positionX,float positionY,float height,float &orgvalue,float value,GLFWwindow* window,float mouseX,float mouseY,float yOffset,bool &firstClick,int textureSize,float screenGapX){
	ColorData colorData;

	//Pointer 
	box(0.0f, 0.015f, positionX, positionY - value, "",colorData.maskPanelSliderColor, 0.095f, false, false, 0.9f, 20, glm::vec4(0), 0); //Mask panel slider
	if(isMouseOnButton(window,0.015f,0.015f,positionX-screenGapX,positionY-value,mouseX,mouseY,false) && firstClick)
		vertRGBarPressed = true;
	
	if(vertRGBarPressed){
		Utilities util;
		orgvalue -= yOffset/(uiMaxScreenHeight/2);
		const float maskPanelRange = ceil(textureSize/3.f) / 8.33333333333 - (0.8f - 0.55f); 
		orgvalue = util.restrictBetween(orgvalue, 0.0f, -maskPanelRange/4.f);//Keep in boundaries
	}
	
	if(glfwGetMouseButton(window,0) == GLFW_RELEASE)
		vertRGBarPressed = false;

	//Background
	box(0.008f, height, positionX, positionY - height, "",colorData.maskPanelSliderBackgroundColor, 0.095f, false, false, 0.9f, 10000, glm::vec4(0), 0); 
	box(0.0f, 0.015f, positionX, positionY , "", colorData.maskPanelSliderBackgroundColor, 0.095f, false, false, 0.9f, 20, glm::vec4(0), 0); 
	box(0.0f, 0.015f, positionX, positionY - height*2.f, "", colorData.maskPanelSliderBackgroundColor, 0.095f, false, false, 0.9f, 20, glm::vec4(0), 0); 

	return false;
}


void UserInterface::numericModifier(float position_x,float position_y,unsigned int leftArrow,unsigned int rightArrow,float z,int value,float mixValP,float mixValN){
	//box(0.005f,0.035f,position_x+0.005f,position_y-0.01f,"",glm::vec3(0),0,0,0,1,10,glm::vec3(0),0);
	ColorData clrData;
	
	glUseProgram(uiPrograms.iconsProgram);
	iconBox(0.02f,0.04f,position_x - 0.06f,position_y,z,leftArrow,mixValN,clrData.numericModifierArrowColor,clrData.numericModifierArrowHoverColor);
	iconBox(0.02f,0.04f,position_x + 0.06f,position_y,z,rightArrow,mixValP,clrData.numericModifierArrowColor,clrData.numericModifierArrowHoverColor);

	glUseProgram(uiPrograms.uiProgram);
	renderText(uiPrograms.uiProgram,std::to_string(value),position_x-0.01f,position_y-0.01f,0.00022f,clrData.textColor,0.99999f,false);
}

void UserInterface::iconBox(float width, float height, float position_x, float position_y, float z,	unsigned int icon,float mixVal,glm::vec4 color,glm::vec4 colorHover){
	GlSet glset;
	ColorData clrData;

	glset.uniform4fv(uiPrograms.iconsProgram,"iconColor",color);
	glset.uniform4fv(uiPrograms.iconsProgram,"iconColorHover",colorHover);
	
	glset.uniform1f(uiPrograms.iconsProgram,"iconMixVal",mixVal);
	glset.activeTexture(GL_TEXTURE6);
	glset.bindTexture(icon);

	glm::mat4 scale = glm::mat4(1);
	scale = glm::scale(scale,glm::vec3(width,height,1));
	glset.uniformMatrix4fv(uiPrograms.uiProgram,"scale",scale);
	
	glm::vec3 pos = glm::vec3(position_x,position_y,z);
	glset.uniform3fv(uiPrograms.uiProgram,"pos",pos);

	glBindBuffer(GL_ARRAY_BUFFER,uiObjects.sqrVBO);
	glBindVertexArray(uiObjects.sqrVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	ColorData colorData;

	scale = glm::mat4(1);
	pos = glm::vec3(0);
	glset.uniformMatrix4fv(uiPrograms.uiProgram,"scale",scale);
	glset.uniform3fv(uiPrograms.uiProgram,"pos",pos);
	
	glBindBuffer(GL_ARRAY_BUFFER,uiObjects.VBO);
	glBindVertexArray(uiObjects.VAO);
}

void UserInterface::circle(float positionX,float positionY,float positionZ,float width, float height, unsigned int circleTexture, glm::vec4 color){
	iconBox(width,height,positionX,positionY,positionZ,circleIcon,0,color,glm::vec4(0));
}

void UserInterface::colorBox(float position_x, float position_y,float valueX, float valueY,Icons icons) {

	ColorData colorData;
	glm::vec3 color = glm::vec3(1.0f,0.0f,0.0f);
	std::vector<float> boxCoor{
		// first triangle								   
		 -0.10f + position_x,  0.20f + position_y, 0.9f,1.0f,1.0f	,1,1,1,  // top right
		 -0.10f + position_x, -0.20f + position_y, 0.9f,1.0f,0.0f	,0,0,0,  // bottom right
		 0.10f + position_x,  0.20f + position_y, 0.9f,0.0f,1.0f	,color.r,color.g+0.22f,color.b + 0.22f,  // top left 
		// second triangle
		 -0.10f + position_x, -0.20f + position_y, 0.9f,1.0f,0.0f	,0,0,0,  // bottom right
		 0.10f + position_x, -0.20f + position_y, 0.9f,0.0f,0.0f	,0,0,0,  // bottom left
		 0.10f + position_x,  0.20f + position_y, 0.9f,0.0f,1.0f	,color.r,color.g + 0.2f,color.b + 0.2f // top left
	};
	

	GlSet glset;
	glset.drawArrays(boxCoor,false);

	glUseProgram(uiPrograms.iconsProgram);
	iconBox(0.01f,0.02f,position_x + valueX, position_y + valueY,0.91f,icons.O,0,colorData.colorBoxIndicatorColor,colorData.colorBoxIndicatorColor);
	glUseProgram(uiPrograms.uiProgram);
	//box(0.0f, 0.01f, position_x + valueX, position_y + valueY, "", colorData.colorBoxIndicatorColor, 0.045f, false, false, 1.0f, 22,glm::vec4(0),0);
}

glm::vec3 hueValue;
glm::vec3 UserInterface::hueBar(float position_x, float position_y,float value,unsigned int FBO,GLFWwindow* window, glm::mat4 projection,bool updateHueVal) { //Changing colorBox value will be once the value changed
	std::vector<float> boxCoor{
		//Color - Normal Vectors Will Be Usen For Color Data Of Vertices

		//Red to pink
		 -0.01f + position_x,  0.03f + position_y + 0.15f, 0.9f ,1, (0.03f + 0.15f) *2.77777777778f + 0.5f  ,1,0,0,  // top right
		 -0.01f + position_x, -0.03f + position_y + 0.15f, 0.9f ,1, (-0.03f + 0.15f)*2.77777777778f + 0.5f  ,1,0,1,  // bottom right
		  0.01f + position_x,  0.03f + position_y + 0.15f, 0.9f ,0, (0.03f + 0.15f) *2.77777777778f + 0.5f  ,1,0,0,  // top left 
		 -0.01f + position_x, -0.03f + position_y + 0.15f, 0.9f ,1, (-0.03f + 0.15f)*2.77777777778f + 0.5f  ,1,0,1,  // bottom right
	 	  0.01f + position_x, -0.03f + position_y + 0.15f, 0.9f ,0, (-0.03f + 0.15f)*2.77777777778f + 0.5f  ,1,0,1,  // bottom left
		  0.01f + position_x,  0.03f + position_y + 0.15f, 0.9f ,0, (0.03f + 0.15f) *2.77777777778f + 0.5f  ,1,0,0,  // top left
		
		//Pink to blue
		 -0.01f + position_x,  0.03f + position_y + 0.09f, 0.9f ,1, (0.03f + 0.09f) *2.77777777778f + 0.5f  ,1,0,1,  // top right
		 -0.01f + position_x, -0.03f + position_y + 0.09f, 0.9f ,1, (-0.03f + 0.09f)*2.77777777778f + 0.5f  ,0,0,1,  // bottom right
		  0.01f + position_x,  0.03f + position_y + 0.09f, 0.9f ,0, (0.03f + 0.09f) *2.77777777778f + 0.5f  ,1,0,1,  // top left 
		 -0.01f + position_x, -0.03f + position_y + 0.09f, 0.9f ,1, (-0.03f + 0.09f)*2.77777777778f + 0.5f  ,0,0,1,  // bottom right
		  0.01f + position_x, -0.03f + position_y + 0.09f, 0.9f ,0, (-0.03f + 0.09f)*2.77777777778f + 0.5f  ,0,0,1,  // bottom left
		  0.01f + position_x,  0.03f + position_y + 0.09f, 0.9f ,0, (0.03f + 0.09f) *2.77777777778f + 0.5f  ,1,0,1,  // top left

		//blue to cyan			3				 					   	     
		 -0.01f + position_x,  0.03f + position_y + 0.03f, 0.9f ,1, (0.03f + 0.03f) *2.77777777778f + 0.5f  ,0,0,1,  // top right
		 -0.01f + position_x, -0.03f + position_y + 0.03f, 0.9f ,1, (-0.03f + 0.03f)*2.77777777778f + 0.5f  ,0,1,1,  // bottom right
		  0.01f + position_x,  0.03f + position_y + 0.03f, 0.9f ,0, (0.03f + 0.03f) *2.77777777778f + 0.5f  ,0,0,1,  // top left 
		 -0.01f + position_x, -0.03f + position_y + 0.03f, 0.9f ,1, (-0.03f + 0.03f)*2.77777777778f + 0.5f  ,0,1,1,  // bottom right
		  0.01f + position_x, -0.03f + position_y + 0.03f, 0.9f ,0, (-0.03f + 0.03f)*2.77777777778f + 0.5f  ,0,1,1,  // bottom left
		  0.01f + position_x,  0.03f + position_y + 0.03f, 0.9f ,0, (0.03f + 0.03f) *2.77777777778f + 0.5f  ,0,0,1,  // top left

		//cyan to green			3							 					   	     
		 -0.01f + position_x,  0.03f + position_y	- 0.03f	 , 0.9f ,1, (0.03f - 0.03f)  *2.77777777778f + 0.5f  ,0,1,1,  // top right
		 -0.01f + position_x, -0.03f + position_y	- 0.03f	 , 0.9f ,1, (-0.03f - 0.03f) *2.77777777778f + 0.5f  ,0,1,0,  // bottom right
		  0.01f + position_x,  0.03f + position_y	- 0.03f	 , 0.9f ,0, (0.03f - 0.03f)  *2.77777777778f + 0.5f  ,0,1,1,  // top left 
		 -0.01f + position_x, -0.03f + position_y	- 0.03f	 , 0.9f ,1, (-0.03f - 0.03f) *2.77777777778f + 0.5f  ,0,1,0,  // bottom right
		  0.01f + position_x, -0.03f + position_y	- 0.03f  , 0.9f ,0, (-0.03f - 0.03f) *2.77777777778f + 0.5f  ,0,1,0,  // bottom left
		  0.01f + position_x,  0.03f + position_y	- 0.03f	 , 0.9f ,0, (0.03f - 0.03f)  *2.77777777778f + 0.5f  ,0,1,1,  // top left

		//Green to yellow		3				  					 					   	     
		 -0.01f + position_x,  0.03f + position_y - 0.09f, 0.9f ,1, (0.03f - 0.09f) *2.77777777778f + 0.5f  ,0,1,0,  // top right
		 -0.01f + position_x, -0.03f + position_y - 0.09f, 0.9f ,1, (-0.03f - 0.09f)*2.77777777778f + 0.5f  ,1,1,0,  // bottom right
		  0.01f + position_x,  0.03f + position_y - 0.09f, 0.9f ,0, (0.03f - 0.09f) *2.77777777778f + 0.5f  ,0,1,0,  // top left 
		 -0.01f + position_x, -0.03f + position_y - 0.09f, 0.9f ,1, (-0.03f - 0.09f)*2.77777777778f + 0.5f  ,1,1,0,  // bottom right
		  0.01f + position_x, -0.03f + position_y - 0.09f, 0.9f ,0, (-0.03f - 0.09f)*2.77777777778f + 0.5f  ,1,1,0,  // bottom left
		  0.01f + position_x,  0.03f + position_y - 0.09f, 0.9f ,0, (0.03f - 0.09f) *2.77777777778f + 0.5f  ,0,1,0,  // top left

		//Yellow to red											 					   	     
		 -0.01f + position_x,  0.03f + position_y - 0.15f, 0.9f ,1, (0.03f - 0.15f) *2.77777777778f + 0.5f  ,1,1,0,  // top right
		 -0.01f + position_x, -0.03f + position_y - 0.15f, 0.9f ,1, (-0.03f - 0.15f)*2.77777777778f + 0.5f  ,1,0,0,  // bottom right
		  0.01f + position_x,  0.03f + position_y - 0.15f, 0.9f ,0, (0.03f - 0.15f) *2.77777777778f + 0.5f  ,1,1,0,  // top left 
		 -0.01f + position_x, -0.03f + position_y - 0.15f, 0.9f ,1, (-0.03f - 0.15f)*2.77777777778f + 0.5f  ,1,0,0,  // bottom right
		  0.01f + position_x, -0.03f + position_y - 0.15f, 0.9f ,0, (-0.03f - 0.15f)*2.77777777778f + 0.5f  ,1,0,0,  // bottom left
		  0.01f + position_x,  0.03f + position_y - 0.15f, 0.9f ,0, (0.03f - 0.15f) *2.77777777778f + 0.5f  ,1,1,0,  // top left
	};

	GlSet glset;
	ColorData colorData;
	HueShaderData hueShaderData;

	if(updateHueVal){

		glm::mat4 screenprojection = glm::ortho(0.0f, 1.77777777778f, 0.0f, 1.0f);
		hueShaderData.renderTextureProjection = screenprojection;
		hueShaderData.useTexCoords = 1;

		//Render color rectangle into the screen to get the value
		glset.useHueShader(uiPrograms.hueProgram,hueShaderData);

		glset.viewport(1920, 1081);
		glset.bindFramebuffer(FBO);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		GLubyte* hueBarPixel = new GLubyte[1 * 1 * 3];//Color value
		glset.drawArrays(boxCoor, false); //Render Model

		if( (value + 0.18f) * 2.77777777778f * 1080.0f != 1080.0f)
			glReadPixels(10, (value + 0.18f) * 2.77777777778f * 1080.0f, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, hueBarPixel);
		else{
			glReadPixels(10, 1079, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, hueBarPixel);
		}
		//Finish
		hueValue.r = hueBarPixel[0];
		hueValue.g = hueBarPixel[1];
		hueValue.b = hueBarPixel[2];

		delete(hueBarPixel);

		glset.bindFramebuffer(0);
		int screenSizeX;
		int screenSizeY;
		glfwGetFramebufferSize(window, &screenSizeX, &screenSizeY);
		glViewport(-(uiMaxScreenWidth - screenSizeX)/2, -(uiMaxScreenHeight - screenSizeY), uiMaxScreenWidth, uiMaxScreenHeight);
	}


	hueShaderData.useTexCoords = 0;
	hueShaderData.renderTextureProjection = projection;
	glset.useHueShader(uiPrograms.hueProgram,hueShaderData);

	//Finish
	glset.drawArrays(boxCoor, false); //Render color rectangle displayer
	glUseProgram(uiPrograms.uiProgram);
	box(0.01f, 0.005f, position_x, position_y + value, "", colorData.colorBoxIndicatorColor, 0.045f, false, false, 1.0f, 10000, glm::vec4(0), 0); //Value indicator


	return hueValue;
}
void UserInterface::rangeBar(float position_x, float position_y,float value) {
	ColorData colorData;
	GlSet gl;

	box(0, 0.015f, position_x+value, position_y, "", colorData.rangeBarFront, 0.035f, false, false, 0.9f, 15, glm::vec4(0), 0);//Value Square
	box((value+0.11f) / 2, 0.0075f, position_x+(value+0.11f) / 2 - 0.11f, position_y, "", colorData.rangeBarSlide, 0.035f, false, false, 0.9f, 15, glm::vec4(0), 0);//Range Rectangle
	box(0.11f, 0.0075f, position_x, position_y, "", colorData.rangeBarBack, 0.035f, false, false, 0.9f, 15, glm::vec4(0), 0);//Range Rectangle

}
void UserInterface::panelChangeButton(float position_x, float position_y) {
	std::vector<float> buttonCoor{
		// first triangle
		 0.00f + position_x,  0.02f + position_y, 0.0f,0,0,0,0,0,  // top right
		 0.00f + position_x, -0.05f + position_y, 0.0f,0,0,0,0,0,  // bottom right
		-0.025f + position_x,  0.02f + position_y, 0.0f,0,0,0,0,0,  // top left 
		// second triangle						     9
		 0.00f + position_x, -0.05f + position_y, 0.0f,0,0,0,0,0,  // bottom right
		-0.025f + position_x, -0.02f + position_y, 0.0f,0,0,0,0,0,  // bottom left
		-0.025f + position_x,  0.02f + position_y, 0.0f,0,0,0,0,0  // top left
	};
	GlSet glset;
	ColorData colorData;
	
	glset.uniform4fv(uiPrograms.uiProgram, "uiColor", colorData.panelHoldColor);
	glset.drawArrays(buttonCoor, false);

}
void UserInterface::decorationSquare(float position_x, float position_y) {
	std::vector<float> buttonCoor{
		// first triangle
		 0.045f + position_x,  0.02f + position_y, 0.9f,0,0,0,0,0,  // top right
		 0.02f + position_x, -0.05f + position_y, 0.9f,0,0,0,0,0,  // bottom right
		-0.02f + position_x,  0.02f + position_y, 0.9f,0,0,0,0,0,  // top left 
		// second triangle						     
		 0.02f + position_x, -0.05f + position_y, 0.9f,0,0,0,0,0,  // bottom right
		-0.02f + position_x, -0.05f + position_y, 0.9f,0,0,0,0,0,  // bottom left
		-0.02f + position_x,  0.02f + position_y, 0.9f,0,0,0,0,0  // top left
	};
	GlSet glset;
	ColorData colorData;
	
	glset.uniform4fv(uiPrograms.uiProgram, "uiColor", colorData.panelColorSnd);

	glset.drawArrays(buttonCoor, false);
}

void UserInterface::checkBox(float position_x, float position_y, std::string text, bool mouseHover,bool checked,unsigned int circleTxtr) {
	ColorData colorData;

	glm::vec4 color;

	if (!checked) {
		if (!mouseHover)
			color = colorData.checkBoxColor;
		else
			color =  colorData.checkBoxHoverColor;
	}
	else {
		color = colorData.checkBoxCheckedColor;
	}
	glUseProgram(uiPrograms.iconsProgram);
	iconBox(0.015,0.0273f,position_x, position_y,0.9f,circleTxtr,0,color,color);
	
	glUseProgram(uiPrograms.uiProgram);
	renderText(uiPrograms.uiProgram, text, position_x+0.02f, position_y - 0.01f, 0.00022f,colorData.textColor,0.99999f,false);
}


std::map<char, character> characters;
int textCursorPhaseCounter = 0;

void UserInterface::renderText(unsigned int program, std::string text, float x, float y, float scale,glm::vec4 color,float z,bool active) {
	GlSet glset;
	ColorData2 colorData2;
	
	const int maxCharCountSize = 100;
	if(active){
		textCursorPhaseCounter++;
		if(textCursorPhaseCounter == maxCharCountSize)
			textCursorPhaseCounter = 0;
	}

	glBindBuffer(GL_ARRAY_BUFFER,uiObjects.sqrVBO);
	glBindVertexArray(uiObjects.sqrVAO);

	glset.activeTexture(GL_TEXTURE2);

	glset.uniform1i(program,"isText", 1);
	glset.uniform1i(program, "isTextF", 1);
	glset.uniform4fv(program, "uiColor", color);
	
	std::string::const_iterator c;
	int counter = 0;
	for (c = text.begin(); c != text.end(); c++)
	{
		character ch = characters[*c];

		float xpos = x + ch.Bearing.x * scale;
		float ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

		float w = ch.Size.x * scale * 0.8f;
		float h = ch.Size.y * scale;

		glm::mat4 scalemat = glm::mat4(1);
		scalemat = glm::scale(scalemat,glm::vec3(w/1.7,h/1.7,1));
		glset.uniformMatrix4fv(uiPrograms.uiProgram,"scale",scalemat);
		
		glm::vec3 pos = glm::vec3(xpos + w/1.7,ypos + h/1.7,z);
		glset.uniform3fv(uiPrograms.uiProgram,"pos",pos);

		glset.bindTexture(ch.TextureID);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		

		if(active){
			if(counter == text.size()+uiTextBoxActiveChar-1 && textCursorPhaseCounter < maxCharCountSize/2){
				glset.uniform1i(program,"isText", 0);
				glset.uniform1i(program, "isTextF", 0);
				glset.uniform4fv(program, "uiColor", colorData2.textboxCursorColor);
				glset.uniform1f(uiPrograms.uiProgram, "uiTransitionMixVal", 0);
				
				scalemat = glm::mat4(1);
				scalemat = glm::scale(scalemat,glm::vec3(0.001f,0.02,1));
				glset.uniformMatrix4fv(uiPrograms.uiProgram,"scale",scalemat);
				pos = glm::vec3(xpos + w/1.7*2.,y+0.01,z);
				glset.uniform3fv(uiPrograms.uiProgram,"pos",pos);
				glDrawArrays(GL_TRIANGLES, 0, 6);
				
				glset.uniform1i(program, "isTextF", 1);
				glset.uniform1i(program,"isText", 1);
				glset.uniform4fv(program, "uiColor", color);
			}
		}

		x += (ch.Advance >> 6) * scale / 1.2f; 
		counter++;
	}
	glset.uniform1i(program, "isTextF", 0);
	glset.uniform1i(program, "isText", 0);

	glm::mat4 scalemat = glm::mat4(1);
	glm::vec3 pos = glm::vec3(0);
	glset.uniformMatrix4fv(uiPrograms.uiProgram,"scale",scalemat);
	glset.uniform3fv(uiPrograms.uiProgram,"pos",pos);

	glBindBuffer(GL_ARRAY_BUFFER,uiObjects.VBO);
	glBindVertexArray(uiObjects.VAO);
}




void UserInterface::container(float positionX,float positionY,float positionZ,float width, float height,glm::vec4 color,Programs &programs,unsigned int circleTexture){
	glUseProgram(programs.uiProgram);
	box(width,height,positionX,positionY,"", color,0,0,0,positionZ,10000,glm::vec4(0),0);
	
	box(0.03f,height,positionX - width + 0.006f,positionY,"", color,0,0,0,positionZ,10000,glm::vec4(0),0); //Left
	box(0.03f,height,positionX + width - 0.006f,positionY,"", color,0,0,0,positionZ,10000,glm::vec4(0),0); //Right
	box(width,0.06f,positionX,positionY + height - 0.012 ,"", color,0,0,0,positionZ,10000,glm::vec4(0),0); //Top
	box(width,0.06f,positionX,positionY - height + 0.012 ,"", color,0,0,0,positionZ,10000,glm::vec4(0),0); //Bottom

	glUseProgram(programs.iconsProgram);
	circle(positionX - width,positionY + height,positionZ,0.03f,0.06f,circleTexture,color);//Left top
	circle(positionX - width,positionY - height,positionZ,0.03f,0.06f,circleTexture,color);//Left bot
	circle(positionX + width,positionY + height,positionZ,0.03f,0.06f,circleTexture,color);//Right top
	circle(positionX + width,positionY - height,positionZ,0.03f,0.06f,circleTexture,color);//Right bot
}
void UserInterface::nodePanel(float mainPanelLoc,float sndPanel, float height,Programs programs,Icons icons,std::vector<NodeScene> nodeScenes,int selectedNodeScene){
	
	const float nodePanelLeft = sndPanel + 0.037f;
	const float nodePanelRight = mainPanelLoc - 0.037f;
	const float nodePanelTop = -1.00f + height;
	const float nodePanelZ = 0.01f;
	

	std::vector<float> boxCoor{
		//first triangle								    
		 nodePanelLeft 			,  -1.00f 			, nodePanelZ,1.0f,1.0f	,1,1,1,  // top right
		 nodePanelLeft			,  nodePanelTop 	, nodePanelZ,1.0f,0.0f	,0,0,0,  // bottom right
		 nodePanelRight			,  -1.00f 			, nodePanelZ,0.0f,1.0f	,0,0,0,  // top left 
		//second triangle
		 nodePanelLeft			,  nodePanelTop		, nodePanelZ,1.0f,0.0f	,0,0,0,  // bottom right
		 nodePanelRight			,  nodePanelTop		, nodePanelZ,0.0f,0.0f	,0,0,0,  // bottom left
		 nodePanelRight			,  -1.00f 			, nodePanelZ,0.0f,1.0f	,0,0,0 // top left
	};

	std::vector<float> topCoor{
		//first triangle								   
		 nodePanelLeft 			,  nodePanelTop+0.076f 			, nodePanelZ,1.0f,1.0f	,1,1,1,  // top right
		 nodePanelLeft			,  nodePanelTop 				, nodePanelZ,1.0f,0.0f	,0,0,0,  // bottom right
		 nodePanelRight			,  nodePanelTop+0.076f 			, nodePanelZ,0.0f,1.0f	,0,0,0,  // top left 
		//second triangle
		 nodePanelLeft			,  nodePanelTop					, nodePanelZ,1.0f,0.0f	,0,0,0,  // bottom right
		 nodePanelRight			,  nodePanelTop					, nodePanelZ,0.0f,0.0f	,0,0,0,  // bottom left
		 nodePanelRight			,  nodePanelTop+0.076f 			, nodePanelZ,0.0f,1.0f	,0,0,0 // top left
	};
	std::vector<float> leftCoor{
		//first triangle								   
		 nodePanelLeft 					,  -1.00f 			, nodePanelZ,1.0f,1.0f	,1,1,1,  // top right
		 nodePanelLeft					,  nodePanelTop 	, nodePanelZ,1.0f,0.0f	,0,0,0,  // bottom right
		 nodePanelLeft - 0.038f			,  -1.00f 			, nodePanelZ,0.0f,1.0f	,0,0,0,  // top left 
		//second triangle
		 nodePanelLeft					,  nodePanelTop		, nodePanelZ,1.0f,0.0f	,0,0,0,  // bottom right
		 nodePanelLeft - 0.038f			,  nodePanelTop		, nodePanelZ,0.0f,0.0f	,0,0,0,  // bottom left
		 nodePanelLeft - 0.038f			,  -1.00f 			, nodePanelZ,0.0f,1.0f	,0,0,0 // top left
	};

	std::vector<float> rightCoor{
		//first triangle								   
		 nodePanelRight 					,  -1.00f 			, nodePanelZ,1.0f,1.0f	,1,1,1,  // top right
		 nodePanelRight						,  nodePanelTop 	, nodePanelZ,1.0f,0.0f	,0,0,0,  // bottom right
		 nodePanelRight + 0.038f			,  -1.00f 			, nodePanelZ,0.0f,1.0f	,0,0,0,  // top left 
		//second triangle
		 nodePanelRight						,  nodePanelTop		, nodePanelZ,1.0f,0.0f	,0,0,0,  // bottom right
		 nodePanelRight + 0.038f			,  nodePanelTop		, nodePanelZ,0.0f,0.0f	,0,0,0,  // bottom left
		 nodePanelRight + 0.038f			,  -1.00f 			, nodePanelZ,0.0f,1.0f	,0,0,0 // top left
	};

	GlSet gl;
	ColorData colorData;


	glUseProgram(programs.uiProgram);
	gl.uniform4fv(uiPrograms.uiProgram,"uiColor",colorData.nodePanelColor);
	gl.drawArrays(boxCoor,false);
	gl.drawArrays(leftCoor,false);
	gl.drawArrays(rightCoor,false);
	
	gl.uniform4fv(uiPrograms.uiProgram,"uiColor",colorData.nodePanelColorSnd);
	gl.drawArrays(topCoor,false);

	if(nodeScenes.size())
		renderText(programs.uiProgram,nodeScenes[selectedNodeScene].sceneName,(nodePanelRight + nodePanelLeft)/2,nodePanelTop + 0.025f,0.00025f,colorData.textColor,0.7f,false);

	glUseProgram(programs.iconsProgram);
	
	if(nodeScenes.size())
		iconBox(0.015f,0.03f,(nodePanelRight + nodePanelLeft)/2 - 0.025,nodePanelTop+0.04f,0.7f,icons.Material,0,colorData.iconColor,colorData.iconColor);
	
	circle(nodePanelLeft+0.001f,nodePanelTop-0.001f,nodePanelZ,0.05f,0.1f,icons.Circle,colorData.nodePanelColorSnd); //Left
	circle(nodePanelRight-0.001f,nodePanelTop-0.001f,nodePanelZ,0.05f,0.1f,icons.Circle,colorData.nodePanelColorSnd); //Right
	
	glUseProgram(programs.uiProgram);
}

void UserInterface::nodePanelBarriers(float mainPanelLoc,float sndPanel, float height){
	
	const float nodePanelLeft = -1.0f;
	const float nodePanelRight = 1.0f;

	const float nodePanelLeftOrg = sndPanel;
	const float nodePanelRightOrg = mainPanelLoc ;

	const float nodePanelTop = -1.00f + height;
	const float nodePanelZ = 0.19f;
	

	std::vector<float> topCoor{
		//first triangle								   
		 nodePanelLeft 			,  1.f				, nodePanelZ,1.0f,1.0f	,1,1,1,  // top right
		 nodePanelLeft			,  nodePanelTop 	, nodePanelZ,1.0f,0.0f	,0,0,0,  // bottom right
		 nodePanelRight			,  1.f 				, nodePanelZ,0.0f,1.0f	,0,0,0,  // top left 
		//second triangle
		 nodePanelLeft			,  nodePanelTop		, nodePanelZ,1.0f,0.0f	,0,0,0,  // bottom right
		 nodePanelRight			,  nodePanelTop		, nodePanelZ,0.0f,0.0f	,0,0,0,  // bottom left
		 nodePanelRight			,  1.f 				, nodePanelZ,0.0f,1.0f	,0,0,0 // top left
	};
	std::vector<float> leftCoor{
		//first triangle								   
		 nodePanelLeftOrg 					,  -1.00f 			, nodePanelZ,1.0f,1.0f	,1,1,1,  // top right
		 nodePanelLeftOrg					,  nodePanelTop 	, nodePanelZ,1.0f,0.0f	,0,0,0,  // bottom right
		 nodePanelLeft						,  -1.00f 			, nodePanelZ,0.0f,1.0f	,0,0,0,  // top left 
		//second triangle
		 nodePanelLeftOrg					,  nodePanelTop		, nodePanelZ,1.0f,0.0f	,0,0,0,  // bottom right
		 nodePanelLeft 						,  nodePanelTop		, nodePanelZ,0.0f,0.0f	,0,0,0,  // bottom left
		 nodePanelLeft 						,  -1.00f 			, nodePanelZ,0.0f,1.0f	,0,0,0 // top left
	};
		std::vector<float> rightCoor{
		//first triangle								   
		 nodePanelRightOrg 					,  -1.00f 			, nodePanelZ,1.0f,1.0f	,1,1,1,  // top right
		 nodePanelRightOrg					,  nodePanelTop 	, nodePanelZ,1.0f,0.0f	,0,0,0,  // bottom right
		 nodePanelRight						,  -1.00f 			, nodePanelZ,0.0f,1.0f	,0,0,0,  // top left 
		//second triangle
		 nodePanelRightOrg					,  nodePanelTop		, nodePanelZ,1.0f,0.0f	,0,0,0,  // bottom right
		 nodePanelRight 						,  nodePanelTop		, nodePanelZ,0.0f,0.0f	,0,0,0,  // bottom left
		 nodePanelRight 						,  -1.00f 			, nodePanelZ,0.0f,1.0f	,0,0,0 // top left
	};


	GlSet gl;
	ColorData colorData;
	
	glUseProgram(uiPrograms.uiProgram);
	glm::vec4 none =  glm::vec4(0);
	gl.uniform4fv(uiPrograms.uiProgram,"uiColor",none);
	gl.drawArrays(topCoor,false);
	gl.drawArrays(leftCoor,false);
	gl.drawArrays(rightCoor,false);
}

void UserInterface::drawLine(float posX,float posY,float posZ,float toPosX,float toPosY,float width,glm::vec4 color,bool isLine){
	GlSet gl;
	if(abs(posY-toPosY)/4.f < 0.004f || abs(posX-toPosX)/2.f < 0.004f || isLine){
		glUseProgram(uiPrograms.uiProgram);
		gl.uniform4fv(uiPrograms.uiProgram,"uiColor",color);
		for (size_t i = 0; i < width; i++)
		{
			const float pixel = 10000.f/2.f;
			std::vector<float> lineCoor{
				//first triangle								   
				 posX + i/pixel	,	posY + i/pixel	,posZ	,1.0f,1.0f,1,1,1,  // top right
				 toPosX + i/pixel	,	toPosY + i/pixel	,posZ	,1.0f,1.0f,1,1,1,  // top right
			};
			gl.drawArrays(lineCoor,true);
			/* code */
		}
		for (size_t i = 0; i < width; i++)
		{
			const float pixel = 10000.f/2.f;
			std::vector<float> lineCoor{
				//first triangle								   
				 posX - i/pixel	,	posY + i/pixel	,posZ	,1.0f,1.0f,1,1,1,  // top right
				 toPosX - i/pixel	,	toPosY + i/pixel	,posZ	,1.0f,1.0f,1,1,1,  // top right
			};
			gl.drawArrays(lineCoor,true);
			/* code */
		}
		for (size_t i = 0; i < width; i++)
		{
			const float pixel = 10000.f/2.f;
			std::vector<float> lineCoor{
				//first triangle								   
				 posX + i/pixel	,	posY - i/pixel	,posZ	,1.0f,1.0f,1,1,1,  // top right
				 toPosX + i/pixel	,	toPosY - i/pixel	,posZ	,1.0f,1.0f,1,1,1,  // top right
			};
			gl.drawArrays(lineCoor,true);
			/* code */
		}
		for (size_t i = 0; i < width; i++)
		{
			const float pixel = 10000.f/2.f;
			std::vector<float> lineCoor{
				//first triangle								   
				 posX - i/pixel	,	posY - i/pixel	,posZ	,1.0f,1.0f,1,1,1,  // top right
				 toPosX - i/pixel	,	toPosY - i/pixel	,posZ	,1.0f,1.0f,1,1,1,  // top right
			};
			gl.drawArrays(lineCoor,true);
			/* code */
		}
	}
	else{
		glUseProgram(uiPrograms.curveProgram);
		gl.uniform4fv(uiPrograms.curveProgram,"lineColor",color);
		

			std::vector<float> box = { 
			// first triangle
			 toPosX,  posY, posZ,1,1,0,0,0,  // top right
			 toPosX,  toPosY, posZ,1,0,0,0,0,  // bottom right
			 posX,  posY, posZ,0,1,0,0,0,  // top left 
			// second triangle
			 toPosX,  toPosY, posZ,1,0,0,0,0,  // bottom right
			 posX,  toPosY, posZ,0,0,0,0,0,  // bottom left
			 posX,  posY, posZ,0,1,0,0,0   // top left
		};
		posX += 1.f;
		toPosX += 1.f;
		posY += 1.f;
		toPosY += 1.f;
		gl.uniform1f(uiPrograms.curveProgram,"w",abs(posX-toPosX)/2.f);
		gl.uniform1f(uiPrograms.curveProgram,"h",abs(posY-toPosY)/4.f);
		gl.drawArrays(box,0);
	}
}



void UserInterface::renderMenubar(GLFWwindow* window) {
	/*ColorData colorD;
	box(0.04f, 0.02f, -0.92f, 0.98f, "Layers", colorD.menuBarColor, 0.034f, false, false);
	box(0.04f, 0.02f, -0.82f, 0.98f, "Nodes", colorD.menuBarColor, 0.034f, false, false);
	box(0.08f, 0.02f, -0.68f, 0.98f, "Texture Paint", colorD.menuBarColor, 0.06f, false, false);
	box(0.08f, 0.02f, -0.50f, 0.98f, "Load Model", colorD.menuBarColor, 0.06f, false, false);
	box(0.04f, 0.02f, -0.36f, 0.98f, "Export", colorD.menuBarColor, 0.034f, false, false);
	box(0.08f, 0.02f, -0.22f, 0.98f, "Load Project", colorD.menuBarColor, 0.06f, false, false);
	box(0.06f, 0.02f, -0.06f, 0.98f, "Settings", colorD.menuBarColor, 0.04f, false, false);

	box(1.0f, 0.08f, 0.0f, 1.03f, "", colorD.menuBarColor, 0.00f, false, false);*/
}

void Load::sendCharsToUI(std::map<char, character> theseCharacters){
	characters = theseCharacters;
}
void UserInterface::sendProgramsToUserInterface(Programs appuiPrograms){
	uiPrograms = appuiPrograms;
}
void UserInterface::sendMaxWindowSize(int maxScreenWidth,int maxScreenHeight){
	uiMaxScreenHeight = maxScreenHeight;
	uiMaxScreenWidth = maxScreenWidth;
	sendMaxWindowSizeToCalculationsAndMore(maxScreenWidth,maxScreenHeight);
}

void UserInterface::coloringPanel(ColoringPanel &coloringPanel,Programs programs,Icons icons,GLFWwindow* window,SaturationValShaderData saturationValShaderData,glm::mat4 orthoProjection,double mouseXpos,double mouseYpos,bool &firstClick,float xOffset,float yOffset,unsigned int FBOscreen,ColorPicker &colorPicker,float screenGapX,glm::vec3 screenHoverPixel){
	const float depth = 0.8f;

	const float panelWidth = 0.2f;
	const float panelHeigth = 0.2f;
	
	ColorData colorData;
	GlSet glset;
	Utilities util;


	container(coloringPanel.panelPosX,coloringPanel.panelPosY,depth,panelWidth,panelHeigth,colorData.panelColor,programs,icons.Circle);
	
	coloringPanel.panelHover = isMouseOnButton(window,panelWidth+0.03f,panelHeigth+0.06f,coloringPanel.panelPosX - screenGapX,coloringPanel.panelPosY,mouseXpos,mouseYpos,false);
	
	if(coloringPanel.panelHover && !coloringPanel.dropperActive){
		coloringPanel.enteredOnce = true;
	}
	if(coloringPanel.hueBarPointerPressed || coloringPanel.saturationValueBoxPointerPressed){
		coloringPanel.enteredOnce = false;
	}
	if(!coloringPanel.panelHover && coloringPanel.enteredOnce && !coloringPanel.dropperActive && !coloringPanel.hueBarPointerPressed && !coloringPanel.saturationValueBoxPointerPressed){
		coloringPanel.active = false;
		coloringPanel.enteredOnce = false;
		coloringPanel.saturationValueBoxPointerPressed = false;
		coloringPanel.hueBarPointerPressed = false;
	}


	glUseProgram(programs.uiProgram); 

	//Color Picker
	glm::vec3 hueResult = hueBar(coloringPanel.panelPosX + 0.02f, coloringPanel.panelPosY, coloringPanel.hueBarPosX, FBOscreen,window,orthoProjection,true); 
	coloringPanel.hueBarHover = isMouseOnButton(window,0.01,0.2f,coloringPanel.panelPosX + 0.02f - screenGapX, coloringPanel.panelPosY,mouseXpos,mouseYpos,false);
	
	saturationValShaderData.boxColor = hueResult/255.f;
	saturationValShaderData.renderTextureProjection = orthoProjection;

	glset.useSaturationValBoxShader(programs.saturationValBoxProgram,saturationValShaderData);
	colorBox(coloringPanel.panelPosX - 0.1f, coloringPanel.panelPosY, coloringPanel.saturationValueBoxPosX, coloringPanel.saturationValueBoxPosY,icons);

	coloringPanel.colorBoxHover = isMouseOnButton(window,0.1f,0.2f,coloringPanel.panelPosX - 0.1f - screenGapX,coloringPanel.panelPosY,mouseXpos,mouseYpos,false);


	int screenSizeX;
	int screenSizeY;
	glfwGetFramebufferSize(window,&screenSizeX,&screenSizeY);

	Render render;
	ColorPicker cp;
	cp.hueColorValue = hueResult;
	cp.saturationValuePosX = coloringPanel.saturationValueBoxPosX;
	cp.saturationValuePosY = coloringPanel.saturationValueBoxPosY;
	coloringPanel.result = render.getColorPickerValue(FBOscreen,cp,screenSizeX,screenSizeY,programs,uiMaxScreenWidth,uiMaxScreenHeight,saturationValShaderData);
	
	box(0.04f, 0.03f, coloringPanel.panelPosX + 0.125f,coloringPanel.panelPosY+0.15f, coloringPanel.hexVal, colorData.textBoxColor, 0, true, false, 0.9f, 10, colorData.textBoxColorClicked, (float)coloringPanel.hexValTextboxActive);//Hex val textbox
	
	coloringPanel.hexValTextboxHover = isMouseOnButton(window,0.05f,0.03f, coloringPanel.panelPosX + 0.125f - screenGapX, coloringPanel.panelPosY + 0.15f,mouseXpos,mouseYpos,false);

	if(coloringPanel.hexValTextboxHover && firstClick){
		coloringPanel.hexValTextboxActive = true;
	}

	coloringPanel.saturationValueBoxPointerHover = isMouseOnButton(window,0.02f,0.04f,coloringPanel.panelPosX - 0.1f + coloringPanel.saturationValueBoxPosX - screenGapX ,coloringPanel.panelPosY+coloringPanel.saturationValueBoxPosY,mouseXpos,mouseYpos,false);
	coloringPanel.hueBarPointerHover = isMouseOnButton(window,0.02f,0.02f,coloringPanel.panelPosX + 0.02f - screenGapX,coloringPanel.panelPosY + coloringPanel.hueBarPosX,mouseXpos,mouseYpos,false);

	if(coloringPanel.newHexValTextboxEntry){
		LigidPainter lp;
		lp.updateColorPicker(util.hexToRGBConverter(coloringPanel.hexVal),true,true,coloringPanel.hueBarPosX,coloringPanel.saturationValueBoxPosX,coloringPanel.saturationValueBoxPosY,false);
		coloringPanel.newHexValTextboxEntry = false;
	}
	if(coloringPanel.colorBoxHover && firstClick && !coloringPanel.saturationValueBoxPointerHover){
		LigidPainter lp;
		lp.updateColorPicker(screenHoverPixel,false,true,coloringPanel.hueBarPosX,coloringPanel.saturationValueBoxPosX,coloringPanel.saturationValueBoxPosY,false);
	}
	if(coloringPanel.hueBarHover && firstClick && !coloringPanel.hueBarPointerHover){
		LigidPainter lp;
		lp.updateColorPicker(screenHoverPixel,true,false,coloringPanel.hueBarPosX,coloringPanel.saturationValueBoxPosX,coloringPanel.saturationValueBoxPosY,false);
	}
	if(coloringPanel.pickerValueChanged){
		coloringPanel.hexVal = util.rgbToHexGenerator(coloringPanel.result);
		coloringPanel.pickerValueChanged = false;
	}



	glUseProgram(programs.iconsProgram);
	iconBox(0.02*1.2,0.0364f*1.2,coloringPanel.panelPosX + 0.125f,coloringPanel.panelPosY+0.075f,0.9f,icons.Circle,0,colorData.panelColorSnd,glm::vec4(0));
	iconBox(0.02,0.0364f,coloringPanel.panelPosX + 0.125f,coloringPanel.panelPosY+0.075f,0.91f,icons.Circle,0,glm::vec4(coloringPanel.result/255.f,1),glm::vec4(0));
	

	iconBox(0.023f,0.0325f,coloringPanel.panelPosX + 0.18f, coloringPanel.panelPosY-0.18,0.9f,icons.dropperIcon,0,colorData.iconColor,colorData.iconColorHover);
	coloringPanel.dropperHover = isMouseOnButton(window,0.02f,0.04f,coloringPanel.panelPosX + 0.18f - screenGapX,coloringPanel.panelPosY-0.18,mouseXpos,mouseYpos,false);

	if(coloringPanel.dropperHover && firstClick){
		coloringPanel.dropperActive = true;
		coloringPanel.enteredOnce = false;
		colorPicker.dropperActive = true;
	}
	

	if(coloringPanel.saturationValueBoxPointerHover && firstClick)
		coloringPanel.saturationValueBoxPointerPressed = true;
		
	if(glfwGetMouseButton(window,0) == GLFW_RELEASE){
		coloringPanel.saturationValueBoxPointerPressed = false;
		coloringPanel.hueBarPointerPressed = false;
	}
	if(coloringPanel.saturationValueBoxPointerPressed){
		coloringPanel.saturationValueBoxPosX += xOffset/uiMaxScreenWidth*2;
		coloringPanel.saturationValueBoxPosX = util.restrictBetween(coloringPanel.saturationValueBoxPosX, 0.099f, -0.1f);//Keep in boundaries
		coloringPanel.saturationValueBoxPosY -= yOffset/uiMaxScreenHeight*2;
		coloringPanel.saturationValueBoxPosY = util.restrictBetween(coloringPanel.saturationValueBoxPosY, 0.199f, -0.2f);//Keep in boundaries
		coloringPanel.pickerValueChanged = true;
	}


	if(coloringPanel.hueBarPointerHover && firstClick)
		coloringPanel.hueBarPointerPressed = true;

	if(coloringPanel.hueBarPointerPressed){
		coloringPanel.hueBarPosX -= yOffset/(uiMaxScreenHeight/2);
		coloringPanel.hueBarPosX = util.restrictBetween(coloringPanel.hueBarPosX, 0.180f, -0.180f);//Keep in boundaries
		coloringPanel.pickerValueChanged = true;
	}

	glUseProgram(programs.uiProgram); 
}
void UserInterface::textureCreatingPanel(TextureCreatingPanel &txtrCreatingPanel,Icons icons,Programs programs,GLFWwindow* window,double mouseXpos,double mouseYpos,bool &firstClick,ColoringPanel &coloringPanel,float screenGapX,std::vector<aTexture> &albedoTextures,int& activeFolderIndex){
	ColorData colorData;
	Utilities util;
	GlSet glset;

	const float depth = 0.51f;

	const float panelWidth = 0.15f;
	const float panelHeigth = 0.12f;

	container(txtrCreatingPanel.panelPosX,txtrCreatingPanel.panelPosY,depth,panelWidth,panelHeigth,colorData.panelColor,programs,icons.Circle);
	txtrCreatingPanel.panelHover = isMouseOnButton(window,panelWidth+0.03f,panelHeigth+0.06f,txtrCreatingPanel.panelPosX-screenGapX,txtrCreatingPanel.panelPosY,mouseXpos,mouseYpos,false);
	
	if(!txtrCreatingPanel.panelHover && !coloringPanel.panelHover && !coloringPanel.active){ 
		txtrCreatingPanel.color = glm::vec3(0);
		txtrCreatingPanel.active = false;
		txtrCreatingPanel.textBoxVal = "txtr";
	}

	glUseProgram(programs.uiProgram);
	
	renderText(programs.uiProgram,"Title :",txtrCreatingPanel.panelPosX - panelWidth,txtrCreatingPanel.panelPosY+0.12f,0.00022f,colorData.textColor,depth+0.01,false);
	box(panelWidth - 0.02f, 0.03f, txtrCreatingPanel.panelPosX,txtrCreatingPanel.panelPosY+0.08f, txtrCreatingPanel.textBoxVal, colorData.textBoxColor, 0, true, false, depth+0.01f, 10, colorData.textBoxColorClicked, (float)txtrCreatingPanel.textBoxActive);
	txtrCreatingPanel.textBoxHover = isMouseOnButton(window,panelWidth - 0.02f, 0.03f,txtrCreatingPanel.panelPosX -screenGapX,txtrCreatingPanel.panelPosY+0.08f,mouseXpos,mouseYpos,false);

	if(txtrCreatingPanel.textBoxHover && firstClick){
		txtrCreatingPanel.textBoxActive = true;
	}

	renderText(programs.uiProgram,"Color :",txtrCreatingPanel.panelPosX - panelWidth,txtrCreatingPanel.panelPosY+0.02f,0.00022f,colorData.textColor,depth+0.01,false);
	box(panelWidth - 0.02f, 0.03f, txtrCreatingPanel.panelPosX,txtrCreatingPanel.panelPosY-0.02f, "", glm::vec4(txtrCreatingPanel.color/255.f,1), 0, true, false, depth+0.01f, 10, glm::vec4(0), 0);
	txtrCreatingPanel.colorBarHover = isMouseOnButton(window,panelWidth - 0.02f, 0.03f,txtrCreatingPanel.panelPosX-screenGapX,txtrCreatingPanel.panelPosY-0.02f,mouseXpos,mouseYpos,false);

	if(txtrCreatingPanel.colorBarHover && firstClick){
		txtrCreatingPanel.coloringPanelActive = true;
		
		coloringPanel.hexVal = util.rgbToHexGenerator(txtrCreatingPanel.color);
		coloringPanel.newHexValTextboxEntry = true;
		coloringPanel.result = txtrCreatingPanel.color;
		coloringPanel.panelPosX = txtrCreatingPanel.panelPosX + panelWidth + 0.2f;
		coloringPanel.panelPosY = txtrCreatingPanel.panelPosY;
		coloringPanel.active = true;
	}
	
	if(!coloringPanel.active)
		txtrCreatingPanel.coloringPanelActive = false;

	if(txtrCreatingPanel.coloringPanelActive){
		txtrCreatingPanel.color = coloringPanel.result;
	}
	
	box(0.04f, 0.03f, txtrCreatingPanel.panelPosX-0.09,txtrCreatingPanel.panelPosY-0.1f, "Create", colorData.buttonColor, 0.03f, false, false, depth+0.01f, 10, colorData.buttonColorHover, txtrCreatingPanel.createButtonHover);
	txtrCreatingPanel.createButtonHover = isMouseOnButton(window,0.04f, 0.03f, txtrCreatingPanel.panelPosX-0.09-screenGapX,txtrCreatingPanel.panelPosY-0.1f,mouseXpos,mouseYpos,false);

	if(txtrCreatingPanel.createButtonHover && firstClick){
		if(txtrCreatingPanel.textBoxVal != ""){
			aTexture txtr;
			txtr.folderIndex = activeFolderIndex;
			glActiveTexture(GL_TEXTURE28);

			unsigned int texture;
			glset.genTextures(texture);
			glset.bindTexture(texture);
			glset.texImage(nullptr,1080,1080,GL_RGB);
			glset.generateMipmap();

			unsigned int FBO;
			glset.genFramebuffers(FBO);
			glset.bindFramebuffer(FBO);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,  texture, 0);

			glClearColor(txtrCreatingPanel.color.r/255.f,txtrCreatingPanel.color.g/255.f,txtrCreatingPanel.color.b/255.f,1);
			glClear(GL_COLOR_BUFFER_BIT);

			glset.bindFramebuffer(0);

			glset.generateMipmap();

			glDeleteFramebuffers(1,&FBO);

			std::vector<std::string> textureNames;
			for (size_t i = 0; i < albedoTextures.size(); i++)
			{
				textureNames.push_back(albedoTextures[i].name);
			}
			
			//Rename if necessary
			txtrCreatingPanel.textBoxVal = util.uniqueName(txtrCreatingPanel.textBoxVal,textureNames);

			txtr.id = texture;
			txtr.name = txtrCreatingPanel.textBoxVal;

			albedoTextures.push_back(txtr);

			txtrCreatingPanel.textBoxVal = "txtr";
			txtrCreatingPanel.color = glm::vec3(0);
			txtrCreatingPanel.active = false;
		}
		else{
			alert("Warning! Title can not be empty.",200);
		}
	}
}
int uiLastTextBoxActiveChar = 0;
void UserInterface::sendTextBoxActiveCharToUI(int textBoxActiveChar){
	if(uiLastTextBoxActiveChar != uiTextBoxActiveChar){
		textCursorPhaseCounter = 0;
	}
	uiLastTextBoxActiveChar = uiTextBoxActiveChar;
	uiTextBoxActiveChar = textBoxActiveChar;
}

void UserInterface::modelMaterialPanel(Model &model,Programs programs,RenderData renderData,float screenGapX,float materialsPanelSlideValue,double mouseXpos,double mouseYpos,bool &texturePanelButtonHover,RenderOutData& uiOut,int& currentMaterialIndex,bool &firstClick,bool& newModelAdded, float texturePanelButtonMixVal,int &selectedNodeScene,Icons icons,std::vector<NodeScene> nodeScenes){
	ColorData colorData;

	bool mouseEnteredOnce = false;
		glUseProgram(programs.uiProgram); 

		if(model.meshes.size() == 0){
			renderText(programs.uiProgram, "Materials of the 3D model", renderData.panelLoc - 1.0f - screenGapX  + 0.095f, 0.8f, 0.00022f,colorData.textColor,0.99999f,false);
			renderText(programs.uiProgram, "will be show up there", renderData.panelLoc - 1.0f - screenGapX  + 0.115f, 0.75f, 0.00022f,colorData.textColor,0.99999f,false);
		}
		for (int i = 0; i < model.meshes.size(); i++)//Render buttons
		{ 	
			//Check if mouse is entered the related button
			bool textureButtonEnter = isMouseOnButton(renderData.window, 0.2f, 0.06f, renderData.panelLoc - 1.0f - screenGapX*2 + 0.205f,0.8f - (i * 0.125f) + materialsPanelSlideValue, mouseXpos, mouseYpos,false);
			bool textureAddButtonEnter = isMouseOnButton(renderData.window, 0.02f, 0.03f, renderData.panelLoc - 1.0f - screenGapX*2 + 0.3f,0.8f - (i * 0.125f) + materialsPanelSlideValue, mouseXpos, mouseYpos,false);
			
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
				box(0.2f, 0.06f, renderData.panelLoc - 1.0f - screenGapX + 0.205f, 0.8f - (i * 0.125f) + materialsPanelSlideValue, model.meshes[i].materialName, currentColor, 0.2f, false, false, 0.9f, 10000, currentColorHover, texturePanelButtonMixVal);

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
				box(0.2f, 0.06f, renderData.panelLoc - 1.0f - screenGapX + 0.205f, 0.8f - (i * 0.125f) + materialsPanelSlideValue, model.meshes[i].materialName, currentColor, 0.2f, false, false, 0.9f, 10000, currentColorHover, 0);
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
			//box(0.03f, 0.06f, renderData.panelLoc - screenGapX + 0.3f, 0.8f - (i * 0.125f), "", colorData.buttonColorHover, 0.048f, true, false, 0.6f, 10000, colorData.buttonColorHover, 0); 
			if(model.meshes[i].materialIndex != 10000)
				renderText(programs.uiProgram, std::to_string(nodeScenes[model.meshes[i].materialIndex].index),renderData.panelLoc - 1.0f - screenGapX + 0.235f,0.8f - (i * 0.125f) + materialsPanelSlideValue - 0.02,0.00022,colorData.materialIconIndexTextColor,0.99999f,false);
			
			glUseProgram(programs.iconsProgram);
			if(model.meshes[i].materialIndex != 10000)
				iconBox(0.02f,0.04f,renderData.panelLoc - 1.0f - screenGapX + 0.255f ,0.8f - (i * 0.125f) + materialsPanelSlideValue,0.99f,icons.Material,0,colorData.iconColor,colorData.iconColor);
			float iconmixVal = 0.0f;
			if(textureAddButtonEnter)
				iconmixVal = 0.5f;
				
			iconBox(0.015f,0.027f,renderData.panelLoc - 1.0f - screenGapX + 0.3f ,0.8f - (i * 0.125f) + materialsPanelSlideValue,0.99f,icons.Plus,iconmixVal,colorData.iconColor,colorData.iconColorHover);
			glUseProgram(programs.uiProgram); 
		}
		if(!mouseEnteredOnce){
			texturePanelButtonHover = false;
		}
}

void UserInterface::brushMaskTexturePanel(Programs programs,std::vector<unsigned int> &maskTextures,float centerCoords, float screenGapX,float &maskPanelSliderValue,unsigned int &currentBrushMaskTexture,bool &firstClick,GLFWwindow* window,double mouseXpos,double mouseYpos,unsigned int FBOScreen,PanelData panelData,int screenSizeX,int screenSizeY,RenderOutData& uiOut,std::vector<UIElement> &UIElements,float brushBlurVal, OutShaderData outShaderData,float posY){
	ColorData colorData;
	GlSet gl;
	Texture txtr;

	glUseProgram(programs.iconsProgram); 

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
			float position_y = posY+ maskYpos - maskPanelSliderValue*(maskTextures.size()/4) - 0.05f;

			const float panelRange = 0.25f;

			//ui.iconBox(0.025f, 0.05f,centerCoords - screenGapX - maskXpos - 0.2f,0.8f + maskYpos - maskPanelSliderValue*(maskTextures.size()/4) - 0.05f,1,maskTextures[i],0);
			float upBotDifMin = std::min(0.05f + position_y,posY) - std::min(-0.05f + position_y,posY);
			float upBotDifMax = std::max(0.05f + position_y,posY-panelRange) - std::max(-0.05f + position_y,posY-panelRange);
			std::vector<float> buttonCoorSq{
				// first trianglev 
				 0.03f + position_x,  std::min(std::max(0.06f + position_y,posY-panelRange),posY), 1,1,upBotDifMin*10,0,0,0,  // top right
				 0.03f + position_x,  std::min(std::max(-0.06f + position_y,posY-panelRange),posY), 1,1,1.0f-upBotDifMax*10,0,0,0,  // bottom right
				-0.03f + position_x,  std::min(std::max(0.06f + position_y,posY-panelRange),posY), 1,0,upBotDifMin*10,0,0,0,  // top left 
				// second triangle						   
				 0.03f + position_x,  std::min(std::max(-0.06f + position_y,posY-panelRange),posY), 1,1,1.0f-upBotDifMax*10,0,0,0,  // bottom right
				-0.03f + position_x,  std::min(std::max(-0.06f + position_y,posY-panelRange),posY), 1,0,1.0f-upBotDifMax*10,0,0,0,  // bottom left
				-0.03f + position_x,  std::min(std::max(0.06f + position_y,posY-panelRange),posY), 1,0,upBotDifMin*10,0,0,0  // top left
			};

			if(std::min(std::max(0.06f + position_y,posY-panelRange),posY) != std::min(std::max(-0.06f + position_y,posY-panelRange),posY)){//Prevent rendering all the textures
				gl.uniform1i(programs.iconsProgram,"isMaskIcon",1);
				if(maskTextures[i] == currentBrushMaskTexture){
					glm::vec4 chosenBrushMaskTextureColor = glm::vec4(colorData.chosenBrushMaskTextureColor,1.0f);
					gl.uniform4fv(programs.iconsProgram,"iconColor",chosenBrushMaskTextureColor);
				}
				else{
					gl.uniform4fv(programs.iconsProgram,"iconColor",colorData.brushMaskIconColor);
				}
				gl.uniform1f(programs.iconsProgram,"iconMixVal",0);
				gl.activeTexture(GL_TEXTURE6);
				gl.bindTexture(maskTextures[i]);
				gl.drawArrays(buttonCoorSq,false);
				gl.uniform1i(programs.iconsProgram,"isMaskIcon",0);
			}

			
			if(isMouseOnCoords(window,mouseXpos+screenGapX*(uiMaxScreenWidth/2),mouseYpos,buttonCoorSq,panelData.movePanel)){
				if(firstClick){
					gl.activeTexture(GL_TEXTURE1);
					gl.bindTexture(maskTextures[i]);
					txtr.updateMaskTexture(FBOScreen,screenSizeX,screenSizeY,UIElements[UIbrushRotationRangeBar].rangeBar.value,false,UIElements[UIbrushBordersRangeBar].rangeBar.value,brushBlurVal,outShaderData,programs,uiMaxScreenWidth,uiMaxScreenHeight);
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
}

bool UserInterface::listBox(float posX,float posY,float posZ,const char* title,float width,Icons icons,std::vector<const char*> list, bool active,GLFWwindow* window, float mouseXpos,float mouseYpos,bool &firstClick,int &chosenIndex,float screenGapX){
	ColorData colorData;
	bool stateChanged = false;
	if(active){
		const float contHeight = list.size()/27.5;
		container(posX,posY-contHeight,posZ,width,contHeight,glm::vec4(0.2),uiPrograms,icons.Circle);
		for (size_t i = 1; i <= list.size(); i++)
		{
			glUseProgram(uiPrograms.uiProgram);
			bool hover = isMouseOnButton(window,width, 0.03f,posX-screenGapX,posY - i/15.f,mouseXpos,mouseYpos,false);
			bool pressed = false;
			
			if(i-1 == chosenIndex){
				pressed = true;
			}

			if(hover && firstClick){
				chosenIndex = i-1;
				stateChanged = true;
			}

			glm::vec4 color;
			if(hover){
				if(pressed){
					color = glm::vec4(colorData.LigidPainterThemeColor,0.2f);
				}
				else{
					color = glm::vec4(0.2,0.2,0.2,0.2);
				}
			}
			else{
				if(pressed){
					color = glm::vec4(colorData.LigidPainterThemeColor,0.5f);
				}
				else{
					color = glm::vec4(0.2,0.2,0.2,0.2f);
				}
			}

			box(width+0.022f, 0.03f, posX,posY - i/15.f, list[i-1], color, width, false, false, posZ+(float)(hover||pressed)/1000.f, 10000, color, 0);
		}
		
	}
	glUseProgram(uiPrograms.uiProgram);
	
	box(width, 0.04f, posX,posY, title, glm::vec4(0.1,0.1,0.1,0.2), width, false, false, posZ+0.0001f, 10000, glm::vec4(0.1,0.1,0.1,0.2), 0);
	
	glUseProgram(uiPrograms.iconsProgram);
	circle(posX-width,posY,posZ+0.0001f,0.025,0.05,icons.Circle,glm::vec4(0.1,0.1,0.1,0.2));
	circle(posX+width,posY,posZ+0.0001f,0.025,0.05,icons.Circle,glm::vec4(0.1,0.1,0.1,0.2));
	
	glUseProgram(uiPrograms.uiProgram);

	return stateChanged;
}

std::vector<float> UserInterface::selectionBox(bool active,float sPX,float sPY,float dPX,float dPY,float depth){
	std::vector<float> box = { 
		// first triangle
		 dPX,  sPY, depth,1,1,0,0,0,  // top right
		 dPX,  dPY, depth,1,0,0,0,0,  // bottom right
		 sPX,  sPY, depth,0,1,0,0,0,  // top left 
		// second triangle
		 dPX,  dPY, depth,1,0,0,0,0,  // bottom right
		 sPX,  dPY, depth,0,0,0,0,0,  // bottom left
		 sPX,  sPY, depth,0,1,0,0,0   // top left
	};
	GlSet gls;
	ColorData2 clrData2;
	glUseProgram(uiPrograms.uiProgram);
	gls.uniform4fv(uiPrograms.uiProgram,"uiColor",clrData2.selectionBoxColor);

	if(active)
		gls.drawArrays(box,false);

	return box;
}

void UserInterface::sendObjectsToUI(Objects aobjects,unsigned int acircleIcon){
	uiObjects = aobjects;
	circleIcon = acircleIcon;
}