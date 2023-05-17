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


int uiTextBoxActiveChar;

unsigned int circleIcon;
unsigned int smoothSquareIcon;

void UserInterface::box(float width, float height, float position_x, float position_y,std::string text,glm::vec4 color, float textRatio,bool isTextBox,bool isMaskImageBox,float z,float buttonCurveReduce, glm::vec4 colorTransitionColor, float mixVal) {
	
	//buttonCurveReduce = 10 | normal  
	//buttonCurveReduce = >10 | smaller
	//buttonCurveReduce = <10 | bigger
	//buttonCurveReduce = 10000 to get a square

	if(width == height*2){
		float ratio = glfwGetVideoMode(glfwGetPrimaryMonitor())->width / glfwGetVideoMode(glfwGetPrimaryMonitor())->height;
		height = width*ratio;
	}

	//parameter z = position_z
	GlSet glset;

	glm::mat4 scale = glm::mat4(1);
	scale = glm::scale(scale,glm::vec3(width,height,1));
	glset.uniformMatrix4fv(uiPrograms.uiProgram,"scale",scale);
	
	glset.uniform1f(uiPrograms.uiProgram,"width",width);
	glset.uniform1f(uiPrograms.uiProgram,"height",height);
	glset.uniform1f(uiPrograms.uiProgram,"radius",buttonCurveReduce);
	
	glm::vec3 pos = glm::vec3(position_x,position_y,z);
	glset.uniform3fv(uiPrograms.uiProgram,"pos",pos);

	glset.uniform1i(uiPrograms.uiProgram,"outline",0);
	glset.uniform1i(uiPrograms.uiProgram,"isUiTextureUsed",isMaskImageBox);
	glset.uniform4fv(uiPrograms.uiProgram,"uiColor",color);
	glset.uniform4fv(uiPrograms.uiProgram, "uiTransitionColor", colorTransitionColor);
	glset.uniform1f(uiPrograms.uiProgram, "uiTransitionMixVal", mixVal);

	glBindBuffer(GL_ARRAY_BUFFER,uiObjects.sqrVBO);
	glBindVertexArray(uiObjects.sqrVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	ColorData colorData;



	scale = glm::mat4(1);
	pos = glm::vec3(0);
	glset.uniformMatrix4fv(uiPrograms.uiProgram,"scale",scale);
	glset.uniform3fv(uiPrograms.uiProgram,"pos",pos);

	glset.uniform1i(uiPrograms.uiProgram, "isUiTextureUsed", 0);

	if(buttonCurveReduce <= 200){
		// glUseProgram(uiPrograms.iconsProgram);
		// unsigned int icon;
		// if(isTextBox)
		// 	icon = smoothSquareIcon;
		// else
		// 	icon = circleIcon;

		// if(width != 0.)
		// 	iconBox((height/2.)*1.3,height*1.3,position_x-width,position_y,z,icon,mixVal,color,colorTransitionColor);
		// iconBox((height/2.)*1.3,height*1.3,position_x+width,position_y,z,icon,mixVal,color,colorTransitionColor);
		// glUseProgram(uiPrograms.uiProgram);
	}

	if (!isTextBox && text != "") {
		renderText(uiPrograms.uiProgram, text, position_x -textRatio, position_y - 0.01f, 0.00022f,colorData.textColor,z+0.001f,false,position_x+width,false,0);
	}
	else if(text != ""){
		if(mixVal > 0.f){
			color = glm::vec4(0);

			container(position_x,position_y-height*4,0.95,width,height*4,color,uiPrograms,circleIcon,colorTransitionColor,mixVal);
			glUseProgram(uiPrograms.uiProgram);


			renderText(uiPrograms.uiProgram, text, -width/1.2 + position_x, position_y - 0.01f, 0.00022f,colorData.textColor,0.96f,mixVal > 0.f,position_x+width,true,0);
			
		}
		else{
			Utilities util;
			renderText(uiPrograms.uiProgram, text, -width/1.2 + position_x, position_y - 0.01f, 0.00022f,colorData.textColor,z+0.001f,mixVal > 0.f,position_x+width,false,0);
		}
	}
	if(width == 0){
		glUseProgram(uiPrograms.iconsProgram);
		iconBox(height/2.f,height,position_x,position_y,z,circleIcon,mixVal,color,colorTransitionColor);
		glUseProgram(uiPrograms.uiProgram);
	}

	glBindBuffer(GL_ARRAY_BUFFER,uiObjects.VBO);
	glBindVertexArray(uiObjects.VAO);
}
void UserInterface::box(float width, float height, float position_x, float position_y,std::string text,glm::vec4 color, float textRatio,bool isTextBox,bool isMaskImageBox,float z,float buttonCurveReduce, glm::vec4 colorTransitionColor, float mixVal,glm::vec4 textColor,glm::vec4 textColor2,float textSize,bool outline) {
	
	//buttonCurveReduce = 10 | normal  
	//buttonCurveReduce = >10 | smaller
	//buttonCurveReduce = <10 | bigger
	//buttonCurveReduce = 10000 to get a square

	//parameter z = position_z
	GlSet glset;

	//if(outline)
		//width*=1.15;

	glm::mat4 scale = glm::mat4(1);
	scale = glm::scale(scale,glm::vec3(width,height,1));
	glset.uniformMatrix4fv(uiPrograms.uiProgram,"scale",scale);
	
	glset.uniform1f(uiPrograms.uiProgram,"width",width);
	glset.uniform1f(uiPrograms.uiProgram,"height",height);
	glset.uniform1f(uiPrograms.uiProgram,"radius",buttonCurveReduce);
	
	glm::vec3 pos = glm::vec3(position_x,position_y,z);
	glset.uniform3fv(uiPrograms.uiProgram,"pos",pos);

	glset.uniform1i(uiPrograms.uiProgram,"outline",outline);

	glset.uniform1i(uiPrograms.uiProgram,"isUiTextureUsed",isMaskImageBox);
	glset.uniform4fv(uiPrograms.uiProgram,"uiColor",color);
	glset.uniform4fv(uiPrograms.uiProgram, "uiTransitionColor", colorTransitionColor);
	glset.uniform1f(uiPrograms.uiProgram, "uiTransitionMixVal", mixVal);

	glBindBuffer(GL_ARRAY_BUFFER,uiObjects.sqrVBO);
	glBindVertexArray(uiObjects.sqrVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	ColorData colorData;



	scale = glm::mat4(1);
	pos = glm::vec3(0);
	glset.uniformMatrix4fv(uiPrograms.uiProgram,"scale",scale);
	glset.uniform3fv(uiPrograms.uiProgram,"pos",pos);

	glset.uniform1i(uiPrograms.uiProgram, "isUiTextureUsed", 0);

	if(buttonCurveReduce <= 200){
		// glUseProgram(uiPrograms.iconsProgram);
		// unsigned int icon;
		// if(isTextBox)
		// 	icon = smoothSquareIcon;
		// else
		// 	icon = circleIcon;

		// if(width != 0.)
		// 	iconBox((height/2.)*1.3,height*1.3,position_x-width,position_y,z,icon,mixVal,color,colorTransitionColor);
		// iconBox((height/2.)*1.3,height*1.3,position_x+width,position_y,z,icon,mixVal,color,colorTransitionColor);
		// glUseProgram(uiPrograms.uiProgram);
	}

	if (!isTextBox && text != "") {
		glset.uniform4fv(uiPrograms.uiProgram, "uiTransitionColor", textColor2);
		renderText(uiPrograms.uiProgram, text, position_x -textRatio, position_y - 0.01f, textSize,textColor,z+0.001f,false,position_x+width,false,mixVal);
	}
	else if(text != ""){
		if(mixVal > 0.f){
			color = glm::vec4(0);

			container(position_x,position_y-height*4,0.95,width,height*4,color,uiPrograms,circleIcon,colorTransitionColor,mixVal);
			glUseProgram(uiPrograms.uiProgram);

			glset.uniform4fv(uiPrograms.uiProgram, "uiTransitionColor", textColor2);
			renderText(uiPrograms.uiProgram, text, -width/1.2 + position_x, position_y - 0.01f, textSize,textColor,0.96f,mixVal > 0.f,position_x+width,true,mixVal);
			
		}
		else{
			Utilities util;
			glset.uniform4fv(uiPrograms.uiProgram, "uiTransitionColor", textColor2);
			renderText(uiPrograms.uiProgram, text, -width/1.2 + position_x, position_y - 0.01f, textSize,textColor,z+0.001f,mixVal > 0.f,position_x+width,false,mixVal);
		}
	}

	glBindBuffer(GL_ARRAY_BUFFER,uiObjects.VBO);
	glBindVertexArray(uiObjects.VAO);
}

void UserInterface::panel(float panelLoc, Icons icons,PanelData &panelData) {
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
	circle(panelLoc + cornerWidth - 0.002f, panelHeigth - cornerWidth + 0.002f,panelZ,cornerWidth+0.01f,(cornerWidth+0.01f)*2,icons.Circle,colorD.panelColor,glm::vec4(0),0);
	circle(panelLoc + cornerWidth,-panelHeigth + cornerWidth,panelZ,cornerWidth+0.01f,(cornerWidth+0.01f)*2,icons.Circle,colorD.panelColor,glm::vec4(0),0);

	Utilities util;
	panelData.modelPanelMixval = util.transitionEffect(panelData.modelPanelActive,panelData.modelPanelMixval,0.1f);
	panelData.texturePanelMixval = util.transitionEffect(panelData.texturePanelActive,panelData.texturePanelMixval,0.1f);
	panelData.exportPanelMixval = util.transitionEffect(panelData.exportPanelActive,panelData.exportPanelMixval,0.1f);
	panelData.paintingPanelMixval = util.transitionEffect(panelData.paintingPanelActive,panelData.paintingPanelMixval,0.1f);
	panelData.settingsPanelMixval = util.transitionEffect(panelData.settingsPanelActive,panelData.settingsPanelMixval,0.1f);
	panelData.generatorPanelMixval = util.transitionEffect(panelData.generatorPanelActive,panelData.generatorPanelMixval,0.1f);

	//Texture panel button
	iconBox(0.017f,0.034f,panelLoc - 0.017f,0.8f,panelZ+0.01f,icons.PanelButtonR,0,colorD.panelHoldColor,colorD.panelHoldColor);
	iconBox(0.01f,0.02f,panelLoc - 0.013f,0.808f,0.6f,icons.TDModel,panelData.modelPanelMixval,colorD.iconColor,glm::vec4(colorD.LigidPainterThemeColor,1));
	
	//Material panel button
	iconBox(0.017f,0.034f,panelLoc - 0.017f,0.72f,panelZ+0.01f,icons.PanelButtonR,0,colorD.panelHoldColor,colorD.panelHoldColor);
	iconBox(0.012f,0.024f,panelLoc - 0.013f,0.725f,0.6f,icons.Material,panelData.texturePanelMixval,colorD.iconColor,glm::vec4(colorD.LigidPainterThemeColor,1));

	//Painting panel button
	iconBox(0.017f,0.034f,panelLoc - 0.017f,0.64f,panelZ+0.01f,icons.PanelButtonR,0,colorD.panelHoldColor,colorD.panelHoldColor);
	iconBox(0.012f,0.024f,panelLoc - 0.013f,0.647f,0.6f,icons.Painting,panelData.paintingPanelMixval,colorD.iconColor,glm::vec4(colorD.LigidPainterThemeColor,1));

	//Export panel button
	iconBox(0.017f,0.034f,panelLoc - 0.017f,0.56f,panelZ+0.01f,icons.PanelButtonR,0,colorD.panelHoldColor,colorD.panelHoldColor);
	iconBox(0.012f,0.024f,panelLoc - 0.013f,0.567f,0.6f,icons.Export,panelData.exportPanelMixval,colorD.iconColor,glm::vec4(colorD.LigidPainterThemeColor,1));

	//Settings panel button
	iconBox(0.017f,0.034f,panelLoc - 0.017f,0.48f,panelZ+0.01f,icons.PanelButtonR,0,colorD.panelHoldColor,colorD.panelHoldColor);
	iconBox(0.011f,0.022f,panelLoc - 0.013f,0.485f,0.6f,icons.Gear,panelData.settingsPanelMixval,colorD.iconColor,glm::vec4(colorD.LigidPainterThemeColor,1));

	//Generator panel button
	iconBox(0.017f,0.034f,panelLoc - 0.017f,0.4f,panelZ+0.01f,icons.PanelButtonR,0,colorD.panelHoldColor,colorD.panelHoldColor);
	iconBox(0.011f,0.022f,panelLoc - 0.013f,0.405f,0.6f,icons.TextureGenerator,panelData.generatorPanelMixval,colorD.iconColor,glm::vec4(colorD.LigidPainterThemeColor,1));

	glUseProgram(uiPrograms.uiProgram);

}

bool sndpanelMoveTexture = false;
bool sndpanelFolderPressed = false;
int sndpanelFolderCounter = 0;

bool sndpanelSliderPressed = false;
int subselectedIndex = 2;
void UserInterface::sndPanel(int state,float panelLoc,Programs programs,Icons icons,std::vector<aTexture> &albedoTextures, GLFWwindow* window,double mouseXpos,
							double mouseYpos,float screenGapX,float removeThisParam, int& selectedAlbedoTextureIndex,std::vector<NodeScene>& nodeScenes,int &selectedNodeScene,
							bool& newModelAdded,float &txtrSlideVal,float &materialSlideVal,bool &firstClick,ColoringPanel &clringPanel,TextureCreatingPanel &txtrCreatingPanel,
							bool& anyTextureNameActive,std::string &textureText,int& folderIndex,NodePanel &nodePanel,std::vector<Node> appNodes,SndPanel &sndpnl,BrushTexture &brushMaskTextures,
							bool maskPanelEnter,float yOffset,std::vector<NodeScene> &nodeScenesHistory,int brushMaskTexturesState,int chosenTextureResIndex,bool &textureDraggingState) {
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
	
	Utilities util;

	//Slider

	std::vector<int> folderIndices;
	for (size_t i = 0; i < albedoTextures.size(); i++)
	{
		folderIndices.push_back(albedoTextures[i].folderIndex);
	}
	

	box(0.007f, panelHeigth - cornerWidth, panelLoc - panelWidth*2 + cornerWidth/2.5f , +0.014f, "", colorD.panelColorSnd, 0.022f, false, false, panelZ+0.01, 10000, colorD.panelColor, 0);
	int elementSize = 0;
	float slideVal = 0;
	if(sndpnl.state == 0){
		slideVal = sndpnl.texturePanelSlideVal;
		elementSize = util.getElementSizeOfTheFolder(folderIndices,folderIndex);
	}
	if(sndpnl.state == 1){
		slideVal = sndpnl.materialPanelSlideVal;
		elementSize = nodeScenes.size();
	}
		
	float slidePHeight = (panelHeigth - cornerWidth) * (7.f/(max((float) util.getElementSizeOfTheFolder(folderIndices,folderIndex)/3.f,7.f))); 
	slideVal *= slidePHeight;
	
	if(sndpnl.state == 0){
		if(util.getElementSizeOfTheFolder(folderIndices,folderIndex) > 21 && util.getElementSizeOfTheFolder(folderIndices,folderIndex) <= 27){
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
			txtrSlideVal += yOffset/(glfwGetVideoMode(glfwGetPrimaryMonitor())->height/2);
			if(txtrSlideVal < 0.f)
					txtrSlideVal = 0.001f;
		}
		if(sndpnl.state){
			materialSlideVal += yOffset/(glfwGetVideoMode(glfwGetPrimaryMonitor())->height/2);
			if(materialSlideVal < 0.f)
				materialSlideVal = 0.001f;
		}
		slideVal += (yOffset/(glfwGetVideoMode(glfwGetPrimaryMonitor())->height/2))*slidePHeight;
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
	circle(panelLoc - cornerWidth + 0.002f,panelHeigth - cornerWidth + 0.002f,panelZ,cornerWidth+0.01f,(cornerWidth+0.01f)*2,icons.Circle,colorD.panelColor,glm::vec4(0),0);
	circle(panelLoc - cornerWidth,-panelHeigth + cornerWidth,panelZ,cornerWidth+0.01f,(cornerWidth+0.01f)*2,icons.Circle,colorD.panelColor,glm::vec4(0),0);
	
	sndpnl.texturePanelButtonMixval = util.transitionEffect(1-sndpnl.state,sndpnl.texturePanelButtonMixval,0.1f);
	sndpnl.materialPanelButtonMixval = util.transitionEffect(sndpnl.state,sndpnl.materialPanelButtonMixval,0.1f);

	//Texture panel button
	iconBox(0.017f,0.034f,panelLoc + 0.017f,0.8f,panelZ+0.01f,icons.PanelButtonL,0,colorD.panelHoldColor,colorD.panelHoldColor);
	iconBox(0.01f,0.02f,panelLoc + 0.013f,0.808f,panelZ+0.02f,icons.Texture,sndpnl.texturePanelButtonMixval,colorD.iconColor,glm::vec4(colorD.LigidPainterThemeColor,1));
	
	//Material panel button
	iconBox(0.017f,0.034f,panelLoc + 0.017f,0.72f,panelZ+0.01f,icons.PanelButtonL,0,colorD.panelHoldColor,colorD.panelHoldColor);
	iconBox(0.012f,0.024f,panelLoc + 0.013f,0.725f,panelZ+0.02f,icons.Material,sndpnl.materialPanelButtonMixval,colorD.iconColor,glm::vec4(colorD.LigidPainterThemeColor,1));
	
	if(panelLoc > -0.98f){

		if(state == 0){
			if(folderIndex != 10000)
				iconBox(0.01f,0.02f,panelLoc - 0.365f,0.86f,panelZ+0.01f,icons.Undo,sndpnl.backSignMixVal,colorD.iconColor,colorD.iconColorHover);
			iconBox(0.01f,0.02f,panelLoc - 0.05f,0.85f,panelZ+0.01f,icons.ArrowDown,sndpnl.downSignMixVal,colorD.iconColor,colorD.iconColorHover);
			iconBox(0.01f,0.02f,panelLoc - 0.17f,0.85f,panelZ+0.01f,icons.LoadFolder,sndpnl.folderSignMixVal,colorD.iconColor,colorD.iconColorHover);
		}
		else{
			iconBox(0.01f,0.02f,panelLoc - 0.17f,0.85f,panelZ+0.01f,icons.MaterialImport,sndpnl.folderSignMixVal,colorD.iconColor,colorD.iconColorHover);
			iconBox(0.01f,0.02f,panelLoc - 0.05f,0.85f,panelZ+0.01f,icons.MaterialExport,sndpnl.downSignMixVal,colorD.iconColor,colorD.iconColorHover);
		}

		iconBox(0.01f,0.02f,panelLoc - 0.08f,0.85f,panelZ+0.01f,icons.Plus,sndpnl.plusSignMixVal,colorD.iconColor,colorD.iconColorHover);
		iconBox(0.01f,0.02f,panelLoc - 0.11f,0.85f,panelZ+0.01f,icons.Minus,sndpnl.minusSignMixVal,colorD.iconColor,colorD.iconColorHover);
		iconBox(0.01f,0.02f,panelLoc - 0.14f,0.85f,panelZ+0.01f,icons.Mirror,sndpnl.duplicateSignMixVal,colorD.iconColor,colorD.iconColorHover);
	


		glUseProgram(programs.uiProgram);

		//Barrier
		box(panelWidth+0.05f, 0.5f, panelLoc - panelWidth - cornerWidth, panelHeigth+0.42f, "", glm::vec4(0), 0.022f, false, false, panelZ+0.05, 10000, colorD.panelColor, 0);
		box(panelWidth+0.05f, 0.5f, panelLoc - panelWidth - cornerWidth, -panelHeigth-0.42f, "", glm::vec4(0), 0.022f, false, false, panelZ+0.05, 10000, colorD.panelColor, 0);

		const float screenGapXSc = glfwGetVideoMode(glfwGetPrimaryMonitor())->width-screenWidth;


		if(albedoTextures.size()){
			if(albedoTextures[selectedAlbedoTextureIndex].isTexture && sndpanelMoveTexture && glfwGetMouseButton(window,0) == GLFW_RELEASE && maskPanelEnter){
				
				int txtrRes = 256;
				for (size_t i = 0; i < chosenTextureResIndex; i++)
				{
					txtrRes*=2;
				}

				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D,albedoTextures[selectedAlbedoTextureIndex].id);
				Texture txtr;
				GLubyte* txtrData = txtr.getTextureFromProgram(GL_TEXTURE0,txtrRes,txtrRes,4);

				glActiveTexture(GL_TEXTURE0);
				unsigned int texture;
				glGenTextures(1,&texture);
				glBindTexture(GL_TEXTURE_2D,texture);
				glset.texImage(txtrData,txtrRes,txtrRes,GL_RGBA);
				glset.generateMipmap();

				aTexture atxtr = albedoTextures[selectedAlbedoTextureIndex];
				Utilities util;

				atxtr.name = atxtr.name;
				atxtr.id = texture;


				//TODO Update that
				if(brushMaskTexturesState == 0)
					brushMaskTextures.maskTextures.push_back(atxtr);
				if(brushMaskTexturesState == 1)
					brushMaskTextures.colorTextures.push_back(atxtr);
				if(brushMaskTexturesState == 2)
					brushMaskTextures.normalTextures.push_back(atxtr);
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
				imageNode.positionY = ((-mouseYpos/glfwGetVideoMode(glfwGetPrimaryMonitor())->height*2 + 1.0f) / nodePanel.zoomVal - nodePanel.panelPositionY);

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

					if(isMouseOnCoords(window,mouseXpos+screenGapX*(glfwGetVideoMode(glfwGetPrimaryMonitor())->width/2),mouseYpos,buttonCoorSq,false) && !clringPanel.active && !txtrCreatingPanel.active){
						
						if(firstClick && glfwGetKey(window,GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS){
							if(selectedAlbedoTextureIndex != i){
								glActiveTexture(GL_TEXTURE17);
								subselectedIndex = i;
								glset.bindTexture(albedoTextures[i].id);
								glActiveTexture(GL_TEXTURE28);
							}
							else{
								glActiveTexture(GL_TEXTURE17);
								subselectedIndex = 10000;
								glset.bindTexture(albedoTextures[i].id);
								glActiveTexture(GL_TEXTURE28);
							}	

						}
						glset.uniform1i(uiPrograms.renderTheTextureProgram, "isHover" ,1);
						isHover = true;
						if(glfwGetMouseButton(window,1) == GLFW_PRESS)
							albedoTextures[i].rightClicked = true;
						if(!albedoTextures[i].isTexture && sndpanelMoveTexture && glfwGetMouseButton(window,0) == GLFW_RELEASE){
							if(selectedAlbedoTextureIndex != i && !albedoTextures[selectedAlbedoTextureIndex].isTrashFolder && selectedAlbedoTextureIndex != 1 && selectedAlbedoTextureIndex != 2){
								albedoTextures[selectedAlbedoTextureIndex].folderIndex = i;
								albedoTextures[selectedAlbedoTextureIndex].changed = true;
							}
						}
						if(firstClick && !albedoTextures[i].rightClicked && glfwGetKey(window,GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE){
							if(albedoTextures[i].isTexture){
								glActiveTexture(GL_TEXTURE0);
								glBindTexture(GL_TEXTURE_2D,albedoTextures[selectedAlbedoTextureIndex].id);
								selectedAlbedoTextureIndex = i;
								if(subselectedIndex == i)
									subselectedIndex = 1000;
								glActiveTexture(GL_TEXTURE28);
							}
							else{
								sndpanelFolderPressed = true;
								selectedAlbedoTextureIndex = i;
								if(subselectedIndex == i)
									subselectedIndex = 1000;
							}
							sndpanelMoveTexture = true;
						}
					}
					else{
						albedoTextures[i].rightClicked = false;
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
					glset.uniform1i(uiPrograms.renderTheTextureProgram, "subSelected" ,i == subselectedIndex);
					if(i == subselectedIndex)
						glset.uniform1i(uiPrograms.renderTheTextureProgram, "isPressed" ,1);


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
						if(albedoTextures[i].isTrashFolder)
							glBindTexture(GL_TEXTURE_2D,icons.Trash);
						else if(i == 1)
							glBindTexture(GL_TEXTURE_2D,icons.ExportFolder);
						else if(i == 2)
							glBindTexture(GL_TEXTURE_2D,icons.BrushFolder);
						else
							glBindTexture(GL_TEXTURE_2D,icons.Folder);
						glm::mat4 scaleMat = glm::mat4(1);
						glm::vec3 posVec = glm::vec3(0);
						glset.uniformMatrix4fv(programs.iconsProgram,"scale",scaleMat);
						glset.uniform3fv(programs.iconsProgram,"pos",posVec);
						glset.uniform4fv(programs.iconsProgram,"iconColor",iconColor);
						glset.uniform1f(programs.iconsProgram,"iconMixVal",0);
					}

					glset.drawArrays(buttonCoorSq,false);

					if(isMouseOnCoords(window,mouseXpos+screenGapX*(glfwGetVideoMode(glfwGetPrimaryMonitor())->width/2),mouseYpos,buttonCoorSq,false) && !clringPanel.active && !txtrCreatingPanel.active){
						if(albedoTextures[i].isTexture && !albedoTextures[i].rightClicked){
							glUseProgram(uiPrograms.uiProgram);
							renderText(uiPrograms.uiProgram,"W : " + std::to_string(albedoTextures[i].width),position_x- 0.04f  ,position_y + 0.025f  ,0.00022f,colorData.textColor,panelZ+0.021f,false);
							renderText(uiPrograms.uiProgram,"H : " + std::to_string(albedoTextures[i].height),position_x- 0.04f ,position_y - 0.025f ,0.00022f,colorData.textColor,panelZ+0.021f,false);
						}
					}

					glUseProgram(programs.uiProgram);

					bool textHover = isMouseOnButton(window,0.06f,0.015f,position_x-screenGapX,position_y - textureWidth*2.5,mouseXpos,mouseYpos,false);

					if(textHover && firstClick && !anyTextureNameActive && albedoTextures[i].isTrashFolder == false){
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
					if(albedoTextures[i].rightClicked){
						glUseProgram(programs.uiProgram);
						bool removeRCHover = false;

						bool selectRCHover = false;
						if(isMouseOnButton(window,textureWidth/1.1,textureWidth/3.f,position_x,position_y,mouseXpos,mouseYpos,false)){
							selectRCHover = true;
							if(firstClick){
								albedoTextures[i].rightClicked = false;
								if(albedoTextures[i].isTexture){
								//Select the texture
									selectedAlbedoTextureIndex = i;
									if(subselectedIndex == i)
										subselectedIndex = 1000;
								}
								else //Open the folder
									folderIndex = i;
								sndpanelMoveTexture = false;
							}
						}	
						if(albedoTextures[i].isTexture)
							box(textureWidth/1.2,textureWidth/3.f,position_x,position_y,"Select",colorData.buttonColor,0.025f,false,false,panelZ+0.03f,10,colorData.buttonColorHover,selectRCHover);
						else
							box(textureWidth/1.2,textureWidth/3.f,position_x,position_y,"Open",colorData.buttonColor,0.025f,false,false,panelZ+0.03f,10,colorData.buttonColorHover,selectRCHover);
						
						bool subSelectRCHover = false;
						if(isMouseOnButton(window,textureWidth/1.1,textureWidth/3.f,position_x,position_y-textureWidth,mouseXpos,mouseYpos,false)){
							subSelectRCHover = true;
							if(firstClick){
								albedoTextures[i].rightClicked = false;
								sndpanelMoveTexture = false;
								if(selectedAlbedoTextureIndex != i){
									glActiveTexture(GL_TEXTURE17);
									subselectedIndex = i;
									glset.bindTexture(albedoTextures[i].id);
									glActiveTexture(GL_TEXTURE28);
								}
								else{
									glActiveTexture(GL_TEXTURE17);
									subselectedIndex = 10000;
									glset.bindTexture(albedoTextures[i].id);
									glActiveTexture(GL_TEXTURE28);
								}					
							}
						}	
						if(albedoTextures[i].isTexture)
							box(textureWidth/1.2,textureWidth/3.f,position_x,position_y-textureWidth,"SubSelect",colorData.buttonColor,0.04f,false,false,panelZ+0.03f,10,colorData.buttonColorHover,subSelectRCHover);

						if(isMouseOnButton(window,textureWidth/1.1,textureWidth/3.f,position_x,position_y+textureWidth,mouseXpos,mouseYpos,false)){
							removeRCHover = true;
							if(firstClick){
								albedoTextures[i].rightClicked = false;
								if(albedoTextures[i].isTexture){
									if(albedoTextures[i].folderIndex == 0){
										//Delete texture
										Texture txtr;
										txtr.deleteOpenglTexture(albedoTextures[i]);
										albedoTextures.erase(albedoTextures.begin()+i);
										if(selectedAlbedoTextureIndex == i)
											selectedAlbedoTextureIndex--;
										i--;
										lC--;
										sndpanelMoveTexture = false;
									}
									else{
										albedoTextures[i].folderIndex = 0;
										albedoTextures[i].changed = true;
									}
								}
								else if(!albedoTextures[i].isTrashFolder){
									if(i == 1 || i == 2){
										alert("This folder can't be deleted",200,false);
									}
									else{
										if(albedoTextures[i].folderIndex == 0){
											//Delete the folder
											for (size_t delI = 0; delI < albedoTextures.size(); delI++)
											{
												//Delete the elements of the folder
												if(albedoTextures[delI].folderIndex == i){
													if(albedoTextures[delI].isTexture){
														Texture txtr;
														txtr.deleteOpenglTexture(albedoTextures[delI]);
													}
													else{
														//TODO : Delete that folder's elements
													}
													albedoTextures.erase(albedoTextures.begin() + delI);
													delI--;
													i--;
													lC--;
												}
											}
											albedoTextures.erase(albedoTextures.begin() + i);
											selectedAlbedoTextureIndex = 0;
											folderIndex = 10000;
											sndpanelFolderPressed = false;
											sndpanelMoveTexture = false;
											i--;
											lC--;
										}
										else{
											//Move folder into trash
											albedoTextures[i].folderIndex = 0;
											albedoTextures[i].changed = true;
											selectedAlbedoTextureIndex = 0;
											sndpanelFolderPressed = false;
											sndpanelMoveTexture = false;
										}
									}
								}
								else if(albedoTextures[i].isTrashFolder){
									alert("Warning! This is THE TRASH folder lol",200,false);
									folderIndex = 10000;
									sndpanelFolderPressed = false;
									sndpanelMoveTexture = false;
								}
							}
						}	
						if(albedoTextures[i].folderIndex == 0)
							box(textureWidth/1.2,textureWidth/3.f,position_x,position_y+textureWidth,"Remove",colorData.buttonColor,0.032f,false,false,panelZ+0.03f,10,colorData.buttonColorHover,removeRCHover);
						else
							box(textureWidth/1.2,textureWidth/3.f,position_x,position_y+textureWidth,"To Trash",colorData.buttonColor,0.035f,false,false,panelZ+0.03f,10,colorData.buttonColorHover,removeRCHover);
						
						glUseProgram(programs.renderTheTextureProgram);
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
					 textureWidth*1.6f + position_x,  std::min(std::max( textureWidth*2 + position_y,minBot),maxTop), 	panelZ+0.02f,	1,		upBotDifMin*10			,0,0,0,  // top right
					 textureWidth*1.6f + position_x,  std::min(std::max(-textureWidth*2 + position_y,minBot),maxTop), 	panelZ+0.02f,	1,		1.0f-upBotDifMax*10		,0,0,0,  // bottom right
					-textureWidth*1.6f + position_x,  std::min(std::max( textureWidth*2 + position_y,minBot),maxTop), 	panelZ+0.02f,	0,		upBotDifMin*10			,0,0,0,  // top left 
					// second tri*1.6fangle						   
					 textureWidth*1.6f + position_x,  std::min(std::max(-textureWidth*2 + position_y,minBot),maxTop), 	panelZ+0.02f,	1,		1.0f-upBotDifMax*10		,0,0,0,  // bottom right
					-textureWidth*1.6f + position_x,  std::min(std::max(-textureWidth*2 + position_y,minBot),maxTop), 	panelZ+0.02f,	0,		1.0f-upBotDifMax*10		,0,0,0,  // bottom left
					-textureWidth*1.6f + position_x,  std::min(std::max( textureWidth*2 + position_y,minBot),maxTop), 	panelZ+0.02f,	0,		upBotDifMin*10			,0,0,0  // top left
				};
				ColorData colorData;
				bool isHover;
				bool isPressed;

				if(isMouseOnCoords(window,mouseXpos+screenGapX*(glfwGetVideoMode(glfwGetPrimaryMonitor())->width/2),mouseYpos,buttonCoorSq,false)){
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

				glUseProgram(programs.renderTheTextureProgram);
				glActiveTexture(GL_TEXTURE14);
				glBindTexture(GL_TEXTURE_2D,nodeScenes[i].renderedTexture);
				glset.uniform1i(programs.renderTheTextureProgram,"isPressed",isPressed);
				glset.uniform1i(programs.renderTheTextureProgram,"isHover",isHover);
				glset.uniform1i(programs.renderTheTextureProgram,"renderMaterials",1);
				//glset.uniform1f(programs.iconsProgram,"iconMixVal",0);
				glset.drawArrays(buttonCoorSq,false);
				glset.uniform1i(programs.renderTheTextureProgram,"renderMaterials",0);

				glUseProgram(programs.uiProgram);
				renderText(programs.uiProgram,nodeScenes[i].sceneName,position_x- textureWidth ,position_y - textureWidth*2,0.00022f,colorData.textColor,panelZ+0.03f,false);

				renderText(programs.uiProgram,std::to_string(nodeScenes[i].index),position_x - textureWidth ,position_y - textureWidth,0.00042f,colorData.materialIconIndexTextColor,panelZ+0.03f,false);
			}
		}
		if(sndpanelMoveTexture && glfwGetMouseButton(window,0) == GLFW_RELEASE && sndpnl.backSignHover){
			albedoTextures[selectedAlbedoTextureIndex].folderIndex = 10000;
			albedoTextures[selectedAlbedoTextureIndex].changed = true;
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
	
		if(sndpanelMoveTexture){
			float posx = ((mouseXpos + screenGapXSc/2.f)/(glfwGetVideoMode(glfwGetPrimaryMonitor())->width/2))-1.f;
			float posy = (2.f - (mouseYpos/(glfwGetVideoMode(glfwGetPrimaryMonitor())->height/2.f)))-1.f;
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
				if(albedoTextures[selectedAlbedoTextureIndex].isTrashFolder)
					glBindTexture(GL_TEXTURE_2D,icons.Trash);
				else if(selectedAlbedoTextureIndex == 1)
					glBindTexture(GL_TEXTURE_2D,icons.ExportFolder);
				else if(selectedAlbedoTextureIndex == 2)
					glBindTexture(GL_TEXTURE_2D,icons.BrushFolder);
				else
					glBindTexture(GL_TEXTURE_2D,icons.Folder);
				
				glm::mat4 scaleMat = glm::mat4(1);
				glm::vec3 posVec = glm::vec3(0);
				glset.uniformMatrix4fv(programs.iconsProgram,"scale",scaleMat);
				glset.uniform3fv(programs.iconsProgram,"pos",posVec);

				glset.uniform4fv(programs.iconsProgram,"iconColor",colorData.iconColor);
				glset.uniform1f(programs.iconsProgram,"iconMixVal",0);
				glset.drawArrays(renderVertices,false);
				glUseProgram(0);
			}
		}
	}
	textureDraggingState = sndpanelMoveTexture;

}

void UserInterface::textureSelectionPanel(TextureSelectionPanel &textureSelectionPanel,std::vector<aTexture> &albedoTextures,Programs programs,GLFWwindow* window,double mouseXpos,
										  double mouseYpos,float screenGapX,int removeThisParam,unsigned int circleTexture, bool firstClick,Icons icons){
	ColorData clrData;
	GlSet glset;

	const float boxWidth = 0.25f;


	textureSelectionPanel.panelHover = isMouseOnButton(window,boxWidth+0.03,boxWidth+0.06,textureSelectionPanel.posX - screenGapX,textureSelectionPanel.posY,mouseXpos,mouseYpos,false);
	if(!textureSelectionPanel.panelHover){
		textureSelectionPanel.active = false;
	}

	container(textureSelectionPanel.posX,textureSelectionPanel.posY,0.95f,boxWidth,boxWidth,clrData.textureSelectionPanelColor,programs,circleTexture,glm::vec4(0),0);

	glUseProgram(programs.renderTheTextureProgram);
	
	glActiveTexture(GL_TEXTURE14);
	glset.uniform1i(uiPrograms.renderTheTextureProgram, "txtr" ,14);


	float maskXpos = 0.0f;
	float maskYpos = 0.0f;

	std::vector<aTexture> onlyTextures;

	for (size_t i = 0; i < albedoTextures.size(); i++)
	{
		if(albedoTextures[i].isTexture)
			onlyTextures.push_back(albedoTextures[i]);
	}
	bool leftAHover = false;
	if(isMouseOnButton(window,0.01f, 0.02f, textureSelectionPanel.posX+0.08f,textureSelectionPanel.posY+0.28f,mouseXpos,mouseYpos,false)){
		leftAHover = true;
		if(glfwGetMouseButton(window,0) == GLFW_PRESS){
			textureSelectionPanel.scroll--;
			if(textureSelectionPanel.scroll < 0)
				textureSelectionPanel.scroll = 0;
		}
	}
	if(glfwGetKey(window,GLFW_KEY_LEFT) == GLFW_PRESS){
			textureSelectionPanel.scroll--;
			if(textureSelectionPanel.scroll < 0)
				textureSelectionPanel.scroll = 0;
	}
	if(glfwGetKey(window,GLFW_KEY_RIGHT) == GLFW_PRESS){
			textureSelectionPanel.scroll++;
	}
	bool rightAHover = false;
	if(isMouseOnButton(window,0.01f, 0.02f, textureSelectionPanel.posX+0.22f,textureSelectionPanel.posY+0.28f,mouseXpos,mouseYpos,false)){
		rightAHover = true;
		if(glfwGetMouseButton(window,0) == GLFW_PRESS)
			textureSelectionPanel.scroll++;
	}

	if(textureSelectionPanel.scroll > std::max((int)onlyTextures.size()-50,0))
		textureSelectionPanel.scroll = onlyTextures.size()-50;
	int counter = 0;
	for (size_t i = textureSelectionPanel.scroll; i < std::min(std::min((int)onlyTextures.size()+textureSelectionPanel.scroll,50+textureSelectionPanel.scroll),(int)onlyTextures.size()); i++)
	{
			if(counter % 10 == 0 && counter != 0){
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
			glBindTexture(GL_TEXTURE_2D,onlyTextures[i].id);

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
			if(isMouseOnCoords(window,mouseXpos+screenGapX*(glfwGetVideoMode(glfwGetPrimaryMonitor())->width/2) - screenGapX,mouseYpos,buttonCoorSq,false)){
				glUseProgram(programs.uiProgram);
				box(0.1f,0.02f,textureSelectionPanel.posX-boxWidth+0.1f,textureSelectionPanel.posY+boxWidth*1.1f,onlyTextures[i].name, glm::vec4(0),0,true,0,0.96,10,clrData.textureSelectionPanelColor,0);
				glUseProgram(programs.renderTheTextureProgram);

				glset.uniform1i(uiPrograms.renderTheTextureProgram, "isHover" ,1);
				if(firstClick && !textureSelectionPanel.firstCycle){
					textureSelectionPanel.selectedIndex = i;
					textureSelectionPanel.selectedTextureName = onlyTextures[i].name;
					textureSelectionPanel.textureClicked = true;
				}
			}
			else{
				glset.uniform1i(uiPrograms.renderTheTextureProgram, "isHover" ,0);
			}

			glset.drawArrays(buttonCoorSq,false);

			counter++;
	}


	glDisable(GL_DEPTH_TEST);
	glUseProgram(programs.iconsProgram);

	iconBox(0.01f, 0.02f, textureSelectionPanel.posX+0.08f,textureSelectionPanel.posY+0.28f,1,icons.ArrowLeft,leftAHover,clrData.iconColor,clrData.iconColorHover);
	iconBox(0.01f, 0.02f, textureSelectionPanel.posX+0.22f,textureSelectionPanel.posY+0.28f,1,icons.ArrowRight,rightAHover,clrData.iconColor,clrData.iconColorHover);

	
	
	glUseProgram(programs.uiProgram);
	renderText(programs.uiProgram, std::to_string(textureSelectionPanel.scroll) + " , "+ std::to_string(std::min(std::min((int)onlyTextures.size()+textureSelectionPanel.scroll,50+textureSelectionPanel.scroll),(int)onlyTextures.size())) + " / " + std::to_string(onlyTextures.size()),textureSelectionPanel.posX+0.1f,textureSelectionPanel.posY+0.27f,0.00022f,clrData.textColor,1.f,false);
	glEnable(GL_DEPTH_TEST);
}

void UserInterface::textureDisplayer(float width,float height, float position_x,float position_y,float z,unsigned int icon){ 
	GlSet glset;
	ColorData clrData;

	glUseProgram(uiPrograms.iconsProgram);
	iconBox(0.02f/2.f,0.02f,position_x+0.005f,position_y-0.01f,z, icon,0.f,glm::vec4(0),glm::vec4(0));
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

	glUseProgram(uiPrograms.textureDisplayer);
	glset.uniform1i(uiPrograms.textureDisplayer, "currentTexture", 9);
	glset.drawArrays(buttonCoorSq,false);
	glUseProgram(uiPrograms.iconsProgram);
	iconBox(0.035f/2.f,0.035f,position_x+0.005f,position_y-0.01f,1.f, icon,0.f,clrData.textureDisplayerButtonColor,clrData.textureDisplayerButtonColor);
	glUseProgram(uiPrograms.uiProgram);
}

bool vertRGBarPressed = false;
bool UserInterface::verticalRangeBar(float positionX,float positionY,float height,float &orgvalue,float value,GLFWwindow* window,float mouseX,float mouseY,float yOffset,bool &firstClick,int textureSize,float screenGapX){
	ColorData colorData;

	if(isMouseOnButton(window,0.015f,0.015f,positionX-screenGapX,positionY-value,mouseX,mouseY,false) && firstClick)
		vertRGBarPressed = true;
	
	if(vertRGBarPressed){
		Utilities util;
		orgvalue -= yOffset/(glfwGetVideoMode(glfwGetPrimaryMonitor())->height/2);
		const float maskPanelRange = ceil(textureSize/3.f) / 8.33333333333 - (0.8f - 0.55f); 
		orgvalue = util.restrictBetween(orgvalue, 0.0f, -maskPanelRange/4.f);//Keep in boundaries
	}
	
	if(glfwGetMouseButton(window,0) == GLFW_RELEASE)
		vertRGBarPressed = false;

	//Background
	box(0.008f, height, positionX, positionY - height, "",colorData.maskPanelSliderBackgroundColor, 0.095f, false, false, 0.9f, 10000, glm::vec4(0), 0); 
	box(0.0f, 0.015f, positionX, positionY , "", colorData.maskPanelSliderBackgroundColor, 0.095f, false, false, 0.9f, 20, glm::vec4(0), 0); 
	box(0.0f, 0.015f, positionX, positionY - height*2.f, "", colorData.maskPanelSliderBackgroundColor, 0.095f, false, false, 0.9f, 20, glm::vec4(0), 0); 

	//Pointer 
	box(0.0f, 0.015f, positionX, positionY - value, "",colorData.maskPanelSliderColor, 0.095f, false, false, 0.91f, 20, glm::vec4(0), 0); //Mask panel slider

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
	if(width == height*2){
		float ratio = glfwGetVideoMode(glfwGetPrimaryMonitor())->width / glfwGetVideoMode(glfwGetPrimaryMonitor())->height;
		height = width*ratio;
	}

	GlSet glset;
	ColorData clrData;

	glset.uniform4fv(uiPrograms.iconsProgram,"iconColor",color);
	glset.uniform4fv(uiPrograms.iconsProgram,"iconColorHover",colorHover);
	
	glset.uniform1f(uiPrograms.iconsProgram,"iconMixVal",mixVal);
	glset.activeTexture(GL_TEXTURE6);
	glset.bindTexture(icon);

	glm::mat4 scale = glm::mat4(1);
	scale = glm::scale(scale,glm::vec3(width,height,1));
	glset.uniformMatrix4fv(uiPrograms.iconsProgram,"scale",scale);
	
	glm::vec3 pos = glm::vec3(position_x,position_y,z);
	glset.uniform3fv(uiPrograms.iconsProgram,"pos",pos);

	glBindBuffer(GL_ARRAY_BUFFER,uiObjects.sqrVBO);
	glBindVertexArray(uiObjects.sqrVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	ColorData colorData;

	scale = glm::mat4(1);
	pos = glm::vec3(0);
	glset.uniformMatrix4fv(uiPrograms.iconsProgram,"scale",-scale);
	glset.uniform3fv(uiPrograms.iconsProgram,"pos",pos);
	
	glBindBuffer(GL_ARRAY_BUFFER,uiObjects.VBO);
	glBindVertexArray(uiObjects.VAO);
}

bool spinnerBoxPressed = false;
void UserInterface::spinnerBox(float width, float height, float position_x, float position_y, float z,float &value,float xOffset,float yOffset,double mouseXpos,double mouseYpos,bool firstClick, GLFWwindow* window){
	GlSet glset;
	ColorData clrData;

	xOffset /= 200;
	yOffset /= -200;

	bool buttonHover = isMouseOnButton(window,width,height,position_x,position_y,mouseXpos,mouseYpos,false);
	
	if(buttonHover && firstClick)
		spinnerBoxPressed = true;
	if(glfwGetMouseButton(window,0) == GLFW_RELEASE)
		spinnerBoxPressed = false;

	if(spinnerBoxPressed){
		if(value >= 0.25 && value < 0.5){
			value += (xOffset-yOffset)/2.f;
		}
		if(value >= 0.5 && value < 0.75){
			value += -yOffset;
		}
		if(value >= 0.75 && value < 1.0){
			value += (-xOffset-yOffset)/2.f;
		}
		if(value >= 1.0 && value < 1.25){
			value += -xOffset;
		}
		if(value >= 1.25 && value < 1.5){
			value += (-xOffset+yOffset)/2.f;
		}
		if(value >= 1.5 && value < 1.75){
			value += yOffset;
		}
		if(value >= 1.75 && value <= 2.0){
			value += (xOffset+yOffset)/2.f;
		}
		if(value <= 0.25){
			value += xOffset;
		}
	}
	Utilities util;
	value = util.restrictBetween(value,2.f,0.f);


	glm::mat4 scale = glm::mat4(1);
	scale = glm::scale(scale,glm::vec3(width,-height,1));
	glset.uniformMatrix4fv(uiPrograms.spinnerProgram,"scale",scale);
	
	glm::vec3 pos = glm::vec3(position_x,position_y,z);
	glset.uniform3fv(uiPrograms.spinnerProgram,"pos",pos);
	
	glset.uniform1f(uiPrograms.spinnerProgram,"val",value);

	glBindBuffer(GL_ARRAY_BUFFER,uiObjects.sqrVBO);
	glBindVertexArray(uiObjects.sqrVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	ColorData colorData;

	scale = glm::mat4(1);
	pos = glm::vec3(0);
	glset.uniformMatrix4fv(uiPrograms.spinnerProgram,"scale",scale);
	glset.uniform3fv(uiPrograms.spinnerProgram,"pos",pos);
	
	glBindBuffer(GL_ARRAY_BUFFER,uiObjects.VBO);
	glBindVertexArray(uiObjects.VAO);
}

void UserInterface::rampBox(float width, float height, float position_x, float position_y, float z){
	GlSet glset;
	glm::mat4 scale = glm::mat4(1);
	scale = glm::scale(scale,glm::vec3(width,height,1));
	glset.uniformMatrix4fv(uiPrograms.rampProgram,"scale",scale);
	
	glm::vec3 pos = glm::vec3(position_x,position_y,z);
	glset.uniform3fv(uiPrograms.rampProgram,"pos",pos);

	glBindBuffer(GL_ARRAY_BUFFER,uiObjects.sqrVBO);
	glBindVertexArray(uiObjects.sqrVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	ColorData colorData;

	scale = glm::mat4(1);
	pos = glm::vec3(0);
	glset.uniformMatrix4fv(uiPrograms.rampProgram,"scale",scale);
	glset.uniform3fv(uiPrograms.rampProgram,"pos",pos);
	
	glBindBuffer(GL_ARRAY_BUFFER,uiObjects.VBO);
	glBindVertexArray(uiObjects.VAO);
}

void UserInterface::circle(float positionX,float positionY,float positionZ,float width, float height, unsigned int circleTexture, glm::vec4 color,glm::vec4 transitionColor, float mixVal){
	iconBox(width,height,positionX,positionY,positionZ,circleIcon,mixVal,color,transitionColor);
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
		LigidPainter lp;
		lp.setViewportToDefault();
	}

	hueShaderData.useTexCoords = 0;
	hueShaderData.renderTextureProjection = projection;
	glset.useHueShader(uiPrograms.hueProgram,hueShaderData);

	//Finish
	glset.drawArrays(boxCoor, false); //Render color rectangle displayer
	glUseProgram(uiPrograms.uiProgram);
	box(0.01f, 0.005f, position_x, position_y + value, "", colorData.colorBoxIndicatorColor, 0.045f, false, false, 0.91f, 10000, glm::vec4(0), 0); //Value indicator


	return hueValue;
}
void UserInterface::rangeBar(float position_x, float position_y,float value,float divideWidth) {
	ColorData colorData;
	GlSet gl;

	box(0, 0.02f, position_x+value/divideWidth, position_y, "", colorData.rangeBarFront, 0.035f, false, false, 0.9f, 15, glm::vec4(0), 0);//Value Square
	box(((value+0.11f)/divideWidth) / 2, 0.0075f, position_x+((value+0.11f)/divideWidth) / 2 - 0.11f/divideWidth, position_y, "", colorData.rangeBarSlide, 0.035f, false, false, 0.9f, 5, glm::vec4(0), 0);//Range Rectangle
	box(0.11f/divideWidth, 0.0075f, position_x, position_y, "", colorData.rangeBarBack, 0.035f, false, false, 0.9f, 5, glm::vec4(0), 0);//Range Rectangle

}
void UserInterface::constRangeBar(float position_x, float position_y,float value,Icons icons,float mixVal,float &lastVal,bool &increase) {
	ColorData colorData;
	GlSet gl;
	glm::vec4 leftColor;
	glm::vec4 rightColor;

	if(value < lastVal)
		increase = false;
	else if (value > lastVal)
		increase = true;

	if(!increase){
		leftColor = colorData.iconColor;
		rightColor = colorData.iconColorHover;
	}
	else{
		rightColor = colorData.iconColor;
		leftColor = colorData.iconColorHover;
	}


	if(mixVal <= 0.f){
		rightColor = colorData.iconColor;
		leftColor = colorData.iconColor;
	}

	lastVal = value;

	box(0.026f, 0.015f, position_x, position_y, "", colorData.buttonColor, 0.035f, false, false, 0.9f, 5, glm::vec4(colorData.LigidPainterThemeColor,0.8f), mixVal);//Range Rectangle

	int charSize = std::to_string((int)(value*20)).size();
	float charRat = 1.6;
	float txtRat = 0.01f;
	if(charSize == 1){
		txtRat = 0.01f;
		charRat = 1;
	}
	else if(charSize == 2){
		charRat = 1.2;
	}
	else if(charSize == 3){
		charRat = 1.4;
	}	

	renderText(uiPrograms.uiProgram,std::to_string((int)(value*20)),position_x-txtRat,position_y-0.006f,0.00022f/charRat,colorData.iconColor,0.91f,false);
	

	glUseProgram(uiPrograms.iconsProgram);
	iconBox(0.013f/2.f,0.013f,position_x-0.017f, position_y,0.92f,icons.LCircle,0.f,leftColor,colorData.buttonColor);
	iconBox(0.013f/2.f,0.013f,position_x+0.017f, position_y,0.92f,icons.RCircle,0.f,rightColor,colorData.buttonColor);
	glUseProgram(uiPrograms.uiProgram);
}
void UserInterface::richConstRangeBar(float position_x, float position_y,float value,Icons icons,float mixVal,float &lastVal,bool &increase,bool hover,std::string name) {
	ColorData colorData;
	GlSet gl;
	glm::vec4 leftColor;
	glm::vec4 rightColor;

	if(value < lastVal)
		increase = false;
	else if (value > lastVal)
		increase = true;

	if(!increase){
		leftColor = colorData.iconColor;
		rightColor = colorData.iconColorHover;
	}
	else{
		rightColor = colorData.iconColor;
		leftColor = colorData.iconColorHover;
	}


	if(mixVal <= 0.f){
		rightColor = colorData.iconColor;
		leftColor = colorData.iconColor;
	}

	if(!hover && mixVal <= 0.f){
		rightColor = glm::vec4(0);
		leftColor = glm::vec4(0);
	}
	if(hover)
		colorData.buttonColor = colorData.buttonColorHover;

	lastVal = value;

	float w = 2.5f;
	
	box(0.02f*w, 0.0175f, position_x, position_y, "", colorData.buttonColor, 0.035f, true, false, 0.9f, 10, glm::vec4(colorData.LigidPainterThemeColor,0.8f), mixVal);//Range Rectangle
	renderTextR(uiPrograms.uiProgram,name,position_x-0.025f*w,position_y-0.006f,0.00022f,colorData.textColor,0.91f,false);

	int charSize = std::to_string((int)(value*20)).size();
	float charRat = 1.6;
	float txtRat = 0.01f;
	if(charSize == 1){
		txtRat = 0.01f;
		charRat = 1;
	}
	else if(charSize == 2){
		charRat = 1.2;
	}
	else if(charSize == 3){
		charRat = 1.4;
	}	

	renderText(uiPrograms.uiProgram,std::to_string((int)(value*20)),position_x-txtRat,position_y-0.006f,0.00022f/charRat,colorData.iconColor,0.91f,false);

	glUseProgram(uiPrograms.iconsProgram);
	iconBox(0.010f/2.f,0.010f,position_x-0.017f*w, position_y,0.92f,icons.ArrowLeft,0.f,leftColor,colorData.buttonColor);
	iconBox(0.010f/2.f,0.010f,position_x+0.017f*w, position_y,0.92f,icons.ArrowRight,0.f,rightColor,colorData.buttonColor);
	glUseProgram(uiPrograms.uiProgram);
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

void UserInterface::checkBox(float position_x, float position_y, std::string text, bool mouseHover,bool checked,unsigned int circleTxtr,Icons icons,float mixVal) {
	ColorData colorData;
	glm::vec4 color1 = colorData.checkBoxColor;
	glm::vec4 color2 = colorData.checkBoxCheckedColor;


	if (mouseHover){
		color1 = color1/glm::vec4(1.25f);
		color2 =  color2*glm::vec4(1.25f);
	}
	
	glUseProgram(uiPrograms.iconsProgram);
	iconBox(0.05f/3.f,0.05f*2/3.f,position_x, position_y,0.9f,icons.O,0,color1,color1); //Outline
	
	iconBox(0.022f/3.f,0.022f*2/3.f,position_x, position_y,0.91f,icons.Circle,0,glm::vec4(color2.r,color2.g,color2.b,mixVal),color2); //Circle
	
	glUseProgram(uiPrograms.uiProgram);
	renderText(uiPrograms.uiProgram, text, position_x+0.03f, position_y - 0.01f, 0.00022f,color1,0.9f,false);
}

void UserInterface::checkBox(float position_x, float position_y, std::string text, bool mouseHover,bool checked,Icons icons,glm::vec4 color1,glm::vec4 color2,float mixVal) {
	ColorData colorData;

	glm::vec4 color;

	if (mouseHover){
		color1 = color1/glm::vec4(1.5f);
		color2 =  color2*glm::vec4(2.f);
	}
	
	glUseProgram(uiPrograms.iconsProgram);
	iconBox(0.05f/1.5,0.05f,position_x, position_y,0.9f,icons.O,0,color1,color1); //Outline
	
	iconBox(0.025f/1.5,0.025f,position_x, position_y,0.91f,icons.Circle,0,glm::vec4(color2.r,color2.g,color2.b,mixVal),color2); //Circle
	
	glUseProgram(uiPrograms.uiProgram);
	renderText(uiPrograms.uiProgram, text, position_x+0.03f, position_y - 0.01f, 0.00032f,glm::vec4(0,0,0,1),0.9f,false);
}


std::map<char, character> characters;
int textCursorPhaseCounter = 0;

float lastXText = 0;

void UserInterface::renderText(unsigned int program, std::string text, float x, float y, float scale,glm::vec4 color,float z,bool active) {
	lastXText = x;
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
	glset.uniform1f(program, "uiTransitionMixVal", 0.f);
	
	std::string::const_iterator c;
	int counter = 0;
	for (c = text.begin(); c != text.end(); c++)
	{
		character ch = characters[*c];
		if(*c == '\n'){
			x=lastXText;
			y-=0.03f;
		}
		else{
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
void UserInterface::renderText(unsigned int program, std::string text, float x, float y, float scale,glm::vec4 color,float z,bool active,float maxX,bool multipleLines,float mixVal) {
	lastXText = x;
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
	glset.uniform1f(program, "uiTransitionMixVal", mixVal);
	
	std::string::const_iterator c;
	int counter = 0;
	for (c = text.begin(); c != text.end(); c++)
	{
		character ch = characters[*c];
		if(*c == '\n'){
			x=lastXText;
			y-=0.03f;
		}
		else{
			if(maxX < x){
				x=lastXText;
				y-=(ch.Size.y) * scale * 1.3f;
				if(!multipleLines)
					break;
			}

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
void UserInterface::renderText(unsigned int program, std::string text, float x, float y, float scale,glm::vec4 color,float z,bool active,Font font) {
	lastXText = x;
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
	glset.uniform1f(program, "uiTransitionMixVal", 0.f);
	

	std::string::const_iterator c;
	int counter = 0;
	for (c = text.begin(); c != text.end(); c++)
	{
		character ch = font.characters[*c];
		if(*c == '\n'){
			x=lastXText;
			y-=0.03f;
		}
		else{
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
void UserInterface::renderText(unsigned int program, std::string text, float x, float y, float scale,glm::vec4 color,float z,bool active,Font font,float maxX) {
	lastXText = x;
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
	glset.uniform1f(program, "uiTransitionMixVal", 0.f);
	
	std::string::const_iterator c;
	int counter = 0;
	for (c = text.begin(); c != text.end(); c++)
	{
		character ch = font.characters[*c];
		if(*c == '\n'){
			x=lastXText;
			y-=(ch.Size.y) * scale;
		}
		else{
			if(maxX < x){
				x=lastXText;
				y-=(ch.Size.y) * scale * 1.2f;
			}
			
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

void UserInterface::renderTextM(unsigned int program, std::string text, float x, float y, float scale,glm::vec4 color,float z,bool active,Font font) {
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
	glset.uniform1f(program, "uiTransitionMixVal", 0.f);
	
	std::string::const_iterator c;
	int counter = 0;

	float totalX = x;

	for (c = text.begin(); c != text.end(); c++){
		character ch = characters[*c];
		totalX += (ch.Advance >> 6) * scale / 1.2f;
	}

	x -= totalX/2.f;
	
	for (c = text.begin(); c != text.end(); c++)
	{
		character ch = font.characters[*c];

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
void UserInterface::renderTextM(unsigned int program, std::string text, float x, float y, float scale,glm::vec4 color,float z,bool active,float maxX,bool multipleLines) {
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
	glset.uniform1f(program, "uiTransitionMixVal", 0.f);
	

	std::string::const_iterator c;
	int counter = 0;

	float totalX = x;

	for (c = text.begin(); c != text.end(); c++){
		character ch = characters[*c];
		totalX += (ch.Advance >> 6) * scale / 1.2f;
		if(maxX < totalX)
			break;
	}

	x -= totalX/2.f;
	
	for (c = text.begin(); c != text.end(); c++)
	{
		character ch = characters[*c];
		if(maxX < x){
			x=lastXText;
			y-=(ch.Size.y) * scale * 1.3f;
			if(!multipleLines)
				break;
		}

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

void UserInterface::renderTextR(unsigned int program, std::string text, float x, float y, float scale,glm::vec4 color,float z,bool active) {
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
	glset.uniform1f(program, "uiTransitionMixVal", 0.f);

	
	int c;
	int counter = 0;
	for (c = text.size()-1; c != -1; c--)
	{
		
		character ch = characters[text[c]];

		float xpos = x - ch.Bearing.x * scale;
		float ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

		float w = ch.Size.x * scale * 0.8f;
		float h = ch.Size.y * scale;

		glm::mat4 scalemat = glm::mat4(1);
		scalemat = glm::scale(scalemat,glm::vec3(w/1.7,h/1.7,1));
		glset.uniformMatrix4fv(uiPrograms.uiProgram,"scale",scalemat);
		
		glm::vec3 pos = glm::vec3(xpos - w/1.7,ypos + h/1.7,z);
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
				pos = glm::vec3(xpos - w/1.7*2.,y+0.01,z);
				glset.uniform3fv(uiPrograms.uiProgram,"pos",pos);
				glDrawArrays(GL_TRIANGLES, 0, 6);
				
				glset.uniform1i(program, "isTextF", 1);
				glset.uniform1i(program,"isText", 1);
				glset.uniform4fv(program, "uiColor", color);
			}
		}

		x -= (ch.Advance >> 6) * scale / 1.2f; 
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
void UserInterface::renderTextR(unsigned int program, std::string text, float x, float y, float scale,glm::vec4 color,float z,bool active,Font font) {
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
	glset.uniform1f(program, "uiTransitionMixVal", 0.f);

	
	int c;
	int counter = 0;
	for (c = text.size()-1; c != -1; c--)
	{
		
		character ch = font.characters[text[c]];

		float xpos = x - ch.Bearing.x * scale;
		float ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

		float w = ch.Size.x * scale * 0.8f;
		float h = ch.Size.y * scale;

		glm::mat4 scalemat = glm::mat4(1);
		scalemat = glm::scale(scalemat,glm::vec3(w/1.7,h/1.7,1));
		glset.uniformMatrix4fv(uiPrograms.uiProgram,"scale",scalemat);
		
		glm::vec3 pos = glm::vec3(xpos - w/1.7,ypos + h/1.7,z);
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
				pos = glm::vec3(xpos - w/1.7*2.,y+0.01,z);
				glset.uniform3fv(uiPrograms.uiProgram,"pos",pos);
				glDrawArrays(GL_TRIANGLES, 0, 6);
				
				glset.uniform1i(program, "isTextF", 1);
				glset.uniform1i(program,"isText", 1);
				glset.uniform4fv(program, "uiColor", color);
			}
		}

		x -= (ch.Advance >> 6) * scale / 1.2f; 
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




void UserInterface::container(float positionX,float positionY,float positionZ,float width, float height,glm::vec4 color,Programs &programs,unsigned int circleTexture,glm::vec4 transitionColor,float mixVal){
	
	glUseProgram(programs.uiProgram);
	box(width,height,positionX,positionY,"", color,0,0,0,positionZ,10000,transitionColor,mixVal);
	
	box(0.03f,height,positionX - width + 0.006f,positionY,"", color,0,0,0,positionZ,10000,transitionColor,mixVal); //Left
	box(0.03f,height,positionX + width - 0.006f,positionY,"", color,0,0,0,positionZ,10000,transitionColor,mixVal); //Right
	box(width,0.06f,positionX,positionY + height - 0.012 ,"", color,0,0,0,positionZ,10000,transitionColor,mixVal); //Top
	box(width,0.06f,positionX,positionY - height + 0.012 ,"", color,0,0,0,positionZ,10000,transitionColor,mixVal); //Bottom

	glUseProgram(programs.iconsProgram);
	circle(positionX - width,positionY + height,positionZ,0.03f,0.06f,circleTexture,color,transitionColor,mixVal);//Left top
	circle(positionX - width,positionY - height,positionZ,0.03f,0.06f,circleTexture,color,transitionColor,mixVal);//Left bot
	circle(positionX + width,positionY + height,positionZ,0.03f,0.06f,circleTexture,color,transitionColor,mixVal);//Right top
	circle(positionX + width,positionY - height,positionZ,0.03f,0.06f,circleTexture,color,transitionColor,mixVal);//Right bot
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
	gl.uniform1i(uiPrograms.uiProgram,"outline",0);
	gl.uniform1f(uiPrograms.uiProgram,"radius",1000.f);
	gl.uniform1f(uiPrograms.uiProgram,"uiTransitionMixVal",0.f);
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
	
	circle(nodePanelLeft+0.001f,nodePanelTop-0.001f,nodePanelZ,0.05f,0.1f,icons.Circle,colorData.nodePanelColorSnd,glm::vec4(0),0); //Left
	circle(nodePanelRight-0.001f,nodePanelTop-0.001f,nodePanelZ,0.05f,0.1f,icons.Circle,colorData.nodePanelColorSnd,glm::vec4(0),0); //Right
	
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
	gl.uniform1f(uiPrograms.uiProgram,"uiTransitionMixVal",0.f);
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
void UserInterface::sendMaxWindowSize(int removeThisParam,int maxScreenHeight){
	sendMaxWindowSizeToCalculationsAndMore(glfwGetVideoMode(glfwGetPrimaryMonitor())->width,maxScreenHeight);
}

bool UserInterface::coloringPanel(ColoringPanel &coloringPanel,Programs programs,Icons icons,GLFWwindow* window,SaturationValShaderData saturationValShaderData,glm::mat4 orthoProjection,double mouseXpos,double mouseYpos,bool &firstClick,float xOffset,float yOffset,unsigned int FBOscreen,ColorPicker &colorPicker,float screenGapX,glm::vec3 screenHoverPixel,std::vector<UIElement> &UIElements,bool modifyBrush){
	const float depth = 0.8f;

	const float panelWidth = 0.2f;
	const float panelHeigth = 0.2f;
	
	ColorData colorData;
	GlSet glset;
	Utilities util;

	bool brushChanged = false;

	container(coloringPanel.panelPosX,coloringPanel.panelPosY,depth,panelWidth,panelHeigth,colorData.panelColor,programs,icons.Circle,glm::vec4(0),0);
	
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
	coloringPanel.result = render.getColorPickerValue(FBOscreen,cp,screenSizeX,screenSizeY,programs,glfwGetVideoMode(glfwGetPrimaryMonitor())->width,glfwGetVideoMode(glfwGetPrimaryMonitor())->height,saturationValShaderData);
	
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
	
	glUseProgram(programs.uiProgram);
	//BRUSH
	if(modifyBrush){
		rangeBar(coloringPanel.panelPosX + 0.125f, coloringPanel.panelPosY, UIElements[UIbrushSizeRangeBar].rangeBar.value,2.f);
		if(isMouseOnButton(window,0.015f/2.f,0.015f,coloringPanel.panelPosX + 0.125f - screenGapX + UIElements[UIbrushSizeRangeBar].rangeBar.value/2.f, coloringPanel.panelPosY,mouseXpos,mouseYpos,false)){
			if(firstClick)
				coloringPanel.brushSizeRangeBarPressed = true;	
		}

		rangeBar(coloringPanel.panelPosX + 0.125f, coloringPanel.panelPosY-0.05f, UIElements[UIbrushBlurRangeBar].rangeBar.value,2.f);
		if(isMouseOnButton(window,0.015f/2.f,0.015f,coloringPanel.panelPosX + 0.125f - screenGapX + UIElements[UIbrushBlurRangeBar].rangeBar.value/2.f, coloringPanel.panelPosY-0.05f,mouseXpos,mouseYpos,false)){
			if(firstClick)
				coloringPanel.brushBlurRangeBarPressed = true;	
		}

		if(glfwGetMouseButton(window,0) == GLFW_RELEASE){
			coloringPanel.brushSizeRangeBarPressed = false;	
			coloringPanel.brushBlurRangeBarPressed = false;	
		}

		if(coloringPanel.brushSizeRangeBarPressed){
			UIElements[UIbrushSizeRangeBar].rangeBar.value += (xOffset / (screenSizeX / 2)) * 2;
			UIElements[UIbrushSizeRangeBar].rangeBar.value = util.restrictBetween(UIElements[UIbrushSizeRangeBar].rangeBar.value, 0.11f, -0.11f);//Keep in boundaries
		}
		if(coloringPanel.brushBlurRangeBarPressed){
			brushChanged = true;
			UIElements[UIbrushBlurRangeBar].rangeBar.value += (xOffset / (screenSizeX / 2)) * 2;
			UIElements[UIbrushBlurRangeBar].rangeBar.value = util.restrictBetween(UIElements[UIbrushBlurRangeBar].rangeBar.value, 0.11f, -0.11f);//Keep in boundaries
		}
		glUseProgram(programs.dynamicPaintingProgram);
		GlSet glset;
		glset.uniform1f(programs.dynamicPaintingProgram,"radius",(UIElements[UIbrushSizeRangeBar].rangeBar.value+0.11f)*1000);
		glset.uniform1f(programs.dynamicPaintingProgram,"hardness",(UIElements[UIbrushBlurRangeBar].rangeBar.value-0.09f)*4.5454545*50);
	}

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
		coloringPanel.saturationValueBoxPosX += xOffset/glfwGetVideoMode(glfwGetPrimaryMonitor())->width*2;
		coloringPanel.saturationValueBoxPosX = util.restrictBetween(coloringPanel.saturationValueBoxPosX, 0.099f, -0.1f);//Keep in boundaries
		coloringPanel.saturationValueBoxPosY -= yOffset/glfwGetVideoMode(glfwGetPrimaryMonitor())->height*2;
		coloringPanel.saturationValueBoxPosY = util.restrictBetween(coloringPanel.saturationValueBoxPosY, 0.199f, -0.2f);//Keep in boundaries
		coloringPanel.pickerValueChanged = true;
	}


	if(coloringPanel.hueBarPointerHover && firstClick)
		coloringPanel.hueBarPointerPressed = true;

	if(coloringPanel.hueBarPointerPressed){
		coloringPanel.hueBarPosX -= yOffset/(glfwGetVideoMode(glfwGetPrimaryMonitor())->height/2);
		coloringPanel.hueBarPosX = util.restrictBetween(coloringPanel.hueBarPosX, 0.180f, -0.180f);//Keep in boundaries
		coloringPanel.pickerValueChanged = true;
	}

	glUseProgram(programs.uiProgram); 

	return brushChanged;
}
void UserInterface::textureCreatingPanel(TextureCreatingPanel &txtrCreatingPanel,Icons icons,Programs programs,GLFWwindow* window,double mouseXpos,double mouseYpos,bool &firstClick,
										ColoringPanel &coloringPanel,float screenGapX,std::vector<aTexture> &albedoTextures,int& activeFolderIndex,int chosenTextureResIndex){
	ColorData colorData;
	Utilities util;
	GlSet glset;

	const float depth = 0.51f;

	const float panelWidth = 0.15f;
	const float panelHeigth = 0.12f;

	container(txtrCreatingPanel.panelPosX,txtrCreatingPanel.panelPosY,depth,panelWidth,panelHeigth,colorData.panelColor,programs,icons.Circle,glm::vec4(0),0);
	txtrCreatingPanel.panelHover = isMouseOnButton(window,panelWidth+0.03f,panelHeigth+0.06f,txtrCreatingPanel.panelPosX-screenGapX,txtrCreatingPanel.panelPosY,mouseXpos,mouseYpos,false);
	
	if(!txtrCreatingPanel.panelHover && !coloringPanel.panelHover && !coloringPanel.active){ 
		txtrCreatingPanel.color = glm::vec3(0);
		txtrCreatingPanel.active = false;
		txtrCreatingPanel.textBoxVal = "txtr";
	}

	glUseProgram(programs.uiProgram);
	
	renderText(programs.uiProgram,"Title :",txtrCreatingPanel.panelPosX - panelWidth,txtrCreatingPanel.panelPosY+0.12f,0.00022f,colorData.textColor,depth+0.01,false);
	box(panelWidth - 0.02f, 0.03f, txtrCreatingPanel.panelPosX,txtrCreatingPanel.panelPosY+0.08f, txtrCreatingPanel.textBoxVal, colorData.textColor, 0, true, false, depth+0.01f, 10, colorData.textBoxColorClicked, (float)txtrCreatingPanel.textBoxActive,colorData.textColor,colorData.textColor,0.00022f,true);
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
			
			int txtrRes = 256;
			for (size_t i = 0; i < chosenTextureResIndex; i++)
			{
				txtrRes*=2;
			}
			
			aTexture txtr;
			txtr.folderIndex = activeFolderIndex;
			glActiveTexture(GL_TEXTURE28);

			unsigned int texture;
			glset.genTextures(texture);
			glset.bindTexture(texture);
			glset.texImage(nullptr,txtrRes,txtrRes,GL_RGBA);
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
			txtr.width = txtrRes;
			txtr.height = txtrRes;
			txtr.name = txtrCreatingPanel.textBoxVal;

			albedoTextures.push_back(txtr);

			txtrCreatingPanel.textBoxVal = "txtr";
			txtrCreatingPanel.color = glm::vec3(0);
			txtrCreatingPanel.active = false;
		}
		else{
			alert("Warning! Title can not be empty.",200,false);
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

void UserInterface::modelMaterialPanel(Model &model,Programs programs,RenderData renderData,float screenGapX,float materialsPanelSlideValue,double mouseXpos,
double mouseYpos,RenderOutData& uiOut,int& currentMaterialIndex,bool &firstClick,bool& newModelAdded,
int &selectedNodeScene,Icons icons,std::vector<NodeScene> nodeScenes,std::vector<aTexture> &albedoTextures,TextureSelectionPanel &textureSelectionPanel){
	ColorData colorData;

	
	int posI = 0;
	bool mouseEnteredOnce = false;
		glUseProgram(programs.uiProgram); 

		if(model.meshes.size() == 0){
			renderText(programs.uiProgram, "Materials of the 3D model", renderData.panelLoc - 1.0f - screenGapX  + 0.095f, 0.8f, 0.00022f,colorData.textColor,0.99999f,false);
			renderText(programs.uiProgram, "will be show up there", renderData.panelLoc - 1.0f - screenGapX  + 0.115f, 0.75f, 0.00022f,colorData.textColor,0.99999f,false);
		}
		for (int i = 0; i < model.meshes.size(); i++)//Render buttons
		{ 	
			//Check if mouse is entered the related button
			bool textureButtonEnter = isMouseOnButton(renderData.window, 0.2f, 0.06f, renderData.panelLoc - 1.0f - screenGapX*2 + 0.205f,0.8f - (posI * 0.125f) + materialsPanelSlideValue, mouseXpos, mouseYpos,false);
			bool textureAddButtonEnter = isMouseOnButton(renderData.window, 0.02f, 0.03f, renderData.panelLoc - 1.0f - screenGapX*2 + 0.3f,0.8f - (posI * 0.125f) + materialsPanelSlideValue, mouseXpos, mouseYpos,false);
			Utilities util;
			model.meshes[i].modelMaterialButtonMixVal = util.transitionEffect(textureButtonEnter,model.meshes[i].modelMaterialButtonMixVal,0.1f);
				//Hover
				if(!textureAddButtonEnter){
					mouseEnteredOnce = true;
				}
				if(textureButtonEnter)
					uiOut.texturePanelButtonHover = true;
				if(glfwGetMouseButton(renderData.window, 0) == GLFW_PRESS && textureButtonEnter){
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
				//Button 
				box(0.2f, 0.06f, renderData.panelLoc - 1.0f - screenGapX + 0.205f, 0.8f - (posI * 0.125f) + materialsPanelSlideValue, model.meshes[i].materialName, currentColor, 0.2f, false, false, 0.9f, 10000, currentColorHover, model.meshes[i].modelMaterialButtonMixVal);

						if(textureAddButtonEnter){
				//Hover
				//uiOut.texturePanelButtonHover = true;
				if(firstClick){
					//Pressed
					//uiOut.texturePanelButtonClicked = true;
					
					//currentMaterialIndex = i;
					//newModelAdded = true; 
					//model.meshes[i].materialIndex = selectedNodeScene;
					if(model.meshes[i].submeshes.size() == 6){
						alert("Submesh count can be maximum 6",200,false);						
					}
					else{
						SubMeshMs submesh;
						model.meshes[i].submeshes.push_back(submesh);
					}
				}
			}
			//box(0.03f, 0.06f, renderData.panelLoc - screenGapX + 0.3f, 0.8f - (posI * 0.125f), "", colorData.buttonColorHover, 0.048f, true, false, 0.6f, 10000, colorData.buttonColorHover, 0); 
			
			glUseProgram(programs.iconsProgram);
			float iconmixVal = 0.0f;
			if(textureAddButtonEnter)
				iconmixVal = 0.5f;
				
			iconBox(0.015f,0.027f,renderData.panelLoc - 1.0f - screenGapX + 0.3f ,0.8f - (posI * 0.125f) + materialsPanelSlideValue,0.92f,icons.Plus,iconmixVal,colorData.iconColor,colorData.iconColorHover);
			glUseProgram(programs.uiProgram); 
			posI++;
			for (int sI = model.meshes[i].submeshes.size()-1; sI >= 0 ; sI--)
			{
				GlSet glset;
				//TODO : Default mask texture for each submesh
				bool submeshButtonHover = false;
				if(isMouseOnButton(renderData.window,0.2f,0.06f,renderData.panelLoc - 1.0f - screenGapX*2.f + 0.205f,0.8f - ((posI) * 0.125f) + materialsPanelSlideValue,mouseXpos,mouseYpos,false)){
					submeshButtonHover = true;
				}

				model.meshes[i].submeshes[sI].modelMaterialButtonMixVal = util.transitionEffect(submeshButtonHover,model.meshes[i].submeshes[sI].modelMaterialButtonMixVal,0.1f);


				box(0.2f, 0.06f, renderData.panelLoc - 1.0f - screenGapX + 0.205f, 0.8f - ((posI) * 0.125f) + materialsPanelSlideValue, "submesh_" + std::to_string(sI), colorData.buttonColor, 0.2f, false, false, 0.9f, 10000,  colorData.buttonColorHover, model.meshes[i].submeshes[sI].modelMaterialButtonMixVal);
				
				box(0.001f, 0.04f, renderData.panelLoc - 1.0f - screenGapX + 0.275f, 0.8f - ((posI) * 0.125f) + materialsPanelSlideValue, "", glm::vec4(colorData.iconColor.r,colorData.iconColor.g,colorData.iconColor.b,0.7), 0.2f, false, false, 0.91f, 10000,  colorData.buttonColorHover, model.meshes[i].submeshes[sI].modelMaterialButtonMixVal);
				box(0.001f, 0.04f, renderData.panelLoc - 1.0f - screenGapX + 0.195f, 0.8f - ((posI) * 0.125f) + materialsPanelSlideValue, "", glm::vec4(colorData.iconColor.r,colorData.iconColor.g,colorData.iconColor.b,0.7), 0.2f, false, false, 0.91f, 10000,  colorData.buttonColorHover, model.meshes[i].submeshes[sI].modelMaterialButtonMixVal);
				
				renderText(programs.uiProgram,std::to_string(model.meshes[i].submeshes[sI].materialIndex),renderData.panelLoc - 1.0f - screenGapX + 0.22f,0.77f - ((posI) * 0.125f) + materialsPanelSlideValue,0.00022f,colorData.textColor,0.92f,false);

				glUseProgram(programs.renderTheTextureProgram);
				glset.uniform1i(programs.renderTheTextureProgram,"isPressed",1);
				glset.uniform1i(programs.renderTheTextureProgram,"isHover",0);
				glset.uniform1i(programs.renderTheTextureProgram,"subSelected",0);
				glActiveTexture(GL_TEXTURE14);
				glset.bindTexture(model.meshes[i].submeshes[sI].maskTexture);

				float maskW = 0.02f;
				glm::vec2 maskPos = glm::vec2(renderData.panelLoc - 1.0f - screenGapX + 0.35f ,0.8f - ((posI) * 0.125f) + materialsPanelSlideValue);
				std::vector<float> maskVertices = { 
					// first triangle
		 			maskW+maskPos.x,  maskW*2.f+maskPos.y, 0.91f,1,1,0,0,0,  // top right
		 			maskW+maskPos.x,  -maskW*2.f+maskPos.y, 0.91f,1,0,0,0,0,  // bottom right
		 			-maskW+maskPos.x,  maskW*2.f+maskPos.y, 0.91f,0,1,0,0,0,  // top left 
					maskW+maskPos.x,  -maskW*2.f+maskPos.y, 0.91f,1,0,0,0,0,  // bottom right
		 			-maskW+maskPos.x,  -maskW*2.f+maskPos.y, 0.91f,0,0,0,0,0,  // bottom left
		 			-maskW+maskPos.x,  maskW*2.f+maskPos.y, 0.91f,0,1,0,0,0   // top left
				};
				glset.drawArrays(maskVertices,false);

				if(!textureSelectionPanel.active){
					model.meshes[i].submeshes[sI].textureSelectionState = false;
				}

				if(textureSelectionPanel.active && textureSelectionPanel.textureClicked && model.meshes[i].submeshes[sI].textureSelectionState){
					model.meshes[i].submeshes[sI].maskTexture = albedoTextures[textureSelectionPanel.selectedIndex].id;
					textureSelectionPanel.active = false; 
					textureSelectionPanel.textureClicked = false;
					model.meshes[i].submeshes[sI].textureSelectionState = false;
				}

				glUseProgram(programs.iconsProgram);
				bool addMaskTxtrIconHover = false;
				if(isMouseOnButton(renderData.window,0.015f,0.03f,renderData.panelLoc - 1.0f - screenGapX*2.f + 0.305f,0.8f - ((posI) * 0.125f) + materialsPanelSlideValue,mouseXpos,mouseYpos,false)){
					addMaskTxtrIconHover = true;
					if(firstClick){
						model.meshes[i].submeshes[sI].textureSelectionState = true;
						textureSelectionPanel.active = true;
						textureSelectionPanel.posX = renderData.panelLoc - 1.0f - screenGapX + 0.07f;
						textureSelectionPanel.posY = 0.7f - ((posI) * 0.125f) + materialsPanelSlideValue;
					}
				}
				iconBox(0.01f,0.02f,renderData.panelLoc - 1.0f - screenGapX + 0.305f,0.8f - ((posI) * 0.125f) + materialsPanelSlideValue,0.91f,icons.AddTexture,addMaskTxtrIconHover,colorData.iconColor,colorData.iconColorHover);
				
				
				bool addMaterialIconHover = false;
				if(isMouseOnButton(renderData.window,0.015f,0.03f,renderData.panelLoc - 1.0f - screenGapX*2.f + 0.25f,0.8f - ((posI) * 0.125f) + materialsPanelSlideValue,mouseXpos,mouseYpos,false)){
					addMaterialIconHover = true;
					if(firstClick){
						model.meshes[i].submeshes[sI].materialIndex = selectedNodeScene;
					}
				}
				iconBox(0.012f,0.024f,renderData.panelLoc - 1.0f - screenGapX + 0.215f,0.8f - ((posI) * 0.125f) + materialsPanelSlideValue,0.91f,icons.Material,0,colorData.iconColor,colorData.iconColorHover);
				iconBox(0.01f,0.02f,renderData.panelLoc - 1.0f - screenGapX + 0.25f,0.8f - ((posI) * 0.125f) + materialsPanelSlideValue,0.91f,icons.ArrowDown,addMaterialIconHover,colorData.iconColor,colorData.iconColorHover);
				
				
				bool deleteSubmeshIconHover = false;
				if(isMouseOnButton(renderData.window,0.015f,0.03f,renderData.panelLoc - 1.0f - screenGapX*2.f + 0.17f,0.8f - ((posI) * 0.125f) + materialsPanelSlideValue,mouseXpos,mouseYpos,false)){
					deleteSubmeshIconHover = true;
					if(firstClick){
						if(sI != 0){
							//Delete the submesh
							model.meshes[i].submeshes.erase(model.meshes[i].submeshes.begin()+sI);
						}
						else{
							alert("First submesh can't be deleted",200,false);
						}
					}
				}
				iconBox(0.01f,0.02f,renderData.panelLoc - 1.0f - screenGapX + 0.17f,0.8f - ((posI) * 0.125f) + materialsPanelSlideValue,0.91f,icons.Minus,deleteSubmeshIconHover,colorData.iconColor,colorData.iconColorHover);
				
				
				glUseProgram(programs.uiProgram);
				posI++;
			}
		}
}

int lastBrushMaskTexturePanelState = 0;
int state1Index = 0;
int state2Index = 0;
int state3Index = 0;
void UserInterface::brushMaskTexturePanel(Programs programs,std::vector<aTexture> &brushmasktextures,float centerCoords, float screenGapX,float &maskPanelSliderValue,unsigned int &currentBrushMaskTexture,bool &firstClick,GLFWwindow* window,double mouseXpos,double mouseYpos,unsigned int FBOScreen,PanelData &panelData,int screenSizeX,int screenSizeY,RenderOutData& uiOut,std::vector<UIElement> &UIElements,float brushBlurVal, OutShaderData outShaderData,float posY,int state,TextureSelectionPanel txtrSelectionPanel){
	ColorData colorData;
	GlSet gl;
	Texture txtr;
		glUseProgram(uiPrograms.renderTheTextureProgram);
		gl.uniform1i(uiPrograms.renderTheTextureProgram,"isMask",true);
		gl.uniform1i(uiPrograms.renderTheTextureProgram,"maskUseColor",state != 0);

		float maskXpos = 0.0f;
		float maskYpos = 0.0f;


		

		unsigned int masktxtrprogram;
			masktxtrprogram = programs.renderTheTextureProgram;

		glUseProgram(masktxtrprogram); 
		

		if(state != lastBrushMaskTexturePanelState){
			//Update mask texture after changing state
			int selectedMaskIndex = 0;
			if(state == 0)
				selectedMaskIndex = state1Index;
			if(state == 1)
				selectedMaskIndex = state2Index;
			if(state == 2)
				selectedMaskIndex = state3Index;
			
			Utilities util;
			UIElements[UImaskTextureFileNameText].text.text = util.cropString(util.getLastWordBySeparatingWithChar(brushmasktextures[selectedMaskIndex].name,'/'),20);
			
			gl.activeTexture(GL_TEXTURE1);
			gl.bindTexture(brushmasktextures[selectedMaskIndex].id);
			txtr.updateMaskTexture(FBOScreen,screenSizeX,screenSizeY,UIElements[UIbrushRotationRangeBar].rangeBar.value,false,UIElements[UIbrushBordersRangeBar].rangeBar.value,brushBlurVal,outShaderData,programs,glfwGetVideoMode(glfwGetPrimaryMonitor())->width,glfwGetVideoMode(glfwGetPrimaryMonitor())->height);
			glUseProgram(masktxtrprogram); 
			
			uiOut.maskPanelMaskClicked = true;
			currentBrushMaskTexture = brushmasktextures[selectedMaskIndex].id;
		}

		lastBrushMaskTexturePanelState = state;

		
		for (size_t i = 0; i < brushmasktextures.size(); i++)
		{
			if(i % 3 == 0 && i != 0){
				maskYpos-=0.15f;
				maskXpos=0.0f;
			}
			maskXpos-=0.08f;
			float position_x = centerCoords - screenGapX - maskXpos - 0.175f;
			float position_y = posY+ maskYpos - maskPanelSliderValue*(brushmasktextures.size()/4) - 0.05f;

			const float panelRange = 0.25f;

			//ui.iconBox(0.025f, 0.05f,centerCoords - screenGapX - maskXpos - 0.2f,0.8f + maskYpos - maskPanelSliderValue*(maskTextures.size()/4) - 0.05f,1,maskTextures[i],0);
			float upBotDifMin = std::min(0.05f + position_y,posY) - std::min(-0.05f + position_y,posY);
			float upBotDifMax = std::max(0.05f + position_y,posY-panelRange) - std::max(-0.05f + position_y,posY-panelRange);
			std::vector<float> buttonCoorSq{
				// first trianglev 
				 0.03f + position_x,  std::min(std::max(0.06f + position_y,posY-panelRange),posY), 0.9f,1,upBotDifMin*10,0,0,0,  // top right
				 0.03f + position_x,  std::min(std::max(-0.06f + position_y,posY-panelRange),posY), 0.9f,1,1.0f-upBotDifMax*10,0,0,0,  // bottom right
				-0.03f + position_x,  std::min(std::max(0.06f + position_y,posY-panelRange),posY), 0.9f,0,upBotDifMin*10,0,0,0,  // top left 
				// second triangle						   
				 0.03f + position_x,  std::min(std::max(-0.06f + position_y,posY-panelRange),posY), 0.9f,1,1.0f-upBotDifMax*10,0,0,0,  // bottom right
				-0.03f + position_x,  std::min(std::max(-0.06f + position_y,posY-panelRange),posY), 0.9f,0,1.0f-upBotDifMax*10,0,0,0,  // bottom left
				-0.03f + position_x,  std::min(std::max(0.06f + position_y,posY-panelRange),posY), 0.9f,0,upBotDifMin*10,0,0,0  // top left
			};

			if(std::min(std::max(0.06f + position_y,posY-panelRange),posY) != std::min(std::max(-0.06f + position_y,posY-panelRange),posY)){//Prevent rendering all the textures
				//gl.uniform1i(programs.iconsProgram,"isMaskIcon",1);
				if(brushmasktextures[i].id == currentBrushMaskTexture){
					glm::vec4 chosenBrushMaskTextureColor = glm::vec4(colorData.chosenBrushMaskTextureColor,1.0f);
					gl.uniform1i(masktxtrprogram,"isPressed",1);
				}
				else{
					gl.uniform1i(masktxtrprogram,"isPressed",0);
				//	gl.uniform4fv(programs.iconsProgram,"iconColor",colorData.brushMaskIconColor);
				}
				//gl.uniform1f(programs.iconsProgram,"iconMixVal",0);
					glUseProgram(masktxtrprogram); 
				
				//if(state == 0)
				//	gl.activeTexture(GL_TEXTURE6);
					gl.activeTexture(GL_TEXTURE14);
			
				gl.uniform1i(uiPrograms.renderTheTextureProgram, "txtr" ,14);

				gl.bindTexture(brushmasktextures[i].id);
				gl.drawArrays(buttonCoorSq,false);
				//gl.uniform1i(programs.iconsProgram,"isMaskIcon",0);
			}

			
			if(isMouseOnCoords(window,mouseXpos+screenGapX*(glfwGetVideoMode(glfwGetPrimaryMonitor())->width/2),mouseYpos,buttonCoorSq,panelData.movePanel) && !txtrSelectionPanel.active){
				if(firstClick){
					Utilities util;
					UIElements[UImaskTextureFileNameText].text.text = util.cropString(util.getLastWordBySeparatingWithChar(brushmasktextures[i].name,'/'),20);
					
					gl.activeTexture(GL_TEXTURE1);
					gl.bindTexture(brushmasktextures[i].id);
					txtr.updateMaskTexture(FBOScreen,screenSizeX,screenSizeY,UIElements[UIbrushRotationRangeBar].rangeBar.value,false,UIElements[UIbrushBordersRangeBar].rangeBar.value,brushBlurVal,outShaderData,programs,glfwGetVideoMode(glfwGetPrimaryMonitor())->width,glfwGetVideoMode(glfwGetPrimaryMonitor())->height);
					glUseProgram(masktxtrprogram); 
					
					uiOut.maskPanelMaskClicked = true;
					currentBrushMaskTexture = brushmasktextures[i].id;
					if(state == 0)
						state1Index = i;
					if(state == 1)
						state2Index = i;
					if(state == 2)
						state3Index = i;
				}
				else{
					uiOut.maskPanelMaskClicked = false;
				}
				uiOut.maskPanelMaskHover = true;
			}
		}

	gl.uniform1i(uiPrograms.renderTheTextureProgram,"isMask",false);
}

bool UserInterface::listBox(float posX,float posY,float posZ,const char* title,float width,Icons icons,std::vector<const char*> list, bool active,GLFWwindow* window, float mouseXpos,float mouseYpos,bool &firstClick,int &chosenIndex,float screenGapX){
	ColorData colorData;
	bool stateChanged = false;
	if(active){
		const float contHeight = list.size()/27.5;
		container(posX,posY-contHeight,posZ,width,contHeight,glm::vec4(0.2),uiPrograms,icons.Circle,glm::vec4(0),0);
		for (size_t i = 1; i <= list.size(); i++)
		{
			glUseProgram(uiPrograms.uiProgram);
			bool hover = isMouseOnButton(window,width, 0.03f,posX-screenGapX,posY - i/15.f,mouseXpos,mouseYpos,false);
			bool pressed = false;
			
			if(i-1 == chosenIndex){
				pressed = true;
			}

			if(hover && firstClick && chosenIndex != i-1){
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
	circle(posX-width,posY,posZ+0.0001f,0.025,0.05,icons.Circle,glm::vec4(0.1,0.1,0.1,0.2),glm::vec4(0),0);
	circle(posX+width,posY,posZ+0.0001f,0.025,0.05,icons.Circle,glm::vec4(0.1,0.1,0.1,0.2),glm::vec4(0),0);
	
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

void UserInterface::renderTheProgram(float posx,float posy,float width,float height){
	
	std::vector<float> renderVertices = { 
		// first triangle
		 width + posx,  height+ posy, 0.9f,1,1,0,0,0,  // top right
		 width+ posx,  -height+ posy, 0.9f,1,0,0,0,0,  // bottom right
		 -width+ posx,  height+ posy, 0.9f,0,1,0,0,0,  // top left 
		// second tria0gle	  ,0,0,0,
		 width+ posx,  -height+ posy, 0.9f,1,0,0,0,0,  // bottom right
		 -width+ posx,  -height+ posy, 0.9f,0,0,0,0,0,  // bottom left
		 -width+ posx,  height+ posy, 0.9f,0,1,0,0,0   // top left
	};

	GlSet glset;
	glset.drawArrays(renderVertices,false);
}

void UserInterface::sendObjectsToUI(Objects aobjects,unsigned int acircleIcon,unsigned int asmoothSquareIcon){
	uiObjects = aobjects;
	circleIcon = acircleIcon;
	smoothSquareIcon = asmoothSquareIcon;
}