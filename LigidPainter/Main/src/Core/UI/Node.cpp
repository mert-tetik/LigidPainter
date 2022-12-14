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
		bool firstRelease = false;

void UserInterface::node(Node &node,Programs programs,Icons icons,GLFWwindow* window,double mouseX,double mouseY,double xOffset,double yOffset,
float maxScreenWidth,float maxScreenHeight, std::vector<Node> &nodes){
	ColorData colorData;
	Utilities util;

	const float iconWidth = node.width/6.f;

	bool anyBarPressed = false;

	//TODO : Check in one for loop
	for (int nodeI = 0; nodeI < nodes.size(); nodeI++)
	{
		if(nodes[nodeI].barPressed){
			anyBarPressed = true;
			break;
		}
	}
	bool anyInputHover = false;
	for (int nodeI = 0; nodeI < nodes.size(); nodeI++)
	{
		for (size_t i = 0; i < nodes[nodeI].inputs.size(); i++)
		{
			if(nodes[nodeI].inputs[i].connectionHover){
				anyInputHover = true;
				break;
			}
		}
	}
		

    //Move the node if it's top bar is pressed
	node.barHover = isMouseOnButton(window , node.width , iconWidth*2.f ,node.positionX,node.positionY + node.height + iconWidth*2.f,mouseX,mouseY,false);
	if(glfwGetMouseButton(window,0) == GLFW_PRESS && node.barHover && !anyBarPressed){
		node.barPressed = true;
	}
	if(glfwGetMouseButton(window,0) == GLFW_RELEASE){
		node.barPressed = false;
	}
	if(node.barPressed){
		node.positionX += xOffset/maxScreenWidth*2.f;
		node.positionY -= yOffset/maxScreenHeight*2.f;
		
		xOffset = 0;
		yOffset = 0;
	}

    //Render the panel
	glUseProgram(programs.uiProgram);
	box(node.width,node.height,node.positionX,node.positionY,"",node.backColor,0,0,0,0.9999f,10000,node.backColor,0);
	
    //Side area
	box(iconWidth,node.height,node.positionX-node.width -iconWidth,node.positionY,"",node.backColor,0,0,0,0.9999f,10000,node.backColor,0);///Left
	box(iconWidth,node.height,node.positionX+node.width +iconWidth,node.positionY,"",node.backColor,0,0,0,0.9999f,10000,node.backColor,0);///Right
	box(node.width,iconWidth*2.f,node.positionX,node.positionY + node.height + iconWidth*2.f,"",node.upBarColor,0,0,0,0.9999f,10000,node.upBarColor,0);///Top
	box(node.width,iconWidth*2.f,node.positionX,node.positionY - node.height - iconWidth*2.f,"",node.backColor,0,0,0,0.9999f,10000,node.backColor,0);///Bottom


	//Smooth corners
	glUseProgram(programs.iconsProgram);
	iconBox(iconWidth , iconWidth*2.f , node.positionX-node.width -iconWidth, node.positionY + node.height + iconWidth*2.f, 0.9999f , icons.TL , 0 , node.upBarColor , node.backColor);
	iconBox(iconWidth , iconWidth*2.f , node.positionX+node.width +iconWidth, node.positionY + node.height + iconWidth*2.f, 0.9999f , icons.TR , 0 , node.upBarColor , node.backColor);
	iconBox(iconWidth , iconWidth*2.f , node.positionX-node.width -iconWidth, node.positionY - node.height - iconWidth*2.f, 0.9999f , icons.BL , 0 , node.backColor , node.backColor);
	iconBox(iconWidth , iconWidth*2.f , node.positionX+node.width +iconWidth, node.positionY - node.height - iconWidth*2.f, 0.9999f , icons.BR , 0 , node.backColor , node.backColor);
	
	int ioIndex = 0; //Interpret the input with output size since they both rendered seperately
	for (size_t i = 0; i < node.outputs.size(); i++)
	{
        //Color of the output (changes related to input type)
		glm::vec4 nodeColor = glm::vec4(0);

        //Process the input type
		if(node.outputs[i].type == "float"){
			nodeColor = colorData.floatNodeInputColor;
		}
		if(node.outputs[i].type == "vec2"){
			nodeColor = colorData.vec2NodeInputColor;
		}
		if(node.outputs[i].type == "vec3"){
			nodeColor = colorData.vec3NodeInputColor;
		}

		//Check if mouse is hover any of the inputs

		//TODO : Check this algorithm
		int inputIndex;
		for (int nodeI = 0; nodeI < nodes.size(); nodeI++)
		{
			for (int inputI = 0; inputI < nodes[nodeI].inputs.size(); inputI++)
			{
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
		node.outputs[i].connectionHover = isMouseOnButton(window , iconWidth/1.5f , iconWidth*1.5f  ,node.positionX+node.width +iconWidth*2.f,(node.positionY + node.height) - i/(20.f/(node.width*15)) - 0.05f * node.width*10,mouseX,mouseY,false);
		if(glfwGetMouseButton(window,0) == GLFW_PRESS && node.outputs[i].connectionHover && !node.outputs[i].pressed){
			node.outputs[i].pressed = true;
			node.outputs[i].connectionPosX = node.positionX+node.width +iconWidth*2.f;
			node.outputs[i].connectionPosY = (node.positionY + node.height) - i/(20.f/(node.width*15)) - 0.05f * node.width*10;
			firstRelease = true;
		}
		if(glfwGetMouseButton(window,0) == GLFW_RELEASE || node.barPressed || anyBarPressed){
			node.outputs[i].pressed = false;
			if(true){
				if(!anyInputHover && firstRelease){
					node.outputs[i].nodeConnectionIndex = 10000;
					node.outputs[i].inputConnectionIndex = 10000;
				}
				node.outputs[i].connectionPosX = node.positionX+node.width +iconWidth*2.f;
				node.outputs[i].connectionPosY = (node.positionY + node.height) - i/(20.f/(node.width*15)) - 0.05f * node.width*10;
				
				if(node.outputs[i].nodeConnectionIndex != 10000 && node.outputs[i].inputConnectionIndex != 10000){
					node.outputs[i].connectionPosX = nodes[node.outputs[i].nodeConnectionIndex].inputs[node.outputs[i].inputConnectionIndex].posX; 
					node.outputs[i].connectionPosY = nodes[node.outputs[i].nodeConnectionIndex].inputs[node.outputs[i].inputConnectionIndex].posY; 
				}
			}
			firstRelease = false;
		}
		if(node.outputs[i].pressed){
			node.outputs[i].connectionPosX += xOffset/maxScreenWidth*2.f;
			node.outputs[i].connectionPosY -= yOffset/maxScreenHeight*2.f;
			
		}
        glUseProgram(programs.uiProgram);
        //Render the connection line
		drawLine(node.positionX+node.width +iconWidth*2.f,(node.positionY + node.height) - i/(20.f/(node.width*15)) - 0.05f * node.width*10,0.99999f,node.outputs[i].connectionPosX,node.outputs[i].connectionPosY,0,nodeColor);

        glUseProgram(programs.iconsProgram);
        //Render the output
		//TODO : Use those values for rendering and tracking
		node.outputs[i].posX = node.positionX+node.width +iconWidth*2.f;
		node.outputs[i].posY = (node.positionY + node.height) - i/(20.f/(node.width*15)) - 0.05f * node.width*10;
		iconBox(iconWidth/1.5f , iconWidth*1.5f , node.positionX+node.width +iconWidth*2.f, (node.positionY + node.height) - i/(20.f/(node.width*15)) - 0.05f * node.width*10, 0.99999f , icons.Circle , 0 , nodeColor , nodeColor);
		ioIndex++;
	}

	float rangeBarCountInputs = 0;
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

		node.inputs[i].connectionHover = isMouseOnButton(window,iconWidth/1.5f , iconWidth*1.5f,node.positionX-node.width - iconWidth*2.f,(node.positionY + node.height) - (i+ioIndex+rangeBarCountInputs)/(20.f/(node.width*16)) - 0.05f * node.width*10,mouseX,mouseY,false);

		node.inputs[i].posX = node.positionX-node.width - iconWidth*2.f;
		node.inputs[i].posY = (node.positionY + node.height) - (i+ioIndex+rangeBarCountInputs)/(20.f/(node.width*16)) - 0.05f * node.width*10;

		iconBox(iconWidth/1.5f , iconWidth*1.5f , node.positionX-node.width - iconWidth*2.f, (node.positionY + node.height) - (i+ioIndex+rangeBarCountInputs)/(20.f/(node.width*16)) - 0.05f * node.width*10, 0.99999f , icons.Circle , 0 , nodeColor , nodeColor);
		
        //Process the gap necessary related to the input type after rendering
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

	glUseProgram(programs.uiProgram);

    //Render the node title
	renderText(programs.uiProgram,node.title,node.positionX-node.width -iconWidth,node.positionY + node.height + iconWidth*1.f,node.width/300.f);
	
    //Render the output titles
	for (size_t i = 0; i < node.outputs.size(); i++)
	{
		renderText(programs.uiProgram,node.outputs[i].text,node.positionX+node.width - (node.outputs[i].text.size()/60.f)*node.width*8.f,(node.positionY + node.height) - i/(20.f/(node.width*16)) - 0.05f * node.width*10,node.width/300.f);
	}
		

	int inputElementIndex = 0;
	
	bool isRangeBarPointerHover = false;
	
    //Render the input elements
	for (size_t i = 0; i < node.inputs.size(); i++)
	{


        //Determine the range bar count
		float rangeBarCount = 0;
		if(node.inputs[i].type == "float"){
			rangeBarCount = 1;
		}
		if(node.inputs[i].type == "vec2"){
			rangeBarCount = 2;
		}
		if(node.inputs[i].type == "vec3"){
			rangeBarCount = 3;
		}



        //Render the input title
		renderText(programs.uiProgram,node.inputs[i].text,node.positionX-node.width -iconWidth + 0.015f,(node.positionY + node.height) - (i+ioIndex+inputElementIndex)/(20.f/(node.width*16)) - 0.05f * node.width*10,node.width/300.f);
		inputElementIndex++;
		
        //Render the range bars
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
			box(node.width,iconWidth*2.f,node.positionX,(node.positionY + node.height) - (i+ioIndex+inputElementIndex)/(20.f/(node.width*16)) - 0.05f * node.width*10,"",colorData.rangeBarBack,0,0,0,0.99998f,8 / (node.width*6),node.backColor,0);///Bottom
			//Pointer
			box(iconWidth/4.f,iconWidth*2.f,node.positionX + pointPosVal,(node.positionY + node.height) - (i+ioIndex+inputElementIndex)/(20.f/(node.width*16)) - 0.05f * node.width*10,"",colorData.rangeBarFront,0,0,0,0.99999f,8 / (node.width*6),node.backColor,0);///Bottom

			//TODO : Calculate the screen gap
            //Check if range bar pointer pressed
			if(!anyPointerPressed){
				isRangeBarPointerHover = isMouseOnButton(window , iconWidth , iconWidth*2.f ,node.positionX + pointPosVal,(node.positionY + node.height) - (i+ioIndex+inputElementIndex)/(20.f/(node.width*16)) - 0.05f * node.width*10,mouseX,mouseY,false);
			}
			if(glfwGetMouseButton(window,0) == GLFW_PRESS && isRangeBarPointerHover){
				node.inputs[i].rangeBarsPointerPressed[k] = true;
			}
			if(glfwGetMouseButton(window,0) == GLFW_RELEASE){
				node.inputs[i].rangeBarsPointerPressed[k] = false;
			}
			if(node.inputs[i].rangeBarsPointerPressed[k]){
                //Adjust the corresponding values
				if(k == 0){
					node.inputs[i].value.x += xOffset/100.f;
					node.inputs[i].value.x = util.restrictBetween(node.inputs[i].value.x,1.f,0.001f);
				}
				if(k == 1){
					node.inputs[i].value.y += xOffset/100.f;
					node.inputs[i].value.y = util.restrictBetween(node.inputs[i].value.y,1.f,0.001f);
				}
				if(k == 2){
					node.inputs[i].value.z += xOffset/100.f;
					node.inputs[i].value.z = util.restrictBetween(node.inputs[i].value.z,1.f,0.001f);
				}
			}	
			inputElementIndex++;
			isRangeBarPointerHover = false;
		}
	}
}