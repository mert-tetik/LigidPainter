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
#include "Core/Load.hpp"
#include "Core/Render/Render.h"

bool establishConnectionFirstRelease = false;

void UserInterface::node(Node &node,Programs programs,Icons icons,GLFWwindow* window,double mouseX,double mouseY,double xOffset,double yOffset,
float maxScreenWidth,float maxScreenHeight, NodeScene &material,NodePanel &nodePanel,TextureSelectionPanel &textureSelectionPanel,int currentNodeIndex,
std::vector<aTexture> albedoTextures,float screenGapX,bool firstClick,ColoringPanel &coloringPanel){
	ColorData colorData;
	Utilities util;

	const float depth = 0.1f;

	const float iconWidth = node.width/6.f;

	//TODO : Check in one for loop
	//Check if any of the node's bar is pressed
	bool anyBarPressed = false;
	for (int nodeI = 0; nodeI < material.nodes.size(); nodeI++)
	{
		if(material.nodes[nodeI].barPressed){
			anyBarPressed = true;
			break;
		}
	}

	//Check if any of the node's input is pressed
	bool anyInputHover = false;
	for (int nodeI = 0; nodeI < material.nodes.size(); nodeI++)
	{
		for (size_t inIndex = 0; inIndex < material.nodes[nodeI].inputs.size(); inIndex++)
		{
			if(material.nodes[nodeI].inputs[inIndex].connectionHover){
				anyInputHover = true;
				break;
			}
		}
	}

	//
	bool anyConnectionPressed = false;
	for (int nodeI = 0; nodeI < material.nodes.size(); nodeI++)
	{
		for (size_t outIndex = 0; outIndex < material.nodes[nodeI].outputs.size(); outIndex++)
		{
			if(material.nodes[nodeI].outputs[outIndex].pressed){
				anyConnectionPressed = true;
				break;
			}
		}
	}

		
	const float screenGap = screenGapX;

    //Move the node if it's top bar is pressed
	if(!anyConnectionPressed){
		if(nodePanel.panelHover)
			node.barHover = isMouseOnButton(window , node.width , iconWidth*2.f ,(node.positionX + nodePanel.panelPositionX) * nodePanel.zoomVal - screenGap,(node.positionY + nodePanel.panelPositionY) * nodePanel.zoomVal + node.height + iconWidth*2.f,mouseX,mouseY,false);
		else
			node.barHover = false;
	}
	if(firstClick && node.barHover && !anyBarPressed){
		node.barPressed = true;
	}
	if(glfwGetMouseButton(window,0) == GLFW_RELEASE){
		node.barPressed = false;
	}
	if(node.barPressed && !nodePanel.boundariesPressed){
		node.positionX += xOffset/maxScreenWidth*2.f / nodePanel.zoomVal;
		node.positionY -= yOffset/maxScreenHeight*2.f / nodePanel.zoomVal;
		
		xOffset = 0;
		yOffset = 0;
	}






    //Render the panel
	glUseProgram(programs.uiProgram);
	box(node.width,node.height,(node.positionX + nodePanel.panelPositionX) * nodePanel.zoomVal,(node.positionY + nodePanel.panelPositionY) * nodePanel.zoomVal,"",node.backColor,0,0,0,depth,10000,node.backColor,0);
    //-Side area
	box(iconWidth,node.height,(node.positionX + nodePanel.panelPositionX) * nodePanel.zoomVal-node.width -iconWidth,(node.positionY + nodePanel.panelPositionY) * nodePanel.zoomVal,"",node.backColor,0,0,0,depth,10000,node.backColor,0);///Left
	box(iconWidth,node.height,(node.positionX + nodePanel.panelPositionX) * nodePanel.zoomVal+node.width +iconWidth,(node.positionY + nodePanel.panelPositionY) * nodePanel.zoomVal,"",node.backColor,0,0,0,depth,10000,node.backColor,0);///Right
	box(node.width,iconWidth*2.f,(node.positionX + nodePanel.panelPositionX) * nodePanel.zoomVal,(node.positionY + nodePanel.panelPositionY) * nodePanel.zoomVal + node.height + iconWidth*2.f,"",node.upBarColor,0,0,0,depth,10000,node.upBarColor,0);///Top
	box(node.width,iconWidth*2.f,(node.positionX + nodePanel.panelPositionX) * nodePanel.zoomVal,(node.positionY + nodePanel.panelPositionY) * nodePanel.zoomVal - node.height - iconWidth*2.f,"",node.backColor,0,0,0,depth,10000,node.backColor,0);///Bottom
	//-Smooth corners
	glUseProgram(programs.iconsProgram);
	iconBox(iconWidth , iconWidth*2.f , (node.positionX + nodePanel.panelPositionX) * nodePanel.zoomVal-node.width -iconWidth, (node.positionY + nodePanel.panelPositionY) * nodePanel.zoomVal + node.height + iconWidth*2.f, depth , icons.TL , 0 , node.upBarColor , node.backColor);
	iconBox(iconWidth , iconWidth*2.f , (node.positionX + nodePanel.panelPositionX) * nodePanel.zoomVal+node.width +iconWidth, (node.positionY + nodePanel.panelPositionY) * nodePanel.zoomVal + node.height + iconWidth*2.f, depth , icons.TR , 0 , node.upBarColor , node.backColor);
	iconBox(iconWidth , iconWidth*2.f , (node.positionX + nodePanel.panelPositionX) * nodePanel.zoomVal-node.width -iconWidth, (node.positionY + nodePanel.panelPositionY) * nodePanel.zoomVal - node.height - iconWidth*2.f, depth , icons.BL , 0 , node.backColor , node.backColor);
	iconBox(iconWidth , iconWidth*2.f , (node.positionX + nodePanel.panelPositionX) * nodePanel.zoomVal+node.width +iconWidth, (node.positionY + nodePanel.panelPositionY) * nodePanel.zoomVal - node.height - iconWidth*2.f, depth , icons.BR , 0 , node.backColor , node.backColor);








	int ioIndex = 0; //Interpret the input with output size since they both rendered seperately
	
	//-----RENDER THE OUTPUT-----
	for (size_t i = 0; i < node.outputs.size(); i++)
	{
        //Color of the output (changes related to output type)
		glm::vec4 nodeColor = glm::vec4(0);

        //Process the output type
		if(node.outputs[i].type == "float"){
			nodeColor = colorData.floatNodeInputColor;
		}
		if(node.outputs[i].type == "vec2"){
			nodeColor = colorData.vec2NodeInputColor;
		}
		if(node.outputs[i].type == "vec3"){
			nodeColor = colorData.vec3NodeInputColor;
		}


		//TODO : Check this algorithm
		//Check if mouse is hover any of the inputs
		int inputIndex;
		for (int nodeI = 0; nodeI < material.nodes.size(); nodeI++)
		{
			for (int inputI = 0; inputI < material.nodes[nodeI].inputs.size(); inputI++)
			{
				//Establish connection
				if(material.nodes[nodeI].inputs[inputI].connectionHover && node.outputs[i].pressed && material.nodes[nodeI].inputs[inputI].nodeConnectionIndex == 10000){
					establishConnectionFirstRelease = true;
				}
				if(glfwGetMouseButton(window,0) == GLFW_RELEASE && establishConnectionFirstRelease){
					node.outputs[i].inputConnectionIndex = inputI;
					node.outputs[i].nodeConnectionIndex = nodeI;

					material.nodes[nodeI].inputs[inputI].inputConnectionIndex = i;
					material.nodes[nodeI].inputs[inputI].nodeConnectionIndex = currentNodeIndex;

					
					node.outputs[i].isConnectedToShaderInput = material.nodes[nodeI].isMainOut;

					
					node.outputs[i].connectionPosX = material.nodes[nodeI].inputs[inputI].posX; 
					node.outputs[i].connectionPosY = material.nodes[nodeI].inputs[inputI].posY; 

					material.stateChanged = true;
					establishConnectionFirstRelease = false;
					break;

				}	
			}
		}
		
        //Check if output pressed
		if(!anyConnectionPressed){//Prevent multiple selection
			if(nodePanel.panelHover)
				node.outputs[i].connectionHover = isMouseOnButton(window , iconWidth/1.5f , iconWidth*1.5f  ,node.outputs[i].connectionPosX- screenGap,node.outputs[i].connectionPosY,mouseX,mouseY,false);
			else
				node.outputs[i].connectionHover = false;
		}
		
		//If output pressed (move the connection)
		if(firstClick && node.outputs[i].connectionHover && !node.outputs[i].pressed){
			node.outputs[i].pressed = true;
		}
		
		//Prevent zooming in and out bug while moving the connection
		if((!node.outputs[i].pressed && nodePanel.zoomValChanged)){
				//Render the connection on top of the default connection circle (hide)
				node.outputs[i].connectionPosX = (node.positionX + nodePanel.panelPositionX) * nodePanel.zoomVal + node.width +iconWidth*2.f;
				node.outputs[i].connectionPosY = ((node.positionY + nodePanel.panelPositionY) * nodePanel.zoomVal + node.height) - i/(20.f/(node.width*15)) - 0.05f * node.width*10;
				
				//Render the connection on top of the connection circle (show)
				if(node.outputs[i].nodeConnectionIndex != 10000 && node.outputs[i].inputConnectionIndex != 10000){
					node.outputs[i].connectionPosX = material.nodes[node.outputs[i].nodeConnectionIndex].inputs[node.outputs[i].inputConnectionIndex].posX; 
					node.outputs[i].connectionPosY = material.nodes[node.outputs[i].nodeConnectionIndex].inputs[node.outputs[i].inputConnectionIndex].posY; 
				}
		}

		//If output released (release the connection)
		if(glfwGetMouseButton(window,0) == GLFW_RELEASE || node.barPressed || anyBarPressed){
			if(true){
				//Severe the connection if connection is not released in a input
				if(!anyInputHover && node.outputs[i].pressed){
					if(node.outputs[i].nodeConnectionIndex != 10000 && node.outputs[i].inputConnectionIndex != 10000){
						material.nodes[node.outputs[i].nodeConnectionIndex].inputs[node.outputs[i].inputConnectionIndex].nodeConnectionIndex = 10000;
						material.nodes[node.outputs[i].nodeConnectionIndex].inputs[node.outputs[i].inputConnectionIndex].inputConnectionIndex = 10000;
					}
					node.outputs[i].nodeConnectionIndex = 10000;
					node.outputs[i].inputConnectionIndex = 10000;

					material.stateChanged = true;
				}
				
				//Render the connection on top of the default connection circle (hide)
				if(node.outputs[i].nodeConnectionIndex == 10000 && node.outputs[i].inputConnectionIndex == 10000){
					node.outputs[i].connectionPosX = (node.positionX + nodePanel.panelPositionX) * nodePanel.zoomVal + node.width +iconWidth*2.f;
					node.outputs[i].connectionPosY = ((node.positionY + nodePanel.panelPositionY) * nodePanel.zoomVal + node.height) - i/(20.f/(node.width*15)) - 0.05f * node.width*10;
				}
				
				//Render the connection on top of the connection circle (show)
				if(node.outputs[i].nodeConnectionIndex != 10000 && node.outputs[i].inputConnectionIndex != 10000){
					node.outputs[i].connectionPosX = material.nodes[node.outputs[i].nodeConnectionIndex].inputs[node.outputs[i].inputConnectionIndex].posX; 
					node.outputs[i].connectionPosY = material.nodes[node.outputs[i].nodeConnectionIndex].inputs[node.outputs[i].inputConnectionIndex].posY; 
				}
			}
			node.outputs[i].pressed = false;
		}
		//Move the connection (on top of the cursor)
		if(node.outputs[i].pressed){
			node.outputs[i].connectionPosX += xOffset/maxScreenWidth*2.f;
			node.outputs[i].connectionPosY -= yOffset/maxScreenHeight*2.f;
		}

        //Render the connection line
        glUseProgram(programs.uiProgram);
		//TODO : Move the rendering of connection lines to the output element rendering
		if(node.outputs[i].nodeConnectionIndex != 10000 && node.outputs[i].inputConnectionIndex != 10000 || node.outputs[i].pressed)//Render the connection lines if output connects to an input or moves
			drawLine((node.positionX + nodePanel.panelPositionX) * nodePanel.zoomVal+node.width +iconWidth*2.f,((node.positionY + nodePanel.panelPositionY) * nodePanel.zoomVal + node.height) - i/(20.f/(node.width*15)) - 0.05f * node.width*10,depth+0.02f,node.outputs[i].connectionPosX,node.outputs[i].connectionPosY, node.width*200.f ,nodeColor);
		

        //Render the output
		//TODO : Use those values for rendering and tracking
        glUseProgram(programs.iconsProgram);
		node.outputs[i].posX = (node.positionX + nodePanel.panelPositionX) * nodePanel.zoomVal+node.width +iconWidth*2.f;
		node.outputs[i].posY = ((node.positionY + nodePanel.panelPositionY) * nodePanel.zoomVal + node.height) - i/(20.f/(node.width*15)) - 0.05f * node.width*10;
		iconBox(iconWidth/1.5f , iconWidth*1.5f , (node.positionX + nodePanel.panelPositionX) * nodePanel.zoomVal+node.width +iconWidth*2.f, ((node.positionY + nodePanel.panelPositionY) * nodePanel.zoomVal + node.height) - i/(20.f/(node.width*15)) - 0.05f * node.width*10, depth+0.01f , icons.Circle , 0 , nodeColor , nodeColor);
		ioIndex++;
	}










	float rangeBarCountInputs = 0;
	//-----RENDER THE INPUTS-----
	for (size_t i = 0; i < node.inputs.size(); i++)
	{
        //Input color
		glm::vec4 nodeColor = glm::vec4(0);
		
        //Process the color (related to the input type)
		if(node.inputs[i].type == "float"){
			nodeColor = colorData.floatNodeInputColor;
		}
		if(node.inputs[i].type == "vec2"){
			nodeColor = colorData.vec2NodeInputColor;
		}
		if(node.inputs[i].type == "vec3"){
			nodeColor = colorData.vec3NodeInputColor;
		}
		if(nodePanel.panelHover)
			node.inputs[i].connectionHover = isMouseOnButton(window,iconWidth/1.5f , iconWidth*1.5f,(node.positionX + nodePanel.panelPositionX) * nodePanel.zoomVal-node.width - iconWidth*2.f - screenGap,((node.positionY + nodePanel.panelPositionY) * nodePanel.zoomVal + node.height) - (i+ioIndex+rangeBarCountInputs)/(20.f/(node.width*16)) - 0.05f * node.width*10,mouseX,mouseY,false);
		else
			node.inputs[i].connectionHover = false;
		
		node.inputs[i].posX = (node.positionX + nodePanel.panelPositionX) * nodePanel.zoomVal-node.width - iconWidth*2.f;
		node.inputs[i].posY = ((node.positionY + nodePanel.panelPositionY) * nodePanel.zoomVal + node.height) - (i+ioIndex+rangeBarCountInputs)/(20.f/(node.width*16)) - 0.05f * node.width*10;

		iconBox(iconWidth/1.5f , iconWidth*1.5f , (node.positionX + nodePanel.panelPositionX) * nodePanel.zoomVal-node.width - iconWidth*2.f, ((node.positionY + nodePanel.panelPositionY) * nodePanel.zoomVal + node.height) - (i+ioIndex+rangeBarCountInputs)/(20.f/(node.width*16)) - 0.05f * node.width*10, depth+0.01f , icons.Circle , 0 , nodeColor , nodeColor);
		
        //Process the gap necessary related to the input type after rendering
		if(node.inputs[i].element == "range"){
			if(node.inputs[i].type == "float"){
				rangeBarCountInputs += 1;
			}
			if(node.inputs[i].type == "vec2"){
				rangeBarCountInputs += 2;
			}
			if(node.inputs[i].type == "vec3"){
				rangeBarCountInputs += 3;
			}
		}
		else if(node.inputs[i].element == "image"){
			rangeBarCountInputs += 1;
		}
		else if(node.inputs[i].element == "color"){
			rangeBarCountInputs += 1;
		}

	}




	//Delete the node
	if(!node.isMainOut){
		bool deleteButtonEnter = false;
		if(nodePanel.panelHover)
			deleteButtonEnter = isMouseOnButton(window,iconWidth, iconWidth*2.f,(node.positionX + nodePanel.panelPositionX) * nodePanel.zoomVal + node.width - screenGap,((node.positionY + nodePanel.panelPositionY) * nodePanel.zoomVal + node.height) + iconWidth*1.7f,mouseX,mouseY,false);
		
		if(deleteButtonEnter && firstClick){
			material.nodes.erase(material.nodes.begin() + currentNodeIndex);
		}
	
		iconBox(iconWidth/3.f , iconWidth/1.5f , (node.positionX + nodePanel.panelPositionX) * nodePanel.zoomVal + node.width, (node.positionY + nodePanel.panelPositionY) * nodePanel.zoomVal + node.height + iconWidth*1.7f, depth+0.02f , icons.X , 0 , colorData.iconColor, colorData.iconColor);
	}

	glUseProgram(programs.uiProgram);

    //Render the node title
	renderText(programs.uiProgram,node.title,(node.positionX + nodePanel.panelPositionX) * nodePanel.zoomVal-node.width -iconWidth,(node.positionY + nodePanel.panelPositionY) * nodePanel.zoomVal + node.height + iconWidth*1.f,node.width/300.f,colorData.textColor,depth+0.01f,false);
	






    //-----RENDER THE OUTPUT ELEMENTS-----
	for (size_t i = 0; i < node.outputs.size(); i++)
	{
		renderText(programs.uiProgram,node.outputs[i].text,(node.positionX + nodePanel.panelPositionX) * nodePanel.zoomVal+node.width - (node.outputs[i].text.size()/60.f)*node.width*8.f,((node.positionY + nodePanel.panelPositionY) * nodePanel.zoomVal + node.height) - i/(20.f/(node.width*16)) - 0.05f * node.width*10,node.width/300.f,colorData.textColor,depth+0.01f,false);
	}
		





	int inputElementIndex = 0;
	
	bool isRangeBarPointerHover = false;
	
    //-----RENDER THE INPUT ELEMENTS-----
	for (size_t i = 0; i < node.inputs.size(); i++)
	{
        //Render the input title
		renderText(programs.uiProgram,node.inputs[i].text,(node.positionX + nodePanel.panelPositionX) * nodePanel.zoomVal-node.width,((node.positionY + nodePanel.panelPositionY) * nodePanel.zoomVal + node.height) - (i+ioIndex+inputElementIndex)/(20.f/(node.width*16)) - 0.05f * node.width*10,node.width/300.f,colorData.textColor,depth+0.01f,false);
		if(node.inputs[i].element == "color"){
			inputElementIndex++;
			box(iconWidth*6,iconWidth*2.f,(node.positionX + nodePanel.panelPositionX) * nodePanel.zoomVal, ((node.positionY + nodePanel.panelPositionY) * nodePanel.zoomVal + node.height) - (i+ioIndex+inputElementIndex)/(20.f/(node.width*16)) - 0.05f * node.width*10,"",glm::vec4(node.inputs[i].color/255.f,1),0,0,0,depth+0.01,8 / (node.width*6),node.backColor,0);///Bottom
			bool colorInputHover = false; 
			if(nodePanel.panelHover){
				colorInputHover = isMouseOnButton(window,iconWidth*6,iconWidth*2.f,(node.positionX + nodePanel.panelPositionX) * nodePanel.zoomVal, ((node.positionY + nodePanel.panelPositionY) * nodePanel.zoomVal + node.height) - (i+ioIndex+inputElementIndex)/(20.f/(node.width*16)) - 0.05f * node.width*10,mouseX,mouseY,false);
			}
			if(colorInputHover && firstClick && !node.inputs[i].coloringPanelActivated){
				node.inputs[i].coloringPanelActivated = true;
				coloringPanel.active = true;
				
				coloringPanel.hexVal = util.rgbToHexGenerator(node.inputs[i].color);
				coloringPanel.newHexValTextboxEntry = true;
				coloringPanel.result = node.inputs[i].color;
				coloringPanel.panelPosX = mouseX/(maxScreenWidth/2.f) - 1.0f + screenGapX ;
				coloringPanel.panelPosY = -mouseY/maxScreenHeight*2.f + 1.0f;
			}

			if(!coloringPanel.active)
				node.inputs[i].coloringPanelActivated = false;

			if(node.inputs[i].coloringPanelActivated){
				node.inputs[i].color = coloringPanel.result;
				material.stateChanged = true;
			}
		}

		if(node.inputs[i].element == "image"){
			inputElementIndex++;
			
			//Select texture
			if(nodePanel.panelHover)
				node.inputs[i].addTextureButtonHover = isMouseOnButton(window,iconWidth, iconWidth*2.f,(node.positionX + nodePanel.panelPositionX) * nodePanel.zoomVal - node.width - screenGap,((node.positionY + nodePanel.panelPositionY) * nodePanel.zoomVal + node.height) - (i+ioIndex+inputElementIndex)/(20.f/(node.width*16)) - 0.05f * node.width*10,mouseX,mouseY,false);
			else
				node.inputs[i].addTextureButtonHover = false;
			//Remove texture
			if(nodePanel.panelHover)
				node.inputs[i].removeTextureButtonHover = isMouseOnButton(window,iconWidth, iconWidth*2.f,(node.positionX + nodePanel.panelPositionX) * nodePanel.zoomVal + node.width - screenGap,((node.positionY + nodePanel.panelPositionY) * nodePanel.zoomVal + node.height) - (i+ioIndex+inputElementIndex)/(20.f/(node.width*16)) - 0.05f * node.width*10,mouseX,mouseY,false);
			else
				node.inputs[i].removeTextureButtonHover = false;
			
			if(node.inputs[i].addTextureButtonHover && firstClick){
				textureSelectionPanel.active = true;
				textureSelectionPanel.posX = node.inputs[i].posX;
				textureSelectionPanel.posY = node.inputs[i].posY;
				node.inputs[i].textureSelectingState = true;
			}

			if(!textureSelectionPanel.active){
				node.inputs[i].textureSelectingState = false;
			}

			if(node.inputs[i].removeTextureButtonHover && firstClick){
				material.stateChanged = true;
				node.inputs[i].selectedTextureIndex = 10000;
				node.inputs[i].selectedTexture = 0;
			}



			if(textureSelectionPanel.active && textureSelectionPanel.textureClicked && node.inputs[i].textureSelectingState){
				material.stateChanged = true;
				node.inputs[i].selectedTextureIndex = textureSelectionPanel.selectedIndex;
				node.inputs[i].selectedTexture = albedoTextures[textureSelectionPanel.selectedIndex].id;


				textureSelectionPanel.selectedIndex = 10000;
				textureSelectionPanel.active = false;
				textureSelectionPanel.textureClicked = false;
			}

			//Select a texture
			box(iconWidth/2.f,iconWidth*2.f,(node.positionX + nodePanel.panelPositionX) * nodePanel.zoomVal - node.width, ((node.positionY + nodePanel.panelPositionY) * nodePanel.zoomVal + node.height) - (i+ioIndex+inputElementIndex)/(20.f/(node.width*16)) - 0.05f * node.width*10,"",colorData.rangeBarFront,0,0,0,depth+0.01,8 / (node.width*6),node.backColor,0);
			//Remove texture
			box(iconWidth/2.f,iconWidth*2.f,(node.positionX + nodePanel.panelPositionX) * nodePanel.zoomVal + node.width, ((node.positionY + nodePanel.panelPositionY) * nodePanel.zoomVal + node.height) - (i+ioIndex+inputElementIndex)/(20.f/(node.width*16)) - 0.05f * node.width*10,"",colorData.rangeBarBack,0,0,0,depth+0.02,8 / (node.width*6),node.backColor,0);
			
			box(iconWidth*6,iconWidth*2.f,(node.positionX + nodePanel.panelPositionX) * nodePanel.zoomVal, ((node.positionY + nodePanel.panelPositionY) * nodePanel.zoomVal + node.height) - (i+ioIndex+inputElementIndex)/(20.f/(node.width*16)) - 0.05f * node.width*10,"",colorData.buttonColor,0,0,0,depth+0.01,8 / (node.width*6),node.backColor,0);///Bottom
			
			renderText(programs.uiProgram,"texture_" + std::to_string(node.inputs[i].selectedTextureIndex),(node.positionX + nodePanel.panelPositionX) * nodePanel.zoomVal - node.width/1.5f,((node.positionY + nodePanel.panelPositionY) * nodePanel.zoomVal + node.height) - (i+ioIndex+inputElementIndex)/(20.f/(node.width*16)) - 0.05f * node.width*10,node.width/300.f,colorData.textColor,depth+0.02,false);
		}

        //Render the range bars
		if(node.inputs[i].element == "range"){
			inputElementIndex++;
		
        	//Determine the range bar count
			float rangeBarCount = 0;
			if(node.inputs[i].type == "float")
				rangeBarCount = 1;
			if(node.inputs[i].type == "vec2")
				rangeBarCount = 2;
			if(node.inputs[i].type == "vec3")
				rangeBarCount = 3;

        	for (size_t k = 0; k < rangeBarCount; k++)
			{
        	    //Check if any other range bar pointer is pressed to prevent multiple selection
				bool anyPointerPressed = false;
				for (size_t a = 0; a < node.inputs.size(); a++)
				{
					float rangeBarCountTest = 0;

					if(node.inputs[a].type == "float"){
						rangeBarCountTest = 1;
					}
					if(node.inputs[a].type == "vec2"){
						rangeBarCountTest = 2;
					}
					if(node.inputs[a].type == "vec3"){
						rangeBarCountTest = 3;
					}

					for (size_t aa = 0; aa < rangeBarCountTest; aa++)
					{
						if(node.inputs[a].rangeBarsPointerPressed[aa]){
							anyPointerPressed = true;
						}
					}
				}

        	    //Assign the corresponding value for the range bar
				float val;
				if(k == 0){
					val = node.inputs[i].value.x;
				} 
				else if(k == 1){
					val = node.inputs[i].value.y;
				}
				else if(k == 2){
					val = node.inputs[i].value.z;
				}

				float pointPosVal = val / (1.f/(node.width*2.f)) - node.width;

				//Range bar
				box(node.width,iconWidth*2.f,(node.positionX + nodePanel.panelPositionX) * nodePanel.zoomVal,((node.positionY + nodePanel.panelPositionY) * nodePanel.zoomVal + node.height) - (i+ioIndex+inputElementIndex)/(20.f/(node.width*16)) - 0.05f * node.width*10,"",colorData.rangeBarBack,0,0,0,depth+0.01f,8 / (node.width*6),node.backColor,0);///Bottom
				//Pointer
				box(iconWidth/4.f,iconWidth*2.f,(node.positionX + nodePanel.panelPositionX) * nodePanel.zoomVal + pointPosVal,((node.positionY + nodePanel.panelPositionY) * nodePanel.zoomVal + node.height) - (i+ioIndex+inputElementIndex)/(20.f/(node.width*16)) - 0.05f * node.width*10,"",colorData.rangeBarFront,0,0,0,depth+0.02f,8 / (node.width*6),node.backColor,0);///Top

				//TODO : Calculate the screen gap
        	    //Check if range bar pointer pressed
				if(!anyPointerPressed){
					if(nodePanel.panelHover)
						isRangeBarPointerHover = isMouseOnButton(window , iconWidth , iconWidth*2.f ,(node.positionX + nodePanel.panelPositionX) * nodePanel.zoomVal + pointPosVal - screenGap,((node.positionY + nodePanel.panelPositionY) * nodePanel.zoomVal + node.height) - (i+ioIndex+inputElementIndex)/(20.f/(node.width*16)) - 0.05f * node.width*10,mouseX,mouseY,false);
					else
						isRangeBarPointerHover = false;
				}
				if(firstClick && isRangeBarPointerHover){
					node.inputs[i].rangeBarsPointerPressed[k] = true;
				}
				if(glfwGetMouseButton(window,0) == GLFW_RELEASE){
					node.inputs[i].rangeBarsPointerPressed[k] = false;
				}
				if(node.inputs[i].rangeBarsPointerPressed[k] && !anyConnectionPressed){
        	        //Adjust the corresponding values
					material.stateChanged = true;
					if(k == 0){
						node.inputs[i].value.x += xOffset/(nodePanel.zoomVal*200);
						node.inputs[i].value.x = util.restrictBetween(node.inputs[i].value.x,1.f,0.001f);
					}
					if(k == 1){
						node.inputs[i].value.y += xOffset/(nodePanel.zoomVal*200);
						node.inputs[i].value.y = util.restrictBetween(node.inputs[i].value.y,1.f,0.001f);
					}
					if(k == 2){
						node.inputs[i].value.z += xOffset/(nodePanel.zoomVal*200);
						node.inputs[i].value.z = util.restrictBetween(node.inputs[i].value.z,1.f,0.001f);
					}
				}	
				if(k != rangeBarCount-1)
					inputElementIndex++;
				isRangeBarPointerHover = false;
			}
		}
	}
}

