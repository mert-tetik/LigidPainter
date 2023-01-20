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

#include "Core/ProcessTheNodeFile.hpp"


void UserInterface::node(Node &node,Programs programs,Icons icons,GLFWwindow* window,double mouseX,double mouseY,double xOffset,double yOffset,
float maxScreenWidth,float maxScreenHeight, std::vector<Node> &nodes,NodePanel &nodePanel,TextureSelectionPanel &textureSelectionPanel){
	ColorData colorData;
	Utilities util;

	const float iconWidth = node.width/6.f;


	//TODO : Check in one for loop
	//Check if any of the node's bar is pressed
	bool anyBarPressed = false;
	for (int nodeI = 0; nodeI < nodes.size(); nodeI++)
	{
		if(nodes[nodeI].barPressed){
			anyBarPressed = true;
			break;
		}
	}

	//Check if any of the node's input is pressed
	bool anyInputHover = false;
	for (int nodeI = 0; nodeI < nodes.size(); nodeI++)
	{
		for (size_t inIndex = 0; inIndex < nodes[nodeI].inputs.size(); inIndex++)
		{
			if(nodes[nodeI].inputs[inIndex].connectionHover){
				anyInputHover = true;
				break;
			}
		}
	}

	//
	bool anyConnectionPressed = false;
	for (int nodeI = 0; nodeI < nodes.size(); nodeI++)
	{
		for (size_t outIndex = 0; outIndex < nodes[nodeI].outputs.size(); outIndex++)
		{
			if(nodes[nodeI].outputs[outIndex].pressed){
				anyConnectionPressed = true;
				break;
			}
		}
	}

		


    //Move the node if it's top bar is pressed
	if(!anyConnectionPressed){
		node.barHover = isMouseOnButton(window , node.width , iconWidth*2.f ,node.positionX + nodePanel.panelPositionX * nodePanel.zoomVal,node.positionY + nodePanel.panelPositionY * nodePanel.zoomVal + node.height + iconWidth*2.f,mouseX,mouseY,false);
	}
	if(glfwGetMouseButton(window,0) == GLFW_PRESS && node.barHover && !anyBarPressed){
		node.barPressed = true;
	}
	if(glfwGetMouseButton(window,0) == GLFW_RELEASE){
		node.barPressed = false;
	}
	if(node.barPressed && !nodePanel.boundariesPressed){
		node.positionX += xOffset/maxScreenWidth*2.f;
		node.positionY -= yOffset/maxScreenHeight*2.f;
		
		xOffset = 0;
		yOffset = 0;
	}






    //Render the panel
	glUseProgram(programs.uiProgram);
	box(node.width,node.height,node.positionX + nodePanel.panelPositionX * nodePanel.zoomVal,node.positionY + nodePanel.panelPositionY * nodePanel.zoomVal,"",node.backColor,0,0,0,0.999f,10000,node.backColor,0);
    //-Side area
	box(iconWidth,node.height,node.positionX + nodePanel.panelPositionX * nodePanel.zoomVal-node.width -iconWidth,node.positionY + nodePanel.panelPositionY * nodePanel.zoomVal,"",node.backColor,0,0,0,0.999f,10000,node.backColor,0);///Left
	box(iconWidth,node.height,node.positionX + nodePanel.panelPositionX * nodePanel.zoomVal+node.width +iconWidth,node.positionY + nodePanel.panelPositionY * nodePanel.zoomVal,"",node.backColor,0,0,0,0.999f,10000,node.backColor,0);///Right
	box(node.width,iconWidth*2.f,node.positionX + nodePanel.panelPositionX * nodePanel.zoomVal,node.positionY + nodePanel.panelPositionY * nodePanel.zoomVal + node.height + iconWidth*2.f,"",node.upBarColor,0,0,0,0.999f,10000,node.upBarColor,0);///Top
	box(node.width,iconWidth*2.f,node.positionX + nodePanel.panelPositionX * nodePanel.zoomVal,node.positionY + nodePanel.panelPositionY * nodePanel.zoomVal - node.height - iconWidth*2.f,"",node.backColor,0,0,0,0.999f,10000,node.backColor,0);///Bottom
	//-Smooth corners
	glUseProgram(programs.iconsProgram);
	iconBox(iconWidth , iconWidth*2.f , node.positionX + nodePanel.panelPositionX * nodePanel.zoomVal-node.width -iconWidth, node.positionY + nodePanel.panelPositionY * nodePanel.zoomVal + node.height + iconWidth*2.f, 0.999f , icons.TL , 0 , node.upBarColor , node.backColor);
	iconBox(iconWidth , iconWidth*2.f , node.positionX + nodePanel.panelPositionX * nodePanel.zoomVal+node.width +iconWidth, node.positionY + nodePanel.panelPositionY * nodePanel.zoomVal + node.height + iconWidth*2.f, 0.999f , icons.TR , 0 , node.upBarColor , node.backColor);
	iconBox(iconWidth , iconWidth*2.f , node.positionX + nodePanel.panelPositionX * nodePanel.zoomVal-node.width -iconWidth, node.positionY + nodePanel.panelPositionY * nodePanel.zoomVal - node.height - iconWidth*2.f, 0.999f , icons.BL , 0 , node.backColor , node.backColor);
	iconBox(iconWidth , iconWidth*2.f , node.positionX + nodePanel.panelPositionX * nodePanel.zoomVal+node.width +iconWidth, node.positionY + nodePanel.panelPositionY * nodePanel.zoomVal - node.height - iconWidth*2.f, 0.999f , icons.BR , 0 , node.backColor , node.backColor);








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
		for (int nodeI = 0; nodeI < nodes.size(); nodeI++)
		{
			for (int inputI = 0; inputI < nodes[nodeI].inputs.size(); inputI++)
			{
				//Establish connection
				if(nodes[nodeI].inputs[inputI].connectionHover && node.outputs[i].pressed){
					node.outputs[i].inputConnectionIndex = inputI;
					node.outputs[i].nodeConnectionIndex = nodeI;

					node.outputs[i].connectionPosX = nodes[nodeI].inputs[inputI].posX; 
					node.outputs[i].connectionPosY = nodes[nodeI].inputs[inputI].posY; 
					break;
				}			
			}
		}
		
        //Check if output pressed
		if(!anyConnectionPressed){//Prevent multiple selection
			node.outputs[i].connectionHover = isMouseOnButton(window , iconWidth/1.5f , iconWidth*1.5f  ,node.outputs[i].connectionPosX,node.outputs[i].connectionPosY,mouseX,mouseY,false);
		}
		
		//If output pressed (move the connection)
		if(glfwGetMouseButton(window,0) == GLFW_PRESS && node.outputs[i].connectionHover && !node.outputs[i].pressed){
			node.outputs[i].pressed = true;
			node.outputs[i].connectionPosX = node.positionX + nodePanel.panelPositionX * nodePanel.zoomVal + node.width +iconWidth*2.f;
			node.outputs[i].connectionPosY = (node.positionY + nodePanel.panelPositionY * nodePanel.zoomVal + node.height) - i/(20.f/(node.width*15)) - 0.05f * node.width*10;
		}
		
		//Prevent zooming in and out bug while moving the connection
		if((!node.outputs[i].pressed && nodePanel.zoomValChanged)){
				//Render the connection on top of the default connection circle (hide)
				node.outputs[i].connectionPosX = node.positionX + nodePanel.panelPositionX * nodePanel.zoomVal + node.width +iconWidth*2.f;
				node.outputs[i].connectionPosY = (node.positionY + nodePanel.panelPositionY * nodePanel.zoomVal + node.height) - i/(20.f/(node.width*15)) - 0.05f * node.width*10;
				
				//Render the connection on top of the connection circle (show)
				if(node.outputs[i].nodeConnectionIndex != 10000 && node.outputs[i].inputConnectionIndex != 10000){
					node.outputs[i].connectionPosX = nodes[node.outputs[i].nodeConnectionIndex].inputs[node.outputs[i].inputConnectionIndex].posX; 
					node.outputs[i].connectionPosY = nodes[node.outputs[i].nodeConnectionIndex].inputs[node.outputs[i].inputConnectionIndex].posY; 
				}
		}

		//If output released (release the connection)
		if(glfwGetMouseButton(window,0) == GLFW_RELEASE || node.barPressed || anyBarPressed){
			if(true){
				//Severe the connection if connection is not released in a input
				if(!anyInputHover && node.outputs[i].pressed){
					node.outputs[i].nodeConnectionIndex = 10000;
					node.outputs[i].inputConnectionIndex = 10000;
				}
				
				//Render the connection on top of the default connection circle (hide)
				node.outputs[i].connectionPosX = node.positionX + nodePanel.panelPositionX * nodePanel.zoomVal + node.width +iconWidth*2.f;
				node.outputs[i].connectionPosY = (node.positionY + nodePanel.panelPositionY * nodePanel.zoomVal + node.height) - i/(20.f/(node.width*15)) - 0.05f * node.width*10;
				
				//Render the connection on top of the connection circle (show)
				if(node.outputs[i].nodeConnectionIndex != 10000 && node.outputs[i].inputConnectionIndex != 10000){
					node.outputs[i].connectionPosX = nodes[node.outputs[i].nodeConnectionIndex].inputs[node.outputs[i].inputConnectionIndex].posX; 
					node.outputs[i].connectionPosY = nodes[node.outputs[i].nodeConnectionIndex].inputs[node.outputs[i].inputConnectionIndex].posY; 
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
			drawLine(node.positionX + nodePanel.panelPositionX * nodePanel.zoomVal+node.width +iconWidth*2.f,(node.positionY + nodePanel.panelPositionY * nodePanel.zoomVal + node.height) - i/(20.f/(node.width*15)) - 0.05f * node.width*10,0.99999f,node.outputs[i].connectionPosX,node.outputs[i].connectionPosY, node.width*200.f ,nodeColor);
		

        //Render the output
		//TODO : Use those values for rendering and tracking
        glUseProgram(programs.iconsProgram);
		node.outputs[i].posX = node.positionX + nodePanel.panelPositionX * nodePanel.zoomVal+node.width +iconWidth*2.f;
		node.outputs[i].posY = (node.positionY + nodePanel.panelPositionY * nodePanel.zoomVal + node.height) - i/(20.f/(node.width*15)) - 0.05f * node.width*10;
		iconBox(iconWidth/1.5f , iconWidth*1.5f , node.positionX + nodePanel.panelPositionX * nodePanel.zoomVal+node.width +iconWidth*2.f, (node.positionY + nodePanel.panelPositionY * nodePanel.zoomVal + node.height) - i/(20.f/(node.width*15)) - 0.05f * node.width*10, 0.99999f , icons.Circle , 0 , nodeColor , nodeColor);
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

		node.inputs[i].connectionHover = isMouseOnButton(window,iconWidth/1.5f , iconWidth*1.5f,node.positionX + nodePanel.panelPositionX * nodePanel.zoomVal-node.width - iconWidth*2.f,(node.positionY + nodePanel.panelPositionY * nodePanel.zoomVal + node.height) - (i+ioIndex+rangeBarCountInputs)/(20.f/(node.width*16)) - 0.05f * node.width*10,mouseX,mouseY,false);

		node.inputs[i].posX = node.positionX + nodePanel.panelPositionX * nodePanel.zoomVal-node.width - iconWidth*2.f;
		node.inputs[i].posY = (node.positionY + nodePanel.panelPositionY * nodePanel.zoomVal + node.height) - (i+ioIndex+rangeBarCountInputs)/(20.f/(node.width*16)) - 0.05f * node.width*10;

		iconBox(iconWidth/1.5f , iconWidth*1.5f , node.positionX + nodePanel.panelPositionX * nodePanel.zoomVal-node.width - iconWidth*2.f, (node.positionY + nodePanel.panelPositionY * nodePanel.zoomVal + node.height) - (i+ioIndex+rangeBarCountInputs)/(20.f/(node.width*16)) - 0.05f * node.width*10, 0.99999f , icons.Circle , 0 , nodeColor , nodeColor);
		
        //Process the gap necessary related to the input type after rendering
		if(node.inputs[i].element == "range"){
			if(node.inputs[i].type == "float"){
				rangeBarCountInputs += 2;
			}
			if(node.inputs[i].type == "vec2"){
				rangeBarCountInputs += 3;
			}
			if(node.inputs[i].type == "vec3"){
				rangeBarCountInputs += 4;
			}
		}
		else if(node.inputs[i].element == "image"){
			rangeBarCountInputs += 1;
		}

	}






	glUseProgram(programs.uiProgram);

    //Render the node title
	renderText(programs.uiProgram,node.title,node.positionX + nodePanel.panelPositionX * nodePanel.zoomVal-node.width -iconWidth,node.positionY + nodePanel.panelPositionY * nodePanel.zoomVal + node.height + iconWidth*1.f,node.width/300.f,colorData.textColor);
	
    //-----RENDER THE OUTPUT ELEMENTS-----
	for (size_t i = 0; i < node.outputs.size(); i++)
	{
		renderText(programs.uiProgram,node.outputs[i].text,node.positionX + nodePanel.panelPositionX * nodePanel.zoomVal+node.width - (node.outputs[i].text.size()/60.f)*node.width*8.f,(node.positionY + nodePanel.panelPositionY * nodePanel.zoomVal + node.height) - i/(20.f/(node.width*16)) - 0.05f * node.width*10,node.width/300.f,colorData.textColor);
	}
		





	int inputElementIndex = 0;
	
	bool isRangeBarPointerHover = false;
	
    //-----RENDER THE INPUT ELEMENTS-----
	for (size_t i = 0; i < node.inputs.size(); i++)
	{
        //Render the input title
		renderText(programs.uiProgram,node.inputs[i].text,node.positionX + nodePanel.panelPositionX * nodePanel.zoomVal-node.width,(node.positionY + nodePanel.panelPositionY * nodePanel.zoomVal + node.height) - (i+ioIndex+inputElementIndex)/(20.f/(node.width*16)) - 0.05f * node.width*10,node.width/300.f,colorData.textColor);
		
		if(node.inputs[i].element == "image"){
			inputElementIndex++;
			
			//Select texture
			node.inputs[i].addTextureButtonHover = isMouseOnButton(window,iconWidth, iconWidth*2.f,node.positionX + nodePanel.panelPositionX * nodePanel.zoomVal - node.width,(node.positionY + nodePanel.panelPositionY * nodePanel.zoomVal + node.height) - (i+ioIndex+inputElementIndex)/(20.f/(node.width*16)) - 0.05f * node.width*10,mouseX,mouseY,false);
			//Remove texture
			node.inputs[i].removeTextureButtonHover = isMouseOnButton(window,iconWidth, iconWidth*2.f,node.positionX + nodePanel.panelPositionX * nodePanel.zoomVal + node.width,(node.positionY + nodePanel.panelPositionY * nodePanel.zoomVal + node.height) - (i+ioIndex+inputElementIndex)/(20.f/(node.width*16)) - 0.05f * node.width*10,mouseX,mouseY,false);
			
			if(node.inputs[i].addTextureButtonHover && glfwGetMouseButton(window,0) == GLFW_PRESS){
				textureSelectionPanel.active = true;
				textureSelectionPanel.posX = node.inputs[i].posX;
				textureSelectionPanel.posY = node.inputs[i].posY;
			}

			if(node.inputs[i].removeTextureButtonHover && glfwGetMouseButton(window,0) == GLFW_PRESS){
				node.inputs[i].selectedTextureIndex = 10000;
			}



			if(textureSelectionPanel.active && textureSelectionPanel.textureClicked){
				node.inputs[i].selectedTextureIndex = textureSelectionPanel.selectedIndex;
				textureSelectionPanel.selectedIndex = 10000;
				textureSelectionPanel.active = false;
				textureSelectionPanel.textureClicked = false;
			}

			//Select a texture
			box(iconWidth/2.f,iconWidth*2.f,node.positionX + nodePanel.panelPositionX * nodePanel.zoomVal - node.width, (node.positionY + nodePanel.panelPositionY * nodePanel.zoomVal + node.height) - (i+ioIndex+inputElementIndex)/(20.f/(node.width*16)) - 0.05f * node.width*10,"",colorData.rangeBarFront,0,0,0,0.99999f,8 / (node.width*6),node.backColor,0);
			//Remove texture
			box(iconWidth/2.f,iconWidth*2.f,node.positionX + nodePanel.panelPositionX * nodePanel.zoomVal + node.width, (node.positionY + nodePanel.panelPositionY * nodePanel.zoomVal + node.height) - (i+ioIndex+inputElementIndex)/(20.f/(node.width*16)) - 0.05f * node.width*10,"",colorData.rangeBarBack,0,0,0,0.99999f,8 / (node.width*6),node.backColor,0);
			
			box(iconWidth*6,iconWidth*2.f,node.positionX + nodePanel.panelPositionX * nodePanel.zoomVal, (node.positionY + nodePanel.panelPositionY * nodePanel.zoomVal + node.height) - (i+ioIndex+inputElementIndex)/(20.f/(node.width*16)) - 0.05f * node.width*10,"",colorData.buttonColor,0,0,0,0.9999f,8 / (node.width*6),node.backColor,0);///Bottom
			
			renderText(programs.uiProgram,"texture_" + std::to_string(node.inputs[i].selectedTextureIndex),node.positionX + nodePanel.panelPositionX * nodePanel.zoomVal - node.width/1.5f,(node.positionY + nodePanel.panelPositionY * nodePanel.zoomVal + node.height) - (i+ioIndex+inputElementIndex)/(20.f/(node.width*16)) - 0.05f * node.width*10,node.width/300.f,colorData.textColor);
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
				box(node.width,iconWidth*2.f,node.positionX + nodePanel.panelPositionX * nodePanel.zoomVal,(node.positionY + nodePanel.panelPositionY * nodePanel.zoomVal + node.height) - (i+ioIndex+inputElementIndex)/(20.f/(node.width*16)) - 0.05f * node.width*10,"",colorData.rangeBarBack,0,0,0,0.99998f,8 / (node.width*6),node.backColor,0);///Bottom
				//Pointer
				box(iconWidth/4.f,iconWidth*2.f,node.positionX + nodePanel.panelPositionX * nodePanel.zoomVal + pointPosVal,(node.positionY + nodePanel.panelPositionY * nodePanel.zoomVal + node.height) - (i+ioIndex+inputElementIndex)/(20.f/(node.width*16)) - 0.05f * node.width*10,"",colorData.rangeBarFront,0,0,0,0.99999f,8 / (node.width*6),node.backColor,0);///Bottom

				//TODO : Calculate the screen gap
        	    //Check if range bar pointer pressed
				if(!anyPointerPressed){
					isRangeBarPointerHover = isMouseOnButton(window , iconWidth , iconWidth*2.f ,node.positionX + nodePanel.panelPositionX * nodePanel.zoomVal + pointPosVal,(node.positionY + nodePanel.panelPositionY * nodePanel.zoomVal + node.height) - (i+ioIndex+inputElementIndex)/(20.f/(node.width*16)) - 0.05f * node.width*10,mouseX,mouseY,false);
				}
				if(glfwGetMouseButton(window,0) == GLFW_PRESS && isRangeBarPointerHover){
					node.inputs[i].rangeBarsPointerPressed[k] = true;
				}
				if(glfwGetMouseButton(window,0) == GLFW_RELEASE){
					node.inputs[i].rangeBarsPointerPressed[k] = false;
				}
				if(node.inputs[i].rangeBarsPointerPressed[k] && !anyConnectionPressed){
        	        //Adjust the corresponding values
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
				inputElementIndex++;
				isRangeBarPointerHover = false;
			}
		}
	}
}

Node UserInterface::createNode(std::string nodeName,float spawningPosX,float spawningPosY){
	processNode process;
	ProcessHppNode node;
	node = process.processNodeFile(".\\LigidPainter\\Resources\\Nodes\\" + nodeName + ".node");
	
	//std::cout << "title : " << nodee.title << "Output size : " << nodee.outputs.size() << "input size : " << nodee.inputs.size() << "Color : " << nodee.color <<"Code : "<< nodee.code;
	
	Node resultNode;
	resultNode.title = node.title;
	
	resultNode.upBarColor.r = node.color[0];
	resultNode.upBarColor.g = node.color[1]; 
	resultNode.upBarColor.b = node.color[2];
	resultNode.upBarColor.a = node.color[3];

	float rangeBarCount = 0;
	//Load inputs
	for (int i = 0; i < node.inputs.size(); i++)
	{
		NodeInput input;
		
		input.element = node.inputs[i].element;
		input.text = node.inputs[i].title;
		input.type = node.inputs[i].type;
		//node.inputs[i].list;
		//node.inputs[i].listIndex;

		resultNode.inputs.push_back(input);
		if(resultNode.inputs[i].type == "float"){
			if(resultNode.inputs[i].element == "range")
				rangeBarCount += 1.5f;
				
			resultNode.inputs[i].rangeBarsPointerPressed.push_back(false);
		}
		if(resultNode.inputs[i].type == "vec2"){
			if(resultNode.inputs[i].element == "range")
				rangeBarCount += 1.5f*2;
			
			if(resultNode.inputs[i].element == "image")
				rangeBarCount += 1.5f;
			resultNode.inputs[i].rangeBarsPointerPressed.push_back(false);
			resultNode.inputs[i].rangeBarsPointerPressed.push_back(false);
		}
		if(resultNode.inputs[i].type == "vec3"){
			if(resultNode.inputs[i].element == "range")
				rangeBarCount += 1.5f*3;
			if(resultNode.inputs[i].element == "image")
				rangeBarCount += 1.5f;
			resultNode.inputs[i].rangeBarsPointerPressed.push_back(false);
			resultNode.inputs[i].rangeBarsPointerPressed.push_back(false);
			resultNode.inputs[i].rangeBarsPointerPressed.push_back(false);
		}

	}

	resultNode.rangeBarCount = rangeBarCount;
	//Load outputs
	for (int i = 0; i < node.outputs.size(); i++){
		NodeInput output;

		output.text = node.outputs[i].title;
		output.type = node.outputs[i].type;

		resultNode.outputs.push_back(output);
	}
	//node.lists

	resultNode.backColor = glm::vec4(0.2,0.2,0.2,0.5);
	resultNode.positionX = spawningPosX;
	resultNode.positionY = spawningPosY;
	resultNode.title = nodeName;
	resultNode.upBarColor =glm::vec4(0.9,0.2,0.2,1);
	resultNode.width = 0.12f;

	const char* defaultVertexShader = 
		"#version 330 core\n"
		"layout(location = 0) in vec3 aPos;\n"
		"layout(location = 1) in vec3 aNormal;\n"
		"layout(location = 2) in vec2 aTexCoords;\n"
		"uniform mat4 view;\n"
		"uniform mat4 projection;\n"
		"out vec2 tex_coords;\n"
		"out vec3 normal;\n"
		"out vec3 posModel;\n"
		"out vec4 posScene;\n"
		"void main() {\n"
		    "posModel = aPos;\n"
		    "tex_coords = aTexCoords;\n"
		    "normal = aNormal;\n"
		    "posScene = projection * view * vec4(aPos, 0.5);\n" 
		    "gl_Position = posScene;\n"
		"}\0";

	//Compile the fragment shader
	const char* shaderSource = node.code.c_str();
	unsigned int fragShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragShader, 1, &shaderSource, NULL);
	glCompileShader(fragShader);
	
	//Test the shader
	int success;
	char infoLog[512];
	glGetShaderiv(fragShader, GL_COMPILE_STATUS, &success);
	//Print the error if any occurs
	if (!success){glGetShaderInfoLog(fragShader, 512, NULL, infoLog);std::cout << "ERROR::SHADER::COMPILATION_FAILED " << infoLog << std::endl;};

	//Compile the vertex shader
	unsigned int vertShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertShader, 1, &defaultVertexShader, NULL);
	glCompileShader(vertShader);

	//Test the shader
	glGetShaderiv(vertShader, GL_COMPILE_STATUS, &success);
	//Print the error if any occurs
	if (!success){glGetShaderInfoLog(vertShader, 512, NULL, infoLog);std::cout << "ERROR::SHADER::COMPILATION_FAILED " << infoLog << std::endl;};

	unsigned int program = glCreateProgram();
	glAttachShader(program, vertShader);
	glAttachShader(program, fragShader);
	glLinkProgram(program);

	glDeleteShader(vertShader);
	glDeleteShader(fragShader);

	resultNode.program = program;
	
	return resultNode;
}