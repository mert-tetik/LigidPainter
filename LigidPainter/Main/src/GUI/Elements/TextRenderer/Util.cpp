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

/// @brief 
/// @return position gap between the text start pos and ending pos  
float TextRenderer::getTextLastCharOffset(){
    
    float overallX = 0.f;

    if(!textDataText.size())
        return 0.f;

    //Get the location of the text's last char
    for (std::string::const_iterator aC = textDataText.begin() + textDataTextPosCharIndex; aC != textDataText.end(); aC++){
        character ch = font.characters[*aC];//Get the current char

        //To the right
        overallX += (ch.Advance >> 6) * textDataScale;
    }

    return overallX;
}

/// @brief 
/// @param charIndex which char's position will be returned
/// @return position gap between the text start pos and the position of the given char index  
float TextRenderer::getIndexOffset(int charIndex){
    
    float overallX = 0.f;

    int counter = 0;

    if(!textDataText.size())
        return 0.f;

    //Get the location of the text's last char
    for (std::string::const_iterator aC = textDataText.begin(); aC != textDataText.end(); aC++){
        character ch = font.characters[*aC];//Get the current char

        //To the right
        overallX += (ch.Advance >> 6) * textDataScale;

        if(counter == charIndex){
            break;
        }

        counter++;
    }

    return overallX;
}

/// @brief 
/// @param x position of the text
/// @return if the text overflow from it's boundaries (textDataMaxX & textDataMinX)
bool TextRenderer::doesTheTextOverflow(float x){
    
    float overallX = 0.f;

    bool hitTheBoundaires = false;

    if(!textDataText.size())
        return 0.f;

    for (std::string::const_iterator aC = textDataText.begin(); aC != textDataText.end(); aC++){
        character ch = font.characters[*aC];//Get the current char

        //TODO Implement the other side
        if(textDataMaxX < x + overallX) {
            hitTheBoundaires = true;
            break;
        }

        //To the right
        overallX += (ch.Advance >> 6) * textDataScale;
    }

    return hitTheBoundaires;
}

/// @brief 
/// @return the position of the text after position is recalculated for the alignment & stuff 
glm::vec3 TextRenderer::positionTheText(){
    
    glm::vec3 textPos = textDataPos; 

    float overallX = getTextLastCharOffset();
	
	if(!textDataAlignment == 0){ //If text is aligned right to left
		textPos.x -= overallX/2.f; //Allign the text in the middle

		//Check if the text's left side is out of the boundaries
		bool hitTheBoundaires = doesTheTextOverflow(textPos.x);

		//If the text's left side is out of the boundaries align the text on the left side of the button
		if(hitTheBoundaires) 
			textPos.x = textDataMinX;
        
	}
    
    //Get the last chars of the textbox to the right side if overflowed
    if(overallX + textPos.x > textDataMaxX && textDataAlignment == 0)
	    textPos.x -= (overallX + textPos.x) - textDataMaxX;

    return textPos;
}


/// @brief Actual rendering
/// @param shader 
/// @param pos 
void TextRenderer::renderLeftToRight(
                                        Shader shader,
                                        glm::vec3 pos
                                    ){

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
			
            //Go to new line if the text is out of it's boundaries
			if(textDataMaxX < pos.x){ 
				pos.x = startingPoint.x;
				pos.y += (ch.Size.y) * textDataScale * 1.3f;
				
				if(!textDataMultipleLines) //Break the for loop & stop rendering the text if multiple lines are not allowed 
					break;
			}

			//Calculate the position of the char
			float xpos = pos.x + ch.Bearing.x * textDataScale;
			float ypos = pos.y + (ch.Size.y - ch.Bearing.y ) * textDataScale;

			//Calculate the size of the char
			float w = ch.Size.x * textDataScale;
			float h = ch.Size.y * textDataScale;

			//Set the transform values
			shader.setVec2("scale",glm::vec2(w/1.7,h/1.7));
			shader.setVec3("pos",glm::vec3(xpos + w/1.7,ypos - (h/1.7) + this->videoScale.y / 210, textDataPos.z));
 
			//Draw the char
			glBindTexture(GL_TEXTURE_2D,ch.TextureID);
			
			if(textDataMinX < xpos)
				glDrawArrays(GL_TRIANGLES, 0, 6);

			//To the right
			pos.x += (ch.Advance >> 6) * textDataScale;
		}
	}
}

/// @brief Is not used rn
/// @param shader 
/// @param pos 
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



void TextRenderer::renderInsertionPointCursor(Shader shader, int &textPosCharIndex){
    
	float activeCharPos = getIndexOffset(textDataActiveChar) + positionTheText().x;
	float activeChar2Pos = getIndexOffset(textDataActiveChar2) + positionTheText().x;
	float textEndingPos = getTextLastCharOffset() + positionTheText().x;
    
    //Render the insertion point cursor
	if(textDataActive){
		
		//insertion point cursor position
		glm::vec3 ipcPos; 
		
		//Set the transform values
		shader.setFloat("radius",     0    );
		shader.setInt("outlineState" ,     0     ); 

		//Scale of the insertion point cursor
		shader.setVec2("scale", glm::vec2(5 * textDataScale,35 * textDataScale));
		
		//Pos of the point cursor position
		ipcPos = glm::vec3(activeCharPos + 5 * textDataScale,textDataPos.y,textDataPos.z);
		shader.setVec3("pos",ipcPos);

		//Move the text if the insertion point cursor is forcing from the boundaries
		if(ipcPos.x < textDataMinX && key == GLFW_KEY_LEFT-256 && textDataActiveChar > 0)
			textPosCharIndex++;
		if(ipcPos.x > textDataMaxX && key == GLFW_KEY_RIGHT-256 && textDataActiveChar < textDataText.size()-1)
			textPosCharIndex--;
		this->textDataTextPosCharIndex = textPosCharIndex; 

		//Multiselection 
		if(textDataActiveChar != textDataActiveChar2){ 
			
			//Scale & the pos of the multiselection insertion point cursor 
			if(textDataActiveChar == textDataText.size()){
				shader.setVec2("scale",glm::vec2((textEndingPos - activeChar2Pos)/2.f,35 * textDataScale));
				shader.setVec3("pos",glm::vec3(textEndingPos - ((textEndingPos - activeChar2Pos)/2.f) , textDataPos.y , textDataPos.z));
			}
			else{
				shader.setVec2("scale",glm::vec2((activeCharPos - activeChar2Pos)/2.f,35 * textDataScale));
				shader.setVec3("pos",glm::vec3(activeCharPos - ((activeCharPos - activeChar2Pos)/2.f),textDataPos.y,textDataPos.z));
			}

			//Render the multiselection insertion point cursor 
			glDrawArrays(GL_TRIANGLES, 0, 6);
		}
		else{
			//Draw the insertion point cursor
			if(this->timer.seconds % 2 == 0) //Hide and show the insertion point cursor every half second
				glDrawArrays(GL_TRIANGLES, 0, 6);
		}
	}
}