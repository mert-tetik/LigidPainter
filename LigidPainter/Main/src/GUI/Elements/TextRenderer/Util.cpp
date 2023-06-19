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
        overallX += (ch.Advance >> 6) * textDataScale / 1.2f;
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
        overallX += (ch.Advance >> 6) * textDataScale / 1.2f;

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
        overallX += (ch.Advance >> 6) * textDataScale / 1.2f;
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