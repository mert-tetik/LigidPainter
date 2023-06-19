/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

Copyright (c) 2022-2023, LigidTools 

All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------
*/

#include<glad/glad.h>
#include<GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "freetype/freetype.h"

#include <string>
#include <iostream>
#include <vector>
#include <map>

#include "GUI/Elements.hpp"
#include "UTIL/Util.hpp"


void TextRenderer::renderLeftToRight(Shader shader,glm::vec3 pos){

	glm::vec3 startingPoint = pos;

	float overallX = getTextLastCharOffset();
	
	std::string::const_iterator c;
	for (c = textDataText.begin(); c != textDataText.end(); c++)
	{
		character ch = font.characters[*c];//Get the current char
		if(*c == '\n'){//New line if the char is \n
			if(!textDataMultipleLines)//Break the for loop & stop rendering the text if multiple lines are not allowed 
				break;

			pos.x = startingPoint.x; //Go to the start 
			pos.y -= 0.03f; //New line
		}
		else{
			
			if(textDataMaxX < pos.x){ //Go to new line if the text is out of it's boundaries
				pos.x = startingPoint.x;
				pos.y += (ch.Size.y) * textDataScale * 1.3f;
				
				if(!textDataMultipleLines) //Break the for loop & stop rendering the text if multiple lines are not allowed 
					break;
			}

			//Calculate the position of the char
			float xpos = pos.x + ch.Bearing.x * textDataScale;
			float ypos = pos.y + (ch.Size.y - ch.Bearing.y ) * textDataScale;

			//Calculate the size of the char
			float w = ch.Size.x * textDataScale * 0.8f;
			float h = ch.Size.y * textDataScale;

			//Set the transform values
			shader.setVec2("scale",glm::vec2(w/1.7,h/1.7));
			shader.setVec3("pos",glm::vec3(xpos + w/1.7,ypos - h/1.7, textDataPos.z));
 
			//Draw the char
			glBindTexture(GL_TEXTURE_2D,ch.TextureID);
			
			if(textDataMinX < xpos)
				glDrawArrays(GL_TRIANGLES, 0, 6);

			//To the right
			pos.x += (ch.Advance >> 6) * textDataScale / 1.2f;
		}
	}
}

void TextRenderer::renderRightToLeft(Shader shader, glm::vec3 pos){
	//Is not used rn
}

void TextRenderer::rndrTxt(Shader shader, int textPosCharIndex){
	glActiveTexture(GL_TEXTURE0);
	
	shader.setInt("txtr",0);
	shader.setInt("renderText",1);

	glm::vec3 textPos = positionTheText();

	//Render all the chars in the text parameter
	int counter = 0;

	if(textDataAlignment == 2) //If the text has a right to left alignment
		renderRightToLeft(shader,textPos);
	else
		renderLeftToRight(shader,textPos);

	shader.setInt("renderText",0);
}


void TextRenderer::renderText(Shader shader){
	rndrTxt(shader,0);
}


void TextRenderer::renderText(Shader shader,int &textPosCharIndex){
	
	float activeCharPos = getIndexOffset(textDataActiveChar) + positionTheText().x;
	float activeChar2Pos = getIndexOffset(textDataActiveChar2) + positionTheText().x;
	
	float textEndingPos = getTextLastCharOffset() + positionTheText().x;

	//If the active char is not equal to the active char 2 the chars between them will be selected

	//Render the text and get the position of the chars from the text
	rndrTxt(shader,textPosCharIndex);
	
	//Get the position of the chars from the text (w axis will contain the position of the char at the index of activeChar2)
	rndrTxt(shader,textPosCharIndex);
	
	if(textDataActive){
		if(glfwGetTime() - this->timer.lastTimeT > 5.)
			this->timer.lastTimeT = glfwGetTime();

		this->timer.runTimer(0.5f);
	}

	//Render the insertion point cursor
	if(textDataActive){
		glm::vec3 ipcPos; //insertion point cursor position
		
		//Set the transform values
		shader.setFloat("radius",     0    );
		shader.setInt("outlineState" ,     0     ); 

		shader.setVec2("scale",glm::vec2(5 * textDataScale,35 * textDataScale));
			
		ipcPos = glm::vec3(activeCharPos + 5 * textDataScale,textDataPos.y,textDataPos.z);
		shader.setVec3("pos",ipcPos);

		if(ipcPos.x < textDataMinX && key == GLFW_KEY_LEFT-256 && textDataActiveChar > 0)
			textPosCharIndex++;
		if(ipcPos.x > textDataMaxX && key == GLFW_KEY_RIGHT-256 && textDataActiveChar < textDataText.size()-1)
			textPosCharIndex--;

		this->textDataTextPosCharIndex = textPosCharIndex; 

		//TODO Special timer for the text renderer
		if(this->timer.seconds % 2 == 0) //Hide and show the insertion point cursor every second
			glDrawArrays(GL_TRIANGLES, 0, 6);


		//Multiselection 
		if(textDataActiveChar != textDataActiveChar2){ 
			if(textDataActiveChar == textDataText.size()){
				shader.setVec2("scale",glm::vec2((textEndingPos - activeChar2Pos)/2.f,35 * textDataScale));
				shader.setVec3("pos",glm::vec3(textEndingPos - ((textEndingPos - activeChar2Pos)/2.f) , textDataPos.y , textDataPos.z));
			}
			else{
				shader.setVec2("scale",glm::vec2((activeCharPos - activeChar2Pos)/2.f,35 * textDataScale));
				shader.setVec3("pos",glm::vec3(activeCharPos - ((activeCharPos - activeChar2Pos)/2.f),textDataPos.y,textDataPos.z));
			}

			glDrawArrays(GL_TRIANGLES, 0, 6);
		}
	}
}

