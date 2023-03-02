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

void UserInterface::node(Node &node,Programs programs,Icons icons,GLFWwindow* window,double mouseX,double mouseY,double xOffset,double yOffset,
float maxScreenWidth,float maxScreenHeight, NodeScene &material,NodePanel &nodePanel,TextureSelectionPanel &textureSelectionPanel,int currentNodeIndex,
std::vector<aTexture> albedoTextures,float screenGapX,bool firstClick,ColoringPanel &coloringPanel,bool &duplicateNodeCall,std::vector<Node> &duplicatedNodes){
	ColorData colorData;
	ColorData2 colorData2;
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
		if(nodePanel.panelHover && !coloringPanel.active)
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
	if((node.barPressed && !nodePanel.boundariesPressed) || (node.active && glfwGetKey(window,GLFW_KEY_G) == GLFW_PRESS)){
		node.positionX += xOffset/maxScreenWidth*2.f / nodePanel.zoomVal;
		node.positionY -= yOffset/maxScreenHeight*2.f / nodePanel.zoomVal;
		
		xOffset = 0;
		yOffset = 0;
	}

	node.panelHover = isMouseOnButton(window,node.width*1.4,node.height,(node.positionX + nodePanel.panelPositionX) * nodePanel.zoomVal,(node.positionY + nodePanel.panelPositionY) * nodePanel.zoomVal,mouseX,mouseY,false);

	glm::vec4 outlineColor = colorData2.nodeOutlineColor;

    //Render the panel
	glUseProgram(programs.uiProgram);
	box(node.width,node.height,(node.positionX + nodePanel.panelPositionX) * nodePanel.zoomVal,(node.positionY + nodePanel.panelPositionY) * nodePanel.zoomVal,"",node.backColor,0,0,0,depth,10000,node.backColor,0);
    //-Side area
	box(iconWidth,node.height,(node.positionX + nodePanel.panelPositionX) * nodePanel.zoomVal-node.width -iconWidth,(node.positionY + nodePanel.panelPositionY) * nodePanel.zoomVal,"",node.backColor,0,0,0,depth,10000,node.backColor,0);///Left
	box(iconWidth,node.height,(node.positionX + nodePanel.panelPositionX) * nodePanel.zoomVal+node.width +iconWidth,(node.positionY + nodePanel.panelPositionY) * nodePanel.zoomVal,"",node.backColor,0,0,0,depth,10000,node.backColor,0);///Right
	box(node.width,iconWidth*2.f,(node.positionX + nodePanel.panelPositionX) * nodePanel.zoomVal,(node.positionY + nodePanel.panelPositionY) * nodePanel.zoomVal + node.height + iconWidth*2.f,"",node.upBarColor,0,0,0,depth,10000,node.upBarColor,0);///Top
	box(node.width,iconWidth*2.f,(node.positionX + nodePanel.panelPositionX) * nodePanel.zoomVal,(node.positionY + nodePanel.panelPositionY) * nodePanel.zoomVal - node.height - iconWidth*2.f,"",node.backColor,0,0,0,depth,10000,node.backColor,0);///Bottom
	
	
	
	if(node.active){
		//-Side area outline
		glUseProgram(programs.uiProgram);
		box(iconWidth*1.1,node.height,(node.positionX + nodePanel.panelPositionX) * nodePanel.zoomVal-node.width -iconWidth,(node.positionY + nodePanel.panelPositionY) * nodePanel.zoomVal,"",outlineColor,0,0,0,depth,10000,node.backColor,0);///Left
		box(iconWidth*1.1,node.height,(node.positionX + nodePanel.panelPositionX) * nodePanel.zoomVal+node.width +iconWidth,(node.positionY + nodePanel.panelPositionY) * nodePanel.zoomVal,"",outlineColor,0,0,0,depth,10000,node.backColor,0);///Right
		box(node.width,iconWidth*2.f*1.1,(node.positionX + nodePanel.panelPositionX) * nodePanel.zoomVal,(node.positionY + nodePanel.panelPositionY) * nodePanel.zoomVal + node.height + iconWidth*2.f,"",outlineColor,0,0,0,depth,10000,node.upBarColor,0);///Top
		box(node.width,iconWidth*2.f*1.1,(node.positionX + nodePanel.panelPositionX) * nodePanel.zoomVal,(node.positionY + nodePanel.panelPositionY) * nodePanel.zoomVal - node.height - iconWidth*2.f,"",outlineColor,0,0,0,depth,10000,node.backColor,0);///Bottom
	
		//-Smooth corners outline
		glUseProgram(programs.iconsProgram);
		iconBox(iconWidth*1.1 , iconWidth*2.f*1.1 , (node.positionX + nodePanel.panelPositionX) * nodePanel.zoomVal-node.width -iconWidth, (node.positionY + nodePanel.panelPositionY) * nodePanel.zoomVal + node.height + iconWidth*2.f, depth , icons.TL , 0 , outlineColor , node.backColor);
		iconBox(iconWidth*1.1 , iconWidth*2.f*1.1 , (node.positionX + nodePanel.panelPositionX) * nodePanel.zoomVal+node.width +iconWidth, (node.positionY + nodePanel.panelPositionY) * nodePanel.zoomVal + node.height + iconWidth*2.f, depth , icons.TR , 0 , outlineColor , node.backColor);
		iconBox(iconWidth*1.1 , iconWidth*2.f*1.1 , (node.positionX + nodePanel.panelPositionX) * nodePanel.zoomVal-node.width -iconWidth, (node.positionY + nodePanel.panelPositionY) * nodePanel.zoomVal - node.height - iconWidth*2.f, depth , icons.BLR , 0 , outlineColor , node.backColor);
		iconBox(iconWidth*1.1 , iconWidth*2.f*1.1 , (node.positionX + nodePanel.panelPositionX) * nodePanel.zoomVal+node.width +iconWidth, (node.positionY + nodePanel.panelPositionY) * nodePanel.zoomVal - node.height - iconWidth*2.f, depth , icons.BRR , 0 , outlineColor , node.backColor);

	}
    
	//-Smooth corners
	glUseProgram(programs.iconsProgram);
	iconBox(iconWidth , iconWidth*2.f , (node.positionX + nodePanel.panelPositionX) * nodePanel.zoomVal-node.width -iconWidth, (node.positionY + nodePanel.panelPositionY) * nodePanel.zoomVal + node.height + iconWidth*2.f, depth+0.001f , icons.TL , 0 , node.upBarColor , node.backColor);
	iconBox(iconWidth , iconWidth*2.f , (node.positionX + nodePanel.panelPositionX) * nodePanel.zoomVal+node.width +iconWidth, (node.positionY + nodePanel.panelPositionY) * nodePanel.zoomVal + node.height + iconWidth*2.f, depth+0.001f , icons.TR , 0 , node.upBarColor , node.backColor);
	iconBox(iconWidth , iconWidth*2.f , (node.positionX + nodePanel.panelPositionX) * nodePanel.zoomVal-node.width -iconWidth, (node.positionY + nodePanel.panelPositionY) * nodePanel.zoomVal - node.height - iconWidth*2.f, depth+0.001f , icons.BL , 0 , node.backColor , node.backColor);
	iconBox(iconWidth , iconWidth*2.f , (node.positionX + nodePanel.panelPositionX) * nodePanel.zoomVal+node.width +iconWidth, (node.positionY + nodePanel.panelPositionY) * nodePanel.zoomVal - node.height - iconWidth*2.f, depth+0.001f , icons.BR , 0 , node.backColor , node.backColor);



	int ioIndex = 0; //Interpret the input with output size since they both rendered seperately
	if(node.listBoxes.size())
		ioIndex++;
	
	for (size_t i = 0; i < node.listBoxes.size(); i++)
	{
		glUseProgram(programs.uiProgram);
		bool listButtonHover = isMouseOnButton(window,node.width,iconWidth*2.f,(node.positionX + nodePanel.panelPositionX) * nodePanel.zoomVal,((node.positionY + nodePanel.panelPositionY) * nodePanel.zoomVal + node.height) - (i+ioIndex)/(20.f/(node.width*16)) - 0.05f * node.width*10,mouseX,mouseY,false);

		if(node.listBoxes[i].active){
			
			bool noButtonHover = true;
			int listElementI = 0;
			for (size_t x = 0; x < node.listBoxes[i].elements.size()/6 + 1; x++)
			{
				for (size_t j = 0; j < 6; j++)
				{
					if(node.listBoxes[i].elements.size() == listElementI)
						break;

					const bool elementHover = isMouseOnButton(window , node.width,iconWidth*2.f,(node.positionX + nodePanel.panelPositionX) * nodePanel.zoomVal + (x)/(20.f/(node.width*40)),((node.positionY + nodePanel.panelPositionY) * nodePanel.zoomVal + node.height) - (i+ioIndex+j+1)/(20.f/(node.width*13)) - 0.05f * node.width*10 - iconWidth*1,mouseX,mouseY,false);
					
					if(elementHover)
						noButtonHover = false;
						
					glm::vec4 color;
					if(elementHover)
						color = colorData.buttonColorHover;
					else
						color = colorData.buttonColor;

					if(elementHover && firstClick){
						node.listBoxes[i].chosenIndex = listElementI;
						node.listBoxes[i].active = false;
						material.stateChanged = true;
						node.stateChanged = true;

					}

					box(node.width*1.12,iconWidth*2.f,(node.positionX + nodePanel.panelPositionX) * nodePanel.zoomVal + (x)/(20.f/(node.width*40)) ,((node.positionY + nodePanel.panelPositionY) * nodePanel.zoomVal + node.height) - (i+ioIndex+j+1)/(20.f/(node.width*13)) - 0.05f * node.width*10 - iconWidth*1,"",color,0,0,0,depth+0.06f,10000,node.backColor,0);///Bottom
					renderText(programs.uiProgram,node.listBoxes[i].elements[listElementI],(node.positionX + nodePanel.panelPositionX ) * nodePanel.zoomVal - node.width/1.2f + (x)/(20.f/(node.width*40)),((node.positionY + nodePanel.panelPositionY) * nodePanel.zoomVal + node.height) - (i+ioIndex+j+1)/(20.f/(node.width*13)) - 0.05f * node.width*10 - iconWidth*2.,node.width/300.f,colorData.textColor,depth+0.07f,false);
					listElementI++;
				}
			
			}
			box(node.width*1.12,iconWidth*2.f,(node.positionX + nodePanel.panelPositionX) * nodePanel.zoomVal,((node.positionY + nodePanel.panelPositionY) * nodePanel.zoomVal + node.height) - (i+ioIndex)/(20.f/(node.width*16)) - 0.05f * node.width*10 - iconWidth*2,"",colorData.buttonColor,0,0,0,depth+0.03f,10000,node.backColor,0);///Bottom
			if(noButtonHover && !listButtonHover){
				node.listBoxes[i].active = false;
			}
		}

		box(node.width,iconWidth*2.f,(node.positionX + nodePanel.panelPositionX) * nodePanel.zoomVal,((node.positionY + nodePanel.panelPositionY) * nodePanel.zoomVal + node.height) - (i+ioIndex)/(20.f/(node.width*16)) - 0.05f * node.width*10,"",colorData.buttonColor,0,0,0,depth+0.03f,8 / (node.width*6),node.backColor,0);///Bottom
		renderText(programs.uiProgram,node.listBoxes[i].elements[node.listBoxes[i].chosenIndex],(node.positionX + nodePanel.panelPositionX) * nodePanel.zoomVal - node.width/1.4f,((node.positionY + nodePanel.panelPositionY) * nodePanel.zoomVal + node.height) - (i+ioIndex)/(20.f/(node.width*16)) - 0.05f * node.width*10,node.width/300.f,colorData.textColor,depth+0.04f,false);


		glUseProgram(programs.iconsProgram);
		const bool downHover = isMouseOnButton(window , iconWidth/1.3 , iconWidth*1.7 , (node.positionX + nodePanel.panelPositionX) * nodePanel.zoomVal - node.width/1.2f, ((node.positionY + nodePanel.panelPositionY) * nodePanel.zoomVal + node.height) - (i+ioIndex)/(20.f/(node.width*13)) - 0.05f * node.width*10,mouseX,mouseY,false);
		
		if(downHover && firstClick && nodePanel.panelHover)
			node.listBoxes[i].active = !node.listBoxes[i].active;
			
		iconBox(iconWidth/1.3 , iconWidth*1.7 , (node.positionX + nodePanel.panelPositionX) * nodePanel.zoomVal - node.width/1.2f, ((node.positionY + nodePanel.panelPositionY) * nodePanel.zoomVal + node.height) - (i+ioIndex)/(20.f/(node.width*16)) - 0.05f * node.width*10, depth+0.04f , icons.ArrowDown , 0 , colorData.iconColor, colorData.iconColor);
		
		ioIndex++;
	}

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
				if(glfwGetMouseButton(window,0) == GLFW_RELEASE && material.nodes[nodeI].inputs[inputI].connectionHover && node.outputs[i].pressed){
					if(material.nodes[nodeI].inputs[inputI].nodeConnectionIndex != 10000){
						for (size_t conI = 0; conI < material.nodes[material.nodes[nodeI].inputs[inputI].nodeConnectionIndex].outputs[material.nodes[nodeI].inputs[inputI].inputConnectionIndex].connections.size(); conI++)
						{
							if(material.nodes[material.nodes[nodeI].inputs[inputI].nodeConnectionIndex].outputs[material.nodes[nodeI].inputs[inputI].inputConnectionIndex].connections[conI].nodeConnectionIndex == nodeI){
								if(material.nodes[material.nodes[nodeI].inputs[inputI].nodeConnectionIndex].outputs[material.nodes[nodeI].inputs[inputI].inputConnectionIndex].connections[conI].inputConnectionIndex == inputI){
									material.nodes[material.nodes[nodeI].inputs[inputI].nodeConnectionIndex].outputs[material.nodes[nodeI].inputs[inputI].inputConnectionIndex].connections.erase(material.nodes[material.nodes[nodeI].inputs[inputI].nodeConnectionIndex].outputs[material.nodes[nodeI].inputs[inputI].inputConnectionIndex].connections.begin());
								}
							}
						}
					}
					// if(node.outputs[i].nodeConnectionIndex != 10000){
					// 	material.nodes[node.outputs[i].nodeConnectionIndex].inputs[node.outputs[i].inputConnectionIndex].nodeConnectionIndex = 10000;
					// 	material.nodes[node.outputs[i].nodeConnectionIndex].inputs[node.outputs[i].inputConnectionIndex].inputConnectionIndex = 10000;
					// }

					NodeConnection connection;
					connection.connectionPosX = material.nodes[nodeI].inputs[inputI].posX;
					connection.connectionPosY = material.nodes[nodeI].inputs[inputI].posY;
					connection.nodeConnectionIndex = nodeI;
					connection.inputConnectionIndex = inputI;
					
					node.outputs[i].connections.push_back(connection);


					material.nodes[nodeI].inputs[inputI].inputConnectionIndex = i;
					material.nodes[nodeI].inputs[inputI].nodeConnectionIndex = currentNodeIndex;

					
					node.outputs[i].isConnectedToShaderInput = material.nodes[nodeI].isMainOut;
					

					material.stateChanged = true;
					break;

				}	
			}
		}
		
        //Check if output pressed
		if(!anyConnectionPressed){//Prevent multiple selection
			if(nodePanel.panelHover && !coloringPanel.active)
				node.outputs[i].connectionHover = isMouseOnButton(window , iconWidth/1.5f , iconWidth*1.5f  ,node.outputs[i].connectionPosX- screenGap,node.outputs[i].connectionPosY,mouseX,mouseY,false);
			else
				node.outputs[i].connectionHover = false;
		}
		
		//If output pressed (move the connection)
		if(firstClick && node.outputs[i].connectionHover && !node.outputs[i].pressed){
			node.outputs[i].pressed = true;
		}

		//If output released (release the connection)
		if(glfwGetMouseButton(window,0) == GLFW_RELEASE || node.barPressed || anyBarPressed){
			if(true){
				//Render the connection on top of the default connection circle (hide)
				if(node.outputs[i].nodeConnectionIndex == 10000 && node.outputs[i].inputConnectionIndex == 10000){
					node.outputs[i].connectionPosX = (node.positionX + nodePanel.panelPositionX) * nodePanel.zoomVal + node.width +iconWidth*2.f;
					node.outputs[i].connectionPosY = ((node.positionY + nodePanel.panelPositionY) * nodePanel.zoomVal + node.height) - i/(20.f/(node.width*15)) - 0.05f * node.width*10;
				}
				
				// //Render the connection on top of the connection circle (show)
				// if(node.outputs[i].nodeConnectionIndex != 10000 && node.outputs[i].inputConnectionIndex != 10000){
				// 	node.outputs[i].connectionPosX = material.nodes[node.outputs[i].nodeConnectionIndex].inputs[node.outputs[i].inputConnectionIndex].posX; 
				// 	node.outputs[i].connectionPosY = material.nodes[node.outputs[i].nodeConnectionIndex].inputs[node.outputs[i].inputConnectionIndex].posY; 
				// }
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
		for (size_t conI = 0; conI < node.outputs[i].connections.size(); conI++)
		{
			//Updating positions is required
			node.outputs[i].connections[conI].connectionPosX = material.nodes[node.outputs[i].connections[conI].nodeConnectionIndex].inputs[node.outputs[i].connections[conI].inputConnectionIndex].posX;
			node.outputs[i].connections[conI].connectionPosY = material.nodes[node.outputs[i].connections[conI].nodeConnectionIndex].inputs[node.outputs[i].connections[conI].inputConnectionIndex].posY;
			
			node.outputs[i].connections[conI].connectionHover = isMouseOnButton(window , iconWidth/1.5f , iconWidth*1.5f  ,node.outputs[i].connections[conI].connectionPosX- screenGap,node.outputs[i].connections[conI].connectionPosY,mouseX,mouseY,false);

			if(node.outputs[i].connections[conI].connectionHover && firstClick)
				node.outputs[i].connections[conI].connectionPressed = true;

			if(glfwGetMouseButton(window,0) == GLFW_RELEASE)
				node.outputs[i].connections[conI].connectionPressed = false;

        	glUseProgram(programs.curveProgram);
			
			drawLine((node.positionX + nodePanel.panelPositionX) * nodePanel.zoomVal+node.width +iconWidth*2.f,((node.positionY + nodePanel.panelPositionY) * nodePanel.zoomVal + node.height) - i/(20.f/(node.width*15)) - 0.05f * node.width*10,depth+0.02f,node.outputs[i].connections[conI].connectionPosX,node.outputs[i].connections[conI].connectionPosY, node.width*200.f ,nodeColor,false);
		
			if(node.outputs[i].connections[conI].connectionPressed){
				//Severe the connection
				node.outputs[i].connectionHover = true;
				node.outputs[i].pressed = true;
				node.outputs[i].connectionPosX = node.outputs[i].connections[conI].connectionPosX;
				node.outputs[i].connectionPosY = node.outputs[i].connections[conI].connectionPosY;
				
				material.stateChanged = true;

				material.nodes[node.outputs[i].connections[conI].nodeConnectionIndex].inputs[node.outputs[i].connections[conI].inputConnectionIndex].nodeConnectionIndex = 10000;
				material.nodes[node.outputs[i].connections[conI].nodeConnectionIndex].inputs[node.outputs[i].connections[conI].inputConnectionIndex].inputConnectionIndex = 10000;

				node.outputs[i].connections.erase(node.outputs[i].connections.begin() + conI);
			}
		}
		
		//TODO : Move the rendering of connection lines to the output element rendering
		if(node.outputs[i].pressed)//Render the connection lines if output connects to an input or moves
			drawLine((node.positionX + nodePanel.panelPositionX) * nodePanel.zoomVal+node.width +iconWidth*2.f,((node.positionY + nodePanel.panelPositionY) * nodePanel.zoomVal + node.height) - i/(20.f/(node.width*15)) - 0.05f * node.width*10,depth+0.02f,node.outputs[i].connectionPosX,node.outputs[i].connectionPosY, node.width*200.f ,nodeColor,false);
		
        glUseProgram(programs.iconsProgram);
        //Render the output
		//TODO : Use those values for rendering and tracking
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
		if(nodePanel.panelHover && !coloringPanel.active)
			node.inputs[i].connectionHover = isMouseOnButton(window,iconWidth/1.5f , iconWidth*1.5f,(node.positionX + nodePanel.panelPositionX) * nodePanel.zoomVal-node.width - iconWidth*2.f - screenGap,((node.positionY + nodePanel.panelPositionY) * nodePanel.zoomVal + node.height) - (i+ioIndex+rangeBarCountInputs)/(20.f/(node.width*16)) - 0.05f * node.width*10,mouseX,mouseY,false);
		else
			node.inputs[i].connectionHover = false;
		
		node.inputs[i].posX = (node.positionX + nodePanel.panelPositionX) * nodePanel.zoomVal-node.width - iconWidth*2.f;
		node.inputs[i].posY = ((node.positionY + nodePanel.panelPositionY) * nodePanel.zoomVal + node.height) - (i+ioIndex+rangeBarCountInputs)/(20.f/(node.width*16)) - 0.05f * node.width*10;

		iconBox(iconWidth/1.5f , iconWidth*1.5f , (node.positionX + nodePanel.panelPositionX) * nodePanel.zoomVal-node.width - iconWidth*2.f, ((node.positionY + nodePanel.panelPositionY) * nodePanel.zoomVal + node.height) - (i+ioIndex+rangeBarCountInputs)/(20.f/(node.width*16)) - 0.05f * node.width*10, depth+0.01f , icons.Circle , 0 , nodeColor , nodeColor);
		
        //Process the gap necessary related to the input type after rendering
		if(node.inputs[i].element == "range" && node.inputs[i].nodeConnectionIndex == 10000 ){
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
		else if(node.inputs[i].element == "image" && node.inputs[i].nodeConnectionIndex == 10000){
			rangeBarCountInputs += 1;
		}
		else if(node.inputs[i].element == "color" && node.inputs[i].nodeConnectionIndex == 10000){
			rangeBarCountInputs += 1;
		}

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
		
		if(node.inputs[i].element == "ramp"){
			box(iconWidth*3,iconWidth*2.f,(node.positionX + nodePanel.panelPositionX-node.width/4.f) * nodePanel.zoomVal, ((node.positionY + nodePanel.panelPositionY) * nodePanel.zoomVal - node.height/4.) - (i+ioIndex+inputElementIndex)/(20.f/(node.width*16)) - 0.05f * node.width*10,"",glm::vec4(node.inputs[i].rampClr[node.inputs[i].selectedRampIndex]/255.f,1),0,0,0,depth+0.01,8 / (node.width*6),node.backColor,0);///Bottom
			bool clrHover = false;
			bool plusHover = false;
			bool minusHover = false;
			if(nodePanel.panelHover && !coloringPanel.active){
				
				clrHover = isMouseOnButton(window,iconWidth*3,iconWidth*2.f,(node.positionX + nodePanel.panelPositionX-node.width/4.f) * nodePanel.zoomVal, ((node.positionY + nodePanel.panelPositionY) * nodePanel.zoomVal - node.height/4.) - (i+ioIndex+inputElementIndex)/(20.f/(node.width*16)) - 0.05f * node.width*10,mouseX,mouseY,false);
				plusHover = isMouseOnButton(window,iconWidth,iconWidth*2,(node.positionX + nodePanel.panelPositionX) * nodePanel.zoomVal - node.width/8., ((node.positionY + nodePanel.panelPositionY) * nodePanel.zoomVal + node.height/2) - (i+ioIndex+inputElementIndex)/(20.f/(node.width*16)) - 0.05f * node.width*10,mouseX,mouseY,false);
				minusHover = isMouseOnButton(window,iconWidth,iconWidth*2,(node.positionX + nodePanel.panelPositionX) * nodePanel.zoomVal - node.width/8., ((node.positionY + nodePanel.panelPositionY) * nodePanel.zoomVal + node.height/6) - (i+ioIndex+inputElementIndex)/(20.f/(node.width*16)) - 0.05f * node.width*10,mouseX,mouseY,false);
			}
			if(clrHover||plusHover||minusHover)
				nodePanel.pointerCursor = true;
			else
				nodePanel.pointerCursor = false;

			if(clrHover && firstClick && !node.inputs[i].coloringPanelActivated){
				node.inputs[i].coloringPanelActivated = true;
				coloringPanel.active = true;
				
				coloringPanel.hexVal = util.rgbToHexGenerator(node.inputs[i].rampClr[node.inputs[i].selectedRampIndex]);
				coloringPanel.newHexValTextboxEntry = true;
				coloringPanel.result = node.inputs[i].rampClr[node.inputs[i].selectedRampIndex];
				coloringPanel.panelPosX = mouseX/(maxScreenWidth/2.f) - 1.0f + screenGapX ;
				coloringPanel.panelPosY = -mouseY/maxScreenHeight*2.f + 1.0f;
			}

			if(!coloringPanel.active)
				node.inputs[i].coloringPanelActivated = false;

			if(node.inputs[i].coloringPanelActivated){
				node.inputs[i].rampClr[node.inputs[i].selectedRampIndex] = coloringPanel.result;
				material.stateChanged = true;
				node.stateChanged = true;
			}

			if(minusHover && firstClick && node.inputs[i].rampPos.size()){
				node.inputs[i].rampPos.erase(node.inputs[i].rampPos.begin() + node.inputs[i].selectedRampIndex);
				node.inputs[i].rampClr.erase(node.inputs[i].rampClr.begin() + node.inputs[i].selectedRampIndex);
				node.inputs[i].rampPress.erase(node.inputs[i].rampPress.begin() + node.inputs[i].selectedRampIndex);
				node.inputs[i].selectedRampIndex = node.inputs[i].rampPos.size()-1; 
			}
			if(plusHover && firstClick){
				node.inputs[i].rampPos.push_back(0.0);
				node.inputs[i].rampPress.push_back(false);
				node.inputs[i].rampClr.push_back(glm::vec3(0));
				node.inputs[i].selectedRampIndex = node.inputs[i].rampPos.size()-1; 
			}
			
			
			
			glUseProgram(programs.iconsProgram);
			glDepthFunc(GL_LEQUAL);
			bool pressingDone = false;
			for (size_t rampi = 0; rampi < node.inputs[i].rampPos.size(); rampi++)
			{
				if(node.inputs[i].selectedRampIndex == rampi){
					iconBox(iconWidth*1.1,iconWidth*2*1.1,(node.positionX + nodePanel.panelPositionX) * nodePanel.zoomVal + node.width/1.6, ((node.positionY + nodePanel.panelPositionY + (node.inputs[i].rampPos[rampi]-0.5)/1.2) * nodePanel.zoomVal + node.height/4) - (i+ioIndex+inputElementIndex)/(20.f/(node.width*16)) - 0.05f * node.width*10,depth+0.01,icons.Pointer,0,glm::vec4(colorData.LigidPainterThemeColor,1),glm::vec4(0));
				}
				iconBox(iconWidth,iconWidth*2,(node.positionX + nodePanel.panelPositionX) * nodePanel.zoomVal + node.width/1.6, ((node.positionY + nodePanel.panelPositionY + (node.inputs[i].rampPos[rampi]-0.5)/1.2) * nodePanel.zoomVal + node.height/4) - (i+ioIndex+inputElementIndex)/(20.f/(node.width*16)) - 0.05f * node.width*10,depth+0.01,icons.Pointer,0,glm::vec4(node.inputs[i].rampClr[rampi]/glm::vec3(255.),1),glm::vec4(0));
				bool rampPointHover = isMouseOnButton(window,iconWidth,iconWidth*2,(node.positionX + nodePanel.panelPositionX) * nodePanel.zoomVal + node.width/1.6, ((node.positionY + nodePanel.panelPositionY+(node.inputs[i].rampPos[rampi]-0.5)/1.2) * nodePanel.zoomVal + node.height/4) - (i+ioIndex+inputElementIndex)/(20.f/(node.width*16)) - 0.05f * node.width*10,mouseX,mouseY,false);
				if(rampPointHover && firstClick){
					node.inputs[i].selectedRampIndex = rampi;
					node.inputs[i].rampPress[rampi] = true;
				}
				if(node.inputs[i].rampPress[rampi] && !pressingDone){
					pressingDone = true;
					material.stateChanged = true;
					node.stateChanged = true;

					node.inputs[i].rampPos[rampi] -= yOffset/(nodePanel.zoomVal*200);
					node.inputs[i].rampPos[rampi] = util.restrictBetween(node.inputs[i].rampPos[rampi],0.98f,0.02f);
				}
				if(glfwGetMouseButton(window,0) == GLFW_RELEASE){
					node.inputs[i].rampPress[rampi] = false;
				}
			}
			glDepthFunc(GL_LESS);

			glUseProgram(programs.rampProgram);
			for (size_t rampi = 0; rampi < 10; rampi++)
			{
				GlSet gl;
				std::string targetPoint = "point1D[" + std::to_string(rampi) + "]";
				std::string targetColor = "color1D[" + std::to_string(rampi) + "]";
				if(rampi < node.inputs[i].rampPos.size()){
					gl.uniform1f(programs.rampProgram,targetPoint.c_str(),node.inputs[i].rampPos[rampi]);
					gl.uniform3fv(programs.rampProgram,targetColor.c_str(),node.inputs[i].rampClr[rampi]);
				}
				else{
					gl.uniform1f(programs.rampProgram,targetPoint.c_str(),5.0f);
					gl.uniform3fv(programs.rampProgram,targetColor.c_str(),glm::vec3(0));
				}
			}
			
			iconBox(iconWidth,iconWidth*20.f,(node.positionX + nodePanel.panelPositionX) * nodePanel.zoomVal + node.width, ((node.positionY + nodePanel.panelPositionY) * nodePanel.zoomVal + node.height/4) - (i+ioIndex+inputElementIndex)/(20.f/(node.width*16)) - 0.05f * node.width*10,depth+0.01,0,0,glm::vec4(0),glm::vec4(0));

			glUseProgram(programs.iconsProgram);

			iconBox(iconWidth,iconWidth*2,(node.positionX + nodePanel.panelPositionX) * nodePanel.zoomVal - node.width/8., ((node.positionY + nodePanel.panelPositionY) * nodePanel.zoomVal + node.height/2) - (i+ioIndex+inputElementIndex)/(20.f/(node.width*16)) - 0.05f * node.width*10,depth+0.02,icons.Plus,0,colorData.iconColor,glm::vec4(0));
			iconBox(iconWidth,iconWidth*2,(node.positionX + nodePanel.panelPositionX) * nodePanel.zoomVal - node.width/8., ((node.positionY + nodePanel.panelPositionY) * nodePanel.zoomVal + node.height/6) - (i+ioIndex+inputElementIndex)/(20.f/(node.width*16)) - 0.05f * node.width*10,depth+0.02,icons.Minus,0,colorData.iconColor,glm::vec4(0));
		}
		
		if(node.inputs[i].element == "color" && node.inputs[i].nodeConnectionIndex == 10000){
			inputElementIndex++;
			box(iconWidth*6,iconWidth*2.f,(node.positionX + nodePanel.panelPositionX) * nodePanel.zoomVal, ((node.positionY + nodePanel.panelPositionY) * nodePanel.zoomVal + node.height) - (i+ioIndex+inputElementIndex)/(20.f/(node.width*16)) - 0.05f * node.width*10,"",glm::vec4(node.inputs[i].color/255.f,1),0,0,0,depth+0.01,8 / (node.width*6),node.backColor,0);///Bottom
			bool colorInputHover = false; 
			if(nodePanel.panelHover && !coloringPanel.active){
				colorInputHover = isMouseOnButton(window,iconWidth*6,iconWidth*2.f,(node.positionX + nodePanel.panelPositionX) * nodePanel.zoomVal - screenGap, ((node.positionY + nodePanel.panelPositionY) * nodePanel.zoomVal + node.height) - (i+ioIndex+inputElementIndex)/(20.f/(node.width*16)) - 0.05f * node.width*10,mouseX,mouseY,false);
				if(colorInputHover)
					nodePanel.pointerCursor = true;
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
				node.stateChanged = true;
			}
		}

		if(node.inputs[i].element == "image" && node.inputs[i].nodeConnectionIndex == 10000){
			inputElementIndex++;
			
			//Select texture
			if(nodePanel.panelHover && !coloringPanel.active){
				node.inputs[i].addTextureButtonHover = isMouseOnButton(window,iconWidth, iconWidth*2.f,(node.positionX + nodePanel.panelPositionX) * nodePanel.zoomVal - node.width - screenGap,((node.positionY + nodePanel.panelPositionY) * nodePanel.zoomVal + node.height) - (i+ioIndex+inputElementIndex)/(20.f/(node.width*16)) - 0.05f * node.width*10,mouseX,mouseY,false);
				if(node.inputs[i].addTextureButtonHover)
					nodePanel.pointerCursor = true;
			}
			else
				node.inputs[i].addTextureButtonHover = false;
			//Remove texture
			if(nodePanel.panelHover && !coloringPanel.active){
				node.inputs[i].removeTextureButtonHover = isMouseOnButton(window,iconWidth, iconWidth*2.f,(node.positionX + nodePanel.panelPositionX) * nodePanel.zoomVal + node.width - screenGap,((node.positionY + nodePanel.panelPositionY) * nodePanel.zoomVal + node.height) - (i+ioIndex+inputElementIndex)/(20.f/(node.width*16)) - 0.05f * node.width*10,mouseX,mouseY,false);
				if(node.inputs[i].removeTextureButtonHover)
					nodePanel.pointerCursor = true;
			}
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
				node.stateChanged = true;

				node.inputs[i].selectedTextureIndex = 10000;
				node.inputs[i].selectedTextureName = "none";
				node.inputs[i].selectedTexture = 0;
			}



			if(textureSelectionPanel.active && textureSelectionPanel.textureClicked && node.inputs[i].textureSelectingState){
				material.stateChanged = true;
				node.stateChanged = true;

				node.inputs[i].selectedTextureIndex = textureSelectionPanel.selectedIndex;
				node.inputs[i].selectedTextureName = textureSelectionPanel.selectedTextureName;
				node.inputs[i].selectedTexture = albedoTextures[textureSelectionPanel.selectedIndex].id;

				textureSelectionPanel.selectedIndex = 10000;
				textureSelectionPanel.active = false;
				textureSelectionPanel.textureClicked = false;
			}

			//Select a texture
			box(iconWidth/2.f,iconWidth*2.f,(node.positionX + nodePanel.panelPositionX) * nodePanel.zoomVal - node.width, ((node.positionY + nodePanel.panelPositionY) * nodePanel.zoomVal + node.height) - (i+ioIndex+inputElementIndex)/(20.f/(node.width*16)) - 0.05f * node.width*10,"",colorData.rangeBarFront,0,0,0,depth+0.01,8 / (node.width*6),node.backColor,0);
			glUseProgram(programs.iconsProgram);
			iconBox(iconWidth,iconWidth*2,(node.positionX + nodePanel.panelPositionX) * nodePanel.zoomVal - node.width, ((node.positionY + nodePanel.panelPositionY) * nodePanel.zoomVal + node.height) - (i+ioIndex+inputElementIndex)/(20.f/(node.width*16)) - 0.05f * node.width*10,depth+0.02,icons.AddTexture,0,colorData.iconColor,glm::vec4(0));
			
			//Remove texture
			
			glUseProgram(programs.uiProgram);
			box(iconWidth/2.f,iconWidth*2.f,(node.positionX + nodePanel.panelPositionX) * nodePanel.zoomVal + node.width, ((node.positionY + nodePanel.panelPositionY) * nodePanel.zoomVal + node.height) - (i+ioIndex+inputElementIndex)/(20.f/(node.width*16)) - 0.05f * node.width*10,"",colorData.rangeBarBack,0,0,0,depth+0.02,8 / (node.width*6),node.backColor,0);
			glUseProgram(programs.iconsProgram);
			iconBox(iconWidth/2,iconWidth,(node.positionX + nodePanel.panelPositionX) * nodePanel.zoomVal + node.width, ((node.positionY + nodePanel.panelPositionY) * nodePanel.zoomVal + node.height) - (i+ioIndex+inputElementIndex)/(20.f/(node.width*16)) - 0.05f * node.width*10,depth+0.03,icons.X,0,colorData.iconColor,glm::vec4(0));
			glUseProgram(programs.uiProgram);
			
			box(iconWidth*6,iconWidth*2.f,(node.positionX + nodePanel.panelPositionX) * nodePanel.zoomVal, ((node.positionY + nodePanel.panelPositionY) * nodePanel.zoomVal + node.height) - (i+ioIndex+inputElementIndex)/(20.f/(node.width*16)) - 0.05f * node.width*10,"",colorData.buttonColor,0,0,0,depth+0.01,8 / (node.width*6),node.backColor,0);///Bottom
			
			renderText(programs.uiProgram,node.inputs[i].selectedTextureName,(node.positionX + nodePanel.panelPositionX) * nodePanel.zoomVal - node.width/1.5f,((node.positionY + nodePanel.panelPositionY) * nodePanel.zoomVal + node.height) - (i+ioIndex+inputElementIndex)/(20.f/(node.width*16)) - 0.05f * node.width*10 - node.width/10,node.width/400.f,colorData.textColor,depth+0.02,false);
		}

        //Render the range bars
		if(node.inputs[i].element == "range" && node.inputs[i].nodeConnectionIndex == 10000){
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
				box(0,iconWidth*2.f,(node.positionX + nodePanel.panelPositionX) * nodePanel.zoomVal + pointPosVal,((node.positionY + nodePanel.panelPositionY) * nodePanel.zoomVal + node.height) - (i+ioIndex+inputElementIndex)/(20.f/(node.width*16)) - 0.05f * node.width*10,"",colorData.rangeBarFront,0,0,0,depth+0.02f,8 / (node.width*4.5),node.backColor,0);///Top

				//TODO : Calculate the screen gap
        	    //Check if range bar pointer pressed
				if(!anyPointerPressed){
					if(nodePanel.panelHover && !coloringPanel.active)
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
					node.stateChanged = true;

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
	

	glm::vec4 markColor;
	if(node.marked){
		markColor = glm::vec4(colorData.LigidPainterThemeColor,1);
	}
	else{
		markColor = colorData.iconColor;
	}
	glUseProgram(programs.iconsProgram);

	//Mark the node
	if(!node.isMainOut){
		iconBox(iconWidth/1.3 , iconWidth*1.7 , (node.positionX + nodePanel.panelPositionX) * nodePanel.zoomVal + node.width/1.5f, (node.positionY + nodePanel.panelPositionY) * nodePanel.zoomVal + node.height - (iconWidth + node.height*2), depth+0.02f , icons.Mark , 0 , markColor, markColor);
		bool markButtonEnter = false;
		if(nodePanel.panelHover && !coloringPanel.active)
			markButtonEnter = isMouseOnButton(window,iconWidth/1.3 , iconWidth*1.7,(node.positionX + nodePanel.panelPositionX) * nodePanel.zoomVal + node.width/1.5f - screenGap,((node.positionY + nodePanel.panelPositionY) * nodePanel.zoomVal + node.height - (iconWidth + node.height*2)) + iconWidth*1.7f,mouseX,mouseY,false);
		if(markButtonEnter){
			nodePanel.pointerCursor = true;
			if(firstClick){
				node.marked = !node.marked;
			}
		}
	}


	if(!node.isMainOut){
		//Delete the node
		bool deleteButtonEnter = false;
		if(nodePanel.panelHover && !coloringPanel.active)
			deleteButtonEnter = isMouseOnButton(window,iconWidth/1.3 , iconWidth*1.7,(node.positionX + nodePanel.panelPositionX) * nodePanel.zoomVal + node.width - screenGap,((node.positionY + nodePanel.panelPositionY) * nodePanel.zoomVal + node.height - (iconWidth + node.height*2)) + iconWidth*1.7f,mouseX,mouseY,false);
		if(deleteButtonEnter)
			nodePanel.pointerCursor = true;

		if(deleteButtonEnter && firstClick || (glfwGetKey(window,GLFW_KEY_X) == GLFW_PRESS && node.active)){
			for (int dOutI = 0; dOutI < material.nodes[currentNodeIndex].outputs.size(); dOutI++)
			{
				for (size_t coni = 0; coni < material.nodes[currentNodeIndex].outputs[dOutI].connections.size(); coni++)
				{
					if(material.nodes[currentNodeIndex].outputs[dOutI].connections[coni].nodeConnectionIndex != 10000){
						material.nodes[material.nodes[currentNodeIndex].outputs[dOutI].connections[coni].nodeConnectionIndex].inputs[material.nodes[currentNodeIndex].outputs[dOutI].connections[coni].inputConnectionIndex].nodeConnectionIndex = 10000;
						material.nodes[material.nodes[currentNodeIndex].outputs[dOutI].connections[coni].nodeConnectionIndex].inputs[material.nodes[currentNodeIndex].outputs[dOutI].connections[coni].inputConnectionIndex].inputConnectionIndex = 10000;
					}
				}
			}
			for (int dInI = 0; dInI < material.nodes[currentNodeIndex].inputs.size(); dInI++)
			{
				if(material.nodes[currentNodeIndex].inputs[dInI].nodeConnectionIndex != 10000){
					for (size_t coni = 0; coni < material.nodes[material.nodes[currentNodeIndex].inputs[dInI].nodeConnectionIndex].outputs[material.nodes[currentNodeIndex].inputs[dInI].inputConnectionIndex].connections.size(); coni++)
					{
						material.nodes[material.nodes[currentNodeIndex].inputs[dInI].nodeConnectionIndex].outputs[material.nodes[currentNodeIndex].inputs[dInI].inputConnectionIndex].connections[coni].nodeConnectionIndex = 10000;
						material.nodes[material.nodes[currentNodeIndex].inputs[dInI].nodeConnectionIndex].outputs[material.nodes[currentNodeIndex].inputs[dInI].inputConnectionIndex].connections[coni].inputConnectionIndex = 10000;
					}
				}
			}
			material.stateChanged = true;
			node.stateChanged = true;

			//Delete the texture outputs of the node
			for (size_t delTxtri = 0; delTxtri < node.outputs.size(); delTxtri++)
			{
				glDeleteTextures(1,&node.outputs[delTxtri].result);
			}
			

			material.nodes.erase(material.nodes.begin() + currentNodeIndex);

			for (size_t delI = 0; delI < material.nodes.size(); delI++)
			{
				for (size_t delInI = 0; delInI < material.nodes[delI].inputs.size(); delInI++)
				{
					if(currentNodeIndex < material.nodes[delI].inputs[delInI].nodeConnectionIndex && material.nodes[delI].inputs[delInI].nodeConnectionIndex != 10000){
						material.nodes[delI].inputs[delInI].nodeConnectionIndex--;
					}
				}
				for (size_t delOutI = 0; delOutI < material.nodes[delI].outputs.size(); delOutI++)
				{
					if(currentNodeIndex < material.nodes[delI].outputs[delOutI].nodeConnectionIndex && material.nodes[delI].outputs[delOutI].nodeConnectionIndex != 10000){
						material.nodes[delI].outputs[delOutI].nodeConnectionIndex--;
					}
				}
			}
		}
	
		iconBox(iconWidth/1.3 , iconWidth*1.7 , (node.positionX + nodePanel.panelPositionX) * nodePanel.zoomVal + node.width, (node.positionY + nodePanel.panelPositionY) * nodePanel.zoomVal + node.height - (iconWidth + node.height*2), depth+0.02f , icons.CircularX , 0 , colorData.iconColor, colorData.iconColor);
	
		//Duplicate the node
		if(node.active){
			if(duplicateNodeCall){
				Node duplicatedNode;
				duplicatedNode = node;
				duplicatedNode.positionY+=0.1f;
				duplicatedNode.active = false;
				for (size_t douti = 0; douti < duplicatedNode.outputs.size(); douti++)
				{
					for (size_t dconi = 0; dconi < duplicatedNode.outputs[douti].connections.size(); dconi++)
					{
						if(!duplicatedNode.outputs[0].isConnectedToShaderInput){
							duplicatedNode.outputs[douti].connections[dconi].nodeConnectionIndex += (((int)material.nodes.size()) - duplicatedNode.outputs[douti].connections[dconi].nodeConnectionIndex); 
						}
						else{
							duplicatedNode.outputs[douti].connections.erase(duplicatedNode.outputs[douti].connections.begin() + dconi); 
							duplicatedNode.outputs[douti].isConnectedToShaderInput = false; 
						}
					}
					
				}
				for (size_t dinI = 0; dinI < duplicatedNode.inputs.size(); dinI++)
				{
					if(duplicatedNode.inputs[dinI].nodeConnectionIndex != 10000)
						duplicatedNode.inputs[dinI].nodeConnectionIndex += (((int)material.nodes.size()) - duplicatedNode.inputs[dinI].nodeConnectionIndex);
				}
				material.stateChanged = true;
				node.stateChanged = true;

				duplicatedNodes.push_back(duplicatedNode);
			}
		}
	}
}